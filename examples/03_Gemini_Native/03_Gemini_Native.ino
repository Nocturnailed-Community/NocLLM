#include <WiFi.h>
#include "NocLLM.h"

const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Gemini Native API Configuration
const String API_KEY = "YOUR_GEMINI_API_KEY"; 
// NocLLM automatically detects that this URL belongs to Gemini Native API
const String BASE_URL = "https://generativelanguage.googleapis.com"; 
const String MODEL_NAME = "gemini-1.5-flash"; // or gemini-3-flash-preview

NocAI ai(API_KEY, BASE_URL, MODEL_NAME);

void onMessage(String chunk) {
    Serial.print(chunk);
}

void onDone() {
    Serial.println("\n\n[GEMINI STREAM DONE]");
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
    
    Serial.println("Asking Native Gemini API...");
    ai.beginStream("Please explain how Machine Learning works in just 2 sentences.");
}

void loop() {
    // Native parser seamlessly catches data chunk through the event loop
    ai.loop();
}
