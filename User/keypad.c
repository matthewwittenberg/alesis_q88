/*
 * keypad.c
 *
 *  Created on: Sep 20, 2023
 *      Author: matt
 */

#include "keypad.h"
#include "NUC100Series.h"

static keypad_event_callback _keypad_callback = NULL;

#define KEYPAD_ROW1 PA9
#define KEYPAD_ROW1_PORT PA
#define KEYPAD_ROW1_PORT_VAL 0
#define KEYPAD_ROW1_PIN 9

#define KEYPAD_ROW2 PA8
#define KEYPAD_ROW2_PORT PA
#define KEYPAD_ROW2_PORT_VAL 0
#define KEYPAD_ROW2_PIN 8

#define KEYPAD_ROW3 PB4
#define KEYPAD_ROW3_PORT PB
#define KEYPAD_ROW3_PORT_VAL 1
#define KEYPAD_ROW3_PIN 4

#define KEYPAD_COL1 PC9
#define KEYPAD_COL1_PORT PC
#define KEYPAD_COL1_PORT_VAL 2
#define KEYPAD_COL1_PIN 9

#define KEYPAD_COL2 PC7
#define KEYPAD_COL2_PORT PC
#define KEYPAD_COL2_PORT_VAL 2
#define KEYPAD_COL2_PIN 7

#define KEYPAD_COL3 PC3
#define KEYPAD_COL3_PORT PC
#define KEYPAD_COL3_PORT_VAL 2
#define KEYPAD_COL3_PIN 3

#define KEYPAD_COL4 PC1
#define KEYPAD_COL4_PORT PC
#define KEYPAD_COL4_PORT_VAL 2
#define KEYPAD_COL4_PIN 1

void keypad_init()
{
	// setup rows
	GPIO_SetMode(KEYPAD_ROW1_PORT, 1 << KEYPAD_ROW1_PIN, GPIO_PMD_OUTPUT);
	GPIO_SetMode(KEYPAD_ROW2_PORT, 1 << KEYPAD_ROW2_PIN, GPIO_PMD_OUTPUT);
	GPIO_SetMode(KEYPAD_ROW3_PORT, 1 << KEYPAD_ROW3_PIN, GPIO_PMD_OUTPUT);

	// set row pins high
	GPIO_PIN_DATA(KEYPAD_ROW1_PORT_VAL, KEYPAD_ROW1_PIN) = 1;
	GPIO_PIN_DATA(KEYPAD_ROW2_PORT_VAL, KEYPAD_ROW2_PIN) = 1;
	GPIO_PIN_DATA(KEYPAD_ROW2_PORT_VAL, KEYPAD_ROW3_PIN) = 1;

	// setup cols
	GPIO_SetMode(KEYPAD_COL1_PORT, 1 << KEYPAD_COL1_PIN, GPIO_PMD_INPUT);
	GPIO_SetMode(KEYPAD_COL2_PORT, 1 << KEYPAD_COL2_PIN, GPIO_PMD_INPUT);
	GPIO_SetMode(KEYPAD_COL3_PORT, 1 << KEYPAD_COL3_PIN, GPIO_PMD_INPUT);
	GPIO_SetMode(KEYPAD_COL4_PORT, 1 << KEYPAD_COL4_PIN, GPIO_PMD_INPUT);
}

void keypad_task()
{
}

void keypad_register_callback(keypad_event_callback callback)
{
	_keypad_callback = callback;
}
