// program that controls the traffic lights in an intersection. The intersection has 2 semaphores, one for cars (red, 
// yellow and green) and one for people (red and green) and obviously the 2 semaphores can't be green at the same time
// in the initial state the car semaphore is always green without a timer to go red and the people semaphore is red,
// the people have access to a button for asking their semaphore to light green, allowing them to cross
// to prevent accidents, after pressing the button there is a timeout before changing traffic lights
// pressing the button more than once won't trigger the light change again, and pressing it outisde the previous state
// described won't trigger any action
// after the timeout the car semaphore turns yellow for a short time to anounce the cars to stop for the changing in
// light and finally the people semaphore turns green and they can traverse the intersection
// there is also a buzzer to play a sound for blind people to know when it's safe to cross
// after some time the people semaphore starts blinking green and the beeping sound from the buzzer becomes faster to
// anounce the people that the semaphore will go red again and to hurry in crossing
// after that the intersection comes back to the initial state, green for cars and red for people, until other
// person presses the button again and the cycle repeats

enum class IntersectionState {
  greenLightForCarSemaphore,
  yellowLightForCarSemaphore,
  greenLightForPeopleSemaphore,
  blinkingGreenLightForPeopleSemaphore
};
volatile IntersectionState insersectionState = IntersectionState::greenLightForCarSemaphore;

const byte carSemaphoreRedLedPin = 13;
const byte carSemaphoreYellowLedPin = 12;
const byte carSemaphoreGreenLedPin = 11;

const byte peopleSemaphoreRedLedPin = 10;
const byte peopleSemaphoreGreenLedPin = 9;

const byte buttonAskingForPeopleGreenLightPin = 2;
const byte peopleGreenBuzzerPin = 3;

volatile byte buttonState = HIGH;
volatile byte lastButtonReading = HIGH;
bool buttonPressed = false;
const byte debounceDelay = 50;
unsigned long lastDebounceTime = 0;

unsigned long buzzerTimeSnapshot = 0;
const int buzzerTone = 440; // NOTE_A4 (LA) music note

unsigned long timeSnapshot = 0;

void setup() {
  setupCarSemaphore();
  setupPeopleSemaphore();

  switchToGreenLightForCarSemaphore();
  Serial.begin(9600);
}

void loop() {
  switch (insersectionState) {
    case IntersectionState::greenLightForCarSemaphore:
      greenLightForCarSemaphoreLogic();
      break;
    case IntersectionState::yellowLightForCarSemaphore:
      yellowLightForCarSemaphoreLogic();
      break;
    case IntersectionState::greenLightForPeopleSemaphore:
      greenLightForPeopleSemaphoreLogic();
      break;
    case IntersectionState::blinkingGreenLightForPeopleSemaphore:
      blinkingGreenLightForPeopleSemaphoreLogic();
      break;
  }
}

void setupCarSemaphore() {
  pinMode(carSemaphoreRedLedPin, OUTPUT);
  pinMode(carSemaphoreYellowLedPin, OUTPUT);
  pinMode(carSemaphoreGreenLedPin, OUTPUT);
}

void setupPeopleSemaphore() {
  pinMode(peopleSemaphoreRedLedPin, OUTPUT);
  pinMode(peopleSemaphoreGreenLedPin, OUTPUT);
  pinMode(peopleGreenBuzzerPin, OUTPUT);
  pinMode(buttonAskingForPeopleGreenLightPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(buttonAskingForPeopleGreenLightPin), peopleAskingForGreenLight, FALLING);
}

void switchToGreenLightForCarSemaphore() {
  unsigned long currentTimestamp = millis();

  insersectionState = IntersectionState::greenLightForCarSemaphore;
  timeSnapshot = currentTimestamp;
  buzzerTimeSnapshot = currentTimestamp;
  lastButtonReading = HIGH;
  buttonState = HIGH;

  digitalWrite(peopleSemaphoreRedLedPin, HIGH);
  digitalWrite(peopleSemaphoreGreenLedPin, LOW);
  digitalWrite(carSemaphoreRedLedPin, LOW);
  digitalWrite(carSemaphoreYellowLedPin, LOW);
  digitalWrite(carSemaphoreGreenLedPin, HIGH);
}

void peopleAskingForGreenLight() {
  if (insersectionState != IntersectionState::greenLightForCarSemaphore) {
    return;
  }

  if (buttonPressed) {
    return;
  }

  unsigned long currentTimestamp = millis();
  if (currentTimestamp - lastDebounceTime < debounceDelay) {
    return;
  }

  lastDebounceTime = currentTimestamp;
  buttonPressed = true;
  timeSnapshot = currentTimestamp;
}

void greenLightForCarSemaphoreLogic() {
  if (buttonPressed) {
    waitingForButtonAskingForPeopleGreenLightTimeout();
  }
}

void waitingForButtonAskingForPeopleGreenLightTimeout() {
  // set timeout after button press before changing traffic lights to prevent accidents
  static const int buttonTimeout = 8 * 1000; // milliseconds

  if (millis() - timeSnapshot >= buttonTimeout) {
    buttonPressed = false;
    switchToYellowLightForCarSemaphore();
  }
}

void switchToYellowLightForCarSemaphore() {
  insersectionState = IntersectionState::yellowLightForCarSemaphore;
  timeSnapshot = millis();

  digitalWrite(carSemaphoreYellowLedPin, HIGH);
  digitalWrite(carSemaphoreGreenLedPin, LOW);
}

void yellowLightForCarSemaphoreLogic() {
  static const int yellowLightDuration = 3 * 1000; // milliseconds

  if (millis() - timeSnapshot >= yellowLightDuration) {
    switchToGreenLightForPeople();
  }
}

void switchToGreenLightForPeople() {
  insersectionState = IntersectionState::greenLightForPeopleSemaphore;
  timeSnapshot = millis();

  digitalWrite(carSemaphoreRedLedPin, HIGH);
  digitalWrite(carSemaphoreYellowLedPin, LOW);
  digitalWrite(peopleSemaphoreRedLedPin, LOW);
  digitalWrite(peopleSemaphoreGreenLedPin, HIGH);
}

void greenLightForPeopleSemaphoreLogic() {
  static const int greenLightDuration = 8 * 1000; // milliseconds
  static const int buzzInterval = 1000; // milliseconds
  static bool buzzerState = 0;

  unsigned long currentTimestamp = millis();
  if (currentTimestamp - timeSnapshot >= greenLightDuration) {
    buzzerState = 0;
    switchToBlinkingGreenForPeople();
    return;
  }

  if (currentTimestamp - buzzerTimeSnapshot >= buzzInterval) {
    buzzerState = !buzzerState;
    if (buzzerState) {
      tone(peopleGreenBuzzerPin, buzzerTone);
    } else {
      noTone(peopleGreenBuzzerPin);
    }
    buzzerTimeSnapshot = currentTimestamp;
  }
}

void switchToBlinkingGreenForPeople() {
  insersectionState = IntersectionState::blinkingGreenLightForPeopleSemaphore;
  timeSnapshot = millis();
}

void blinkingGreenLightForPeopleSemaphoreLogic() {
  static const int blinkingGreenLightDuration = 4 * 1000; // milliseconds
  static const int buzzInterval = 400; // milliseconds
  static bool buzzerState = 0;

  unsigned long currentTimestamp = millis();
  if (currentTimestamp - timeSnapshot >= blinkingGreenLightDuration) {
    buzzerState = 0;
    noTone(peopleGreenBuzzerPin);
    switchToGreenLightForCarSemaphore();
    return;
  }

  if (currentTimestamp - buzzerTimeSnapshot >= buzzInterval) {
    buzzerState = !buzzerState;
    if (buzzerState) {
      tone(peopleGreenBuzzerPin, buzzerTone);
      digitalWrite(peopleSemaphoreGreenLedPin, HIGH);
    } else {
      noTone(peopleGreenBuzzerPin);
      digitalWrite(peopleSemaphoreGreenLedPin, LOW);
    }
    buzzerTimeSnapshot = currentTimestamp;
  }
}