/**
 * File for the Menu class
 * The Menu class is a singleton class that allows
 */

#ifndef MENU_H
#define MENU_H

#include "config.h"
#include "enums.h"
#include "point2d.h"
#include "settings.h"
#include "joystick.h"
#include "lcd.h"
#include "matrix.h"
#include "soundDevice.h"
#include "highscores.h"

class Menu {
public:
  /**
   * Static method to get a pointer to the instance of the class
   * No @params
   * @return pointer to the instance of the class
   */
  static Menu *getInstance() {
    static Menu *instance = new Menu();

    return instance;
  }

  /**
   * Function that loads the menu in action, checks for menu changes and user interactions
   * Needs to be called in a loop
   * No @params
   * @return if the user requested to play the game and the menu should disappear
   */
  bool loadMenu() {
    if (requestToPlayGame) { // early exit
      return requestToPlayGame;
    }

    // change of menu
    bool swPressed = joystick->isSwitchPressed();
    if (swPressed) {
      if (settings->getIsSoundOn()) {
        soundDevice->playSound(NOTE_A4, TONE_DURATION);
      }

      bool resetMenu = changeMenu();
      if (resetMenu) {
        menuSectionIndex = 1; // reset section index on switching back and forth
        lcdNeedsUpdating = true;
        loadMenuSectionItems();
      }
    } else { // scrolling in the menu
      YDirection yDirection = joystick->detectMovementOnYAxis();

      if (yDirection != YDirection::MIDDLE) {
        if (settings->getIsSoundOn()) {
          soundDevice->playSound(NOTE_C5, TONE_DURATION);
        }
      }

      if (yDirection == YDirection::DOWN && menuSectionIndex < numbersMenuSections[currentMenu] - 1) {
        menuSectionIndex++;
        lcdNeedsUpdating = true;
      } else if (yDirection == YDirection::UP && menuSectionIndex > 1) {
        menuSectionIndex--;
        lcdNeedsUpdating = true;
      }
    }

    if (lcdNeedsUpdating) {
      showMenuSections();
      lcdNeedsUpdating = false;
    }

    // scroll on flash string message if any
    lcd->printScrollingFlashStringMessage();

    return requestToPlayGame;
  }

  /**
   * Function that resets tha state of the menu
   * No @params
   * No @return
   */
  void resetMenu() {
    currentMenu = MenuItem::MAIN;
    menuSectionIndex = 1;
    lcdNeedsUpdating = true;
    requestToPlayGame = false;
    changeMatrixSymbol();
  }

  /**
   * Function that presents the start message with theme song.
   * Need to be called in a loop
   * No @params
   * @return true if the intro is still running, false when the intro ended
   */
  bool showStartMessage() {
    static bool firstCall = true;

    if (firstCall) {
      lcMatrix->displaySnake();
      firstCall = false;
      lcd->printScrollingFlashStringMessage(true, F("Welcome! Let's play Snake!"), 1, 1);
      if (settings->getIsSoundOn()) {
        soundDevice->playSong(true);
      }
    }

    if (settings->getIsSoundOn()) {
      soundDevice->playSong();
    }
    lcd->printScrollingFlashStringMessage();

    if (millis() >= INTRO_MESSAGE_TIME_IN_MILLIS) {
      lcd->printScrollingFlashStringMessage(true, nullptr);
      return false;
    }

    return true;
  }

private:
  /**
   * Pointers to the interfaces of input & output devices to present the game
   */
  Joystick *joystick = nullptr;
  LCD *lcd = nullptr;
  Matrix *lcMatrix = nullptr;
  SoundDevice *soundDevice = nullptr;

  Settings *settings = nullptr;
  Highscores *highscores = nullptr;

  MenuItem currentMenu = MenuItem::MAIN;
  byte menuSectionIndex = 1;
  const byte numbersMenuSections[6] = {6, 0, 7, 9, 5, 3};
  __FlashStringHelper *menuSectionsMessage[9] = {nullptr};

  bool lcdNeedsUpdating = true;
  bool requestToPlayGame = false;

  /**
   * Private constructor for the singleton class
   * The constructor will get the interfaces of the input & output devices to present the game
   * and the settings & highscores to have access to the game's settings and highscores
   * Will update the devices settings using the saved settings from storage and load the first menu section items
   */
  Menu() {
    joystick = Joystick::getInstance();
    lcd = LCD::getInstance();
    lcMatrix = Matrix::getInstance();
    soundDevice = SoundDevice::getInstance();

    settings = Settings::getInstance();
    highscores = Highscores::getInstance();

    updateLcdContrast(settings->getLcdContrast());
    updateLcdBrightness(settings->getLcdBrightness());
    updateMatrixBrightness(settings->getMatrixBrightness());
    updateGameDifficulty(settings->getGameDifficulty());

    loadMenuSectionItems();
  }

  Menu(const Menu &) = delete;

  Menu &operator=(const Menu &) = delete;

  /**
   * Function that will load the current menu section's messages
   * No @params
   * No @return
   */
  void loadMenuSectionItems() {
    static const char backMessage[]
    PROGMEM = "Back";

    switch (currentMenu) {
      case MenuItem::MAIN:
        menuSectionsMessage[0] = F("<Snake!>");
        menuSectionsMessage[1] = F("Play Snake!");
        menuSectionsMessage[2] = F("Highscores");
        menuSectionsMessage[3] = F("Settings");
        menuSectionsMessage[4] = F("About");
        menuSectionsMessage[5] = F("How to play?");
        break;
      case MenuItem::PLAY:
        break;
      case MenuItem::HIGHSCORES:
        menuSectionsMessage[0] = F("<Highscores>");
        // can't save to progmem dynamically the char* for scores, will use exception in print for this
        menuSectionsMessage[6] = (__FlashStringHelper *) backMessage;
        break;
      case MenuItem::SETTINGS:
        menuSectionsMessage[0] = F("<Settings>");
        menuSectionsMessage[1] = F("Change name");
        menuSectionsMessage[2] = F("Change LCD Contrast");
        menuSectionsMessage[3] = F("Change LCD Brightness");
        menuSectionsMessage[4] = F("Change Matrix Brightness");
        menuSectionsMessage[5] = F("Change Difficulty");
        menuSectionsMessage[6] = F("Reset Highscores");
        if (settings->getIsSoundOn()) {
          menuSectionsMessage[7] = F("Sound is ON. Press SW to turn it OFF");
        } else {
          menuSectionsMessage[7] = F("Sound is OFF. Press SW to turn it ON");
        }
        menuSectionsMessage[8] = (__FlashStringHelper *) backMessage;
        break;
      case MenuItem::ABOUT:
        menuSectionsMessage[0] = F("<About>");
        menuSectionsMessage[1] = F("Title: Snake");
        menuSectionsMessage[2] = F("By: George Radu");
        menuSectionsMessage[3] = F("Github username: george-radu-cs");
        menuSectionsMessage[4] = (__FlashStringHelper *) backMessage;
        break;
      case MenuItem::HOW_TO_PLAY:
        menuSectionsMessage[0] = F("<How to play?>");
        menuSectionsMessage[1] = F(
            "Use the joystick to move ^<v>, can't move diagonally. Eat apples to grow and get points. Be carefull not to eat yourself or exit the matrix. Higher difficulty => higher score.");
        menuSectionsMessage[2] = (__FlashStringHelper *) backMessage;
        break;
      default:
        break;
    }
  }

  /**
   * Function that prints the previous & the current menu section on the LCD with helper arrows for navigation
   * No @params
   * No @return
   */
  void showMenuSections() {
    lcd->clear();

    if (currentMenu == MenuItem::HIGHSCORES) { // highscores exception
      lcd->printScrollingFlashStringMessage(true, nullptr);

      if (menuSectionIndex == 1) {
        lcd->printIndentedMessageOnRow(0, menuSectionsMessage[menuSectionIndex - 1]); // <highscores>
      } else {
        char buffer[15];
        sprintf(buffer, "<%d.%s:%03d>", menuSectionIndex - 1,
                highscores->getHighscoresDataArray()[menuSectionIndex - 2].playerName,
                highscores->getHighscoresDataArray()[menuSectionIndex - 2].score);
        buffer[14] = '\0';
        lcd->setCursorPosition(1, 0);
        lcd->printMessage(buffer);
      }

      if (menuSectionIndex == numbersMenuSections[MenuItem::HIGHSCORES] - 1) {
        lcd->printIndentedMessageOnRow(1, menuSectionsMessage[menuSectionIndex]); // back
      } else {
        char buffer[15];
        sprintf(buffer, "<%d.%s:%03d>", menuSectionIndex,
                highscores->getHighscoresDataArray()[menuSectionIndex - 1].playerName,
                highscores->getHighscoresDataArray()[menuSectionIndex - 1].score);
        buffer[14] = '\0';
        lcd->setCursorPosition(1, 1);
        lcd->printMessage(buffer);
      }
    } else { // normal menus
      // first row always fixed size
      lcd->printIndentedMessageOnRow(0, menuSectionsMessage[menuSectionIndex - 1]);

      // second row fixed size or scrolling depending on the section message length
      if (getLengthOfFlashString(menuSectionsMessage[menuSectionIndex]) <= 14) {
        lcd->printScrollingFlashStringMessage(true, nullptr);
        lcd->printIndentedMessageOnRow(1, menuSectionsMessage[menuSectionIndex]);
      } else {
        lcd->printIndentedMessageOnRow(1, menuSectionsMessage[menuSectionIndex]);
        lcd->printScrollingFlashStringMessage(true, menuSectionsMessage[menuSectionIndex], 1, 1);
      }
    }

    // print helper arrows
    lcd->printCustomCharAtPosition(0, 1, byte(RIGHT_ARROW_CHAR)); // indicate current section in menu

    // indicate ways to move in the menu
    if (menuSectionIndex > 1) {
      lcd->printCustomCharAtPosition(15, 0, byte(UP_ARROW_CHAR));
    }
    if (menuSectionIndex < numbersMenuSections[currentMenu] - 1) {
      lcd->printCustomCharAtPosition(15, 1, byte(DOWN_ARROW_CHAR));
    }
  }

  /**
   * Function that changes the menu states based on the current menu and the current section index selected
   * No @params
   * @return boolean in case the LCD needs to be updated after the change
   */
  bool changeMenu() {
    switch (currentMenu) {
      case MenuItem::MAIN:
        currentMenu = menuSectionIndex;
        if (menuSectionIndex == MenuItem::PLAY) {
          requestToPlayGame = true;
        }
        menuSectionIndex = 1;
        break;
      case MenuItem::PLAY:
        break;
      case MenuItem::HIGHSCORES:
        if (menuSectionIndex == numbersMenuSections[MenuItem::HIGHSCORES] - 1) {
          currentMenu = MenuItem::MAIN;
          menuSectionIndex = 1;
        } else {
          return false;
        }
        break;
      case MenuItem::SETTINGS:
        if (menuSectionIndex == CHANGE_NAME) {
          changePlayerNameMenu();
        } else if (menuSectionIndex == CHANGE_LCD_CONTRAST) {
          sliderMenu(settings->getLcdContrast(), MAX_LCD_CONTRAST_BLOCK_COUNT, &updateLcdContrast);
        } else if (menuSectionIndex == CHANGE_LCD_BRIGHTNESS) {
          sliderMenu(settings->getLcdBrightness(), MAX_LCD_BRIGHTNESS_BLOCK_COUNT, &updateLcdBrightness);
        } else if (menuSectionIndex == CHANGE_MATRIX_BRIGHTNESS) {
          lcMatrix->activateAll();
          sliderMenu(settings->getMatrixBrightness(), MAX_MATRIX_BRIGHTNESS_BLOCK_COUNT, &updateMatrixBrightness);
        } else if (menuSectionIndex == CHANGE_DIFFICULTY) {
          sliderMenu(settings->getGameDifficulty(), MAX_DIFFICULTY_BLOCK_COUNT, &updateGameDifficulty);
        } else if (menuSectionIndex == RESET_HIGHSCORES) {
          highscores->resetHighscores();
        } else if (menuSectionIndex == CHANGE_SOUND_ON_OFF) {
          settings->setIsSoundOn(!settings->getIsSoundOn());
          settings->saveInStorage();
        } else if (menuSectionIndex == numbersMenuSections[MenuItem::SETTINGS] - 1) {
          currentMenu = MenuItem::MAIN;
          menuSectionIndex = 1;
        }
        break;
      case MenuItem::ABOUT:
        if (menuSectionIndex == numbersMenuSections[MenuItem::ABOUT] - 1) {
          currentMenu = MenuItem::MAIN;
          menuSectionIndex = 1;
        } else {
          return false;
        }
        break;
      case MenuItem::HOW_TO_PLAY:
        if (menuSectionIndex == 1) {
          return false;
        }
        if (menuSectionIndex == numbersMenuSections[MenuItem::HOW_TO_PLAY] - 1) {
          currentMenu = MenuItem::MAIN;
          menuSectionIndex = 1;
        }
        break;
      default:
        break;
    }

    changeMatrixSymbol();
    return true;
  }

  /**
   * Function that enters the menu for changing the player name menu.
   * The menu is running in a loop and requires the user to save the name before exiting.
   * No @params
   * No @return
   */
  void changePlayerNameMenu() {
    // print current name saved and indications to use the menu
    int padding = (LCD_DISPLAY_WIDTH - MAX_PLAYER_NAME_LENGTH) / 2;
    lcd->clear();
    lcd->setCursorPosition(padding - 1, 0);
    lcd->printMessage("<");
    lcd->printMessage(settings->getPlayerName());
    lcd->setCursorPosition(LCD_DISPLAY_WIDTH - padding, 0);
    lcd->printMessage(">");
    lcd->setCursorPosition(0, 1);
    lcd->printMessage("Press SW to save");
    lcd->setCursorPosition(padding, 0);
    lcd->showCursor();

    int letterIndex = 0;
    char name[MAX_PLAYER_NAME_LENGTH + 1];
    strcpy(name, settings->getPlayerName());

    while (!joystick->isSwitchPressed()) { // user didn't confirm the save of the player name
      XDirection xDirection = joystick->detectMovementOnXAxis();
      YDirection yDirection = joystick->detectMovementOnYAxis();
      if (xDirection != XDirection::MIDDLE) {
        letterIndex += xDirection == XDirection::RIGHT ? 1 : -1;
        if (letterIndex < 0) {
          letterIndex = 0;
        } else if (letterIndex >= MAX_PLAYER_NAME_LENGTH) {
          letterIndex = MAX_PLAYER_NAME_LENGTH - 1;
        }
        lcd->setCursorPosition(letterIndex + padding, 0);
      } else if (yDirection != YDirection::MIDDLE) {
        name[letterIndex] += yDirection == YDirection::DOWN ? 1 : -1;
        // nice changes between ASCII characters
        switch (name[letterIndex]) {
          case ' ' - 1:
            name[letterIndex] = 'z';
            break;
          case ' ' + 1:
            name[letterIndex] = '0';
            break;
          case '0' - 1:
            name[letterIndex] = ' ';
            break;
          case '9' + 1:
            name[letterIndex] = 'A';
            break;
          case 'A' - 1:
            name[letterIndex] = '9';
            break;
          case 'Z' + 1:
            name[letterIndex] = 'a';
            break;
          case 'a' - 1:
            name[letterIndex] = 'Z';
            break;
          case 'z' + 1:
            name[letterIndex] = ' ';
            break;
          default:
            break;
        }
        lcd->printMessage(name[letterIndex]);
        lcd->setCursorPosition(letterIndex + padding, 0);
      }
    }

    if (settings->getIsSoundOn()) {
      soundDevice->playSound(NOTE_A4, TONE_DURATION);
    }

    settings->setPlayerName(name);
    settings->saveInStorage();
    lcd->hideCursor();
  }

  /**
   * Function that enters a customizable slider menu for a setting byte info
   * The menu is running in a loop and requires the user to save the state before exiting.
   * @param activeBlockCount - the slider's level of the setting saved in storage
   * @param maxBlockCount - the maximum blocks on the slider
   * @param updateSetting - function that will update the setting on slider changes
   */
  void sliderMenu(byte activeBlockCount, const byte maxBlockCount, void (Menu::*updateSetting)(byte)) {
    // print the current setting saved and indications to use the menu
    lcd->clear();
    byte padding = (LCD_DISPLAY_WIDTH - 2 - maxBlockCount) / 2;
    lcd->setCursorPosition(padding, 0);
    lcd->printMessage(F("-"));
    for (byte i = 0; i < activeBlockCount; i++) {
      lcd->printCustomChar(byte(FULL_BLOCK_CHAR));
    }
    for (byte i = activeBlockCount; i < maxBlockCount; i++) {
      lcd->printMessage(F(" "));
    }
    lcd->printMessage(F("+"));
    lcd->setCursorPosition(0, 1);
    lcd->printMessage(F("Press SW to save"));

    while (!joystick->isSwitchPressed()) { // user didn't confirm the save of the new setting
      XDirection xDirection = joystick->detectMovementOnXAxis();
      if (xDirection == XDirection::RIGHT && activeBlockCount < maxBlockCount) {
        lcd->setCursorPosition(padding + 1 + activeBlockCount, 0);
        lcd->printCustomChar(byte(FULL_BLOCK_CHAR));
        activeBlockCount++;
      } else if (xDirection == XDirection::LEFT && activeBlockCount > 1) {
        lcd->setCursorPosition(padding + activeBlockCount, 0);
        lcd->printMessage(F(" "));
        activeBlockCount--;
      }

      if (xDirection != XDirection::MIDDLE) { // on setting change
        (this->*updateSetting)(activeBlockCount);

        if (settings->getIsSoundOn()) {
          soundDevice->playSound(NOTE_F5, TONE_DURATION);
        }
      }
    }

    if (settings->getIsSoundOn()) {
      soundDevice->playSound(NOTE_A4, TONE_DURATION);
    }

    settings->saveInStorage();
  }

  /**
   * Function that updates the LCD contrast in the settings data and on the LCD
   * @param lcdContrast
   * No @return
   */
  void updateLcdContrast(byte lcdContrast) {
    settings->setLcdContrast(lcdContrast);
    int value = map(lcdContrast, MIN_SLIDER_BLOCK_COUNT, MAX_LCD_CONTRAST_BLOCK_COUNT, MIN_CONTRAST_VALUE,
                    MAX_CONTRAST_VALUE);
    lcd->setContrast(value);
  }

  /**
   * Function that updates the LCD brightness in the settings data and on the LCD
   * @param lcdBrightness
   * No @return
   */
  void updateLcdBrightness(byte lcdBrightness) {
    settings->setLcdBrightness(lcdBrightness);
    int value = map(lcdBrightness, MIN_SLIDER_BLOCK_COUNT, MAX_LCD_BRIGHTNESS_BLOCK_COUNT, MIN_LCD_BRIGHTNESS_VALUE,
                    MAX_LCD_BRIGHTNESS_VALUE);
    lcd->setBrightness(value);
  }

  /**
   * Function that updates the LC Matrix brightness in the settings data and on the lc matrix
   * @param matrixBrightness
   * No @return
   */
  void updateMatrixBrightness(byte matrixBrightness) {
    settings->setMatrixBrightness(matrixBrightness);
    int value = map(matrixBrightness, MIN_SLIDER_BLOCK_COUNT, MAX_MATRIX_BRIGHTNESS_BLOCK_COUNT,
                    MIN_MATRIX_BRIGHTNESS_VALUE, MAX_MATRIX_BRIGHTNESS_VALUE);
    lcMatrix->setBrightness(value);
  }

  /**
   * Function that updates the game difficulty level in the settings data
   * @param gameDifficulty
   * No @return
   */
  void updateGameDifficulty(byte gameDifficulty) {
    settings->setGameDifficulty(gameDifficulty);
  }

  /**
   * Function that updates the symbol on the LC matrix depending on the currentMenu
   * No @params
   * No @return
   */
  void changeMatrixSymbol() {
    switch (currentMenu) {
      case MenuItem::MAIN:
        lcMatrix->displaySnake();
        break;
      case MenuItem::PLAY:
        lcMatrix->displayPlay();
        break;
      case MenuItem::HIGHSCORES:
        lcMatrix->displayHighScore();
        break;
      case MenuItem::SETTINGS:
        lcMatrix->displaySettings();
        break;
      case MenuItem::ABOUT:
        lcMatrix->displayAboutMe();
        break;
      case MenuItem::HOW_TO_PLAY:
        lcMatrix->displayQuestionMark();
        break;
      default:
        break;
    }
  }
};

#endif