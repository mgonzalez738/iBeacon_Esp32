/* 
    Servidor BLE que envia periodicamente tramas iBeacon
    Basado en: https://circuitdigest.com/microcontroller-projects/esp32-based-bluetooth-ibeacon
*/

//////////// CONFIGURACION

#define BEACON_UUID "1c1abacc-17b7-4090-9330-5fa84076b4e5" // https://www.uuidgenerator.net/
#define BEACON_MAJOR 333
#define BEACON_MINOR 444
#define BEACON_SIGNAL_POWER 0     // Valor RSSI medido a 1 m
#define ADVERTISE_PERIOD 1        // segundos

///////////////////////////

#include "Arduino.h"
#include "BLEDevice.h"
#include "BLEBeacon.h"

#define ONBOARD_LED 2
BLEAdvertising *pAdvertising;
static uint64_t send_interval_ms;

void setBeacon() {

  BLEBeacon oBeacon = BLEBeacon();
  oBeacon.setManufacturerId(0x4C00); // Apple 0x004C LSB (ENDIAN_CHANGE_U16!)
  BLEUUID bleUUID = BLEUUID(BEACON_UUID) ;
  bleUUID = bleUUID.to128();
  oBeacon.setProximityUUID(BLEUUID( bleUUID.getNative()->uuid.uuid128, 16, true ));
  oBeacon.setMajor(BEACON_MAJOR);
  oBeacon.setMinor(BEACON_MINOR);
  oBeacon.setSignalPower(BEACON_SIGNAL_POWER);
  
  BLEAdvertisementData oAdvertisementData = BLEAdvertisementData();
  BLEAdvertisementData oScanResponseData = BLEAdvertisementData();
  oAdvertisementData.setFlags(0x04); // BR_EDR_NOT_SUPPORTED 0x04
    
  std::string strServiceData = "";
  
  strServiceData += (char)26;     // Len
  strServiceData += (char)0xFF;   // Type
  strServiceData += oBeacon.getData(); 
  oAdvertisementData.addData(strServiceData);
  
  pAdvertising->setAdvertisementData(oAdvertisementData);
  pAdvertising->setScanResponseData(oScanResponseData);

}

void setup() {

  Serial.begin(115200);
  pinMode(ONBOARD_LED, OUTPUT);
  
  // Create the BLE Device
  BLEDevice::init("");

  pAdvertising = BLEDevice::getAdvertising();
  setBeacon();

  send_interval_ms = millis();
}

void loop() {
  
  if ( (int)(millis() - send_interval_ms) >= (ADVERTISE_PERIOD * 1000))
  {
    send_interval_ms = millis();

    pAdvertising->start();
    digitalWrite(ONBOARD_LED, HIGH);
    Serial.println("Advertizing...");
    delay(50);

    pAdvertising->stop();
    digitalWrite(ONBOARD_LED, LOW);
  }
}