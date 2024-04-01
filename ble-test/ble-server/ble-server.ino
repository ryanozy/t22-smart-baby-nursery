#include <BLEDevice.h>
#include <BLEServer.h>
#include <M5StickCPlus.h>

// Change to unique BLE server name
#define bleServerName "CSC2106-BLE#369"

double currentTime = 0.0;
float vBatt = 5.0; // initial value

int deviceConnected = 0;

#define SERVICE_UUID "01234567-0123-4567-89ab-0123456789ab"

// Temperature Characteristic and Descriptor
BLECharacteristic ProcessTimeCharacteristics("01234567-0123-4567-89ab-0123456789cd", BLECharacteristic::PROPERTY_NOTIFY);
BLEDescriptor ProcessTimeDescriptor(BLEUUID((uint16_t)0x2902));

// LED Characteristic and Descriptor
BLECharacteristic ledCharacteristics("01234567-0123-4567-89ab-0123456789ff", BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY);
BLEDescriptor ledDescriptor(BLEUUID((uint16_t)0x2904));

bool setStatus();

double processStartTime = 0.0;
double processEndTime = 0.0;

class MyServerCallbacks : public BLEServerCallbacks
{
    void onConnect(BLEServer *pServer)
    {
        deviceConnected++;
        Serial.println("MyServerCallbacks::Connected...");
        BLEDevice::startAdvertising();
    };
    void onDisconnect(BLEServer *pServer)
    {
        deviceConnected--;
        Serial.println("MyServerCallbacks::Disconnected...");
    }
};

bool setStatus()
{

    double processTime = processEndTime - processStartTime;
    // Convert Value to String
    static char processTimeStr[20];
    static char ledStr[10];

    dtostrf(processTime, 6, 2, processTimeStr);

    // Set IMU temperature
    ProcessTimeCharacteristics.setValue(processTimeStr);
    ProcessTimeCharacteristics.notify();

    if (digitalRead(M5_LED) == 0)
    {
        strncpy(ledStr, "ON ", sizeof(ledStr));
    }
    else
    {
        strncpy(ledStr, "OFF", sizeof(ledStr));
    }

    // Set LED status
    ledCharacteristics.setValue(ledStr);
    ledCharacteristics.notify();

    return true;
}

void printReadings()
{
    // Print to LCD
    // Temperature Celsius
    M5.Lcd.setCursor(0, 20, 2);
    M5.Lcd.print("Timestamp = ");
    M5.Lcd.print(currentTime);

    // display LED status
    M5.Lcd.setCursor(0, 40, 2);
    M5.Lcd.print("LED Status = ");

    if (digitalRead(M5_LED) == 0)
    {
        M5.Lcd.print("ON ");
        Serial.println("ON ");
    }
    else
    {
        M5.Lcd.print("OFF");
        Serial.println("OFF");
    }
}

void setup()
{
    // Start serial communication
    Serial.begin(115200);

    // Start M5StickC
    M5.begin();

    // Setup IMU
    M5.IMU.Init();

    // Set LCD display
    M5.Lcd.setRotation(3);
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(0, 0, 2);
    M5.Lcd.printf("BLE Server", 0);

    // Setup button
    pinMode(M5_BUTTON_HOME, INPUT);
    delay(100);

    // Setup LED
    pinMode(M5_LED, OUTPUT);
    digitalWrite(M5_LED, 1); // Turn off LED

    // Setup AXP192
    M5.Axp.begin();

    // Create BLE Server
    BLEDevice::init(bleServerName);

    // Create BLE Server
    BLEServer *pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    BLEService *bleService = pServer->createService(SERVICE_UUID);

    // Create BLE Characteristics
    bleService->addCharacteristic(&ProcessTimeCharacteristics);
    ProcessTimeDescriptor.setValue("IMU Temperature(C)");
    ProcessTimeCharacteristics.addDescriptor(&ProcessTimeDescriptor);

    // LED
    bleService->addCharacteristic(&ledCharacteristics);
    ledDescriptor.setValue("LED");
    ledCharacteristics.addDescriptor(&ledDescriptor);

    // Start the service
    bleService->start();

    // Start advertising
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pServer->getAdvertising()->start();
    Serial.println("Waiting a client connection to notify...");
}

void loop()
{
    if (deviceConnected >= 1)
    {
        std::string value = ledCharacteristics.getValue();
        processStartTime = millis();
        if (value[0] == 'R')
        {
            processEndTime = millis();
            setStatus();
            printReadings();
        }
        else if (value[0] == 'L')
        {
            digitalWrite(M5_LED, !digitalRead(M5_LED));
            processEndTime = millis();
            setStatus();
            printReadings();
        }
    }

    // Check button
    if (digitalRead(M5_BUTTON_HOME) == LOW)
    {
        // Toggle LED
        digitalWrite(M5_LED, !digitalRead(M5_LED));

        // Set all status to BLE
        setStatus();

        // Print readings
        printReadings();

        // Wait for button release
        while (digitalRead(M5_BUTTON_HOME) == LOW)
            ;
    }
}
