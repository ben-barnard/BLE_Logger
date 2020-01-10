# BLE_Logger
An Offline BLE Beacon Proximity Logger

Project developed to detect and log whether a wearable BLE beacon comes within range (relative, not absolute positioning), for research purposes. Emphasis is on reliability, versatility, and being able to use inexpensive components. The code borrows heavily from various projects and stackoverflow posts (links in sketch file).   

The idea is that you build some of these devices (one for each area you wish to track), power them on, and they just continually scan for the BLE devices you specify in the sketch. When the scanner sees a beacon that it's been told to look for, it creates a new line of the log file with that detail (which scanner, which beacon, what time). It's mostly trivial to then combine these data and come up with a story on what beacon was where and when. 

Yes - I know - it would be **much** easier to use WiFi/LoRa/ESP-NOW and MQTT, but this application is intended for situations where you have mains power but no wireless options (beyond Bluetooth, of course). Not everything needs to be connected to the internet, especially if you're simply researching and don't need live data.

## Install ESP32 for Arduino
There's a <a href="https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/" target="_blank">good tutorial</a> to do this by randomnerdtutorials. If you follow my BOM, you will want to select the model "DOIT ESP32 DEVKIT V1".

## Required Libraries
1. <a href="https://github.com/nhatuan84/esp32-micro-sdcard" target="_blank">esp32_nanosdcard</a> - You could probably use the standard library, but I like this one.
2. <a href="https://github.com/adafruit/RTClib" target="_blank">RTClib</a> - You'll also need to set the time on your RTC module.
3. <a href="https://arduinojson.org/" target="_blank">ArduinoJson</a> 

Note: The Bluetooth libraries should already be installed by default if you followed the instructions on installing ESP32 for Arduino.

## Bill of Materials (Shop around, the links are just for reference)
1. <a href="https://www.amazon.com/HiLetgo-ESP-WROOM-32-Development-Microcontroller-Integrated/dp/B0718T232Z" target="_blank">ESP32 Devkit V1</a> - $6
2. <a href="https://www.amazon.com/SenMod-Adapter-Reader-Module-Arduino/dp/B01JYNEX56/ref=sr_1_5?crid=2K6RLKFNAKBL2" target="_blank">MicroSD Card Adapter</a> - $1
3. <a href="https://www.amazon.com/HiLetgo-AT24C32-Arduino-Without-Battery/dp/B00LX3V7F0/ref=sr_1_3" target="_blank">RTC Module (ZS-042)</a> - $1
4. <a href="https://www.amazon.com/Energizer-2032-Battery-CR2032-Lithium/dp/B0042A9UXC/" target="_blank">CR2032 Coin-cell batteries</a> $.50
5. Bluetooth Beacons of your choice, I used <a href="https://www.aliexpress.com/item/32863939944.html?spm=a2g0s.9042311.0.0.27424c4ddpJpHT" target="_blank">these devices</a> Important note - there appears to be a variant where the broadcasted RSSI is actually the battery level and *not the signal strength*. Don't get those. Signal strength is the value we need. $4/each
6. If you'd like to have an external LED indicator, grab a resistor and LED from anywhere. 
7. MicroSD cards. Don't worry about massive capacity. It would take a *long time* to fill a 16GB card with periodic readings. - $7 (if not on sale at Microcenter)
8. A micro usb cable or microusb phone charger. The ESP32 dev board has a microusb plugin, so we'll just use that.
9. An enclosure. You can easily <a href="https://www.thingiverse.com/thing:1264391" target="_blank">3D print one</a> or order some cheaply on Amazon. <a href="https://www.amazon.com/gp/product/B07WCKF6P4/ref=ppx_yo_dt_b_asin_title_o08_s00?ie=UTF8&psc=1" target="_blank">These fit well</a> even if you are connecting everything with jumper wires. If you do connect everything with jumpers and cram it all into a box, be aware that you may end up inadvertently shorting the DS3231 and wiping the time. Protect the DS3231.
10. A way for to connect everything. If you want to keep things super simple, just using <a href="https://www.amazon.com/gp/product/B077N58HFK/ref=ppx_yo_dt_b_asin_title_o01_s00?ie=UTF8&psc=1" target="_blank">female jumper wires</a> is fine. $5

## Mapping

| ESP32 | DS3231 |
|----------|----------|
| 3V3 | VCC |
| GND | GND |
| D22 | SCL |
| D21 | SDA |

| ESP32 | MicroSD Module |
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
1. ~~The log file should be dynamic by default, preferably using the unique ssid_retrieve() value.~~ done
2. Set up a JSON configuration file (saved to MicroSD) to:
    - load the BLE MACs we're interested in (**high priority, this is a pain**)
    - allow for a custom log file name
    - allow for a custom scan interval
3. Post completed gerber file for easy setup. *Update* - I  did print PCBs for this project but they are ugly and have mistakes, so I will wait until version 2 before posting it. 
4. Change the pin of the indicator LED to a PWM pin so that we can use multi-color LEDs
5. ~~Consider code changes to reduce wear on MicroSD cards~~ done - the current version loads all currently seen beacons to a buffer to write once, not the old version where there was a separate write for each. 
6. Add the instructions for the wearable beacons - they need their signal strength to be consistent, and we can't be sure that is the case upon arrival. Also, changing their name is useful, so you see "BEACON1" in your scanner as opposed to "RDL5XXXX".
7. Only write the durations of the beacons being in proximity, not the minute-by-minute "I'm here!"
8. Need to add instructions on how to use an ESP32 or ESP8266 to put the correct time on the DS3231 module (TimeNTP_ESP8266WiFi.ino). 
9. Get rid of *constant* ESP reboot - retain the details of what the current conditions are in memory OR save them to JSON and store on the file. The important time variables are "first_seen" and "last_seen". 
10. Set a DS3231 alarm for 2AM (configurable, obviously) where we do have one daily reboot.
11. Try to use the ESP32 dev boards in the D1 Mini footprint instead - there is a D1 mini data logger board that we could use and make these devices incredibly small with no PCB needed!
