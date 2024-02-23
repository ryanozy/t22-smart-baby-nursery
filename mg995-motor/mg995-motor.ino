#include "M5StickCPlus.h"
#include <WiFi.h>
#include <PubSubClient.h>

WiFiClient espClient;
PubSubClient client(espClient);

const char* ssid        = "Pixel_5725A";
const char* password    = "82208220*Tt";
const char* mqtt_server = "192.168.12.183";

#define MOTOR_PIN 26

void setupWifi();
void callback(char* topic, byte* payload, unsigned int length);
void reConnect();

void setup() {
    M5.begin();
    M5.Lcd.setRotation(3);
    setupWifi();
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);

    pinMode(MOTOR_PIN, OUTPUT);
    digitalWrite(MOTOR_PIN, LOW);
}

void loop() {
    if (!client.connected()) {
        reConnect();
    }
    client.loop();

    // Other loop tasks if any
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
    if (strcmp(topic, "motor-movement") == 0) {
        // Convert payload to integer (PWM duty cycle)
        int dutyCycle = atoi((char *)payload);

        // Ensure duty cycle is within valid range (0-255)
        dutyCycle = constrain(dutyCycle, 0, 255);

        // Send PWM signal to motor pin
        analogWrite(MOTOR_PIN, dutyCycle);
    }
}

void reConnect() {
    while (!client.connected()) {
        M5.Lcd.print("Attempting MQTT connection...");
        String clientId = "M5Stack-";
        clientId += String(random(0xffff), HEX);
        if (client.connect(clientId.c_str())) {
            M5.Lcd.printf("\nSuccess\n");
            // Subscribe to the motor movement topic
            client.subscribe("motor-movement");
        } else {
            M5.Lcd.print("failed, rc=");
            M5.Lcd.print(client.state());
            M5.Lcd.println("try again in 5 seconds");
            delay(5000);
        }
    }
}
