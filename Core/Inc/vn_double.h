#pragma once

#include "stdint.h"
#include "gpio.h"

typedef enum {
    vn_double_state_off,
    vn_double_state_on,
    vn_double_state_check,
    vn_double_state_ocp,
    vn_double_state_short_gnd
}vn_double_stat_t;


typedef struct {
    const gpio_t *en_n_pin;
    const gpio_t *en_p_pin;    
    const gpio_t *sen_n_pin;
    const gpio_t *sen_p_pin;    
    volatile uint32_t *const current;

    const uint32_t max_current;
    const uint32_t max_current_time;
    const uint32_t ovc_lock_time;
    const uint32_t current_scale; /// =3300000*2*K/(R * 4096)


    int enable;
    vn_double_stat_t status;
    vn_double_stat_t state;  
    uint32_t counter;
    uint32_t current_ma;   
}vn_double_t;

void vn_double_poll(vn_double_t *vn_p);
void vn_double_ctl(vn_double_t *vn_ic, int en);
int vn_double_get_cur (vn_double_t *vn_ic);