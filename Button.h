#ifndef Button_h
#define Button_h

#include <Arduino.h>

class Button {
 public:
  Button(int pin);
  bool isOn();
  void reset();
  bool wasPressed();
  void updateButton();
    
 private:
  int _pin;
  bool _state;
  bool _lastState;
  uint16_t _startDebounce;
  bool _debouncing;
  unsigned long debounceMS;

};

#endif
