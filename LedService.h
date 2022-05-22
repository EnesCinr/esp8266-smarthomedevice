#include <Arduino.h>

class LedService
{
private:
public:
    int _redPin;
    int _greenPin;
    int _bluePin;

    void initialize(int redPin, int greenPin, int bluePin);
    void Fade(int repeatCount);
    void SetLedColor(int red, int green, int blue, int brightness);
};

void _fade(int firstPin, int secondPin)
{
    for (int i = 0; i < 255; i += 1)
    {
        analogWrite(firstPin, 255 - i);
        analogWrite(secondPin, i);
        delay(1);
    }
}

void LedService::Fade(int repeatCount)
{
    for (int i = 0; i < repeatCount; i++)
    {
        _fade(_greenPin, _redPin);
        _fade(_redPin, _bluePin);
        _fade(_bluePin, _greenPin);
    }

    analogWrite(_redPin, 255);
    analogWrite(_greenPin, 255);
    analogWrite(_bluePin, 255);
}

void LedService::initialize(int redPin, int greenPin, int bluePin)
{
    _redPin = redPin;
    _greenPin = greenPin;
    _bluePin = bluePin;

    pinMode(this->_redPin, OUTPUT);
    pinMode(this->_greenPin, OUTPUT);
    pinMode(this->_bluePin, OUTPUT);
}

void LedService::SetLedColor(int red, int green, int blue, int brightness)
{
    analogWrite(_redPin, red);
    analogWrite(_greenPin, green);
    analogWrite(_bluePin, blue);
}