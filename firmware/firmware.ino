//OHIO UNIVERSITY
//FALL2022-2023
//Devin Macy, Dominic Benintendi, Husain Bani Orada, Colin Russel

#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include "./src/calculator.h"
#include "./src/key.h"
#include "./src/util.h"

// Lexer Class
class CalcLexer
{
  public:
  enum TokenType
  {
    END,
    NUMBER,
    LEFT_PAR = '(',
    RIGHT_PAR = ')',
    ADD = '+',
    SUB = '-',
    MUL = '*',
    DIV = '/',
  };
  private:
    float curNumber;
    String curExp;
    TokenType curToken;
  public:
  
  float getNumber() const
    {
      return curNumber;
    }
 
  TokenType getCurTok() const
    {
        return curToken;
    }
  void setExpression(const String &exp)
    {
      curExp = exp;
    }
  void printData()
  {
    Serial.print('\n');
  Serial.print("Current Expression: ");
    Serial.print(curExp);
    Serial.print('\n');
    Serial.print("What type of token is it?: ");
    if(curToken != CalcLexer::TokenType::NUMBER)
    Serial.print((char)curToken);
    else
    Serial.print("NUM");
    Serial.print('\n');
    Serial.print("Any associated Numbers?: ");
    Serial.print(curNumber);

  }
  
  CalcLexer::TokenType findNext() 
    {
    curNumber = 0;
      if (curExp.length() == 0)
      {
        curToken = TokenType::END;
        return curToken;
      }
      char temp = curExp.charAt(0);
    
 //     Serial.print('\n');
 //     Serial.print("Current Char: ");
 //     Serial.print(temp);
    
      curExp.remove(0,1);
      
      switch (temp)
      {
        case '(': return curToken = TokenType::LEFT_PAR;
        case ')': return curToken = TokenType::RIGHT_PAR;
        case '/': return curToken = TokenType::DIV;
        case '*': return curToken = TokenType::MUL;
        case '-': return curToken = TokenType::SUB;
        case '+': return curToken = TokenType::ADD;
      }
      String stringNumber = "";
      if (isDigit(temp))
      {
        stringNumber += temp;
        while (!curExp.length() == 0 &&
               (isDigit(curExp.charAt(0)) || curExp.charAt(0) == '.'))
        {
          stringNumber += curExp.charAt(0);
          curExp.remove(0,1);
        }
        curNumber = stringNumber.toFloat();
        curToken = TokenType::NUMBER;
        return curToken;
      }
      else
      {
        Serial.print("Error");
      }
    
    }
};
// End Lexer
// Start Parser
class CalcParser
{
  public:
   CalcParser(CalcLexer* curlex)
     {
       lex = curlex;
     }
    // Usually returns the a number for the next token
    // Handles edge cases like Negative Numbers and Parenthasis
    // Top of the mountain, Gets to go first
    float parseExpression()
    {
      lex->findNext();
      switch(lex->getCurTok())
      {
        // Simply returns the number, This is our end case. all expression will end here eventially
        case CalcLexer::NUMBER:
        {
          float curNum = lex->getNumber();
          lex->findNext();
//          Serial.print('\n');
//          Serial.print("Our raw::number is: ");
//          Serial.print(curNum);
//          Serial.print('\n');
          return curNum;
          
        }
        case CalcLexer::SUB:// Edge Case for handling Negative Numbers
          return -1 * parseExpression();
        // Edge Case for handling Parenthasis, If you find a left par, Solve the Expression between it and the right par
        case CalcLexer::LEFT_PAR:
        {
          float curNum = parseAddSub();
          if (lex->getCurTok() != CalcLexer::TokenType::RIGHT_PAR)
          {
            Serial.print('\n');
            Serial.print("Error:: No right par");
          }
          lex->findNext();
          return curNum;
        }
        default:
        {
          Serial.print('\n');
          Serial.print(lex->getCurTok());
          Serial.print("Error:: Invalid Expression");
        }
      }
    }
    // middle of the mountain, Assuming all conditions are met, this div/mul get to go second, before findExpression, which handles edge cases
    // This function parses for MUL/DIV Tokens
    float parseDivMul()
    {
      // Number at the left side of our equation
      float curNum = parseExpression();
      // Finds the right side of our equation, and uses the appropriate arithamtic, edge case for divided by 0
      while (true)
      {
        switch(lex->getCurTok())
        {
          case CalcLexer::TokenType::MUL:// Find Right side, and Multiply
          {
            curNum *= parseExpression();
            break;
          }
          case CalcLexer::TokenType::DIV:
          {
            // Find Right side, and Divide
            float temp = parseExpression();
            if(temp != 0.0)
            {
              curNum /= temp;
              break;
            }
            else
            {
              Serial.print('\n');
              Serial.print("Error:: Cannot Divide by 0");
              break;
            }
          }
          default: // No multiply or Div tokens found.. So just return back to parseAddSub() to wince you where called
            return curNum;
        }
      } 
            
    }
    // bottom of the mountain, this is where we start parsing every Expression and sub expression
    // Assuming there are no mul, div, or edge cases This goes 3rd
    // This function parses for ADD/SUB Tokens
    float parseAddSub()
    {
      // Number at the left side of our equation, Solve using Mul/Div to keep PEMDAS intact
      float curNum = parseDivMul();
//      Serial.print('\n');
//      Serial.print("Our left::addsub is: ");
//      Serial.print(curNum);
//      Serial.print('\n');
 
      // Finds the right side of our equation, and uses the appropriate arithamtic, edge case for divided by 0
      while (true)
      {
        switch(lex->getCurTok())
        {
          case CalcLexer::TokenType::ADD:
          {
            curNum += parseDivMul();
            break;
          }
          case CalcLexer::TokenType::SUB:
          {
            curNum -= parseDivMul();
            break;
          }
          default:// No ADD/SUB tokens founds, Probly means we are at the end of a expression or end of a parenthasis
          {
//            Serial.print('\n');
//            Serial.print("Our final::addsub is: ");
//            Serial.print(curNum);
//            Serial.print('\n');
            return curNum;
          }
        }
      }
    }
    float calc()
    {
      return parseAddSub();
    }
  
  private:
    CalcLexer* lex = NULL;
};
// End Parser


String expression = "";
byte expLength = 0;

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
