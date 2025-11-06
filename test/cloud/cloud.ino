#include <WiFi.h>
#include <esp_now.h>
#include <HTTPClient.h>

#define WIFI_SSID     "Pure"
#define WIFI_PASSWORD "12345678"

// กำหนดโหมดการทำงาน
enum Mode { MODE_ESPNOW, MODE_CLOUD };
Mode currentMode = MODE_ESPNOW;

unsigned long lastUpload = 0;
const unsigned long uploadInterval = 10000; // ทุก 10 วิ

// โครงสร้างข้อมูลที่จะส่ง/รับ
typedef struct {
  float sensorValue;
  unsigned long count;
} struct_message;

struct_message myData;

// ========== 📡 CALLBACK รับข้อมูล ==========
void OnDataRecv(const esp_now_recv_info *info, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  
  Serial.print("📡 Received from: ");
  for (int i = 0; i < 6; i++) {
    Serial.printf("%02X", info[i]);
    if (i < 5) Serial.print(":");
  }
  
  Serial.print(" | Value: ");
  Serial.println(myData.sensorValue);
  Serial.print(" | Count: ");
  Serial.println(myData.count);

}

// ========== ⚙️ เริ่มต้น ESP-NOW ==========
void startESPNOW() {
  delay(300);
  WiFi.disconnect(true);
  WiFi.mode(WIFI_AP_STA);   // โหมดผสม (รองรับทั้งส่งและรับ)
  delay(200);
  
  if (esp_now_init() != ESP_OK) {
    Serial.println("❌ Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(OnDataRecv);
  Serial.println("✅ ESP-NOW started");
}

// ========== 🛑 ปิด ESP-NOW ==========
void stopESPNOW() {
  esp_now_deinit();
  Serial.println("🛑 ESP-NOW stopped");
}

// ========== 🌐 เชื่อมต่อ Wi-Fi ==========
void connectWiFi() {
  WiFi.disconnect(true);
  delay(200);
  WiFi.mode(WIFI_STA);
  delay(200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("🌐 Connecting to WiFi");
  unsigned long startAttemptTime = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 20000) {
    Serial.print(".");
    delay(500);
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ Connected to WiFi: " + WiFi.localIP().toString());
  } else {
    Serial.println("\n❌ Failed to connect to WiFi");
  }
}

// ========== 🔌 ตัดการเชื่อมต่อ ==========
void disconnectWiFi() {
  WiFi.disconnect(true);
  delay(300);
  Serial.println("🔌 WiFi disconnected");
}

// ========== ☁️ อัปโหลดข้อมูล ==========
void uploadToCloud() {
  Serial.print("🚀 Uploading data: ");
  Serial.println(myData.sensorValue);

  HTTPClient http;
  http.begin("https://api.thingspeak.com/update?api_key=0XD0OSAA1YHLH6RD&field1=" + String(myData.sensorValue));
  http.GET();
  http.end();
}

// ========== ⚙️ SETUP ==========
void setup() {
  Serial.begin(115200);
  startESPNOW();
}

// ========== 🔁 LOOP ==========
void loop() {
  unsigned long now = millis();

  if (currentMode == MODE_ESPNOW) {
    // โหมดรับข้อมูล ESP-NOW
    if (now - lastUpload > uploadInterval) {
      stopESPNOW();
      currentMode = MODE_CLOUD;
    }
  } 
  else if (currentMode == MODE_CLOUD) {
    connectWiFi();
    uploadToCloud();
    disconnectWiFi();
    startESPNOW();
    lastUpload = now;
    currentMode = MODE_ESPNOW;
  }
}
