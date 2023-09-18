#ifndef _MIDI_DEVICE_H_
#define _MIDI_DEVICE_H_

#include <stdint.h>
#include <stdbool.h>

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
    void (*pitch_wheel)(uint8_t channel, int32_t pitch);
    void (*modulation_wheel)(uint8_t channel, uint16_t modulation);
    void (*volume)(uint8_t channel, uint16_t volume);
    void (*sense)();
    void (*sustain)(uint8_t channel, bool on);
    void (*task)();
    MIDI_VERSION_T version;
} USBD_MIDI_DEVICE_T;

void midi_device_init();

extern USBD_MIDI_DEVICE_T MIDI_DEVICE;

#endif
