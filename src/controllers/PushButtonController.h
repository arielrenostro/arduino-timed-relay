#include <Arduino.h>
#include <Vector.h>
#include "../components/PushButton.h"

class PushButtonController
{
private:
    Vector<PushButton *> _buttons;
    unsigned int _holdDelay;
    unsigned int _holdRepeatDelay;

    PushButton *_pushed = nullptr;
    bool _holding = false;
    unsigned long _pushTime = 0;
    unsigned long _calcAux = 0;

public:
    PushButtonController(Vector<PushButton *> buttons, unsigned int holdDelay, unsigned int holdRepeatDelay);
    void setup();
    PushButton *onLoop();
    bool isHolding();
};

PushButtonController::PushButtonController(Vector<PushButton *> buttons, unsigned int holdDelay, unsigned int holdRepeatDelay)
{
    _buttons = buttons;
    _holdDelay = holdDelay;
    _holdRepeatDelay = holdRepeatDelay;
}

void PushButtonController::setup()
{
    for (unsigned int i = 0; i < _buttons.size(); i++)
    {
        _buttons.at(i)->setup();
    }
}

PushButton *PushButtonController::onLoop()
{
    if (_pushed != nullptr)
    {
        if (_pushed->isPressed())
        {
            _calcAux = millis() - _pushTime;
            if (!_holding && _calcAux >= _holdDelay)
            {
                _holding = true;
                _pushTime = millis();
                return _pushed;
            }

            if (_holding && _calcAux >= _holdRepeatDelay)
            {
                _pushTime = millis();
                return _pushed;
            }
            return nullptr;
        }
        else
        {
            if (!_holding)
            {
                PushButton *aux = _pushed;
                _pushTime = millis();
                _pushed = nullptr;
                return aux;
            }
            _holding = false;
            _pushed = nullptr;
        }
    }
    else
    {
        if (millis() - _pushTime < 200) // prevent ghost click
        {
            return nullptr;
        }
    }

    for (unsigned int i = 0; i < _buttons.size(); i++)
    {
        if (_buttons.at(i)->isPressed())
        {
            _pushTime = millis();
            _pushed = _buttons.at(i);
            break;
        }
    }
    return nullptr;
}

bool PushButtonController::isHolding()
{
    return _holding;
}
