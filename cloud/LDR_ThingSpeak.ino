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
#include <WiFiClient.h>

const char* ssid = "OnePlus";
const char* password = "12345678";

const char* server = "api.thingspeak.com";
String apiKey = "3W9I7D66JOZ64JD1";

int ldrPin = A0;
int threshold = 540;   // <--- Based on your values

WiFiClient client;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  int ldrValue = analogRead(ldrPin);
  Serial.print("LDR Value: ");
  Serial.println(ldrValue);

  int darkStatus = (ldrValue < threshold) ? 1 : 0;

  if (client.connect(server, 80)) {
    String postStr = apiKey;
    postStr += "&field1=";
    postStr += String(ldrValue);
    postStr += "&field2=";
    postStr += String(darkStatus);
    postStr += "\r\n\r\n";

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);
  }

  client.stop();

  Serial.println("Data sent to ThingSpeak!");
  Serial.println("-------------------------");

  delay(15000);
}
