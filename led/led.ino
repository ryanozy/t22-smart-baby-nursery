#include "M5StickCPlus.h"
#include <WiFi.h>
#include <PubSubClient.h>

WiFiClient espClient;
PubSubClient client(espClient);

const char* ssid        = "Pixel_5725A";
const char* password    = "82208220*Tt";
const char* mqtt_server = "192.168.96.167";

#define LED_PIN 26

void setupWifi();
void callback(char* topic, byte* payload, unsigned int length);
void reConnect();

void setup() {
    M5.begin();
    Serial.begin(115200);
    M5.Lcd.setRotation(3);
    setupWifi();
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);

    pinMode(LED_PIN, OUTPUT);
}

void loop() {
    if (!client.connected()) {
        reConnect();
    }
    client.loop();
}

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

void callback(char* topic, byte* payload, unsigned int length) {
    // Handle messages received on subscribed topics
    if (strcmp(topic, "led-control") == 0) {
      Serial.println("Received LED Control Msg");

      // Extract the LED control command from the payload
      char controlCommand[length + 1];
      memcpy(controlCommand, payload, length);
      controlCommand[length] = '\0';

      if (strcmp(controlCommand, "on") == 0) {
        digitalWrite(LED_PIN, HIGH);
      } else if (strcmp(controlCommand, "off") == 0) {
        digitalWrite(LED_PIN, LOW);
      }
    }
}

void reConnect() {
    while (!client.connected()) {
        M5.Lcd.print("Attempting MQTT connection...");
        String clientId = "M5Stack-";
        clientId += String(random(0xffff), HEX);
        if (client.connect(clientId.c_str())) {
            M5.Lcd.printf("\nSuccess\n");
            // Subscribe to the LED control topic
            client.subscribe("led-control");
        } else {
            M5.Lcd.print("failed, rc=");
            M5.Lcd.print(client.state());
            M5.Lcd.println("try again in 5 seconds");
            delay(5000);
        }
    }
}
