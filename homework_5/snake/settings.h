/**
 * File for the Settings class
 * The Settings class is a singleton class that allows access to the settings data of the game,
 * save/load to/from storage
 */

#ifndef SETTINGS_H
#define SETTINGS_H

#include <EEPROM.h>

class Settings {
public:
  /**
   * struct for settings data for easier access to the data, and passing it around
   * it allows for easier access to the data in the EEPROM
   */
  struct SettingsData {
    byte lcdContrast;
    byte lcdBrightness;
    byte matrixBrightness;
    byte gameDifficulty;
    bool isSoundOn;
    char playerName[MAX_PLAYER_NAME_LENGTH + 1];
  };

  /**
   * Static method to get a pointer to the instance of the class
   * No @params
   * @return pointer to the instance of the class
   */
  static Settings *getInstance() {
    static Settings *instance = new Settings();

    return instance;
  }

  /**
   * Function that returns the size needed to save the settings data
   * No @params
   * @return size of the settings data
   */
  static unsigned int sizeOfSettingsData() {
    return sizeof(settingsData);
  }

  /* getters for the settings data */
  byte getLcdContrast() const {
    return settingsData.lcdContrast;
  }

  byte getLcdBrightness() const {
    return settingsData.lcdBrightness;
  }

  byte getMatrixBrightness() const {
    return settingsData.matrixBrightness;
  }

  byte getGameDifficulty() const {
    return settingsData.gameDifficulty;
  }

  bool getIsSoundOn() const {
    return settingsData.isSoundOn;
  }

  char *getPlayerName() const {
    return settingsData.playerName;
  }

  /* setters for the settings data */
  void setLcdContrast(const byte lcdContrast) {
    settingsData.lcdContrast = lcdContrast;
  }

  void setLcdBrightness(const byte lcdBrightness) {
    settingsData.lcdBrightness = lcdBrightness;
  }

  void setMatrixBrightness(const byte matrixBrightness) {
    settingsData.matrixBrightness = matrixBrightness;
  }

  void setGameDifficulty(const byte gameDifficulty) {
    settingsData.gameDifficulty = gameDifficulty;
  }

  void setIsSoundOn(const bool isSoundOn) {
    settingsData.isSoundOn = isSoundOn;
  }

  void setPlayerName(const char *playerName) {
    strcpy(settingsData.playerName, playerName);
  }

  /**
   * Function that loads the settings from the EEPROM
   * The settings data are saved in EEPROM at offset 0
   * No @params
   * No @return
   */
  void loadFromStorage() {
    EEPROM.get(SETTINGS_OFFSET_IN_STORAGE, settingsData);
  }

  /**
   * Function that saves the settings data in the EEPROM
   * The settings data are saved in EEPROM at offset 0
   * No @params
   * No @return
   */
  void saveInStorage() {
    EEPROM.put(SETTINGS_OFFSET_IN_STORAGE, settingsData);
  }

private:
  SettingsData settingsData;

  /**
   * Private constructor for the singleton class
   * The constructor will load the settings data from the storage
   */
  Settings() {
    loadFromStorage();
  }

  Settings(const Settings &other) = delete;

  Settings &operator=(const Settings &) = delete;
};

#endif