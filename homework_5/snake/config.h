/**
 * Config file for the project
 * Contains configurations for the project and modules used
 */

#ifndef CONFIG_H
#define CONFIG_H

// 16x2 lcd pins configuration
#define LCD_INTENSITY_PIN 9
#define LCD_CONTRAST_PIN 10
#define LCD_RS 12
#define LCD_ENABLE 8
#define LCD_D4 7
#define LCD_D5 6
#define LCD_D6 5
#define LCD_D7 4
// lcd constants
#define LCD_DISPLAY_WIDTH 16
#define LCD_DISPLAY_HEIGHT 2
#define LCD_INDENT_CHARS 1
#define LCD_DEFAULT_SCROLL_CUT_LENGTH 14

// joystick pins configuration
#define JOYSTICK_SW_PIN 2
#define JOYSTICK_X_PIN A0
#define JOYSTICK_Y_PIN A1
// joystick constants
#define JOYSTICK_MIN_MIDDLE_THRESHOLD 400
#define JOYSTICK_MAX_MIDDLE_THRESHOLD 640
#define JOYSTICK_MIN_THRESHOLD 300
#define JOYSTICK_MAX_THRESHOLD 740

// 8x8 matrix pins configuration
#define MATRIX_SIZE 8
#define MATRIX_DIN_PIN A3
#define MATRIX_CLOCK_PIN A4
#define MATRIX_LOAD_PIN A5
#define MATRIX_NUM_DRIVER 1

// sound device pins configuration
#define SOUND_DEVICE_PIN 3
// sound constants
#define NOTE_A4 440
#define NOTE_A7 3520
#define NOTE_C5 523
#define NOTE_F5 698
#define TONE_DURATION 200
#define LOSING_TONE_DURATION 500

// snake game configuration
#define INITIAL_SNAKE_NUMBER_OF_LIVES 3
#define STARVING_TIME_INTERVAL 20000
#define INITIAL_SNAKE_LENGTH 3
#define MAX_SNAKE_LENGTH 64
#define MIN_SNAKE_SPEED 400
#define MAX_SNAKE_SPEED 1600
#define ASKING_FOR_NEW_FOOD_VALUE 255
#define MIN_SCORE_VALUE 0
#define MAX_SCORE_VALUE 999
#define MAX_GAME_END_MESSAGE_LENGTH 82

// time constants
#define FOOD_BLINK_TIME 500
#define INTRO_MESSAGE_TIME_IN_MILLIS 11000
#define PRINT_MESSAGE_SCROLL_DELAY 750
#define QUARTER_SECOND_IN_MILLIS 250
#define NOTE_DURATION_SCALAR .9

// settings menu's sections instead of enum
#define CHANGE_NAME 1
#define CHANGE_LCD_CONTRAST 2
#define CHANGE_LCD_BRIGHTNESS 3
#define CHANGE_MATRIX_BRIGHTNESS 4
#define CHANGE_DIFFICULTY 5
#define RESET_HIGHSCORES 6
#define CHANGE_SOUND_ON_OFF 7

// settings slider constants
#define MAX_LCD_CONTRAST_BLOCK_COUNT 12
#define MAX_LCD_BRIGHTNESS_BLOCK_COUNT 10
#define MAX_MATRIX_BRIGHTNESS_BLOCK_COUNT 7
#define MAX_DIFFICULTY_BLOCK_COUNT 4
#define MIN_SLIDER_BLOCK_COUNT 1
#define MIN_CONTRAST_VALUE 0
#define MAX_CONTRAST_VALUE 90
#define MIN_LCD_BRIGHTNESS_VALUE 25
#define MAX_LCD_BRIGHTNESS_VALUE 255
#define MIN_MATRIX_BRIGHTNESS_VALUE 1
#define MAX_MATRIX_BRIGHTNESS_VALUE 15
#define MIN_DIFFICULTY_LEVEL 1
#define MAX_DIFFICULTY_LEVEL 4
#define NUMBER_OF_HIGHSCORES_SAVED 5
#define MAX_PLAYER_NAME_LENGTH 6

// storage eeprom offsets
#define SETTINGS_OFFSET_IN_STORAGE 0
// highscores offset is the size of the settingsData struct

#define MAX_DIGITAL_OUTPUT_VALUE 255

#endif