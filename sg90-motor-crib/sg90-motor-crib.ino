#include "M5StickCPlus.h"
#include <WiFi.h>
#include <PubSubClient.h>

WiFiClient espClient;
PubSubClient client(espClient);

const char *ssid = "Pixel_5725A";     // WiFi SSID
const char *password = "82208220*Tt"; // WiFi password
const char *mqtt_server = "192.168.96.194";
const char *username = "mqtt-user";
const char *pwd = "P@ssw0rd";

const double PWM_Hz = 50;            // PWM frequency
const uint8_t PWM_level = 16;        // PWM 16bit(0～65535)
const uint16_t MIN_degree0 = 1638;   // 0.5 / 20 * 65535
const uint16_t MAX_degree180 = 7864; // 2.4 / 20 * 65535

int movement = (MAX_degree180 - MIN_degree0) / 18; // move by 10 degrees each time
bool isRocking = false;
// Define variables for smoother movement
const int steps = 50; // Number of steps for smoother movement
const int delayTime = 10; // Delay time between each step

#define MOTOR_PIN 26

void setupWifi();
void callback(char *topic, byte *payload, unsigned int length);
void reConnect();

void setup()
{
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

void loop()
{
    if (!client.connected())
    {
        reConnect();
    }
    client.loop();

    if (isRocking)
    {
        smoothMove(MIN_degree0, MIN_degree0 + movement, steps);
        smoothMove(MIN_degree0 + movement, MIN_degree0 + movement + movement, steps);
        smoothMove(MIN_degree0 + movement + movement, MIN_degree0 + movement, steps);
        smoothMove(MIN_degree0 + movement, MIN_degree0, steps);
    }
}

void smoothMove(int startPos, int endPos, int numSteps)
{
    int posIncrement = (endPos - startPos) / numSteps;
    for (int i = 0; i < numSteps; i++)
    {
        int newPos = startPos + (posIncrement * i);
        ledcWrite(1, newPos);
        delay(delayTime);
    }
}

void setupWifi()
{
    delay(10);
    M5.Lcd.printf("Connecting to %s", ssid);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        M5.Lcd.print(".");
    }
    M5.Lcd.printf("\nSuccess\n");
    Serial.println(WiFi.localIP());
    M5.Lcd.print("IP: ");
    M5.Lcd.print(WiFi.localIP());
}

void callback(char *topic, byte *payload, unsigned int length)
{
    // Handle messages received on subscribed topics
    if (strcmp(topic, "motor-movement") == 0)
    {
        Serial.println("Received Msg");

        // Extract the movement duration from the payload and convert it to an integer
        char durationString[length + 1];
        memcpy(durationString, payload, length);
        durationString[length] = '\0';

        if (strcmp(durationString, "rock") == 0)
        {
            isRocking = true;
        }
        else if (strcmp(durationString, "stop") == 0)
        {
            isRocking = false;
        }
    }
}

void reConnect()
{
    while (!client.connected())
    {
        M5.Lcd.print("Attempting MQTT connection...");
        String clientId = "M5Stack-Baby-Crib";
        if (client.connect(clientId.c_str(), username, pwd))
        {
            M5.Lcd.printf("\nSuccess\n");
            // Subscribe to the motor movement topic
            client.subscribe("motor-movement");
        }
        else
        {
            M5.Lcd.print("failed, rc=");
            M5.Lcd.print(client.state());
            M5.Lcd.println("try again in 5 seconds");
            delay(5000);
        }
    }
}
