#include <Queue.h>
#include "Note.h"
#include "Display.h"

Note::Note(int serialPin) {
  lcd = new Display(serialPin, 2); // Only need to account for the display's rx pin
  currentScale = majorScale;
  _tonic = 48;
  _sounding = false;
  updateLCDTonic();
  strumValQueue = DataQueue<int>(1000);
}


// Function that returns the midi value of the current note
int Note::updateNote(Potentiometer* mainPot, Potentiometer* octavePot, Potentiometer* midStrumPot, Potentiometer* sharpStrumPot, Potentiometer* flatStrumPot, Potentiometer* neckPot, Potentiometer* pitchBendPot) {
  // Deal with the note played by main pot
  // Is the note being strummed?
  _strummed = midStrumPot->isOn() || sharpStrumPot->isOn(); // || flatStrumPot->isOn();
  _lastStrummedPot = _strummedPot;
  
  // Add an offset if played sharp or flat
  int accentOffset;
  // Allow whichever pot was strummed MOST RECENTLY to count as the _strummedPot
  // Ensure by giving priority to the pot if it's not the same as the last one
  if (midStrumPot->isOn() && _lastStrummedPot != midStrumPot) {
    accentOffset = 0;
    _strummedPot = midStrumPot;
  } else if (sharpStrumPot->isOn() && _lastStrummedPot != sharpStrumPot) {
    accentOffset = 1;
    _strummedPot = sharpStrumPot;
  } 
//  The flat strumPot is not connected currently.
//  else if (flatStrumPot->isOn() && _lastStrummedPot != flatStrumPot) {
//    accentOffset = -1;
//    _strummedPot = flatStrummedPot;
//  }
  int mainV = mainPot->getValue();
  int index = findInterval(mainV, mainPot->getMin());
  _vibrato = mapNeckPressure(neckPot);

  _volume = mapStrumPressure(_strummedPot);
  _pitchBend = mapPitchBend(pitchBendPot);
  // TODO: Stop jittering from holding
  
  _holding = _strummed && !mainPot->isOn();

  // Determine if we're close to the note boundary
  int cracklingRange = 5;
  _crackling = findInterval(mainV + cracklingRange, mainPot->getMin()) != index || findInterval(mainV - cracklingRange, mainPot->getMin()) != index;
  lastOctaveShift = octaveShift;
  // Apply octave shift
  int octaveV = octavePot->getValue();
  
  if (octaveV > 400) {                   // Octave pot pressed high.  Octave Up
    octaveShift = -12;
  } else if (octaveV < 300 && octaveV > octavePot->getMin()) {  // Octave pot pressed low. Octave Down
    octaveShift = 12;
  } else {
    octaveShift = 0;
  }
  _noteValue = _tonic + currentScale[index] + octaveShift + accentOffset;  

  lcd->updateNote(_noteValue); // Make the display show the correct note
  return _volume;
}


int Note::findInterval(int mainV, int minimum) {
  // Tuning values for intervals
  const unsigned int SECOND1 = 340;
  const unsigned int THIRD1 = 352;
  const unsigned int FOURTH1 = 365;
  const unsigned int FIFTH1 = 380;
  const unsigned int SIXTH1 = 395;
  const unsigned int SEVENTH1 = 413;
  const unsigned int EIGHTH1 = 431;
  const unsigned int SECOND2 = 450;
  const unsigned int THIRD2 = 474;
  const unsigned int FOURTH2 = 499;
  const unsigned int FIFTH2 = 523;
  const unsigned int SIXTH2 = 563;
  const unsigned int SEVENTH2 = 594;
  const unsigned int EIGHTH2 = 634;
  const unsigned int SECOND3 = 679;
  const unsigned int THIRD3 = 732;
  const unsigned int FOURTH3 = 788;
  const unsigned int FIFTH3 = 847;
  const unsigned int SIXTH3 = 931;
  int intervals[19] = {SECOND1, THIRD1, FOURTH1, FIFTH1, SIXTH1, SEVENTH1, EIGHTH1,
                       SECOND2, THIRD2, FOURTH2, FIFTH2, SIXTH2, SEVENTH2, EIGHTH2, SECOND3, THIRD3, FOURTH3,
                       FIFTH3, SIXTH3};
  if (mainV < minimum) {
    return 0;
  } else {
    int index = 0; // Start indexing at the end
    while (mainV > intervals[index]) { // Loop until we find the correct interval
      index++;
      if (index == sizeof(intervals) / sizeof(intervals[0])) {
        break;      
      }
    }  
    return index + 1;
  } 
}


// FIt a 4th degree polynomial to map the raw sensor output to MIDI outputs betweeen 0 and 16383
int Note::mapPitchBend(Potentiometer* joyX) {
  float x = joyX->getValue();
  float A = -9.82E-9;
  float B = -4.11E-5;
  float C = 7.649E-2;
  float D = -41.4;
  float E = 1.606E4;
  float flippedResult = constrain(A*pow(x, 4) + B*pow(x, 3) + C*pow(x, 2) + D*x + E, 0, 16383);
  return flippedResult;
}


int Note::mapNeckPressure(Potentiometer* neckPot) {
  float x = neckPot->getValue();
  int midMIDI = 63;
  if (x < neckPot->getMin()) {
    return midMIDI;
  }
  float A = 1.13e-9;
  float B = -2.04E-6;
  float C = 1.11E-3;
  float D = -5.408E-2;
  float E = -5.053E-1;
  // Helm's LFO is 0 at the midpoint and goes to maxima at 0 and 127, so map between 63 and 127
  return constrain(A*pow(x, 4) + B*pow(x, 3) + C*pow(x, 2) + D*x + E, midMIDI, 127);
 
}


int Note::getStrumAverage() {
    DataQueue<int> temp = DataQueue<int>(strumValQueue.max_queue_size());
    int sum = 0;
    for (int i = 0; i < strumValQueue.item_count(); i++) {
      int val = strumValQueue.dequeue();
      temp.enqueue(val);
      sum += val;
    }
    strumValQueue = temp;
    return sum / strumValQueue.item_count();
}

int Note::mapStrumPressure(Potentiometer* strumPot) {
  // Smooth the values from the strum pot by averaging the queue
  float readVal = strumPot->getValue();
  if (readVal < strumPot->getMin()) {
    return 0;
  }
  strumValQueue.enqueue(readVal);
  int x = getStrumAverage();
  
  float A = -3.079E-12;
  float B = 7.088E-9;
  float C = -5.084E-6;
  float D = 1.257E-3;
  float E = 1.208E-1;
  float F = -2.019E-1;
  return constrain((A*pow(x, 5) + B*pow(x, 4) + C*pow(x, 3) + D*pow(x,2) + E*x + F) / 3.4, 0, 127);
}


void Note::cycleScale() {
  if (currentScale == majorScale) {
    currentScale = minorScale;
  } else if (currentScale == minorScale) {
    currentScale = chromaticScale;
  } else if (currentScale == bluesScale) {
    currentScale = majorScale;
  } else if (currentScale == chromaticScale) {
    currentScale = bluesScale;
  } 
}

void Note::setSounding(bool value) {
  _sounding = value;
}

void Note::incrementTonic() {
  _tonic += 1;
  updateLCDTonic();
}

void Note::decrementTonic() {
  _tonic -= 1;
  updateLCDTonic();
}

void Note::setLastNote(int noteValue) {
  _lastNote = noteValue;
}

int Note::getTonic() {
  return _tonic;
}

int Note::getPitchBend() {
  return _pitchBend;
}

int Note::getNote() {
  return _noteValue;
}

Potentiometer* Note::getStrummedPot() {
  return _strummedPot;
}

Potentiometer* Note::getLastStrummedPot() {
  return _lastStrummedPot;
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

bool Note::isStrummed() {
  return _strummed;
}
bool Note::isHolding() {
  return _holding;
}

void Note::updateLCDTonic() {
  lcd->setTonic(_tonic);
}
