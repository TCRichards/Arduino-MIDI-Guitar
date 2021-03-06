#pragma once
#include <Queue.h>
#include "Arduino.h"
#include "Potentiometer.h"
#include "Display.h"

class Note {
  
  public:
  
    Note(int lcdPin);
    int updateNote(Potentiometer* mainPot, Potentiometer* octavePot, Potentiometer* midStrumPot, Potentiometer* sharpStrumPot, Potentiometer* flatStrumPot, 
      Potentiometer* neckPot, Potentiometer* pitchBendPot);

    int getTonic();
    int getNote();
    int getLastNote();
    int getVibrato();
    int getVolume();
    int getPitchBend();
    int getParam1();
    int getParam2();
    bool isSounding();
    bool isHolding();
    bool isCrackling();
    bool isStrummed();
    void setSounding(bool value);
    void setLastNote(int noteValue);

    void cycleScale(); 
    void incrementTonic();
    void decrementTonic();
    
    void applyPitchBend(int value);
    bool octaves();

    Potentiometer* getStrummedPot();
    Potentiometer* getLastStrummedPot();

    int octaveShift; 
    int lastOctaveShift;

    int findInterval(int mainV, int minimum);

  private:

    Display* lcd;

    int majorScale[21] = {0, 2, 4, 5, 7, 9, 11, 12, 14, 16, 17, 19, 21, 23, 24, 26, 28, 29, 31, 33, 35};
    int minorScale[21] = {0, 2, 3, 5, 7, 8, 10, 12, 14, 15, 17, 19, 20, 22, 24, 26, 27, 29, 31, 32, 34};
    int chromaticScale[21] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
    int bluesScale[21] = {0, 3, 5, 6, 7, 10, 12, 15, 17, 18, 19, 22, 24, 27, 29, 30, 31, 34, 36, 39, 41};
    int* currentScale;
        
    int _tonic;  // Lowest note in current range (in midi value)
    int _lastNote;
    int _noteValue; // MIDI value corresponding to note played
    int _volume; 
    int _vibrato;
    int _param1;
    int _param2;
    int _pitchBend;
    bool _holding;
    bool _sounding;
    bool _crackling;
    bool _strummed;


    Potentiometer* _strummedPot;
    Potentiometer* _lastStrummedPot;
    
    DataQueue<int> strumValQueue;

    int getStrumAverage();

    int mapPitchBend(Potentiometer* joyX);
    int mapNeckPressure(Potentiometer* neckPot);
    int mapStrumPressure(Potentiometer* strumPot);

    void updateLCDTonic();

};
