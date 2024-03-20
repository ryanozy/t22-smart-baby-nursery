/*
*******************************************************************************
* Copyright (c) 2021 by M5Stack
*                  Equipped with M5StickC-Plus sample source code
*                             M5StickC-Plus
* Visit for more information: https://docs.m5stack.com/en/core/m5stickc_plus
*
* Describe: MQTT
* Date: 2021/11/5
*******************************************************************************
*/
#include "M5StickCPlus.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"

#define DHTPIN 26
#define DHTTYPE DHT11

WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHTPIN, DHTTYPE);

const char* ssid        = "Pixel_5725A";
const char* password    = "82208220*Tt";
const char* mqtt_server = "192.168.96.194"; // Update according to your MQTT server
const char* username    = "mqtt-user";
const char* pwd         = "P@ssw0rd";

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE]; 
int value = 0;

void setupWifi();
void reConnect();

void setup() {
    M5.begin();
    M5.Lcd.setRotation(3);
    setupWifi();
    client.setServer(mqtt_server, 1883);

    dht.begin();
}

void loop() {
    if (!client.connected()) {
        reConnect();
    }
    client.loop();

    delay(2000);

    float h = dht.readHumidity();
    float t = dht.readTemperature();

    if (isnan(h) || isnan(t)) {
        Serial.println("Failed to read from DHT sensor!");
        return;
    }

    snprintf (msg, MSG_BUFFER_SIZE, "temperature:%.2f, humidity:%.2f", t, h);
    client.publish("temperature-humidity", msg);
}

/**
 * Setup WiFi connection
 */
void setupWifi() {
    delay(10);
    M5.Lcd.printf("Connecting to %s", ssid);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        M5.Lcd.print(".");
    }
    M5.Lcd.printf("\nSuccess\n");
    Serial.println(WiFi.localIP());
    M5.Lcd.print("IP: ");
    M5.Lcd.print(WiFi.localIP());
}

/**
 * Reconnect to MQTT server
 */
void reConnect() {
    while (!client.connected()) {
        M5.Lcd.print("Attempting MQTT connection...");
        String clientId = "M5Stack-Temp-Sensor";
        if (client.connect(clientId.c_str(), username, pwd)) {
            M5.Lcd.printf("\nSuccess\n");
        } else {
            M5.Lcd.print("failed, rc=");
            M5.Lcd.print(client.state());
            M5.Lcd.println("try again in 5 seconds");
            delay(5000);
        }
    }
}
