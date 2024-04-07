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

void midi_serial_aftertouch(uint8_t note, uint8_t channel, uint16_t data)
{
	uint8_t message[3];
	message[0] = MIDI_AFTERTOUCH | channel;
	message[1] = note;
	message[2] = data;
	midi_serial_driver_tx(message, sizeof(message));
}

void midi_serial_channel_pressure(uint8_t channel, uint16_t data)
{
	uint8_t message[2];
	message[0] = MIDI_CHANNEL_PRESSURE | channel;
	message[1] = data;
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

void midi_serial_controller(uint8_t index, uint8_t channel, uint32_t value)
{
	uint8_t message[3];

	message[0] = MIDI_CONTROLLER | channel;
	message[1] = index;
	message[2] = value;
	midi_serial_driver_tx(message, sizeof(message));
}

void midi_serial_modulation_wheel(uint8_t channel, uint16_t modulation)
{
	modulation = modulation & 0x3FFF;

	midi_serial_controller(MIDI_CONT_MOD_WHEEL_FINE, channel, modulation & 0x7F);
	midi_serial_controller(MIDI_CONT_MOD_WHEEL_COARSE, channel, (modulation >> 7) & 0x7F);
}

void midi_serial_volume(uint8_t channel, uint16_t volume)
{
	volume = volume & 0x3FFF;

	midi_serial_controller(MIDI_CONT_VOLUME_FINE, channel, volume & 0x7F);
	midi_serial_controller(MIDI_CONT_VOLUME_COARSE, channel, (volume >> 7) & 0x7F);
}

void midi_serial_sense()
{
	uint8_t message[1];
	message[0] = MIDI_SENSE;
	midi_serial_driver_tx(message, sizeof(message));
}

void midi_serial_sustain(uint8_t channel, bool on)
{
	midi_serial_controller(MIDI_CONT_HOLD_PEDAL, channel, on ? 0x7F : 0);
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

void midi_serial_register_callback(message_callback callback)
{
}
