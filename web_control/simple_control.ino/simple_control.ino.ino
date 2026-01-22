#include <FastLED.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

#define NUM_LEDS 240
#define DATA_PIN 2 // D4 on breakout is GPIO2

CRGB leds[NUM_LEDS];
uint8_t r = 255, g = 100, b = 20; // Initial color
uint8_t brightness = 100;         // Initial brightness
bool ledState = true;             // LED on/off state

const char* ssid = "Shell";
const char* password = "411411411";

ESP8266WebServer server(80);

void setup() {
  Serial.begin(115200);

  EEPROM.begin(512);
  loadFromEEPROM();

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(brightness);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/update", handleUpdate);
  server.on("/save", handleSave);
  server.on("/toggle", handleToggle);
  server.on("/snake", handleSnake);
  server.on("/flash", handleFlash);
  server.begin();
}

void loop() {
  server.handleClient();

  if (ledState) {
    fill_solid(leds, NUM_LEDS, CRGB(r, g, b));
  } else {
    fill_solid(leds, NUM_LEDS, CRGB::Black);
  }
  FastLED.setBrightness(brightness);
  FastLED.show();
}

// Send the dynamic HTML page
void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>RGB Controller</title>
  <style>
    body { font-family: Arial; text-align: center; }
    button { margin: 5px; padding: 10px 20px; }
  </style>
</head>
<body>
  <h1>Mohan's RGB Controller</h1>
  <form action="/update" method="get">
    <label>R:</label> <input type="number" name="r" min="0" max="255" value=")rawliteral" + String(r) + R"rawliteral("><br>
    <label>G:</label> <input type="number" name="g" min="0" max="255" value=")rawliteral" + String(g) + R"rawliteral("><br>
    <label>B:</label> <input type="number" name="b" min="0" max="255" value=")rawliteral" + String(b) + R"rawliteral("><br>
    <label>Brightness:</label> <input type="number" name="brightness" min="0" max="255" value=")rawliteral" + String(brightness) + R"rawliteral("><br>
    <button type="submit">Update</button>
  </form>
  <button onclick="fetch('/save');">Save to EEPROM</button>
  <button onclick="fetch('/toggle');">On/Off</button>
  <button onclick="fetch('/snake');">Snake</button>
  <button onclick="fetch('/flash');">Slow Flash</button>
</body>
</html>
)rawliteral";

  server.send(200, "text/html", html);
}

// Update values based on form submission
void handleUpdate() {
  if (server.hasArg("r")) r = server.arg("r").toInt();
  if (server.hasArg("g")) g = server.arg("g").toInt();
  if (server.hasArg("b")) b = server.arg("b").toInt();
  if (server.hasArg("brightness")) brightness = server.arg("brightness").toInt();
  
  // Print updated values to Serial Monitor
  Serial.print("Updated Values - R: ");
  Serial.print(r);
  Serial.print(", G: ");
  Serial.print(g);
  Serial.print(", B: ");
  Serial.print(b);
  Serial.print(", Brightness: ");
  Serial.println(brightness);

  server.sendHeader("Location", "/");
  server.send(303);
}

void handleSave() {
  saveToEEPROM();
  Serial.println("Settings saved to EEPROM.");
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleToggle() {
  ledState = !ledState;
  Serial.print("LED State: ");
  Serial.println(ledState ? "ON" : "OFF");
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleSnake() {
  Serial.println("Snake effect activated.");
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB(r, g, b);
    FastLED.show();
    delay(50);
    leds[i] = CRGB::Black;
  }
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleFlash() {
  Serial.println("Slow Flash effect activated.");
  for (int i = 0; i < 10; i++) {
    fill_solid(leds, NUM_LEDS, CRGB(r, g, b));
    FastLED.show();
    delay(500);
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
    delay(500);
  }
  server.sendHeader("Location", "/");
  server.send(303);
}

void saveToEEPROM() {
  EEPROM.write(0, r);
  EEPROM.write(1, g);
  EEPROM.write(2, b);
  EEPROM.write(3, brightness);
  EEPROM.commit();
}

void loadFromEEPROM() {
  r = EEPROM.read(0);
  g = EEPROM.read(1);
  b = EEPROM.read(2);
  brightness = EEPROM.read(3);
}
