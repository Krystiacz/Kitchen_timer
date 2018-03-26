# Kitchen_timer
Kitchen timer based on AVR MCU with clock and thermometer (DS3231) with rotary encoder

This project using a 
- clone of Arduino nano v3 board 
- rotary encoder (24 steps)
- 7-segment display with TM1637 controller (SPI)
- DS3232 RTC module with integrated thermometer (I2C)

The software is written in C language in Eclipse with AVR Plugins

CONNECTIONS:

TM1637 :

DIO -> PD2 (D2)
CLK -> PD3 (D3)
GND -> GND
5V  -> VCC

DS3231 :

SDA -> PC4 (A4)
SCL -> PC5 (A5)
GND -> GND
VCC -> VCC

Rotary encoder :

OUT_A -> PD5
OUT_B -> PD4
SWITCH -> PD6
