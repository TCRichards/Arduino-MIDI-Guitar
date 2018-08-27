#include "Button.h"
#include "LED.h"
#include "Note.h"
#include "Potentiometer.h"
#include <Arduino.h>

// Initialize Hardware Components
// Potentiometers 
Potentiometer* strumPot = new Potentiometer(A1);
Potentiometer* mainPot = new Potentiometer(A2);
Potentiometer* octavePot = new Potentiometer(A3);
Potentiometer* neckPot = new Potentiometer(A4);
Potentiometer* knob1 = new Potentiometer(A5);
Potentiometer* knob2 = new Potentiometer(A6);
Potentiometer* joyX = new Potentiometer(A7);
Potentiometer* joyY = new Potentiometer(A9);

// LED's
LED* led1 = new LED(10);
LED* led2 = new LED(11);
LED* led3 = new LED(12);

// Buttons
Button* joyBut = new Button(6);
Button* upBut = new Button(7);
Button* downBut = new Button(8);
Button* scaleBut = new Button(9);


// Requred Variables for processing
Note* currentNote = new Note();

int octaveShift;
boolean bothOctaves;

bool sliding; // If notes are slurred (joystick pressed)


void setup() {

}

void loop() { 
  // Update the buttons
  upBut->updateButton();
  downBut->updateButton();
  scaleBut->updateButton();
  joyBut->updateButton();
  // Has a button been pressed?
  if (upBut->wasPressed()) {
    currentNote->incrementTonic();
  }
  if (downBut->wasPressed()) {
    currentNote->decrementTonic();
  } 
  if (scaleBut->wasPressed()) {
    currentNote->cycleScale();
  }
  // Deal with multiple octaves
  
  sliding = joyBut->isOn();
  // put your main code here, to run repeatedly:

  currentNote->updateNote(mainPot, octavePot);
  
  if (currentNote->isSounding()) {
    currentNote->playNote();
    
  } else {
    
  }


















}
