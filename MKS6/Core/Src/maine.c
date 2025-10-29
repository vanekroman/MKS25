#include "maine.h"
#include "1wire.h"
#include "stm32f0xx_hal.h"
#include "sct.h"
#include "adc.h"

extern ADC_HandleTypeDef hadc;

/**
 * @brief  called within main() function after generated code and used
 *         as user code main to prevent code removal durring code generation
 */
void maine(void) {
    HAL_ADCEx_Calibration_Start(&hadc);
    HAL_ADC_Start(&hadc);

    OWInit();
    sct_init();

    int16_t temperature;

    while (1) {
        static uint32_t lastChangeTime = 0;
        static uint16_t value = 0;

        static enum {SHOW_NONE, SHOW_TEMP1, SHOW_TEMP2 } state = SHOW_TEMP1;

        // Add delay to display the value on the segment display
        if (state != SHOW_NONE && HAL_GetTick() - lastChangeTime >= CONVERT_T_DELAY) {
            state = SHOW_NONE;
        }

        // Change the state based on the button press
        if (HAL_GPIO_ReadPin(S1_GPIO_Port, S1_Pin) == GPIO_PIN_RESET) {
            state = SHOW_TEMP1;
            lastChangeTime = HAL_GetTick();
        }
        else if (HAL_GPIO_ReadPin(S2_GPIO_Port, S2_Pin) == GPIO_PIN_RESET) {
            state = SHOW_TEMP2;
            lastChangeTime = HAL_GetTick();
        }

        // Behaviour based on the state
        if (state == SHOW_TEMP1) {
            if (OWReadTemperature(&temperature)) {
                sct_float(temperature / 10);
            } else {
                sct_value(0);
            }
            OWConvertAll();

        } else if (state == SHOW_TEMP2) {
            uint32_t index = HAL_ADC_GetValue(&hadc);
            temperature = ntc_conv[index];
            sct_float(temperature);
        } else {
            sct_value(0);
        }

        HAL_Delay(50);
    }
}
