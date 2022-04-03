#include <Arduino.h>
#include <ESPAsyncWebServer.h>

#ifdef ESP32
  #include <WiFi.h>
  #include <AsyncTCP.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
#endif

#define USE_INTRANET true
#define DEBUG true

#include "credentials.h"
#include "debug.h"

AsyncWebServer server(80);

bool connectToAccessPoint() {
  #if USE_INTRANET
    log("Connecting to WiFi ... ");
    WiFi.mode(WIFI_STA);
    WiFi.begin(WiFi_SSID, WiFi_PASSWORD);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
      logln("Failed!");
      return false;
    }
    logln("OK");
    log("IP Address: "); logln(WiFi.localIP());
  #else
    logln("Creating Access Point ... ");
    WiFi.softAP(AP_SSID, AP_PASSWORD);
    logln("OK");
    log("IP Address: "); logln(WiFi.softAPIP());
  #endif

  return true;
}

void initServer() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/html", "<h1>Hello world</h1>");
  });

  server.begin();
}

void setup() {
  // Init serial connection
  Serial.begin(115200);
  while (!Serial && !Serial.available()) {}
  Serial.println();

  // Init server
  if(!connectToAccessPoint()) return;
  initServer();
}

void loop() {}