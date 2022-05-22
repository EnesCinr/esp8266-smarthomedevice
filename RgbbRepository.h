#include <Arduino.h>
#include <models/Rgbb.h>
#include <ArduinoJson.h>

class RgbbRepository
{
public:
    Rgbb fromJson(String data);
    Rgbb fromFirebaseJson(String data);

private:
    //const char *_red;
    // const char *_blue;
    // const char *_green;
    // const char *_brightness;
};

Rgbb RgbbRepository::fromJson(String data)
{
    StaticJsonDocument<20000> doc;
    deserializeJson(doc, data);
    Rgbb rgbb;

    const char *_red = doc["Red"];
    const char *_blue = doc["Blue"];
    const char *_green = doc["Green"];
    const char *_brightness = doc["Brightness"];

    // String Red = _red;
    // String Blue = _blue;
    // String Green = _green;
    // String Brightness = _brightness;

    rgbb.Red = _red;
    rgbb.Blue = _blue;
    rgbb.Green = _green;
    rgbb.Brightness = _brightness;

    return rgbb;
}

Rgbb RgbbRepository::fromFirebaseJson(String data)
{
    StaticJsonDocument<20000> doc;
    deserializeJson(doc, data);
    Rgbb rgbb2 = fromJson(doc["Rgbb"]);

    return rgbb2;
}