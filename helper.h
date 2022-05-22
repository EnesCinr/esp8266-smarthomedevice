#include <Arduino.h>
#include <environments.h>

enum BlinkType
{
    PAIR,
    NOT_CONNECTED
};

unsigned long previousMillis = 0;
int ledState = LOW;
void Blink(BlinkType blinkType)
{
    unsigned long interval;
    switch (blinkType)
    {
    case PAIR:
        interval = 300;
        break;
    case NOT_CONNECTED:
        interval = 800;
        break;
    default:
        interval = 100;
        break;
    }

    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval)
    {
        previousMillis = currentMillis;

        if (ledState == LOW)
            ledState = HIGH;
        else
            ledState = LOW;

        digitalWrite(LED_PIN, ledState);
    }
}

unsigned long previousBlinkLedMillis = 0;
void BlinkLed(BlinkType blinkType, int redPin, int greenPin, int bluePin)
{
    unsigned long interval;
    switch (blinkType)
    {
    case PAIR:
        interval = 300;
        break;
    case NOT_CONNECTED:
        interval = 800;
        break;
    default:
        interval = 100;
        break;
    }

    unsigned long currentMillis = millis();
    if (currentMillis - previousBlinkLedMillis >= interval)
    {
        previousBlinkLedMillis = currentMillis;

        if (ledState == LOW)
        {
            ledState = HIGH;
            analogWrite(redPin, 255);
            analogWrite(greenPin, 255);
            analogWrite(bluePin, 255);
        }
        else
        {
            ledState = LOW;
            analogWrite(redPin, 0);
            analogWrite(greenPin, 0);
            analogWrite(bluePin, 0);
        }

        if (blinkType != NOT_CONNECTED)
            digitalWrite(LED_PIN, ledState);
    }
}

bool timeout(unsigned long timeoutMillis, unsigned long duration)
{
    if (millis() - timeoutMillis > duration)
        return true;
    else
        return false;
}