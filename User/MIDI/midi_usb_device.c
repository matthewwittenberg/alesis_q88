/*
 * midi_1_0_device.c
 *
 *  Created on: Sep 18, 2023
 *      Author: matt
 */

#include "midi_usb_driver.h"
#include "midi_spec.h"
#include "midi20_ci.h"
#include <stdint.h>
#include <stdbool.h>

void midi_usb_init()
{
	midi_usb_driver_init();
}

#if MIDI_VERSION == 1

void midi_usb_task()
{
	midi_usb_driver_task();
}

void midi_usb_note_on(uint8_t note, uint8_t channel, uint16_t velocity)
{
	uint32_t message = CIN_NOTE_ON | ((MIDI_NOTE_ON | channel) << 8) | (note << 16) | (velocity << 24);
	midi_usb_driver_tx((uint8_t*)&message, 4);
}

void midi_usb_note_off(uint8_t note, uint8_t channel, uint16_t velocity)
{
	uint32_t message = CIN_NOTE_OFF | ((MIDI_NOTE_OFF | channel) << 8) | (note << 16);
	midi_usb_driver_tx((uint8_t*)&message, 4);
}

void midi_usb_pitch_wheel(uint8_t channel, int32_t pitch)
{
	pitch = (pitch + 8192) & 0x3FFF;
	uint32_t pitch_new = (pitch & 0x7F) | ((pitch << 1) & 0x7F00);
	uint32_t message = CIN_PITCH_BEND | ((MIDI_PITCH_WHEEL | channel) << 8) | (pitch_new << 16);
	midi_usb_driver_tx((uint8_t*)&message, 4);
}

void midi_usb_modulation_wheel(uint8_t channel, uint16_t modulation)
{
	modulation = modulation & 0x3FFF;

	uint32_t fine = modulation & 0x7F;
	uint32_t message = CIN_CONTROL_CHANGE | ((MIDI_CONTROLLER | channel) << 8) | (MIDI_CONT_MOD_WHEEL_FINE << 16) | (fine << 24);
	midi_usb_driver_tx((uint8_t*)&message, 4);

	uint32_t course = (modulation >> 7) & 0x7F;
	message = CIN_CONTROL_CHANGE | ((MIDI_CONTROLLER | channel) << 8) | (MIDI_CONT_MOD_WHEEL_COARSE << 16) | (course << 24);
	midi_usb_driver_tx((uint8_t*)&message, 4);
}

void midi_usb_volume(uint8_t channel, uint16_t volume)
{
	volume = volume & 0x3FFF;

	uint32_t fine = volume & 0x7F;
	uint32_t message = CIN_CONTROL_CHANGE | ((MIDI_CONTROLLER | channel) << 8) | (MIDI_CONT_VOLUME_FINE << 16) | (fine << 24);
	midi_usb_driver_tx((uint8_t*)&message, 4);

	uint32_t course = (volume >> 7) & 0x7F;
	message = CIN_CONTROL_CHANGE | ((MIDI_CONTROLLER | channel) << 8) | (MIDI_CONT_VOLUME_COARSE << 16) | (course << 24);
	midi_usb_driver_tx((uint8_t*)&message, 4);
}

void midi_usb_sense()
{
	uint32_t message = CIN_SINGLE_BYTE | (MIDI_SENSE << 8);
	midi_usb_driver_tx((uint8_t*)&message, 4);
}

void midi_usb_sustain(uint8_t channel, bool on)
{
	uint32_t hold_value = on ? 0x7F : 0;
	uint32_t message = CIN_CONTROL_CHANGE | ((MIDI_CONTROLLER | channel) << 8) | (MIDI_CONT_HOLD_PEDAL << 16) | (hold_value << 24);
	midi_usb_driver_tx((uint8_t*)&message, 4);
}

void midi_usb_program_change(uint8_t channel, uint8_t program)
{
	uint32_t message = CIN_PROGRAM_CHANGE | ((MIDI_PROGRAM_CHANGE | channel) << 8) | (program << 16);
	midi_usb_driver_tx((uint8_t*)&message, 4);
}

#elif MIDI_VERSION == 2

#define SYSEX_BUFFER_LENGTH 128
#define MIDI20_MESSAGE_LENGTH 16

uint8_t _midi20_sysex_buffer[SYSEX_BUFFER_LENGTH];
uint32_t _midi20_sysex_buffer_index = 0;

void midi_ci_process_handler(uint8_t *pmessage, uint32_t length)
{
    uint32_t index = 0;
    uint8_t message[8];
    uint32_t total_messages = length / 6;
    uint32_t i, j;

    if(length % 6)
        total_messages++;

    // build usb packet chunks to send
    for(i=0; i<total_messages; i++)
    {
        message[0] = MIDI20_MESSAGE_TYPE_DATA64;

        if(total_messages == 1)
        {
            message[1] = MIDI20_SYSEX_STATUS_COMPLETE_IN_1;
        }
        else if(i == 0)
        {
            message[1] = MIDI20_SYSEX_STATUS_START;
        }
        else if(i < (total_messages - 1))
        {
            message[1] = MIDI20_SYSEX_STATUS_CONTINUE;
        }
        else
        {
            message[1] = MIDI20_SYSEX_STATUS_STOP;
        }

        for(j=0; j<6 && index<length; j++)
        {
            message[2+j] = pmessage[index++];
        }
        message[1] |= (j & 0x0F);

        midi_usb_driver_tx(message, sizeof(message));
    }
}

void midi_process_sysex()
{
    // pass to ci manager
    midi20_ci_process(_midi20_sysex_buffer, _midi20_sysex_buffer_index, midi_ci_process_handler);
}

void midi_usb_task()
{
	uint8_t byte;

	midi_usb_driver_task();

	if(midi_usb_driver_rx(&byte, 1))
	{
		uint8_t message_length = 0;
		uint8_t message_type = byte & 0xF0;
		switch(message_type)
		{
			case MIDI20_MESSAGE_TYPE_UTILITY:
			case MIDI20_MESSAGE_TYPE_SYSTEM:
			case MIDI20_MESSAGE_TYPE_10_CHANNEL_VOICE:
				message_length = 4;
				break;
			case MIDI20_MESSAGE_TYPE_DATA64:
			case MIDI20_MESSAGE_TYPE_20_CHANNEL_VOICE:
				message_length = 8;
				break;
			case MIDI20_MESSAGE_TYPE_DATA128:
				message_length = 16;
				break;
		}

		if(message_length == 0)
		{
			// reset driver rx
			midi_usb_flush_rx();
			return;
		}

		uint8_t message[MIDI20_MESSAGE_LENGTH];
		message[0] = byte;

		if(message_length - 1 != midi_usb_driver_rx(&message[1], message_length - 1))
		{
			// reset driver rx
			midi_usb_flush_rx();
			return;
		}

		// check for sysex, otherwise eat the message
		if(message_type == MIDI20_MESSAGE_TYPE_DATA64)
		{
			uint8_t status = message[1] & 0xF0;
			uint8_t byte_count = message[1] & 0x0F;

			if(byte_count > 6)
				byte_count = 6;

			for(uint8_t i=0; i<byte_count; i++)
			{
				_midi20_sysex_buffer[_midi20_sysex_buffer_index] = message[i+2];
				_midi20_sysex_buffer_index++;
				if(_midi20_sysex_buffer_index >= SYSEX_BUFFER_LENGTH)
					_midi20_sysex_buffer_index = 0;
			}

			if((status == MIDI20_SYSEX_STATUS_COMPLETE_IN_1) || (status == MIDI20_SYSEX_STATUS_STOP))
			{
				midi_process_sysex();
				_midi20_sysex_buffer_index = 0;
			}
		}
	}
}

void midi_usb_note_on(uint8_t note, uint8_t channel, uint16_t velocity)
{
    uint8_t message[8];
    message[0] = MIDI20_MESSAGE_TYPE_20_CHANNEL_VOICE;
    message[1] = MIDI_NOTE_ON | channel;
    message[2] = note;
    message[3] = 0; // no attribute
    message[4] = velocity;
    message[5] = velocity >> 8;
    message[6] = 0; // attribute lsb
    message[7] = 0; // attribute msb
    midi_usb_driver_tx(message, sizeof(message));
}

void midi_usb_note_off(uint8_t note, uint8_t channel, uint16_t velocity)
{
    uint8_t message[8];
    message[0] = MIDI20_MESSAGE_TYPE_20_CHANNEL_VOICE;
    message[1] = MIDI_NOTE_OFF | channel;
    message[2] = note;
    message[3] = 0; // no attribute
    message[4] = velocity;
    message[5] = velocity >> 8;
    message[6] = 0; // attribute lsb
    message[7] = 0; // attribute msb
    midi_usb_driver_tx(message, sizeof(message));
}

void midi_usb_pitch_wheel(uint8_t channel, int32_t pitch)
{
    pitch += 0x80000000;

    uint8_t message[8];
    message[0] = MIDI20_MESSAGE_TYPE_20_CHANNEL_VOICE;
    message[1] = MIDI_PITCH_WHEEL | channel;
    message[2] = 0;
    message[3] = 0;
    message[4] = pitch;
    message[5] = pitch >> 8;
    message[6] = pitch >> 16;
    message[7] = pitch >> 24;
    midi_usb_driver_tx(message, sizeof(message));
}

void midi_usb_modulation_wheel(uint8_t channel, uint16_t modulation)
{
    modulation = modulation & 0x3FFF;

    uint8_t message[8];
    message[0] = MIDI20_MESSAGE_TYPE_20_CHANNEL_VOICE;
    message[1] = MIDI_CONTROLLER | channel;
    message[2] = MIDI_CONT_MOD_WHEEL_COARSE;
    message[3] = 0;
    message[4] = modulation;
    message[5] = modulation >> 8;
    message[6] = 0;
    message[7] = 0;
    midi_usb_driver_tx(message, sizeof(message));
}

void midi_usb_volume(uint8_t channel, uint16_t volume)
{
    volume = volume & 0x3FFF;

    uint8_t message[8];
    message[0] = MIDI20_MESSAGE_TYPE_20_CHANNEL_VOICE;
    message[1] = MIDI_CONTROLLER | channel;
    message[2] = MIDI_CONT_VOLUME_COARSE;
    message[3] = 0;
    message[4] = volume;
    message[5] = volume >> 8;
    message[6] = 0;
    message[7] = 0;
    midi_usb_driver_tx(message, sizeof(message));
}

void midi_usb_sense()
{
    uint8_t message[4];
    message[0] = MIDI20_MESSAGE_TYPE_SYSTEM;
    message[1] = MIDI_SENSE;
    message[2] = 0;
    message[3] = 0;
    midi_usb_driver_tx(message, sizeof(message));
}

void midi_usb_sustain(uint8_t channel, bool on)
{
    uint32_t hold_value = on ? 0x7F : 0;

    uint8_t message[8];
    message[0] = MIDI20_MESSAGE_TYPE_20_CHANNEL_VOICE;
    message[1] = MIDI_CONTROLLER | channel;
    message[2] = MIDI_CONT_HOLD_PEDAL;
    message[3] = 0;
    message[4] = hold_value;
    message[5] = hold_value >> 8;
    message[6] = 0;
    message[7] = 0;
    midi_usb_driver_tx(message, sizeof(message));
}

void midi_usb_program_change(uint8_t channel, uint8_t program)
{
	// todo
}

#else
	#error MIDI_VERSION should be 1 or 2
#endif




