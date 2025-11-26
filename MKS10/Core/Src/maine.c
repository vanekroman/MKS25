#include "maine.h"
#include "main.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_tim.h"
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "usbd_hid.h"

#include <math.h>

extern TIM_HandleTypeDef htim3;
extern USBD_HandleTypeDef hUsbDeviceFS;

// Function to send a mouse movement report
void step(int8_t dx, int8_t dy, bool presed) {
    uint8_t buff[4];
    buff[0] = presed; // no button press
    buff[1] = dx;   // shift X
    buff[2] = dy;   // shift Y
    buff[3] = 0;    // no scroll
    
    USBD_HID_SendReport(&hUsbDeviceFS, buff, sizeof(buff));
    HAL_Delay(USBD_HID_GetPollingInterval(&hUsbDeviceFS));
}

// Function to draw a circle
void draw_circle(float radius) {
    const int steps = 50;
    const float angle_step = (2.0f * M_PI) / steps;
    
    // Starting position tracking
    float x_start = radius * cosf(0);
    float y_start = radius * sinf(0);
    int sx = (int)roundf(x_start);
    int sy = (int)roundf(y_start);
    
    // Draw the circle by stepping through angles
    for (int i = 0; i <= steps; i++) {
        float phi = i * angle_step;
        
        // Calculate current position using parametric equations
        float x = radius * cosf(phi);
        float y = radius * sinf(phi);
        
        // Convert to integer coordinates
        int ix = (int)roundf(x);
        int iy = (int)roundf(y);
        
        // Calculate the difference from previous position
        int8_t dx = (int8_t)(ix - sx);
        int8_t dy = (int8_t)(iy - sy);
        
        // Send the movement command
        if (dx != 0 || dy != 0) {
            step(dx, dy, 1);
        }
        
        // Update previous position
        sx = ix;
        sy = iy;
    }
}

void draw_arc(float radius, float start_angle_rad, float end_angle_rad) {
    const int steps = 50;
    const float angle_step = (end_angle_rad - start_angle_rad) / steps;

    // Starting point
    float x_start = radius * cosf(start_angle_rad);
    float y_start = radius * sinf(start_angle_rad);
    int sx = (int)roundf(x_start);
    int sy = (int)roundf(y_start);

    // Draw the arc
    for (int i = 0; i <= steps; i++) {
        float phi = start_angle_rad + i * angle_step;

        float x = radius * cosf(phi);
        float y = radius * sinf(phi);

        int ix = (int)roundf(x);
        int iy = (int)roundf(y);

        int8_t dx = (int8_t)(ix - sx);
        int8_t dy = (int8_t)(iy - sy);

        if (dx != 0 || dy != 0) {
            step(dx, dy, 1);
        }

        sx = ix;
        sy = iy;
    }
}


void draw_smile(void) {
    float head_radius   = 200.0f;

    float eye_radius    = 40.0f;
    float eye_offset_x  = 300.0f;
    float eye_offset_y  = -50.0f;

    int mouth_radius  = 200;
    int mouth_start_deg = 210 + 180;
    int mouth_end_deg   = 330 + 180;

    float nose_end = 30;

    int mouth_steps = 40;

    float const move_l = 200.0f;
    step(-head_radius - move_l,0,false);
    draw_circle(head_radius);
    step(head_radius + move_l,0,false);

    step(-eye_offset_x, eye_offset_y, false);
    draw_circle(eye_radius);
    step(+eye_offset_x, -eye_offset_y, false);

    step(+eye_offset_x, eye_offset_y, false);
    draw_circle(eye_radius);
    step(-eye_offset_x - 10.0f, -eye_offset_y, false);

    step(0, -nose_end, true);

    step(0, -nose_end, false);

    step(-mouth_radius + 30, -mouth_radius , false);
    draw_arc(mouth_radius, mouth_start_deg * M_PI / 180.0f, mouth_end_deg * M_PI / 180.0f);
}


// Function to draw a spiral galaxy pattern
void draw_spiral_galaxy(void) {
    const int steps = 200;
    float radius = 5.0f;
    float radius_increment = 0.8f;
    
    int sx = 0;
    int sy = 0;
    
    for (int i = 0; i <= steps; i++) {
        float phi = i * 0.3f;  // Angle increment
        radius += radius_increment;
        
        float x = radius * cosf(phi);
        float y = radius * sinf(phi);
        
        int ix = (int)roundf(x);
        int iy = (int)roundf(y);
        
        int8_t dx = (int8_t)(ix - sx);
        int8_t dy = (int8_t)(iy - sy);
        
        if (dx != 0 || dy != 0) {
            step(dx, dy, true);
        }
        
        sx = ix;
        sy = iy;
    }
    
    step(0, 0, false);
}

void maine(void) {

    while (1) {

        // Simple debounce
        if (HAL_GPIO_ReadPin(USER_Btn_GPIO_Port, USER_Btn_Pin) == GPIO_PIN_SET) {
            HAL_Delay(50); 
            if (HAL_GPIO_ReadPin(USER_Btn_GPIO_Port, USER_Btn_Pin) == GPIO_PIN_SET) {

                draw_smile();

                step(0, 0, 0);
            }
        }
    }
}

