#include "NocLLM.h"

// Initialize the class with API configurations
NocAI::NocAI(String apiKey, String baseUrl, String model) {
    _apiKey = apiKey;
    _model = model;
    
    // Remove trailing slash if present
    if (baseUrl.endsWith("/")) {
        baseUrl = baseUrl.substring(0, baseUrl.length() - 1);
    }
    _baseUrl = baseUrl;
    _isGeminiNative = (_baseUrl.indexOf("generativelanguage.googleapis.com") != -1) && (_baseUrl.indexOf("/openai") == -1);
    
    _client = nullptr;
    _onMessageCallback = nullptr;
    _onCompleteCallback = nullptr;

    parseUrl(_baseUrl);
}

NocAI::~NocAI() {
    if (_client) {
        _client->stop();
        delete _client;
    }
}

// Manual extraction helper to separate domain, port, and URI path
void NocAI::parseUrl(String url) {
    _isHTTPS = url.startsWith("https://");
    int i = url.indexOf("://");
    if (i != -1) url = url.substring(i + 3);
    
    int pathIdx = url.indexOf("/");
    if (pathIdx != -1) {
        _host = url.substring(0, pathIdx);
        _path = url.substring(pathIdx);
    } else {
        _host = url;
        _path = "";
    }
    
    int portIdx = _host.indexOf(":");
    if (portIdx != -1) {
        _port = _host.substring(portIdx + 1).toInt();
        _host = _host.substring(0, portIdx);
    } else {
        _port = _isHTTPS ? 443 : 80;
    }
}

// Callback registration
void NocAI::onMessage(void (*callback)(String)) {
    _onMessageCallback = callback;
}

void NocAI::onComplete(void (*callback)()) {
    _onCompleteCallback = callback;
}

// Lightweight JSON Scanner
String NocAI::extractContent(const String& jsonChunk) {
    String searchKey = _isGeminiNative ? "\"text\":" : "\"content\":";
    int contentIdx = jsonChunk.indexOf(searchKey);
    if (contentIdx != -1) {
        int startIdx = jsonChunk.indexOf("\"", contentIdx + searchKey.length());
        if (startIdx != -1) {
            String extracted = "";
            bool escaped = false;
            for (unsigned int i = startIdx + 1; i < jsonChunk.length(); i++) {
                char c = jsonChunk[i];
                if (escaped) {
                    if (c == 'n') extracted += '\n';
                    else if (c == 't') extracted += '\t';
                    else if (c == 'r') extracted += '\r';
                    else if (c == '"') extracted += '"';
                    else if (c == '\\') extracted += '\\';
                    else extracted += c;
                    escaped = false;
                } else if (c == '\\') {
                    escaped = true;
                } else if (c == '"') {
                    break;
                } else {
                    extracted += c;
                }
            }
            return extracted;
        }
    }
    return "";
}

// Prepare TCP Request manually for full asynchronous Event-Loop
void NocAI::beginStream(String prompt) {
    if (_client) {
        _client->stop();
        delete _client;
        _client = nullptr;
    }
    
    if (_isHTTPS) {
        WiFiClientSecure* sClient = new WiFiClientSecure();
        sClient->setInsecure(); // Skip SSL Validation for lightweight ESP compatibility
        _client = sClient;
    } else {
        _client = new WiFiClient();
    }

    if (!_client->connect(_host.c_str(), _port)) {
        Serial.println("[NocLLM] TCP Connection Failed to " + _host);
        return;
    }

    // Escape Prompt Characters
    String escapedPrompt = prompt;
    escapedPrompt.replace("\\", "\\\\");
    escapedPrompt.replace("\"", "\\\"");
    escapedPrompt.replace("\n", "\\n");
    escapedPrompt.replace("\r", "");
    escapedPrompt.replace("\t", "\\t");

    String payload;
    String endpoint;

    if (_isGeminiNative) {
        payload = "{\"contents\":[{\"parts\":[{\"text\":\"" + escapedPrompt + "\"}]}]}";
        endpoint = _path + "/v1beta/models/" + _model + ":streamGenerateContent?alt=sse";
    } else {
        payload = "{\"model\":\"" + _model + "\",\"messages\":[{\"role\":\"user\",\"content\":\"" + escapedPrompt + "\"}],\"stream\":true}";
        endpoint = _path + "/chat/completions";
    }

    // Send Manual Headers (Raw HTTP/1.1 chunked style request)
    _client->print("POST " + endpoint + " HTTP/1.1\r\n");
    _client->print("Host: " + _host + "\r\n");
    _client->print("Content-Type: application/json\r\n");
    if (_apiKey.length() > 0) {
        if (_isGeminiNative) {
            _client->print("x-goog-api-key: " + _apiKey + "\r\n");
        } else {
            _client->print("Authorization: Bearer " + _apiKey + "\r\n");
        }
    }
    _client->print("Content-Length: " + String(payload.length()) + "\r\n");
    _client->print("Connection: close\r\n\r\n");
    
    // Send Payload Body
    _client->print(payload);
    
    _buffer = "";
}

// Background Stream Handler to prevent blocking ESP32 main loop
void NocAI::loop() {
    if (!_client) return;

    if (!_client->connected() && !_client->available()) {
        _client->stop();
        delete _client;
        _client = nullptr;
        if (_onCompleteCallback) _onCompleteCallback(); // Trigger done event
        return;
    }
    
    // Fetch TCP response chunks sequentially and lightly
    while (_client->available() > 0) {
        char c = _client->read();
        _buffer += c;
        if (c == '\n') {
            _buffer.trim(); // Clears \r or trailing spaces from SSE server response
            
            // Core Logic: Automatically ignores HTTP headers & chunk length prefixes
            // because valid SSE payload stream always starts perfectly with 'data: '
            if (_buffer.startsWith("data: ")) {
                String jsonChunk = _buffer.substring(6);
                
                if (jsonChunk == "[DONE]") {
                    _client->stop();
                } else {
                    String content = extractContent(jsonChunk);
                    if (content.length() > 0 && _onMessageCallback) {
                        _onMessageCallback(content);
                    }
                }
            }
            _buffer = ""; // Reset buffer for the next incoming line
        }
    }
}
