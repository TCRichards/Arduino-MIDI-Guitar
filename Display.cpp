#include "Display.h"
#include <SoftwareSerial.h>


Display::Display(int rx, int tx):lcd(rx, tx) {
  SoftwareSerial s(51,53);
  lcd = s;
  
  lcd.begin(19200);
  lcd.write(12); // Clears the current display
 // lcd.write(22); // Makes cursor invisible
  lcd.print("Tonic: C");
  lcd.write(13); // Moves the cursor to the next row
  lcd.print("Current Note: C");

  _curInterval = 0;
  _curOctave = 4; // Octave containing middle C
  _curTonic = 0; // Index in noteNames of tonic note
  _curNote = 0;
}

// Cursor should begin at end of line 2
// Edit current note accordingly
// Move cursor back to end of line 2
void Display::updateNote(int interval) {

  // Don't do anything if the note hasn't changed
  if (_curNote == (_curTonic + interval) % 12) {
    return;
  }
  int i = 0;
  while (i < noteNames[_curNote].length()) { // Delete the last note (may be 1 or 2 characters depending on note name)
    backspace();
    i++;
  }
  int tempInterval = (_curInterval + interval);
  if (tempInterval > 12) {
    _curOctave++;
  }
  _curNote = (_curTonic + interval) % 12; // The distance from C is the tonic's distance plus the interval's distance from tonic  
  lcd.print(noteNames[_curNote]); // Print the new note  
}

void Display::backspace() {
  lcd.write(8);
}

// Move cursor to line 1
// Edit the tonic note accordingly
// Move cursor back to end of line 2
void Display::shiftTonic(int numHalfSteps) {
  int pos = 7 + noteNames[_curTonic].length();
  moveCursor(pos, true);
  for (int i = 0; i < noteNames[_curTonic].length(); i++) {
    backspace();
  }
  _curTonic = (_curTonic + numHalfSteps) % 12;
  if (_curTonic < 0) { // Because C doesn't count this for modulus :(
    _curTonic = 11;
  }
  lcd.print(noteNames[_curInterval]);
 // moveCursor(15, false);
}

// Moves the cursor to one of the 16 allowed postionsu
// @Param pos = horizontal position (hexadecimal), which line to write to 
// Use Hexadecimal characters for 0 - 15 range
void Display::moveCursor(int horiz, boolean firstLine) {
  if (firstLine) {
    lcd.write(map(horiz, 0, 15, 128, 143)); 
    Serial.println(map(horiz, 0, 15, 128, 143));
  } else {
    lcd.write(map(horiz, 0, 15, 148, 163));
  }

}
