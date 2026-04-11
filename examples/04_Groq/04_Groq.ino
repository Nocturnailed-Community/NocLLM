#include <WiFi.h>
#include "NocLLM.h"

const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Groq Fast Inference Configuration
const String API_KEY = "gsk_YOUR_GROQ_API_KEY";
const String BASE_URL = "https://api.groq.com/openai/v1";
const String MODEL_NAME = "llama3-70b-8192";

NocAI ai(API_KEY, BASE_URL, MODEL_NAME);

void onMessage(String chunk) {
    Serial.print(chunk);
}

void onDone() {
    Serial.println("\n\n[GROQ STREAM DONE]");
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
    
    Serial.println("Asking Groq Cloud...");
    ai.beginStream("Solve 3482 * 193 step by step.");
}

void loop() {
    ai.loop();
}
