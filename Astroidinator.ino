#include <timer.h> // Timer library.
#include <Wire.h> // Include wire library, which facilitates serial communication with lcd.
#include <LiquidCrystal_I2C.h> // Include I2C library for lcd screen.

LiquidCrystal_I2C lcd4x20Shoo(0x3F, 20, 4); // Initialize new object called 'lcd', that will control what is displayed on lcd.

#define aiJoyXValShoo   A2  // X val on joy.
#define aiJoyYValShoo   A3  // Y val on joy.
#define diJoyPressShoo  2   // Define button on joystick.
#define doBuzzerShoo    3   // Define buzzer to corresponding pin.

void setup() {
  pinMode(diJoyPressShoo, INPUT); // Initialize joystick button.
  pinMode(doBuzzerShoo, OUTPUT); // Set buzzer output.
  digitalWrite(diJoyPressShoo, HIGH); // Write joystick button high(default, unpressed value is high).
  
  Serial.begin(9600); // Open serial console.
  lcd4x20Shoo.init();
  lcd4x20Shoo.setBacklight(1);
  lcd4x20Shoo.clear();
  
  MethodWriteToLcd(0, 0, "Astroidinator! V0.1");
  MethodWriteToLcd(0, 1, "Made by: Stijn");
  MethodWriteToLcd(0, 2, "Lingmont & Sander vd");
  MethodWriteToLcd(0, 3, "Hoogen - 24/6/2019");
}

void loop() {

  
}

// Method that prints a string to specific place on Lcd, where x val is place on row, y is colomn and lcdString is string to be printed.
void MethodWriteToLcd(int a_valX, int a_valY, String a_lcdString) {
  lcd4x20Shoo.setCursor(a_valX, a_valY); // Set lcd to given values.
  for(int m_stringIndex = 0; m_stringIndex < a_lcdString.length(); m_stringIndex++)
  {
    lcd4x20Shoo.print(a_lcdString[m_stringIndex]);
  }
}
