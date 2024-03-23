# T22 Smart Baby Nursery Project
This project will be a proof-of-concept for our IoT module, where we will design a smart baby nursery. Leveraging on **Smart Home** technology to build an all-in-one **Smart Baby Nursery**

## Project Equipment
1. Raspberry Pi4 (MQTT Broker)
2. Raspberry Pi Pico W
3. M5 StickC-Plus
4. Temperature + Humidity Sensor (MQTT Topic: `temperature-humidity`)
5. Servos Motor (SG995/SG90) (MQTT Topics: `motor-movement` and `curtain-movement`)
6. DC Motor
7. Raspberry Pi-Cam 2
8. LED-Lights (MQTT Topic: `led-control`)
9. Sound Sensor (KY037 Sound Detection Module) / ~~USB Microphone~~
10. Raspberry Pi3 

## Software/Protocols/Technology Used
1. Python
2. Home Assistant
3. MQTT
4. Arduino / C

## Project Logs
1. We will abandon the usage of the Pico W, as we had some issues setting up the MQTT protocol on the Pico W. We will be using the M5 StickC-Plus instead.
2. Currently unable to connect the Pi-Cam to the Raspberry Pi4, as the Pi-Cam is incompatible with the Raspberry Pi4. We will be using a Pi3 instead.
3. Due to the power consumption of the SG995 Servo Motor, we will use the SG90 Micro Servo Motor Module to allow the M5StickC Plus to control the movement via PWM.
4. For the Pi-Camera, we will utilise the in-built customisation within Home Assistant to stream the video feed to Home Assistant for the Homeowner to view the baby.
~~5. To limit the MQTT traffic in attempts to reduce the latency, we will update the DHT11 Temperature and Humidity Sensors to update on Command from the Home Assistant~~
6. Added the scripts to autorun on startup (For Pi-Cam and Sound Detection Sensors)
7. Added the automation scripts to the Home Assistant OS
8. Added user customisation for Automation values into the Home Assistant Dashboard
    * Customise threshold to automatically turn on or off the fan
    * Customise response to baby crying. *Fan*, *Crib Rocker* and *LED*
9. Implement security measure on the Raspberry Pi 3 *(With the Pi Cam)*. To ensure that only the Home Assistant can communicate with the Camera, and secure the Baby Monitor Footage

## Project Setup
(In construction...)

## Protocol Logs
1. First, we landed on MQTT because we did not need to use long-range protocols, thus ruling out LoRa.
2. Second, we decided against subscription-based protocols like Z-Wave and Zigbee, reducing overhead costs and allowing us to use IoT devices that aren't compatible with these protocols.
3. We will not use the external Broker. Instead, we will utilise the internal Home Assistant Add-On MQTT Broker to save on the number of Raspberry Pi used for this project.

## Pinouts
#### Blinds Control
1. Servo Motor Red Wire (5V) to M5StickC Plus Grey Wire (5V Output)
2. Servo Motor Brown Wire (GND) to M5StickC Plus Red Wire (GND)
3. Servo Motor Orange Wire (PWM) to M5StickC Plus Green Wire (G26)

#### Crib Control
1. Servo Motor Red Wire (5V) to M5StickC Plus Blue Wire (5V Output)
2. Servo Motor Brown Wire (GND) to M5StickC Plus Green Wire (GND)
3. Servo Motor Orange Wire (PWM) to M5StickC Plus Black Wire (G26)

#### LED Control
1. LED Longer Wire to G26
2. LED Shorter Wire to GND

#### DHT11 Sensor
1. DHT11 Red Wire (VCC PIN) to M5StickC Plus 3V3 PIN
2. DHT11 Yellow Wire (Data) to M5StickC Plus G26 PIN
3. DHT11 Orange Wire (GND) to M5StickC Plus GND PIN

#### KY037 - Sound Detection Module
1. KY037 Red Wire (+) to Raspberry Pi 3 PIN 4 (5V Output)
2. KY037 Yellow Wire (GND) to Raspberry Pi 3 PIN 6 (GND)
3. KY037 Orange Wire (Digital Pin / D0) to Raspberry Pi 3 PIN 8 (GPIO 14)

## References and Sources:
1. https://www.instructables.com/ESP32-Based-M5Stack-M5stick-C-Weather-Monitor-With/
2. https://randomnerdtutorials.com/how-to-install-mosquitto-broker-on-raspberry-pi/
3. https://gist.github.com/hkaji/8beef2790886b383e82e02585f9a5071
4. https://www.home-assistant.io/integrations/stream/
5. https://einar.slaskete.net/2018/08/16/using-a-raspberry-pi-as-a-surveillance-camera-in-home-assistant/
6. https://www.instructables.com/Sound-Sensor-Raspberry-Pi/
7. https://gist.github.com/emxsys/a507f3cad928e66f6410e7ac28e2990f


