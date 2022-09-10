// OHIO UNIVERSITY
// Fall 2022-2023
// 4-Function Calculator
// Devin Macy, Dominic Benintendi, Husain Bani Orada, Collin Russel

#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <./src/calculator.h>
#include <./src/key.h>
#include <./src/util.h>

// Lexer
class CalcLexer
{
  public:
  enum TokenType
  {
    END,
    NUMBER,
    LEFT_PAR = '(',
    RIGHT_PAR = ')',
    PLUS = '+',
    MINUS = '-',
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
 
  TokenType getCur() const
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
    
      Serial.print('\n');
      Serial.print("Current Char: ");
      Serial.print(temp);
    
      curExp.remove(0,1);
      
      switch (temp)
      {
        case '(': return curToken = TokenType::LEFT_PAR;
        case ')': return curToken = TokenType::RIGHT_PAR;
        case '/': return curToken = TokenType::DIV;
        case '*': return curToken = TokenType::MUL;
        case '-': return curToken = TokenType::MINUS;
        case '+': return curToken = TokenType::PLUS;
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
// Parser $ && Lexer Stuff that dom is working on 
// Feel free to ask questions

//Tinkercad uses 27, Nano uses 0x27
LiquidCrystal_I2C lcd(0x27, 16, 2);
void setup()
{
    // Set the LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Hello World");
  
  Serial.begin(9600);
  CalcLexer lex;
  String startingexp = "2+3*4";
  lex.setExpression(startingexp);
  Serial.print('\n');
  Serial.print("booting.... Welcome to our Calculator");
  Serial.print('\n');
  Serial.print("Our expression is: ");
  Serial.print(startingexp);
  CalcLexer::TokenType temp;
  while (lex.findNext() != 0)
  {
    lex.printData();
  }
}

void loop()
{
}
