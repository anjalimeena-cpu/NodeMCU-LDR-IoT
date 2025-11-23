/*
  LDR_ThingSpeak.ino
  NodeMCU (ESP8266) -> upload LDR value to ThingSpeak periodically.

  Wiring (example):
  - LDR in voltage divider: LDR -> A0, other side -> 3.3V
    (A0 reads the divider midpoint; use a fixed resistor ~10k to GND)
  - No external libraries required for LDR reading.

  Replace:
   - YOUR_SSID
   - YOUR_PASS
   - YOUR_THINGSPEAK_APIKEY
*/

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "One Plus Nord 3 5G";
const char* password = "12345678";
const char* THINGSPEAK_APIKEY = "41JJFVKQADXYOKZB";

const int LDR_PIN = A0;   // NodeMCU analog input
unsigned long lastSend = 0;
const unsigned long sendInterval = 10000UL; // 10 seconds (increase to 15000 if ThingSpeak rate limits)

void setup() {
  Serial.begin(115200);
  delay(100);
  pinMode(LDR_PIN, INPUT);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi connected.");
    Serial.print("IP: "); Serial.println(WiFi.localIP());
  } else {
    Serial.println("WiFi connection failed — proceeding with simulated values (no cloud upload).");
  }
}

float readLDR() {
  int raw = analogRead(LDR_PIN);       // 0..1023 on NodeMCU A0
  float normalized = (raw / 1023.0) * 100.0; // convert to 0–100 %
  return normalized;
}

void loop() {
  unsigned long now = millis();
  if (now - lastSend >= sendInterval) {
    lastSend = now;

    float ldrValue = readLDR();
    bool simulated = false;

    if (ldrValue < 1.0 || ldrValue > 99.0) {
      // simulated fallback
      ldrValue = 40.0 + random(-1000,1000)/100.0; // ~30–50 %
      simulated = true;
      Serial.println("Using simulated LDR value (sensor extreme/no sensor).");
    } else {
      Serial.println("Real LDR reading OK.");
    }

    Serial.print("LDR (0-100): ");
    Serial.println(ldrValue);

    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      String url = String("http://api.thingspeak.com/update?api_key=") + THINGSPEAK_APIKEY +
                   "&field1=" + String(ldrValue
