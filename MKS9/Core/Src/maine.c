#include "maine.h"
#include "main.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_tim.h"
#include <stdint.h>
#include <stdio.h>

extern TIM_HandleTypeDef htim3;

static volatile char key = 0;
static const char super_secure_code[] = { '7', '9', '3', '2', '#' };


void maine(void) {

    HAL_TIM_Base_Start_IT(&htim3);  

    HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_RESET);

    uint32_t last_key_tick = HAL_GetTick(); 
    uint8_t index_correct = 0;

    while (1) {

        uint32_t now = HAL_GetTick();

        if ((now - last_key_tick) > 5000) {
            index_correct = 0;
            last_key_tick = now;
            printf("Timeout, index reset\n");
        }

        if (key != 0) {
            last_key_tick = HAL_GetTick(); 

            printf("pressed '%c'\n", key);
            HAL_Delay(500);

            if (key == super_secure_code[index_correct]) {
                index_correct++;
            } else {
                index_correct = 0;
            }

            if (index_correct >= sizeof(super_secure_code) ) {
                printf("Unlocked <3\n");
                HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_SET);
                HAL_Delay(500);
            }

            key = 0;  
        }
    }
}

int __io_putchar(int ch) {
    ITM_SendChar(ch);
    return 0;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    static uint8_t row = 0;
    static const char keyboard[4][4] = {
        { '1', '2', '3', 'A' },
        { '4', '5', '6', 'B' },
        { '7', '8', '9', 'C' },
        { '*', '0', '#', 'D' },
    };  

    if (key == 0) {
        if (HAL_GPIO_ReadPin(COL1_GPIO_Port, COL1_Pin) == GPIO_PIN_RESET) {
            key = keyboard[row][0];
        }
        if (HAL_GPIO_ReadPin(COL2_GPIO_Port, COL2_Pin) == GPIO_PIN_RESET) {
            key = keyboard[row][1];
        }
        if (HAL_GPIO_ReadPin(COL3_GPIO_Port, COL3_Pin) == GPIO_PIN_RESET) {
            key = keyboard[row][2];
        }
        if (HAL_GPIO_ReadPin(COL4_GPIO_Port, COL4_Pin) == GPIO_PIN_RESET) {
            key = keyboard[row][3];
        }
    }
    
    HAL_GPIO_WritePin(ROW1_GPIO_Port, ROW1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(ROW2_GPIO_Port, ROW2_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(ROW3_GPIO_Port, ROW3_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(ROW4_GPIO_Port, ROW4_Pin, GPIO_PIN_SET);
    
    switch (row) {
        case 0: 
            row = 1;
            HAL_GPIO_WritePin(ROW2_GPIO_Port, ROW2_Pin, GPIO_PIN_RESET);
            break;

        case 1: 
            row = 2;
            HAL_GPIO_WritePin(ROW3_GPIO_Port, ROW3_Pin, GPIO_PIN_RESET);
            break;
        case 2: 
            row = 3;
            HAL_GPIO_WritePin(ROW4_GPIO_Port, ROW4_Pin, GPIO_PIN_RESET);
            break;
        case 3: 
            row = 0;
            HAL_GPIO_WritePin(ROW1_GPIO_Port, ROW1_Pin, GPIO_PIN_RESET);
            break;
    }  
}  
