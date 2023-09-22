/*
 * midi_1_0_serial_device.c
 *
 *  Created on: Sep 22, 2023
 *      Author: matt
 */


#include "midi_1_0_serial_driver.h"
#include "midi_spec.h"
#include <stdint.h>
#include <stdbool.h>

void midi_1_0_serial_init()
{
}

void midi_1_0_serial_note_on(uint8_t note, uint8_t channel, uint16_t velocity)
{
}

void midi_1_0_serial_note_off(uint8_t note, uint8_t channel, uint16_t velocity)
{
}

void midi_1_0_serial_pitch_wheel(uint8_t channel, int32_t pitch)
{
}

void midi_1_0_serial_modulation_wheel(uint8_t channel, uint16_t modulation)
{
}

void midi_1_0_serial_volume(uint8_t channel, uint16_t volume)
{
}

void midi_1_0_serial_sense()
{
}

void midi_1_0_serial_sustain(uint8_t channel, bool on)
{
}

void midi_1_0_serial_program_change(uint8_t channel, uint8_t program)
{
}

void midi_1_0_serial_task()
{
}
