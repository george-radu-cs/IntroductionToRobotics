// program that controls the states (led active or inactive) for each segment in a 7-segment diplay
// the program has 2 states, one for selecting the segment to update and one to choose the segment state 
// the initial state is the one for selecting the segment for updating, starting from the dp led; 
// to let the user know which is the current segment, the current segment is set to blink
// using a joystick the user can move to the other segments, the program allow the user one move at a time, after each 
// one the joystick needs to come back in its init state (both axis on middle)
// the program supports 8 directions (up, down, left, right by moving the joystick in a single direction 
// and up-left, up-right, down-left, down-right by moving the joystick in both directions - on diagonal)
// the movements are bounded by the neighbors for each segment, 
// trying to move from a segment to a non-existent neighbor will result in remaning at the same segment
// while in the state of selecting a segment, by short pressing the joystick switch the user changes the system state to
// choosingSegmentState in which he can set the state of the current segment; the initial value of the segment will be 
// the previous state saved, by moving the joystick on the x coordinate, to each end, the user can toggle between the
// state active or inactive of the segment; for saving the desired state the user can press the joystick switch 
// (no additional requirements for pressing), after the press the system will save the current segment state and change 
// back to the inital state of selecting a segment, and the current segment will remain the same
// if a user wants to reset the configuration made to the display, he can make a long press on the joystick switch while
// in the first state of selecting segment which will trigger the reset of the system, turn all segments off and move 
// the current segment back to the dp led

enum class SystemState {
  selectingSegment,
  choosingSegmentState,
};
SystemState systemState = SystemState::selectingSegment;

enum XYDirection {
  UP,
  UP_LEFT,
  LEFT,
  DOWN_LEFT,
  DOWN,
  DOWN_RIGHT,
  RIGHT,
  UP_RIGHT,
  STAY,
};

enum class XDirection {
  RIGHT,
  LEFT,
  MIDDLE,
};

enum class YDirection {
  UP,
  DOWN,
  MIDDLE,
};

// 7-segment led pins
const int pinA = 4;
const int pinB = 5;
const int pinC = 6;
const int pinD = 7;
const int pinE = 8;
const int pinF = 9;
const int pinG = 10;
const int pinDP = 11;

const byte numberOfSegments = 8;
const bool sevenSegmentDisplayHasCommonAnnode = false;
byte segmentActiveValue = HIGH;
byte segmentPins[numberOfSegments] = { pinA, pinB, pinC, pinD, pinE, pinF, pinG, pinDP };
byte segmentsState[numberOfSegments];

const byte numberOfDirections = 9;
byte movements[numberOfSegments][numberOfDirections] = {
  // UP UP_LEFT LEFT DOWN_LEFT DOWN DOWN_RIGHT RIGHT UP_RIGHT STAY
  { 0, 0, 0, 5, 6, 1, 0, 0, 0 },  // a 0
  { 1, 0, 5, 6, 2, 1, 1, 1, 1 },  // b 1
  { 1, 6, 4, 3, 2, 2, 7, 2, 2 },  // c 2
  { 6, 4, 3, 3, 3, 3, 3, 2, 3 },  // d 3
  { 5, 4, 4, 4, 4, 3, 2, 6, 4 },  // e 4
  { 5, 5, 5, 5, 4, 6, 1, 0, 5 },  // f 5
  { 0, 5, 6, 4, 3, 2, 6, 1, 6 },  // g 6
  { 7, 7, 2, 7, 7, 7, 7, 7, 7 },  // dp 7
};

byte currentSegmentIndex = 7;  // dp pin
byte currentSegmentState = HIGH;
const int blinkingInterval = 350;
unsigned long timeSnapshot = 0;

// joystick pins
const int pinSW = 2;
const int pinX = A0;
const int pinY = A1;

byte switchReading = LOW;
byte lastSwitchReading = LOW;
int xValue = 0;
int yValue = 0;
XDirection xDir = XDirection::LEFT;
YDirection yDir = YDirection::UP;

const int minMiddleTreshold = 400;
const int maxMiddleTreshold = 640;
const int minTreshold = 300;
const int maxTreshold = 740;
bool joyMoved = false;
bool segmentMoved = false;

unsigned long lastDebounceTime = 0;
const byte debounceDelay = 50;

void setup() {
  setup7SegmentDisplay();
  setupJoystick();

  resetDisplayState();
  Serial.begin(9600);
}

void loop() {
  switch (systemState) {
    case SystemState::selectingSegment:
      selectingSegmentLogic();
      break;
    case SystemState::choosingSegmentState:
      choosingSegmentStateLogic();
      break;
  }
}

void setup7SegmentDisplay() {
  for (int i = 0; i < numberOfSegments; i++) {
    pinMode(segmentPins[i], OUTPUT);
  }
  if (sevenSegmentDisplayHasCommonAnnode) {
    segmentActiveValue = !segmentActiveValue;
    currentSegmentState = segmentActiveValue;
  }
}

void setupJoystick() {
  pinMode(pinX, INPUT);
  pinMode(pinY, INPUT);
  pinMode(pinSW, INPUT_PULLUP);
}

void resetDisplayState() {
  // initial state all segments off, dp led blinking
  for (int i = 0; i < numberOfSegments; i++) {
    segmentsState[i] = !segmentActiveValue;
    digitalWrite(segmentPins[i], segmentsState[i]);
  }
  currentSegmentIndex = 7;  // dp segment index
}

void selectingSegmentLogic() {
  readJoystickSwitchPressed();
  readJoystickMovement();
  changeCurrentSegment();
  setCurrentSegmentBlinking();
}

void readJoystickSwitchPressed() {
  static const byte minShortPressingInterval = 100;
  static const int minLongPressingInterval = 1000;
  static unsigned long lastSwitchReadingTime = 0;
  static unsigned long pressedTimestamp = 0;
  static long pressingTime = 0;
  static byte swState = HIGH;

  switchReading = digitalRead(pinSW);
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

  if (pressingTime >= minLongPressingInterval) {
    resetDisplayState();
    pressingTime = 0;
  } else if (pressingTime >= minShortPressingInterval) {
    systemState = SystemState::choosingSegmentState;
    pressingTime = 0;
  }
}

void readJoystickMovement() {
  static unsigned long lastDebounceTime = 0;
  static bool xJoyMoved = false;
  static bool yJoyMoved = false;
  static bool lastXJoyMoved = false;
  static bool lastYJoyMoved = false;

  xValue = analogRead(pinX);
  yValue = analogRead(pinY);

  if (!joyMoved) {
    if (xValue < minTreshold) {
      xDir = XDirection::RIGHT;
      xJoyMoved = true;
    }
    if (xValue > maxTreshold) {
      xDir = XDirection::LEFT;
      xJoyMoved = true;
    }
    if (yValue < minTreshold) {
      yDir = YDirection::DOWN;
      yJoyMoved = true;
    }
    if (yValue > maxTreshold) {
      yDir = YDirection::UP;
      yJoyMoved = true;
    }
  }

  if (xValue > minMiddleTreshold && xValue < maxMiddleTreshold
      && yValue > minMiddleTreshold && yValue < maxMiddleTreshold) {
    joyMoved = false;
    xJoyMoved = false;
    yJoyMoved = false;
    segmentMoved = false;
    xDir = XDirection::MIDDLE;
    yDir = YDirection::MIDDLE;
  }

  if (lastXJoyMoved != xJoyMoved || lastYJoyMoved != yJoyMoved) {
    lastDebounceTime = millis();
  }

  if (millis() - lastDebounceTime > debounceDelay) {
    joyMoved = xJoyMoved || yJoyMoved;
  }

  lastXJoyMoved = xJoyMoved;
  lastYJoyMoved = yJoyMoved;
}

void changeCurrentSegment() {
  if (joyMoved && !segmentMoved) {
    // preserve saved state for previous current segment before change of segment
    digitalWrite(segmentPins[currentSegmentIndex], segmentsState[currentSegmentIndex]);

    const XYDirection direction = getDirection();
    currentSegmentIndex = movements[currentSegmentIndex][direction];
    currentSegmentState = segmentActiveValue;
    digitalWrite(segmentPins[currentSegmentIndex], currentSegmentState);

    timeSnapshot = millis();
    segmentMoved = true;
  }
}

void setCurrentSegmentBlinking() {
  unsigned long currentTimestamp = millis();
  if (currentTimestamp - timeSnapshot >= blinkingInterval) {
    currentSegmentState = !currentSegmentState;
    digitalWrite(segmentPins[currentSegmentIndex], currentSegmentState);
    timeSnapshot = currentTimestamp;
  }
}

XYDirection getDirection() {
  if (xDir == XDirection::LEFT) {
    if (yDir == YDirection::UP) {
      return XYDirection::UP_LEFT;
    }
    if (yDir == YDirection::DOWN) {
      return XYDirection::DOWN_LEFT;
    }
    if (yDir == YDirection::MIDDLE) {
      return XYDirection::LEFT;
    }
  }
  if (xDir == XDirection::RIGHT) {
    if (yDir == YDirection::UP) {
      return XYDirection::UP_RIGHT;
    }
    if (yDir == YDirection::DOWN) {
      return XYDirection::DOWN_RIGHT;
    }
    if (yDir == YDirection::MIDDLE) {
      return XYDirection::RIGHT;
    }
  }
  if (xDir == XDirection::MIDDLE) {
    if (yDir == YDirection::UP) {
      return XYDirection::UP;
    }
    if (yDir == YDirection::DOWN) {
      return XYDirection::DOWN;
    }
    if (yDir == YDirection::MIDDLE) {
      return XYDirection::STAY;
    }
  }
}

void choosingSegmentStateLogic() {
  setCurrentSegmentPreviousSavedState();
  checkConfirmingSegmentState();
  checkChangeSegmentState();
}

void setCurrentSegmentPreviousSavedState() {
  digitalWrite(segmentPins[currentSegmentIndex], segmentsState[currentSegmentIndex]);
}

void checkConfirmingSegmentState() {
  static unsigned long lastSwitchReadingTime = 0;
  static byte swState = HIGH;

  switchReading = digitalRead(pinSW);
  if (switchReading != lastSwitchReading) {
    lastSwitchReadingTime = millis();
  }

  unsigned long currentTimestamp = millis();
  if (currentTimestamp - lastSwitchReadingTime >= debounceDelay) {
    if (switchReading != swState) {
      swState = switchReading;
      if (swState == HIGH) {
        systemState = SystemState::selectingSegment;
      }
    }
  }

  lastSwitchReading = switchReading;
}

void checkChangeSegmentState() {
  static unsigned long lastDebounceTime = 0;
  static bool xJoyMoved = false;
  static bool lastXJoyMoved = false;

  xValue = analogRead(pinX);

  if (!joyMoved) {
    if (xValue < minTreshold) {
      xDir = XDirection::RIGHT;
      xJoyMoved = true;
    }
    if (xValue > maxTreshold) {
      xDir = XDirection::LEFT;
      xJoyMoved = true;
    }
  }

  if (xValue > minMiddleTreshold && xValue < maxMiddleTreshold) {
    xJoyMoved = false;
    segmentMoved = false;
    xDir = XDirection::MIDDLE;
  }

  if (lastXJoyMoved != xJoyMoved) {
    lastDebounceTime = millis();
  }

  if (millis() - lastDebounceTime > debounceDelay) {
    if (xJoyMoved && xDir != XDirection::MIDDLE) {
      if (xDir == XDirection::RIGHT) {
        segmentsState[currentSegmentIndex] = segmentActiveValue;
      }
      if (xDir == XDirection::LEFT) {
        segmentsState[currentSegmentIndex] = !segmentActiveValue;
      }
      xJoyMoved = false;
      digitalWrite(segmentPins[currentSegmentIndex], segmentsState[currentSegmentIndex]);
    }
  }

  lastXJoyMoved = xJoyMoved;
}