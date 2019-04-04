
#ifndef Potentiometer_h
#define Potentiometer_h

#include <Arduino.h>

class Potentiometer {
  
  public:
    int getValue();
    void setMin(int minimum);
    int getMin();
    bool isOn();
    Potentiometer(int pin);
    
  private:
    int _value;
    int _pin;
    int _min;  
};


#endif
