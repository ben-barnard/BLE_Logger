# BLE_Logger
An Offline BLE Beacon Proximity Logger

Project to detect and log whether a BLE beacon comes within range, for research purposes. 

Yes - I know - it would be much easier to use WiFi/LoRa/ESP-NOW and MQTT, but this application is intended for situations where you have mains power but no wireless options. The idea is to deploy a few of these, give out BLE beacons to the folks (or inventory items) you'd like to track, and then come back at the end of the collection period to analyze the logs.

## BOM (Shop around, the links are just for reference)
1. <a href="https://www.amazon.com/HiLetgo-ESP-WROOM-32-Development-Microcontroller-Integrated/dp/B0718T232Z" target="_blank">ESP32 Devkit V1</a>
2. <a href="https://www.amazon.com/SenMod-Adapter-Reader-Module-Arduino/dp/B01JYNEX56/ref=sr_1_5?crid=2K6RLKFNAKBL2" target="_blank">MicroSD Card Adapter</a>
3. <a href="https://www.amazon.com/HiLetgo-AT24C32-Arduino-Without-Battery/dp/B00LX3V7F0/ref=sr_1_3" target="_blank">RTC Module (ZS-042)</a>
4. <a href="https://www.amazon.com/Energizer-2032-Battery-CR2032-Lithium/dp/B0042A9UXC/" target="_blank">CR2032 Coin-cell batteries</a>
5. Bluetooth Beacons of your choice, I used <a href="https://www.aliexpress.com/item/32863939944.html?spm=a2g0s.9042311.0.0.27424c4ddpJpHT" target="_blank">these devices</a>
6. If you'd like to have an external LED indicator, grab a resistor and LED from anywhere.  
