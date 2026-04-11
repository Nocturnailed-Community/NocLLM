# NocLLM (C++)

*Baca dalam bahasa lain: [English](README.md) | [Indonesian](README_id.md)*

---

NocLLM adalah C++ library yang luar biasa ringan dan asinkron untuk ESP32/ESP8266. Ditujukan untuk berinteraksi dengan AI Large Language Models (LLMs) dari Cloud (OpenAI, Gemini, Groq, Sumopod) maupun Local API server (LM Studio, Ollama).

**Pembuat:** Muhammad Ikhwan Fathulloh / Nocturnailed Community  
**Github:** [https://github.com/Nocturnailed-Community/NocLLM](https://github.com/Nocturnailed-Community/NocLLM)  
**Lisensi:** [MIT](LICENSE)

Prioritas utama library ini adalah efisiensi RAM dan arsitektur **non-blocking**. Alih-alih melakukan *freeze* loop karena `HTTPClient` atau menggunakan `ArduinoJson` yang memakan RAM tinggi, NocLLM memakai `WiFiClient` TCP murni dengan *Event Loop* `ai.loop()`. Cara kerjanya persis seperti WebSocketsClient!

### Fitur Utama
- **Asinkron / Non-Blocking**: Jalankan proses request AI tanpa harus menghentikan operasi sensor atau aktuator di `void loop()` ESP32 mu! AI stream masuk secara elegan di belakang layar (*background*).
- **Adaptasi Dinamis (Cloud & Lokal)**: Otomatis memakai HTTPS `WiFiClientSecure` untuk link SSL dan melakukan optimasi jatuh ke `WiFiClient` HTTP biasa yang super cepat jika terdeteksi IP server lokal.
- **Native Gemini Deteksi AI**: Mendeteksi server Google Gemini langsung dari base URL tanpa jembatan (*bridge*) tambahan, dan otomatis mengganti payload internal `"messages"` menjadi `"contents"` serta mengganti *HTTP headers* nya menjadi `x-goog-api-key`.
- **Zero-Dependency Micro-JSON Parser**: Tidak perlu pusing *memory leak*. Algoritma ini memiliki scanner karakter string manual bawaan yang sangat hemat pemanfaatan RAM tanpa library tambahan!

### Instalasi
1. Pergi ke Github dan unduh NocLLM sebagai `.zip`.
2. Pada Arduino IDE, pergi ke `Sketch` -> `Include Library` -> `Add .ZIP Library...` dan pilih file yang telah diunduh.

### Contoh Tersedia di Library
5 File contoh komprehensif sudah ada di (`File -> Examples -> NocLLM`):
1. **01_Sumopod**: Integrasi endpoint Sumopod untuk model `deepseek-v3` atau `gpt-4o`.
2. **02_OpenAI**: Integrasi klasik HTTPs untuk chatgpt.
3. **03_Gemini_Native**: Menghubungkan endpoint asli Google GenAI secara langsung untuk `gemini-1.5-flash` menggunakan Native JSON formatting API dari Google.
4. **04_Groq**: Konfigurasi khusus untuk Llama 3 menggunakan arsitektur latensi super rendah di Groq Api.
5. **05_Local_LMStudio**: Terhubung mudah di WiFi rumahan untuk Ollama atau LM Studio via Port 1234 tanpa overhead perlambatan SSL. Sangat cepat!

### Sekilas Cara Pakai
```cpp
#include "NocLLM.h"

// Inisialisasi API
NocAI ai("API_KEY", "https://api.openai.com/v1", "gpt-3.5-turbo");

// Callback Ringan ketika text chunk baru masuk
void onStream(String chunk) {
    Serial.print(chunk);
}

void setup() {
    // ... [WiFi connect logic] ...
    
    ai.onMessage(onStream);
    ai.beginStream("Ceritakan fabel kura-kura"); // Perintah langsung tereksekusi instan
}

void loop() {
    // Menangkap respon Event Stream perlahan-lahan di latar belakang
    ai.loop();
    
    // Bebas mengerjakan tugas Sensor DHT atau Servo disini berbarengan!
}
```

## Pembuat & Kredit
- **Dibuat oleh:** Muhammad Ikhwan Fathulloh
- **Komunitas:** Nocturnailed Community
- **GitHub Profil:** [https://github.com/Muhammad-Ikhwan-Fathulloh](https://github.com/Muhammad-Ikhwan-Fathulloh)
- **Repositori Utama:** [https://github.com/Nocturnailed-Community/NocLLM](https://github.com/Nocturnailed-Community/NocLLM)
