/* Set ESP32 as a server that sends a notify message every 2 seconds 
when a client is connected */
#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>


#include <BLE2902.h> // Client Characteristic Configuration Descriptor

BLECharacteristic *pCharacteristic; // create a pointer to BLECharacteristic
bool deviceConnected = false;
uint8_t value = 0;

// set UUID for new service and characteristic; generate UUID
#define SERVICE_UUID "b67dc645-3a7e-4c29-81ec-8b5c8262c0db"
#define CHARACTERISTIC_UUID "25fa8fc2-afda-4edc-a487-bb60db7d4ee8"

// initialize two callback functions for the server
class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer){
    deviceConnected = true;
  } // removed a closing semicolon *****

  void onDisconnect(BLEServer* pServer){
    deviceConnected = false;
  }
};

void setup() {
  Serial.begin(9600);

  // 1. create the BLE device
  BLEDevice::init("DOIT-ESP32");
  
  // 2. Create a BLE Server, set the device to be a server
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks);

  // 3. Create a BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // 4. Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ |
    BLECharacteristic::PROPERTY_NOTIFY |
    BLECharacteristic::PROPERTY_WRITE |
    BLECharacteristic::PROPERTY_INDICATE
  );

  // 5. Create a BLE Descriptor
  pCharacteristic->addDescriptor(new BLE2902());

  // 6. Start the Service
  pService->start();

  // 7. Start advertising
  pServer->getAdvertising()->start();
  Serial.println("Waiting for a client connection to notify ... ");
}

void loop() {
  // put your main code here, to run repeatedly:
  if (deviceConnected){
    Serial.printf("NOTIFY: %d \n", value);
    pCharacteristic->setValue(&value, 1);
    pCharacteristic->indicate();
    value++;
  }
  delay(2000);
}