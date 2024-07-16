#ifndef _MIDI_DEVICE_H_
#define _MIDI_DEVICE_H_

#include <stdint.h>
#include <stdbool.h>
#include "midi_spec.h"

typedef enum
{
    MIDI_VERSION_1_0,
    MIDI_VERSION_2_0,
} MIDI_VERSION_T;

typedef struct
{
    void (*init)();
    void (*note_on)(uint8_t note, uint8_t channel, uint16_t velocity);
    void (*note_off)(uint8_t note, uint8_t channel, uint16_t velocity);
    void (*aftertouch)(uint8_t note, uint8_t channel, uint16_t data);
    void (*channel_pressure)(uint8_t channel, uint16_t data);
    void (*pitch_wheel)(uint8_t channel, int32_t pitch);
    void (*controller)(uint8_t index, uint8_t channel, uint32_t value);
    void (*modulation_wheel)(uint8_t channel, uint16_t modulation);
    void (*volume)(uint8_t channel, uint16_t volume);
    void (*sustain)(uint8_t channel, bool on);
    void (*program_change)(uint8_t channel, uint8_t program);
    void (*time_code)(uint8_t code);
    void (*song_position)(uint16_t position);
    void (*song_select)(uint8_t song);
    void (*generic_status)(uint8_t status);
    void (*raw)(uint8_t *pdata, uint32_t length);
    void (*task)();
    void (*register_message_callback)(message_callback callback);
    void (*register_sysex_callback)(sysex_callback callback);
    MIDI_VERSION_T version;
} MIDI_DEVICE_T;

void midi_device_init();

extern MIDI_DEVICE_T MIDI_USB_DEVICE;
extern MIDI_DEVICE_T MIDI_SERIAL_DEVICE;

#endif
