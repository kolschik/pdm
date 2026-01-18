#pragma once
#include "gpio.h"
#include "errno.h"

#if defined (FREERTOS)
#include "cmsis_os.h"
#endif

typedef enum {
    led_off,
    led_on,
    led_blink,
    led_blink_inv, /// Мигает несколько раз
    led_flash_s_inv,
    led_flash_s,
    led_flash_m_inv,
    led_flash_m,
    led_flash_l_inv,
    led_flash_l,
    led_flash, /// Мигает медленно  
    led_run,
    led_run_shadow,   
    led_auto,   
} led_mode_t;

typedef enum {
    led_white,
    led_red,
    led_green,
    led_blue,
    led_orange
} led_color_t;

typedef struct {
    led_mode_t new_mode;    
    led_mode_t mode;
    led_color_t color;
    uint16_t counter;
    gpio_t * const gpio; 
    const uint8_t inv;  // Инвертировать уровни
}led_t;

typedef struct {
    led_t ** const led;
    uint8_t led_num;    
    uint32_t last_wakeup;
    uint8_t active_led;    
    uint16_t comm_counter;
}leds_t;



int led_init(leds_t *p_led);
void led_poll();
int led_change_index(uint8_t index, led_mode_t mode);
int led_change(led_t *led, led_mode_t mode);