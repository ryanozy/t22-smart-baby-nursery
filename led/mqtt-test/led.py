#!/usr/bin/python
import time
import socket
from paho.mqtt import client as mqtt_client

broker = "192.168.96.183"
port = 1883
topic = 'led-control'
# Generate a Client ID with the publish prefix.
client_id = 'Sound-Detection-Module'

time_start = int(time.time() * 1000)
time_end = 0

def connect_mqtt():
    def on_connect(client, userdata, flags, rc):
        if rc == 0:
            print("Connected to MQTT Broker!")
            client.subscribe('led-ack')
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

    client.publish(topic, "on")
    
def on_message(client, userdata, message):
    msg = message.payload.decode()
    print(f"Received message: {msg}")
    if msg == "Command received":
        time_end = int(time.time() * 1000)
        print(f"Latency: {(time_end - time_start)}ms")
        # Print start and end time
        print(f"Start time: {time_start}")
        print(f"End time: {time_end}")
        client.disconnect()

def run():
    client = connect_mqtt()
    publish(client)
    client.on_message = on_message
    client.loop_forever()


if __name__ == '__main__':
    run()
