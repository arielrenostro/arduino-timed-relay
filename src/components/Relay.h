#include "IBaseComponent.h"

#include <Arduino.h>

#define RELAY_STATE_UNSPECIFICATED 0
#define RELAY_STATE_ON 1
#define RELAY_STATE_OFF 2

class Relay : public IBaseComponent
{
private:
  int _pin;
  uint8_t _state = RELAY_STATE_UNSPECIFICATED;
  bool _startState;

public:
  Relay(int pin, bool startState);
  bool setup();
  void turnOn();
  void turnOff();
  bool getState();
};

Relay::Relay(int pin, bool startState)
{
  _pin = pin;
  _startState = startState;
}

bool Relay::setup()
{
  Serial.print(F("Relay: "));
  Serial.print(_pin);
  Serial.println(" setup");
  pinMode(_pin, OUTPUT);

  if (_startState)
  {
    turnOn();
  }
  else
  {
    turnOff();
  }

  return true;
}

void Relay::turnOn()
{
  Serial.print(F("Relay: "));
  Serial.print(_pin);
  if (_state == RELAY_STATE_ON)
  {
    Serial.println(F(" is already ON"));
    return;
  }
  Serial.println(" ON");
  digitalWrite(_pin, HIGH);
  _state = RELAY_STATE_ON;
}

void Relay::turnOff()
{
  Serial.print(F("Relay: "));
  Serial.print(_pin);
  if (_state == RELAY_STATE_OFF)
  {
    Serial.println(F(" is already OFF"));
    return;
  }
  Serial.println(F(" OFF"));
  digitalWrite(_pin, LOW);
  _state = RELAY_STATE_OFF;
}

bool Relay::getState()
{
    return _state == RELAY_STATE_ON;
}
