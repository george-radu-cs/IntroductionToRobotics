/**
 * File for the LCD class
 * The LCD class is a singleton class that allows safe custom printing methods on the lcd using cstrings either from
 * RAM or the Flash memory. Also allow methods to interact with the LiquidCrystal library object proprieties, since
 * the library object is hidden from the user, to avoid direct printing
 */

#ifndef LCD_H
#define LCD_H

#include <LiquidCrystal.h>
#include "lcdCharacters.h"
#include "config.h"
#include "utils.h"

class LCD {
public:
  /**
   * Static method to get a pointer to the instance of the class
   * No @params
   * @return pointer to the instance of the class
   */
  static LCD *getInstance() {
    static LCD *instance = new LCD();

    return instance;
  }

  /**
   * Function that sets the brightness of the LCD backlight
   * @param value - the value to set the brightness to
   * No @return
   */
  void setBrightness(const int value) {
    analogWrite(LCD_INTENSITY_PIN, value);
  }

  /**
   * Function that sets the contrast of the LCD
   * @param value - the value to set the contrast to
   * No @return
   */
  void setContrast(const int value) {
    analogWrite(LCD_CONTRAST_PIN, value);
  }

  /**
   * Function that clears all the content on the LCD
   * No @params
   * No @return
   */
  void clear() {
    lcd.clear();
  }

  /**
   * Function that will print the cursor position on the LCD (as a bottom line on the cursor position block)
   * No @params
   * No @return
   */
  void showCursor() {
    lcd.cursor();
  }

  /**
   * Function that will hide the cursor on the LCD
   * No @params
   * No @return
   */
  void hideCursor() {
    lcd.noCursor();
  }

  /**
   * Function that will set the cursor position on the LCD
   * @param col - the column to set the cursor to
   * @param row - the row to set the cursor to
   * No @return
   */
  void setCursorPosition(const byte &col, const byte &row) {
    lcd.setCursor(col, row);
  }

  /**
   * Blocking Function that will animate a loading bar on the LCD for 1 second. Uses delay for the animation
   * No @params
   * No @return
   */
  void animateOneSecondLCDLoadingWithDelay() {
    for (byte i = 0; i < 4; i++) {
      delay(QUARTER_SECOND_IN_MILLIS);
      printCustomChar(byte(FULL_BLOCK_CHAR));
    }
  }

  /**
   * Function that will print a custom saved character on the LCD given it's byte value
   * @param c - the character byte encoding to print
   * No @return
   */
  void printCustomChar(const byte &c) {
    lcd.write(c);
  }

  /**
   * Function that will print a custom saved character on the LCD given it's byte value at a given position
   * @param col - the column to print the character to
   * @param row - the row to print the character to
   * @param c - the character byte encoding to print
   * No @return
   */
  void printCustomCharAtPosition(const byte col, const byte row, const byte &c) {
    setCursorPosition(col, row);
    lcd.write(c);
  }

  /**
   * Function that will print a character on the LCD
   * @param message - char reference to print
   * No @return
   */
  void printMessage(const char &message) {
    lcd.print(message);
  }

  /**
   * Function that will print a message from the RAM on the LCD
   * @param message - char pointer to the message to print
   * No @return
   */
  void printMessage(const char *message) {
    lcd.print(message);
  }

  /**
   * Function that will print a message from the Flash memory on the LCD
   * @param message - pointer to the message from the Flash memory to print
   * No @return
   */
  void printMessage(const __FlashStringHelper *message) {
    lcd.print(message);
  }

  /**
   * Function that will print a message from the Flash memory on the LCD at a given row with the default LCD left and
   * right indentation. If the message is longer than the LCD width - the 2 indentations, it will be truncated to fit
   * @param row - the row to print the message to
   * @param message - pointer to the message from the Flash memory to print
   * No @return
   */
  void printIndentedMessageOnRow(byte row, __FlashStringHelper *message) {
    // print an indented message on the selected row padded indented left and right by LCD_INDENT_CHARS
    lcd.setCursor(LCD_INDENT_CHARS, row);

    // cut the message to now allow overflow printing on the lcd
    char outputCutMessage[LCD_DISPLAY_WIDTH - LCD_INDENT_CHARS];
    strncpy_P(outputCutMessage, (const char *) message, LCD_DISPLAY_WIDTH - 2 * LCD_INDENT_CHARS);
    outputCutMessage[LCD_DISPLAY_WIDTH - 2 * LCD_INDENT_CHARS] = '\0';
    lcd.print(outputCutMessage);
  }

  /**
   * Function that will scroll a message on a single row at a custom position and with a custom scrollable output length
   * without always sending the message to be scrolled
   * Needs to be called in a loop to allow the scrolling
   * @param resetMessage - boolean to decide if the function will reset it's saved scrolling data
   * @param message - pointer to a message of type __FlashStringHelper, to be able to use the flash memory, need to be
   * set on resetMessage, ignored otherwise
   * @param col - column to start the scrolling print, need to be set on resetMessage, ignored otherwise
   * @param row - row to start the scrolling print, need to be set on resetMessage, ignored otherwise
   * @param maxCutLength - size of the cutted message to be print at a time on the lcd
   * No @return
   */
  void printScrollingFlashStringMessage(bool resetMessage = false, __FlashStringHelper *message = nullptr,
                                        const byte col = 1, const byte row = 1,
                                        const byte maxCutLength = LCD_DEFAULT_SCROLL_CUT_LENGTH) {
    static char *paddedMessage = nullptr;
    static unsigned long lastScrollTime = 0;
    static byte paddedMessageIndex = 0;
    static byte sCol = col;
    static byte sRow = row;
    static byte sMaxCutLength = maxCutLength;

    if (resetMessage) {
      lastScrollTime = millis();
      paddedMessageIndex = 0;
      sCol = col;
      sRow = row;
      sMaxCutLength = maxCutLength;

      // free the memory for the last saved scrollable message
      delete[] paddedMessage;
      paddedMessage = nullptr;

      if (message) {
        paddedMessage = new char[getLengthOfFlashString(message) + 3]; // message length + spacing
        // copy from the flash memory into the RAM
        strncpy_P(paddedMessage, (char *) message, getLengthOfFlashString(message));
        paddedMessage[getLengthOfFlashString(message)] = ' ';
        paddedMessage[getLengthOfFlashString(message) + 1] = ' ';
        paddedMessage[getLengthOfFlashString(message) + 2] = '\0';
      }
    }

    if (!paddedMessage) { // didn't set any message for scroll printing
      return;
    }

    // animate a step for the scrolling message
    if (millis() - lastScrollTime >= PRINT_MESSAGE_SCROLL_DELAY) {
      lcd.setCursor(sCol, sRow);

      // construct the scrolling outputCutMessage to be printed
      char outputCutMessage[sMaxCutLength + 1];
      if (paddedMessageIndex + sMaxCutLength <
          strlen(paddedMessage)) { // cut message output contained straight by the padded message
        strncpy(outputCutMessage, &paddedMessage[paddedMessageIndex], sMaxCutLength);
      } else { // cut message reached the ending part of the padded message, need to concatenate the starting part create looping scroll effect
        strncpy(outputCutMessage, &paddedMessage[paddedMessageIndex], strlen(paddedMessage) - paddedMessageIndex);
        strncpy(&outputCutMessage[strlen(paddedMessage) - paddedMessageIndex], paddedMessage,
                paddedMessageIndex + sMaxCutLength - strlen(paddedMessage));
      }
      outputCutMessage[sMaxCutLength] = '\0';
      lcd.print(outputCutMessage);

      paddedMessageIndex++;
      lastScrollTime = millis();
      if (paddedMessageIndex == strlen(paddedMessage)) { // avoid overflow by scrolling too much
        paddedMessageIndex = 0;
      }
    }
  }

  /**
    * Function that will scroll a message on a single row at a custom position and with a custom scrollable output length
    * without always sending the message to be scrolled
    * Needs to be called in a loop to allow the scrolling
    * @param resetMessage - boolean to decide if the function will reset it's saved scrolling data
    * @param message - pointer to a message of type const char *, to be able to use the RAM, need to be set on
    * resetMessage, ignored otherwise
    * @param col - column to start the scrolling print, need to be set on resetMessage, ignored otherwise
    * @param row - row to start the scrolling print, need to be set on resetMessage, ignored otherwise
    * @param maxCutLength - size of the cutted message to be print at a time on the lcd
    * No @return
    */
  void printScrollingMessage(bool resetMessage = false, const char *message = nullptr, const byte col = 1,
                             const byte row = 1,
                             const byte maxCutLength = LCD_DEFAULT_SCROLL_CUT_LENGTH) {
    static char *paddedMessage = nullptr;
    static unsigned long lastScrollTime = 0;
    static byte paddedMessageIndex = 0;
    static byte sCol = col;
    static byte sRow = row;
    static byte sMaxCutLength = maxCutLength;

    if (resetMessage) {
      lastScrollTime = millis();
      paddedMessageIndex = 0;
      sCol = col;
      sRow = row;
      sMaxCutLength = maxCutLength;

      // free the memory for the last saved scrollable message
      delete[] paddedMessage;
      paddedMessage = nullptr;

      if (message) {
        paddedMessage = new char[strlen(message) + 3]; // message length + spacing
        strncpy(paddedMessage, message, strlen(message));
        paddedMessage[strlen(message)] = ' ';
        paddedMessage[strlen(message) + 1] = ' ';
        paddedMessage[strlen(message) + 2] = '\0';
      }
    }

    if (!paddedMessage) { // didn't set any message for scroll printing
      return;
    }

    // construct the scrolling outputCutMessage to be printed
    if (millis() - lastScrollTime >= PRINT_MESSAGE_SCROLL_DELAY) {
      lcd.setCursor(sCol, sRow);
      char outputCutMessage[sMaxCutLength + 1];
      if (paddedMessageIndex + sMaxCutLength <
          strlen(paddedMessage)) { // cut message output contained straight by the padded message
        strncpy(outputCutMessage, &paddedMessage[paddedMessageIndex], sMaxCutLength);
      } else { // cut message reached the ending part of the padded message, need to concatenate the starting part create looping scroll effect
        strncpy(outputCutMessage, &paddedMessage[paddedMessageIndex], strlen(paddedMessage) - paddedMessageIndex);
        strncpy(&outputCutMessage[strlen(paddedMessage) - paddedMessageIndex], paddedMessage,
                paddedMessageIndex + sMaxCutLength - strlen(paddedMessage));
      }
      outputCutMessage[sMaxCutLength] = '\0';
      lcd.print(outputCutMessage);

      paddedMessageIndex++;
      lastScrollTime = millis();
      if (paddedMessageIndex == strlen(paddedMessage)) { // avoid overflow by scrolling too much
        paddedMessageIndex = 0;
      }
    }
  }

private:
  // object interface to control the lcd from the LiquidCrystal library
  LiquidCrystal lcd = LiquidCrystal(LCD_RS, LCD_ENABLE, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

  /**
   * Private constructor for the singleton class
   * The constructor will set the LCD pins, create the custom characters and set the LCD to the default state
   */
  LCD() {
    // setup lcd pins
    pinMode(LCD_INTENSITY_PIN, OUTPUT);
    pinMode(LCD_CONTRAST_PIN, OUTPUT);

    // create custom chars for the lcd
    createFullBlockChar(lcd);
    createCupChar(lcd);
    createUpArrowChar(lcd);
    createLeftArrowChar(lcd);
    createDownArrowChar(lcd);
    createRightArrowChar(lcd);

    // lcd init
    lcd.begin(LCD_DISPLAY_WIDTH, LCD_DISPLAY_HEIGHT);
    lcd.clear();
    lcd.noCursor();
    lcd.noBlink();
  }

  LCD(const LCD &) = delete;

  LCD &operator=(const LCD &) = delete;
};

#endif
