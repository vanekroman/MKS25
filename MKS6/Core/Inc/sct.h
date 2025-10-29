/*
 * sct.h
 *
 *  created on: oct 8, 2025
 *      author: 240707
 */

#ifndef inc_sct_h_
#define inc_sct_h_

#include "main.h"

void sct_led(uint32_t value);
void sct_value(uint16_t value);
void sct_float(uint16_t value);
void sct_value_bar(uint16_t value, uint8_t bar);
void sct_init();

#endif /* inc_sct_h_ */
