#include "Note.h"
#include "Display.h"

Note::Note(int serialPin) {
  lcd = new Display(serialPin, 2); // Only need to account for the display's rx pin
  currentScale = majorScale;
  _tonic = 60;
  _sounding = false;
}

// Function that returns the midi value of the current note
void Note::updateNote(Potentiometer* mainPot, Potentiometer* octavePot, Potentiometer* strumPot, Potentiometer* neckPot, Potentiometer* pitchBendPot) {
  // Deal with the note played by main pot
  int index = findInterval(mainPot->getValue(), mainPot->getMin());
  int mainV = mainPot->getValue();
  _vibrato = constrain(map((neckPot->getValue() - neckPot->getMin()), 0, 1023, 0, 127), 0, 127);
  _volume = constrain(map(strumPot->getValue() - strumPot->getMin(), -strumPot->getMin(), 1023, 0, 127), 0, 127);
  _pitchBend = pitchBendPot->getValue();
  _holding = strumPot->isOn() && mainPot->isOn();

  // Determine if we're close to the note boundary
  _crackling = findInterval(mainV + 2, mainPot->getMin()) != index || findInterval(mainV - 2, mainPot->getMin()) != index;
    
  lastOctaveShift = octaveShift;
  // Apply octave shift
  int octaveV = octavePot->getValue();
  
  if (octaveV < octavePot->getMin()) {          // Octave pot not pressed
    octaveShift = 0;
  } else if (octaveV > 275) {                   // Octave pot pressed high.  Octave Up
    octaveShift = 12;
  } else {                   // Octave pot pressed low. Octave Down
    octaveShift = -12;
  }
  
  _noteValue = _tonic + currentScale[index] + octaveShift;
  lcd->updateNote(_noteValue); // Make the display show the correct note
}

int Note::findInterval(int mainV, int minimum) {
  // Tuning values for intervals
  const unsigned int SECOND1 = 907;
  const unsigned int THIRD1 = 810;
  const unsigned int FOURTH1 = 725;
  const unsigned int FIFTH1 = 650;
  const unsigned int SIXTH1 = 582;
  const unsigned int SEVENTH1 = 524;
  const unsigned int EIGHTH1 = 477;
  const unsigned int SECOND2 = 431;
  const unsigned int THIRD2 = 393;
  const unsigned int FOURTH2 = 355;
  const unsigned int FIFTH2 = 317;
  const unsigned int SIXTH2 = 286;
  const unsigned int SEVENTH2 = 246;
  const unsigned int EIGHTH2 = 215;
  const unsigned int SECOND3 = 189;
  const unsigned int THIRD3 = 157;
  const unsigned int FOURTH3 = 121;
  const unsigned int FIFTH3 = 88;
  const unsigned int SIXTH3 = 47;
  int intervals[19] = {SECOND1, THIRD1, FOURTH1, FIFTH1, SIXTH1, SEVENTH1, EIGHTH1,
                       SECOND2, THIRD2, FOURTH2, FIFTH2, SIXTH2, SEVENTH2, EIGHTH2, SECOND3, THIRD3, FOURTH3,
                       FIFTH3, SIXTH3};
  if (mainV < minimum) {
    return 0;
  } else {
    int index = 18; // Start indexing at the end
    while (mainV > intervals[index]) { // Loop until we find the correct interval
      index--;
      if (index == -1) {
        break;      
      }
    }  
    return index + 2;
  }
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

void Note::setSounding(bool value) {
  _sounding = value;
}

void Note::incrementTonic() {
  _tonic += 1;
  lcd -> shiftTonic(1);
}

void Note::decrementTonic() {
  _tonic -= 1;
  lcd -> shiftTonic(-1);
}

void Note::setLastNote(int noteValue) {
  _lastNote = noteValue;
}

int Note::getTonic() {
  return _tonic;
}

int Note::getNote() {
  return _noteValue;
}

int Note::getVibrato() {
  return _vibrato;
}

int Note::getLastNote() {
  return _lastNote;
}

int Note::getVolume() {
  return _volume;
}

bool Note::isSounding() {
  return _sounding;
}

bool Note::isCrackling() {
  return _crackling;
}

bool Note::isHolding() {
  return _holding;
}
