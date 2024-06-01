#pragma once

#include <Arduino.h>

/*
  Item: https://www.aliexpress.com/item/1005005219315035.html
  Name: Static drive 74HC595 2 3 4 segment Digital Tube LED Display Module 2 Digital 74HC595 tube 100% New Original

        128
     --------
    |        |
  4 |        | 64
    |   2    |
     --------
    |        |
  8 |        | 32
    |   16   |
     --------   x 1
*/

class TwoDigitDisplay {
public:
  static constexpr byte TOP_SEGMENT = 128;
  static constexpr byte UPPER_RIGHT_SEGMENT = 64;
  static constexpr byte LOWER_RIGHT_SEGMENT = 32;
  static constexpr byte BOTTOM_SEGMENT = 16;
  static constexpr byte LOWER_LEFT_SEGMENT = 8;
  static constexpr byte UPPER_LEFT_SEGMENT = 4;
  static constexpr byte MIDDLE_SEGMENT = 2;
  static constexpr byte DOT_SEGMENT = 1;
  
  // Constants for segment codes for digits 0-9, minus sign, error, and blank
  const byte N0     = 0b11111100;
  const byte N1     = 0b01100000;
  const byte N2     = 0b11011010;
  const byte N3     = 0b11110010;
  const byte N4     = 0b01100110;
  const byte N5     = 0b10110110;
  const byte N6     = 0b10111110;
  const byte N7     = 0b11100000;
  const byte N8     = 0b11111110;
  const byte N9     = 0b11110110;
  const byte NMinus = 0b00000010;
  const byte NError = 0b10011110;
  const byte NNone  = 0b00000000;
  
  // Array to map digits to segment codes
  const byte numbers[10] = {N0, N1, N2, N3, N4, N5, N6, N7, N8, N9};

private:
  // Define connections to 74HC595 shift register
  byte latchPin;  // 74HC595 RCLK   ST_CP (LOAD)
  byte clockPin;  // 74HC595 SRCLK  SH_CP (SCLK)
  byte dataPin;   // 74HC595 SER    DS    (DATA)
  bool isAnodeDriven; // Flag to indicate if display is common anode or common cathode

public:
  TwoDigitDisplay();
  void setup(byte latchPin, byte clockPin, byte dataPin, bool isAnodeDriven);

public:
  void displayNumbers(int dozens, int units);
  void displaySegments(const byte dozens, const byte units);
};