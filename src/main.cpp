#include <AceButton.h>
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
#include "servo.h"

Servo SERVOS[] = {
    Servo(48, 530, 7, 219.090912, &SPECS_SG_90),
    Servo(48, 530, 7.164179, 215.820908, &SPECS_SG_90),
    Servo(48, 530, -0.918367, 221.326523, &SPECS_SG_90),
    Servo(48, 530, 6.887755, 221.326523, &SPECS_SG_90),
};
const uint8_t NUM_SERVOS = sizeof(SERVOS) / sizeof(Servo);
Adafruit_PWMServoDriver servoDriver = Adafruit_PWMServoDriver(0x40);

uint8_t BUTTON_PINS[NUM_SERVOS] = {
    GPIO_NUM_32, GPIO_NUM_33, GPIO_NUM_25, GPIO_NUM_26,
    // GPIO_NUM_27, GPIO_NUM_14, GPIO_NUM_12,
};
ace_button::AceButton BUTTONS[NUM_SERVOS];

struct Gate {
    bool isOpen;
};
Gate GATES[NUM_SERVOS] = {{false}, {false}, {false}, {false}};
uint8_t lastServoId = 0;
unsigned long nextServoMoveTime = 0;

AsyncWebServer server(80);
IPAddress ip;

void initSerial() {
    Serial.begin(115200);
    while (!Serial && !Serial.available()) {
    }
    Serial.println();
}

void turnOffAllServos() {
    for (int i = 0; i < NUM_SERVOS; i++) {
        servoDriver.setPin(i, 0);
    }
}

void initServoDriver() {
    servoDriver.begin();
    servoDriver.setOscillatorFrequency(27000000);
    servoDriver.setPWMFreq(SERVOS[0].specs->frequency);
    turnOffAllServos();
}

void handleButtonEvent(ace_button::AceButton* btn, uint8_t eventType, uint8_t btnState) {
    if (eventType != ace_button::AceButton::kEventClicked)
        return;
    if (millis() < nextServoMoveTime) {
        log_i("Another servo is already moving. Next servo can move in: %d ms\n",
              nextServoMoveTime - millis());
        return;
    }

    uint8_t id = btn->getId();
    Gate* gate = &GATES[id];
    Servo* servo = &SERVOS[id];

    // Toggle gate state
    gate->isOpen = !gate->isOpen;
    log_i("Gate %d: %s", id, gate->isOpen ? "Openening" : "Closing");

    lastServoId = id;
    nextServoMoveTime = millis() + 1000;

    // Move servo
    if (gate->isOpen) {
        servoDriver.setPin(id, servo->angleToTicks(-90));
    } else {
        servoDriver.setPin(id, servo->angleToTicks(90));
    }
}

void initButtons() {
    ace_button::ButtonConfig* cfg = ace_button::ButtonConfig::getSystemButtonConfig();
    cfg->setFeature(ace_button::ButtonConfig::kFeatureClick);
    cfg->setEventHandler(handleButtonEvent);

    for (int i = 0; i < NUM_SERVOS; i++) {
        pinMode(BUTTON_PINS[i], INPUT_PULLDOWN);
        BUTTONS[i].init(BUTTON_PINS[i], LOW, i);
    }
}

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

void setup() {
    initSerial();
    initServoDriver();
    initButtons();

    // Connect to WiFi. Create Access Point as a fallback
    // if (!connectToWiFi() && !createAccessPoint())
    //     return;
    // initServer();
}

void loop() {
    for (int i = 0; i < NUM_SERVOS; i++) {
        BUTTONS[i].check();
    }
}
