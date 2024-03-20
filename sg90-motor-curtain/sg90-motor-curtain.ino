#include "M5StickCPlus.h"
#include <WiFi.h>
#include <PubSubClient.h>

WiFiClient espClient;
PubSubClient client(espClient);

const char* ssid        = "Pixel_5725A";     // WiFi SSID
const char* password    = "82208220*Tt";    // WiFi password
const char* mqtt_server = "192.168.96.194 "; // MQTT server IP address
const char* username    = "mqtt-user";
const char* pwd         = "P@ssw0rd";

const double PWM_Hz = 50;   // PWM frequency
const uint8_t PWM_level = 16; // PWM 16bit(0～65535)
const uint16_t MIN_degree0 = 1638;    // 0.5 / 20 * 65535
const uint16_t MAX_degree180 = 7864;  // 2.4 / 20 * 65535

int movement = (MAX_degree180 - MIN_degree0) / 3; // move by 60 degrees each time

#define MOTOR_PIN 26

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

    pinMode(MOTOR_PIN, OUTPUT);
    ledcSetup((uint8_t)1, PWM_Hz, PWM_level);
    ledcAttachPin(MOTOR_PIN, 1);
    ledcWrite(1, MIN_degree0);
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
    if (strcmp(topic, "blinds-movement") == 0) {
      Serial.println("Received Msg");

      // Extract the movement duration from the payload and convert it to an integer
      char movementString[length + 1];
      memcpy(movementString, payload, length);
      movementString[length] = '\0';

      if (strcmp(movementString, "open") == 0) {
        ledcWrite(1, MIN_degree0); // Move the motor to the minimum position (0 degrees)
      } else if (strcmp(movementString, "close") == 0) {
        ledcWrite(1, MAX_degree180); // Move the motor to the maximum position (180 degrees)
      }
    }
}

void reConnect() {
    while (!client.connected()) {
        M5.Lcd.print("Attempting MQTT connection...");
        String clientId = "M5Stack-Blinds-Control";
        if (client.connect(clientId.c_str(), username, pwd)) {
            M5.Lcd.printf("\nSuccess\n");
            // Subscribe to the motor movement topic
            client.subscribe("blinds-movement");
        } else {
            M5.Lcd.print("failed, rc=");
            M5.Lcd.print(client.state());
            M5.Lcd.println("try again in 5 seconds");
            delay(5000);
        }
    }
}
