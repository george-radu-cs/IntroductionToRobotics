/**
 * File containing utils functions
 */
#ifndef UTILS_H
#define UTILS_H

/**
 * Function that returns the size of a string saved in the Flash memory
 * Function created since strlen_P didn't give me the right values for some reasons
 * implementation idea taken from the function: (line 44) size_t Print::print(const __FlashStringHelper *ifsh) from arduino Print
 * link: https://github.com/arduino/ArduinoCore-avr/blob/master/cores/arduino/Print.cpp
 * @param fs - pointer to the string saved in Flash memory
 * @return the size of the string
 */
size_t getLengthOfFlashString(const __FlashStringHelper *fs) {
  PGM_P p = reinterpret_cast<PGM_P>(fs);
  size_t n = 0;
  while (true) {
    unsigned char c = pgm_read_byte(p++);
    if (c == 0) { break; }
    n++;
  }
  return n;
}

/**
 * Function that seeds the random system with a custom seed created with some entropy from the readings on the
 * arduino's analog pins
 * No @params
 * No @return
 */
void configureRandomSeed() {
  // get some entropy
  int r = 0;
  for (int i = A0; i <= A7; i++) {
    r += analogRead(i);
  }

  randomSeed(r);
  randomSeed(random() ^ ((uint32_t) analogRead(0) << 22) ^ micros());
}

#endif