#!/usr/bin/python
import time
import socket
from paho.mqtt import client as mqtt_client

broker = "192.168.96.183"
port = 1883
topic = 'led-control'
# Generate a Client ID with the publish prefix.
client_id = 'Sound-Detection-Module'


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
    client.on_connect = on_connect
    try:
        client.connect(broker, port)
    except socket.error:
        print("Network unreachable. Retrying in 5 seconds...")
        time.sleep(5)
        connect_mqtt()
    return client


def publish(client):

    msg = str(int(time.time() * 1000))
    client.publish(topic, msg)
    


def run():
    client = connect_mqtt()
    publish(client)
    client.loop_forever()


if __name__ == '__main__':
    run()
