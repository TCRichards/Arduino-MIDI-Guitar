#include <frequencyToNote.h>
#include <pitchToFrequency.h>
#include <pitchToNote.h>

#include <MIDI.h>
#include <SoftwareSerial.h>

#include <midi_Defs.h>
#include <midi_Message.h>
#include <midi_Namespace.h>
#include <midi_Settings.h>

#include "Potentiometer.h"
#include "Button.h"
#include "LED.h"
#include "Note.h"
#include "Display.h"

struct MySettings : public midi::DefaultSettings
{
  //See https://github.com/projectgus/hairless-midiserial/issues/16 for details.
  static const bool UseRunningStatus = false;
  static const long BaudRate = 115200;
};

MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, Serial, MIDI, MySettings);

// Initialize Hardware Components

// Potentiometers 
Potentiometer* strumPot = new Potentiometer(A8);
Potentiometer* mainPot = new Potentiometer(A1);
Potentiometer* neckPot = new Potentiometer(A3);
Potentiometer* octavePot = new Potentiometer(A15);
Potentiometer* joyY = new Potentiometer(A6);
Potentiometer* joyX = new Potentiometer(A7);

// LEDs
LED* led1 = new LED(4); // On if Major
LED* led2 = new LED(3); // On if Minor
LED* led3 = new LED(5); // On if Chromatic

// Buttons
Button* joyBut = new Button(8);
Button* upBut = new Button(13); 
Button* downBut = new Button(12);
Button* scaleBut = new Button(11);

int serialBit = 53; 

// Requred Variables for processing
Note* currentNote = new Note(serialBit); // Also initializes the LCD display

boolean bothOctaves;

void setup() {
  //MIDI.begin();
  Serial.begin(9600);
  led1->turnOn();

  // Zero the potentiometers whose exact values don't matter
  strumPot->setMin(50); // No false positives!
  neckPot->setMin(30);
  octavePot->setMin(50);
  
  joyX->setMin(joyX->getValue());
  joyY->setMin(joyY->getValue());
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
    cycleLEDs();
  } 

  currentNote->updateNote(mainPot, octavePot, strumPot, neckPot, joyX); // Update the notes based on potentiometer values
  /*
  char chars[60];
  sprintf(chars, "Vibrato: %d \t Voltage: %d", currentNote->getVibrato(), neckPot->getValue());
  Serial.println(chars);
  */
  
  if (strumPot->getValue() > strumPot->getMin() && !currentNote->isSounding()) {
    playNote();
  } else if (currentNote->isSounding() && strumPot->getValue() < strumPot->getMin()) { // Don't stop the note if the neck has any pressure
    stopNote();
  } else if (currentNote->isHolding() && currentNote->getNote() != currentNote->getLastNote() && !currentNote->isCrackling()) {
    stopNote();
    playNote();
  }
  int bend = map(joyX->getValue(), 0, 1023, 8191, -8192);
  MIDI.sendPitchBend((int) bend, 1);
  applyControlChanges();
 
 
  }

void playNote() {
    MIDI.sendNoteOn(currentNote->getNote(), currentNote->getVolume(), 1);
    currentNote->setSounding(true);
    currentNote->setLastNote(currentNote->getNote());
} 

void stopNote() {
     MIDI.sendNoteOff(currentNote->getLastNote(), currentNote->getVolume(), 1);
     currentNote->setSounding(false);
}

void applyControlChanges() {
    MIDI.sendControlChange(7, currentNote->getVolume(), 1);
    MIDI.sendControlChange(9, currentNote->getVibrato(), 1);
    int yValue = map(joyY->getValue(), 0, 1023, 0, 127);
    MIDI.sendControlChange(10, yValue, 1);
}

void cycleLEDs() {
  if (led1->isOn()) {
    led1->turnOff();
    led2->turnOn();
  } else if (led2->isOn()) {
    led2->turnOff();
    led3->turnOn();
  } else {
    led3->turnOff();
    led1->turnOn();
  }
}
