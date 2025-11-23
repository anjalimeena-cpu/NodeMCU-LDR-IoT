# 🌟 IoT Light Monitoring System using ESP8266 (NodeMCU) + LDR + ThingSpeak  
A complete IoT project for the **IIT Bombay FOSSEE Internship**, demonstrating real-time light sensing using an **LDR sensor**, and cloud data upload to **ThingSpeak** every 15 seconds for live graph visualization.

---

## 📌 Project Overview  
This project uses a **NodeMCU ESP8266** and an **LDR** to measure ambient light intensity.  
The sensor values are uploaded to **ThingSpeak Cloud** every 10 seconds using an HTTP GET request.  
ThingSpeak automatically plots the values on a **live updating line graph**, making this a simple and complete IoT monitoring system.

---

## 🧩 Features  
- ✔ Real-time light intensity monitoring  
- ✔ Uploads data every 10 seconds  
- ✔ Automatic graphing on ThingSpeak (no extra code required)  
- ✔ ESP8266 WiFi connectivity  
- ✔ Simple, low-cost hardware setup  
- ✔ Fully documented for internship submission  

---

## 🛠 Components Used  

| Component | Quantity | Description |
|----------|----------|-------------|
| **NodeMCU ESP8266** | 1 | WiFi-enabled microcontroller |
| **LDR Sensor** | 1 | Light intensity sensor |
| **10kΩ Resistor** | 1 | Voltage divider resistor |
| **Breadboard + Wires** | — | Circuit building |
| **USB Cable** | 1 | Power + programming |

---

## 🔌 Circuit Connections  

### **LDR + Resistor Voltage Divider**
| Component | NodeMCU Pin |
|----------|-------------|
| LDR → 3.3V | 3.3V |
| LDR → A0 (analog output) | A0 |
| 10kΩ Resistor → Same A0 node | A0 |
| Resistor other end → GND | GND |

💡 **Why voltage divider?**  
Because NodeMCU cannot read resistance directly —  
it can only read voltage (0–3.3V).  
The LDR + 10kΩ resistor convert light intensity → voltage → A0 reading (0–1023).

---

## 🧠 How the System Works (Explanation)

### 🔹 **LDR Behavior**
- More light → **lower resistance**  
- Less light → **higher resistance**

This changes the voltage at A0:
Bright → higher voltage → higher A0 reading
Dark → lower voltage → lower A0 reading


### 🔹 **Why A0 values range from 0–1023?**  
NodeMCU has a **10-bit ADC** →  
0 = 0V, 1023 = 3.3V.

### 🔹 **Why ThingSpeak is used?**  
ThingSpeak acts as a:
- Data receiver  
- Cloud database  
- Auto-graph generator  
- Visualization dashboard  

No custom web server required.

---

## ☁️ ThingSpeak Setup Steps  
1. Create a Thingspeak account  
2. Create a new Channel  
3. Enable **Field 1 → Light Level**  
4. Copy your **Write API Key**  
5. Replace it in the code  

ThingSpeak automatically graphs the LDR readings.

---

## 💻 Final Code (Fully Working, Ready to Upload)

```cpp
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

const char* ssid = "Bhavya";
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

📸 Hardware Setup Images
![Circuit Setup](images/circuit.jpg)
![LDR Placement](images/ldr.jpg)
![NodeMCU Closeup](images/nodemcu.jpg)

📊 ThingSpeak Graph Screenshot
![ThingSpeak Live Graph](images/thingspeak_graph.jpg)

🎥 Demo Video
[Click Here to watch the Vedio]()
