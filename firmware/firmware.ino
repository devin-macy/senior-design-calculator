//OHIO UNIVERSITY
//FALL2022-2023
//4-Function Calculator
//Devin Macy, Dominic Benintendi, Husain Bani Orada, Colin Russel

#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include "./src/calculator.h"
#include "./src/keypad.h"
#include "./src/lcd.h"

bool diff_key1 = 0;
bool diff_key2 = 0;

void setup()
{
  // Turning on Serial and I2C
  Serial.begin(9600);
  Wire.setClock(1000000);
  
  // Setting push button inputs
  pinMode(KEY1, INPUT);
  pinMode(KEY2, INPUT);
  
  // LCD startup
  bool trash = 0;
  startLCD();
  print_keys(trash,trash);
}


void loop()
{
  // Keypad select
  bool key1 = digitalRead(KEY1);
  bool key2 = digitalRead(KEY2);
  
  // Keypad display handling
  if(key1 != diff_key1){
  	print_keys(key1,key2);
    diff_key1 = key1;
  }
  
  if(key2 != diff_key2){
   	print_keys(key1,key2);
    diff_key2 = key2;
  }
  
  // Get input from keypad
  char input;
  if(key1 && key2){
	input = defaultKeypad.getKey();
  } else if(key1){
    input = keypad1.getKey();
  } else if(key2){
    input = keypad2.getKey();
  } else{
    input = defaultKeypad.getKey();
  }
  
  // Input handling
  if(input){
    handle_input(input,key1,key2);
  }
  
}
