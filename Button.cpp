#include "Button.h"

Button::Button(int pin) {
  pinMode(pin, INPUT);
  _pin = pin;
  _state = false;
  _lastState = false;
  debounceMS = 200;
}

// Returns if the button is currently pressed (no debounce)
boolean Button::isOn() {
  return digitalRead(_pin);
}

// returns if the button has been pressed and resets the state
boolean Button::wasPressed() { // Only evaluates true once per press
  bool value = _state;
  _state = false;
  return value;
}

void Button::updateButton() {
  int reading = digitalRead(_pin);
  if (reading != _lastState) { // if the reading has changed do to pressing or noise
    _startDebounce = millis();
  }
 // Evaluate the state only when enough time has passed without changes
 if (millis() - _startDebounce > debounceMS) {  
    _state = reading;
  } 
  _lastState = reading;
}
