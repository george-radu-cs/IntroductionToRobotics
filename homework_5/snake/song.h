/**
 * File that contains the definition for the theme song of the old Nokia phones
 * credits to https://github.com/robsoncouto/arduino-songs/blob/master/nokia/nokia.ino
 * for melody transcript
 */

#ifndef SONG_H
#define SONG_H

#include "pitches.h"

// save notes of the melody followed by the duration
// 4 = quarter note, 8 = eighteenth, 16 sixteenth & so on
// negative numbers represent a dotted note
// ex: -4 is a dotted quarter note, meaning a quarter + an eighteenth
int melody[] = {
    NOTE_E5, 8, NOTE_D5, 8, NOTE_FS4, 4, NOTE_GS4, 4,
    NOTE_CS5, 8, NOTE_B4, 8, NOTE_D4, 4, NOTE_E4, 4,
    NOTE_B4, 8, NOTE_A4, 8, NOTE_CS4, 4, NOTE_E4, 4,
    NOTE_A4, 2, REST, 4, REST, 4
};

#define SONG_TEMPO 180
#define WHOLE_NOTE_DURATION (int)((60000 * 4) / SONG_TEMPO)

const int NUMBER_OF_NOTES = sizeof(melody) / sizeof(melody[0]) / 2;

#endif