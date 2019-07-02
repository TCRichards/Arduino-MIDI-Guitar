#include "LED.h"

LED::LED(int pin) {
  pinMode(pin, OUTPUT);
  _pin = pin;
}

bool LED::isOn() {
  return digitalRead(_pin);
}

void LED::turnOn() {
  digitalWrite(_pin, HIGH);
}

void LED::turnOff() {
  digitalWrite(_pin, LOW);
}

void LED::alternate() {
  digitalWrite(_pin, !isOn());
}

void LED::flash(int period) {
  _flashing = true;
  while(_flashing) {
    alternate();
    delay(period);  
  }
}
