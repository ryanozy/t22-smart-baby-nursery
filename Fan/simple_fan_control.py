import RPi.GPIO as GPIO
import time 

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
        
def main():
    try:
        while True:
            fan_on()
            time.sleep(5)
            fan_off()
            time.sleep(5)
            
    except KeyboardInterrupt:
        cleanup()
        
        
if __name__ == "__main__":
    main()
    
# End of File