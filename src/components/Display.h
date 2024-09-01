#include "IBaseComponent.h"

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

class Display : public IBaseComponent
{
private:
  Adafruit_SSD1306 _display;

public:
  Display(int width, int height, TwoWire *wire, int oledReset);

  bool setup();
  void clearDisplay();
  void setTextSize(uint8_t i);
  void setTextColor(int c);
  void setCursor(int16_t x, int16_t y);
  void drawRect(int16_t x, int16_t y, int16_t width, int16_t height, uint16_t color);
  void print(char v);
  void println(const __FlashStringHelper *ifsh);
  void display();
};

Display::Display(int width, int height, TwoWire *wire, int oledReset)
{
  _display = Adafruit_SSD1306(width, height, wire, oledReset);
}

bool Display::setup()
{
  Serial.println(F("Display: Starting"));
  if (_display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println(F("Display: Started"));
    clearDisplay();
    return true;
  }
  Serial.println(F("Display: Failed"));
  return false;
}

void Display::clearDisplay()
{
  _display.clearDisplay();
}

void Display::setTextSize(uint8_t i)
{
  _display.setTextSize(i);
}

void Display::setTextColor(int c)
{
  _display.setTextColor(c);
}

void Display::setCursor(int16_t x, int16_t y)
{
  _display.setCursor(x, y);
}

void Display::print(char v)
{
  _display.print(v);
}

void Display::println(const __FlashStringHelper *ifsh)
{
  _display.println(ifsh);
}

void Display::display()
{
  _display.display();
}

void Display::drawRect(int16_t x, int16_t y, int16_t width, int16_t height, uint16_t color)
{
  _display.drawRect(x, y, width, height, color);
}
