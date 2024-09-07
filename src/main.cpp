#include <Arduino.h>
#include <Vector.h>

#include "components/Relay.h"
#include "components/Display.h"
#include "components/PushButton.h"
#include "controllers/PushButtonController.h"
#include "controllers/TimerController.h"
#include "controllers/UIController.h"

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET -1    // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for relay valve
#define VALVE_RELAY_PIN 3
#define VALVE_RELAY_START_STATE false

#define BUTTON_UP_PIN 4
#define BUTTON_DOWN_PIN 5
#define BUTTON_CANCEL_PIN 7
#define BUTTON_CONFIRM_PIN 6

// Declaration for "ui" behaviors
#define BUTTON_HOLD_DELAY 600
#define BUTTON_HOLD_REPEAT_DELAY 300

// Declaration for timer behaviors
#define TIMER_CLEAN_VALUE (uint32_t)(8 * 60 * 60) // 28.800 | 8h
#define TIMER_DEFAULT_VALUE (uint32_t)(3 * 60)    // 120s   | 3min
#define TIMER_STEP (uint32_t)30                   // 30s
#define TIMER_STEP_HOLDING (uint32_t)60           // 60s

// Objects
Relay valve(VALVE_RELAY_PIN, VALVE_RELAY_START_STATE);
Display display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
PushButton buttonUp(BUTTON_UP_PIN);
PushButton buttonDown(BUTTON_DOWN_PIN);
PushButton buttonCancel(BUTTON_CANCEL_PIN);
PushButton buttonConfirm(BUTTON_CONFIRM_PIN);

// Controllers
PushButton *x[4] = {&buttonUp, &buttonDown, &buttonCancel, &buttonConfirm};
PushButtonController pushButtonController(Vector<PushButton *>(x, 4), BUTTON_HOLD_DELAY, BUTTON_HOLD_REPEAT_DELAY);
TimerController timerController(TIMER_DEFAULT_VALUE);
TimerController cleanTimerController(TIMER_CLEAN_VALUE);
UIController uiController(&display, &timerController, &cleanTimerController);

// state variables
PushButton *pushedButton = nullptr;

// function declarations
void processPushButtonsLoop();
void processTimerLoop();
void processValveLoop();
void processDisplayLoop();

// main functions
void setup()
{
  Serial.begin(115200);

  valve.setup();
  pushButtonController.setup();

  if (!uiController.setup())
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
    {
      // Don't proceed, loop forever
    }
  }
}

void loop()
{
  processPushButtonsLoop();
  processTimerLoop();
  processValveLoop();
  processDisplayLoop();
}

// functions implementations
void processPushButtonsLoop()
{
  pushedButton = pushButtonController.onLoop();
  if (pushedButton != nullptr)
  {
    if (&buttonUp == pushedButton)
    {
      if (pushButtonController.isHolding())
      {
        timerController.addTime(TIMER_STEP_HOLDING);
      }
      else
      {
        timerController.addTime(TIMER_STEP);
      }
    }
    else if (&buttonDown == pushedButton)
    {
      if (pushButtonController.isHolding())
      {
        timerController.subTime(TIMER_STEP_HOLDING);
      }
      else
      {
        timerController.subTime(TIMER_STEP);
      }
    }
    else if (&buttonCancel == pushedButton)
    {
      timerController.cancel();
    }
    else if (&buttonConfirm == pushedButton)
    {
      timerController.start();
    }
  }
}

void processTimerLoop()
{
  if (timerController.isRunning())
  {
    timerController.onLoop();

    if (!timerController.isRunning())
    {
      cleanTimerController.setTime(TIMER_CLEAN_VALUE);
      cleanTimerController.start();
    }
  }
  else if (cleanTimerController.isRunning())
  {
    cleanTimerController.onLoop();
  }
}

void processValveLoop()
{
  if (timerController.isRunning())
  {
    if (!valve.getState())
    {
      valve.turnOn();
    }
    return;
  }

  if (valve.getState())
  {
    valve.turnOff();
  }
}

void processDisplayLoop()
{
  uiController.onLoop();
}