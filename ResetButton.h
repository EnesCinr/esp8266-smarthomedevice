#include <Arduino.h>
#include <AceButton.h>
#include <services/WifiService.h>

using namespace ace_button;

WifiService wifiService;

ButtonConfig config;
AceButton button(&config);

void handleEvent(AceButton *, uint8_t eventType, uint8_t)
{
  switch (eventType)
  {
  case AceButton::kEventLongPressed: //UZUN BASMA FONKSİYONU
    Serial.println("pair");
    wifiService.pair();
    break;
  }
}

void configureResetButton()
{
  button.init(RSTBUTTON_PIN);
  config.setEventHandler(handleEvent);
  config.setFeature(ButtonConfig::kFeatureClick);
  config.setFeature(ButtonConfig::kFeatureLongPress);
  //config.setClickDelay(200);
  config.setLongPressDelay(4000);
}