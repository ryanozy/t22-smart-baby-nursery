#include "BLEDevice.h"
#include <M5StickCPlus.h>

// change the BLE Server name to connect to
#define bleServerName "CSC2106-BLE#369"

/* UUID's of the service, characteristic that we want to read*/
// BLE Service
static BLEUUID bleServiceUUID("01234567-0123-4567-89ab-0123456789ab");

static BLEUUID processTimeCharacteristicUUID("01234567-0123-4567-89ab-0123456789cd");

// LED Characteristic
static BLEUUID ledCharacteristicUUID("01234567-0123-4567-89ab-0123456789ff");

// Flags stating if should begin connecting and if the connection is up
static boolean doConnect = false;
static boolean connected = false;

// Address of the peripheral device. Address will be found during scanning...
static BLEAddress *pServerAddress;

// Characteristicd that we want to read
static BLERemoteCharacteristic *processTimeCharacteristic;
static BLERemoteCharacteristic *voltageCharacteristic;
static BLERemoteCharacteristic *ledCharacteristic;

// Activate notify
const uint8_t notificationOn[] = {0x1, 0x0};
const uint8_t notificationOff[] = {0x0, 0x0};

// Variables to store process and voltage
char processStr[20];  
char ledStr[10]; 

boolean newProcess = false;
boolean newVoltage = false;
boolean newLed = false;

double messageSentTime = 0.0;
double messageReceivedTime = 0.0;

// Connect to the BLE Server that has the name, Service, and Characteristics
bool connectToServer(BLEAddress pAddress)
{
    BLEClient *pClient = BLEDevice::createClient();

    // Connect to the remove BLE Server.
    pClient->connect(pAddress);
    Serial.println(" - Connected to server");

    // Obtain a reference to the service we are after in the remote BLE server.
    BLERemoteService *pRemoteService = pClient->getService(bleServiceUUID);
    if (pRemoteService == nullptr)
    {
        Serial.print("Failed to find our service UUID: ");
        Serial.println(bleServiceUUID.toString().c_str());
        return (false);
    }

    // Obtain a reference to the characteristics in the service of the remote BLE server.
    processTimeCharacteristic = pRemoteService->getCharacteristic(processTimeCharacteristicUUID);
    ledCharacteristic = pRemoteService->getCharacteristic(ledCharacteristicUUID);

    if (processTimeCharacteristic == nullptr || ledCharacteristic == nullptr)
    {
        Serial.print("Failed to find our characteristic UUID");
        return false;
    }
    Serial.println(" - Found all characteristics");

    // Assign callback functions for the Characteristics
    processTimeCharacteristic->registerForNotify(processNotifyCallback);

    // Assign callback function for the LED characteristic
    if (ledCharacteristic->canWrite())
    {
        std::string value = ledCharacteristic->readValue();
        Serial.print("The characteristic value was: ");
        Serial.println(value.c_str());
    }

    if (ledCharacteristic->canNotify())
    {
        ledCharacteristic->registerForNotify(ledNotifyCallback);
    }

    return true;
}

// Callback function that gets called, when another device's advertisement has been received
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
    void onResult(BLEAdvertisedDevice advertisedDevice)
    {
        if (advertisedDevice.getName() == bleServerName)
        {                                                                   // Check if the name of the advertiser matches
            advertisedDevice.getScan()->stop();                             // Scan can be stopped, we found what we are looking for
            pServerAddress = new BLEAddress(advertisedDevice.getAddress()); // Address of advertiser is the one we need
            doConnect = true;                                               // Set indicator, stating that we are ready to connect
            Serial.println("Device found. Connecting!");
        }
        else
            Serial.print(".");
    }
};

// When the BLE Server sends a new process timing reading with the notify property
static void processNotifyCallback(BLERemoteCharacteristic *pBLERemoteCharacteristic,
                                      uint8_t *pData, size_t length, bool isNotify)
{
    // store process timing 
    strncpy(processStr, (char*)pData, length);
    processStr[length] = '\0';
    newProcess = true;
}

static void ledNotifyCallback(BLERemoteCharacteristic *pBLERemoteCharacteristic,
                              uint8_t *pData, size_t length, bool isNotify)
{
    // store LED status
    strncpy(ledStr, (char*)pData, length);
    ledStr[length] = '\0';
    newLed = true;
}

// function that prints the latest sensor readings in the OLED display
void printReadings()
{
    // Server Processing Time
    double receivedVal = atof(processStr);
    M5.Lcd.setCursor(0, 20, 2);
    M5.Lcd.print("Server Process Time = ");
    M5.Lcd.print(receivedVal);
    M5.Lcd.println("");

    // total time taken to receive the data
    double totalLatency = messageReceivedTime - messageSentTime;

    // Total Time taken to receive the data
    M5.Lcd.setCursor(0, 40, 2);
    M5.Lcd.print("Latency = ");
    M5.Lcd.print(totalLatency);
    M5.Lcd.println("");

    // Total 

    // display LED status
    M5.Lcd.setCursor(0, 60, 2);
    M5.Lcd.print("LED Status = ");
    M5.Lcd.print(ledStr);
    M5.Lcd.println(" ");
}

void setup()
{
    // Start serial communication
    Serial.begin(115200);
    Serial.println("Starting BLE Client application...");

    // put your setup code here, to run once:
    M5.begin();
    M5.Lcd.setRotation(3);
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(0, 0, 2);
    M5.Lcd.printf("BLE Client", 0);

    // Setup Button
    pinMode(M5_BUTTON_HOME, INPUT);
    delay(100);

    // Init BLE device
    BLEDevice::init("");

    // Retrieve a Scanner and set the callback we want to use to be informed when we
    // have detected a new device.  Specify that we want active scanning and start the
    // scan to run for 30 seconds.
    BLEScan *pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true);
    pBLEScan->start(30);
}

void loop()
{
    // If the flag "doConnect" is true then we have scanned for and found the desired
    // BLE Server that we want to connect.  Now we connect to it.  Once we are
    // connected we set the connected flag to be true.
    if (doConnect == true)
    {
        if (connectToServer(*pServerAddress))
        {
            Serial.println("Connected to the BLE Server.");

            // Activate the Notify property of each Characteristic
            processTimeCharacteristic->getDescriptor(BLEUUID((uint16_t)0x2902))->writeValue((uint8_t *)notificationOn, 2, true);
            ledCharacteristic->getDescriptor(BLEUUID((uint16_t)0x2904))->writeValue((uint8_t *)notificationOn, 2, true);
            connected = true;
        }
        else
        {
            Serial.println("Failed to connect to the server; Restart device to scan for nearby BLE server again.");
        }
        doConnect = false;
    }

    if (connected)
    {
        if (newProcess || newVoltage || newLed)
        {
            messageReceivedTime = millis();
            printReadings();
            newProcess = false;
            newLed = false;
        }

        if (digitalRead(M5_BUTTON_HOME) == LOW)
        {
            // String Reqest = "Read Data";
            // ledCharacteristic->writeValue(Reqest.c_str(), Reqest.length());

            messageSentTime = millis();
            String Request = "Led";
            ledCharacteristic->writeValue(Request.c_str(), Request.length());

            // Wait for button release
            while (digitalRead(M5_BUTTON_HOME) == LOW)
            ;
        }
    }


    delay(500); // Delay one second between loops.
}
