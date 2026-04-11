#include <WiFi.h>
#include "NocLLM.h"

const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Sumopod Cloud Configuration
const String API_KEY = "sk-YOUR_SUMOPOD_API_KEY";
const String BASE_URL = "https://ai.sumopod.com/v1";
const String MODEL_NAME = "deepseek-v3"; // or gpt-4o-mini

NocAI ai(API_KEY, BASE_URL, MODEL_NAME);

void onMessage(String chunk) {
    Serial.print(chunk);
}

void onDone() {
    Serial.println("\n\n[SUMOPOD STREAM DONE]");
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
    
    // Starting Async Non-Blocking TCP Stream
    Serial.println("Asking Sumopod Cloud...");
    ai.beginStream("Hello Deepseek! Please explain why the sky is blue.");
}

void loop() {
    // Run the highly lightweight Event Loop Processor
    ai.loop();

    // Your ESP32 can breathe and run other tasks simultaneously here!
}
