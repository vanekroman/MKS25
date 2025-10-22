#include "main.h"
#include "sct.h"
#include "stm32f0xx_hal.h"
#include "stm32f0xx_hal_adc.h"
#include "stm32f0xx_hal_adc_ex.h"
#include "stm32f0xx_hal_gpio.h"
#include <stdint.h>

#define POT_MAX 500

/* Temperature sensor calibration value address */
#define TEMP110_CAL_ADDR ((uint16_t*) ((uint32_t) 0x1FFFF7C2))
#define TEMP30_CAL_ADDR ((uint16_t*) ((uint32_t) 0x1FFFF7B8))
#define VDD_CALIB ((uint16_t) (330))
#define VDD_APPLI ((uint16_t) (300))
/* Internal voltage reference calibration value address */
#define VREFINT_CAL_ADDR ((uint16_t*) ((uint32_t) 0x1FFFF7BA))

// ADC exponential smoothing
#define ADC_Q   12

extern ADC_HandleTypeDef hadc;
// UART_HandleTypeDef huart2;

volatile uint32_t raw_pot;
volatile uint32_t avg_pot;
volatile uint32_t raw_temp;
volatile uint32_t raw_volt;

// contains the computed temperature
int32_t temperature_C;

/**
 * @brief  called within main() function after generated code and used
 *         as user code main to prevent code removal durring code generation
 */
void maine(void) {

    // init LED driver
    sct_init();

    HAL_ADCEx_Calibration_Start(&hadc);
    HAL_ADC_Start_IT(&hadc);

    while (1) {
        static enum { SHOW_POT, SHOW_VOLT, SHOW_TEMP } state = SHOW_POT;
        static uint32_t lastChangeTime = 0;
        static uint16_t value = 0;
        static uint8_t bar = 0;

        // Add delay to display the value on the segment display
        if (state != SHOW_POT && HAL_GetTick() - lastChangeTime >= 1000) {
            state = SHOW_POT;
        }

        // Change the state based on the button press
        if (HAL_GPIO_ReadPin(S1_GPIO_Port, S1_Pin) == GPIO_PIN_RESET) {
            state = SHOW_VOLT;
            lastChangeTime = HAL_GetTick();
        }

        if (HAL_GPIO_ReadPin(S2_GPIO_Port, S2_Pin) == GPIO_PIN_RESET) {
            state = SHOW_TEMP;
            lastChangeTime = HAL_GetTick();
        }

        // Behaviour based on the state
        if (state == SHOW_VOLT) {
            uint32_t voltage = 330 * (*VREFINT_CAL_ADDR) / raw_volt;
            value = voltage;


        } else if (state == SHOW_TEMP) {
            int32_t temperature = (raw_temp - (int32_t)(*TEMP30_CAL_ADDR));
            temperature = temperature * (int32_t)(110 - 30);
            temperature = temperature / (int32_t)(*TEMP110_CAL_ADDR - *TEMP30_CAL_ADDR);
            temperature = temperature + 30;
            value = temperature;

        } else {// state == SHOW_POT
            uint16_t pot = (raw_pot * POT_MAX) / 4096;
            bar = (pot * 9) / POT_MAX;
            value = pot;
        }

        // display the bar and the value
        sct_value_bar(value, bar);

        HAL_Delay(50);
    }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
    static uint32_t channel = 0;

    if (channel == 0) {
        raw_pot = avg_pot >> ADC_Q;
        avg_pot -= raw_pot;
        avg_pot += HAL_ADC_GetValue(hadc);

    } else if (channel == 1) {
        raw_temp = HAL_ADC_GetValue(hadc);

    } else {
        raw_volt = HAL_ADC_GetValue(hadc);
    }

    if (__HAL_ADC_GET_FLAG(hadc, ADC_FLAG_EOS)) channel = 0;
    else channel++;
}


