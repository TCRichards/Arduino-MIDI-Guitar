#ifndef Display_h
#define Display_h
#include <SoftwareSerial.h>

#include "Arduino.h"

class SoftwareSerial;

class Display {
  
  public:
  
    Display(int rx, int tx);
    void shiftTonic(int numHalfSteps);
    void updateNote(int midiValue);
    void setTonic(int tonicValue);
 
  private:
    void backspace();
    void moveCursor(char horiz, boolean firstLine);

    String noteNames[12] = {"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};
    String getTonicName();

    int _curTonic;
    int _curInterval; // Number of half steps from C to the current tonic note
    int _curNote; 
    int _curOctave;   // Displays which piano octave the note lies in
   
    SoftwareSerial lcd;
};


#endif
