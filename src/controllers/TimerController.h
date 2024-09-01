#include <Arduino.h>

class TimerController
{
private:
    uint32_t _default;
    uint64_t _lastLoopTime = 0;
    int32_t _lastUpdateCheck = 0;
    int32_t _timer = 0;
    bool _running = false;

public:
    TimerController(uint32_t default_);
    void addTime(uint32_t value);
    void subTime(uint32_t value);
    void cancel();
    void start();
    void onLoop();
    bool isRunning();
    bool isUpdated();
    int32_t getValue();
    void getTextualValue(char (*value)[5]);
};

TimerController::TimerController(uint32_t default_)
{
    _default = default_ * 1000;
    _timer = default_ * 1000;
}

void TimerController::addTime(uint32_t value)
{
    _timer += value * 1000;

    Serial.print(F("Timer: "));
    Serial.println(_timer);
}

void TimerController::subTime(uint32_t value)
{
    _timer -= value * 1000;
    if (_timer < 0)
    {
        _timer = 0;
    }

    Serial.print(F("Timer: "));
    Serial.println(_timer);
}

void TimerController::cancel()
{
    Serial.print(F("Timer: "));
    Serial.println(F("stopped"));

    _running = false;
    _timer = _default;
}

void TimerController::start()
{
    if (_running)
    {
        return;
    }

    if (_timer <= 0)
    {
        cancel();
        return;
    }

    Serial.print(F("Timer: "));
    Serial.print(_timer);
    Serial.println(F(" started"));

    _running = true;
    _lastLoopTime = millis();
}

void TimerController::onLoop()
{
    if (!_running)
    {
        return;
    }

    _timer -= millis() - _lastLoopTime;
    if (_timer > 0)
    {
        _lastLoopTime = millis();
    }
    else
    {
        cancel();
    }
}

bool TimerController::isRunning()
{
    return _running;
}

int32_t TimerController::getValue()
{
    return _timer;
}

void TimerController::getTextualValue(char (*value)[5])
{
    uint8_t minutes = _timer / 1000 / 60;
    uint8_t seconds = _timer / 1000 % 60;

    (*value)[0] = *utoa(minutes / 10, NULL, 10);
    (*value)[1] = *utoa(minutes % 10, NULL, 10);
    (*value)[2] = ':';
    (*value)[3] = *utoa(seconds / 10, NULL, 10);
    (*value)[4] = *utoa(seconds % 10, NULL, 10);
}

bool TimerController::isUpdated()
{
    if (_timer / 1000 != _lastUpdateCheck)
    {
        _lastUpdateCheck = _timer / 1000;
        return true;
    }
    return false;
}
