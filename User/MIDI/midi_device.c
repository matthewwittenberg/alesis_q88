/*
 * midi_device.c
 *
 *  Created on: Sep 18, 2023
 *      Author: matt
 */

#include "midi_device.h"

extern void midi_usb_init();
extern void midi_usb_note_on(uint8_t note, uint8_t channel, uint16_t velocity);
extern void midi_usb_note_off(uint8_t note, uint8_t channel, uint16_t velocity);
extern void midi_usb_aftertouch(uint8_t note, uint8_t channel, uint16_t data);
extern void midi_usb_channel_pressure(uint8_t channel, uint16_t data);
extern void midi_usb_pitch_wheel(uint8_t channel, int32_t pitch);
extern void midi_usb_controller(uint8_t index, uint8_t channel, uint32_t value);
extern void midi_usb_modulation_wheel(uint8_t channel, uint16_t modulation);
extern void midi_usb_volume(uint8_t channel, uint16_t volume);
extern void midi_usb_sustain(uint8_t channel, bool on);
extern void midi_usb_program_change(uint8_t channel, uint8_t program);
extern void midi_usb_time_code(uint8_t code);
extern void midi_usb_song_position(uint16_t position);
extern void midi_usb_song_select(uint8_t song);
extern void midi_usb_generic_status(uint8_t status);
extern void midi_usb_raw(uint8_t *pdata, uint32_t length);
extern void midi_usb_task();
extern void midi_usb_register_message_callback(message_callback callback);
extern void midi_usb_register_sysex_callback(sysex_callback callback);

extern void midi_serial_init();
extern void midi_serial_note_on(uint8_t note, uint8_t channel, uint16_t velocity);
extern void midi_serial_note_off(uint8_t note, uint8_t channel, uint16_t velocity);
extern void midi_serial_aftertouch(uint8_t note, uint8_t channel, uint16_t data);
extern void midi_serial_channel_pressure(uint8_t channel, uint16_t data);
extern void midi_serial_pitch_wheel(uint8_t channel, int32_t pitch);
extern void midi_serial_controller(uint8_t index, uint8_t channel, uint32_t value);
extern void midi_serial_modulation_wheel(uint8_t channel, uint16_t modulation);
extern void midi_serial_volume(uint8_t channel, uint16_t volume);
extern void midi_serial_sustain(uint8_t channel, bool on);
extern void midi_serial_program_change(uint8_t channel, uint8_t program);
extern void midi_serial_time_code(uint8_t code);
extern void midi_serial_song_position(uint16_t position);
extern void midi_serial_song_select(uint8_t song);
extern void midi_serial_generic_status(uint8_t status);
extern void midi_serial_raw(uint8_t *pdata, uint32_t length);
extern void midi_serial_task();
extern void midi_serial_register_message_callback(message_callback callback);
extern void midi_serial_register_sysex_callback(sysex_callback callback);

MIDI_DEVICE_T MIDI_USB_DEVICE;
MIDI_DEVICE_T MIDI_SERIAL_DEVICE;

void midi_device_init()
{
	MIDI_SERIAL_DEVICE.init = midi_serial_init;
	MIDI_SERIAL_DEVICE.note_on = midi_serial_note_on;
	MIDI_SERIAL_DEVICE.note_off = midi_serial_note_off;
	MIDI_SERIAL_DEVICE.aftertouch = midi_serial_aftertouch;
	MIDI_SERIAL_DEVICE.channel_pressure = midi_serial_channel_pressure;
	MIDI_SERIAL_DEVICE.pitch_wheel = midi_serial_pitch_wheel;
	MIDI_SERIAL_DEVICE.controller = midi_serial_controller;
	MIDI_SERIAL_DEVICE.modulation_wheel = midi_serial_modulation_wheel;
	MIDI_SERIAL_DEVICE.volume = midi_serial_volume;
	MIDI_SERIAL_DEVICE.sustain = midi_serial_sustain;
	MIDI_SERIAL_DEVICE.program_change = midi_serial_program_change;
	MIDI_SERIAL_DEVICE.time_code = midi_serial_time_code;
	MIDI_SERIAL_DEVICE.song_position = midi_serial_song_position;
	MIDI_SERIAL_DEVICE.song_select = midi_serial_song_select;
	MIDI_SERIAL_DEVICE.generic_status = midi_serial_generic_status;
	MIDI_SERIAL_DEVICE.raw = midi_serial_raw;
	MIDI_SERIAL_DEVICE.task = midi_serial_task;
	MIDI_SERIAL_DEVICE.register_message_callback = midi_serial_register_message_callback;
	MIDI_SERIAL_DEVICE.register_sysex_callback = midi_serial_register_sysex_callback;
	MIDI_SERIAL_DEVICE.version = MIDI_VERSION_1_0;
	MIDI_SERIAL_DEVICE.init();

	MIDI_USB_DEVICE.init = midi_usb_init;
	MIDI_USB_DEVICE.note_on = midi_usb_note_on;
	MIDI_USB_DEVICE.note_off = midi_usb_note_off;
	MIDI_USB_DEVICE.aftertouch = midi_usb_aftertouch;
	MIDI_USB_DEVICE.channel_pressure = midi_usb_channel_pressure;
	MIDI_USB_DEVICE.pitch_wheel = midi_usb_pitch_wheel;
	MIDI_USB_DEVICE.controller = midi_usb_controller;
	MIDI_USB_DEVICE.modulation_wheel = midi_usb_modulation_wheel;
	MIDI_USB_DEVICE.volume = midi_usb_volume;
	MIDI_USB_DEVICE.sustain = midi_usb_sustain;
	MIDI_USB_DEVICE.program_change = midi_usb_program_change;
	MIDI_USB_DEVICE.time_code = midi_usb_time_code;
	MIDI_USB_DEVICE.song_position = midi_usb_song_position;
	MIDI_USB_DEVICE.song_select = midi_usb_song_select;
	MIDI_USB_DEVICE.generic_status = midi_usb_generic_status;
	MIDI_USB_DEVICE.raw = midi_usb_raw;
	MIDI_USB_DEVICE.register_message_callback = midi_usb_register_message_callback;
	MIDI_USB_DEVICE.register_sysex_callback = midi_usb_register_sysex_callback;
	MIDI_USB_DEVICE.task = midi_usb_task;
	MIDI_USB_DEVICE.version = MIDI_VERSION_1_0;
	MIDI_USB_DEVICE.init();
}
