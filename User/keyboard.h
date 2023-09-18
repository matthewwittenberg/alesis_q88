/*
 * keyboard.h
 *
 *  Created on: Sep 18, 2023
 *      Author: matt
 */

#ifndef KEYBOARD_H_
#define KEYBOARD_H_

#include <stdint.h>
#include <stdbool.h>

#define KEYBOARD_TOTAL_KEYS 88
#define KEYBOARD_NOTES_PER_OCTAVE 12
#define KEYBOARD_START_NOTE 21
#define KEYBOARD_CHANNEL 0

void keyboard_init();
void keyboard_task();
uint8_t keyboard_get_start_note();
void keyboard_set_start_note(uint8_t note);

#endif /* KEYBOARD_H_ */
