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
#include <string.h>
#include <stdbool.h>

#define MIDI20_BUFFER_LENGTH 128
#define MIDI20_MESSAGE_LENGTH 16

uint8_t _midi20_buffer[MIDI20_BUFFER_LENGTH];
uint32_t _midi20_buffer_index = 0;
message_callback _message_callback = NULL;
sysex_callback _sysex_callback = NULL;

void midi_usb_init()
{
	midi_usb_driver_init();
}

void midi_ci_process_handler(uint8_t *pmessage, uint32_t length)
{
    uint8_t cable_group = 0;
    uint32_t index = 1;
    uint8_t message[8];
    uint32_t total_messages = (length - 2) / 6; // leave out the sysex start & end bytes
    uint32_t i, j;

    if((length - 2) % 6) {
        total_messages++;
    }

    // build usb packet chunks to send
    for(i=0; i<total_messages; i++)
    {
        memset(message, 0, sizeof(message));

        message[3] = MIDI20_MESSAGE_TYPE_DATA64 | cable_group;

        if(total_messages == 1)
        {
            message[2] = MIDI20_SYSEX_STATUS_COMPLETE_IN_1;
        }
        else if(i == 0)
        {
            message[2] = MIDI20_SYSEX_STATUS_START;
        }
        else if(i < (total_messages - 1))
        {
            message[2] = MIDI20_SYSEX_STATUS_CONTINUE;
        }
        else
        {
            message[2] = MIDI20_SYSEX_STATUS_STOP;
        }

        for(j=0; j<6; j++)
        {
            if(index >= (length-1)) {
                break;
            }

            switch(j)
            {
                case 0:
                    message[1] = pmessage[index++];
                    break;
                case 1:
                    message[0] = pmessage[index++];
                    break;
                case 2:
                    message[7] = pmessage[index++];
                    break;
                case 3:
                    message[6] = pmessage[index++];
                    break;
                case 4:
                    message[5] = pmessage[index++];
                    break;
                case 5:
                    message[4] = pmessage[index++];
                    break;
            }
        }
        message[2] |= (j & 0x0F);

        midi_usb_driver_tx(message, sizeof(message));
    }
}

void midi_stream_process_handler(uint8_t *pmessage, uint32_t length)
{
	midi_usb_driver_tx(pmessage, length);
}

void midi_process_sysex()
{
    midi20_ci_process(_midi20_buffer, _midi20_buffer_index, midi_ci_process_handler);
}

void midi_process_stream(uint16_t status)
{
	midi20_stream_process(status, _midi20_buffer, _midi20_buffer_index, midi_stream_process_handler);
}

void midi_usb_task()
{
	uint8_t byte;
	static uint8_t message[MIDI20_MESSAGE_LENGTH];
	static uint8_t message_index = 0;

	midi_usb_driver_task();

	if(IS_MIDI_2_0)
	{
		while(midi_usb_driver_rx(&byte, 1))
		{
			message[message_index++] = byte;
			if(message_index == 4)
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
					case MIDI20_MESSAGE_TYPE_FLEX:
					case MIDI20_MESSAGE_TYPE_STREAM:
						message_length = 16;
						break;
				}

				// if we could not identify the packet type, reset
				if(message_length == 0)
				{
					midi_usb_flush_rx();
					message_index = 0;
					break;
				}

				message_length -= 4;

				if(message_length)
				{
					// read the rest of the packet
					if(message_length != midi_usb_driver_rx(&message[4], message_length))
					{
						// reset driver rx
						midi_usb_flush_rx();
						message_index = 0;
						break;
					}
				}

				// sysex?
				if(message_type == MIDI20_MESSAGE_TYPE_DATA64)
				{
					uint8_t status = message[2] & 0xF0;
					uint8_t byte_count = message[2] & 0x0F;

					// add the sysex start byte
					if((status == MIDI20_SYSEX_STATUS_COMPLETE_IN_1) || (status == MIDI20_SYSEX_STATUS_START)) {
						_midi20_buffer_index = 0;
						_midi20_buffer[_midi20_buffer_index++] = MIDI_SYSEX_START;
					}

					if(byte_count > 6) {
						byte_count = 6;
					}

					if(byte_count > 0) {
						_midi20_buffer[_midi20_buffer_index++] = message[1];
					}
					if(byte_count > 1) {
						_midi20_buffer[_midi20_buffer_index++] = message[0];
					}
					if(byte_count > 2) {
						_midi20_buffer[_midi20_buffer_index++] = message[7];
					}
					if(byte_count > 3) {
						_midi20_buffer[_midi20_buffer_index++] = message[6];
					}
					if(byte_count > 4) {
						_midi20_buffer[_midi20_buffer_index++] = message[5];
					}
					if(byte_count > 5) {
						_midi20_buffer[_midi20_buffer_index++] = message[4];
					}

					// add the sysex end byte
					if((status == MIDI20_SYSEX_STATUS_COMPLETE_IN_1) || (status == MIDI20_SYSEX_STATUS_STOP)) {
						_midi20_buffer[_midi20_buffer_index++] = MIDI_SYSEX_END;
					}

					// bounds check
					if(_midi20_buffer_index >= MIDI20_BUFFER_LENGTH) {
						_midi20_buffer_index = 0;
					}

					if((status == MIDI20_SYSEX_STATUS_COMPLETE_IN_1) || (status == MIDI20_SYSEX_STATUS_STOP))
					{
						midi_process_sysex();
						_midi20_buffer_index = 0;
					}
				}
				// stream?
				else if(message_type == MIDI20_MESSAGE_TYPE_STREAM)
				{
					uint16_t status = message[2] | ((message[3] & 0x0C) << 8);
					uint8_t format = message[3] & 0x0C;

					_midi20_buffer[_midi20_buffer_index++] = message[0];
					_midi20_buffer[_midi20_buffer_index++] = message[1];
					_midi20_buffer[_midi20_buffer_index++] = message[4];
					_midi20_buffer[_midi20_buffer_index++] = message[5];
					_midi20_buffer[_midi20_buffer_index++] = message[6];
					_midi20_buffer[_midi20_buffer_index++] = message[7];
					_midi20_buffer[_midi20_buffer_index++] = message[8];
					_midi20_buffer[_midi20_buffer_index++] = message[9];
					_midi20_buffer[_midi20_buffer_index++] = message[10];
					_midi20_buffer[_midi20_buffer_index++] = message[11];
					_midi20_buffer[_midi20_buffer_index++] = message[12];
					_midi20_buffer[_midi20_buffer_index++] = message[13];
					_midi20_buffer[_midi20_buffer_index++] = message[14];
					_midi20_buffer[_midi20_buffer_index++] = message[15];

					if((format == MIDI20_STREAM_STATUS_COMPLETE_IN_1) || (format == MIDI20_STREAM_STATUS_STOP))
					{
						midi_process_stream(status);
						_midi20_buffer_index = 0;
					}
				}
				// 10 voice?
				else if(message_type == MIDI20_MESSAGE_TYPE_10_CHANNEL_VOICE)
				{
					if(_message_callback) {
						_message_callback(message[3] & 0x0F, message[2], message[1], message[0]);
					}
				}
				// system?
				else if(message_type == MIDI20_MESSAGE_TYPE_SYSTEM)
				{
					if(_message_callback) {
						_message_callback(message[3] & 0x0F, message[2], message[1], message[0]);
					}
				}

				message_index = 0;
			}
		}
	}
	else
	{
		while(midi_usb_driver_rx(&byte, 1))
		{
			message[message_index++] = byte;
			if(message_index == 4)
			{
				if((message[0] & CIN_MASK) == CIN_SYSEX_START)
				{
					_midi20_buffer[0] = message[1];
					_midi20_buffer[1] = message[2];
					_midi20_buffer[2] = message[3];

					if(_sysex_callback) {
						_sysex_callback((message[0] & 0xF0) >> 4, _midi20_buffer, 3);
					}
				}
				else if((message[0] & CIN_MASK) == CIN_SINGLE_BYTE_COMMON_OR_SYSEX_END_ONE)
				{
					_midi20_buffer[0] = message[1];

					if(_sysex_callback) {
						_sysex_callback((message[0] & 0xF0) >> 4, _midi20_buffer, 1);
					}
				}
				else if((message[0] & CIN_MASK) == CIN_SYSEX_END_TWO)
				{
					_midi20_buffer[0] = message[1];
					_midi20_buffer[1] = message[2];

					if(_sysex_callback) {
						_sysex_callback((message[0] & 0xF0) >> 4, _midi20_buffer, 2);
					}
				}
				else if((message[0] & CIN_MASK) == CIN_SYSEX_END_THREE)
				{
					_midi20_buffer[0] = message[1];
					_midi20_buffer[1] = message[2];
					_midi20_buffer[2] = message[3];

					if(_sysex_callback) {
						_sysex_callback((message[0] & 0xF0) >> 4, _midi20_buffer, 3);
					}
				}
				else
				{
					if(_message_callback) {
						_message_callback((message[0] & 0xF0) >> 4, message[1], message[2], message[3]);
					}
				}

				message_index = 0;
			}
		}
	}
}

void midi_usb_note_on(uint8_t note, uint8_t channel, uint16_t velocity)
{
	if(IS_MIDI_2_0)
	{
#ifdef USE_VOICE_10_MESSAGE
		uint8_t message[4];
		message[3] = MIDI20_MESSAGE_TYPE_10_CHANNEL_VOICE;
		message[2] = MIDI_NOTE_ON | channel;
		message[1] = note;
		message[0] = velocity / 512;
#else
    uint8_t message[8];
		message[3] = MIDI20_MESSAGE_TYPE_20_CHANNEL_VOICE;
		message[2] = MIDI_NOTE_ON | channel;
		message[1] = note;
		message[0] = 0;
		message[7] = velocity >> 8;
		message[6] = velocity;
		message[5] = 0;
		message[4] = 0;
#endif
		midi_usb_driver_tx(message, sizeof(message));
	}
	else
	{
		uint32_t message = CIN_NOTE_ON | ((MIDI_NOTE_ON | channel) << 8) | (note << 16) | (velocity << 24);
		midi_usb_driver_tx((uint8_t*)&message, 4);
	}
}

void midi_usb_note_off(uint8_t note, uint8_t channel, uint16_t velocity)
{
	if(IS_MIDI_2_0)
	{
#ifdef USE_VOICE_10_MESSAGE
		uint8_t message[4];
		message[3] = MIDI20_MESSAGE_TYPE_10_CHANNEL_VOICE;
		message[2] = MIDI_NOTE_OFF | channel;
		message[1] = note;
		message[0] = velocity / 512;
#else
		uint8_t message[8];
		message[3] = MIDI20_MESSAGE_TYPE_20_CHANNEL_VOICE;
		message[2] = MIDI_NOTE_OFF | channel;
		message[1] = note;
		message[0] = 0;
		message[7] = velocity >> 8;
		message[6] = velocity;
		message[5] = 0;
		message[4] = 0;
#endif
    	midi_usb_driver_tx(message, sizeof(message));
	}
	else
	{
		uint32_t message = CIN_NOTE_OFF | ((MIDI_NOTE_OFF | channel) << 8) | (note << 16) | (velocity << 24);
		midi_usb_driver_tx((uint8_t*)&message, 4);
	}
}

void midi_usb_aftertouch(uint8_t note, uint8_t channel, uint16_t data)
{
	// keyboard does not support
}

void midi_usb_channel_pressure(uint8_t channel, uint16_t data)
{
	// keyboard does not support
}

void midi_usb_pitch_wheel(uint8_t channel, int32_t pitch)
{
	if(IS_MIDI_2_0)
	{
#ifdef USE_VOICE_10_MESSAGE
		pitch = (pitch + 8192) & 0x3FFF;

		uint8_t message[4];
		message[3] = MIDI20_MESSAGE_TYPE_10_CHANNEL_VOICE;
		message[2] = MIDI_PITCH_WHEEL | channel;
		message[1] = (pitch >> 7) & 0x7F;
		message[0] = pitch & 0x7F;
#else
		pitch += 0x80000000;

		uint8_t message[8];
		message[3] = MIDI20_MESSAGE_TYPE_20_CHANNEL_VOICE;
		message[2] = MIDI_PITCH_WHEEL | channel;
		message[1] = 0;
		message[0] = 0;
		message[7] = pitch >> 24;
		message[6] = pitch >> 16;
		message[5] = pitch >> 8;
		message[4] = pitch;
#endif
		midi_usb_driver_tx(message, sizeof(message));
	}
	else
	{
		pitch = (pitch + 8192) & 0x3FFF;
		uint32_t pitch_new = (pitch & 0x7F) | ((pitch << 1) & 0x7F00);
		uint32_t message = CIN_PITCH_BEND | ((MIDI_PITCH_WHEEL | channel) << 8) | (pitch_new << 16);
		midi_usb_driver_tx((uint8_t*)&message, 4);
	}
}

void midi_usb_controller(uint8_t index, uint8_t channel, uint32_t value)
{
	if(IS_MIDI_2_0)
	{
#ifdef USE_VOICE_10_MESSAGE
		uint8_t message[4];
		message[3] = MIDI20_MESSAGE_TYPE_10_CHANNEL_VOICE;
		message[2] = MIDI_CONTROLLER | channel;
		message[1] = index;
		message[0] = value;
#else
		uint8_t message[8];
		message[3] = MIDI20_MESSAGE_TYPE_20_CHANNEL_VOICE;
		message[2] = MIDI_CONTROLLER | channel;
		message[1] = index;
		message[0] = 0;
		message[7] = value >> 24;
		message[6] = value >> 16;
		message[5] = value >> 8;
		message[4] = value;
#endif
		midi_usb_driver_tx(message, sizeof(message));
	}
	else
	{
		uint32_t message = CIN_CONTROL_CHANGE | ((MIDI_CONTROLLER | channel) << 8) | (index << 16) | (value << 24);
		midi_usb_driver_tx((uint8_t*)&message, 4);
	}
}

void midi_usb_modulation_wheel(uint8_t channel, uint16_t modulation)
{
	if(IS_MIDI_2_0)
	{
#ifdef USE_VOICE_10_MESSAGE
		uint8_t message[4];

		if(modulation > 16383) {
			modulation = 16383;
		}

		message[3] = MIDI20_MESSAGE_TYPE_10_CHANNEL_VOICE;
		message[2] = MIDI_CONTROLLER | channel;
		message[1] = MIDI_CONT_MOD_WHEEL_FINE;
		message[0] = modulation & 0x7F;
		midi_usb_driver_tx(message, sizeof(message));

		message[3] = MIDI20_MESSAGE_TYPE_10_CHANNEL_VOICE;
		message[2] = MIDI_CONTROLLER | channel;
		message[1] = MIDI_CONT_MOD_WHEEL_COARSE;
		message[0] = (modulation >> 7) & 0x7F;
		midi_usb_driver_tx(message, sizeof(message));
#else
		modulation = modulation & 0x3FFF;

		uint8_t message[8];
		message[3] = MIDI20_MESSAGE_TYPE_20_CHANNEL_VOICE;
		message[2] = MIDI_CONTROLLER | channel;
		message[1] = MIDI_CONT_MOD_WHEEL_COARSE;
		message[0] = 0;
		message[7] = 0;
		message[6] = 0;
		message[5] = modulation >> 8;
		message[4] = modulation;
		midi_usb_driver_tx(message, sizeof(message));
#endif
	}
	else
	{
		modulation = modulation & 0x3FFF;

		uint32_t fine = modulation & 0x7F;
		uint32_t message = CIN_CONTROL_CHANGE | ((MIDI_CONTROLLER | channel) << 8) | (MIDI_CONT_MOD_WHEEL_FINE << 16) | (fine << 24);
		midi_usb_driver_tx((uint8_t*)&message, 4);

		uint32_t course = (modulation >> 7) & 0x7F;
		message = CIN_CONTROL_CHANGE | ((MIDI_CONTROLLER | channel) << 8) | (MIDI_CONT_MOD_WHEEL_COARSE << 16) | (course << 24);
		midi_usb_driver_tx((uint8_t*)&message, 4);
	}
}

void midi_usb_volume(uint8_t channel, uint16_t volume)
{
	if(IS_MIDI_2_0)
	{
#ifdef USE_VOICE_10_MESSAGE
		uint8_t message[4];

		if(volume > 16383) {
			volume = 16383;
		}

		message[3] = MIDI20_MESSAGE_TYPE_10_CHANNEL_VOICE;
		message[2] = MIDI_CONTROLLER | channel;
		message[1] = MIDI_CONT_VOLUME_FINE;
		message[0] = volume & 0x7F;
		midi_usb_driver_tx(message, sizeof(message));

		message[3] = MIDI20_MESSAGE_TYPE_10_CHANNEL_VOICE;
		message[2] = MIDI_CONTROLLER | channel;
		message[1] = MIDI_CONT_VOLUME_COARSE;
		message[0] = (volume >> 7) & 0x7F;
		midi_usb_driver_tx(message, sizeof(message));
#else
		volume = volume & 0x3FFF;

		uint8_t message[8];
		message[3] = MIDI20_MESSAGE_TYPE_20_CHANNEL_VOICE;
		message[2] = MIDI_CONTROLLER | channel;
		message[1] = MIDI_CONT_VOLUME_COARSE;
		message[0] = 0;
		message[7] = 0;
		message[6] = 0;
		message[5] = volume >> 8;
		message[4] = volume;
		midi_usb_driver_tx(message, sizeof(message));
#endif
	}
	else
	{
		volume = volume & 0x3FFF;

		uint32_t fine = volume & 0x7F;
		uint32_t message = CIN_CONTROL_CHANGE | ((MIDI_CONTROLLER | channel) << 8) | (MIDI_CONT_VOLUME_FINE << 16) | (fine << 24);
		midi_usb_driver_tx((uint8_t*)&message, 4);

		uint32_t course = (volume >> 7) & 0x7F;
		message = CIN_CONTROL_CHANGE | ((MIDI_CONTROLLER | channel) << 8) | (MIDI_CONT_VOLUME_COARSE << 16) | (course << 24);
		midi_usb_driver_tx((uint8_t*)&message, 4);
	}
}

void midi_usb_sustain(uint8_t channel, bool on)
{
	if(IS_MIDI_2_0)
	{
#ifdef USE_VOICE_10_MESSAGE
		uint8_t message[4];
		message[3] = MIDI20_MESSAGE_TYPE_10_CHANNEL_VOICE;
		message[2] = MIDI_CONTROLLER | channel;
		message[1] = MIDI_CONT_HOLD_PEDAL;
		message[0] = on ? 0x7F : 0x00;
		midi_usb_driver_tx(message, sizeof(message));
#else
		uint32_t hold_value = on ? 0x7F : 0;

		uint8_t message[8];
		message[3] = MIDI20_MESSAGE_TYPE_20_CHANNEL_VOICE;
		message[2] = MIDI_CONTROLLER | channel;
		message[1] = MIDI_CONT_HOLD_PEDAL;
		message[0] = 0;
		message[7] = 0;
		message[6] = 0;
		message[5] = hold_value >> 8;
		message[4] = hold_value;
		midi_usb_driver_tx(message, sizeof(message));
#endif
	}
	else
	{
		uint32_t hold_value = on ? 0x7F : 0;
		uint32_t message = CIN_CONTROL_CHANGE | ((MIDI_CONTROLLER | channel) << 8) | (MIDI_CONT_HOLD_PEDAL << 16) | (hold_value << 24);
		midi_usb_driver_tx((uint8_t*)&message, 4);
	}
}

void midi_usb_program_change(uint8_t channel, uint8_t program)
{
	if(IS_MIDI_2_0)
	{
#ifdef USE_VOICE_10_MESSAGE
		uint8_t message[4];
		message[3] = MIDI20_MESSAGE_TYPE_10_CHANNEL_VOICE;
		message[2] = MIDI_PROGRAM_CHANGE | channel;
		message[1] = program;
		message[0] = 0;
#else
		uint8_t message[8];
		message[3] = MIDI20_MESSAGE_TYPE_20_CHANNEL_VOICE;
		message[2] = MIDI_NOTE_ON | channel;
		message[1] = 0;
		message[0] = 0;
		message[7] = program;
		message[6] = 0;
		message[5] = 0;
		message[4] = 0;
#endif
		midi_usb_driver_tx(message, sizeof(message));
	}
	else
	{
		uint32_t message = CIN_PROGRAM_CHANGE | ((MIDI_PROGRAM_CHANGE | channel) << 8) | (program << 16);
		midi_usb_driver_tx((uint8_t*)&message, 4);
	}
}

void midi_usb_time_code(uint8_t code)
{
	if(IS_MIDI_2_0)
	{
		uint8_t message[4];
		message[3] = MIDI20_MESSAGE_TYPE_SYSTEM;
		message[2] = MIDI_QUARTER_FRAME_MESSAGE;
		message[1] = code;
		message[0] = 0;
		midi_usb_driver_tx(message, sizeof(message));
	}
	else
	{
		uint32_t message = CIN_TWO_BYTE_COMMON | (MIDI_QUARTER_FRAME_MESSAGE << 8) | (code << 16);
		midi_usb_driver_tx((uint8_t*)&message, 4);
	}
}

void midi_usb_song_position(uint16_t position)
{
	if(IS_MIDI_2_0)
	{
		uint8_t message[4];
		message[3] = MIDI20_MESSAGE_TYPE_SYSTEM;
		message[2] = MIDI_SONG_POSITION_POINTER;
		message[1] = position & 0x7F;
		message[0] = (position >> 7) & 0x7F;
		midi_usb_driver_tx(message, sizeof(message));
	}
	else
	{
		uint8_t low = position & 0x7F;
		uint8_t high = (position >> 7) & 0x7F;
		uint32_t message = CIN_THREE_BYTE_COMMON | (MIDI_SONG_POSITION_POINTER << 8) | (low << 16) | (high << 24);
		midi_usb_driver_tx((uint8_t*)&message, 4);
	}
}

void midi_usb_song_select(uint8_t song)
{
	if(IS_MIDI_2_0)
	{
		uint8_t message[4];
		message[3] = MIDI20_MESSAGE_TYPE_SYSTEM;
		message[2] = MIDI_SONG_SELECT;
		message[1] = song;
		message[0] = 0;
		midi_usb_driver_tx(message, sizeof(message));
	}
	else
	{
		uint32_t message = CIN_TWO_BYTE_COMMON | (MIDI_SONG_SELECT << 8) | (song << 16);
		midi_usb_driver_tx((uint8_t*)&message, 4);
	}
}

void midi_usb_generic_status(uint8_t status)
{
	if(IS_MIDI_2_0)
	{
		uint8_t message[4];
		message[3] = MIDI20_MESSAGE_TYPE_SYSTEM;
		message[2] = status;
		message[1] = 0;
		message[0] = 0;
		midi_usb_driver_tx(message, sizeof(message));
	}
	else
	{
		uint32_t message = CIN_SINGLE_BYTE | (status << 8);
		midi_usb_driver_tx((uint8_t*)&message, 4);
	}
}

void midi_usb_raw(uint8_t *pdata, uint32_t length)
{
	midi_usb_driver_tx(pdata, 4);
}

void midi_usb_register_message_callback(message_callback callback)
{
	_message_callback = callback;
}

void midi_usb_register_sysex_callback(sysex_callback callback)
{
	_sysex_callback = callback;
}

