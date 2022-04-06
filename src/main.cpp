#include <Adafruit_PWMServoDriver.h>
#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <Wire.h>

#define USE_INTRANET true
#define DEBUG true

#include "credentials.h"
#include "debug.h"

const size_t BUTTON_PINS[] = {GPIO_NUM_12, GPIO_NUM_14, GPIO_NUM_27, GPIO_NUM_26,
                              GPIO_NUM_25, GPIO_NUM_33, GPIO_NUM_32};
const size_t NUM_BUTTONS = sizeof(BUTTON_PINS) / sizeof(size_t);

struct ServoTickRange {
    uint16_t min;
    uint16_t max;
};

const ServoTickRange SERVO_TICK_RANGES[] = {{150, 586}, {150, 452}, {150, 490}, {150, 479},
                                            {150, 463}, {150, 492}, {150, 465}};

AsyncWebServer server(80);
IPAddress ip;

#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates
Adafruit_PWMServoDriver servoController = Adafruit_PWMServoDriver(0x40);

void handleRoot(AsyncWebServerRequest* request) {
    if (!SPIFFS.begin()) {
        logln("Error: Cannot mount filesystem");
        return;
    }

    request->send(SPIFFS, "/index.html", "text/html");
}

bool connectToWiFi() {
    log_i("Connecting to WiFi as '%s' ... ", WiFi_HOSTNAME);
    WiFi.mode(WIFI_STA);
    WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
    WiFi.setHostname(WiFi_HOSTNAME);
    WiFi.begin(WiFi_SSID, WiFi_PASSWORD);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
        log_i("Failed!");
        return false;
    }
    log_i("OK");
    ip = WiFi.localIP();
    log_i("IP Address: %d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
    return true;
}

bool createAccessPoint() {
    log_i("Creating Access Point '%s' ... ", AP_SSID);
    WiFi.softAP(AP_SSID, AP_PASSWORD);
    log_i("OK");
    ip = WiFi.softAPIP();
    log_i("IP Address: %d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
    return true;
}

void initServer() {
    server.on("/", HTTP_GET, handleRoot);
    server.serveStatic("/", SPIFFS, "/");
    server.begin();
}

void initServoController() {
    servoController.begin();
    servoController.setOscillatorFrequency(27000000);
    servoController.setPWMFreq(SERVO_FREQ);
}

unsigned int angleToTicks(unsigned int angle, uint16_t min_ticks, uint16_t max_ticks) {
    return map(angle, 0, 180, min_ticks, max_ticks);
}

void setup() {
    // Init button pins
    for (int i = 0; i < NUM_BUTTONS; i++) {
        pinMode(BUTTON_PINS[i], INPUT_PULLDOWN);
    }

    // Init serial connection
    Serial.begin(115200);
    while (!Serial && !Serial.available()) {
    }
    Serial.println();

    // Connect to WiFi. Create Access Point as a fallback
    if (!connectToWiFi() && !createAccessPoint())
        return;
    initServer();

    initServoController();
}

void loop() {}
