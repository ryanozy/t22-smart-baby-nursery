#!/usr/bin/python
import time
import socket
from paho.mqtt import client as mqtt_client

broker = "192.168.96.183"
port = 1883
topic = 'led-control'
# Generate a Client ID with the subscribe prefix.
client_id = f'Sound-Detection-Module-Sub-{time.time()}'

def connect_mqtt():
    def on_connect(client, userdata, flags, rc):
        if rc == 0:
            print("Connected to MQTT Broker!")
            client.subscribe(topic)
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


def calculate_latency(timestamp):
    current_time = int(time.time() * 1000)
    msg_latency = current_time - int(timestamp)
    print(f"Message latency: {msg_latency} ms")


def on_message(client, userdata, message):
    msg = message.payload.decode()
    print(f"Received message: {msg}")
    calculate_latency(msg)


def run():
    client = connect_mqtt()
    client.on_message = on_message
    client.loop_forever()


if __name__ == '__main__':
    run()
