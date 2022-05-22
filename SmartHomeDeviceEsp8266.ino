#include <Arduino.h>
#include <components/ResetButton.h>
#include "HomekitService.h"

String DeviceID = "DILARA";
String MacAddress = "2C:F4:32:11:55:E9";
String SerialNumber = "11223344556677";
DeviceType deviceType = DeviceType::RgbController;

const char* DeviceName = "ENESRGB";
const char* HomekitSerialNumber = "12312345678456";
char* DevicePass = "12345678";
char* DeviceSetupID = "1234";

int RgbPin_Red = 4;
int RgbPin_Green = 5;
int RgbPin_Blue = 2;

void setup()
{
  Serial.begin(9600);
  configureResetButton();
  pinMode(LED_PIN, OUTPUT);

  wifiService.initialize(DeviceID, MacAddress, SerialNumber, deviceType);
  
  if (deviceType == DeviceType::RgbController)
    ledService.initialize(RgbPin_Red, RgbPin_Green, RgbPin_Blue);

  if (wifiService.checkWifiInfo())
    firebaseService.initialize(deviceType);
  else
  if (wifiService.checkWifiInfo()){
    homekitInitialize(DeviceName, HomekitSerialNumber, DevicePass, DeviceSetupID);
    firebaseService.initialize(deviceType);
  }
  else{
    Serial.println("not connected");
}
}

void loop()
{
  button.check();
  if (wifiService.checkConnected())
  {
    firebaseService.streamListener();
    if (deviceType == DeviceType::RgbController)
      firebaseService.streamRgbbListener();
    {
      firebaseService.streamRgbbListener();
      homekitLoop();
    }
  }
  else
  {
    BlinkLed(NOT_CONNECTED, RgbPin_Red, RgbPin_Green, RgbPin_Blue);
  }
}
