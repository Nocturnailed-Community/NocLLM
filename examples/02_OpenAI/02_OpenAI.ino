#include <WiFi.h>
#include "NocLLM.h"

const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// OpenAI Cloud Configuration
const String API_KEY = "sk-YOUR_OPENAI_API_KEY";
const String BASE_URL = "https://api.openai.com/v1";
const String MODEL_NAME = "gpt-4"; // or gpt-3.5-turbo

NocAI ai(API_KEY, BASE_URL, MODEL_NAME);

// Triggered when new streaming data chunks arrive (Similar to WebSocket)
void onMessage(String chunk) {
    Serial.print(chunk);
}

// Triggered when LLM connection safely closes
void onDone() {
    Serial.println("\n\n[OPENAI STREAM DONE]");
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500); Serial.print(".");
    }
    Serial.println("\nWiFi Connected!");

    // Event Registration
    ai.onMessage(onMessage);
    ai.onComplete(onDone);
    
    Serial.println("Asking OpenAI Cloud...");
    ai.beginStream("Please write a short poem about friendship between a human and a robot.");
}

void loop() {
    // The loop keeps pulling stream events sequentially
    ai.loop();
}
