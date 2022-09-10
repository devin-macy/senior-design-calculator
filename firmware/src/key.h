#ifndef KEYPAD
#define KEYPAD

// Keypad
const byte ROWS = 4; 
const byte COLS = 4; 

char defaultKeys[ROWS][COLS] = {
  {'1', '2', '3', '+'},
  {'4', '5', '6', '-'},
  {'7', '8', '9', '*'},
  {'C', '0', '=', '/'}
};

char keys1[ROWS][COLS] = {
  {'1', '2', '3', '('},
  {'4', '5', '6', ')'},
  {'7', '8', '9', '.'},
  {'C', '0', '=', '-'}
};

char keys2[ROWS][COLS] = {
  {'1', '2', '3', 'X'},
  {'4', '5', '6', 'X'},
  {'7', '8', '9', 'X'},
  {'C', '0', '=', 'X'}
};

byte rowPins[ROWS] = {9, 8, 7, 6}; 
byte colPins[COLS] = {5, 4, 3, 2};


// End Keypad


#endif