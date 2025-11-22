/*
  LDR_LocalDashboard.ino
  NodeMCU (ESP8266) -> Local Web Dashboard for LDR values

  Shows a live-updating chart + latest LDR value on a webpage.
  Auto-refresh every 5 seconds.
  Includes simulated fallback values if LDR not connected / extreme readings.

  Replace:
   - YOUR_SSID
   - YOUR_PASS
*/

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASS";
ESP8266WebServer server(80);

const int LDR_PIN = A0;
const int MAX_READINGS = 60;
float readings[MAX_READINGS];
int idx = 0;
int countReadings = 0;

unsigned long lastSample = 0;
const unsigned long sampleInterval = 5000UL; // 5 seconds sample interval

// ----------------------------
// Read LDR (normalized 0–100)
// ----------------------------
float readLDR() {
  int raw = analogRead(LDR_PIN);      // 0–1023
  float normalized = (raw / 1023.0) * 100.0;
  return normalized;
}

// ----------------------------
// Record reading in buffer
// ----------------------------
void addReading(float v) {
  readings[idx] = v;
  idx = (idx + 1) % MAX_READINGS;
  if (countReadings < MAX_READINGS) countReadings++;
}

// ----------------------------
// Send JSON array to browser
// ----------------------------
String jsonData() {
  String s = "[";
  for (int i = 0; i < countReadings; ++i) {
    int j = (idx + MAX_READINGS - countReadings + i) % MAX_READINGS;
    s += "{\"val\":";
    s += String(readings[j], 2);
    s += "}";
    if (i < countReadings - 1) s += ",";
  }
  s += "]";
  return s;
}

// ----------------------------
// Main HTML Page
// ----------------------------
void handleRoot() {
  String page = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8">
  <title>NodeMCU LDR Dashboard</title>
  <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
</head>
<body>
  <h3>NodeMCU LDR Dashboard</h3>
  <div>IP Address: <span id="ip">...</span></div>
  <div>LDR Value: <span id="value">--</span> %</div>
  <canvas id="chart" width="400" height="150"></canvas>

<script>
let chart;

function init(data) {
  document.getElementById('ip').innerText = location.hostname || 'ESP8266';

  const labels = data.map((_,i)=>i);
  const vals = data.map(x=>x.val);

  document.getElementById('value').innerText =
      vals.length ? vals[vals.length-1].toFixed(2) : '--';

  const ctx = document.getElementById('chart').getContext('2d');
  chart = new Chart(ctx,{
    type:'line',
    data:{
      labels:labels,
      datasets:[{
        label:'LDR (%)',
        data:vals,
        fill:false,
        tension:0.2
      }]
    },
    options:{animation:false,scales:{x:{display:false}}}
  });

  setInterval(refresh, 5000);
}

function refresh(){
  fetch('/data').then(r=>r.json()).then(d=>{
    const vals = d.map(x=>x.val);
    chart.data.labels = d.map((_,i)=>i);
    chart.data.datasets[0].data = vals;
    chart.update();
    document.getElementById('value').innerText =
        vals.length ? vals[vals.length-1].toFixed(2) : '--';
  });
}

fetch('/data').then(r=>r.json()).then(init);
</script>

</body>
</html>
  )rawliteral";

  server.send(200, "text/html", page);
}

// ----------------------------
// JSON endpoint
// ----------------------------
void handleData() {
  server.send(200, "application/json", jsonData());
}

// ----------------------------
// Setup
// ----------------------------
void setup() {
  Serial.begin(115200);
  delay(10);

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
    Serial.print("Connected, IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("WiFi failed — serving page might require retry.");
  }

  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.begin();

  // initial reading (fake)
  addReading(40.0);
}

// ----------------------------
// Loop
// ----------------------------
void loop() {
  server.handleClient();

  unsigned long now = millis();
  if (now - lastSample >= sampleInterval) {
    lastSample = now;

    float v = readLDR();

    // If sensor extreme/not connected -> simulated reading
    if (v < 1.0 || v > 99.0) {
      v = 40.0 + random(-1000,1000)/100.0;
      Serial.println("Simulated LDR used.");
    } else {
      Serial.println("Real LDR read.");
    }

    addReading(v);

    Serial.print("LDR: ");
    Serial.println(v);
  }
}
