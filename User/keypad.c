/*
 * keypad.c
 *
 *  Created on: Sep 20, 2023
 *      Author: matt
 */

#include "keypad.h"
#include "NUC100Series.h"
#include "sys_timer.h"
#include <string.h>

static keypad_event_callback _keypad_callback = NULL;

#define KEYPAD_DEBOUNCE_MS 50
#define KEYPAD_DELAY_LOOP_COUNT 10
#define KEYPAD_DELAY() delay = KEYPAD_DELAY_LOOP_COUNT; while(--delay){}

// the matrix
//           COL1      COL2      COL3      COL4
// ROW1      ENTER     STOP      PLAY      RECORD
// ROW2      UP        DOWN      LEFT      RIGHT
// ROW3      ADVANCE   OCTAVE+   OCTAVE-

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

// NOTE: COLUMNS ARE SHARED WITH THE KEYBOARD MODULE

#define KEYPAD_COL1 PC1
#define KEYPAD_COL1_PORT PC
#define KEYPAD_COL1_PORT_VAL 2
#define KEYPAD_COL1_PIN 1

#define KEYPAD_COL2 PC3
#define KEYPAD_COL2_PORT PC
#define KEYPAD_COL2_PORT_VAL 2
#define KEYPAD_COL2_PIN 3

#define KEYPAD_COL3 PC7
#define KEYPAD_COL3_PORT PC
#define KEYPAD_COL3_PORT_VAL 2
#define KEYPAD_COL3_PIN 7

#define KEYPAD_COL4 PC9
#define KEYPAD_COL4_PORT PC
#define KEYPAD_COL4_PORT_VAL 2
#define KEYPAD_COL4_PIN 9

typedef enum
{
	KEYPAD_KEY_RELEASE,
	KEYPAD_KEY_DETECT,
	KEYPAD_KEY_PRESS,
} KEYPAD_PRESS_STATUS_T;

typedef struct
{
	KEYPAD_PRESS_STATUS_T status;
	uint32_t detect_ms;
	KEYPAD_KEY_T key;
	uint32_t col;
} KEYPAD_KEY_STATE_T;

KEYPAD_KEY_STATE_T _keypad_keys[KEYPAD_KEY_TOTAL];

void keypad_init()
{
	// setup rows
	GPIO_SetMode(KEYPAD_ROW1_PORT, 1 << KEYPAD_ROW1_PIN, GPIO_PMD_OUTPUT);
	GPIO_SetMode(KEYPAD_ROW2_PORT, 1 << KEYPAD_ROW2_PIN, GPIO_PMD_OUTPUT);
	GPIO_SetMode(KEYPAD_ROW3_PORT, 1 << KEYPAD_ROW3_PIN, GPIO_PMD_OUTPUT);

	// set row pins low
	KEYPAD_ROW1 = 0;
	KEYPAD_ROW2 = 0;
	KEYPAD_ROW3 = 0;

	// setup cols
	GPIO_SetMode(KEYPAD_COL1_PORT, 1 << KEYPAD_COL1_PIN, GPIO_PMD_INPUT);
	GPIO_SetMode(KEYPAD_COL2_PORT, 1 << KEYPAD_COL2_PIN, GPIO_PMD_INPUT);
	GPIO_SetMode(KEYPAD_COL3_PORT, 1 << KEYPAD_COL3_PIN, GPIO_PMD_INPUT);
	GPIO_SetMode(KEYPAD_COL4_PORT, 1 << KEYPAD_COL4_PIN, GPIO_PMD_INPUT);

	memset(_keypad_keys, 0, sizeof(_keypad_keys));
}

void keypad_process_key(KEYPAD_KEY_T key, bool pressed)
{
	if(pressed)
	{
		if(_keypad_keys[key].status == KEYPAD_KEY_RELEASE)
		{
			_keypad_keys[key].detect_ms = get_ms();
			_keypad_keys[key].status = KEYPAD_KEY_DETECT;
		}
		else if(_keypad_keys[key].status == KEYPAD_KEY_DETECT)
		{
			if((_keypad_keys[key].detect_ms + KEYPAD_DEBOUNCE_MS) < get_ms())
			{
				_keypad_keys[key].status = KEYPAD_KEY_PRESS;
				_keypad_callback(KEYPAD_EVENT_PRESS, key);
			}
		}
	}
	else
	{
		if(_keypad_keys[key].status == KEYPAD_KEY_PRESS)
		{
			_keypad_keys[key].status = KEYPAD_KEY_RELEASE;
			_keypad_callback(KEYPAD_EVENT_RELEASE, key);
		}
	}
}

void keypad_task()
{
	volatile uint32_t delay;

	// all row pins high
	KEYPAD_ROW1 = 1;
	KEYPAD_ROW2 = 1;
	KEYPAD_ROW3 = 1;

	KEYPAD_DELAY();

	if(KEYPAD_COL1)
	{
		KEYPAD_ROW1 = 0;
		KEYPAD_DELAY();
		keypad_process_key(KEYPAD_KEY_ENTER, KEYPAD_COL1 == 0 ? true : false);
		KEYPAD_ROW1 = 1;

		KEYPAD_ROW2 = 0;
		KEYPAD_DELAY();
		keypad_process_key(KEYPAD_KEY_UP, KEYPAD_COL1 == 0 ? true : false);
		KEYPAD_ROW2 = 1;

		KEYPAD_ROW3 = 0;
		KEYPAD_DELAY();
		keypad_process_key(KEYPAD_KEY_ADVANCE, KEYPAD_COL1 == 0 ? true : false);
		KEYPAD_ROW3 = 1;

		KEYPAD_DELAY();
	}
	else
	{
		keypad_process_key(KEYPAD_KEY_ENTER, false);
		keypad_process_key(KEYPAD_KEY_UP, false);
		keypad_process_key(KEYPAD_KEY_ADVANCE, false);
	}

	if(KEYPAD_COL2)
	{
		KEYPAD_ROW1 = 0;
		KEYPAD_DELAY();
		keypad_process_key(KEYPAD_KEY_STOP, KEYPAD_COL2 == 0 ? true : false);
		KEYPAD_ROW1 = 1;

		KEYPAD_ROW2 = 0;
		KEYPAD_DELAY();
		keypad_process_key(KEYPAD_KEY_DOWN, KEYPAD_COL2 == 0 ? true : false);
		KEYPAD_ROW2 = 1;

		KEYPAD_ROW3 = 0;
		KEYPAD_DELAY();
		keypad_process_key(KEYPAD_KEY_OCTAVE_PLUS, KEYPAD_COL2 == 0 ? true : false);
		KEYPAD_ROW3 = 1;

		KEYPAD_DELAY();
	}
	else
	{
		keypad_process_key(KEYPAD_KEY_STOP, false);
		keypad_process_key(KEYPAD_KEY_DOWN, false);
		keypad_process_key(KEYPAD_KEY_OCTAVE_PLUS, false);
	}

	if(KEYPAD_COL3)
	{
		KEYPAD_ROW1 = 0;
		KEYPAD_DELAY();
		keypad_process_key(KEYPAD_KEY_PLAY, KEYPAD_COL3 == 0 ? true : false);
		KEYPAD_ROW1 = 1;

		KEYPAD_ROW2 = 0;
		KEYPAD_DELAY();
		keypad_process_key(KEYPAD_KEY_LEFT, KEYPAD_COL3 == 0 ? true : false);
		KEYPAD_ROW2 = 1;

		KEYPAD_ROW3 = 0;
		KEYPAD_DELAY();
		keypad_process_key(KEYPAD_KEY_OCTAVE_MINUS, KEYPAD_COL3 == 0 ? true : false);
		KEYPAD_ROW3 = 1;

		KEYPAD_DELAY();
	}
	else
	{
		keypad_process_key(KEYPAD_KEY_PLAY, false);
		keypad_process_key(KEYPAD_KEY_LEFT, false);
		keypad_process_key(KEYPAD_KEY_OCTAVE_MINUS, false);
	}

	if(KEYPAD_COL4)
	{
		KEYPAD_ROW1 = 0;
		KEYPAD_DELAY();
		keypad_process_key(KEYPAD_KEY_RECORD, KEYPAD_COL4 == 0 ? true : false);
		KEYPAD_ROW1 = 1;

		KEYPAD_ROW2 = 0;
		KEYPAD_DELAY();
		keypad_process_key(KEYPAD_KEY_RIGHT, KEYPAD_COL4 == 0 ? true : false);
		KEYPAD_ROW2 = 1;
	}
	else
	{
		keypad_process_key(KEYPAD_KEY_RECORD, false);
		keypad_process_key(KEYPAD_KEY_RIGHT, false);
	}

	// all row pins low
	KEYPAD_ROW1 = 0;
	KEYPAD_ROW2 = 0;
	KEYPAD_ROW3 = 0;
}

void keypad_register_callback(keypad_event_callback callback)
{
	_keypad_callback = callback;
}
