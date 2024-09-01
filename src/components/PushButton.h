#ifndef PushButton_C
#define PushButton_C

#include "IBaseComponent.h"

#include <Arduino.h>

class PushButton : public IBaseComponent
{
private:
  int _pin;

public:
  PushButton(int pin);
  bool setup();
  bool isPressed();
};

PushButton::PushButton(int pin)
{
  _pin = pin;
}

bool PushButton::setup()
{
  Serial.print(F("PushButton: "));
  Serial.print(_pin);
  Serial.println(F(" setup"));
  pinMode(_pin, INPUT_PULLUP);
  return true;
}

bool PushButton::isPressed()
{
  return digitalRead(_pin) == LOW;
}

#endif