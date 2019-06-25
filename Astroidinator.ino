/*
  Astroidinator
  from 24-6-2019 till 26-6-2019
  
  Made by:
  Sander van den Hoogen
  Stijn Lingmont

  IC18.AO.A
*/

#include <timer.h> // Timer library.
#include <Wire.h> // Include wire library, which facilitates serial communication with lcd.
#include <LiquidCrystal_I2C.h> // Include I2C library for lcd screen.
#include <EEPROM.h> // Include library that allows us to write data to memory.

LiquidCrystal_I2C lcd4x20Shoo(0x3F, 20, 4); // Initialize new object called 'lcd', that will control what is displayed on lcd.

Timer tmrInputsShoo; // Define timer to read joystick inputs.
Timer tmrGameShoo; // Define timer to tick.

#define aiJoyXValShoo   A2  // X val on joy.
#define aiJoyYValShoo   A3  // Y val on joy.
#define diJoyPressShoo  2   // Define button on joystick.
#define doBuzzerShoo    3   // Define buzzer to corresponding pin.
#define iTotalGameTimeShoo  50 // Defines total seconds game will run for.

int iAstroidCountSlin = 0; // Astroids user has collected.
int iSpaceShipCountSlin = 0; // Spaceships user has crashed into
int iHighscoreSlin = 0; // Holds highscore.
char cUserNameSlin = "A";
bool bPreviousBtnStateSlin = 1; // Previous state of joystick button.
int iCurrentGameScreenShoo = 0; // Default screen is start game, 1 = difficulty, 2 = enter name, 3 = game, 4 = post-game, 5 = highscore.

int iDifficultylevelShoo = 14; // Define upper boundary for spaceship generation, which determines difficulty.
int iCursorBoundsYSlin[] = {1, 2}; // Cursor boundaries for y.
int iCursorPositionShoo[] = {15, 1}; // Where cursor is positioned.
int iEntityPositionsShoo[] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  }; // Defines entities on playfield, 0 = nothing, 1 = space ships, 2 = asteroids.

unsigned long uiGameTimerStartShoo = 0; //Timer variable

void setup() {
  pinMode(diJoyPressShoo, INPUT); // Initialize joystick button.
  pinMode(doBuzzerShoo, OUTPUT); // Set buzzer output.
  digitalWrite(diJoyPressShoo, HIGH); // Write joystick button high(default, unpressed value is high).
  randomSeed(analogRead(0));

  //EEPROM.write(0, 1); // Overwrite byte with default score.
  //EEPROM.write(1, 'a'); // Overwrite byte with default score.
  
  iHighscoreSlin = EEPROM.read(0); // Get highscore from byte 0.
  cUserNameSlin = EEPROM.read(1); // Get username char from byte 1.
  
  Serial.begin(115200); // Open serial console.
  lcd4x20Shoo.init(); // Initialise lcd.
  lcd4x20Shoo.setBacklight(1); // Enable backlight for bright screen.
  lcd4x20Shoo.clear(); // Clear lcd.
  Serial.println(iHighscoreSlin);
  Serial.println(cUserNameSlin);


  // Print startup info.
  MethodWriteToLcdShoo(0, 0, "Astroidinator! V1.2");
  MethodWriteToLcdShoo(0, 1, "Made by: Stijn");
  MethodWriteToLcdShoo(0, 2, "Lingmont & Sander vd");
  MethodWriteToLcdShoo(0, 3, "Hoogen - 24/6/2019");
  delay(2000);
  
  MethodDisplayLayoutShoo(); // Print start game logic.

  tmrInputsShoo.setInterval(200); // The timer will repeat every 400 ms
  tmrInputsShoo.setCallback(MethodReadJoystickShoo); // The function to be called
  tmrInputsShoo.start(); // Start the timer
  
  tmrGameShoo.setInterval(1000); // The timer will repeat every 1000 ms
  tmrGameShoo.setCallback(MethodRunGameLogicShoo); // The function to be called
}

void loop() {
  //Update the timers.
  tmrInputsShoo.update();
  tmrGameShoo.update();
}

//Read the input of the joystick
void MethodReadJoystickShoo() {
  int m_joyXValShoo = analogRead(aiJoyXValShoo); // X val.
  int m_joyYvalShoo = analogRead(aiJoyYValShoo); // Y val.
  int m_joyBtnPressShoo = digitalRead(diJoyPressShoo); // Button press.
  delay(5); // Debounce.
  int m_joyBtnPress2Shoo = digitalRead(diJoyPressShoo); // Button press.

  // Button debounce.
  if (m_joyBtnPressShoo == m_joyBtnPress2Shoo && m_joyBtnPressShoo == 0 && m_joyBtnPressShoo != bPreviousBtnStateSlin) {
    Serial.println("Button pressed...");
    MethodHandleInputsShoo(0);
  }

  bPreviousBtnStateSlin = m_joyBtnPressShoo;

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

// Handles joystick inputs.
void MethodHandleInputsShoo(int a_iJoyShoo) {
  // If on first gamescreen, only process joystick click and up/down.
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
  // If on second gamescreen, only process joystick click and up/down.
  else if (iCurrentGameScreenShoo == 1) {
    if (a_iJoyShoo == 0) {
      iDifficultylevelShoo = 14 - iCursorPositionShoo[1]; //Set difficulty of the game by defining upper boundary for spaceship generation, increasing spaceship frequency.
      iCurrentGameScreenShoo = 2;
      Serial.println("Difficulty is: " + String(iDifficultylevelShoo));
      MethodDisplayLayoutShoo();
    }
    else if (a_iJoyShoo < 3) {
      MethodUpdateCursorSlin(a_iJoyShoo, 0);
    }
  }
  // If on third game screen, only process click, left, right.
  else if (iCurrentGameScreenShoo == 2) {
    String m_sNameSelectedShoo = "ABCDEFGHIJKLMNOPQRST";
    String m_sNameUnselectedShoo = "abcdefghijklmnopqrst";
    
    if (a_iJoyShoo == 0) {
      cUserNameSlin = m_sNameSelectedShoo.charAt(iCursorPositionShoo[0]);
      Serial.println("Selected name is: " + cUserNameSlin);
      iCurrentGameScreenShoo = 3;
      MethodDisplayLayoutShoo();
    }
    else if (a_iJoyShoo == 3 && iCursorPositionShoo[0] < 20) {
      MethodWriteToLcdShoo(iCursorPositionShoo[0], 1, String(m_sNameUnselectedShoo.charAt(iCursorPositionShoo[0])));
      iCursorPositionShoo[0]++;
      MethodWriteToLcdShoo(iCursorPositionShoo[0], 1, String(m_sNameSelectedShoo.charAt(iCursorPositionShoo[0])));
    }
    else if (a_iJoyShoo == 4 && iCursorPositionShoo[0] > 0) {
      MethodWriteToLcdShoo(iCursorPositionShoo[0], 1, String(m_sNameUnselectedShoo.charAt(iCursorPositionShoo[0])));
      iCursorPositionShoo[0]--;
      MethodWriteToLcdShoo(iCursorPositionShoo[0], 1, String(m_sNameSelectedShoo.charAt(iCursorPositionShoo[0])));
    }
  }
  // If on fourth gamescreen, process all buttons(except click WIP.).
  else if (iCurrentGameScreenShoo == 3) {
    if (a_iJoyShoo != 0) {
      MethodUpdateCursorSlin(a_iJoyShoo, 0);
    }
  }
  // If on Fifth gamescreen, only process joystick click and up/down.
  else if (iCurrentGameScreenShoo == 4) {
    if (a_iJoyShoo == 0) {
      if (iCursorPositionShoo[1] == 1) {
        iCurrentGameScreenShoo = 3;
        Serial.println("Starting new game!"); // Debug info.
      }
      else {
        iCurrentGameScreenShoo = 0;
        Serial.println("Going back to start screen."); // Debug info.
      }
      MethodDisplayLayoutShoo();
    }
    else if (a_iJoyShoo < 3) {
      MethodUpdateCursorSlin(a_iJoyShoo, 1);
    }
  }
  // If on sixth gamescreen, only process button click.
  else if (iCurrentGameScreenShoo == 5) {
    if (a_iJoyShoo == 0) {
      Serial.println("Switching back to start game..."); // Debug info.
      iCurrentGameScreenShoo = 0;
      MethodDisplayLayoutShoo();
    }
  }
}

// Update the cursor based on joystick input.
void MethodUpdateCursorSlin(int a_iJoyDirectionShoo, int a_iBoundaryLowerSlin) {
  MethodWriteToLcdShoo(iCursorPositionShoo[0], iCursorPositionShoo[1], " "); // Clear out cursor.

  // Check in which direction cursor is moving.
  if (a_iJoyDirectionShoo == 1 && iCursorPositionShoo[1] < 2) {
    iCursorPositionShoo[1]++; // Moving up
  }
  else if (a_iJoyDirectionShoo == 2 && iCursorPositionShoo[1] > a_iBoundaryLowerSlin) {
    iCursorPositionShoo[1]--; // Moving down.
  }
  else if(a_iJoyDirectionShoo == 3 && iCursorPositionShoo[0] < 19) {
    iCursorPositionShoo[0]++; // Moving left
  }
  else if(a_iJoyDirectionShoo == 4 && iCursorPositionShoo[0] > 0) {
    iCursorPositionShoo[0]--; // Moving right.
  }
  
  MethodWriteToLcdShoo(iCursorPositionShoo[0], iCursorPositionShoo[1], "="); // Print cursor to display.
  
  // Run collision detection if currently playing game.
  if(iCurrentGameScreenShoo == 3) {
    MethodColisionDetectionSlin();
  }
}

// Base layout for each game screen.
void MethodDisplayLayoutShoo() {
  lcd4x20Shoo.clear(); //Clear lcd, in preperation for new elements.
  Serial.println("Displaying gamescreen: " + String(iCurrentGameScreenShoo)); // Debug info.
  
  // Startup menu.
  if (iCurrentGameScreenShoo == 0) {
    MethodWriteToLcdShoo(4, 1, "Start game");
    MethodWriteToLcdShoo(4, 2, "Highscores");
    iCursorPositionShoo[0] = 15;
    MethodWriteToLcdShoo(iCursorPositionShoo[0], iCursorPositionShoo[1], "=");
  }
  // Difficulty menu.
  else if (iCurrentGameScreenShoo == 1) {
    MethodWriteToLcdShoo(4, 0, "Recruit");
    MethodWriteToLcdShoo(4, 1, "Regular");
    MethodWriteToLcdShoo(4, 2, "Veteran");
    iCursorPositionShoo[0] = 15;
    iCursorPositionShoo[1] = 1;
    MethodWriteToLcdShoo(iCursorPositionShoo[0], iCursorPositionShoo[1], "=");
  }
  // Enter name menu (WIP).
  else if (iCurrentGameScreenShoo == 2) {
    MethodWriteToLcdShoo(0, 0, "Select your name!");
    MethodWriteToLcdShoo(0, 1, "Abcdefghijklmnopqrst");
    iCursorPositionShoo[0] = 0;
    iCursorPositionShoo[1] = 1;
  }
  // Game screen.
  else if (iCurrentGameScreenShoo == 3) {
    // Clears all entities from entity arrays.
    for(int m_iLoopCountSlin = 0; m_iLoopCountSlin < 60; m_iLoopCountSlin++) {
      iEntityPositionsShoo[m_iLoopCountSlin] = 0;
    }
    
    iAstroidCountSlin = 0; // Reset astroids user has collected.
    iSpaceShipCountSlin = 0; // Reset spaceships user has crashed into.
    
    MethodWriteToLcdShoo(0, 0, "--------------------");
    MethodWriteToLcdShoo(0, 1, "|");
    MethodWriteToLcdShoo(0, 2, "|");
    MethodWriteToLcdShoo(4, 1, "Heads up!");
    MethodWriteToLcdShoo(19, 1, "|");
    MethodWriteToLcdShoo(19, 2, "|");
    MethodWriteToLcdShoo(0, 3, "--------------------");

    // Play audio tune.
    MethodBuzzerShoo(250, 100);
    delay(50);
    MethodBuzzerShoo(350, 100);
    delay(50);
    MethodBuzzerShoo(450, 100);
    delay(50);
    MethodBuzzerShoo(150, 100);
    delay(25);
    MethodBuzzerShoo(700, 100);
    delay(25);
    MethodBuzzerShoo(1000, 100);
    delay(25);
    MethodBuzzerShoo(400, 100);
    delay(300);
    
    lcd4x20Shoo.clear(); // Clear lcd
    iCursorPositionShoo[0] = 19;
    iCursorPositionShoo[1] = 1;
    MethodUpdateCursorSlin(-1, 0); // Pass along -1, so it'll just redraw cursor.
    MethodWriteToLcdShoo(0, 3, "*=000 >=000 s000 300"); // Scoreboard elements.
    uiGameTimerStartShoo = millis(); // Set start time to current time.
    tmrGameShoo.start(); // Comence game logic.
  }
  // Post-game menu.
  else if (iCurrentGameScreenShoo == 4) {
    int m_iCurrentScore = iAstroidCountSlin - iSpaceShipCountSlin;
    MethodWriteToLcdShoo(1, 2, "Your score was " + String(MethodIntToString3Slin(m_iCurrentScore))); // Print gotten score to screen.
    
    // User has lost, play lost sound and print You have lost! for ~2 seconds.
    if (iAstroidCountSlin - iSpaceShipCountSlin < 0) {
      MethodWriteToLcdShoo(3, 1,"You have lost!");
      MethodBuzzerShoo(1000, 100);
      delay(10);
      MethodBuzzerShoo(750, 100);
      delay(10);
      MethodBuzzerShoo(500, 100);
      delay(15);
      MethodBuzzerShoo(400, 100);
      delay(20);
      MethodBuzzerShoo(300, 100);
      delay(25);
      MethodBuzzerShoo(200, 100);
      delay(30);
      MethodBuzzerShoo(50, 100);
      delay(50);
    }
    // User has won, play won sound and print You have won! for ~2 seconds.
    else {
      MethodWriteToLcdShoo(3, 1 ,"You have won!");
      MethodBuzzerShoo(50, 100);
      delay(30);
      MethodBuzzerShoo(200, 100);
      delay(25);
      MethodBuzzerShoo(300, 100);
      delay(20);
      MethodBuzzerShoo(400, 100);
      delay(15);
      MethodBuzzerShoo(500, 100);
      delay(10);
      MethodBuzzerShoo(750, 100);
      delay(10);
      MethodBuzzerShoo(1000, 100);
      delay(10);
    }
    delay(2000);
    lcd4x20Shoo.clear(); // Clear screen, update cursor and print new dialogue.

    // If current score is higher then highscore, set new highscore.
    if(m_iCurrentScore > iHighscoreSlin) {
      iHighscoreSlin = m_iCurrentScore;
      EEPROM.write(0, iHighscoreSlin); // Write highscore to memory
      EEPROM.write(1, cUserNameSlin); // Write highscore to memory
      MethodWriteToLcdShoo(2, 1, "You've achieved");
      MethodWriteToLcdShoo(2, 2, "a new highscore!");
      Serial.println("New highscore achieved: " + String(cUserNameSlin) + " " + String(iHighscoreSlin));
      delay(2000);
      lcd4x20Shoo.clear(); // Clear screen, update cursor and print new dialogue.
    }
    
    iCursorBoundsYSlin[0] = 1;
    iCursorPositionShoo[0] = 11;
    iCursorPositionShoo[1] = 1;
    MethodWriteToLcdShoo(1, 0,"Wanna play again?");
    MethodWriteToLcdShoo(7, 1,"Yes =");
    MethodWriteToLcdShoo(7, 2,"No");
  }
  // Highscores menu.
  else if (iCurrentGameScreenShoo == 5) {
    MethodWriteToLcdShoo(1, 0, "Current highscore:");
    MethodWriteToLcdShoo(7, 1, String(cUserNameSlin) + " " + MethodIntToString3Slin(iHighscoreSlin));
    MethodWriteToLcdShoo(6, 3, "Return =");
  }
}

// Logic that will run every seconds, to perform logic for game when it is in progress.
void MethodRunGameLogicShoo() {
  // For loop that will run through array and move each entity one step to the left, except if it goes of the screen, then it deletes entity.
  for(int m_iLineCellShoo = 59; m_iLineCellShoo >= 0; m_iLineCellShoo--) {
    // If an entity is in current array position, move it.
    if (iEntityPositionsShoo[m_iLineCellShoo] > 0) {
      // If the entity isn't at the edge of screen, move it one spot over and print it's new location to screen.
      if (m_iLineCellShoo != 19 && m_iLineCellShoo != 39 && m_iLineCellShoo != 59) {
        iEntityPositionsShoo[m_iLineCellShoo + 1] = iEntityPositionsShoo[m_iLineCellShoo]; // Moves current entity one spot over.
        
        int m_iXvalShoo = m_iLineCellShoo + 1; // X coordinate entity on screen.
        int m_iYvalShoo = 0; // Y coordinate of entity.

        // Converts array value back to display coordinates.
        while (m_iXvalShoo > 19) {
          m_iXvalShoo = m_iXvalShoo - 20;
          m_iYvalShoo++;
        }

        // If entity is spaceship, print ">" to it's calculated location on display, else print asteroid.
        if (iEntityPositionsShoo[m_iLineCellShoo + 1] == 1) {
          MethodWriteToLcdShoo(m_iXvalShoo, m_iYvalShoo, ">");
        }
        else {
          MethodWriteToLcdShoo(m_iXvalShoo, m_iYvalShoo, "*");
        }
      }
      
      iEntityPositionsShoo[m_iLineCellShoo] = 0; // Clear last location of entity, where it no longer is supposed to be.

      int m_iXvalShoo = m_iLineCellShoo; // X coordinate entity on screen.
      int m_iYvalShoo = 0; // Y coordinate of entity.
      
      // Converts array value back to display coordinates.
      while (m_iXvalShoo > 19) {
        m_iXvalShoo = m_iXvalShoo - 20;
        m_iYvalShoo++;
      }

      MethodWriteToLcdShoo(m_iXvalShoo, m_iYvalShoo, " "); // Remove previous entity from display.
    }
  }

  int m_iRandomEntityShoo[] = {random(0, 16), random(0, 16), random(0, 16)}; // Create 3 random int's in array for entity spawning.

  // Go through this array and check whether the random number corresponds to an asteroid or spaceship, and if it does add the entity to the beginning of entity position array.
  for(int m_iLoopCountShoo = 0; m_iLoopCountShoo < 3; m_iLoopCountShoo++) {
    // Check if current random num is higher then difficulty level, which is either 12, 13 or 14, and if it is the random entity is a spaceship.
    if (m_iRandomEntityShoo[m_iLoopCountShoo] > iDifficultylevelShoo) {
      iEntityPositionsShoo[m_iLoopCountShoo * 20] = 1; // Add spaceship to it's corresponding location in entity array, which is the position in array * 20.
      MethodWriteToLcdShoo(0, m_iLoopCountShoo, ">"); // Print spaceship to display.
    }
    // See above for comments.
    else if (m_iRandomEntityShoo[m_iLoopCountShoo] > iDifficultylevelShoo - 2) {
      iEntityPositionsShoo[m_iLoopCountShoo * 20] = 2;
      MethodWriteToLcdShoo(0, m_iLoopCountShoo, "*");
    }
  }

  // If a wall of enemies has been generated, remove random enemy from array and screen so that user is always able to dodge new row.
  if (iEntityPositionsShoo[0] == 1 && iEntityPositionsShoo[20] == 1 && iEntityPositionsShoo[40] && 1) {
    int m_iRandomEntityShoo = random(0, 3) * 20;
    iEntityPositionsShoo[m_iRandomEntityShoo] = 0;
    MethodWriteToLcdShoo(0, m_iRandomEntityShoo, "*");
  }
  
  MethodUpdateCursorSlin(-1, 0); // Pass along -1, so it'll just redraw cursor position, otherwise if player collides with entity entity will take priority which isn't supposed to happen.
  MethodColisionDetectionSlin(); // Check for collisions.
  int m_iTimeLeftShoo = iTotalGameTimeShoo - ((millis() - uiGameTimerStartShoo) / 1000); // Calculate time.
  MethodWriteToLcdShoo(17, 3, MethodIntToString3Slin(m_iTimeLeftShoo)); // Update time.

  // Time is up, so perform logic for and go to endgame screen.
  if (m_iTimeLeftShoo <= 0) {
    tmrGameShoo.stop();
    iCursorPositionShoo[0] = 15;
    iCursorPositionShoo[1] = 1;
    iCurrentGameScreenShoo = 4;
    MethodDisplayLayoutShoo();
  }
}

// Method that will detect whether cursor(so player) has collided with an entity, if it has, remove entity, play sound and update various scores accordingly.
void MethodColisionDetectionSlin() {
  int m_iCursorArrayPositionSlin = iCursorPositionShoo[0] + iCursorPositionShoo[1] * 20; // Convert cursor position to array position, x val + y * 20 val.

  // If cursor collides with an entity, add 1 to corresponding entity count, remove entity from array, play sound and update scores in bottom of screen.
  if(iEntityPositionsShoo[m_iCursorArrayPositionSlin] > 0) {
    // Player has hit a spaceship.
    if (iEntityPositionsShoo[m_iCursorArrayPositionSlin] == 1) {
      iSpaceShipCountSlin++;
      MethodWriteToLcdShoo(8, 3, MethodIntToString3Slin(iSpaceShipCountSlin)); // Update spaceship scoreboard.
      MethodBuzzerShoo(400, 100);
      delay(10);
      MethodBuzzerShoo(200, 100);
    }
    // Player has hit an astroid.
    else if (iEntityPositionsShoo[m_iCursorArrayPositionSlin] == 2) {
      iAstroidCountSlin++;
      MethodWriteToLcdShoo(2, 3, MethodIntToString3Slin(iAstroidCountSlin)); // Update asteroid scoreboard.
      MethodBuzzerShoo(500, 100);
      delay(10);
      MethodBuzzerShoo(750, 100);
    }

    iEntityPositionsShoo[m_iCursorArrayPositionSlin] = 0; // Remove entity from game.
    int m_iPlayerScoreSlin = iAstroidCountSlin - iSpaceShipCountSlin; // Calculate score by subtracting spaceships from asteroids.

    // Score can't be negative, so if it is, just print a score of 0, else print calculated score.
    if(m_iPlayerScoreSlin < 0) {
      MethodWriteToLcdShoo(13, 3, "000");
    }
    else {
      MethodWriteToLcdShoo(13, 3, MethodIntToString3Slin(m_iPlayerScoreSlin));
    }
  }
}

// Method that will convert an integer to a string and make sure it has length of 3, otherwise adds 0's to string until it fits desired length, then returns converted string.
String MethodIntToString3Slin(int a_iConvertIntSlin) {
  String m_sConvertedStringSlin = String(a_iConvertIntSlin); // Convert to string.
  
  while(m_sConvertedStringSlin.length() < 3) {
    m_sConvertedStringSlin = "0" + m_sConvertedStringSlin;
  }

  return m_sConvertedStringSlin; // Return 3 length String.
}

// Method that prints passed string to position on lcd, determined by PosX and PosY.
void MethodWriteToLcdShoo(int a_iPosXSlin, int a_iPosYSlin, String a_sLcdStringSlin) {
  lcd4x20Shoo.setCursor(a_iPosXSlin, a_iPosYSlin); // Set lcd to given values.
  
  //Print string that is given within the method
  for(int m_iStringIndexSlin = 0; m_iStringIndexSlin < a_sLcdStringSlin.length(); m_iStringIndexSlin++)
  {
    lcd4x20Shoo.print(a_sLcdStringSlin[m_iStringIndexSlin]);
  }
}

// Method playing sound through buzzer, found on internet, source provided below. Code used from 274 to 290 with minor tweaks.
// https://create.arduino.cc/projecthub/jrance/super-mario-theme-song-w-piezo-buzzer-and-arduino-1cc2e4?ref=search&ref_id=buzzer&offset=5
void MethodBuzzerShoo(long a_lFrequencySlin, long a_lMLengthSlin) {
  Serial.println("Playing sound..."); // Debug info.
  long m_lDelaySlin = 1000000 / a_lFrequencySlin / 1; // Calculate delay value between transitions. 1 seconds worth of microseconds, divided by the a_lFrequencySlin, then split in half since there are two phases to each cycle.
  long m_lNumCyclesSlin = a_lFrequencySlin * a_lMLengthSlin / 1000; // Calculate the number of cycles for proper timing. Multiply a_lFrequencySlin, which is really cycles per second, by the number of seconds to get the total number of cycles to produce.
  
  // For loop that will push out and pull back diaphram for amount of cycles calculated above.
  for (long m_lCurrentCycleSlin = 0; m_lCurrentCycleSlin < m_lNumCyclesSlin; m_lCurrentCycleSlin++) {
    digitalWrite(doBuzzerShoo, HIGH); // write the buzzer pin high to push out the diaphram.
    delayMicroseconds(m_lDelaySlin); // wait for the calculated delay.
    digitalWrite(doBuzzerShoo, LOW); // write the buzzer pin low to pull back the diaphram.
    delayMicroseconds(m_lDelaySlin); // wait again for the calculated delay.
  }
}
