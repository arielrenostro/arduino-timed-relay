# Arduino Timed Relay

A C++ POO Arduino project of a timer for a relay. It uses a relay module, an OLED 0.96 inch display for timer visualization and 4 push buttons to operate it. These buttons have the following behaviors:
- Up: Increase timer in 30s (hold will increase in 1min)
- Down: Decrease timer in 30s (hold will decrease in 1min)
- Start: Starts the timer and opens the valve
- Cancel: Cancels the timer and closes the valve

Timer value can be changed while it's running.

## Components
- Arduino Nano AT Mega 328
- Relay module
- OLED 0.96 I2c inch display
- 4 Push buttons
- Water 12v valve
- 12v power supply

## Pins
- D3: Relay signal
- D4: Push button Up (pull up, GND)
- D5: Push button Down (pull up, GND)
- D6: Push button Start (pull up, GND)
- D7: Push button Cancel (pull up, GND)
- A4: Display SDA
- A5: Display SCK
