#include <WiFi.h>
#include <esp_now.h>
#include <HTTPClient.h>

// ===== ⚙️ ตั้งค่า Wi-Fi =====
#define WIFI_SSID     "Pure"
#define WIFI_PASSWORD "12345678"

// ===== 🌐 ตั้งค่า ThingsBoard Cloud =====
#define THINGSBOARD_TOKEN  "issIsqItHzhsnue1wZs5"  // <-- ใส่ token ของเพียว
#define THINGSBOARD_SERVER "http://thingsboard.cloud/api/v1/" THINGSBOARD_TOKEN "/telemetry"

enum Mode { MODE_ESPNOW, MODE_CLOUD };
Mode currentMode = MODE_ESPNOW;

unsigned long lastUpload = 0;
const unsigned long uploadInterval = 10000; // ทุก 10 วินาที

typedef struct {
  float sensorValue;
  unsigned long count;
} struct_message;

struct_message myData;

// 📡 รับข้อมูลจาก ESP-NOW
void OnDataRecv(const esp_now_recv_info *info, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.printf("📡 From %02X:%02X:%02X:%02X:%02X:%02X | Value: %.2f | Count: %lu\n",
                info->src_addr[0], info->src_addr[1], info->src_addr[2],
                info->src_addr[3], info->src_addr[4], info->src_addr[5],
                myData.sensorValue, myData.count);
}

// ⚙️ เริ่ม ESP-NOW
void startESPNOW() {
  WiFi.disconnect(true);
  WiFi.mode(WIFI_AP_STA);
  delay(300);

  if (esp_now_init() != ESP_OK) {
    Serial.println("❌ ESP-NOW init failed");
    return;
  }
  esp_now_register_recv_cb(OnDataRecv);
  Serial.println("✅ ESP-NOW started");
}

// 🌐 เชื่อมต่อ Wi-Fi
void connectWiFi() {
  WiFi.disconnect(true);
  delay(200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("🌐 Connecting to WiFi");
  unsigned long startAttemptTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 20000) {
    Serial.print(".");
    delay(500);
  }

  if (WiFi.status() == WL_CONNECTED)
    Serial.println("\n✅ Connected: " + WiFi.localIP().toString());
  else
    Serial.println("\n❌ WiFi failed");
}

// ☁️ ส่งข้อมูลขึ้น ThingsBoard Cloud
void uploadToThingsBoard() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("❌ WiFi not connected, skip upload");
    return;
  }

  HTTPClient http;
  http.begin(THINGSBOARD_SERVER);
  http.addHeader("Content-Type", "application/json");

  // JSON payload
  String jsonData = "{\"sensorValue\": " + String(myData.sensorValue, 2) +
                    ", \"count\": " + String(myData.count) + "}";

  Serial.println("🚀 Uploading to ThingsBoard: " + jsonData);

  int httpCode = http.POST(jsonData);
  if (httpCode > 0) {
    Serial.printf("✅ Uploaded, Response: %d\n", httpCode);
  } else {
    Serial.printf("❌ Upload failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
}

void setup() {
  Serial.begin(115200);
  startESPNOW();
}

void loop() {
  unsigned long now = millis();

  if (currentMode == MODE_ESPNOW && now - lastUpload > uploadInterval) {
    currentMode = MODE_CLOUD;
  }

  if (currentMode == MODE_CLOUD) {
    connectWiFi();
    uploadToThingsBoard();
    startESPNOW();
    lastUpload = now;
    currentMode = MODE_ESPNOW;
  }
}
