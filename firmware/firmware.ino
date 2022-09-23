#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
// 9/7/22
// EE Capstone CPR2 Tinker Cad Tests
// Main
#define HISTORY_MAX 10 // Dont go above 10 if you can help it
String history[HISTORY_MAX];
byte history_len = 0;
byte curr_exp = 0;
byte iterator = 0;
// Current Expression information
LiquidCrystal_I2C lcd(0x27, 16, 2);
// Custom Chars
byte up_arrow[] = {
 B00100,
 B01110,
 B10101,
 B00100,
 B00100,
 B00100,
 B00100,
 B00000
};

byte down_arrow[] = {
 B00000,
 B00100,
 B00100,
 B00100,
 B00100,
 B10101,
 B01110,
 B00100
};

byte left_arrow[] = {
 B00000,
 B00100,
 B01000,
 B11111,
 B01000,
 B00100,
 B00000,
 B00000
};

byte right_arrow[] = {
 B00000,
 B00100,
 B00010,
 B11111,
 B00010,
 B00100,
 B00000,
 B00000 
};

const byte ROWS = 4; 
const byte COLS = 4;
// Mode 1
const String DEF_KEYS = "+-*/";
char defaultKeys[ROWS][COLS] = {
  {'1', '2', '3', '+'},
  {'4', '5', '6', '-'},
  {'7', '8', '9', '*'},
  {'A', '0', '=', '/'}
};
// Mode 2
const byte KEY1 = 10;
const String KEYS_1 = "().^";
char keys1[ROWS][COLS] = {
  {'1', '2', '3', '('},
  {'4', '5', '6', ')'},
  {'7', '8', '9', '.'},
  {'C', '0', '=', '^'}
};
// Mode 3
const byte KEY2 = 11;
const String KEYS_2 = "";
char keys2[ROWS][COLS] = {
  {'1', '2', '3', 'u'},
  {'4', '5', '6', 'd'},
  {'7', '8', '9', 'l'},
  {'D', '0', 'I', 'r'}
};

// Pin Settings
byte rowPins[ROWS] = {9, 8, 7, 6}; 
byte colPins[COLS] = {5, 4, 3, 2};

// Keypads
Keypad defaultKeypad = Keypad(makeKeymap(defaultKeys), rowPins, colPins, ROWS, COLS);
Keypad keypad1 = Keypad(makeKeymap(keys1), rowPins, colPins, ROWS, COLS);
Keypad keypad2 = Keypad(makeKeymap(keys2), rowPins, colPins, ROWS, COLS);
// Keypad Controller
// End Keypads


// Start Lexer
class CalcLexer
{
  public:
  // Our token Object
  enum Token
  {
    NUM,
    LEFT_PAR = '(',
    RIGHT_PAR = ')',
    ADD = '+',
    SUB = '-',
    MUL = '*',
    DIV = '/',
    POW = '^',
    END,
  };
  private:
    float curNumber;
    String curExp;
    Token curToken;
  public:
  
  float getNumber()
    {
      return curNumber;
    }
 
  Token getCurTok()
    {
     	 return curToken;
    }
  void setExp(String exp)
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
    if(curToken != CalcLexer::Token::NUM)
    Serial.print((char)curToken);
    else
    Serial.print("NUM");
    Serial.print('\n');
    Serial.print("Any associated Numbers?: ");
    Serial.print(curNumber);

  }
  
  CalcLexer::Token findNext(String &e) 
  {
    curNumber = 0;
    if (curExp.length() == 0)
    {
      // Serial.print("Reached END TOKEN");
      curToken = Token::END;
      return curToken;
    }
    char temp = curExp.charAt(0);
    curExp.remove(0,1);
    switch (temp)
    {
      case '(': 
      {
        curToken = Token::LEFT_PAR;
        return curToken
      }
      case ')':
      {
        curToken = Token::RIGHT_PAR;
        return curToken
      }
      case '/':
      {
        curToken = Token::DIV;
        return curToken
      }
      case '*':
      {
        curToken = Token::MUL;
        return curToken
      }
      case '-':
      {
        curToken = Token::SUB;
        return curToken
      }
      case '+':
      {
        curToken = Token::ADD;
        return curToken
      }
      case '^':
      {
        curToken = Token::POW;
        return curToken
      }
    }
    String stringNumber = "";
    if (isDigit(temp))
    {
      stringNumber += temp;
      bool foundDecimal = false;
      while ((isDigit(curExp.charAt(0)) || curExp.charAt(0) == '.'))
      {
        if(!foundDecimal)
        {
          stringNumber += curExp.charAt(0);
          curExp.remove(0,1);
          foundDecimal = true;
        }
        else
        {
          e = "ERR:DECM SYTX";
          break;
        }
        
      }
      curNumber = stringNumber.toFloat();
      curToken = Token::NUM;
      return curToken;
    }
    else
    {
      e = "ERR:SYNTAX";
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
     CalcParser(String exp)
     {
       lex = new CalcLexer();
       lex->setExpression(exp);
     }
  	 ~CalcParser()
     {
       if(lex);
       delete(lex);
     }
    // Usually returns the a number for the next token
    // Handles edge cases like Negative Numbers and Parenthasis
  	// Top of the mountain, Gets to go first
    float parseEdge(String &e)
    {
      lex->findNext(e);
      switch(lex->getCurTok())
      {
        // Simply returns the number, This is our end case. all expression will end here eventially
        case CalcLexer::NUM:
        {
          float curNum = lex->getNumber();
          lex->findNext(e);
          return curNum;
          
        }
      	case CalcLexer::SUB:// Edge Case for handling Negative Numbers
          return (parseEdge(e) * -1);
      	case CalcLexer::LEFT_PAR:// Edge Case for handling Parenthasis, If you find a left par, Solve the Expression between it and the right par
        {
          float curNum = parseAddSub(e);
          if (lex->getCurTok() == CalcLexer::Token::RIGHT_PAR)
          {
            lex->findNext(e);
            return curNum;
          }
          else
          {
            Serial.print('\n');
            Serial.print("Parser::Error:: No right par");
            e = "ERR:NO R_PAR";
            return curNum;
          }
          
        }
      	default:
        {
          	e = "ERR:INV EXP";
        }
      }
      
    }
  	// Middle-top of the moutain, Handels POW(x,y) function
  	float parsePow(String&e)
    {
      float curNum = parseEdge(e);
       while(true)
      {
        switch(lex->getCurTok())
        {
          case CalcLexer::Token::POW:
          {
            curNum = pow(curNum,parseEdge(e));
            break;
          }
          default: // No multiply or Div tokens found.. So just return back to parseAddSub() to wince you where called
            return curNum;
        }
      }
      
    }
  	// middle of the mountain, Assuming all conditions are met, this div/mul get to go second, before findExpression, which handles edge cases
    // This function parses for MUL/DIV Tokens
    float parseDivMul(String &e)
    {
      // Number at the left side of our equation
      float curNum = parsePow(e);
      // Finds the right side of our equation, and uses the appropriate arithamtic, edge case for divided by 0
      while (true)
      {
        switch(lex->getCurTok())
      	{
          case CalcLexer::Token::DIV:
          {
            // Find Right side, and Divide
            float temp = parsePow(e);
            if(temp != 0.0)
            {
              curNum = (curNum / temp);
              break;
            }
            else
            {
              e = "ERR:DIV BY 0";
              break;
            }
          }
          case CalcLexer::Token::MUL:// Find Right side, and Multiply
          {
            curNum *= parsePow(e);
            break;
          }
          default: // No multiply or Div tokens found.. So just return back to parseAddSub() to wince you where called
            return curNum;
        }
      } 
            
    }
  	// bottom of the mountain, this is where we start parsing every Expression and sub expression
  	// Assuming there are no mul, div, or edge cases This goes 3rd looking for ADD/SUB Tokens
    float parseAddSub(String & e)
    {
      // Number at the left side of our equation, Solve using Mul/Div to keep PEMDAS intact
      float curNum = parseDivMul(e);
      // Finds the right side of our equation, and uses the appropriate arithamtic, edge case for divided by 0
      while (true)
      {
        switch(lex->getCurTok())
      	{
          case CalcLexer::Token::ADD:
          {
            curNum += parseDivMul(e);
            break;
          }
          case CalcLexer::Token::SUB:
          {
            curNum -= parseDivMul(e);
            break;
          }
          default:// No ADD/SUB tokens founds, Probly means we are at the end of a expression or end of a parenthasis
          {
            return curNum;
          }
        }
      }
    }
    String calc()
    {
      String e = "";
      float solution = parseAddSub(e);
      Serial.print(e);
      if(e == "")
      {
        return (String)solution;
      }
      else
      {
        return e;
      }
    }
  
  private:
  	CalcLexer* lex = NULL;
};
// End Parser

// KeypadController Class
class KeypadController
{
  public:
  enum mode
  {
    MODE1,
    MODE2,
    MODE3,
    MODE4,
  };
  KeypadController(Keypad &a,Keypad &b,Keypad &c,Keypad &d)
  {
    keypad1 = &a;
    keypad2 = &b;
    keypad3 = &c;
    keypad4 = &d;
  }
  void changeMode(bool key1, bool key2)
  {
    
    if(key1 && key2){
      if (cur_mode != MODE1)
      {
        is_mode_changed = true;
      }
      cur_mode = MODE1;
    } else if(key1){
      if (cur_mode != MODE2)
      {
        is_mode_changed = true;
      }
      cur_mode = MODE2;
    } else if(key2){
      if (cur_mode != MODE3)
      {
        is_mode_changed = true;
      }
      cur_mode = MODE3;
    } else{
      if (cur_mode != MODE4)
      {
        is_mode_changed = true;
      }
      cur_mode = MODE4;
    }
  }
  bool isModeChanged()
  {
    bool temp = is_mode_changed;
    is_mode_changed = false;
    return temp;
  }
  char getKeys()
  {
    switch(cur_mode)
    {
      case KeypadController::MODE1:{return keypad1->getKey(); break;}
      case KeypadController::MODE2:{return keypad2->getKey(); break;}
      case KeypadController::MODE3:{return keypad3->getKey(); break;}
      case KeypadController::MODE4:{return keypad4->getKey(); break;}
    }
  }
  KeypadController::mode getCurMode()
  {
    return cur_mode;
  }
    
  	
  private:
  	bool is_mode_changed = false;
    KeypadController::mode cur_mode = MODE1;
    Keypad* keypad1;
    Keypad* keypad2;
    Keypad* keypad3;
    Keypad* keypad4;
  
};
class LcdController
{
  public:
  	LcdController(LiquidCrystal_I2C & curlcd,String keys1, String keys2, String keys3, String keys4)
    {
      lcd = & curlcd;
      KEYS1 = keys1;
      KEYS2 = keys2;
      KEYS3 = keys3;
      KEYS4 = keys4;
      Serial.print(KEYS2);
      Serial.print(KEYS1);
    }
  	// init function
    void startLCD()
    {
      // Setup LCD
      lcd->init();
      lcd->backlight();
      lcd->clear();


      // Display welcome message
      lcd->setCursor(1,0);
      lcd->print("Welcome to our");
      lcd->setCursor(3,1);
      lcd->print("Calculator");
      delay(1500);
      // Dope as hell scroll effect
      for (int i = 0; i < 16; i++)
      {
        lcd->scrollDisplayLeft();
        delay(100);
      }
      // Set cursor at starting position
      lcd->clear();
      lcd->setCursor(0,0);
      lcd->blink();
      
      // Create custom characters
      lcd->createChar(0, up_arrow);
      lcd->createChar(1, down_arrow);
      lcd->createChar(2, left_arrow);
      lcd->createChar(3, right_arrow);
      
      // Display Keybindings
      printKeys(KeypadController::mode::MODE1);
  	}
  	
  
  	// Prints our Keybindings
  	void printKeys(KeypadController::mode cur_mode)
    {
    lcd->setCursor(14,0);
    switch(cur_mode)
      {
        case KeypadController::MODE1:
      	{
          lcd->print(KEYS1.substring(0,2));
          lcd->setCursor(14,1);
          lcd->print(KEYS1.substring(2));
          break;
      	}
        case KeypadController::MODE2:
      	{
          lcd->print(KEYS_1.substring(0,2));
          lcd->setCursor(14,1);
          lcd->print(KEYS_1.substring(2));
          break;
      	}
     	case KeypadController::MODE3:
      	{
          lcd->write(0);
          lcd->write(1);
          lcd->setCursor(14,1);
          lcd->write(2);
          lcd->write(3);
          break;
      	}
    	case KeypadController::MODE4:
      	{
          lcd->print(KEYS4.substring(0,2));
          lcd->setCursor(14,1);
          lcd->print(KEYS4.substring(2));
          break;
      	}
      }
      last_known_mode = cur_mode;
  	  setCursor(curX,curY);
    }
  	// Handles our Input
    void handle_input(char &input)
    {

      if(showing_awnser)
      {
        clear();
        curExp+=1;
        showing_awnser = false;
      }
      
      CalcParser* parse = new CalcParser(expression);

      switch(input){
        case 'u':{
          //Up arrow - move through history
          if(curr_exp < history_len-1){
            curr_exp += 1;
          }
          iterator = 0;
          expression = history[history_len-(curr_exp+1)];
          clear();
          printLineOne(expression);
          break;
        }
        case 'd':{
          //Down arrow - move through history
          if(curr_exp > 0){
            curr_exp -= 1;
          }
          iterator = 0;
          expression = history[history_len-(curr_exp+1)];
          clear();
          printLineOne(expression);
          break;
        }
        case 'l':{
          //Left arrow - move through expression
          if((iterator < expLength) && (iterator > 0)){
           moveCurLeft(); 
           iterator += 1;
          }
          break;
        }
        case 'r':{
          //Right arrow - move through expression
          if((iterator > 0) && (iterator < expLength)){
           moveCurRight(); 
           iterator -= 1;
          }
          break;
        }
        case 'A':{
          //Clear all
          resetExpression();
          answer = "";
          clear();
          break;
        }
        case 'I':{
          //Insert at cursor in expression
          break;
        }
        case 'D':{
          //Delete at cursor in expression
          expression.remove(expLength-iterator, 1);
          expLength = expression.length();
          clear();
          printLineOne(expression);
          break; 
        }
        case 'C':{
          
          //Clear just expression
          resetExpression();
          clear();
          printLineTwo(answer);
          showing_awnser = true;
          break;
        }
        case '=':{
          //History
          answer = parse->calc();
          Serial.print("Solution found");
      	  Serial.print(answer);
          if(history_len < HISTORY_MAX){
          	history[history_len] = expression;
         	history_len += 1;
          } else{
            //This is naive and I don't like it but works
            //We need a FIFO buffer
            String temp[HISTORY_MAX];
            temp[0] = expression;
            for(byte i = 1; i < HISTORY_MAX; i++){
				temp[i] = history[i-1];
            }
            for(byte i = 0; i < HISTORY_MAX; i++){
              history[i] = temp[i];
            }
          }
          expression = "";
          printLineTwo(answer);
          showing_awnser = true;
          break;
        }
        default:{
          expression += input;
      	  expLength = expression.length();
          // If Expression is less that 13, Tack on our input
          if(expLength <= 13){
            setCursor(expLength-1,0);
            printChar(input);
          // If Expression is more than 13, Slide over the digits
          } else {
            printLineOne(expression);
          }
        }

      }
      if (parse)
      delete(parse);

    }
  	// Sets cursor to end of current expression
  	void resetCursorToExpression()
    {
      setCursor(expLength,0);
    }
  	void resetExpression()  	// Resets Our Expression to empty
    {
      expression = "";
      expLength = 0;
    }
    void printLineOne(String s)// Prints String on First Row of LCD (Left Respecting), Slides extra digits
    {
      int s_length = s.length();
      lcd->setCursor(0,0);
      if(s_length < 13)
      {
         lcd->print(s);
      }
      else if(s_length >= 13)
      {
         lcd->print(s.substring(s_length-13));
      }
      setCursor(curX,curY);
    }
  	void printLineTwo(String s)//Prints String on Second Row of LCD (Right Respecting)
    {
      int s_length = s.length();
      
      if(s_length < 13)
      {
        Serial.print(13-s_length);
         setCursor(13-s_length,1);
         lcd->print(s);
      }
      else if(s_length >= 13)
      {
         setCursor(0,1);
         lcd->print(s.substring(s_length-13));
      }
      resetCursorToExpression();
      
    }
  	void printChar(char c) // Prints a single char and records the change in cursor pos
    {
      lcd->print(c);
      curX += 1;
    }
    void clear()// Clear and redraw keybindings
    {
      lcd->clear();
      printKeys(last_known_mode);
      if(expLength == 0) setCursor(0,0);
      else setCursor((expLength-1) % 13,0);
    }
  	void setCursor(int x, int y)// Sets cursor to x, y, Records to CurX CurY
    {
      lcd->setCursor(x,y);
      curX = x;
      curY = y;
    }
  
    void moveCurUp(){setCursor(curX,curY-1);}
  	void moveCurDown(){setCursor(curX,curY+1);}
  	void moveCurLeft(){setCursor(curX-1,curY);}
  	void moveCurRight(){setCursor(curX+1,curY);}
  	
  private:
  	byte curX; // Cursor Cords
  	byte curY;
  	String expression = ""; // Current Expression
  	byte expLength = 0; // Cur Expression Length
  	byte curExp = 0;// How many expressions have we calculated
    String answer = ""; // cur answer
    bool showing_awnser = false;// Is true if the calculator is displaying an awnser
    // Last known mode, So we dont have to recheck everytime we want to redraw our Keybindings
  	KeypadController::mode last_known_mode;
  	LiquidCrystal_I2C *lcd;  	// lcd reference
    String KEYS1;// Keybinding Strings
    String KEYS2;
    String KEYS3;
    String KEYS4;
};
LcdController lcdC(lcd,DEF_KEYS,KEYS_1,KEYS_2,DEF_KEYS);
KeypadController keypadC(defaultKeypad,keypad1,keypad2,defaultKeypad);

void setup() // Main
{
  // Turning on Serial and I2C
  Serial.begin(9600);
  Wire.setClock(1000000);
  // Setting push button inputs
  pinMode(KEY1, INPUT_PULLUP);
  pinMode(KEY2, INPUT_PULLUP);
  // LCD startup
  lcdC.startLCD();
}
void loop(){
  // Keypad select
  bool key1 = digitalRead(KEY1);
  bool key2 = digitalRead(KEY2);
  // Keypad display handling
  keypadC.changeMode(key1,key2);
  char input = keypadC.getKeys();
  // Input handling
  if(input){lcdC.handle_input(input);}
  if(keypadC.isModeChanged()){lcdC.printKeys(keypadC.getCurMode());}
}