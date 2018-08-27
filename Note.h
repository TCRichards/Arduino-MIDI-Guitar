#ifndef Note_h
#define Note_h

#include <MIDI.h>
#include "Arduino.h"
#include "Potentiometer.h"

class Note {
  public:
    Note();
    void cycleScale(); 
    void incrementTonic();
    void decrementTonic();
    int getTonic();
    void updateNote(Potentiometer* mainPot, Potentiometer* octavePot);
    int getNote();
    void applyPitchBend(int value);
    void playNote();
    void stopNote();  
    bool isSounding();
    

  private:
    int majorScale[17] = {0, 2, 4, 5, 7, 9, 11, 12, 14, 16, 17, 19, 21, 23, 24, 26, 28};
    int minorScale[17] = {0, 2, 3, 5, 7, 8, 10, 12, 14, 15, 17, 19, 20, 22, 24, 26, 27};
    int chromaticScale[17] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    int* currentScale;
    int _tonic;  //Lowest note (in midi value)
    int _noteValue;
    bool _bothOctaves;
    bool _noteOn;
};


#endif
