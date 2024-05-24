#include "TwoDigitDisplay.h"

// Constructor for TwoDigitDisplay, initializes member variables
TwoDigitDisplay::TwoDigitDisplay()
  : latchPin(0), clockPin(0), dataPin(0), isAnodeDriven(false)
{
}

// Set up the display with the specified pins and anode/cathode type
void TwoDigitDisplay::setup(byte latchPin, byte clockPin, byte dataPin, bool isAnodeDriven)
{
  this->latchPin = latchPin;
  this->clockPin = clockPin;
  this->dataPin = dataPin;
  this->isAnodeDriven = isAnodeDriven;

  // Initialize display pins to LOW before setting as outputs
  digitalWrite(latchPin, LOW);
  digitalWrite(clockPin, LOW);
  digitalWrite(dataPin, LOW);
  
  // Set up display pins as outputs
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  
  displaySegments(NNone, NNone); // Initialize display to show nothing

  #ifdef _DEBUG
  Serial.println("Display ready");
  #endif
}

// Display numbers on the two-digit display
void TwoDigitDisplay::displayNumbers(int dozens, int units)
{
  displaySegments(numbers[dozens], numbers[units]);
}

// Display specific segments on the two-digit display
void TwoDigitDisplay::displaySegments(byte first, byte second)
{
  // Set latch pin to LOW to prepare for data shifting
  digitalWrite(latchPin, LOW);

  // Shift out the data for the second and first digit
  // Adjust for common anode/cathode configuration
  shiftOut(dataPin, clockPin, LSBFIRST, isAnodeDriven ? 255 - second : second);
  shiftOut(dataPin, clockPin, LSBFIRST, isAnodeDriven ? 255 - first : first);

  #ifdef _DEBUG
  Serial.print("display: ");
  Serial.print(first, BIN);
  Serial.print(" ");
  Serial.println(second, BIN);
  #endif

  // Set latch pin to HIGH to display the shifted data
  digitalWrite(latchPin, HIGH);
}