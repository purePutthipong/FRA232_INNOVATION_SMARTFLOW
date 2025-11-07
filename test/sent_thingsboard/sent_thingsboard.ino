#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "Pure";
const char* password = "12345678";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected!");

  HTTPClient http;
  http.begin("http://thingsboard.cloud/api/v1/issIsqItHzhsnue1wZs5/telemetry");
  http.addHeader("Content-Type", "application/json");

  String payload = "{\"temperature\":25}";
  int httpResponseCode = http.POST(payload);

  Serial.print("HTTP Response code: ");
  Serial.println(httpResponseCode);

  http.end();
}

void loop() {}
