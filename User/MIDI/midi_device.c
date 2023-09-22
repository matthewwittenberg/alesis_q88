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
extern void midi_usb_pitch_wheel(uint8_t channel, int32_t pitch);
extern void midi_usb_modulation_wheel(uint8_t channel, uint16_t modulation);
extern void midi_usb_volume(uint8_t channel, uint16_t volume);
extern void midi_usb_sense();
extern void midi_usb_sustain(uint8_t channel, bool on);
extern void midi_usb_program_change(uint8_t channel, uint8_t program);
extern void midi_usb_task();

extern void midi_1_0_serial_init();
extern void midi_1_0_serial_note_on(uint8_t note, uint8_t channel, uint16_t velocity);
extern void midi_1_0_serial_note_off(uint8_t note, uint8_t channel, uint16_t velocity);
extern void midi_1_0_serial_pitch_wheel(uint8_t channel, int32_t pitch);
extern void midi_1_0_serial_modulation_wheel(uint8_t channel, uint16_t modulation);
extern void midi_1_0_serial_volume(uint8_t channel, uint16_t volume);
extern void midi_1_0_serial_sense();
extern void midi_1_0_serial_sustain(uint8_t channel, bool on);
extern void midi_1_0_serial_program_change(uint8_t channel, uint8_t program);
extern void midi_1_0_serial_task();

MIDI_DEVICE_T MIDI_USB_DEVICE;
MIDI_DEVICE_T MIDI_SERIAL_DEVICE;

void midi_device_init()
{
	MIDI_SERIAL_DEVICE.init = midi_1_0_serial_init;
	MIDI_SERIAL_DEVICE.note_on = midi_1_0_serial_note_on;
	MIDI_SERIAL_DEVICE.note_off = midi_1_0_serial_note_off;
	MIDI_SERIAL_DEVICE.pitch_wheel = midi_1_0_serial_pitch_wheel;
	MIDI_SERIAL_DEVICE.modulation_wheel = midi_1_0_serial_modulation_wheel;
	MIDI_SERIAL_DEVICE.volume = midi_1_0_serial_volume;
	MIDI_SERIAL_DEVICE.sense = midi_1_0_serial_sense;
	MIDI_SERIAL_DEVICE.sustain = midi_1_0_serial_sustain;
	MIDI_SERIAL_DEVICE.program_change = midi_1_0_serial_program_change;
	MIDI_SERIAL_DEVICE.task = midi_1_0_serial_task;
	MIDI_SERIAL_DEVICE.version = MIDI_VERSION_1_0;

	MIDI_USB_DEVICE.init = midi_usb_init;
	MIDI_USB_DEVICE.note_on = midi_usb_note_on;
	MIDI_USB_DEVICE.note_off = midi_usb_note_off;
	MIDI_USB_DEVICE.pitch_wheel = midi_usb_pitch_wheel;
	MIDI_USB_DEVICE.modulation_wheel = midi_usb_modulation_wheel;
	MIDI_USB_DEVICE.volume = midi_usb_volume;
	MIDI_USB_DEVICE.sense = midi_usb_sense;
	MIDI_USB_DEVICE.sustain = midi_usb_sustain;
	MIDI_USB_DEVICE.program_change = midi_usb_program_change;
	MIDI_USB_DEVICE.task = midi_usb_task;
#if MIDI_VERSION == 1
	MIDI_USB_DEVICE.version = MIDI_VERSION_1_0;
#elif MIDI_VERSION == 2
	MIDI_USB_DEVICE.version = MIDI_VERSION_2_0;
#endif

	MIDI_USB_DEVICE.init();
}
