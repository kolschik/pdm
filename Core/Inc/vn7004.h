#pragma once

#include "stdint.h"
#include "gpio.h"

typedef enum {
    vn7004_status_off,
    vn7004_status_on,
    vn7004_status_ocp,
    vn7004_status_short_gnd,
    vn7004_status_short_vcc
}vn7004_stat_t;

typedef struct {
    gpio_t *const en_pin;
    gpio_t *const csen_pin;
    uint16_t *const current;

    const int cs_common;
    const uint32_t max_current;
    const uint32_t max_current_time;
    const uint32_t ovc_lock_time;

    int enable;
    vn7004_stat_t status;
    vn7004_stat_t state;  
    uint32_t counter;
    uint32_t current_ma;    
}vn7004_t;