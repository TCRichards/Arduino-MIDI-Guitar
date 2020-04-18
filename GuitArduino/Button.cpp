#include "Button.h"

Button::Button(int pin) {
  pinMode(pin, INPUT);
  _pin = pin;
  _state = false;
  _lastState = false;
  debounceMS = 400;
}

void Button::updateButton() {
  int reading = isOn();
  if (isOn() != _lastState) { // if the reading has changed do to pressing or noise
    _startDebounce = millis();
    _debouncing = true;
    
    _newlyLifted = isOn();
    _newlyPressed = !_newlyLifted;
  }
 // Evaluate the state only when enough time has passed without changes
 if (_debouncing && millis() - _startDebounce > debounceMS) {
    _state = reading;
    _debouncing = false;
  }
  _lastState = reading;
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

boolean Button::stateChanged() {
  return wasLifted() || wasPressedAndDown();
}


boolean Button::wasPressedAndDown() {
  if (_newlyPressed) {
    _newlyPressed = false;
    return true;
  }
  return false;
}

boolean Button::wasLifted() {
  if (_newlyLifted) {
    _newlyLifted = false;
    return true;
  }
  return false;
}
