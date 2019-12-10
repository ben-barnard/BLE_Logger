/*
   Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleScan.cpp
   Ported to Arduino ESP32 by Evandro Copercini
*/
int ledPin = 16;

//Based on the explanation by user Majenko here:
//https://arduino.stackexchange.com/questions/58677/get-esp32-chip-id-into-a-string-variable-arduino-c-newbie-here
char* ssid_retrieve(){
  uint64_t chipid = ESP.getEfuseMac(); // The chip ID is essentially its MAC address(length: 6 bytes).
  uint16_t chip = (uint16_t)(chipid >> 32);
  static char ssid[23];
  snprintf(ssid, 23, "%04X%08X", chip, (uint32_t)chipid);
  return ssid;
  }

//Used the example from here:
//http://acoptex.com/project/1285/basics-project-072y-esp32-development-board-with-sd-card-module-at-acoptexcom/
//**********************************************
#include <mySD.h>//include library code
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

//Bluetooth Stuff
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

//Beacon List
const int NUMBER_OF_ELEMENTS = 9;
const int MAX_SIZE = 18;
char beacons [NUMBER_OF_ELEMENTS] [MAX_SIZE] = {
 { "d0:fa:94:da:23:64" }, //Beacon #1
 { "d2:67:5e:27:cf:0b" }, //Beacon #2
 { "cc:8c:56:f5:68:29" }, //Beacon #3
 { "ef:e2:d7:a1:5d:ca" }, //Beacon #4
 { "e8:ff:31:f2:fc:19" }, //Beacon #5
 { "d9:00:a5:f0:e4:7d" }, //Beacon #6
 { "e4:fd:3c:56:87:99" }, //Beacon #7
 { "c5:20:67:1c:21:b7" }, //Beacon #8
 { "eb:ce:9a:73:50:a0" }, //Beacon #9
 };

bool mac_Check(const char test_mac[]) {
  for (int i = 0; i < NUMBER_OF_ELEMENTS; i++) {//Checking to see if the Bluetooth Device is one of ours
        if (strcmp(test_mac, beacons[i]) == 0){return true;} //Match! 
        }
  return false; //No match. 
}

int scanTime = 5; //In seconds
BLEScan* pBLEScan;

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      //Serial.printf("Advertised Device: %s \n", advertisedDevice.getAddress().toString().c_str());
      if(mac_Check(advertisedDevice.getAddress().toString().c_str())) //this matches our list of addresses, let's log it
      {
      //if (!SD.begin(CS, MOSI, MISO, SCK)) {
      //        Serial.println("initialization failed!");
      //        while(1);
      //        }
      SD.begin(CS, MOSI, MISO, SCK);
      //root = SD.open("/");
      root = SD.open("logging.csv", FILE_WRITE);
      //strcat(ssid_retrieve(),".csv")
      //char filename = strcat(ssid_retrieve(),".csv")
      //if (!SD.exists(filename)){}
      //root = SD.open(filename), FILE_WRITE);
      Serial.print(advertisedDevice.getAddress().toString().c_str());
      Serial.println(" matches our list of addresses, logging it");
      //Serial.println(ssid_retrieve());
      root.print(ssid_retrieve());
      root.print(", ");
      root.print(advertisedDevice.getAddress().toString().c_str());
      root.print(", "); 
      root.print(advertisedDevice.getRSSI());
      root.print(", "); 
      DateTime now = RTC.now();
      //root.print(now.year(), DEC);
      //root.print('/');
      root.print(now.month(), DEC);
      root.print('/');
      root.print(now.day(), DEC);
      root.print('/');
      root.print(now.year(), DEC);
      root.print(' ');
      root.print(now.hour(), DEC);
      root.print(':');
      if(now.minute() < 10){
        root.print("0");
        }
      root.print(now.minute(), DEC);
      root.print(':');
      if(now.second() < 10){
        root.print("0");
        }
      root.print(now.second(), DEC);
      root.println("");
      root.flush();
     /* close the file */
      root.close();
      digitalWrite(ledPin, HIGH);
      }
    }
};



void setup() {
  pinMode(ledPin, OUTPUT);
  Wire.begin();
  Serial.begin(115200);
  Serial.println("Scanning...");
  //Serial.println(strcat(ssid_retrieve(),".csv");
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);  // less or equal setInterval value
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  //Serial.print("Devices found: ");
  //Serial.println(foundDevices.getCount());
  //Serial.println("Scan done!");
  pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory
  Serial.println("Restarting in 15 seconds!");
  delay(15000);
  digitalWrite(ledPin, LOW);
  ESP.restart();
}

void loop() {
  // put your main code here, to run repeatedly:

}
