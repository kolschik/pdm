#pragma once

#include "stdint.h"
#include "gpio.h"
#define STAGE_COUNT 3

typedef enum {
    vn7004_status_off,
    vn7004_status_on,
    vn7004_status_ocp,
    vn7004_status_short_gnd,
    vn7004_status_short_vcc
}vn7004_stat_t;

typedef struct {
    const gpio_t *en_pin;
    const gpio_t *csen_pin;
    uint16_t *const current;

    const uint32_t max_current;
    const uint32_t max_current_time;
    const uint32_t ovc_lock_time;

    int enable;
    vn7004_stat_t status;
    vn7004_stat_t state;  
    uint32_t counter;
    uint32_t current_ma;    
}vn7004_ic_t;

typedef struct {
    vn7004_ic_t *const ic;
    const uint32_t ic_count;
    uint32_t counter;
}vn7004_t;

void vn7004_poll(vn7004_t *vn_p);