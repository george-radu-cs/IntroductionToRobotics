/**
 * File for the game class
 * The Game class is a singleton class to control the snake's game logic
 */

#ifndef GAME_H
#define GAME_H

#include "config.h"
#include "enums.h"
#include "point2d.h"
#include "settings.h"
#include "highscores.h"
#include "joystick.h"
#include "lcd.h"
#include "matrix.h"
#include "soundDevice.h"

class Game {
public:
  /**
   * Static method to get a pointer to the instance of the class
   * No @params
   * @return pointer to the instance of the class
   */
  static Game *getInstance() {
    static Game *instance = new Game();

    return instance;
  }

  /**
   * Play function expected to be called in a loop
   * No @params
   * @return true if the game is still running, false if the game has ended. Need to return the state of the game since
   * this function has logic to be called in a loop
   */
  bool play() {
    if (hasGameEnded) {
      if (inTransition) { // if the game has ended and the end transition is still running
        playGameEndedTransition();
      }

      // show any scrolling message if any
      lcd->printScrollingFlashStringMessage();
      lcd->printScrollingMessage();

      // user saw the game ending and his status, and requested to go back to the main menu
      if (joystick->isSwitchPressed()) {
        // disable all scrolls
        lcd->printScrollingMessage(true, nullptr);
        lcd->printScrollingFlashStringMessage(true, nullptr);

        // set game status for the next game
        hasGameEnded = false;
        inTransition = true;

        // save the highscore if the user has a new highscore
        int score = getGameScoreValue(snakeLength);
        highscores->updateHighscores(score, settings->getPlayerName());

        return false; // announce that the game is over
      }
    } else {
      if (inTransition) { // if the game has not ended and the start transition is still running
        playStartGameTransition();

        initGame();
        showGameStats();
        if (settings->getIsSoundOn()) {
          soundDevice->playSong(true);
        }
      }

      // play theme song while the game is running
      if (settings->getIsSoundOn()) {
        soundDevice->playSong();
      }

      checkIfGameHasEnded();
      if (food.x == ASKING_FOR_NEW_FOOD_VALUE || food.y == ASKING_FOR_NEW_FOOD_VALUE) { // snake ate food
        // update game status on lcd since the user progressed
        showGameStats();
        generateNewFood();
      }
      displayFood(); // display blinking food on matrix
      checkSnakeChangedDirection();
      updateSnakePosition();
    }

    return true; // announce that the game is still playing
  }

private:
  /**
   * game matrix containing the position of the snake
   * each segment of the snake's body starting from i = 0 (start of the head) has a value of snakeLength - i
   * to be easier to display and update the snake's body
   */
  byte gameMatrix[MATRIX_SIZE][MATRIX_SIZE] = {
      {0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0, 0, 0}
  };

  volatile Direction lastSnakeDirection = Direction::RIGHT;
  volatile Direction snakeDirection = Direction::RIGHT;
  int snakeSpeed;
  byte snakeLength = INITIAL_SNAKE_LENGTH;
  Point2D snakeHead;
  Point2D food{ASKING_FOR_NEW_FOOD_VALUE, ASKING_FOR_NEW_FOOD_VALUE};

  bool hasGameEnded = false;
  bool inTransition = true; // used to announce both start and end transitions, since they can't happen at the same time

  /**
   * Pointers to the interfaces of input & output devices to present the game
   */
  Joystick *joystick = nullptr;
  LCD *lcd = nullptr;
  Matrix *lcMatrix = nullptr;
  SoundDevice *soundDevice = nullptr;

  Settings *settings = nullptr;
  Highscores *highscores = nullptr;

  /**
   * Private constructor for the singleton class
   * The constructor will get the interfaces of the input & output devices to present the game
   * and the settings & highscores to have access to the game's settings and highscores
   */
  Game() {
    joystick = Joystick::getInstance();
    lcd = LCD::getInstance();
    lcMatrix = Matrix::getInstance();
    soundDevice = SoundDevice::getInstance();

    settings = Settings::getInstance();
    highscores = Highscores::getInstance();
  }

  Game(const Game &) = delete;

  Game &operator=(const Game &) = delete;

  /**
   * Function that prints on the lcd the game's status. Printed information:
   * - player name
   * - snake length (displayed as SL:value) - max 2 digits since the max length of the matrix is 8x8
   * - game difficulty (displayed as D:value) - max 1 digit
   * - current game score (displayed as cup char:value) - max 3 digits since the scores ranges from 0 to 999
   * No @params
   * No @return
   */
  void showGameStats() {
    lcd->clear();

    // print name of the player message
    lcd->printIndentedMessageOnRow(0, F("Name: "));
    lcd->printMessage(settings->getPlayerName());

    // print snake length message
    char snakeLengthMessage[11];
    sprintf(snakeLengthMessage, "SL:%.2d - D:%.1d ", snakeLength, settings->getGameDifficulty());
    lcd->setCursorPosition(0, 1);
    lcd->printMessage(snakeLengthMessage);

    // print current score message
    lcd->printCustomChar(byte(CUP_CHAR));
    int score = getGameScoreValue(snakeLength);
    char scoreMessage[4];
    sprintf(scoreMessage, "%03d", score);
    lcd->printMessage(scoreMessage);
  }

  /**
   * Function that computes the game score
   * The score gets calculated based on snake length and difficulty, both needs to be higher for a higher score,
   * lower difficulty levels are capped even at max snake length
   * @param snakeLength
   * @return the score computed
   */
  int getGameScoreValue(int snakeLength) {
    return map((snakeLength - INITIAL_SNAKE_LENGTH) * settings->getGameDifficulty(),
               INITIAL_SNAKE_LENGTH - INITIAL_SNAKE_LENGTH,
               (MAX_SNAKE_LENGTH - INITIAL_SNAKE_LENGTH) * MAX_DIFFICULTY_LEVEL, MIN_SCORE_VALUE,
               MAX_SCORE_VALUE);
  }

  /**
   * Function that initialize the game configuration based on saved settings
   * No @params
   * No @return
   */
  void initGame() {
    lcMatrix->clearDisplay();

    // reset gameMatrix to full zeros
    memset(gameMatrix, 0, sizeof(gameMatrix[0][0]) * MATRIX_SIZE * MATRIX_SIZE);

    // set the snake settings to initial values
    snakeSpeed = map(settings->getGameDifficulty(), MIN_DIFFICULTY_LEVEL, MAX_DIFFICULTY_LEVEL, MAX_SNAKE_SPEED,
                     MIN_SNAKE_SPEED);
    snakeLength = INITIAL_SNAKE_LENGTH;
    snakeDirection = Direction::RIGHT;
    snakeHead = {5, 2};
    for (byte i = 0; i <= 2; i++) { // snake body
      gameMatrix[5][i] = i + 1;
      lcMatrix->setLed(5, i, true);
    }

    configureRandomSeed(); // new random seed for generating food positions to have a different game loop each time
  }

  /**
   * Function that checks if the game has finished and marks the game as ended. Checks made:
   * - if the snake has hit a wall
   * - if the snake has reached the max length
   * No @params
   * No @return
   */
  void checkIfGameHasEnded() {
    // check if snake head hit a wall
    if (snakeHead.x < 0 || snakeHead.y < 0 || snakeHead.x >= MATRIX_SIZE || snakeHead.y >= MATRIX_SIZE) {
      hasGameEnded = true;
      inTransition = true;
    }
    if (snakeLength > MATRIX_SIZE * MATRIX_SIZE) { // ended because user reached max length
      hasGameEnded = true;
      inTransition = true;
    }
  }

  /**
   * Function that checks if the snake ate himself and marks the game as ended.
   * The snake movement is considered natural, the snake moves his head first then his body, so it's possible to eat
   * the end of his tail; if after the move, the snake's head is in the same spot as one of his body parts then we
   * consider the snake has eaten himself
   * No @params
   * No @return
   */
  void checkSnakeAteHimself() {
    if (gameMatrix[snakeHead.x][snakeHead.y] > 0) {
      hasGameEnded = true;
      inTransition = true;
    }
  }

  /**
   * Function that ask the game to generate a new food position
   * No @params
   * No @return
   */
  void askForNewFood() {
    food = {ASKING_FOR_NEW_FOOD_VALUE, ASKING_FOR_NEW_FOOD_VALUE}; // trigger value to generate new food
  }

  /**
   * Function that generates a new random food position.
   * The food won't be generated in a position where the snake is already present
   * No @params
   * No @return
   */
  void generateNewFood() {
    do {
      food.x = random(MATRIX_SIZE);
      food.y = random(MATRIX_SIZE);
    } while (gameMatrix[food.x][food.y] > 0); // don't spawn food on snake's body
  }

  /**
   * Function that display the food on the matrix with blinking effect
   * Expected to be called in a loop
   * No @params
   * No @return
   */
  void displayFood() {
    static bool foodLightState = true;
    static unsigned long lastBlinkTime = 0;

    unsigned long currentTimestamp = millis();
    if (currentTimestamp - lastBlinkTime >= FOOD_BLINK_TIME) {
      foodLightState = !foodLightState;
      lastBlinkTime = currentTimestamp;
    }

    lcMatrix->setLed(food.x, food.y, foodLightState);
  }

  /**
   * Function that checks if the snake has eaten the food and updates the snake status accordingly
   * No @params
   * No @return
   */
  void checkSnakeAteFood() {
    if (snakeHead == food) { // snake eats food when his head is on the same spot as the food
      if (settings->getIsSoundOn()) { // play a sound when eating food
        soundDevice->playSound(NOTE_F5, TONE_DURATION);
      }

      snakeLength++;
      for (byte i = 0; i < MATRIX_SIZE; i++) {
        for (byte j = 0; j < MATRIX_SIZE; j++) {
          if (gameMatrix[i][j] > 0) {
            gameMatrix[i][j]++;
          }
        }
      }
      askForNewFood();
    }
  }

  /**
   * Function that checks the request on changing the snake direction and updates the snake status accordingly and allow
   * only valid direction changes (no 180 degrees turns)
   * No @params
   * No @return
   */
  void checkSnakeChangedDirection() {
    // get user input directions
    XDirection xDirection = joystick->getStateOnXAxis();
    YDirection yDirection = joystick->getStateOnYAxis();

    // correct direction to not allow the case of 180 deg. turns
    if (xDirection == XDirection::RIGHT && yDirection == YDirection::MIDDLE && lastSnakeDirection != Direction::LEFT) {
      snakeDirection = Direction::RIGHT;
    } else if (xDirection == XDirection::LEFT && yDirection == YDirection::MIDDLE &&
               lastSnakeDirection != Direction::RIGHT) {
      snakeDirection = Direction::LEFT;
    } else if (xDirection == XDirection::MIDDLE && yDirection == YDirection::DOWN &&
               lastSnakeDirection != Direction::UP) {
      snakeDirection = Direction::DOWN;
    } else if (xDirection == XDirection::MIDDLE && yDirection == YDirection::UP &&
               lastSnakeDirection != Direction::DOWN) {
      snakeDirection = Direction::UP;
    }
    lastSnakeDirection = snakeDirection;
  }

  /**
   * Function that updates the snake on the new position he has to move to. The snake movement is considered natural,
   * the snake moves his head first then his body. Also checks if the snake has eaten himself or the food
   * Expected to be called in a loop
   * No @params
   * No @return
   */
  void updateSnakePosition() {
    static unsigned long lastPositionUpdateTimestamp = 0;
    static unsigned long currentTimestamp = 0;

    currentTimestamp = millis();
    if (currentTimestamp - lastPositionUpdateTimestamp >= snakeSpeed) {
      updateSnakeHeadPosition();
      checkSnakeAteFood();
      updateSnakeWholeBody();

      lastPositionUpdateTimestamp = currentTimestamp;
    }
  }

  /**
   * Function that updates the snake head position based on the snake move direction
   * and keeps it in the matrix boundaries
   * No @params
   * No @return
   */
  void updateSnakeHeadPosition() {
    switch (snakeDirection) {
      case Direction::UP:
        snakeHead.x--;
        break;
      case Direction::LEFT:
        snakeHead.y--;
        break;
      case Direction::DOWN:
        snakeHead.x++;
        break;
      case Direction::RIGHT:
        snakeHead.y++;
        break;
      default:
        break;
    }

    checkSnakeAteHimself();

    // don't display the head in case it goes off the screen
    if (!(snakeHead.x < 0 || snakeHead.y < 0 || snakeHead.x >= MATRIX_SIZE || snakeHead.y >= MATRIX_SIZE)) {
      gameMatrix[snakeHead.x][snakeHead.y] = snakeLength + 1;
    }
  }

  /**
   * Function that moves the body of the snake after a step moved and update it's position in the game matrix
   * No @params
   * No @return
   */
  void updateSnakeWholeBody() {
    for (byte i = 0; i < MATRIX_SIZE; i++) {
      for (byte j = 0; j < MATRIX_SIZE; j++) {
        if (gameMatrix[i][j] > 0) {
          gameMatrix[i][j]--;
        }
        lcMatrix->setLed(i, j, gameMatrix[i][j] > 0);
      }
    }
  }

  /**
   * Function that plays the starting game transition.
   * Displays play button on the matrix then a countdown from 3 to 1 before starting
   * Displays prepare message on the lcd and loading bar
   * Uses delay for transitions
   * No @params
   * No @return
   */
  void playStartGameTransition() {
    lcd->clear();

    lcd->setCursorPosition(4, 0);
    lcd->printMessage("Prepare");
    lcd->setCursorPosition(0, 1);
    lcd->animateOneSecondLCDLoadingWithDelay();

    lcMatrix->displayThree();
    lcd->animateOneSecondLCDLoadingWithDelay();

    lcMatrix->displayTwo();
    lcd->animateOneSecondLCDLoadingWithDelay();

    lcMatrix->displayOne();
    lcd->animateOneSecondLCDLoadingWithDelay();

    inTransition = false;
  }

  /**
   * Function that plays the game over transition.
   * - Plays a sound announcing the game over
   * - Waits ~1s with delay to let the player see the state of the game when is over
   * - Displays on the lcd the user score, the difficulty level played and a message depending if the users has beaten a
   * high score or not, which position he has in the high scores table and that he need to press the switch button to
   * return to the main menu
   * - Displays on the lcd matrix a happy or sad face depending if the user has beaten a high score or not
   * No @params
   * No @return
   */
  void playGameEndedTransition() {
    soundDevice->removeSound();
    delay(100); // delay to not interfere with the theme's song
    if (settings->getIsSoundOn()) {
      soundDevice->playSound(NOTE_C5, LOSING_TONE_DURATION);
    }
    delay(1000); // delay to allow the user to look at the state of the game

    lcd->clear();
    int score = getGameScoreValue(snakeLength);
    char scoreMessage[11];
    sprintf(scoreMessage, "Score:%03d - D:%.1d", score, settings->getGameDifficulty());
    lcd->setCursorPosition(0, 0);
    lcd->printMessage(scoreMessage);

    byte place = highscores->getNewHighscorePlace(score);
    if (place < NUMBER_OF_HIGHSCORES_SAVED) {
      lcMatrix->displayHappyFace();
      char *message = new char[MAX_GAME_END_MESSAGE_LENGTH];
      sprintf(message, "Congrats! You are on place %.1d on highscores board :) - Press SW to save & continue", place + 1);
      lcd->printScrollingMessage(true, message, 0, 1, LCD_DISPLAY_WIDTH);
      delete[] message;
    } else {
      lcMatrix->displaySadFace();
      lcd->printScrollingFlashStringMessage(true, F("You didn't beat any highscores :(  Press SW to continue"), 0, 1,
                                            LCD_DISPLAY_WIDTH);
    }

    inTransition = false;
  }
};

#endif