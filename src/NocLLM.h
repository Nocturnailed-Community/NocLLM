#ifndef NOCLLM_H
#define NOCLLM_H

#include <Arduino.h>
#include <WiFiClient.h>

#ifdef ESP32
#include <WiFiClientSecure.h>
#elif defined(ESP8266)
#include <WiFiClientSecure.h>
#endif

class NocAI {
  private:
    String _apiKey;
    String _baseUrl;
    String _model;
    
    // Internal WebSockets-style async variables
    WiFiClient* _client;
    bool _isHTTPS;
    String _host;
    int _port;
    String _path;
    String _buffer;
    bool _isGeminiNative;
    
    void (*_onMessageCallback)(String);
    void (*_onCompleteCallback)();
    
    // Internal helpers
    void parseUrl(String url);
    String extractContent(const String& jsonChunk);

  public:
    // Dynamic Constructor
    NocAI(String apiKey, String baseUrl, String model);
    ~NocAI();

    // Event Registration
    void onMessage(void (*callback)(String));
    void onComplete(void (*callback)());

    // Starts the stream without blocking
    void beginStream(String prompt);
    
    // Event Loop Processor (Must be called in void loop())
    void loop();
};

#endif
