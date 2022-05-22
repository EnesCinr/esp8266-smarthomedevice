#include <Arduino.h>
#include <helpers/helper.h>
#include <EEPROM.h>

enum class MemoryValues
{
    SSID = 0,
    PASSWORD = 1,
    LAST_STATE = 2
};

class MemoryService
{
private:
    int getStartIndexWithMemoryValue(MemoryValues key);

public:
    bool writeString(MemoryValues key, String value);
    String readString(MemoryValues key);
    void resetMemory();
};

int MemoryService::getStartIndexWithMemoryValue(MemoryValues key)
{
    switch (key)
    {
    case MemoryValues::SSID:
        return 0;
        break;
    case MemoryValues::PASSWORD:
        return 33;
        break;
    case MemoryValues::LAST_STATE:
        return 65;
        break;
    default:
        return 0;
        break;
    }
}

bool MemoryService::writeString(MemoryValues key, String value)
{
    EEPROM.begin(2048);
    int startIndex = getStartIndexWithMemoryValue(key);
    //Serial.println(startIndex);

    // unsigned int i;
    // for (i = 0; i < 31; i++)
    // {
    //     if (value.length() > i)
    //         EEPROM.write(startIndex + i, value[i]);
    //     else
    //     {
    //         Serial.println("aaa");
    //         EEPROM.write(startIndex + i, 'c');
    //     }
    // }
    // //EEPROM.write(startIndex + i, '\0');
    byte len = value.length();
    EEPROM.write(startIndex, len);
    for (int i = 0; i < len; i++)
    {
        EEPROM.write(startIndex + 1 + i, value[i]);
    }

    return EEPROM.commit();
}

String MemoryService::readString(MemoryValues key)
{
    // Serial.println(startIndex);
    // char data[32];
    // char character;
    // for (int i = 0; i < 32; i++)
    // {
    //     character = EEPROM.read(startIndex + i);
    //     if (character == 'c')
    //     {
    //         Serial.println("aaa");
    //         break;
    //     }
    //     data[i] = character;
    // }
    EEPROM.begin(2048);
    int startIndex = getStartIndexWithMemoryValue(key);
    int len = EEPROM.read(startIndex);
    char data[len + 1];
    for (int i = 0; i < len; i++)
    {
        data[i] = EEPROM.read(startIndex + 1 + i);
    }
    data[len] = '\0';
    return String(data);
}

void MemoryService::resetMemory()
{
    EEPROM.begin(2048);
    for (int i = 0; i < 2048; i++)
    {
        EEPROM.write(i, 0);
    }
    EEPROM.end();
}