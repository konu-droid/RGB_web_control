#include <FastLED.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <ESP8266mDNS.h>

#define NUM_LEDS 60
#define DATA_PIN 2 //D4 on breakout is GPIO2
#define ONBOARD_LED 2

CRGB leds[NUM_LEDS];
uint8_t r = 255, g = 100, b = 20;  // Initial color (warm white)
uint8_t brightness = 50;  // Initial brightness (full)
bool ledState[NUM_LEDS] = {true};  // State of each LED

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

  int timeout = 0;
  while (WiFi.status() != WL_CONNECTED && timeout < 60) {
    delay(500);
    Serial.print(".");
    timeout++;
  }
  Serial.println("");
  Serial.print("Connected to WiFi. IP address: ");
  Serial.println(WiFi.localIP());

  // --- ADD THIS SECTION FOR mDNS ---
  if (MDNS.begin("ledcontrol")) { // This means you will go to http://ledcontrol.local
    Serial.println("mDNS responder started: http://ledcontrol.local");
  }

  server.on("/", handleRoot);
  server.on("/update", handleUpdate);
  server.on("/save", handleSave);
  server.begin();
}

void loop() {
  server.handleClient();
  MDNS.update();
  
  for (int i = 0; i < NUM_LEDS; i++) {
    if (ledState[i]) {
      leds[i] = CRGB(r, g, b);
    } else {
      leds[i] = CRGB::Black;
    }
  }
  FastLED.setBrightness(brightness);
  FastLED.show();
}

void handleRoot() {
  String html = "<!DOCTYPE html>"
    "<html lang='en'>"
    "<head>"
        "<meta charset='UTF-8'>"
        "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
        "<title>LED Control</title>"
        "<style>"
            "body {"
                "font-family: Arial, sans-serif;"
                "display: flex;"
                "justify-content: center;"
                "align-items: center;"
                "min-height: 100vh;"
                "margin: 0;"
                "background-color: #f0f0f0;"
            "}"
            ".container {"
                "background-color: white;"
                "padding: 20px;"
                "border-radius: 10px;"
                "box-shadow: 0 0 10px rgba(0,0,0,0.1);"
            "}"
            "h1 {"
                "color: #333;"
                "text-align: center;"
            "}"
            ".control-group {"
                "display: flex;"
                "align-items: center;"
                "margin-bottom: 10px;"
            "}"
            "label {"
                "display: inline-block;"
                "width: 100px;"
            "}"
            "input[type='range'] {"
                "width: 200px;"
            "}"
            "input[type='number'] {"
                "width: 60px;"
                "margin-left: 10px;"
            "}"
            "#colorBox {"
                "width: 100px;"
                "height: 100px;"
                "margin: 20px auto;"
                "border: 1px solid #ddd;"
            "}"
            ".buttons {"
                "text-align: center;"
                "margin-top: 20px;"
            "}"
            "button {"
                "padding: 10px 20px;"
                "margin: 0 10px;"
                "font-size: 16px;"
                "cursor: pointer;"
            "}"
            ".led-grid {"
                "display: grid;"
                "grid-template-columns: repeat(10, 1fr);"
                "gap: 5px;"
                "margin-top: 20px;"
            "}"
            ".led-checkbox {"
                "width: 20px;"
                "height: 20px;"
            "}"
        "</style>"
    "</head>"
    "<body>"
        "<div class='container'>"
            "<h1>LED Control</h1>"
            "<form id='ledForm' action='/update' method='get'>"
                "<div class='control-group'>"
                    "<label for='r'>Red:</label>"
                    "<input type='range' id='r' name='r' min='0' max='255' value='" + String(r) + "' oninput='updateColor()'>"
                    "<input type='number' id='rNumber' min='0' max='255' value='" + String(r) + "' oninput='updateColorFromNumber()'>"
                "</div>"
                "<div class='control-group'>"
                    "<label for='g'>Green:</label>"
                    "<input type='range' id='g' name='g' min='0' max='255' value='" + String(g) + "' oninput='updateColor()'>"
                    "<input type='number' id='gNumber' min='0' max='255' value='" + String(g) + "' oninput='updateColorFromNumber()'>"
                "</div>"
                "<div class='control-group'>"
                    "<label for='b'>Blue:</label>"
                    "<input type='range' id='b' name='b' min='0' max='255' value='" + String(b) + "' oninput='updateColor()'>"
                    "<input type='number' id='bNumber' min='0' max='255' value='" + String(b) + "' oninput='updateColorFromNumber()'>"
                "</div>"
                "<div class='control-group'>"
                    "<label for='bright'>Brightness:</label>"
                    "<input type='range' id='bright' name='bright' min='0' max='255' value='" + String(brightness) + "' oninput='updateBrightness()'>"
                    "<input type='number' id='brightNumber' min='0' max='255' value='" + String(brightness) + "' oninput='updateBrightnessFromNumber()'>"
                "</div>"
                "<div id='colorBox'></div>"
                "<div class='led-grid'>";

  for (int i = 0; i < NUM_LEDS; i++) {
    html += "<input type='checkbox' class='led-checkbox' id='led" + String(i) + "' name='led" + String(i) + "' " + (ledState[i] ? "checked" : "") + ">";
  }

  html += "</div>"
                "<div class='buttons'>"
                    "<button type='submit'>Update</button>"
                    "<button type='button' onclick='saveToEEPROM()'>Save to EEPROM</button>"
                "</div>"
            "</form>"
        "</div>"
        "<script>"
            "function updateColor() {"
                "var r = document.getElementById('r').value;"
                "var g = document.getElementById('g').value;"
                "var b = document.getElementById('b').value;"
                "document.getElementById('colorBox').style.backgroundColor = 'rgb('+r+','+g+','+b+')';"
                "document.getElementById('rNumber').value = r;"
                "document.getElementById('gNumber').value = g;"
                "document.getElementById('bNumber').value = b;"
            "}"
            "function updateColorFromNumber() {"
                "var r = document.getElementById('rNumber').value;"
                "var g = document.getElementById('gNumber').value;"
                "var b = document.getElementById('bNumber').value;"
                "document.getElementById('r').value = r;"
                "document.getElementById('g').value = g;"
                "document.getElementById('b').value = b;"
                "updateColor();"
            "}"
            "function updateBrightness() {"
                "document.getElementById('brightNumber').value = document.getElementById('bright').value;"
            "}"
            "function updateBrightnessFromNumber() {"
                "document.getElementById('bright').value = document.getElementById('brightNumber').value;"
            "}"
            "function saveToEEPROM() {"
                "var xhr = new XMLHttpRequest();"
                "xhr.open('GET', '/save', true);"
                "xhr.send();"
            "}"
            "updateColor();"
            "updateBrightness();"
        "</script>"
    "</body>"
    "</html>";
  Serial.println(html.length());
  server.send(200, "text/html", html);
}

void handleUpdate() {
  if (server.hasArg("r")) {
    r = server.arg("r").toInt();
  }
  if (server.hasArg("g")) {
    g = server.arg("g").toInt();
  }
  if (server.hasArg("b")) {
    b = server.arg("b").toInt();
  }
  if (server.hasArg("bright")) {
    brightness = server.arg("bright").toInt();
  }
  
  for (int i = 0; i < NUM_LEDS; i++) {
    String ledArg = "led" + String(i);
    ledState[i] = server.hasArg(ledArg);
  }
  
  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "Updated");
}

void handleSave() {
  saveToEEPROM();
  server.send(200, "text/plain", "Saved to EEPROM");
}

void saveToEEPROM() {
  EEPROM.write(0, r);
  EEPROM.write(1, g);
  EEPROM.write(2, b);
  EEPROM.write(3, brightness);
  for (int i = 0; i < NUM_LEDS; i++) {
    EEPROM.write(4 + i, ledState[i]);
  }
  EEPROM.commit();
}

void loadFromEEPROM() {
  r = EEPROM.read(0);
  g = EEPROM.read(1);
  b = EEPROM.read(2);
  brightness = EEPROM.read(3);
  for (int i = 0; i < NUM_LEDS; i++) {
    ledState[i] = EEPROM.read(4 + i);
  }
}