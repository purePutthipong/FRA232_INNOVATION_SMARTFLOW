#include <esp_now.h>
#include <WiFi.h>

// กำหนด MAC Address ของผู้รับ (เปลี่ยนตาม MAC ของอีกเครื่อง)
uint8_t receiverAddress[] = { 0x3C, 0x8A, 0x1F, 0xA4, 0x97, 0x0C };
int successCount = 0;
int failCount = 0;

// โครงสร้างข้อมูลที่จะส่ง
typedef struct {
  float temp;
  float hum;
  float pres;
  unsigned long count;  // จำนวนครั้งที่ส่ง
} struct_message;

struct_message outgoingData;

// callback เมื่อส่งข้อมูลเสร็จ
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print(" \t| Send Status: ");
  Serial.print(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");

  if (status == ESP_NOW_SEND_SUCCESS) successCount++;
  else failCount++;

  Serial.print("\t| Success: ");
  Serial.print(successCount);
  Serial.print("\t| Fail: ");
  Serial.println(failCount);
}

void setup() {
  Serial.begin(115200);
  Serial.println("-------------------------------------------------- START --------------------------------------------------");
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent);

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  Serial.println("ESP-NOW Sender Ready!");
}

void loop() {
  outgoingData.count++;

  // ข้อมูลจำลองที่จะส่ง
  outgoingData.temp = random(20, 30);
  outgoingData.hum = random(40, 70);
  outgoingData.pres = random(990, 1020);

  esp_err_t result = esp_now_send(receiverAddress, (uint8_t *)&outgoingData, sizeof(outgoingData));

  Serial.print(result == ESP_OK ? "Sent struct successfully!" : "Error sending struct...");
  Serial.print("\t| Sent count: ");
  Serial.print(outgoingData.count);

  delay(2000);  // ส่งทุก 5 วินาที
}
