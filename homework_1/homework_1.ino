// program that controls the brightness of all light components in an rgb led
// by reading the analog brightness level for each rgb color and mapping them
// to analog write values, used to output to the led via the pwm pins

const int MIN_ANALOG_READ_VALUE = 0;
const int MAX_ANALOG_READ_VALUE = 1023;
const int MIN_ANALOG_WRITE_VALUE = 0;
const int MAX_ANALOG_WRITE_VALUE = 255;

const int redInputPin = A2;
const int greenInputPin = A1;
const int blueInputPin = A0;

const int redOutputPin = 11;
const int greenOutputPin = 10;
const int blueOutputPin = 9;

// used to save the rgb analog read values and the rgb analog write values
int redValue = 0;
int greenValue = 0;
int blueValue = 0;

void setup() {
  setRGBInputPins();
  setRGBOutputPins();

  Serial.begin(9600);
}

void loop() {
  readRGBAnalogValues();
  convertRGBAnalogReadValuesToAnalogWriteValues();
  setLedRGBColors(redValue, greenValue, blueValue);
}

void setRGBInputPins() {
  pinMode(redInputPin, INPUT);
  pinMode(greenInputPin, INPUT);
  pinMode(blueInputPin, INPUT);
}

void setRGBOutputPins() {
  pinMode(redOutputPin, OUTPUT);
  pinMode(greenOutputPin, OUTPUT);
  pinMode(blueOutputPin, OUTPUT);
}

void readRGBAnalogValues() {
  redValue = analogRead(redInputPin);
  greenValue = analogRead(greenInputPin);
  blueValue = analogRead(blueInputPin);
}

int mapAnalogReadValueToAnalogWriteValue(const int value) {
  return map(value, MIN_ANALOG_READ_VALUE, MAX_ANALOG_READ_VALUE, MIN_ANALOG_WRITE_VALUE, MAX_ANALOG_WRITE_VALUE);
}

void convertRGBAnalogReadValuesToAnalogWriteValues() {
  redValue = mapAnalogReadValueToAnalogWriteValue(redValue);
  greenValue = mapAnalogReadValueToAnalogWriteValue(greenValue);
  blueValue = mapAnalogReadValueToAnalogWriteValue(blueValue);
}

void setLedRGBColors(const int redValue, const int greenValue, const int blueValue) {
  analogWrite(redOutputPin, redValue);
  analogWrite(greenOutputPin, greenValue);
  analogWrite(blueOutputPin, blueValue);
}