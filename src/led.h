/**
 * ----------------------------------------------------------------
 * ESP32 Web App to control a LED
 * ----------------------------------------------------------------
 * © 2020 Stéphane Calderoni
 * ----------------------------------------------------------------
 */

#pragma once

#include <WiFi.h>
#include <SPIFFS.h>
#include <AsyncElegantOTA.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include <Arduino.h>

// ----------------------------------------------------------------
// Loads WiFi credentials
// ----------------------------------------------------------------

#include "secret.h"

// ----------------------------------------------------------------
// Global constants
// ---------------------------------------------------------------

const uint16_t HTTP_PORT = 80;
const uint16_t WS_PORT   = 1234;
const uint8_t  LED_PIN   = 32;

// ----------------------------------------------------------------
// Global variables
// ----------------------------------------------------------------

uint8_t ledState = 0;

AsyncWebServer   server(HTTP_PORT);
WebSocketsServer webSocket(WS_PORT);

// ----------------------------------------------------------------
// WebServer management
// ----------------------------------------------------------------

void onPageRequest(AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/led/index.html", "text/html");
}

void onStyleRequest(AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/led/index.css", "text/css");
}

void onScriptRequest(AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/led/index.js", "text/javascript");
}

void onPageNotFound(AsyncWebServerRequest * request) {
    request->send(404, "text/plain", "Not found");
}

// ----------------------------------------------------------------
// WebSockets management
// ----------------------------------------------------------------

bool isEqual(uint8_t * payload, char * message) {
    return strcmp((char *)payload, message) == 0;
}

void onWebSocketEvent(uint8_t clientID, WStype_t type, uint8_t * payload, size_t length) {
    if (type == WStype_TEXT) {
        if (isEqual(payload, "toggleLED")) {
            ledState = !ledState;
            digitalWrite(LED_PIN, ledState);
        } else if (isEqual(payload, "getLEDState")) {
            webSocket.sendTXT(clientID, ledState ? "1" : "0");
        }
    }
}

// ----------------------------------------------------------------
// Initialization
// ----------------------------------------------------------------

void initLEDs() {
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(LED_PIN,     OUTPUT);
}

void initSerial() {
    Serial.begin(115200);
    delay(1000);
}

void initSPIFFS() {
    if (!SPIFFS.begin()) {
        Serial.println("Cannot mount SPIFFS volume...");
        while(1) digitalWrite(LED_BUILTIN, millis() % 200 < 50 ? HIGH : LOW);
    }
}

/**
 * if you prefer to set up an access point
 */
void initWiFiAP() {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(SSID, PASSWORD);
    IPAddress ip = WiFi.softAPIP();
    Serial.println("WiFi Access Point initialized");
    Serial.print("ESP32 IP address: ");
    Serial.println(ip);
}

/**
 * if you prefer to use an ambient WiFi network
 */
void initWiFi() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(SSID, PASSWORD);
    Serial.print("Trying to connect");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.println();
    Serial.printf("Connected to %s\n", SSID);
    Serial.print("ESP32 IP address: ");
    Serial.println(WiFi.localIP());
}

void initWebServer() {
    server.on("/",          HTTP_GET, onPageRequest);
    server.on("/index.css", HTTP_GET, onStyleRequest);
    server.on("/index.js",  HTTP_GET, onScriptRequest);
    server.onNotFound(onPageNotFound);
    AsyncElegantOTA.begin(&server);
    server.begin();
}

void initWebSockets() {
    webSocket.begin();
    webSocket.onEvent(onWebSocketEvent);
}

void setup() {
    initLEDs();
    initSerial();
    initSPIFFS();
    initWiFi();
    initWebServer();
    initWebSockets();
}

// ----------------------------------------------------------------
// Main control loop
// ----------------------------------------------------------------

void loop() {
    digitalWrite(LED_BUILTIN, millis() % 1000 < 50 ? HIGH : LOW);
    webSocket.loop();
    AsyncElegantOTA.loop();
}