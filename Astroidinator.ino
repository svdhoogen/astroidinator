#include <timer.h> // Timer library.
#include <Wire.h> // Include wire library, which facilitates serial communication with lcd.
#include <LiquidCrystal_I2C.h> // Include I2C library for lcd screen.

LiquidCrystal_I2C lcd4x20Shoo(0x3F, 20, 4); // Initialize new object called 'lcd', that will control what is displayed on lcd.

Timer tmrInputsShoo; // Define timer to read joystick inputs.
Timer tmrGameShoo; // Define timer to tick.

#define aiJoyXValShoo   A2  // X val on joy.
#define aiJoyYValShoo   A3  // Y val on joy.
#define diJoyPressShoo  2   // Define button on joystick.
#define doBuzzerShoo    3   // Define buzzer to corresponding pin.

int iCursorPositionShoo[] = {15, 1}; // Where cursor is positioned.
int iCurrentGameScreenShoo = 0; // Default screen is start game, 1 = difficulty, 2 = enter name, 3 = game, 4 = post-game, 5 = highscore.
int iCursorBoundsYSlin[] = {1, 2}; // Cursor boundaries for y.
int iGameDifficulty = 0; // Game difficulty.

unsigned int uiGameTimerStartShoo = 0;

void setup() {
  pinMode(diJoyPressShoo, INPUT); // Initialize joystick button.
  pinMode(doBuzzerShoo, OUTPUT); // Set buzzer output.
  digitalWrite(diJoyPressShoo, HIGH); // Write joystick button high(default, unpressed value is high).
  
  Serial.begin(115200); // Open serial console.
  lcd4x20Shoo.init();
  lcd4x20Shoo.setBacklight(1);
  lcd4x20Shoo.clear();
  
  MethodWriteToLcdShoo(0, 0, "Astroidinator! V0.1");
  MethodWriteToLcdShoo(0, 1, "Made by: Stijn");
  MethodWriteToLcdShoo(0, 2, "Lingmont & Sander vd");
  MethodWriteToLcdShoo(0, 3, "Hoogen - 24/6/2019");
  delay(2000);
  
  MethodDisplayLayoutShoo(); // Print start game logic.

  // The timer will repeat every 400 ms
  tmrInputsShoo.setInterval(400); 

  // The function to be called
  tmrInputsShoo.setCallback(MethodReadJoystickShoo);

  // Start the timer
  tmrInputsShoo.start();

  // The timer will repeat every 1000 ms
  tmrGameShoo.setInterval(1000); 

  // The function to be called
  tmrGameShoo.setCallback(MethodRunGameLogicShoo);
}

void loop() {
  //Update the timer
  tmrInputsShoo.update();

  tmrGameShoo.update();
}

void MethodReadJoystickShoo() {
  int m_joyXValShoo = analogRead(aiJoyXValShoo); // X val.
  int m_joyYvalShoo = analogRead(aiJoyYValShoo); // Y val.
  int m_joyBtnPressShoo = digitalRead(diJoyPressShoo); // Button press.
  delay(10); // Debounce.
  int m_joyBtnPress2Shoo = digitalRead(diJoyPressShoo); // Button press.

  if(m_joyBtnPressShoo == m_joyBtnPress2Shoo && m_joyBtnPressShoo == 0) {
    Serial.println("Button pressed...");
    MethodHandleInputsShoo(0);
  }

  if (m_joyYvalShoo > 600) {
    Serial.println("Joystick down.");
    MethodHandleInputsShoo(1);
  }
  else if (m_joyYvalShoo <400) {
    Serial.println("Joystick up.");
    MethodHandleInputsShoo(2);
  }
  
  if (m_joyXValShoo > 600) {
    Serial.println("Joystick right.");
    MethodHandleInputsShoo(3);
  }
  else if (m_joyXValShoo < 400) {
    Serial.println("Joystick left.");
    MethodHandleInputsShoo(4);
  }
}

void MethodHandleInputsShoo(int a_iJoyShoo) {
  if (iCurrentGameScreenShoo == 0) {
    if (a_iJoyShoo == 0) {
      if (iCursorPositionShoo[1] == 1) {
        iCurrentGameScreenShoo = 1;
        Serial.println("Starting game!");
      }
      else {
        iCurrentGameScreenShoo = 4;
        Serial.println("Displaying highscores...");
      }

      MethodDisplayLayoutShoo();
    }
    else {
      MethodUpdateCursorSlin(a_iJoyShoo, 1);
    }
  }
  else if (iCurrentGameScreenShoo == 1) {
    if (a_iJoyShoo == 0) {
      iGameDifficulty = iCursorPositionShoo[1]; // Get difficulty.
      iCurrentGameScreenShoo = 2;
      Serial.println("Difficulty is: " + String(iGameDifficulty));
      MethodDisplayLayoutShoo();
    }
    else {
      MethodUpdateCursorSlin(a_iJoyShoo, 0);
    }
  }
  else if (iCurrentGameScreenShoo == 2) {
    
  }
  else if (iCurrentGameScreenShoo == 3) {
    
  }
  else if (iCurrentGameScreenShoo == 4) {
    
  }
}

void MethodUpdateCursorSlin(int a_iJoyDirectionShoo, int a_iBoundaryLowerSlin) {
  if (a_iJoyDirectionShoo == 1 && iCursorPositionShoo[1] < 2) {
    MethodWriteToLcdShoo(iCursorPositionShoo[0], iCursorPositionShoo[1], " ");
    iCursorPositionShoo[1]++;
    MethodWriteToLcdShoo(iCursorPositionShoo[0], iCursorPositionShoo[1], "<");
  }
  else if (a_iJoyDirectionShoo == 2 && iCursorPositionShoo[1] > a_iBoundaryLowerSlin) {
    MethodWriteToLcdShoo(iCursorPositionShoo[0], iCursorPositionShoo[1], " ");
    iCursorPositionShoo[1]--;
    MethodWriteToLcdShoo(iCursorPositionShoo[0], iCursorPositionShoo[1], "<");
  }
}

void MethodDisplayLayoutShoo() {
  lcd4x20Shoo.clear();
  Serial.println("Displaying gamescreen: " + String(iCurrentGameScreenShoo));
  if (iCurrentGameScreenShoo == 0) {
    MethodWriteToLcdShoo(4, 1, "Start game");
    MethodWriteToLcdShoo(4, 2, "Highscores");
    MethodWriteToLcdShoo(iCursorPositionShoo[0], iCursorPositionShoo[1], "<");
  }
  else if (iCurrentGameScreenShoo == 1) {
    iCursorPositionShoo[0] = 19;
    iCursorPositionShoo[1] = 0;
    MethodWriteToLcdShoo(0, 0, "Recruit");
    MethodWriteToLcdShoo(0, 1, "Regular");
    MethodWriteToLcdShoo(0, 2, "Veteran");
    MethodWriteToLcdShoo(iCursorPositionShoo[0], iCursorPositionShoo[1], "<");
  }
  else if (iCurrentGameScreenShoo == 2) {
    MethodWriteToLcdShoo(4, 1, "Enter name!");
    delay(1000);
    iCurrentGameScreenShoo++;
    MethodDisplayLayoutShoo();
  }
  else if (iCurrentGameScreenShoo == 3) {
    MethodWriteToLcdShoo(0, 0, "--------------------");
    MethodWriteToLcdShoo(0, 1, "|");
    MethodWriteToLcdShoo(0, 2, "|");
    MethodWriteToLcdShoo(4, 1, "Heads up!");
    MethodWriteToLcdShoo(19, 1, "|");
    MethodWriteToLcdShoo(19, 2, "|");
    MethodWriteToLcdShoo(0, 3, "--------------------");
    delay(1000);
    lcd4x20Shoo.clear();
    MethodWriteToLcdShoo(0, 3, "*=000 >=000 s000 300");
    uiGameTimerStartShoo = millis(); // Set start time to current time.
    tmrGameShoo.start();
  }
  else if (iCurrentGameScreenShoo == 4) {
    MethodWriteToLcdShoo(1, 1, "Endgame screen.");
    MethodWriteToLcdShoo(1, 2, "Work in progress.");
    delay(1000);
    iCurrentGameScreenShoo = 0;
    MethodDisplayLayoutShoo();
  }
  else if (iCurrentGameScreenShoo == 5) {
    MethodWriteToLcdShoo(1, 1, "Highscores.");
    MethodWriteToLcdShoo(1, 2, "Work in progress.");
    delay(1000);
    iCurrentGameScreenShoo = 0;
    MethodDisplayLayoutShoo();
  }
}

void MethodRunGameLogicShoo() {
  int m_iTimeLeftShoo = 10 - ((millis() - uiGameTimerStartShoo) / 1000);
  String m_sTimeLeftShoo = String(m_iTimeLeftShoo);
  while(m_sTimeLeftShoo.length() < 3) {
    m_sTimeLeftShoo = " " + m_sTimeLeftShoo;
  }
  MethodWriteToLcdShoo(17, 3, m_sTimeLeftShoo);
  
  if (m_iTimeLeftShoo <= 0) {
    tmrGameShoo.stop();
    iCursorPositionShoo[1] = 1;
    iCurrentGameScreenShoo = 4;
    MethodDisplayLayoutShoo();
  }
}

// Method that prints a string to specific place on Lcd, where x val is place on row, y is colomn and lcdString is string to be printed.
void MethodWriteToLcdShoo(int a_valX, int a_valY, String a_lcdString) {
  lcd4x20Shoo.setCursor(a_valX, a_valY); // Set lcd to given values.
  for(int m_stringIndex = 0; m_stringIndex < a_lcdString.length(); m_stringIndex++)
  {
    lcd4x20Shoo.print(a_lcdString[m_stringIndex]);
  }
}
