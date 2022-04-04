#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <WiFi.h>

#define USE_INTRANET true
#define DEBUG true

#include "credentials.h"
#include "debug.h"

const char* hostname = "lokschuppen";
AsyncWebServer server(80);

void handleRoot(AsyncWebServerRequest* request) {
    if (!SPIFFS.begin()) {
        logln("Error: Cannot mount filesystem");
        return;
    }

    request->send(SPIFFS, "/index.html", "text/html");
}

bool connectToAccessPoint() {
#if USE_INTRANET
    log("Connecting to WiFi ... ");
    WiFi.mode(WIFI_STA);
    WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
    WiFi.setHostname(hostname);
    WiFi.begin(WiFi_SSID, WiFi_PASSWORD);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        logln("Failed!");
        return false;
    }
    logln("OK");
    log("IP Address: ");
    logln(WiFi.localIP());
#else
    logln("Creating Access Point ... ");
    WiFi.softAP(AP_SSID, AP_PASSWORD);
    logln("OK");
    log("IP Address: ");
    logln(WiFi.softAPIP());
#endif

    return true;
}

void initServer() {
    server.on("/", HTTP_GET, handleRoot);
    server.serveStatic("/", SPIFFS, "/");
    server.begin();
}

void setup() {
    // Init serial connection
    Serial.begin(115200);
    while (!Serial && !Serial.available()) {
    }
    Serial.println();

    // Init server
    if (!connectToAccessPoint())
        return;
    initServer();
}

void loop() {}
