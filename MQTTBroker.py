import paho.mqtt.server as mqtt

def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))
    client.subscribe("test/topic")

def on_message(client, userdata, msg):
    print(msg.topic + " " + str(msg.payload))

def on_publish(client, userdata, mid):
    print("Message published")

def on_subscribe(client, userdata, mid, granted_qos):
    print("Subscribed to topic")

def on_disconnect(client, userdata, rc):
    if rc != 0:
        print("Unexpected disconnection")

broker = mqtt.MQTTServer()
broker.on_connect = on_connect
broker.on_message = on_message
broker.on_publish = on_publish
broker.on_subscribe = on_subscribe
broker.on_disconnect = on_disconnect

broker.bind("localhost", 8883)
broker.start()