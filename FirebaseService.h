
#include <Arduino.h>
#include <ArduinoJson.h>
#include <FirebaseESP8266.h>
#include <repositories/DeviceRepository.h>
#include <services/LedService.h>

DeviceRepository deviceRepository;
LedService ledService;

class FirebaseService
{
private:
    FirebaseData firebaseData;
    FirebaseData deviceConnectionStatusStream;
    FirebaseData rgbbStream;
    FirebaseData statusStream;

    
    DeviceType _deviceType;

public:
    void initialize(DeviceType deviceType);
    void setItem(Device device);
    void setItem(String path, String value);
    Device getDeviceItem();
    Rgbb getRgbbItem();
    void streamRgbbListener();
    void streamListener();
    void ipAddressChanged();
    void deleteItem();
    void setDeviceID(String deviceId);
    void setBool(String path, bool value);
    String initDeviceId;
};

void FirebaseService::initialize(DeviceType deviceType)
{
    _deviceType = deviceType;
    Firebase.begin("https://smart-home-602ec-default-rtdb.firebaseio.com/", "NFK0xkUIDClor3wmvs8LIhsLCRWw0qiKNyldna8i");

    deviceConnectionStatusStream.setBSSLBufferSize(512, 512);
    Firebase.beginStream(deviceConnectionStatusStream, "/devices/" + initDeviceId + "/DeviceConnectionStatus");

    if (_deviceType == DeviceType ::RgbController)
    {
        rgbbStream.setBSSLBufferSize(512, 512);
        Firebase.beginStream(rgbbStream, "/devices/" + initDeviceId + "/Rgbb");
    }

    statusStream.setBSSLBufferSize(512, 512);
    Firebase.beginStream(statusStream, "/devices/" + initDeviceId + "/Status");
    Serial.println("Firebase Initialized");
}

void FirebaseService::setItem(Device device)
{
    FirebaseJson jDevice;

    jDevice.add("ConnectionStatus", device.ConnectionStatus);
    jDevice.add("CreatedDate", device.CreatedDate);
    jDevice.add("DeviceConnectionStatus", device.DeviceConnectionStatus);
    jDevice.add("DeviceMacAddress", device.DeviceMacAddress);
    jDevice.add("DeviceName", device.DeviceName);
    jDevice.add("DeviceType", device.DeviceType);
    jDevice.add("Id", initDeviceId);
    jDevice.add("Status", device.Status);
    jDevice.add("SubType", device.SubType);
    jDevice.add("LocalIpAddress", device.LocalIpAddress);
    jDevice.add("SerialNumber", device.SerialNumber);

    if (device.DeviceType == "RgbController")
    {
        FirebaseJson jRgbb;
        jRgbb.add("Red", device.RGBB.Red);
        jRgbb.add("Green", device.RGBB.Green);
        jRgbb.add("Blue", device.RGBB.Blue);
        jRgbb.add("Brightness", device.RGBB.Brightness);
        jDevice.add("Rgbb", jRgbb);
    }

    Firebase.setJSON(firebaseData, "/devices/" + initDeviceId, jDevice);
}

void FirebaseService::setItem(String path, String value)
{
    Firebase.setString(firebaseData, path, value);
}

void FirebaseService::setBool(String path, bool value)
{
    Firebase.setBool(firebaseData, path, value);
}

void FirebaseService::streamRgbbListener()
{
    if (Firebase.readStream(rgbbStream) && rgbbStream.streamAvailable())
    {
        Rgbb rgbb = rgbbRepository.fromJson(rgbbStream.stringData());
        ledService.SetLedColor(rgbb.Red.toInt(), rgbb.Green.toInt(), rgbb.Blue.toInt(), rgbb.Brightness.toInt());
    }
}

void FirebaseService::streamListener()
{
    if (Firebase.readStream(deviceConnectionStatusStream) && deviceConnectionStatusStream.streamAvailable())
    {
        if (deviceConnectionStatusStream.stringData() == "WaitingResponse")
        {
            setItem("/devices/" + initDeviceId + "/DeviceConnectionStatus", "Connected");
        }
    }
    else if (Firebase.readStream(statusStream) && statusStream.streamAvailable())
    {
        if (statusStream.boolData())
        {
            Rgbb rgbb = getRgbbItem();
            ledService.SetLedColor(rgbb.Red.toInt(), rgbb.Green.toInt(), rgbb.Blue.toInt(), rgbb.Brightness.toInt());
        }
        else
            ledService.SetLedColor(0, 0, 0, 0);
    }
}

Device FirebaseService::getDeviceItem()
{
    Device device2;
    if (Firebase.getJSON(firebaseData, "devices/" + initDeviceId))
    {
        device2 = deviceRepository.fromJson(firebaseData.jsonString());
    }
    else
    {
        Serial.println(firebaseData.errorReason());
    }
    return device2;
}

Rgbb FirebaseService::getRgbbItem()
{
    Rgbb rgbb;
    if (Firebase.getJSON(firebaseData, "/devices/" + initDeviceId + "/Rgbb"))
    {
        rgbb = rgbbRepository.fromJson(firebaseData.jsonString());
    }
    else
    {
        Serial.println(firebaseData.errorReason());
    }
    return rgbb;
}

void FirebaseService::ipAddressChanged()
{
}

void FirebaseService::deleteItem()
{
    if (!Firebase.deleteNode(firebaseData, "/devices/" + initDeviceId))
    {
        Serial.println(firebaseData.errorReason());
    }
}

void FirebaseService::setDeviceID(String deviceId)
{
    initDeviceId = deviceId;
}
