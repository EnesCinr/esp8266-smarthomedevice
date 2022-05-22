#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include "coredecls.h"

extern "C"{
#include "homeintegration.h"
}
#include "homekitintegrationcpp.h"
#include <hapfilestorage/hapfilestorage.hpp>


struct device_data_t{
  bool IsOn=false;
  uint8_t Brightness=50;
  float Hue=180.0;
  float Saturation=0.0;
};

device_data_t DeviceData;

homekit_service_t* hapservice=NULL;
homekit_service_t* hapservice_motion=NULL;
homekit_characteristic_t * hap_on=NULL;
homekit_characteristic_t * hap_br=NULL;
homekit_characteristic_t * hap_hue=NULL;
homekit_characteristic_t * hap_saturation=NULL;

String pair_file_name="/pair.dat";

int rgb[3];

void HSV2RGB(float h, float s, float v) {

  int i;
  float m, n, f;

  s /= 100;
  v /= 100;

  if (s == 0) {
    rgb[0] = rgb[1] = rgb[2] = round(v * 255);
    return;
  }

  h /= 60;
  i = floor(h);
  f = h - i;

  if (!(i & 1)) {
    f = 1 - f;
  }

  m = v * (1 - s);
  n = v * (1 - s * f);

  switch (i) {

    case 0: case 6:
      rgb[0] = round(v * 255);
      rgb[1] = round(n * 255);
      rgb[2] = round(m * 255);
      break;

    case 1:
      rgb[0] = round(n * 255);
      rgb[1] = round(v * 255);
      rgb[2] = round(m * 255);
      break;

    case 2:
      rgb[0] = round(m * 255);
      rgb[1] = round(v * 255);
      rgb[2] = round(n * 255);
      break;

    case 3:
      rgb[0] = round(m * 255);
      rgb[1] = round(n * 255);
      rgb[2] = round(v * 255);
      break;

    case 4:
      rgb[0] = round(n * 255);
      rgb[1] = round(m * 255);
      rgb[2] = round(v * 255);
      break;

    case 5:
      rgb[0] = round(v * 255);
      rgb[1] = round(m * 255);
      rgb[2] = round(n * 255);
      break;
  }
}
void set_strip(int rgb[3]){
    firebaseService.setItem("/devices/" + firebaseService.initDeviceId + "/Rgbb/Red", String(rgb[0]));
    firebaseService.setItem("/devices/" + firebaseService.initDeviceId + "/Rgbb/Green", String(rgb[1]));
    firebaseService.setItem("/devices/" + firebaseService.initDeviceId + "/Rgbb/Blue", String(rgb[2]));
}

void notifyHAP(){
  if(hap_on && hap_on->value.bool_value!=DeviceData.IsOn){
      HAP_NOTIFY_CHANGES(int,hap_on,DeviceData.IsOn, 0)
    }
    if(hap_br && hap_br->value.int_value !=DeviceData.Brightness){
      HAP_NOTIFY_CHANGES(int,hap_br,DeviceData.Brightness, 0)
    }
    if(hap_hue && hap_hue->value.float_value !=DeviceData.Hue){
      HAP_NOTIFY_CHANGES(int,hap_hue,DeviceData.Hue, 0)
    }
    if(hap_saturation && hap_saturation->value.float_value !=DeviceData.Saturation){
      HAP_NOTIFY_CHANGES(int,hap_saturation,DeviceData.Saturation, 0)
    }
}

void init_hap_storage(){
  //Serial.print("init_hap_storage");
  File fsDAT=SPIFFS.open(pair_file_name, "r");
  
 if(!fsDAT){
   //Serial.println("Failed to read pair.dat");
   SPIFFS.format(); 
 }
  int size=hap_get_storage_size_ex();
  char* buf=new char[size];
  memset(buf,0xff,size);
  if(fsDAT)
  fsDAT.readBytes(buf,size);

  hap_init_storage_ex(buf,size);
  if(fsDAT)
    fsDAT.close();
  delete []buf;
}

void storage_changed(char * szstorage,int bufsize){
  SPIFFS.remove(pair_file_name);
  File fsDAT=SPIFFS.open(pair_file_name, "w+");
  if(!fsDAT){
    //Serial.println("Failed to open pair.dat");
    return;
  }
  fsDAT.write((uint8_t*)szstorage, bufsize);
  fsDAT.close();
}

void led_callback(homekit_characteristic_t *ch, homekit_value_t value, void *context) {
    bool isSet=false;
    bool isOff=false;
    if(ch==hap_on && ch->value.bool_value!=DeviceData.IsOn){
        DeviceData.IsOn=ch->value.bool_value;
        if (DeviceData.IsOn == false){
          firebaseService.setBool("/devices/" + firebaseService.initDeviceId + "/Status", false);
          isOff=true;
        }
        else{
          firebaseService.setBool("/devices/" + firebaseService.initDeviceId + "/Status", false);
        }
        isSet=true;
    }
    if(ch==hap_br && ch->value.int_value!=DeviceData.Brightness && DeviceData.IsOn){
        DeviceData.Brightness=ch->value.int_value;
        isSet=true;
    }
    if(ch==hap_hue && ch->value.float_value!=DeviceData.Hue){
        DeviceData.Hue=ch->value.float_value;
        isSet=true;
    }
    if(ch==hap_saturation && ch->value.float_value!=DeviceData.Saturation){
        DeviceData.Saturation=ch->value.float_value;
        isSet=true;
    }
    if(isSet){
        if(isOff){
          HSV2RGB(DeviceData.Hue,DeviceData.Saturation,0);
        }else{
          HSV2RGB(DeviceData.Hue,DeviceData.Saturation,DeviceData.Brightness);
        }
        set_strip(rgb);
    }
}


void homekitInitialize(const char* device_name, const char* serial_number, char* pass, char* setupid)
{
  disable_extra4k_at_link_time();
  if (!SPIFFS.begin())
  {
    Serial.print("SPIFFS Mount failed");
  }

  Serial.print("Free heap: ");
  Serial.println(system_get_free_heap_size());

  init_hap_storage();
  
  set_callback_storage_change(storage_changed);

  hap_setbase_accessorytype(homekit_accessory_category_lightbulb);

  hap_initbase_accessory_service(device_name,"Povodu",serial_number,"RGB Controller","1.0");
    
  hapservice=hap_add_rgbstrip_service("RGB",led_callback,NULL);
  hap_on=homekit_service_characteristic_by_type(hapservice, HOMEKIT_CHARACTERISTIC_ON);;
  hap_br=homekit_service_characteristic_by_type(hapservice, HOMEKIT_CHARACTERISTIC_BRIGHTNESS);;
  hap_hue=homekit_service_characteristic_by_type(hapservice, HOMEKIT_CHARACTERISTIC_HUE);;
  hap_saturation=homekit_service_characteristic_by_type(hapservice, HOMEKIT_CHARACTERISTIC_SATURATION);

  hap_set_device_password(pass);

  hap_set_device_setupId(setupid);

  hap_init_homekit_server();

	INIT_CHARACHTERISTIC_VAL(bool,hap_on,true);
}

void homekitLoop()
{
  hap_homekit_loop();
}
