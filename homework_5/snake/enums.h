/**
 * File for generic enums like:
 * - direction on a x-axis
 * - direction on a y-axis
 * - 4 way direction in a 2d space
 * - main menu item sections
 */

#ifndef ENUMS_H
#define ENUMS_H

enum class XDirection {
  RIGHT,
  MIDDLE,
  LEFT,
};

enum class YDirection {
  UP,
  MIDDLE,
  DOWN,
};

enum class Direction {
  UP,
  LEFT,
  DOWN,
  RIGHT
};

enum MenuItem {
  MAIN = 0,
  PLAY = 1,
  HIGHSCORES = 2,
  SETTINGS = 3,
  ABOUT = 4,
  HOW_TO_PLAY = 5,
};

#endif