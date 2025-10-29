/*
 * sct.c
 *
 *  Created on: Oct 8, 2025
 *      Author: 240707
 */

#include "sct.h"
#include "main.h"
#include <stdint.h>

static const uint32_t reg_values[4][10] = {
    {
       // PCDE--------GFAB @ DIS1
       0b0111000000000111 << 16,
       0b0100000000000001 << 16,
       0b0011000000001011 << 16,
       0b0110000000001011 << 16,
       0b0100000000001101 << 16,
       0b0110000000001110 << 16,
       0b0111000000001110 << 16,
       0b0100000000000011 << 16,
       0b0111000000001111 << 16,
       0b0110000000001111 << 16,
   },
   {
       //----PCDEGFAB---- @ DIS2
       0b0000011101110000 << 0,
       0b0000010000010000 << 0,
       0b0000001110110000 << 0,
       0b0000011010110000 << 0,
       0b0000010011010000 << 0,
       0b0000011011100000 << 0,
       0b0000011111100000 << 0,
       0b0000010000110000 << 0,
       0b0000011111110000 << 0,
       0b0000011011110000 << 0,
   },
   {
       // PCDE--------GFAB @ DIS3
       0b0111000000000111 << 0,
       0b0100000000000001 << 0,
       0b0011000000001011 << 0,
       0b0110000000001011 << 0,
       0b0100000000001101 << 0,
       0b0110000000001110 << 0,
       0b0111000000001110 << 0,
       0b0100000000000011 << 0,
       0b0111000000001111 << 0,
       0b0110000000001111 << 0,
   },
   {
       //----43215678---- @ LED
       0b0000000000000000 << 16,
       0b0000000100000000 << 16,
       0b0000001100000000 << 16,
       0b0000011100000000 << 16,
       0b0000111100000000 << 16,
       0b0000111110000000 << 16,
       0b0000111111000000 << 16,
       0b0000111111100000 << 16,
       0b0000111111110000 << 16,
   }
};

void sct_led(uint32_t value) {
    for (uint32_t i = 0; i < sizeof(i) * 8; i++) {
        HAL_GPIO_WritePin(SCT_SDI_GPIO_Port, SCT_SDI_Pin, (value >> i) & 0b1);

        HAL_GPIO_WritePin(SCT_CLK_GPIO_Port, SCT_CLK_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(SCT_CLK_GPIO_Port, SCT_CLK_Pin, GPIO_PIN_RESET);
    }

    HAL_GPIO_WritePin(SCT_NLA_GPIO_Port, SCT_NLA_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(SCT_NLA_GPIO_Port, SCT_NLA_Pin, GPIO_PIN_RESET);
}

void sct_value(uint16_t value) {
    uint32_t reg = 0U;
    reg |= reg_values[0][value / 100 % 10];
    reg |= reg_values[1][value / 10 % 10];
    reg |= reg_values[2][value / 1 % 10];

    sct_led(reg);
}

void sct_float(uint16_t value) {
    uint32_t reg = 0U;
    reg |= reg_values[0][value / 100 % 10];
    reg |= reg_values[1][value / 10 % 10];
    reg |= reg_values[2][value / 1 % 10];

    if (value >= 0) {
        reg |= 0b0000100000000000 << 0;
    }

    sct_led(reg);
}


// podle zadání by se funkce měla deklarovat jako
// void sct_value(uint16_t, uint8_t led);
void sct_value_bar(uint16_t value, uint8_t bar) { 
    uint32_t reg = 0U;
    reg |= reg_values[0][value / 100 % 10];
    reg |= reg_values[1][value / 10 % 10];
    reg |= reg_values[2][value / 1 % 10];

    reg |= reg_values[3][bar];

    sct_led(reg);
}

void sct_init() { sct_led(0U); }
