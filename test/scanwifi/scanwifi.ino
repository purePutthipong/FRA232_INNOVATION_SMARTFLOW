#include <WiFi.h>

void setup() {
  Serial.begin(115200);
  Serial.println("กำลังสแกน WiFi...");
  
  // เริ่มต้น WiFi ในโหมด Station
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();  // ตัดการเชื่อมต่อก่อนสแกนเพื่อให้แน่ใจว่าระบบพร้อม
  delay(100);

  int networksFound = WiFi.scanNetworks();
  Serial.println("การสแกนเสร็จสิ้น");

  if (networksFound == 0) {
    Serial.println("ไม่พบเครือข่าย WiFi");
  } else {
    Serial.printf("พบทั้งหมด %d เครือข่าย:\n", networksFound);
    for (int i = 0; i < networksFound; i++) {
      Serial.printf("%d: %s (RSSI: %d dBm)%s\n",
                    i + 1,
                    WiFi.SSID(i).c_str(),
                    WiFi.RSSI(i),
                    (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " (เปิด)" : " (ล็อก)");
      delay(10);
    }
  }
}

void loop() {
  // ไม่ต้องทำอะไรใน loop
}
