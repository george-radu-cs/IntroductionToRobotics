/**
 * Main file of the project that will load the instances of the menu and the snake's game and run them
 */

#include "game.h"
#include "menu.h"
#include "utils.h"

Game *game = nullptr;
Menu *menu = nullptr;
bool playingGame = false;
bool startGameIntro = true;

void setup() {
  Serial.begin(9600);

  // uncomment before first run to have default values saved in storage for the settings and highscores data
  // and not get garbage info
//  initDefaultDataInStorage();

  menu = Menu::getInstance();
  game = Game::getInstance();
}

void loop() {
  if (startGameIntro) {
    startGameIntro = menu->showStartMessage();
  } else if (!playingGame) {
    playingGame = menu->loadMenu();
  } else {
    playingGame = game->play();
    if (!playingGame) {
      menu->resetMenu();
    }
  }
}
