#include "Particle.h"

// This example does not require the cloud so you can run it in manual mode or
// normal cloud-connected mode
//SYSTEM_MODE(MANUAL);

//Beacon List
const int NUMBER_OF_ELEMENTS = 9;
const int MAX_SIZE = 18;
char beacons [NUMBER_OF_ELEMENTS] [MAX_SIZE] = {
  { "D0:FA:94:DA:23:64" }, //BEACON #1
  { "D2:67:5E:27:CF:0B" }, //BEACON #2
  { "CC:8C:56:F5:68:29" }, //BEACON #3
  { "EF:E2:D7:A1:5D:CA" }, //BEACON #4
  { "E8:FF:31:F2:FC:19" }, //BEACON #5
  { "D9:00:A5:F0:E4:7D" }, //BEACON #6 (OUT OF COMMISSION, USED FOR TESTING)
  { "E4:FD:3C:56:87:99" }, //BEACON #7
  { "C5:20:67:1C:21:B7" }, //BEACON #8
  { "EB:CE:9A:73:50:A0" }, //BEACON #9
};

bool mac_Check(const char test_mac[]) {
  for (int i = 0; i < NUMBER_OF_ELEMENTS; i++) {//Checking to see if the Bluetooth Device is one of ours
    if (strcmp(test_mac, beacons[i]) == 0) {
      return true; //Match!
    }
  }
  return false; //No match.
}

const size_t SCAN_RESULT_MAX = 30;

BleScanResult scanResults[SCAN_RESULT_MAX];

void setup()
{
    //selectExternalMeshAntenna();
    Serial.begin();
    //while (!Serial.available()) delay(10);
    Serial.println("finished setup");
}

void loop()
{
    // Only scan for 500 milliseconds
    //Serial.println("starting loop");
    BLE.setScanTimeout(50);
    int count = BLE.scan(scanResults, SCAN_RESULT_MAX);
    //Serial.println(count);
    for (int ii = 0; ii < count; ii++)
    {
        
        uint8_t buf[BLE_MAX_ADV_DATA_LEN];
        size_t len;
        len = scanResults[ii].advertisingData.get(BleAdvertisingDataType::MANUFACTURER_SPECIFIC_DATA, buf, BLE_MAX_ADV_DATA_LEN);
        // comment above and uncomment below if using pointer for reference (i.e. callback overload)
        // len = scanResult->scanResponse.get(BleAdvertisingDataType::MANUFACTURER_SPECIFIC_DATA, buf, BLE_MAX_ADV_DATA_LEN);
        if (len == 0) {
            len = scanResults[ii].scanResponse.get(BleAdvertisingDataType::MANUFACTURER_SPECIFIC_DATA, buf, BLE_MAX_ADV_DATA_LEN);
            // comment above and uncomment below if using pointer for reference (i.e. callback overload)
            // len = scanResult->scanResponse.get(BleAdvertisingDataType::MANUFACTURER_SPECIFIC_DATA, buf, BLE_MAX_ADV_DATA_LEN);
        }
        
        
        
        //uint8_t buf[BLE_MAX_ADV_DATA_LEN];
        //size_t len;

        //len = scanResults[ii].advertisingData.get(BleAdvertisingDataType::MANUFACTURER_SPECIFIC_DATA, buf, BLE_MAX_ADV_DATA_LEN);
        // We have manufacturer-specific advertising data (0xff) and it's 7 bytes (without the AD type)

        // Byte: BLE_SIG_AD_TYPE_MANUFACTURER_SPECIFIC_DATA 
        // 16-bit: Company ID
        // Byte: Internal packet identifier

        //Need a char string [18]
        char BLE_MA_Address[18] = "";
        //For 5 to 0
            //For 
        // = scanResults[ii].address[5] + ':' + scanResults[ii].address[4] + ':' + scanResults[ii].address[3] + ':' +
         //   scanResults[ii].address[2] + ':' + scanResults[ii].address[1] + ':' + scanResults[ii].address[0];
        //if (mac_Check(device.getAddress().toString().c_str())) //this matches our list of addresses, let's log it
        //{
            
            for (int i = 5; i > -1; i--) {
                //char* segment[3];
                //char* data_to_send = (char*) buf;
                //buf[len] = '\0';
                //ble.println((char *)buf);
                //segment = (char*) scanResults[ii].address[i];
                char* segment = (char*) scanResults[ii].address[i];
                strcat(BLE_MA_Address, segment);
                
                if(i > 1){strcat(BLE_MA_Address,":");}
                else{strcat(BLE_MA_Address,"/0");};
                //strcat(combined, one);
            }
            //Particle.publish("buttonPush", BLE_MA_Address,60,PRIVATE);
            Serial.printf("index: %2i RSSI: %d len: %i MANUF: %02X:%02X  %02X:%02X  MAC: %02X:%02X:%02X:%02X:%02X:%02X ",
            ii, scanResults[ii].rssi, len, buf[1], buf[0], buf[2], buf[3],
            //It's interesting, the same code has the MAC segments in backwards order.
            //scanResults[ii].address[0], scanResults[ii].address[1], scanResults[ii].address[2],
            //scanResults[ii].address[3], scanResults[ii].address[4], scanResults[ii].address[5]);
            scanResults[ii].address[5], scanResults[ii].address[4], scanResults[ii].address[3],
            scanResults[ii].address[2], scanResults[ii].address[1], scanResults[ii].address[0]);
            String name = scanResults[ii].advertisingData.deviceName();
            if (name.length() > 0) {Serial.printlnf("Name: %s", name.c_str());}
            else {Serial.println();}
        //}
    }
    delay(4000);
}
