/*
* Código gerado pelo Chat GPT e refinado conforme as necessidades do projeto em 31/12/2025 
https://github.com/marcelovictorio @MarceloVictorio YouTube
Sistema completo de medição de RPM com ESP32, exibindo dados em um display OLED animado e 
em um dashboard web automotivo em tempo real, acessível pelo navegador do celular ou PC.

Sensor IR → ESP32 → RPM
              ├─ OLED (VU Meter + RPM)
              └─ Servidor Web
                    ├─ RPM
                    ├─ Barra VU animada
                    ├─ RPM Máx
                    ├─ Status
                    └─ Atualização em tempo real

*/

#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/* ================= CONFIGURAÇÕES ================= */
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C
#define OLED_RESET -1

#define SENSOR_PIN 27
#define PULSES_PER_REV 1
#define RPM_MAX 6000

const char* ssid = "ESP32-RPM";
const char* password = "12345678";
const char* mdnsName = "esp32-rpm";
/* ================================================= */

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
WebServer server(80);

volatile unsigned long pulseCount = 0;
unsigned long lastCalc = 0;

float rpm = 0;
float rpmSmooth = 0;
float rpmMax = 0;

/* ================= INTERRUPÇÃO ================= */
void IRAM_ATTR countPulse() {
  pulseCount++;
}

/* ================= OLED – VU METER ================= */
void drawVUMeter(int value) {
  int bars = map(value, 0, RPM_MAX, 0, 16);
  bars = constrain(bars, 0, 16);

  int barWidth = 6;
  int spacing = 2;
  int baseY = 63;

  for (int i = 0; i < bars; i++) {
    int h = map(i, 0, 15, 6, 48);
    int x = i * (barWidth + spacing);
    display.fillRect(x, baseY - h, barWidth, h, SSD1306_WHITE);
  }
}

/* ================= OLED – IP / DNS ================= */
void showIPonOLED() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0,0);
  display.println("ESP32 RPM");

  display.setCursor(0,14);
  display.print("IP:");
  display.println(WiFi.softAPIP());

  display.setCursor(0,28);
  display.print("DNS:");
  display.println("esp32-rpm.local");

  display.display();
}

/* ================= DASHBOARD AUTOMOTIVO ================= */
String dashboardHTML() {
  return R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="utf-8">
<title>ESP32 RPM</title>
<style>
body {
  background:#0b0b0b;
  color:#f5f5f5;
  font-family:Arial, Helvetica, sans-serif;
  text-align:center;
}
canvas {
  margin-top:20px;
}
.rpm {
  font-size:48px;
  color:#00ff99;
}
.redline {
  color:#ff3333;
}
</style>
</head>

<body>
<h2>CLUSTER DIGITAL</h2>
<canvas id="tacho" width="320" height="220"></canvas>
<div id="rpm" class="rpm">0 RPM</div>

<script>
const canvas = document.getElementById("tacho");
const ctx = canvas.getContext("2d");
const RPM_MAX = 6000;

function drawGauge(rpm){
  ctx.clearRect(0,0,320,220);

  ctx.lineWidth = 18;
  ctx.strokeStyle = "#333";
  ctx.beginPath();
  ctx.arc(160,200,130,Math.PI,0);
  ctx.stroke();

  ctx.strokeStyle = "#ff3333";
  ctx.beginPath();
  ctx.arc(160,200,130,Math.PI*1.8,Math.PI*2);
  ctx.stroke();

  let angle = Math.PI + (rpm / RPM_MAX) * Math.PI;
  ctx.strokeStyle = rpm > RPM_MAX*0.8 ? "#ff3333" : "#00ff99";
  ctx.lineWidth = 4;
  ctx.beginPath();
  ctx.moveTo(160,200);
  ctx.lineTo(
    160 + 110 * Math.cos(angle),
    200 + 110 * Math.sin(angle)
  );
  ctx.stroke();
}

setInterval(()=>{
  fetch("/api")
    .then(r=>r.json())
    .then(d=>{
      drawGauge(d.rpm);
      const el = document.getElementById("rpm");
      el.innerText = d.rpm + " RPM";
      el.className = d.rpm > RPM_MAX*0.8 ? "rpm redline" : "rpm";
    });
}, 200);
</script>
</body>
</html>
)rawliteral";
}

/* ================= API ================= */
void handleAPI() {
  String json = "{";
  json += "\"rpm\":" + String((int)rpmSmooth) + ",";
  json += "\"max\":" + String((int)rpmMax) + ",";
  json += "\"rpmMax\":" + String(RPM_MAX);
  json += "}";
  server.send(200, "application/json", json);
}

/* ================= SETUP ================= */
void setup() {
  Serial.begin(115200);

  pinMode(SENSOR_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(SENSOR_PIN), countPulse, FALLING);

  Wire.begin(21, 22);
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  display.setTextColor(SSD1306_WHITE);

  WiFi.softAP(ssid, password);

  if (MDNS.begin(mdnsName)) {
    Serial.println("mDNS ativo: esp32-rpm.local");
  }

  Serial.print("IP do ESP32: ");
  Serial.println(WiFi.softAPIP());

  showIPonOLED();
  delay(3000);

  server.on("/", []() {
    server.send(200, "text/html", dashboardHTML());
  });
  server.on("/api", handleAPI);
  server.begin();

  lastCalc = millis();
}

/* ================= LOOP ================= */
void loop() {
  server.handleClient();

  if (millis() - lastCalc >= 200) {
    noInterrupts();
    unsigned long pulses = pulseCount;
    pulseCount = 0;
    interrupts();

    rpm = (pulses * 60.0 * 5) / PULSES_PER_REV;
    rpmSmooth = rpmSmooth * 0.7 + rpm * 0.3;
    if (rpmSmooth > rpmMax) rpmMax = rpmSmooth;

    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0,0);
    display.print("RPM ");
    display.print((int)rpmSmooth);

    display.setCursor(0,10);
    display.print("MAX ");
    display.print((int)rpmMax);

    drawVUMeter((int)rpmSmooth);
    display.display();

    lastCalc = millis();
  }
}
