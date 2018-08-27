#ifndef LED_h
#define LED_h

#include <Arduino.h>



class LED {
  public: 
    LED(int pin);
    bool isOn();
    void turnOn();
    void turnOff();
    void alternate();
    void flash(int period);

  private:
    int _pin;
  
};

#endif
