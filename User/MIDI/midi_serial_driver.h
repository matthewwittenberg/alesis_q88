/*
 * midi_1_0_serial_driver.h
 *
 *  Created on: Sep 22, 2023
 *      Author: matt
 */

#ifndef MIDI_MIDI_1_0_SERIAL_DRIVER_H_
#define MIDI_MIDI_1_0_SERIAL_DRIVER_H_

#include <stdint.h>
#include <stdbool.h>

void midi_serial_driver_init();
void midi_serial_driver_tx(uint8_t *pmessage, uint32_t length);

#endif /* MIDI_MIDI_1_0_SERIAL_DRIVER_H_ */
