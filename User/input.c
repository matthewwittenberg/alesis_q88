/*
 * input.c
 *
 *  Created on: Sep 20, 2023
 *      Author: matt
 */

#include "input.h"
#include "NUC100Series.h"

#define INPUT_SUSTAIN PB7
#define INPUT_SUSTAIN_PORT PB
#define INPUT_SUSTAIN_PIN 7

void input_init()
{
	GPIO_SetMode(INPUT_SUSTAIN_PORT, 1 << INPUT_SUSTAIN_PIN, GPIO_PMD_INPUT);
}

bool input_get_sustain()
{
	return INPUT_SUSTAIN ? false : true;
}

