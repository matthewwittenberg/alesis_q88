/*
 * midi_1_0_device.c
 *
 *  Created on: Sep 18, 2023
 *      Author: matt
 */

#include "midi_1_0_driver.h"
#include "midi_spec.h"
#include <stdint.h>
#include <stdbool.h>

void midi_1_0_init()
{
	midi_1_0_driver_init();
}

void midi_1_0_note_on(uint8_t note, uint8_t channel, uint16_t velocity)
{
	uint32_t message = CIN_NOTE_ON | ((MIDI_NOTE_ON | channel) << 8) | (note << 16) | (velocity << 24);
	midi_1_0_driver_tx(message);
}

void midi_1_0_note_off(uint8_t note, uint8_t channel, uint16_t velocity)
{
	uint32_t message = CIN_NOTE_OFF | ((MIDI_NOTE_OFF | channel) << 8) | (note << 16);
	midi_1_0_driver_tx(message);
}

void midi_1_0_pitch_wheel(uint8_t channel, int32_t pitch)
{
	pitch = (pitch + 8192) & 0x3FFF;
	uint32_t pitch_new = (pitch & 0x7F) | ((pitch << 1) & 0x7F00);
	uint32_t message = CIN_PITCH_BEND | ((MIDI_PITCH_WHEEL | channel) << 8) | (pitch_new << 16);
	midi_1_0_driver_tx(message);
}

void midi_1_0_modulation_wheel(uint8_t channel, uint16_t modulation)
{
	modulation = modulation & 0x3FFF;

	uint32_t fine = modulation & 0x7F;
	uint32_t message = CIN_CONTROL_CHANGE | ((MIDI_CONTROLLER | channel) << 8) | (MIDI_CONT_MOD_WHEEL_FINE << 16) | (fine << 24);
	midi_1_0_driver_tx(message);

	uint32_t course = (modulation >> 7) & 0x7F;
	message = CIN_CONTROL_CHANGE | ((MIDI_CONTROLLER | channel) << 8) | (MIDI_CONT_MOD_WHEEL_COARSE << 16) | (course << 24);
	midi_1_0_driver_tx(message);
}

void midi_1_0_volume(uint8_t channel, uint16_t volume)
{
	volume = volume & 0x3FFF;

	uint32_t fine = volume & 0x7F;
	uint32_t message = CIN_CONTROL_CHANGE | ((MIDI_CONTROLLER | channel) << 8) | (MIDI_CONT_VOLUME_FINE << 16) | (fine << 24);
	midi_1_0_driver_tx(message);

	uint32_t course = (volume >> 7) & 0x7F;
	message = CIN_CONTROL_CHANGE | ((MIDI_CONTROLLER | channel) << 8) | (MIDI_CONT_VOLUME_COARSE << 16) | (course << 24);
	midi_1_0_driver_tx(message);
}

void midi_1_0_sense()
{
	uint32_t message = CIN_SINGLE_BYTE | (MIDI_SENSE << 8);
	midi_1_0_driver_tx(message);
}

void midi_1_0_sustain(uint8_t channel, bool on)
{
	uint32_t hold_value = on ? 0x7F : 0;
	uint32_t message = CIN_CONTROL_CHANGE | ((MIDI_CONTROLLER | channel) << 8) | (MIDI_CONT_HOLD_PEDAL << 16) | (hold_value << 24);
	midi_1_0_driver_tx(message);
}

void midi_1_0_task()
{
	midi_1_0_driver_task();
}


