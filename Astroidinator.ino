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

int iAstroidCountSlin = 0; // Astroids user has collected.
int iSpaceShipCountSlin = 0; // Spaceships user has crashed into.
int iCurrentGameScreenShoo = 0; // Default screen is start game, 1 = difficulty, 2 = enter name, 3 = game, 4 = post-game, 5 = highscore.
int iCursorBoundsYSlin[] = {1, 2}; // Cursor boundaries for y.
int iGameDifficulty = 0; // Game difficulty.
int iCursorPositionShoo[] = {15, 1}; // Where cursor is positioned.
int iEntityPositionsShoo[] = {
  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  }; // Defines entities on playfield, 0 = nothing, 1 = space ships, 2 = asteroids.

unsigned long uiGameTimerStartShoo = 0;

void setup() {
  pinMode(diJoyPressShoo, INPUT); // Initialize joystick button.
  pinMode(doBuzzerShoo, OUTPUT); // Set buzzer output.
  digitalWrite(diJoyPressShoo, HIGH); // Write joystick button high(default, unpressed value is high).
  randomSeed(analogRead(0));
  
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
  //Update the timers
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
        iCurrentGameScreenShoo = 5;
        Serial.println("Displaying highscores...");
      }

      MethodDisplayLayoutShoo();
    }
    else if (a_iJoyShoo < 3) {
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
    else if (a_iJoyShoo < 3) {
      MethodUpdateCursorSlin(a_iJoyShoo, 0);
    }
  }
  else if (iCurrentGameScreenShoo == 2) {
    
  }
  else if (iCurrentGameScreenShoo == 3) {
    if (a_iJoyShoo == 0) {
      
    }
    else {
      MethodUpdateCursorSlin(a_iJoyShoo, 0);
    }
  }
  else if (iCurrentGameScreenShoo == 4) {
    
  }
  else if (iCurrentGameScreenShoo == 5) {
    
  }
}

void MethodUpdateCursorSlin(int a_iJoyDirectionShoo, int a_iBoundaryLowerSlin) {
  MethodWriteToLcdShoo(iCursorPositionShoo[0], iCursorPositionShoo[1], " ");
  
  if (a_iJoyDirectionShoo == 1 && iCursorPositionShoo[1] < 2) {
    iCursorPositionShoo[1]++;
  }
  else if (a_iJoyDirectionShoo == 2 && iCursorPositionShoo[1] > a_iBoundaryLowerSlin) {
    iCursorPositionShoo[1]--;
  }
  else if(a_iJoyDirectionShoo == 3 && iCursorPositionShoo[0] < 19) {
    iCursorPositionShoo[0]++;
  }
  else if(a_iJoyDirectionShoo == 4 && iCursorPositionShoo[0] > 0) {
    iCursorPositionShoo[0]--;
  }

  // Run collision detection if currently playing game.
  if(iCurrentGameScreenShoo == 3) {
    MethodColisionDetectionSlin();
  }
  
  MethodWriteToLcdShoo(iCursorPositionShoo[0], iCursorPositionShoo[1], "=");
}

void MethodDisplayLayoutShoo() {
  lcd4x20Shoo.clear();
  Serial.println("Displaying gamescreen: " + String(iCurrentGameScreenShoo));
  if (iCurrentGameScreenShoo == 0) {
    MethodWriteToLcdShoo(4, 1, "Start game");
    MethodWriteToLcdShoo(4, 2, "Highscores");
    MethodWriteToLcdShoo(iCursorPositionShoo[0], iCursorPositionShoo[1], "=");
  }
  else if (iCurrentGameScreenShoo == 1) {
    iCursorPositionShoo[0] = 15;
    iCursorPositionShoo[1] = 1;
    MethodWriteToLcdShoo(4, 0, "Recruit");
    MethodWriteToLcdShoo(4, 1, "Regular");
    MethodWriteToLcdShoo(4, 2, "Veteran");
    MethodWriteToLcdShoo(iCursorPositionShoo[0], iCursorPositionShoo[1], "=");
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
    iCursorPositionShoo[0] = 19;
    iCursorPositionShoo[1] = 1;
    MethodUpdateCursorSlin(-1, 0); // Pass along -1, so it'll just update position.
    MethodWriteToLcdShoo(0, 3, "*=000 >=000 s000 300");
    uiGameTimerStartShoo = millis(); // Set start time to current time.
    tmrGameShoo.start(); // Comence game logic.
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
  int m_iRandomNumShoo = random(0,3);

  for(int m_iLineCellShoo = 59; m_iLineCellShoo >= 0; m_iLineCellShoo--) {
    if (iEntityPositionsShoo[m_iLineCellShoo] > 0) {
      if (m_iLineCellShoo != 19 && m_iLineCellShoo != 39 && m_iLineCellShoo != 59) {
        iEntityPositionsShoo[m_iLineCellShoo + 1] = iEntityPositionsShoo[m_iLineCellShoo];
        
        int m_iXvalShoo = m_iLineCellShoo + 1;
        int m_iYvalShoo = 0;
      
        while (m_iXvalShoo > 19) {
          m_iXvalShoo = m_iXvalShoo - 20;
          m_iYvalShoo++;
        }
        
        if (iEntityPositionsShoo[m_iLineCellShoo + 1] == 1) {
          MethodWriteToLcdShoo(m_iXvalShoo, m_iYvalShoo, ">");
        }
        else {
          MethodWriteToLcdShoo(m_iXvalShoo, m_iYvalShoo, "*");
        }
      }
      
      iEntityPositionsShoo[m_iLineCellShoo] = 0;

      int m_iXvalShoo = m_iLineCellShoo;
      int m_iYvalShoo = 0;
      
      while (m_iXvalShoo > 19) {
        m_iXvalShoo = m_iXvalShoo - 20;
        m_iYvalShoo++;
      }

      MethodWriteToLcdShoo(m_iXvalShoo, m_iYvalShoo, " ");
    }
  }
  
  MethodUpdateCursorSlin(-1, 0); // Pass along -1, so it'll just redraw cursor position.
  MethodColisionDetectionSlin(); // Check if updated positions has caused a collision, and update scoreboard if it has.
  int m_iTimeLeftShoo = 300 - ((millis() - uiGameTimerStartShoo) / 1000); // Calculate time.
  MethodWriteToLcdShoo(17, 3, MethodString3CharsSlin(m_iTimeLeftShoo)); // Update time.

  // Time is up, so go to endgame screen.
  if (m_iTimeLeftShoo <= 0) {
    tmrGameShoo.stop();
    iCursorPositionShoo[0] = 15;
    iCursorPositionShoo[1] = 1;
    iCurrentGameScreenShoo = 4;
    MethodDisplayLayoutShoo();
  }
}

void MethodColisionDetectionSlin() {
  int m_iCursorArrayPosition = iCursorPositionShoo[0] + iCursorPositionShoo[1] * 20; // Convert cursor position to array position.

  // If cursor collides with an entity, add 1 to entity count, remove entity from array and update score in bottom of screen.
  if(iEntityPositionsShoo[m_iCursorArrayPosition] > 0) {
    if (iEntityPositionsShoo[m_iCursorArrayPosition] == 1) {
      iSpaceShipCountSlin++;
      MethodWriteToLcdShoo(8, 3, MethodString3CharsSlin(iSpaceShipCountSlin)); // Update spaceship scoreboard.
    }
    else if (iEntityPositionsShoo[m_iCursorArrayPosition] == 2) {
      iAstroidCountSlin++;
      MethodWriteToLcdShoo(2, 3, MethodString3CharsSlin(iAstroidCountSlin)); // Update asteroid scoreboard.
    }

    iEntityPositionsShoo[m_iCursorArrayPosition] = 0; // Remove entity from game.
    int m_iPlayerScoreSlin = iAstroidCountSlin - iSpaceShipCountSlin; // Calculate score by subtracting spaceships from asteroids.

    // Score can't be negative, so if it is, just print a score of 0, else print score.
    if(m_iPlayerScoreSlin < 0) {
      MethodWriteToLcdShoo(13, 3, "000");
    }
    else {
      MethodWriteToLcdShoo(13, 3, MethodString3CharsSlin(m_iPlayerScoreSlin));
    }
  }
}

String MethodString3CharsSlin(int a_iScoreboardValSlin) {
  String m_sScoreboardStringSlin = String(a_iScoreboardValSlin);
  while(m_sScoreboardStringSlin.length() < 3) {
    m_sScoreboardStringSlin = "0" + m_sScoreboardStringSlin;
  }

  return m_sScoreboardStringSlin; // Return 3 length String.
}

// Method that prints a string to specific place on Lcd, where x val is place on row, y is colomn and lcdString is string to be printed.
void MethodWriteToLcdShoo(int a_valX, int a_valY, String a_sLcdString) {
  lcd4x20Shoo.setCursor(a_valX, a_valY); // Set lcd to given values.
  
  for(int m_stringIndex = 0; m_stringIndex < a_sLcdString.length(); m_stringIndex++)
  {
    lcd4x20Shoo.print(a_sLcdString[m_stringIndex]);
  }
}
