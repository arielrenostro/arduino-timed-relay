#include <Arduino.h>
#include <Adafruit_GFX.h>

#include "../components/Display.h"
#include "TimerController.h"

#define HEADER_SIZE 15
#define BROOM_H 15
#define BROOM_W 11
#define WATER_DROP_H 15
#define WATER_DROP_W 9
#define LETTER_2_W 12
#define LETTER_2_H 16
#define LETTER_3_W 18
#define LETTER_3_H 24

uint8_t water[] = {0xf7, 0x80, 0xf7, 0x80, 0xeb, 0x80, 0xeb, 0x80, 0xdd, 0x80, 0xdd, 0x80, 0xbe, 0x80, 0xbe, 0x80, 0x7f, 0x00, 0x7f, 0x00, 0x7f, 0x00, 0xbe, 0x80, 0xbe, 0x80, 0xc1, 0x80, 0xff, 0x80};
uint8_t broom[] = {0xf1, 0xe0, 0xf5, 0xe0, 0xf5, 0xe0, 0xf5, 0xe0, 0xf5, 0xe0, 0xf5, 0xe0, 0xf5, 0xe0, 0xf5, 0xe0, 0xc4, 0x60, 0xbf, 0xa0, 0x7f, 0xc0, 0x7f, 0xc0, 0x7f, 0xc0, 0x55, 0x40, 0xaa, 0xa0};

class UIController
{
private:
    Display *_display;
    TimerController *_timerController;
    TimerController *_cleanTimerController;

    // state variables
    bool _timerRunning;
    bool _cleanTimerRunning;
    char _timerBuff[7] = {'0', ':', '0', '0', ':', '0', '0'};
    char _timerBuffOld[7] = {'z', 'z', 'z', 'z', 'z', 'z', 'z'};
    char _timerCleanBuff[7] = {'0', ':', '0', '0', ':', '0', '0'};
    char _timerCleanBuffOld[7] = {'z', 'z', 'z', 'z', 'z', 'z', 'z'};

    // water animation
    int16_t _waterX = 1;
    bool _waterDrawing = true;
    unsigned long _waterLastTime = 0;

    // private functions
    void _clearTitle();
    void _drawTitle();
    void _drawWater();
    void _drawTimer();

public:
    UIController(Display *display, TimerController *timerController, TimerController *cleanTimerController);
    bool setup();
    void onLoop();
};

UIController::UIController(Display *display, TimerController *timerController, TimerController *cleanTimerController)
{
    _display = display;
    _timerController = timerController;
    _cleanTimerController = cleanTimerController;
}

bool UIController::setup()
{
    if (_display->setup())
    {
        _drawTitle();
        return true;
    }
    return false;
}

void UIController::onLoop()
{
    if (_timerController->isRunning() != _timerRunning || _cleanTimerController->isRunning() != _cleanTimerRunning)
    {
        _timerRunning = _timerController->isRunning();
        _cleanTimerRunning = _cleanTimerController->isRunning();
        _clearTitle();
    }

    if (_timerRunning)
    {
        _drawWater();
    }
    else
    {
        _drawTitle();
    }

    if (_timerController->isUpdated())
    {
        _drawTimer();
    }
}

void UIController::_clearTitle()
{
    _waterX = 1;
    _waterDrawing = true;
    for (uint8_t i = 0; i < 7; i++)
    {
        _timerCleanBuffOld[i] = 'z';
    }

    _display->fillRect(1, 1, _display->getWidth(), HEADER_SIZE, BLACK);
    _display->display();
}

void UIController::_drawTitle()
{
    _display->setTextSize(2);
    _display->setTextColor(WHITE);

    if (_cleanTimerController->isRunning() && _cleanTimerController->getValue() > 0)
    {
        _cleanTimerController->getTextualValue(&_timerCleanBuff);
        for (uint8_t i = 0; i < 7; i++)
        {
            if (_timerCleanBuff[i] != _timerCleanBuffOld[i])
            {
                _timerCleanBuffOld[i] = _timerCleanBuff[i];
                _display->fillRect(22 + (i * LETTER_2_W), 1, LETTER_2_W, LETTER_2_H, BLACK);
                _display->setCursor(22 + (i * LETTER_2_W), 1);
                _display->print(_timerCleanBuff[i]);
            }
        }
        _display->drawBitmap(1, 1, water, WATER_DROP_W, WATER_DROP_H, BLACK, WHITE);
        _display->drawBitmap(115, 1, water, WATER_DROP_W, WATER_DROP_H, BLACK, WHITE);
    }
    else
    {
        _display->setCursor(22, 1);
        _display->println(F("LIMPEZA"));
        _display->drawBitmap(1, 1, broom, BROOM_W, BROOM_H, BLACK, WHITE);
        _display->drawBitmap(113, 1, broom, BROOM_W, BROOM_H, BLACK, WHITE);
    }

    _display->display();
}

void UIController::_drawWater()
{
    if (millis() - _waterLastTime < 250)
    {
        return;
    }

    _waterLastTime = millis();
    if (_waterDrawing)
    {
        _display->drawBitmap(_waterX, 1, water, WATER_DROP_W, WATER_DROP_H, BLACK, WHITE);
        _display->display();
        _waterX += 13;

        if (_waterX >= 128 - 9)
        {
            _waterDrawing = false;
            _waterX -= 13;
        }
    }
    else
    {
        _display->fillRect(_waterX, 1, WATER_DROP_W, WATER_DROP_H, BLACK);
        _display->display();
        _waterX -= 13;
        if (_waterX < 0)
        {
            _waterDrawing = true;
            _waterX = 1;
        }
    }
}

void UIController::_drawTimer()
{
    _timerController->getTextualValue(&_timerBuff);

    _display->setTextSize(3);
    for (uint8_t i = 0; i < 5; i++)
    {
        if (_timerBuff[i + 2] != _timerBuffOld[i + 2])
        { // clear only this char
            _timerBuffOld[i + 2] = _timerBuff[i + 2];
            _display->fillRect(20 + (i * LETTER_3_W), 30, LETTER_3_W, LETTER_3_H, BLACK);
            _display->setCursor(20 + (i * LETTER_3_W), 30);
            _display->print(_timerBuff[i + 2]);
        }
    }
    _display->display();

    Serial.print(F("Screen Timer: "));
    for (uint8_t i = 0; i < 7; i++)
    {
        Serial.print(_timerBuff[i]);
    }
    Serial.print("\n");
}
