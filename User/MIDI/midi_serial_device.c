/*
 * midi_1_0_serial_device.c
 *
 *  Created on: Sep 22, 2023
 *      Author: matt
 */


#include "midi_serial_driver.h"
#include "midi_spec.h"
#include <stdint.h>
#include <stdbool.h>

void midi_serial_init()
{
	midi_serial_driver_init();
}

void midi_serial_note_on(uint8_t note, uint8_t channel, uint16_t velocity)
{
	uint8_t message[3];
	message[0] = MIDI_NOTE_ON | channel;
	message[1] = note;
	message[2] = velocity;
	midi_serial_driver_tx(message, sizeof(message));
}

void midi_serial_note_off(uint8_t note, uint8_t channel, uint16_t velocity)
{
	uint8_t message[3];
	message[0] = MIDI_NOTE_OFF | channel;
	message[1] = note;
	message[2] = velocity;
	midi_serial_driver_tx(message, sizeof(message));
}

void midi_serial_pitch_wheel(uint8_t channel, int32_t pitch)
{
	pitch = (pitch + 8192) & 0x3FFF;

	uint8_t message[3];
	message[0] = MIDI_PITCH_WHEEL | channel;
	message[1] = pitch & 0x7F;
	message[2] = (pitch >> 7) & 0x7F;
	midi_serial_driver_tx(message, sizeof(message));
}

void midi_serial_modulation_wheel(uint8_t channel, uint16_t modulation)
{
	uint8_t message[3];

	modulation = modulation & 0x3FFF;

	message[0] = MIDI_CONTROLLER | channel;
	message[1] = MIDI_CONT_MOD_WHEEL_FINE;
	message[2] = modulation & 0x7F;
	midi_serial_driver_tx(message, sizeof(message));

	message[0] = MIDI_CONTROLLER | channel;
	message[1] = MIDI_CONT_MOD_WHEEL_COARSE;
	message[2] = (modulation >> 7) & 0x7F;
	midi_serial_driver_tx(message, sizeof(message));
}

void midi_serial_volume(uint8_t channel, uint16_t volume)
{
	uint8_t message[3];

	volume = volume & 0x3FFF;

	message[0] = MIDI_CONTROLLER | channel;
	message[1] = MIDI_CONT_VOLUME_FINE;
	message[2] = volume & 0x7F;
	midi_serial_driver_tx(message, sizeof(message));

	message[0] = MIDI_CONTROLLER | channel;
	message[1] = MIDI_CONT_VOLUME_COARSE;
	message[2] = (volume >> 7) & 0x7F;
	midi_serial_driver_tx(message, sizeof(message));
}

void midi_serial_sense()
{
	uint8_t message[1];
	message[0] = MIDI_SENSE;
	midi_serial_driver_tx(message, sizeof(message));
}

void midi_serial_sustain(uint8_t channel, bool on)
{
	uint8_t message[3];
	message[0] = MIDI_CONTROLLER | channel;
	message[1] = MIDI_CONT_HOLD_PEDAL;
	message[2] = on ? 0x7F : 0;
	midi_serial_driver_tx(message, sizeof(message));
}

void midi_serial_program_change(uint8_t channel, uint8_t program)
{
	uint8_t message[2];
	message[0] = MIDI_PROGRAM_CHANGE | channel;
	message[1] = program & 0x7F;
	midi_serial_driver_tx(message, sizeof(message));
}

void midi_serial_task()
{
}
