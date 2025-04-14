#pragma once

#include "bsp.h"
#include "cmsis_os.h"

typedef struct
{
    uint32_t time;
    int water_level;
    int override;
    int disable;
    int water_state;
    uint32_t enable_delay;
    uint32_t disable_delay;

} pump_t;



int app_init();
void adc1_cb();
void adc2_cb();