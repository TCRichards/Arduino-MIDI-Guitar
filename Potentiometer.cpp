#include "Potentiometer.h"

Potentiometer::Potentiometer(int pin) {
  _pin = pin;
  pinMode(pin, INPUT);
  _min = 50; // Default minimum value (should be changed by each unique piece)
}

int Potentiometer::getValue() {
  return analogRead(_pin);
}

void Potentiometer::setMin(int minimum) {
  _min = minimum;
}

int Potentiometer::getMin() {
  return _min;
}
