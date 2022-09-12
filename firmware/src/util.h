#ifndef UTILITY
#define UTILITY
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

void startLCD(){
  // Setup LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  
  // Display welcome message
  lcd.setCursor(1,0);
  lcd.print("Welcome to our");
  lcd.setCursor(3,1);
  lcd.print("Calculator");
  delay(1500);
  
  // Set cursor at starting position
  lcd.clear();
  lcd.home();
  lcd.blink();
}

void print_keys(bool &key1, bool &key2){
  lcd.setCursor(14,0);
  if(key1 && key2){
	lcd.print(DEF_KEYS.substring(0,2));
    lcd.setCursor(14,1);
    lcd.print(DEF_KEYS.substring(2));
              
  } else if(key1){
    lcd.print(KEYS_1.substring(0,2));
    lcd.setCursor(14,1);
    lcd.print(KEYS_1.substring(2));
              
  } else if(key2){
    lcd.print(KEYS_2.substring(0,2));
    lcd.setCursor(14,1);
    lcd.print(KEYS_2.substring(2));
              
  } else{
    lcd.print(DEF_KEYS.substring(0,2));
    lcd.setCursor(14,1);
    lcd.print(DEF_KEYS.substring(2));
  }
  
  lcd.home();
  
}

void handle_input(char &input,bool &key1,bool &key2){
  
  expression += input;
  expLength = expression.length();
  
  CalcLexer* lex = new CalcLexer();
  lex->setExpression(expression);
  CalcParser* parse = new CalcParser(lex);
  
  switch(input){
    case 'C':{
      lcd.clear();
      print_keys(key1,key2);
      expression = "";
      break;
    }
    case '=':{
      lcd.clear();
      print_keys(key1,key2);
      expression = String(parse->calc());
      lcd.print(expression);
      break;
    }
    default:{
      lcd.clear();
      print_keys(key1,key2);
  	  if(expLength < 13){
  	  	lcd.print(expression);
      }else if(expLength >= 13){
   	    lcd.print(expression.substring(0,13));
        lcd.setCursor(0,1);
        lcd.print(expression.substring(13));
  	  }
    }
    
  }
  
}





#endif