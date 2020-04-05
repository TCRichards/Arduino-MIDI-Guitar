/*
   Source code for the GuitArduino: an Arduino controlled MIDI instrument
   Author: Thomas Richards
   Date Modified: 9/2/2019
*/

#include <Arduino.h>
#include <MIDI.h>

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
Potentiometer* strumPot = new Potentiometer(A10);
Potentiometer* mainPot = new Potentiometer(A2);
Potentiometer* octavePot = new Potentiometer(A3);
Potentiometer* neckPot = new Potentiometer(A11);

Potentiometer* joyY = new Potentiometer(A6);
Potentiometer* joyX = new Potentiometer(A7);
Potentiometer* knob1 = new Potentiometer(A13);
Potentiometer* knob2 = new Potentiometer(A14);
Potentiometer* knob3 = new Potentiometer(A15);

// LEDs
LED* led1 = new LED(3); // On if Major
LED* led2 = new LED(4); // On if Minor
LED* led3 = new LED(5); // On if Chromatic

// Buttons
Button* upBut = new Button(8);
Button* downBut = new Button(7);
Button* scaleBut = new Button(6);
Button* joyBut = new Button(9);

int lcdPin = 32;

// To avoid flooding software with MIDI messages, only send on change
byte lastPitchbend = 0;
byte lastVolume = 0;
byte lastPortamento = 0;
byte lastJoyValue = 0;
byte lastVibrato = 0;
int lastBend = 0;

// Requred Variables for processing
Note* currentNote = new Note(lcdPin); // Also initializes the LCD display

boolean bothOctaves;

void setup() {
  // Make additional ground pin
  digitalWrite(22, HIGH);
  // Write the joystick input high -- apparently this is necessary
  digitalWrite(9, HIGH);

  //MIDI.begin();
  Serial.begin(9600);
  led1->turnOn();


  // Zero the potentiometers whose exact values don't matter
  strumPot->setMin(30); // No false positives! (This one is particularly important)
  octavePot->setMin(30);
  mainPot->setMin(2);

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
  if (joyBut->wasPressed()) {
  }

  currentNote->updateNote(mainPot, octavePot, strumPot, neckPot, joyX); // Update the notes based on potentiometer values
//  Serial.println(currentNote->getPitchBend());
  bool tonicOctave = currentNote->isSounding() && currentNote->findInterval(mainPot->getValue(), mainPot->getMin()) == 0 && currentNote->octaveShift != currentNote->lastOctaveShift;
  if (strumPot->getValue() > strumPot->getMin() && !currentNote->isSounding()) {
    playNote();
  }
  else if (currentNote->isSounding() && strumPot->getValue() < strumPot->getMin()) { // Don't stop the note if the neck has any pressure
    stopNote();
  } else if ((currentNote->isHolding() && currentNote->getNote() != currentNote->getLastNote() && !currentNote->isCrackling()) || tonicOctave) {
    stopNote();
    playNote();
  }
  // Apply note effects such as pitch bend and control changes
  applyControlChanges();
}

void playNote() {
  MIDI.sendNoteOn(currentNote->getNote(), 127, 1);
  currentNote->setSounding(true);
  currentNote->setLastNote(currentNote->getNote());
}

void stopNote() {
  MIDI.sendNoteOff(currentNote->getLastNote(), currentNote->getVolume(), 1);
  currentNote->setSounding(false);
}

// These are the style and sound effects that change the sound
void applyControlChanges() {
  // Only apply each control change if different than last
  if (currentNote->getVolume() != lastVolume) {
    lastVolume = currentNote->getVolume();
    MIDI.sendControlChange(7, currentNote->getVolume(), 1);
  }
  if (abs(currentNote->getVibrato() - lastVibrato) > 10) {
    lastVibrato = currentNote->getVibrato();
    MIDI.sendControlChange(9, currentNote->getVibrato(), 1);
  }

  // Determine whether joystick adds pitch bend or distortion based on relative deltas in x and y directions
  int yValue = map(joyY->getValue(), 0, 1023, 0, 127);
  int midMIDIVal = map(joyY->getMin(), 0, 1023, 0, 127);  // Joystick's default value in Y direction scaled to 0 - 127
  int deltaY = midMIDIVal - yValue; // Displacement in Y direction
  int xValue = map(joyX->getValue(), 0, 1023, 0, 127);
  int midXValue = map(joyX->getMin(), 0, 1023, 0, 127);
  int deltaX = midXValue - xValue;

  if (abs(deltaX) > abs(deltaY)) { // Send Pitch Bend
    int bend = currentNote->getPitchBend() - 8192;
    if (abs(lastBend - bend) > 4) {
      MIDI.sendPitchBend(bend, 1);
      lastBend = bend;
    }
  } else {  // Send distortion instead
    if (lastBend != 8192) { // Make sure pitch bend is set to 0
      MIDI.sendPitchBend(0, 1);
      lastBend = 0;
    }
    if (yValue != lastJoyValue) { // Joystick value has changed
      if (deltaY < 0) {  // Moved joystick up (to lower Voltage)
        int downVal = constrain(map(deltaY, 0, midMIDIVal - 127, 0, 127), 0, 127);  // Scaled to MIDI value (0 - 127)
        MIDI.sendControlChange(17, downVal, 1); // Send over control channel 17
      } else {  // Moved joystick down (to higher V)
        int upVal = constrain(map(deltaY, 0, midMIDIVal, 0, 127), 0, 127);
        MIDI.sendControlChange(15, upVal, 1);
      }
      lastJoyValue = yValue;
    }
  }

  // Send Portamento Data
  int portScaled = constrain(map(knob1->getValue(), 0, 1023, 0, 127), 0, 127);
  if (abs(portScaled - lastPortamento) > 5) {
    MIDI.sendControlChange(12, portScaled, 1);
    lastPortamento = portScaled;
  }
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
