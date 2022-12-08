/**
 * File for the Sound device class
 * The SoundDevice class is a singleton class that allows control to a sound output device,
 * controls the play of its audio and have the capability to play songs
 */

#ifndef SOUND_DEVICE_H
#define SOUND_DEVICE_H

#include "config.h"
#include "song.h"

class SoundDevice {
public:
  /**
   * Static method to get a pointer to the instance of the class
   * No @params
   * @return pointer to the instance of the class
   */
  static getInstance() {
    static SoundDevice *instance = new SoundDevice();

    return instance;
  }

  /**
   * Function that plays a note for a given duration on the sound device
   * @param note - the note frequency
   * @param duration - the note duration in millis to be played
   * No @return
   */
  void playSound(int note, int duration) {
    tone(SOUND_DEVICE_PIN, note, duration);
  }

  /**
   * Function that removes any sound output on the sound device
   * No @params
   * No @return
   */
  void removeSound() {
    noTone(SOUND_DEVICE_PIN);
  }

  /**
   * Utility function to play the theme song
   * Needs to be called in a loop, since at each call will play a note if timestamp on melody reached
   * @param reset - boolean to request to start the song over again
   * No @return
   */
  void playSong(bool reset = false) {
    static unsigned long lastUpdateTime = 0;
    static int currentNote = 0;
    static int noteDuration = 0;
    static bool wasCurrentNotePlayed = false;

    if (reset) {
      currentNote = 0;
      wasCurrentNotePlayed = false;
      lastUpdateTime = millis();
    }

    if (!wasCurrentNotePlayed) {
      int customDuration = melody[currentNote + 1];
      if (customDuration > 0) { // if the encoded duration is positive, the note is a regular one
        noteDuration = WHOLE_NOTE_DURATION / customDuration; // regular notes keep the same duration
      } else { // if the encoded duration is negative, the note is a dotted one
        noteDuration = WHOLE_NOTE_DURATION / abs(customDuration);
        noteDuration *= 1.5; // increase duration with one half for dotted notes
      }

      // output to the sound device the frequency of the note with time scalled of the note duration
      tone(SOUND_DEVICE_PIN, melody[currentNote], noteDuration * NOTE_DURATION_SCALAR);
      wasCurrentNotePlayed = true;
      lastUpdateTime = millis();
    }

    if (millis() - lastUpdateTime >= noteDuration) { // check if the note
      noTone(SOUND_DEVICE_PIN); // remove any sound
      currentNote += 2; // move to the next note freq
      wasCurrentNotePlayed = false;
    }

    if (currentNote == NUMBER_OF_NOTES * 2) { // reached end of the song, loop
      currentNote = 0;
    }
  }

private:
  /**
   * Private constructor for the singleton class
   * The constructor initializes the sound device pins (in this case a single pin for the buzzer)
   */
  SoundDevice() {
    pinMode(SOUND_DEVICE_PIN, OUTPUT);
  }

  SoundDevice(const SoundDevice &) = delete;

  SoundDevice &operator=(const SoundDevice &) = delete;
};

#endif