// Program that controls the hex number on a 4 digit 7-segment display, where each display will represent a hex digit. 
// The hardware uses a 74hc595 shift register (serial to parallel shifting-out register) to connect the arduino to the 
// display for using less arduino pins. The shift register adds a little deplay, but for this usecase we will consider 
// it negligible. For choosing and updating the value of a digit display we will use a joystick.
// The system has 2 states: selecting the digit display & updating the value of the selected digit display
// The initial state of the system is the one for choosing the digit display to modify. The init hex number on the 
// disoplay is 0000. The selected digit display is the rightmost one (considered as index 0). To mark which display is
// currently selected(hovered) the dp led on the digit display is set to blinking. Using the joystick (on x-axis) the 
// user can move to its neighbors.
// Pressing a short time on the joystick switch the user can lock the selected digit display and change the system state
// to the one for updating the hex value of the selected digit display. The dp is set to be always on to mark the new 
// state. Using the joystick (on y-axis this time) the user can change the hex value(from 0 to F) displayed on the 
// digit. At each joystick move the value is either incremented or decremented by 1, based on the direction of the 
// y-axis. Trying to increment on F, or decrement on 0, won't trigger any overflows, and remain at the hex limit value. 
// To save the desired value, the user can press the joystick switch again (no pressing time requirements). 
// The hex value of digit display will be saved and the system state will change back to selecting a digit display,
// The user can reset the number on the 4 digit 7-segment display by long pressing the joystick switch (only) in the 
// first state. The action will reset the value on the display back to 0000, and move the selected digit display to the 
// rightmost one (digit display with index 0).
// To display different values on each digit display on the 4 digit 7-segment display, we will use multiplexing. 
// To avoid changing the displayed value to fast and causing overlays on a display from other displays we will use a 
// small delay after each activation of a display. Using a delay to big, will cause flickering.

enum class SystemState {
  selectingDigitDisplay,
  changingHexNumberOnSelectedDigitDisplay
};
SystemState systemState = SystemState::selectingDigitDisplay;

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

// joystick pins
const int joystickSwitchPin = 2;
const int joystickXDirectionPin = A0;
const int joystickYDirectionPin = A1;

const int minMiddleTreshold = 400;
const int maxMiddleTreshold = 640;
const int minTreshold = 300;
const int maxTreshold = 740;

// shift register pins
const int latchPin = 11;  // ST]orage [C]lock [P]in latch on Shift Register
const int clockPin = 10;  // [SH]ift register [C]lock [P]in clock on Shift Register
const int dataPin = 12;   // [D]ata [S]torage on Shift Register

const int digitDisplay1Pin = 7;
const int digitDisplay2Pin = 6;
const int digitDisplay3Pin = 5;
const int digitDisplay4Pin = 4;

const int numberOfDigitDisplays = 4;
const int digitDisplaysPins[numberOfDigitDisplays] = {
  digitDisplay1Pin, digitDisplay2Pin, digitDisplay3Pin, digitDisplay4Pin
};
const byte minDisplayDigitIndex = 0;
const byte maxDisplayDigitIndex = 3;
int selectedDisplayDigitIndex = 0;
int digitDisplayValues[numberOfDigitDisplays] = { 0, 0, 0, 0 };
bool dpStateActive = false;

const byte minHexNumber = 0;
const byte maxHexNumber = 15;
const int encodingsNumber = 16;
const int byteEncodings[encodingsNumber] = {
  // A B C D E F G DP
  B11111100,  // 0
  B01100000,  // 1
  B11011010,  // 2
  B11110010,  // 3
  B01100110,  // 4
  B10110110,  // 5
  B10111110,  // 6
  B11100000,  // 7
  B11111110,  // 8
  B11110110,  // 9
  B11101110,  // A
  B00111110,  // b
  B10011100,  // C
  B01111010,  // d
  B10011110,  // E
  B10001110   // F
};

const byte debounceDelay = 50;

void setup() {
  setup4Digit7SegmentDisplay();
  setupShiftRegister();
  setupJoystick();

  resetSystemState();
  Serial.begin(9600);
}

void loop() {
  switch (systemState) {
    case SystemState::selectingDigitDisplay:
      selectingDigitDisplayLogic();
      break;
    case SystemState::changingHexNumberOnSelectedDigitDisplay:
      changingHexNumberOnSelectedDigitDisplayLogic();
      break;
  }

  writeDigitDisplayValues();
}

void setup4Digit7SegmentDisplay() {
  for (byte i = 0; i < numberOfDigitDisplays; i++) {
    pinMode(digitDisplaysPins[i], OUTPUT);
  }
}

void setupShiftRegister() {
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
}

void setupJoystick() {
  pinMode(joystickXDirectionPin, INPUT);
  pinMode(joystickYDirectionPin, INPUT);
  pinMode(joystickSwitchPin, INPUT_PULLUP);
}

void resetSystemState() {
  // initial state, all digits are 0, first digit display is selected
  for (byte i = 0; i < numberOfDigitDisplays; i++) {
    digitDisplayValues[i] = 0;
  }
  selectedDisplayDigitIndex = 0;
}

void selectingDigitDisplayLogic() {
  checkRequestOfChangingState();
  checkRequestOfChangingDigitDisplayWithCallback(updateSelectedDigitDisplay);
  setDpLedStateBlinking();
}

void changingHexNumberOnSelectedDigitDisplayLogic() {
  checkRequestOfChangingState();
  checkRequestOfChangingSelectedDigitValueWithCallback(updateSelectedDigitValue);
  setDpLedStateAlwaysActive();
}

void checkRequestOfChangingState() {
  static const byte minShortPressingInterval = 50;
  static const int minLongPressingInterval = 1000;
  static unsigned long lastSwitchReadingTime = 0;
  static unsigned long pressedTimestamp = 0;
  static long pressingTime = 0;
  static byte swState = HIGH;
  static byte switchReading = LOW;
  static byte lastSwitchReading = LOW;

  switchReading = digitalRead(joystickSwitchPin);
  if (switchReading != lastSwitchReading) {
    lastSwitchReadingTime = millis();
  }

  unsigned long currentTimestamp = millis();
  if (currentTimestamp - lastSwitchReadingTime >= debounceDelay) {
    if (switchReading != swState) {
      swState = switchReading;
      if (swState == LOW) {
        pressedTimestamp = currentTimestamp;
      }
      if (swState == HIGH) {
        pressingTime = currentTimestamp - pressedTimestamp;
      }
    }
  }

  lastSwitchReading = switchReading;

  if (pressingTime) {
    if (systemState == SystemState::selectingDigitDisplay) {
      if (pressingTime >= minLongPressingInterval) {
        resetSystemState();
      } else if (pressingTime >= minShortPressingInterval) {
        systemState = SystemState::changingHexNumberOnSelectedDigitDisplay;
      }
    } else {
      if (pressingTime >= minShortPressingInterval) {
        systemState = SystemState::selectingDigitDisplay;
      }
    }
    pressingTime = 0;
  }
}

void checkRequestOfChangingDigitDisplayWithCallback(void (*callbackFunction)(const XDirection xDirectionState)) {
  static unsigned long lastDebounceTime = 0;
  static int xValue = 0;
  static XDirection xDirectionReading = XDirection::MIDDLE;
  static XDirection lastXDirectionReading = XDirection::MIDDLE;
  static XDirection xDirectionState = XDirection::MIDDLE;

  xValue = analogRead(joystickXDirectionPin);

  if (xValue < minTreshold) {
    xDirectionReading = XDirection::RIGHT;
  } else if (xValue > maxTreshold) {
    xDirectionReading = XDirection::LEFT;
  } else if (xValue > minMiddleTreshold && xValue < maxMiddleTreshold) {
    xDirectionReading = XDirection::MIDDLE;
  }

  if (xDirectionReading != lastXDirectionReading) {
    lastDebounceTime = millis();
  }

  if (millis() - lastDebounceTime >= debounceDelay) {
    if (xDirectionReading != xDirectionState) {
      xDirectionState = xDirectionReading;
      if (xDirectionState != XDirection::MIDDLE) {
        callbackFunction(xDirectionState);
      }
    }
  }

  lastXDirectionReading = xDirectionReading;
}

void checkRequestOfChangingSelectedDigitValueWithCallback(void (*callbackFunction)(const YDirection yDirectionState)) {
  static unsigned long lastDebounceTime = 0;
  static int yValue = 0;
  static YDirection yDirectionReading = YDirection::MIDDLE;
  static YDirection lastYDirectionReading = YDirection::MIDDLE;
  static YDirection yDirectionState = YDirection::MIDDLE;


  yValue = analogRead(joystickYDirectionPin);

  if (yValue < minTreshold) {
    yDirectionReading = YDirection::DOWN;
  } else if (yValue > maxTreshold) {
    yDirectionReading = YDirection::UP;
  } else if (yValue > minMiddleTreshold && yValue < maxMiddleTreshold) {
    yDirectionReading = YDirection::MIDDLE;
  }

  if (yDirectionReading != lastYDirectionReading) {
    lastDebounceTime = millis();
  }

  if (millis() - lastDebounceTime >= debounceDelay) {
    if (yDirectionReading != yDirectionState) {
      yDirectionState = yDirectionReading;
      if (yDirectionState != YDirection::MIDDLE) {
        callbackFunction(yDirectionState);
      }
    }
  }

  lastYDirectionReading = yDirectionReading;
}

void setDpLedStateBlinking() {
  static const int blinkingInterval = 500;
  static unsigned long timeSnapshot = 0;

  unsigned long currentTimestamp = millis();
  if (currentTimestamp - timeSnapshot >= blinkingInterval) {
    dpStateActive = !dpStateActive;
    timeSnapshot = currentTimestamp;
  }
}

void setDpLedStateAlwaysActive() {
  dpStateActive = true;
}

void updateSelectedDigitDisplay(const XDirection xDirectionState) {
  if (xDirectionState == XDirection::LEFT) {
    selectedDisplayDigitIndex++;
  } else if (xDirectionState == XDirection::RIGHT) {
    selectedDisplayDigitIndex--;
  }

  // keep selected display digit index in valid range
  if (selectedDisplayDigitIndex < minDisplayDigitIndex) {
    selectedDisplayDigitIndex = minDisplayDigitIndex;
  }
  if (selectedDisplayDigitIndex > maxDisplayDigitIndex) {
    selectedDisplayDigitIndex = maxDisplayDigitIndex;
  }
}

void updateSelectedDigitValue(const YDirection yDirectionState) {
  if (yDirectionState == YDirection::UP) {
    digitDisplayValues[selectedDisplayDigitIndex]++;
  } else if (yDirectionState == YDirection::DOWN) {
    digitDisplayValues[selectedDisplayDigitIndex]--;
  }

  // keep the hex digit value in valid range
  if (digitDisplayValues[selectedDisplayDigitIndex] < minHexNumber) {
    digitDisplayValues[selectedDisplayDigitIndex] = minHexNumber;
  }
  if (digitDisplayValues[selectedDisplayDigitIndex] > maxHexNumber) {
    digitDisplayValues[selectedDisplayDigitIndex] = maxHexNumber;
  }
}

void writeDigitDisplayValues() {
  int displayIndex = 0;
  while (displayIndex < numberOfDigitDisplays) {
    int digitValue = byteEncodings[digitDisplayValues[displayIndex]];
    if (displayIndex == selectedDisplayDigitIndex && dpStateActive) {
      digitValue ^= 1;
    }

    turnOffAllDigitDisplays();
    writeReg(digitValue);
    activateDigitDisplayByIndex(displayIndex);

    displayIndex++;
    delay(5); // used for stabilization
  }
}

void turnOffAllDigitDisplays() {
  for (int i = 0; i < numberOfDigitDisplays; i++) {
    digitalWrite(digitDisplaysPins[i], HIGH);
  }
}

void activateDigitDisplayByIndex(int digitDisplayIndex) {
  digitalWrite(digitDisplaysPins[digitDisplayIndex], LOW);
}

void writeReg(int encoding) {
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, encoding);
  digitalWrite(latchPin, HIGH);
}