#include <Arduino.h>
#include <services/FirebaseService.h>
#include <services/MemoryService.h>
#include <ESP8266WiFi.h>

FirebaseService firebaseService;
MemoryService memoryService;

class WifiService
{
private:
    // const String wifi_ssid = "SUPERONLINE_WiFi_6054";
    // const String wifi_password = "UJMURKKPRHAU";

    // const String wifi_ssid = "TP-Link_72E7";
    // const String wifi_password = "27939765";

    String _macAddress;
    String _serialNumber;
    DeviceType _deviceType;

    char connectedSsid[32];
    char connectedPassword[64];

    bool pairingMode = false;

    bool startSmartConfig();
    void disconnect();
    void getConnectedWifiInfo();
    bool connectWifi(String wifi_ssid, String wifi_password);

public:
    String localIpAddress;

    bool checkWifiInfo();
    void restartDevice();
    void pair();
    bool checkConnected();
    void initialize(String deviceId, String macAddress, String serialNumber, DeviceType deviceType);
};

bool WifiService::connectWifi(String wifi_ssid, String wifi_password)
{
    BlinkLed(NOT_CONNECTED, ledService._redPin, ledService._greenPin, ledService._bluePin);
    WiFi.begin(wifi_ssid, wifi_password);
    Serial.println("connecting");
    Serial.println("ssid:" + wifi_ssid);
    Serial.println("pass:" + wifi_password);
    unsigned long timeoutMillis = millis();
    while (WiFi.status() != WL_CONNECTED)
    {
        if (!timeout(timeoutMillis, 10000))
        {
            Serial.print(".");
            BlinkLed(NOT_CONNECTED, ledService._redPin, ledService._greenPin, ledService._bluePin);
            delay(500);
        }
        else
            return false;
    }
    BlinkLed(NOT_CONNECTED, ledService._redPin, ledService._greenPin, ledService._bluePin);
    localIpAddress = WiFi.localIP().toString();
    Serial.println();
    Serial.print("Connected: ");
    Serial.println(localIpAddress);
    digitalWrite(LED_PIN, LOW);
    return true;
}

bool WifiService::checkWifiInfo()
{
    String ssid = memoryService.readString(MemoryValues::SSID);
    String password = memoryService.readString(MemoryValues::PASSWORD);

    if (ssid != "" && password != "")
    {
        Serial.println("1-ssid:" + ssid);
        Serial.println("1-pass:" + password);
        bool isConnectedWifi = connectWifi(ssid, password);
        if (isConnectedWifi)
        {
            Serial.println("Connected!");
            return true;
        }
        else
        {
            Serial.println("Not Connected!");
            return false;
        }
    }
    else
    {
        Serial.println("SSID and Password has not found!");
        return false;
    }
}

bool WifiService::startSmartConfig()
{
    WiFi.mode(WIFI_STA);
    unsigned long timeoutMillis = millis();
    while (WiFi.status() != WL_CONNECTED)
    {
        BlinkLed(PAIR, ledService._redPin, ledService._greenPin, ledService._bluePin);
        WiFi.beginSmartConfig();
        delay(200);
        while (!WiFi.smartConfigDone())
        {
            if (!timeout(timeoutMillis, 90000))
            {
                Serial.print(".");
                BlinkLed(PAIR, ledService._redPin, ledService._greenPin, ledService._bluePin);
                delay(200);
            }
            else
            {
                Serial.println("false");
                return false;
            }
        }
    }
    localIpAddress = WiFi.localIP().toString();
    ledService.SetLedColor(255, 255, 255, 100);
    //digitalWrite(LED_PIN, LOW);
    return true;
}

void WifiService::getConnectedWifiInfo()
{
    struct station_config wifiConfig;
    wifi_station_get_config(&wifiConfig);
    //Serial.println(wifiConfig.ssid);
    memcpy(connectedSsid, wifiConfig.ssid, sizeof(wifiConfig.ssid));
    memcpy(connectedPassword, wifiConfig.password, sizeof(wifiConfig.password));

    Serial.println(connectedSsid);
    Serial.println(connectedPassword);
}

void WifiService::disconnect()
{
    WiFi.disconnect();
}

void WifiService::restartDevice()
{
    ESP.restart();
}

void WifiService::pair()
{
    ledService.Fade(2);
    BlinkLed(PAIR, ledService._redPin, ledService._greenPin, ledService._bluePin);
    disconnect();
    memoryService.resetMemory();
    firebaseService.deleteItem();
    if (startSmartConfig())
    {
        firebaseService.deleteItem();
        getConnectedWifiInfo();
        bool ssidIsSaved = memoryService.writeString(MemoryValues::SSID, connectedSsid);
        bool passwordIsSaved = memoryService.writeString(MemoryValues::PASSWORD, connectedPassword);
        if (ssidIsSaved && passwordIsSaved)
        {
            Serial.println("memory'e kaydedildi");
            switch (_deviceType)
            {
            case DeviceType::RgbController:
                firebaseService.setItem(deviceRepository.getSavingRgbController(_macAddress, localIpAddress, _serialNumber));
                break;
            case DeviceType::Relay:
                firebaseService.setItem(deviceRepository.getSavingRelay(_macAddress, localIpAddress, _serialNumber));
                break;
            default:
                break;
            }
        }
        else
        {
            Serial.println("kaydedilemedi");
        }
    }
}

bool WifiService::checkConnected()
{
    return WiFi.status() == WL_CONNECTED;
}

void WifiService::initialize(String deviceId, String macAddress, String serialNumber, DeviceType deviceType)
{
    if (deviceId != "" && macAddress != "" && serialNumber != "")
    {
        firebaseService.setDeviceID(deviceId);
        _macAddress = macAddress;
        _serialNumber = serialNumber;
        _deviceType = deviceType;
    }
}