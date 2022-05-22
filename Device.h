#include <Arduino.h>
#include <repositories/RgbbRepository.h>

enum class DeviceType
{
    RgbController = 0,
    Relay = 1
};

class Device
{
public:
    String ConnectionStatus;
    String CreatedDate;
    String DeviceConnectionStatus;
    String DeviceMacAddress;
    String DeviceName;
    String DeviceType;
    String Id;
    bool Status;
    String SubType;
    String LocalIpAddress;
    Rgbb RGBB;
    String SerialNumber;
};
