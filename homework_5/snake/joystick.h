/**
 * File for the joystick class
 * The Joystick class is a singleton class that allows access to the joystick movement, change of states, and switch
 */

#ifndef JOYSTICK_H
#define JOYSTICK_H

#include "config.h"
#include "enums.h"

class Joystick {
public:
  /**
   * Static method to get a pointer to the instance of the class
   * No @params
   * @return pointer to the instance of the class
   */
  static Joystick *getInstance() {
    static Joystick *instance = new Joystick();

    return instance;
  }

  /**
   * Function that obtains the current direction reading on the x-axis of the joystick
   * No @params
   * @return the state of the joystick on the x-axis as a XDirection enum
   */
  XDirection getStateOnXAxis() {
    static XDirection lastReadState = XDirection::MIDDLE;

    int readValue = analogRead(JOYSTICK_X_PIN);
    if (readValue >= JOYSTICK_MAX_THRESHOLD) {
      lastReadState = XDirection::RIGHT;
      return XDirection::RIGHT;
    } else if (readValue <= JOYSTICK_MIN_THRESHOLD) {
      lastReadState = XDirection::LEFT;
      return XDirection::LEFT;
    } else if (readValue >= JOYSTICK_MIN_MIDDLE_THRESHOLD && readValue <= JOYSTICK_MAX_MIDDLE_THRESHOLD) {
      lastReadState = XDirection::MIDDLE;
      return XDirection::MIDDLE;
    }

    return lastReadState;
  }

  /**
   * Function that obtains the current direction reading on the y-axis of the joystick
   * No @params
   * @return the state of the joystick on the y-axis as a YDirection enum
   */
  YDirection getStateOnYAxis() {
    static YDirection lastReadState = YDirection::MIDDLE;

    int readValue = analogRead(JOYSTICK_Y_PIN);
    if (readValue >= JOYSTICK_MAX_THRESHOLD) {
      lastReadState = YDirection::DOWN;
      return YDirection::DOWN;
    } else if (readValue <= JOYSTICK_MIN_THRESHOLD) {
      lastReadState = YDirection::UP;
      return YDirection::UP;
    } else if (readValue >= JOYSTICK_MIN_MIDDLE_THRESHOLD && readValue <= JOYSTICK_MAX_MIDDLE_THRESHOLD) {
      lastReadState = YDirection::MIDDLE;
      return YDirection::MIDDLE;
    }

    return lastReadState;
  }

  /**
   * Function that detects a single movement (no holding) from the init state of MIDDLE on the x-axis of the joystick
   * and returns the direction of the movement. To reset a movement, the joystick must return in the MIDDLE state
   * No @params
   * @return the direction of the movement on the x-axis as a Direction enum, if no movement is detected, returns MIDDLE
   */
  XDirection detectMovementOnXAxis() {
    XDirection state = getStateOnXAxis();

    if (state == XDirection::MIDDLE) {
      joyMovedOnXAxis = false;
      return XDirection::MIDDLE;
    }

    if (!joyMovedOnXAxis) {
      joyMovedOnXAxis = true;
      return state;
    }

    return XDirection::MIDDLE;
  }

  /**
   * Function that detects a single movement (no holding) from the init state of MIDDLE on the y-axis of the joystick
   * and returns the direction of the movement. To reset a movement, the joystick must return in the MIDDLE state
   * No @params
   * @return the direction of the movement on the y-axis as a Direction enum, if no movement is detected, returns MIDDLE
   */
  YDirection detectMovementOnYAxis() {
    YDirection state = getStateOnYAxis();

    if (state == YDirection::MIDDLE) {
      joyMovedOnYAxis = false;
      return YDirection::MIDDLE;
    }

    if (!joyMovedOnYAxis) {
      joyMovedOnYAxis = true;
      return state;
    }

    return YDirection::MIDDLE;
  }

  /**
   * Function that detects a single press (no holding) on the joystick switch.
   * To reset a press, the joystick must be released. If holding the joystick switch, the function will return false
   * No @params
   * @return true if the joystick switch is pressed, false otherwise and on hold
   */
  bool isSwitchPressed() {
    bool lastSwitchState = switchState;
    updateSwitchPressed();
    if (lastSwitchState == LOW) {
      return false;
    }
    return switchState == LOW;
  }

private:
  bool joyMovedOnXAxis, joyMovedOnYAxis;
  byte switchState;

  /**
   * Private constructor for the singleton class
   * The constructor initializes the joystick pins and set the initial state of the joystick
   */
  Joystick() {
    pinMode(JOYSTICK_X_PIN, INPUT);
    pinMode(JOYSTICK_Y_PIN, INPUT);
    pinMode(JOYSTICK_SW_PIN, INPUT_PULLUP);

    joyMovedOnXAxis = false;
    joyMovedOnYAxis = false;
    switchState = HIGH;
  }

  Joystick(const Joystick &other) = delete;

  Joystick &operator=(const Joystick &) = delete;

  /**
   * Function that updates the state of the joystick switch with debounce
   * No @params
   * No @return
   */
  void updateSwitchPressed() {
    static const byte debounceDelay = 50;
    static unsigned long lastSwitchReadingTime = 0;
    static byte switchReading = HIGH;
    static byte lastSwitchReading = HIGH;

    switchReading = digitalRead(JOYSTICK_SW_PIN);
    if (switchReading != lastSwitchReading) {
      lastSwitchReadingTime = millis();
    }
    lastSwitchReading = switchReading;

    if (millis() - lastSwitchReadingTime >= debounceDelay) {
      if (switchReading != switchState) {
        switchState = switchReading;
      }
    }
  }
};

#endif