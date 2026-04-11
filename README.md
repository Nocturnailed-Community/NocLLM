# NocLLM (C++)

*Read this in other languages: [English](README.md) | [Indonesian](README_id.md)*

---

NocLLM is a highly lightweight, asynchronous C++ library designed for ESP32 and ESP8266 to easily interact with Large Language Models (LLMs) from both Cloud (OpenAI, Gemini, Groq, Sumopod) and Local APIs (LM Studio, Ollama). 

**Creator:** Muhammad Ikhwan Fathulloh / Nocturnailed Community  
**Github:** [https://github.com/Nocturnailed-Community/NocLLM](https://github.com/Nocturnailed-Community/NocLLM)  
**License:** [MIT](LICENSE)

The library heavily prioritizes low RAM usage and **non-blocking** architecture. Instead of relying on heavy parsing libraries like `ArduinoJson` or blocking the execution loop with `HTTPClient`, NocLLM uses raw TCP `WiFiClient` with an event-driven `loop()` mechanism, behaving perfectly like WebSockets!

### Features
- **Asynchronous / Non-Blocking (No Polling delays)**: Send raw prompts and let your ESP32 continue running other codes (like reading sensors or blinking LEDs)! The AI stream arrives elegantly in the background.
- **Dynamic Connection (Local or Cloud)**: Automatically utilizes `WiFiClientSecure` for `https://` URLs, and extremely fast standard `WiFiClient` for local `http://` URLs reducing SSL overheads.
- **Native Gemini AI Switcher**: Seamlessly detects Google's `generativelanguage` URL and natively bypasses the structure to fit Gemini's custom header and specific JSON payload!
- **Zero-Dependency Micro-JSON Parser**: Built-in character scanner to selectively parse stream events removing `ArduinoJson` dependencies.

### Installation
1. Go to Github and download NocLLM as `.zip`.
2. In Arduino IDE, go to `Sketch` -> `Include Library` -> `Add .ZIP Library...` and select the downloaded file.

### Examples Included
All 5 comprehensive examples are shipped in the library (`File -> Examples -> NocLLM`):
1. **01_Sumopod**: Using `deepseek-v3` with the Sumopod Cloud.
2. **02_OpenAI**: Classic GPT-4 or GPT-3.5 integration.
3. **03_Gemini_Native**: Uses `gemini-1.5-flash` with Google GenAI infrastructure (NocLLM automatically transitions its interior parsing configuration just from the URL).
4. **04_Groq**: High-speed, ultra-fast latency `llama3-70b` integration via OpenAI bridging routes.
5. **05_Local_LMStudio**: Perfect for Ollama or LMStudio. Utilizes bare TCP Http streams with 0 SSL configuration overhead for blazing fast local network streaming.

### Usage Overview
```cpp
#include "NocLLM.h"

// Initialize configuration
NocAI ai("API_KEY", "https://api.openai.com/v1", "gpt-3.5-turbo");

// Lightweight Callback when text arrives
void onStream(String chunk) {
    Serial.print(chunk);
}

void setup() {
    // ... [WiFi connect logic] ...
    
    ai.onMessage(onStream);
    ai.beginStream("Tell me a short fable"); // Non-blocking trigger!
}

void loop() {
    // Gently pulls the TCP queue in the background
    ai.loop();
    
    // You can safely blink led or read DHT sensors below!
}
```

## Creator & Credits
- **Created by:** Muhammad Ikhwan Fathulloh
- **Community:** Nocturnailed Community
- **GitHub Profile:** [https://github.com/Muhammad-Ikhwan-Fathulloh](https://github.com/Muhammad-Ikhwan-Fathulloh)
- **Repository:** [https://github.com/Nocturnailed-Community/NocLLM](https://github.com/Nocturnailed-Community/NocLLM)
