/*
 * sct.h
 *
 *  Created on: Oct 8, 2025
 *      Author: 240707
 */

#ifndef INC_SCT_H_
#define INC_SCT_H_

#include "main.h"

void sct_led(uint32_t value);
void sct_value(uint16_t value);
void sct_value_bar(uint16_t value, uint8_t bar);
void sct_init();

#endif /* INC_SCT_H_ */
