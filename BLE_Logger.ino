/*
   Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleScan.cpp
   Ported to Arduino ESP32 by Evandro Copercini
*/

const int ledPin = 2;

#include <ArduinoJson.h>

//**********************************************
#include <mySD.h>//SD Card library code
//**********************************************
//VSPI pins
#define MOSI 23
#define MISO 19
#define SCK 18
#define CS 5
File root;
//**********************************************

//DS3231 RTC Stuff
#include <Wire.h>
#include "DS3231.h"

RTClib RTC;

char* ssid_retrieve() {
  uint64_t chipid = ESP.getEfuseMac(); // The chip ID is essentially its MAC address(length: 6 bytes).
  uint16_t chip = (uint16_t)(chipid >> 32);
  static char ssid[23];
  snprintf(ssid, 23, "%04X%08X", chip, (uint32_t)chipid);
  return ssid;
}

//Beacon List
const int NUMBER_OF_ELEMENTS = 9;
const int MAX_SIZE = 18;
char beacons [NUMBER_OF_ELEMENTS] [MAX_SIZE] = {
  { "d0:fa:94:da:23:64" }, //Beacon #1
  { "d2:67:5e:27:cf:0b" }, //Beacon #2
  { "cc:8c:56:f5:68:29" }, //Beacon #3
  { "ef:e2:d7:a1:5d:ca" }, //Beacon #4
  { "e8:ff:31:f2:fc:19" }, //Beacon #5
  { "d9:00:a5:f0:e4:7d" }, //Beacon #6 (out of commission, damaged)
  { "e4:fd:3c:56:87:99" }, //Beacon #7
  { "c5:20:67:1c:21:b7" }, //Beacon #8
  { "eb:ce:9a:73:50:a0" }, //Beacon #9
};

bool mac_Check(const char test_mac[]) {
  for (int i = 0; i < NUMBER_OF_ELEMENTS; i++) {//Checking to see if the Bluetooth Device is one of ours
    if (strcmp(test_mac, beacons[i]) == 0) {
      return true; //Match!
    }
  }
  return false; //No match.
}

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

int scanTime = 10; //In seconds
int time_between_scans = 60; //In seconds
BLEScan* pBLEScan;

/*
  class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      Serial.printf("Advertised Device: %s \n", advertisedDevice.toString().c_str());
    }
  };
*/

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  Wire.begin();
  DateTime now = RTC.now();

  //char fileName[] = "380EC6C40A24.csv";  //380EC6C40A24.csv
  //fileName = String(ssid_retrieve()) + ".csv";
  //sprintf(fileName, "%s.csv", ssid_retrieve());
  //Serial.println(fileName);
  //fileName[17] = '\0';

  SD.begin(CS, MOSI, MISO, SCK);
  root = SD.open("/");

  //creating the custom log name, based on first four chars of hardware id
  char fileName[9];
  for (uint32_t i = 0; i < 4; i++)
  {
    fileName[i] = ssid_retrieve()[i];
  }
  fileName[4] = '\0';
  strcat( fileName, ".csv" );

  root = SD.open("/");
  Serial.print("Logging data to ");
  Serial.print(fileName);
  if (SD.exists(fileName)) {
    Serial.println(" (already exists).");
  }
  else {
    Serial.println(" (doesn't exist yet, creating).");
  }
  root = SD.open(fileName, FILE_WRITE);
  Serial.print("Scanning for ");
  Serial.print(scanTime);
  Serial.println(" seconds.");

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  //pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);  // less or equal setInterval value
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);

  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print('/');
  Serial.println(now.year(), DEC);
  //Serial.println(ssid_retrieve());
  int deviceCount = foundDevices.getCount();
  int beaconCount = 0;

  for (uint32_t i = 0; i < deviceCount; i++)
  {
    BLEAdvertisedDevice device = foundDevices.getDevice(i);
    if (mac_Check(device.getAddress().toString().c_str())) //this matches our list of addresses, let's log it
    {

      //digitalWrite(ledPin, HIGH);

      beaconCount++;
      //Serial.print(device.getAddress().toString().c_str());
      //Serial.println(" matches our list of addresses, logging it");
      
      //char beacon_name[9];
      //beacon_name = device.getName();
      //strcpy(beacon_name, device.getName());
      const size_t capacity = JSON_OBJECT_SIZE(5);
      DynamicJsonDocument doc(capacity);
      
      
      doc["sensor"] = ssid_retrieve();
      doc["beacon_name"] = device.getName().c_str();
      //doc["beacon_uuid"] = device.getAddress().toString().c_str();
      doc["rssi"] = device.getRSSI();
      doc["time_seen"] = now.unixtime();
      serializeJson(doc, Serial);
      Serial.println("");
      
      root.print(ssid_retrieve());
      root.print(", ");
      root.print(device.getName().c_str());
      root.print(", ");
      root.print(device.getAddress().toString().c_str());
      root.print(", ");
      root.print(device.getRSSI());
      root.print(", ");

      root.print(now.month(), DEC);
      root.print('/');
      root.print(now.day(), DEC);
      root.print('/');
      root.print(now.year(), DEC);
      root.print(' ');
      root.print(now.hour(), DEC);
      root.print(':');
      if (now.minute() < 10) {
        root.print("0");
      }
      root.print(now.minute(), DEC);
      root.print(':');
      if (now.second() < 10) {
        root.print("0");
      }
      root.print(now.second(), DEC);
      root.println("");
    }
  }
  // test area

  root.flush();
  /* close the file */
  delay(100);
  root.close();
  delay(100);


  pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory
  Serial.print(beaconCount);
  Serial.println(" beacons seen during last scan.");
  Serial.print("Waiting ");
  Serial.print(time_between_scans);
  Serial.println(" seconds for restart and next scan.");
  digitalWrite(ledPin, LOW);
  delay(500);
  for (uint32_t i = 0; i < beaconCount; i++)
  {
    digitalWrite(ledPin, HIGH);
    delay(500);
    digitalWrite(ledPin, LOW);
    delay(500);
  }
  delay(abs((time_between_scans * 1000) - (beaconCount * 1000)));
  digitalWrite(ledPin, LOW);

  ESP.restart();

}

void loop() {
  // put your main code here, to run repeatedly:
  //BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  //Serial.print("Devices found: ");
  //Serial.println(foundDevices.getCount());
  //Serial.println("Scan done!");


}
