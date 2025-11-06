#include <esp_now.h>
#include <WiFi.h>

// 🟢 กำหนด MAC Address ของเครื่องรับ (เปลี่ยนให้ตรงกับของอีกเครื่อง)
uint8_t receiverAddress[] = { 0x3C, 0x8A, 0x1F, 0xA4, 0x97, 0x0C };

// ตัวแปรนับจำนวนครั้งที่ส่ง
int successCount = 0;
int failCount = 0;

// 🟢 โครงสร้างข้อมูลที่ส่ง
typedef struct {
  float sensorValue;     // ค่าที่จะส่ง
  unsigned long count;   // นับจำนวนครั้งที่ส่ง
} struct_message;

struct_message outgoingData;

// 🟢 callback หลังส่งข้อมูล
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "✅ Success" : "❌ Fail");

  if (status == ESP_NOW_SEND_SUCCESS) successCount++;
  else failCount++;

  Serial.print("Success Count: ");
  Serial.print(successCount);
  Serial.print(" | Fail Count: ");
  Serial.println(failCount);
}

void setup() {
  Serial.begin(115200);
  Serial.println("🚀 ESP-NOW Single Value Sender Start!");

  WiFi.mode(WIFI_STA); // ตั้งเป็นโหมด Station

  // 🟢 เริ่มต้น ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("❌ Error initializing ESP-NOW");
    return;
  }

  // 🟢 ตั้ง callback
  esp_now_register_send_cb(OnDataSent);

  // 🟢 เพิ่ม peer (เครื่องรับ)
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("❌ Failed to add peer");
    return;
  }

  Serial.println("✅ ESP-NOW Ready to Send!");
}

void loop() {
  outgoingData.count++;
  
  // 🟢 สร้างค่าจำลอง (เช่นค่าจากเซนเซอร์)
  outgoingData.sensorValue = random(0, 100);  // เปลี่ยนเป็นค่าจาก analogRead() ได้เลย เช่น analogRead(34)
  
  // 🟢 ส่งข้อมูล
  esp_err_t result = esp_now_send(receiverAddress, (uint8_t *)&outgoingData, sizeof(outgoingData));

  if (result == ESP_OK) {
    Serial.print("📤 Sent: ");
    Serial.print(outgoingData.sensorValue);
    Serial.print(" | Count: ");
    Serial.println(outgoingData.count);
  } else {
    Serial.println("❌ Error sending data");
  }

  delay(2000); // ส่งทุก 2 วินาที
}
