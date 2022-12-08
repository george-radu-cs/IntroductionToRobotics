/**
 * Main file of the project that will load the instances of the menu and the snake's game and run them
 */

#include "game.h"
#include "menu.h"

Game *game = nullptr;
Menu *menu = nullptr;
bool playingGame = false;
bool startGameIntro = true;

void setup() {
  Serial.begin(9600);

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
