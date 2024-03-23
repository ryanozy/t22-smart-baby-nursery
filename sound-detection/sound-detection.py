#!/usr/bin/python
import RPi.GPIO as GPIO
import time
import socket
from paho.mqtt import client as mqtt_client

# GPIO SETUP
channel = 14
GPIO.setmode(GPIO.BCM)
GPIO.setup(channel, GPIO.IN)

broker = "192.168.96.194"
port = 1883
topic = 'sound-detection'
# Generate a Client ID with the publish prefix.
client_id = 'Sound-Detection-Module'
username = "mqtt-user"
password = "P@ssw0rd"

def connect_mqtt():
    def on_connect(client, userdata, flags, rc):
        if rc == 0:
            print("Connected to MQTT Broker!")
        else:
            print(f"Failed to connect, return code {rc}")
            try:
                client.reconnect()
            except socket.error:
                print("Network unreachable. Retrying in 5 seconds...")
                time.sleep(5)
                connect_mqtt()

    client = mqtt_client.Client(client_id)
    client.username_pw_set(username, password)
    client.on_connect = on_connect
    try:
        client.connect(broker, port)
    except socket.error:
        print("Network unreachable. Retrying in 5 seconds...")
        time.sleep(5)
        connect_mqtt()
    return client

def publish(client):
    def callback(channel):
        if GPIO.input(channel):
            print("GPIO pin %s is %s" % (channel, "HIGH"))
            msg = "Sound Detected"
            client.publish(topic, msg)
        else:
            print("GPIO pin %s is %s" % (channel, "LOW"))
            msg = "Sound Detected"
            client.publish(topic, msg)

    # let us know when the pin goes HIGH or LOW
    GPIO.add_event_detect(channel, GPIO.BOTH, bouncetime=100)
    # assign function to GPIO PIN, Run function on change
    GPIO.add_event_callback(channel, callback)

    # infinite loop
    while True:
        time.sleep(2)
        if not GPIO.event_detected(channel):
            try:
                client.publish(topic, "No Sound Detected")
            except (socket.error, ConnectionResetError):
                print("Network error occurred. Attempting to reconnect...")
                client.reconnect()

def run():
    client = connect_mqtt()
    publish(client)
    client.loop_forever()

if __name__ == '__main__':
    run()
