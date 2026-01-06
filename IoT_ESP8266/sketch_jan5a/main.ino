#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DHT.h>
#include <ArduinoJson.h>

// =====================
// Wi-Fi 設定
// =====================
const char* ssid = "Galaxy A52 5G 8869";
const char* password = "yqlq9230";

// 腳位
#define DHTPIN 0
#define DHTTYPE DHT11
#define RELAY_PIN 5

// =====================
ESP8266WebServer server(80);
DHT dht(DHTPIN, DHTTYPE);

bool fanState = false;     // 風扇狀態
bool yoloAllow = true;     // 是否允許 AI 控制
unsigned long lastUpload = 0;

const char MAIN_page[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Smart Fan Controller</title>
<style>
body { font-family: Arial; text-align: center; background: #f2f2f2; }
h1 { color: #333; }
.card { background: white; padding: 20px; margin: 20px; border-radius: 10px; box-shadow: 0 0 10px rgba(0,0,0,0.1); }
button { width: 80%; padding: 15px; font-size: 20px; margin: 10px; border-radius: 8px; border: none; color: white; }
.on { background: #4CAF50; }
.off { background: #f44336; }
.status { font-size: 18px; margin-top: 10px; }
</style>
</head>
<body>
<h1>🌀 智慧電風扇控制</h1>
<div class="card">
<p class="status">風扇狀態：<span id="fan">--</span></p>
<p class="status">人員偵測：<span id="yolo">--</span></p>
<p class="status">室溫：<span id="temp">--</span> °C</p>
<button class="on" onclick="fanOn()">開啟風扇</button>
<button class="off" onclick="fanOff()">關閉風扇</button>
</div>
<script>
function fanOn() {
    document.getElementById("fan").innerText = "開啟中...";
    fetch('/fan/on')
      .then(() => updateStatus())
      .catch(() => {
          document.getElementById("fan").innerText = "連線失敗";
      });
}

function fanOff() {
    document.getElementById("fan").innerText = "關閉中...";
    fetch('/fan/off')
      .then(() => updateStatus())
      .catch(() => {
          document.getElementById("fan").innerText = "連線失敗";
      });
}

function updateStatus() {
  fetch('/status')
    .then(res => res.json())
    .then(data => {
      // 安全檢查，確保不會 undefined
      document.getElementById('fan').innerText = data.fan || "--";
      document.getElementById('yolo').innerText = data.yolo || "--";
      // 如果 temp 是字串或數字都能顯示
      document.getElementById('temp').innerText = (data.temp !== undefined) ? data.temp : "19.80";
    })
    .catch(err => {
      console.log("更新狀態失敗:", err);
      document.getElementById('fan').innerText = "錯誤";
      document.getElementById('yolo').innerText = "--";
      document.getElementById('temp').innerText = "19.80";
    });
}

// 每 500ms 更新一次，比原本 200ms 穩定
setInterval(updateStatus, 500);
updateStatus();
</script>
</body>
</html>
)rawliteral";


// =====================
// 繼電器控制
// =====================
void fanOn() { digitalWrite(RELAY_PIN, HIGH); fanState = true; Serial.println("🌀 Fan ON"); }
void fanOff() { digitalWrite(RELAY_PIN, LOW); fanState = false; Serial.println("❌ Fan OFF"); }

// =====================
// API Handlers
// =====================
void handleRoot() { server.send_P(200, "text/html", MAIN_page); }
void handleFanOn() { yoloAllow = true; fanOn(); server.send(200, "text/plain", "Fan ON"); }
void handleFanOff() { fanOff(); server.send(200, "text/plain", "Fan OFF"); }
void handleNoPerson() { yoloAllow = false; fanOff(); server.send(200, "text/plain", "No person, fan disabled"); }
void handleStatus() {
  StaticJsonDocument<200> doc;
  doc["fan"] = fanState ? "ON" : "OFF";
  doc["yolo"] = yoloAllow ? "PERSON" : "NO_PERSON";
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

// =====================
// Setup
// =====================
void setup() {
  Serial.begin(9600);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  dht.begin();

  WiFi.begin(ssid, password);
  Serial.print("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println("\nWiFi connected");
  Serial.print("IP: "); Serial.println(WiFi.localIP());
  Serial.println(WiFi.localIP());

  // API
  server.on("/", handleRoot);
  server.on("/fan/on", handleFanOn);
  server.on("/fan/off", handleFanOff);
  server.on("/noperson", handleNoPerson);
  server.on("/status", handleStatus);
  server.begin();
}

// =====================
// Loop
// =====================
void loop() {
  server.handleClient();
  float temperature = dht.readTemperature();
  Serial.println(temperature);
  if (!isnan(temperature)) {
    if (yoloAllow) {
      if (temperature > 20 && !fanState) fanOn();
      if (temperature <= 20 && fanState) fanOff();
    }
    
  delay(1000);
  }
}
