#ifndef KEYPAD
#define KEYPAD

#include <Arduino.h>
#include <Keypad.h>

// Keypad
const byte ROWS = 4; 
const byte COLS = 4; 

const String DEF_KEYS = "+-*/";
char defaultKeys[ROWS][COLS] = {
  {'1', '2', '3', '+'},
  {'4', '5', '6', '-'},
  {'7', '8', '9', '*'},
  {'C', '0', '=', '/'}
};

const byte KEY1 = 12;
const String KEYS_1 = "().-";
char keys1[ROWS][COLS] = {
  {'1', '2', '3', '('},
  {'4', '5', '6', ')'},
  {'7', '8', '9', '.'},
  {'C', '0', '=', '-'}
};

const byte KEY2 = 13;
const String KEYS_2 = "^XXX";
char keys2[ROWS][COLS] = {
  {'1', '2', '3', 'X'},
  {'4', '5', '6', 'X'},
  {'7', '8', '9', 'X'},
  {'C', '0', '=', 'X'}
};

byte rowPins[ROWS] = {9, 8, 7, 6}; 
byte colPins[COLS] = {5, 4, 3, 2};

Keypad defaultKeypad = Keypad(makeKeymap(defaultKeys), rowPins, colPins, ROWS, COLS);
Keypad keypad1 = Keypad(makeKeymap(keys1), rowPins, colPins, ROWS, COLS);
Keypad keypad2 = Keypad(makeKeymap(keys2), rowPins, colPins, ROWS, COLS);

#endif //End