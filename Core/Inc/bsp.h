#pragma once

#include "adc.h"
#include "gpio.h"
#include "tmr.h"

void start_adc();
uint8_t read_pin();

int bsp_init();
void can_ctl(int en);