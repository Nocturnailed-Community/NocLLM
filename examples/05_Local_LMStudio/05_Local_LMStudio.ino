#include <WiFi.h>
#include "NocLLM.h"

const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Local TCP Server Configuration (No SSL overhead) (Ollama / LM Studio)
const String API_KEY = "no-key-required"; 
const String BASE_URL = "http://192.168.1.100:1234/v1"; // Change 192.168.1.100 to your computer's local IP
const String MODEL_NAME = "local-model"; // ID Model is usually ignored in local LM Studio endpoints

NocAI ai(API_KEY, BASE_URL, MODEL_NAME);

// Triggered dynamically when local AI stream chunks arrive
void onMessage(String chunk) {
    Serial.print(chunk);
}

// Triggered when local AI finishes generating
void onDone() {
    Serial.println("\n\n[LOCAL STREAM DONE]");
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
    
    Serial.println("Asking Local Network LLM (LM Studio / Ollama)...");
    ai.beginStream("What is the default size of a float in C++?");
}

void loop() {
    // Because generating using local PC takes time, ai.loop() fetches 
    // chunks passively in the background without freezing the ESP main loop!
    ai.loop();
}
