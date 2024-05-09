#include "TwoSegmentDisplay.h"

TwoSegmentDisplay::TwoSegmentDisplay()
  : latchPin(0), clockPin(0), dataPin(0), isAnodeDriven(false)
{
}

void TwoSegmentDisplay::setup(byte latchPin, byte clockPin, byte dataPin, bool isAnodeDriven)
{
  this->latchPin = latchPin;
  this->clockPin = clockPin;
  this->dataPin = dataPin;
  this->isAnodeDriven = isAnodeDriven;

  displaySegments(NNone, NNone);

  // Setup display pins as Outputs
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  
  #ifdef _DEBUG
  Serial.println("Display ready");
  #endif
}

void TwoSegmentDisplay::displayNumbers(int dozens, int units)
{
  displaySegments(numbers[dozens], numbers[units]);
}

void TwoSegmentDisplay::displaySegments(byte first, byte second)
{
  // ST_CP LOW to keep LEDs from changing while reading serial data
  digitalWrite(latchPin, LOW);

  // Použité segmenty mají společnou anodu, tedy segment svítí, pokud je jeho pin na hodnotě logická nula.
  shiftOut(dataPin, clockPin, LSBFIRST, isAnodeDriven ? 255 - second : second);
  shiftOut(dataPin, clockPin, LSBFIRST, isAnodeDriven ? 255 - first : first);

  #ifdef _DEBUG
  Serial.print("display: ");
  Serial.print(first, BIN);
  Serial.print(" ");
  Serial.println(second, BIN);
  #endif

  // ST_CP HIGH change LEDs
  digitalWrite(latchPin, HIGH);
}
