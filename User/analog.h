/*
 * adc.h
 *
 *  Created on: Sep 20, 2023
 *      Author: matt
 */

#ifndef ADC_H_
#define ADC_H_

#include <stdint.h>
#include <stdbool.h>

void analog_init();
void analog_get(int16_t *pmodulation, int16_t *ppitch, int16_t *pvolume, int16_t *pexpression);

#endif /* ADC_H_ */
