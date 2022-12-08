/**
 * File for the Led 8x8 Matrix class
 * The Matrix class is a singleton class that allows default control settings on a 8x8 LC Matrix and displaying custom
 * 8x8 configurations
 */

#ifndef MATRIX_H
#define MATRIX_H

#include "LedControl.h"
#include "config.h"

class Matrix {
public:
  /**
   * Static method to get a pointer to the instance of the class
   * No @params
   * @return pointer to the instance of the class
   */
  static Matrix *getInstance() {
    static Matrix *instance = new Matrix();

    return instance;
  }

  /**
   * Function that sets the brightness of the matrix
   * @param brightness - the brightness to set the matrix to
   * No @return
   */
  void setBrightness(const byte &value) {
    lc.setIntensity(0, value);
  }

  /**
   * Function that clears the display - turns off all the leds
   * No @params
   * No @return
   */
  void clearDisplay() {
    lc.clearDisplay(0);
  }

  /**
   * Functions that sets the state of a led on the matrix
   * @param row - the row of the led to set
   * @param col - the column of the led to set
   * @param state - the state to set the led to
   * No @return
   */
  void setLed(const byte row, const byte col, const bool state) {
    lc.setLed(0, row, col, state);
  }

  /**
   * Functions that turns on all the LEDs on the matrix
   * No @params
   * No @return
   */
  void activateAll() {
    for (byte i = 0; i < 8; i++) {
      lc.setRow(0, i, MAX_DIGITAL_OUTPUT_VALUE);
    }
  }

  /**
   * Function that displays a snake symbol on the matrix
   * No @params
   * No @return
   */
  void displaySnake() {
    byte symbol[MATRIX_SIZE] = {
        B00000000,
        B01110010,
        B01000111,
        B01100010,
        B00110000,
        B00010000,
        B01110000,
        B00000000
    };

    displaySymbol(symbol);
  };

  /**
   * Function that displays a play symbol on the matrix
   * No @params
   * No @return
   */
  void displayPlay() {
    byte symbol[MATRIX_SIZE] = {
        B00100000,
        B00110000,
        B00111000,
        B00111100,
        B00111100,
        B00111000,
        B00110000,
        B00100000
    };

    displaySymbol(symbol);
  }

  /**
   * Function that displays a settings symbol on the matrix
   * No @params
   * No @return
   */
  void displaySettings() {
    byte symbol[MATRIX_SIZE] = {
        B00011000,
        B01111110,
        B01100110,
        B11011011,
        B11011011,
        B01100110,
        B01111110,
        B00011000
    };

    displaySymbol(symbol);
  }

  /**
   * Function that displays an me symbol on the matrix
   * No @params
   * No @return
   */
  void displayAboutMe() {
    byte symbol[MATRIX_SIZE] = {
        B10001000,
        B11011010,
        B10101000,
        B10001111,
        B10001000,
        B00001111,
        B01001000,
        B00001111
    };

    displaySymbol(symbol);
  }

  /**
   * Function that displays a question mark(?) symbol on the matrix
   * No @params
   * No @return
   */
  void displayQuestionMark() {
    byte symbol[MATRIX_SIZE] = {
        B00111110,
        B01100110,
        B01100110,
        B00001100,
        B00011000,
        B00011000,
        B00000000,
        B00011000
    };

    displaySymbol(symbol);
  }

  /**
   * Function that displays a cup symbol on the matrix
   * No @params
   * No @return
   */
  void displayHighScore() {
    byte symbol[MATRIX_SIZE] = {
        B11111111,
        B01111110,
        B01111110,
        B00111100,
        B00111100,
        B00011000,
        B00011000,
        B00111100
    };

    displaySymbol(symbol);
  }

  /**
   * Function that displays a happy face symbol on the matrix
   * No @params
   * No @return
   */
  void displayHappyFace() {
    byte symbol[MATRIX_SIZE] = {
        B00000000,
        B00100100,
        B00100100,
        B00100100,
        B00000000,
        B01000010,
        B00111100,
        B00000000
    };

    displaySymbol(symbol);
  }

  /**
   * Function that displays a sad face symbol on the matrix
   * No @params
   * No @return
   */
  void displaySadFace() {
    byte symbol[MATRIX_SIZE] = {
        B00000000,
        B00100100,
        B00100100,
        B00100100,
        B00000000,
        B00111100,
        B01000010,
        B00000000
    };

    displaySymbol(symbol);
  }

  /**
   * Function that displays number one on the matrix
   * No @params
   * No @return
   */
  void displayOne() {
    byte symbol[MATRIX_SIZE] = {
        B00000000,
        B00000100,
        B00001100,
        B00010100,
        B00000100,
        B00000100,
        B00000100,
        B00000100
    };

    displaySymbol(symbol);
  }

  /**
   * Function that displays number two on the matrix
   * No @params
   * No @return
   */
  void displayTwo() {
    byte symbol[MATRIX_SIZE] = {
        B00000000,
        B00111100,
        B00000100,
        B00000100,
        B00111100,
        B00100000,
        B00100000,
        B00111100
    };

    displaySymbol(symbol);
  }

  /**
   * Function that displays number three on the matrix
   * No @params
   * No @return
   */
  void displayThree() {
    byte symbol[MATRIX_SIZE] = {
        B00000000,
        B00111100,
        B00000100,
        B00000100,
        B00111100,
        B00000100,
        B00000100,
        B00111100
    };

    displaySymbol(symbol);
  }

private:
  // object interface to control the lc matrix from the LedControl library
  LedControl lc = LedControl(MATRIX_DIN_PIN, MATRIX_CLOCK_PIN, MATRIX_LOAD_PIN, MATRIX_NUM_DRIVER);

  /**
   * Private constructor for the singleton class
   * The constructor will set the LC Matrix to the default state
   */
  Matrix() {
    lc.shutdown(0, false);
    lc.clearDisplay(0);
  }

  Matrix(const Matrix &) = delete;

  Matrix &operator=(const Matrix &) = delete;

  /**
   * Function that displays a symbol to the matrix
   * @param symbol - the byte representation of the symbol
   * No @return
   */
  void displaySymbol(byte symbol[]) {
    for (byte i = 0; i < MATRIX_SIZE; i++) {
      lc.setRow(0, i, symbol[i]);
    }
  }

};

#endif