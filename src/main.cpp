#include <Arduino.h>
#include <Vector.h>

#include "components/Relay.h"
#include "components/Display.h"
#include "components/PushButton.h"
#include "controllers/PushButtonController.h"
#include "controllers/TimerController.h"

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
#define BUTTON_CANCEL_PIN 6
#define BUTTON_CONFIRM_PIN 7

// Declaration for "ui" behaviors
#define BUTTON_HOLD_DELAY 600
#define BUTTON_HOLD_REPEAT_DELAY 300

// Declaration for timer behaviors
#define TIMER_DEFAULT_VALUE (uint32_t)(3 * 60) // 120s | 3min
#define TIMER_STEP (uint32_t)30                // 30s
#define TIMER_STEP_HOLDING (uint32_t)60        // 60s

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

// state variables
PushButton *pushedButton = nullptr;
char timerBuff[5] = {'0', '0', ':', '0', '0'};

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
  timerController.onLoop();
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

void setup()
{
  Serial.begin(115200);

  valve.setup();
  pushButtonController.setup();

  if (!display.setup())
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }

  display.drawRect(1, 1, SCREEN_WIDTH, SCREEN_HEIGHT, WHITE);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(3, 3);
  display.println(F("Water Value Timed"));
  display.display();
}

void processDisplayLoop()
{
  if (!timerController.isUpdated())
  {
    return;
  }
  timerController.getTextualValue(&timerBuff);

  display.setCursor(3, 10);
  Serial.print("Timer: ");

  for (uint8_t i = 0; i < 5; i++)
  {
    Serial.print(timerBuff[i]);
    display.print(timerBuff[i]);
  }
  display.display();
  Serial.print("\n");
}

void loop()
{
  processPushButtonsLoop();
  processTimerLoop();
  processValveLoop();
  processDisplayLoop();
}