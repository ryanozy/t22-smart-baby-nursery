import RPi.GPIO as GPIO
from paho.mqtt import client as mqtt_client

# Define MQTT parameters
MQTT_IP = "192.168.96.194"
MQTT_PORT = 1883
MQTT_TOPIC = "fan"
MQTT_CLIENT_ID = "PiFan"
MQTT_USERNAME = "mqtt-user"
MQTT_PASSWORD = "P@ssw0rd"

# Define the GPIO pin that the fan is connected to
FAN_PIN_IN1 = 20
FAN_PIN_IN2 = 21

# Set the GPIO pin to be an output
GPIO.setmode(GPIO.BCM)
GPIO.setup(FAN_PIN_IN1, GPIO.OUT)
GPIO.setup(FAN_PIN_IN2, GPIO.OUT)

# Function to turn the fan on
def fan_on():
    GPIO.output(FAN_PIN_IN1, GPIO.HIGH)
    GPIO.output(FAN_PIN_IN2, GPIO.LOW)
    
# Function to turn the fan off
def fan_off():
    GPIO.output(FAN_PIN_IN1, GPIO.LOW)
    GPIO.output(FAN_PIN_IN2, GPIO.LOW)
    
# Function to clean up the GPIO pins
def cleanup():
    GPIO.cleanup()
    
def subscribe(client: mqtt_client):
    def on_message(client, userdata, msg):
        print(f"Received `{msg.payload.decode()}` from `{msg.topic}` topic")
        if msg.payload.decode() == "on":
            fan_on()
        elif msg.payload.decode() == "off":
            fan_off()
        else:
            print("Invalid command")
    client.subscribe(MQTT_TOPIC)
    client.on_message = on_message
    client.loop_forever()
    
def connect_mqtt():
    def on_connect(client, userdata, flags, rc):
        if rc == 0:
            print("Connected to MQTT Broker!")
        else:
            print("Failed to connect, return code %d\n", rc)
    
    client = mqtt_client.Client(MQTT_CLIENT_ID)
    client.username_pw_set(MQTT_USERNAME, MQTT_PASSWORD)
    client.on_connect = on_connect
    client.connect(MQTT_IP, MQTT_PORT)
    return client

# Main function
def start_subscriber():
    # Define the callback function
    try: 
        client = connect_mqtt()
        subscribe(client)
    except KeyboardInterrupt:
        print("Keyboard interrupt detected. Exiting...")
    except Exception as e:
        print(f"An error occurred: {e}")
    finally:
        cleanup()
        client.loop_stop()
        client.disconnect()
        print("Exiting...")
    
def main():
    start_subscriber()
        
if __name__ == "__main__":
    main()
    
# End of File