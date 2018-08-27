#include "Note.h"
#include <MIDI.h>
#include <midi_Defs.h>
#include <midi_Message.h>
#include <midi_Namespace.h>
#include <midi_Settings.h>


struct MySettings : public midi::DefaultSettings
{
  //By default, the Arduino MIDI Library tries to be smart by 
  //excluding the CC byte if it doesn't change (to save bandwidth).
  //This is a problem when starting up Serial<->MIDI software
  //after starting up the Arduino because we miss the first CC byte.
  //Setting UseRunningStatus to false removes this "feature."
  //See https://github.com/projectgus/hairless-midiserial/issues/16 for details.
  static const bool UseRunningStatus = false;
  // Set MIDI baud rate. MIDI has a default baud rate of 31250,
  // but we're setting our baud rate higher in order to 
  // properly decode and read outgoing MIDI data on the computer.
  static const long BaudRate = 115200;
};

MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, Serial, MIDI, MySettings);

Note::Note() {
  currentScale = majorScale;
  _tonic = 60;
  _noteOn = false;
  MIDI.begin();
}

bool Note::isSounding() {
  return _noteOn;
}
void Note::cycleScale() {
  if (currentScale == majorScale) {
    currentScale = minorScale;
  } else if (currentScale == minorScale) {
    currentScale = chromaticScale;
  } else if (currentScale == chromaticScale) {
    currentScale = majorScale;
  }
}

void Note::incrementTonic() {
  _tonic += 1;
}

void Note::decrementTonic() {
  _tonic -= 1;
}

int Note::getTonic() {
  return _tonic;
}

int Note::getNote() {
  return _noteValue;
}

void Note::playNote() {
  MIDI.sendNoteOn(_noteValue, 127, 1);
  if (_bothOctaves) {
    MIDI.sendNoteOn(_noteValue + 12, 127, 1);
  }
  _noteOn = true;
}

void Note::stopNote() {
  MIDI.sendNoteOff(_noteValue, 127, 1);
  MIDI.sendNoteOff(_noteValue + 12, 126, 1);
  _noteOn = false;
}

void Note::applyPitchBend(int value) {
  MIDI.sendPitchBend(value, 1);
}

// Function that returns the midi value of the current note 
// @Param: mainV of the main potentiometer
void Note::updateNote(Potentiometer* mainPot, Potentiometer* octavePot) {
  // Deal with the note played by main pot
  int index = 0;
  int mainV = mainPot->getValue();
  if (mainV > 1000) {
    index = 1;
  } else if (mainV > 900) {
    index = 2;
  } else if (mainV > 800) {
    index = 3;
  } else if (mainV > 700) {
    index = 4;
  } else if (mainV > 600) {
    index = 5;
  } else if (mainV > 550) {
    index = 6;
  } else if (mainV > 500) {
    index = 7;
  } else if (mainV > 450) {
    index = 8;
  } else if (mainV > 400) {
    index = 9;
  } else if (mainV > 350) {
    index = 10;
  } else if (mainV > 300) {
    index = 11;
  } else if (mainV > 250) {
    index = 12;
  } else if (mainV > 200) {
    index = 13;
  } else if (mainV > 100) {
    index = 14;
  } else if (mainV > 50) {
    index = 15;
  } else if (mainV > mainPot->getMin()) {
    index = 16;
  }
  // Apply octave shift
  int octaveV = octavePot->getValue();
  int octaveShift = 0;
  if (octaveV < octavePot->getMin()) {          // Octave pot not pressed
    octaveShift = 0;
    _bothOctaves = false;
  } else if (octaveV > 700) {                   // Octave pot pressed high.  Octave Up
    _bothOctaves = false;
    octaveShift = 12;
  } else if (octaveV < 300) {                   // Octave pot pressed low. Octave Down
    _bothOctaves = false;
    octaveShift = -12;
  } else {                                      // Octave pot pressed mid.  Play two octaves simultaneously
    _bothOctaves = true;
    octaveShift = 0;
  }
  _noteValue = _tonic + currentScale[index] + octaveShift;
}
