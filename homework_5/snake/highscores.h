/**
 * File for the highscores board class
 * The Highscores class is a singleton class that allows access to the highscores board, and allow methods for updating
 * the board and save it in storage
 */

#ifndef HIGHSCORES_H
#define HIGHSCORES_H

#include <EEPROM.h>
#include "settings.h"

class Highscores {
public:
  /**
   * struct for highscore entry for easier access to the data, and passing it around
   * it allows for easier access to the data in the EEPROM
   * also defined an operator for assigning the struct variable to another struct variable
   */
  struct HighscoresData {
    int score;
    char playerName[MAX_PLAYER_NAME_LENGTH + 1];

    HighscoresData &operator=(const HighscoresData &other) {
      score = other.score;
      strcpy(playerName, other.playerName);
    }
  };

  // define special type to return the array of highscores
  typedef HighscoresData HighscoresDataArray[NUMBER_OF_HIGHSCORES_SAVED];

  /**
   * Static method to get a pointer to the instance of the class
   * No @params
   * @return pointer to the instance of the class
   */
  static Highscores *getInstance() {
    static Highscores *instance = new Highscores();

    return instance;
  }

  /**
   * Getter method to get the highscores data array
   * No @params
   * @return a const reference to the highscores data array
   */
  const HighscoresDataArray &getHighscoresDataArray() const {
    return highscoresData;
  }

  /**
   * Function that computes the place a score would be in the highscores board
   * @param score the score to check
   * @return the place the score would be in the highscores board
   */
  byte getNewHighscorePlace(int score) {
    byte place = 0;
    while (place < NUMBER_OF_HIGHSCORES_SAVED) {
      if (score > highscoresData[place].score) {
        break;
      }
      place++;
    }
    return place;
  }

  /**
   * Function that updates the highscores board with a new score and player name.
   * If the score is not high enough to be in the highscores board, it will not be added
   * The function will also save the new highscores board in the EEPROM
   * @param score
   * @param playerName
   * No @return
   */
  void updateHighscores(const int score, const char *playerName) {
    // find the new index where the score should be inserted
    byte placeIndex = getNewHighscorePlace(score);
    if (placeIndex >= NUMBER_OF_HIGHSCORES_SAVED) {
      return; // didn't achieve a better score to be saved on the board
    }

    // shift the highscores down one position to make room for the new score
    for (byte i = NUMBER_OF_HIGHSCORES_SAVED - 1; i > placeIndex; i--) {
      highscoresData[i] = highscoresData[i - 1];
    }

    // insert the new highscore and the player name in the correct position
    highscoresData[placeIndex].score = score;
    strcpy(highscoresData[placeIndex].playerName, playerName);

    saveInStorage();
  }

  /**
   * Function that resets the highscores board to the default values.
   * The default values are all scores of 0 achieved by "NO ONE"
   * The function will also save the new highscores board in the EEPROM
   * No @params
   * No @return
   */
  void resetHighscores() {
    for (byte i = 0; i < NUMBER_OF_HIGHSCORES_SAVED; i++) {
      highscoresData[i].score = 0;
      strcpy(highscoresData[i].playerName, "NO ONE");
    }
    saveInStorage();
  }

  /**
   * Function that loads the highscores board from the EEPROM
   * The highscores are saved in EEPROM after the settings data
   * No @params
   * No @return
   */
  void loadFromStorage() {
    EEPROM.get(Settings::sizeOfSettingsData(), highscoresData);
  }

  /**
   * Function that saves the highscores board in the EEPROM
   * The highscores will be saved in EEPROM after the settings data
   * No @params
   * No @return
   */
  void saveInStorage() {
    EEPROM.put(Settings::sizeOfSettingsData(), highscoresData);
  }

private:
  // array of highscores data board
  HighscoresData highscoresData[NUMBER_OF_HIGHSCORES_SAVED];

  /**
   * Private constructor for the singleton class
   * The constructor will load the highscores board from the storage
   */
  Highscores() {
    loadFromStorage();
  }

  Highscores(const Highscores &) = delete;

  Highscores &operator=(const Highscores &) = delete;
};

#endif