#include "M5StickCPlus.h"
#include <WiFi.h>
#include <PubSubClient.h>

WiFiClient espClient;
PubSubClient client(espClient);

const char* ssid        = "ZP hotspot";
const char* password    = "mynameisjeff2";
const char* mqtt_server = "192.168.105.194";
const char* username    = "mqtt-client";
const char* pwd         = "P@ssw0rd";

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

/**
 * @brief Callback function for handling messages received on subscribed topics.
 * 
 * This function is called when a message is received on the "led-control" topic.
 * It extracts the LED control command from the payload and performs the corresponding action.
 * 
 * @param topic The topic of the received message.
 * @param payload The payload of the received message.
 * @param length The length of the payload.
 */
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
        if (client.connect("person.mqtt_broker", username, password)) {
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
