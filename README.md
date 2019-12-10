# BLE_Logger
An Offline BLE Beacon Proximity Logger

Project developed to detect and log whether a wearable BLE beacon comes within range, for the purposes of research. Emphasis is on reliability, versatility, and being able to use inexpensive components. The code borrows heavily from various projects and stackoverflow posts. I am working on giving proper credit. 

Yes - I know - it would be much easier to use WiFi/LoRa/ESP-NOW and MQTT, but this application is intended for situations where you have mains power but no wireless options (beyond Bluetooth, of course). Not everything needs to be connected to the internet, especially if you're simply researching and don't need live data.

## Install ESP32 for Arduino
There's a <a href="https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/" target="_blank">good tutorial</a> to do this by randomnerdtutorials. If you follow my BOM, you will want to select the model "DOIT ESP32 DEVKIT V1".

## Required Libraries
1. <a href="https://github.com/nhatuan84/esp32-micro-sdcard" target="_blank">esp32_nanosdcard</a> - You could probably use the standard library, but I like this one.
2. <a href="https://github.com/adafruit/RTClib" target="_blank">RTClib</a> - You'll also need to set the time on your RTC module.
Note: The Bluetooth libraries should already be installed by default if you followed the instructions on installing ESP32 for Arduino.

## Bill of Materials (Shop around, the links are just for reference)
1. <a href="https://www.amazon.com/HiLetgo-ESP-WROOM-32-Development-Microcontroller-Integrated/dp/B0718T232Z" target="_blank">ESP32 Devkit V1</a> - $6
2. <a href="https://www.amazon.com/SenMod-Adapter-Reader-Module-Arduino/dp/B01JYNEX56/ref=sr_1_5?crid=2K6RLKFNAKBL2" target="_blank">MicroSD Card Adapter</a> - $1
3. <a href="https://www.amazon.com/HiLetgo-AT24C32-Arduino-Without-Battery/dp/B00LX3V7F0/ref=sr_1_3" target="_blank">RTC Module (ZS-042)</a> - $1
4. <a href="https://www.amazon.com/Energizer-2032-Battery-CR2032-Lithium/dp/B0042A9UXC/" target="_blank">CR2032 Coin-cell batteries</a> $.50
5. Bluetooth Beacons of your choice, I used <a href="https://www.aliexpress.com/item/32863939944.html?spm=a2g0s.9042311.0.0.27424c4ddpJpHT" target="_blank">these devices</a> $4/each
6. If you'd like to have an external LED indicator, grab a resistor and LED from anywhere. 
7. MicroSD cards. Don't worry about massive capacity. It would take a *long time* to fill a 16GB card with periodic readings. - $7 (if not on sale at Microcenter)

## Mapping

| ESP32 | DS331 |
|----------|----------|
| 3V3 | VCC |
| GND | GND |
| D22 | SCL |
| D21 | SDA |

| ESP32 | MicroSD |
|----------|----------|
| VIN | VCC |
| GND | GND |
| D5 | CS |
| D18 | SCK |
| D23 | MOSI |
| D19 | MISO |

LED is setup for Pin 16.

## Determining the MAC Addresses of your BLE Devices/Beacons
In the Arduino IDE, go to the scan sketch by: File -> Examples -> (Header) Examples for DOIT ESP32 DEVKIT V1 -> ESP 32 BLE Arduino -> BLE_scan . Compile and upload this sketch, and confirm that you are seeing successful scans in the serial screen. Now, go through your BLE devices, one at a time, and power them on, copy the MAC value, and save it somewhere. Shut that device off, and move onto the next one. You can enter these values in the sketch.

## Todo
1. The log file should be dynamic by default, preferably using the unique ssid_retrieve() value.
2. Set up a JSON configuration file to:
    - load the BLE MACs we're interested in
    - allow for a custom log file name
    - allow for a custom scan interval
3. Post completed gerber file for easy setup. 
4. Change the pin of the indicator LED to a PWM pin so that we can use multi-color LEDs
5. Consider code changes to reduce wear on MicroSD cards
