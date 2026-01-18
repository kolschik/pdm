#include "led.h"
static leds_t *leds;

const uint32_t SHORT_FLASH_PERIOD_MS = 100;
const uint32_t MED_FLASH_PERIOD_MS = 500;
const uint32_t LONG_FLASH_PERIOD_MS = 1000;
int led_init(leds_t *p_led) {
    if (p_led == 0){
        return EINVAL;
    }
    leds = p_led;
    leds->comm_counter = 0;
    leds->last_wakeup = 0;
    return 0;
}

void led_poll() {
#if defined (FREERTOS)
    const uint32_t ticks = xTaskGetTickCount();
#else
    const uint32_t ticks = HAL_GetTick();
#endif    
    const uint32_t tick_el = ticks - leds->last_wakeup;
    leds->last_wakeup = ticks;
    for (uint32_t i=0; i < leds->led_num; i++){
        int black = leds->led[i]->inv;
        leds->led[i]->counter += tick_el;

        if (leds->led[i]->new_mode != leds->led[i]->mode){
            leds->led[i]->counter = 0;
            leds->comm_counter = 0;
            leds->led[i]->mode = leds->led[i]->new_mode;
        }

        switch (leds->led[i]->mode){
        case led_on:
            black ^= 1;
            break;
        case led_flash_s_inv:
            black ^= 1;
            __attribute__ ((fallthrough));
        case led_flash_s:
            
            if (leds->led[i]->counter < SHORT_FLASH_PERIOD_MS){
                black ^= 1;
            } else if (leds->led[i]->counter >= SHORT_FLASH_PERIOD_MS * 2) {
                leds->led[i]->counter = 0;
            }
            break;
        case led_flash_m_inv:
            black ^= 1;
            __attribute__ ((fallthrough));
        case led_flash_m:
            if (leds->led[i]->counter < MED_FLASH_PERIOD_MS){
                black ^= 1;
            } else if (leds->led[i]->counter >= MED_FLASH_PERIOD_MS * 2) {
                leds->led[i]->counter = 0;
            }
            break;
        case led_flash_l_inv:
            black ^= 1;
            __attribute__ ((fallthrough));
        case led_flash_l:
            if (leds->led[i]->counter < LONG_FLASH_PERIOD_MS){
                black ^= 1;
            } else if (leds->led[i]->counter >= LONG_FLASH_PERIOD_MS * 2) {
                leds->led[i]->counter = 0;
            }
            break;     
        case led_blink:
            if (leds->led[i]->counter < SHORT_FLASH_PERIOD_MS){
                black = 0;
            } else if (leds->led[i]->counter >= LONG_FLASH_PERIOD_MS) {
                leds->led[i]->counter = 0;
            }        
            break;
        case led_run_shadow:
            black ^= 1;
            __attribute__ ((fallthrough));
        case led_run:
            leds->comm_counter += tick_el;        
            if (leds->comm_counter > MED_FLASH_PERIOD_MS){
                leds->comm_counter = 0;
                leds->active_led++;
            }
            if (leds->active_led >= leds->led_num){
                leds->active_led = 0;
            }
            if (leds->active_led == i){
                black ^= 1;
            }
            break; 
        default:
            break;                       
        }
        gpio_set(leds->led[i]->gpio, black);
    }
}

int led_change_index(uint8_t index, led_mode_t mode) {
    if (index >= leds->led_num){
        return EINVAL;
    }
    leds->led[index]->new_mode = mode;
    return 0;
}

int led_change(led_t *led, led_mode_t mode) {
    if (led == 0){
        return EINVAL;
    }
    led->new_mode = mode;
    return 0;
}