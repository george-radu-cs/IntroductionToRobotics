/**
 * File defining functions to create custom characters to draw on an LCD
 * each custom character has 8 rows and 5 columns
 */

#ifndef LCD_CHARACTERS_H
#define LCD_CHARACTERS_H

#define FULL_BLOCK_CHAR 0

void createFullBlockChar(LiquidCrystal &lcd) {
  static const byte fullBlockEncoding[8]
  PROGMEM = {
      B11111,
      B11111,
      B11111,
      B11111,
      B11111,
      B11111,
      B11111,
      B11111
  };
  byte charEncoding[8];
  memcpy_P(charEncoding, fullBlockEncoding, sizeof(charEncoding));
  lcd.createChar(FULL_BLOCK_CHAR, charEncoding);
}

#define CUP_CHAR 1

void createCupChar(LiquidCrystal &lcd) {
  static const byte cupEncoding[8]
  PROGMEM = {
      B11111,
      B11111,
      B01110,
      B01110,
      B00100,
      B00100,
      B01110,
      B11111
  };
  byte charEncoding[8];
  memcpy_P(charEncoding, cupEncoding, sizeof(charEncoding));
  lcd.createChar(CUP_CHAR, charEncoding);
}

#define UP_ARROW_CHAR 2

void createUpArrowChar(LiquidCrystal &lcd) {
  static const byte upArrowEncoding[8]
  PROGMEM = {
      B00000,
      B00100,
      B01110,
      B11111,
      B00100,
      B00100,
      B00100,
      B00000
  };
  byte charEncoding[8];
  memcpy_P(charEncoding, upArrowEncoding, sizeof(charEncoding));
  lcd.createChar(UP_ARROW_CHAR, charEncoding);
}

#define LEFT_ARROW_CHAR 3

void createLeftArrowChar(LiquidCrystal &lcd) {
  static const byte leftArrowEncoding[8]
  PROGMEM = {
      B00001,
      B00011,
      B00111,
      B01111,
      B01111,
      B00111,
      B00011,
      B00001
  };
  byte charEncoding[8];
  memcpy_P(charEncoding, leftArrowEncoding, sizeof(charEncoding));
  lcd.createChar(LEFT_ARROW_CHAR, charEncoding);
}

#define DOWN_ARROW_CHAR 4

void createDownArrowChar(LiquidCrystal &lcd) {
  static const byte downArrowEncoding[8]
  PROGMEM = {
      B00000,
      B00100,
      B00100,
      B00100,
      B11111,
      B01110,
      B00100,
      B00000
  };
  byte charEncoding[8];
  memcpy_P(charEncoding, downArrowEncoding, sizeof(charEncoding));
  lcd.createChar(DOWN_ARROW_CHAR, charEncoding);
}

#define RIGHT_ARROW_CHAR 5

void createRightArrowChar(LiquidCrystal &lcd) {
  static const byte rightArrowEncoding[8]
  PROGMEM = {
      B10000,
      B11000,
      B11100,
      B11110,
      B11110,
      B11100,
      B11000,
      B10000
  };
  byte charEncoding[8];
  memcpy_P(charEncoding, rightArrowEncoding, sizeof(charEncoding));
  lcd.createChar(RIGHT_ARROW_CHAR, charEncoding);
}

#define HEART_CHAR 6

void createHeartChar(LiquidCrystal &lcd) {
  static const byte heartEncoding[8]
  PROGMEM = {
      B10001,
      B00000,
      B01010,
      B11111,
      B01110,
      B00100,
      B00000,
      B10001
  };
  byte charEncoding[8];
  memcpy_P(charEncoding, heartEncoding, sizeof(charEncoding));
  lcd.createChar(HEART_CHAR, charEncoding);
}

#endif