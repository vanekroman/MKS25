#include "main.h"
#include "rfm95.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_rng.h"
#include "stm32f4xx_hal_tim.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define MSG_TIME 3000
#define BTN_TIME 5000

extern SPI_HandleTypeDef hspi3;
extern TIM_HandleTypeDef htim2;
extern RNG_HandleTypeDef hrng;

uint32_t get_precision_tick() {
    return (__HAL_TIM_GET_COUNTER(&htim2));
}

void precision_sleep_until(uint32_t target_ticks) {
    while (get_precision_tick() < target_ticks) {
        // DO NOTHING
    }
}

uint8_t random_int(uint8_t max) {
    uint32_t rng;
    HAL_RNG_GenerateRandomNumber(&hrng, &rng);
    return (rng % (max + 1));
}

// Create the handle for the RFM95 module.
rfm95_handle_t rfm95_handle = {
    .spi_handle = &hspi3,
    .nss_port = RFM95_NSS_GPIO_Port,
    .nss_pin = RFM95_NSS_Pin,
    .device_address = {
        0x26, 0x0B, 0x04, 0x81
    },
    .application_session_key = {
        0x40, 0x0A, 0xE6, 0x65, 0x1A, 0xA8, 0x92, 0x92,  0x12, 0xC0, 0x08, 0xEC, 0xC2, 0xC1, 0x53, 0x72
    },
    .network_session_key = {
        0x80, 0x70, 0x52, 0xDA, 0x22, 0x5F, 0x54, 0xC3, 0xB8,  0xDC, 0xAD, 0xA1, 0x8A, 0xF2, 0x85, 0x34
    },
    .receive_mode = RFM95_RECEIVE_MODE_NONE,
    .precision_tick_frequency = 20000,
    .get_precision_tick = get_precision_tick,
    .precision_sleep_until = precision_sleep_until,
    .random_int = random_int
};


uint32_t old_btn_tick;
uint32_t old_tick;
uint32_t counter = 0;
char send_data[32];
/**
 * @brief  called within main() function after generated code and used
 *         as user code main to prevent code removal durring code generation
 */
void maine(void) {
    HAL_TIM_Base_Start(&htim2);

    old_btn_tick = HAL_GetTick();
    old_tick = HAL_GetTick();

    // Initialise RFM95 module.
    if (!rfm95_init(&rfm95_handle)) {
        printf("RFM95 init failed\n\r");
        HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, 1);
        while (1) {}
    } 

    while (1) {
        if ((old_tick + MSG_TIME) <= HAL_GetTick() && (old_btn_tick + BTN_TIME) <= HAL_GetTick()) {
            sprintf(send_data, "240707;%ld", counter++);
            HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, 1);
            rfm95_send_receive_cycle(&rfm95_handle, (uint8_t *)send_data, strlen(send_data));
            HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, 0);

            old_tick = HAL_GetTick();
        }

        if ((HAL_GPIO_ReadPin(USER_Btn_GPIO_Port, USER_Btn_Pin) == GPIO_PIN_SET) && (old_btn_tick + BTN_TIME) <= HAL_GetTick()) {
            sprintf(send_data, "240707;press");
            HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, 1);
            rfm95_send_receive_cycle(&rfm95_handle, (uint8_t *)send_data, strlen(send_data));
            HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, 0);

            old_btn_tick= HAL_GetTick();
        }
    }
}

int __io_putchar(int ch) {
    ITM_SendChar(ch);
    return 0;
}

