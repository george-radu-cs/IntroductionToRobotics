/**
 * File defining functions to create custom characters to draw on an LCD
 * each custom character has 8 rows and 5 columns
 */

#ifndef LCD_CHARACTERS_H
#define LCD_CHARACTERS_H

#define FULL_BLOCK_CHAR 0

void createFullBlockChar(LiquidCrystal &lcd) {
  byte fullBlockEncoding[8] = {
      B11111,
      B11111,
      B11111,
      B11111,
      B11111,
      B11111,
      B11111,
      B11111
  };
  lcd.createChar(FULL_BLOCK_CHAR, fullBlockEncoding);
}

#define CUP_CHAR 1

void createCupChar(LiquidCrystal &lcd) {
  byte cupEncoding[8] = {
      B11111,
      B11111,
      B01110,
      B01110,
      B00100,
      B00100,
      B01110,
      B11111
  };
  lcd.createChar(CUP_CHAR, cupEncoding);
}

#define UP_ARROW_CHAR 2

void createUpArrowChar(LiquidCrystal &lcd) {
  byte upArrowEncoding[8] = {
      B00000,
      B00100,
      B01110,
      B11111,
      B00100,
      B00100,
      B00100,
      B00000
  };
  lcd.createChar(UP_ARROW_CHAR, upArrowEncoding);
}

#define LEFT_ARROW_CHAR 3

void createLeftArrowChar(LiquidCrystal &lcd) {
  byte leftArrowEncoding[8] = {
      B00001,
      B00011,
      B00111,
      B01111,
      B01111,
      B00111,
      B00011,
      B00001
  };
  lcd.createChar(LEFT_ARROW_CHAR, leftArrowEncoding);
}

#define DOWN_ARROW_CHAR 4

void createDownArrowChar(LiquidCrystal &lcd) {
  byte downArrowEncoding[8] = {
      B00000,
      B00100,
      B00100,
      B00100,
      B11111,
      B01110,
      B00100,
      B00000
  };
  lcd.createChar(DOWN_ARROW_CHAR, downArrowEncoding);
}

#define RIGHT_ARROW_CHAR 5

void createRightArrowChar(LiquidCrystal &lcd) {
  byte rightArrowEncoding[8] = {
      B10000,
      B11000,
      B11100,
      B11110,
      B11110,
      B11100,
      B11000,
      B10000
  };
  lcd.createChar(RIGHT_ARROW_CHAR, rightArrowEncoding);
}

#endif