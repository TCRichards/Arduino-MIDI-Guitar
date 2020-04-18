#ifndef Button_h
#define Button_h

#include <Arduino.h>

class Button {
  
 public:
  
  Button(int pin);
  bool isOn();
  void reset();
  bool wasPressed();
  bool stateChanged();
  void updateButton();

  bool wasPressedAndDown();
  bool wasLifted();
    
 private:
  
  int _pin;
  bool _state;
  bool _lastState;
  uint16_t _startDebounce;
  bool _debouncing;
  bool _newlyPressed;
  bool _newlyLifted;
  unsigned long debounceMS;
  

};

#endif
