#include "Display.h"


Display::Display(int rx, int tx): lcd(rx, tx) {
  SoftwareSerial s(tx, rx);
  lcd = s;

  lcd.begin(19200);
  lcd.write(12);    // Clears the current display
  lcd.write(22);  // Makes cursor invisible
  lcd.print("Tonic: C");
  _curInterval = 0;
  _curOctave = 4; // Octave containing middle C
  _curNote = 0;
}

// Cursor should begin at end of line 2
// Edit current note accordingly
// Move cursor back to end of line 2
void Display::updateNote(int midiValue) {
  // Don't do anything if the note hasn't changed
  if (midiValue == _curNote) {
    return;
  }

  moveCursor(9, true);
  int i = 0;
  while (i < 3) { // Delete the last note (may be 1 or 2 characters depending on note name)
    backspace();
    lcd.print(' ');
    backspace();
    i++;
  }
  _curNote = midiValue; // The distance from C is the tonic's distance plus the interval's distance from tonic
  lcd.print(noteNames[midiValue % 12]); // Print the new note
}

void Display::backspace() {
  lcd.write(8);
}

// Move cursor to line 1
// Edit the tonic note accordingly
// Move cursor back to end of line 2
void Display::shiftTonic(int numHalfSteps) {
  _curTonic += numHalfSteps;
  int pos = 7 + noteNames[_curTonic].length();
  moveCursor(pos, true);
  for (int i = 0; i < noteNames[_curNote].length(); i++) {
    backspace();
  }
  _curTonic = (_curTonic + numHalfSteps) % 12;
  if (_curTonic < 0) { // Because C doesn't count this for modulus :(
    _curTonic = 11;
  }
  lcd.print(noteNames[_curNote]);
  //  moveCursor(15, false);
}

// Moves the cursor to one of the 16 allowed postionsu
// @Param pos = horizontal position (hexadecimal), which line to write to
// Use Hexadecimal characters for 0 - 15 range
void Display::moveCursor(char horiz, boolean firstLine) {
  if (firstLine) {
    lcd.write(map(horiz, 0, 15, 128, 143));
  } else {
    lcd.write(map(horiz, 0, 15, 148, 163));
  }
}

void Display::setTonic(int tonicValue) {
  _curTonic = tonicValue;
}

String Display::getTonicName() {
  return noteNames[_curTonic % 12]; 
}
