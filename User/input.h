/*
 * input.h
 *
 *  Created on: Sep 20, 2023
 *      Author: matt
 */

#ifndef INPUT_H_
#define INPUT_H_

#include <stdint.h>
#include <stdbool.h>

void input_init();
bool input_get_sustain();
bool input_get_expression();

#endif /* INPUT_H_ */
