#include <Arduino.h>
#include <models/Device.h>

RgbbRepository rgbbRepository;

class DeviceRepository
{
public:
    Device fromJson(String data);
    Device getSavingRgbController(String macAddress, String ipAddress, String serialNumber);
    Device getSavingRelay(String macAddress, String ipAddress, String serialNumber);

private:
};

Device DeviceRepository::fromJson(String data)
{
    StaticJsonDocument<20000> doc;
    deserializeJson(doc, data);
    Device device4;
    const char *_connectionStatus = doc["ConnectionStatus"];
    const char *_createdDate = doc["CreatedDate"];
    const char *_deviceConnectionStatus = doc["DeviceConnectionStatus"];
    const char *_deviceMacAddress = doc["DeviceMacAddress"];
    const char *_deviceName = doc["DeviceName"];
    const char *_deviceType = doc["DeviceType"];
    const char *_id = doc["Id"];
    const char *_localIpAddress = doc["LocalIpAddress"];
    const char *_serialNumber = doc["SerialNumber"];
    const char *_status = doc["Status"];
    const char *_subType = doc["SubType"];

    device4.ConnectionStatus = _connectionStatus;
    device4.CreatedDate = _createdDate;
    device4.DeviceConnectionStatus = _deviceConnectionStatus;
    device4.DeviceMacAddress = _deviceMacAddress;
    device4.DeviceName = _deviceName;
    device4.DeviceType = _deviceType;
    device4.Id = _id;
    device4.RGBB = rgbbRepository.fromJson(doc["Rgbb"]);
    device4.LocalIpAddress = _localIpAddress;
    device4.SerialNumber = _serialNumber;
    device4.Status = _status;
    device4.SubType = _subType;

    return device4;
}

Device DeviceRepository::getSavingRgbController(String macAddress, String ipAddress, String serialNumber)
{
    Serial.println("get device başladı");
    Device rgbControllerDevice;
    Rgbb rgbb;

    rgbb.Blue = "255";
    rgbb.Brightness = "100";
    rgbb.Green = "255";
    rgbb.Red = "255";

    rgbControllerDevice.ConnectionStatus = "Online";
    rgbControllerDevice.CreatedDate = "2022-01-01T00:00:00.000000";
    rgbControllerDevice.DeviceConnectionStatus = "Connected";
    rgbControllerDevice.DeviceMacAddress = macAddress;
    rgbControllerDevice.DeviceName = "RGB Controller";
    rgbControllerDevice.DeviceType = "RgbController";
    //rgbControllerDevice.Id = "DenemeID";
    rgbControllerDevice.LocalIpAddress = ipAddress;
    rgbControllerDevice.SerialNumber = serialNumber;
    rgbControllerDevice.RGBB = rgbb;
    rgbControllerDevice.Status = true;
    rgbControllerDevice.SubType = "Switch";
    Serial.println("get device bitti");
    Serial.println(rgbControllerDevice.Status);
    Serial.println(rgbControllerDevice.ConnectionStatus);
    return rgbControllerDevice;
};

Device DeviceRepository::getSavingRelay(String macAddress, String ipAddress, String serialNumber)
{
    Serial.println("get device başladı");
    Device relayDevice;
    relayDevice.ConnectionStatus = "Online";
    relayDevice.CreatedDate = "";
    relayDevice.DeviceConnectionStatus = "Connected";
    relayDevice.DeviceMacAddress = macAddress;
    relayDevice.DeviceName = "Anahtar";
    relayDevice.DeviceType = "Relay";
    //relayDevice.Id = "DenemeID";
    relayDevice.LocalIpAddress = ipAddress;
    relayDevice.SerialNumber = serialNumber;
    relayDevice.Status = true;
    relayDevice.SubType = "Switch";
    Serial.println("get device bitti");
    Serial.println(relayDevice.Status);
    Serial.println(relayDevice.ConnectionStatus);
    return relayDevice;
};