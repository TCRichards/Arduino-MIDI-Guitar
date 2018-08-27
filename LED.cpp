#include "LED.h"

LED::LED(int pin) {
  pinMode(pin, OUTPUT);
  _pin = pin;
}

boolean LED::isOn() {
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
  while(true) {
    alternate();
    delay(period);  
  }
}