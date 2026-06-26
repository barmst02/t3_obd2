/*
   Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleScan.cpp
   Ported to Arduino ESP32 by Evandro Copercini
*/
#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <OneButtonTiny.h>
#include <pin_config.h>
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

BLEScan *pBLEScan;

uint32_t flags = 0b10000000;  // Bits 2 and 4 are set

// Setup a new OneButton on pin PIN_BUTTON_1.
// OneButton button(PIN_BUTTON_1, true);
OneButtonTiny button1(PIN_BUTTON_1, true);  // This example also works with reduced OneButtonTiny class saving.
OneButtonTiny button2(PIN_BUTTON_2, true);  // This example also works with reduced OneButtonTiny class saving.


// Helper macros for printing binary
#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
    (byte & 0x80 ? '1' : '0'), \
    (byte & 0x40 ? '1' : '0'), \
    (byte & 0x20 ? '1' : '0'), \
    (byte & 0x10 ? '1' : '0'), \
    (byte & 0x08 ? '1' : '0'), \
    (byte & 0x04 ? '1' : '0'), \
    (byte & 0x02 ? '1' : '0'), \
    (byte & 0x01 ? '1' : '0')

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial.printf("Name: %s, Address: %s\n", 
                  advertisedDevice.getName().c_str(), 
                  advertisedDevice.getAddress().toString().c_str());
    if (advertisedDevice.haveName()) {
      tft.println(advertisedDevice.getName().c_str());
    }
  }
};

void RotateLeft() { // value gets larger
    // --- Rotate left through 8 positions ---
    uint32_t v = flags;
    uint32_t lsb = (v >> 7) & 1;   // Will lose the MSB, so move it to the LSB (1) position and store
    v = ((v << 1) & 0xFF) | lsb;   // Shift left and set the new LSB
    Serial.printf("Rotate Left: " BYTE_TO_BINARY_PATTERN "->" BYTE_TO_BINARY_PATTERN "\n", BYTE_TO_BINARY(flags), BYTE_TO_BINARY(v));
    flags = v;
    tft.println(flags);
  }

void RotateRight() { // value gets smaller  
    // --- Rotate right through 8 positions ---
    uint32_t v = flags;
    uint32_t lsb = v & 1; // Will lose the LSB, so store it's value 
    v = (v >> 1) | (lsb << 7);  // Shift right and set the new MSB
    Serial.printf("Rotate Right: " BYTE_TO_BINARY_PATTERN "->" BYTE_TO_BINARY_PATTERN "\n", BYTE_TO_BINARY(flags), BYTE_TO_BINARY(v));
    flags = v;
    tft.println(flags);
} 

void myClickFunction() {
  // put your main code here, to run repeatedly:
  //BLEScanResults *foundDevices = pBLEScan->start(scanTime, false);
  pBLEScan->start(5, false);
  //Serial.print("Devices found: ");
  //Serial.println(foundDevices->getCount());
  //Serial.println("Scan done!");
  pBLEScan->clearResults();  // delete results fromBLEScan buffer to release memory
}

void clearScreen() {
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 0);
}

// setup code here, to run once.
void setup() {
  Serial.begin(115200);
  Serial.println("Setting up...");

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();  //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);  //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);  // less or equal setInterval value

  // link the myClickFunction function to be called on a click event.
  button1.attachDoubleClick(myClickFunction);
  button1.attachClick(RotateLeft);

  button2.attachLongPressStart(clearScreen);
  button2.attachClick(RotateRight);

  // set 80 msec. debouncing time. Default is 50 msec.
  button1.setDebounceMs(80);
  button2.setDebounceMs(80);

  // This IO15 must be set to HIGH, otherwise nothing will be displayed when USB is not connected.
  pinMode(PIN_POWER_ON, OUTPUT);
  digitalWrite(PIN_POWER_ON, HIGH);

  Serial.begin(115200);

  tft.begin();
  tft.setRotation(3);
  tft.setTextSize(3);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);

  tft.println("TFT initialized.");
  // Turn on backlight
  pinMode(PIN_LCD_BL, OUTPUT);
  digitalWrite(PIN_LCD_BL, HIGH);

}

void loop()
{
    button1.tick();
    button2.tick();
}
