#pragma once

#include "adc.h"
#include "gpio.h"
#include "tmr.h"

void start_adc();
uint8_t read_pin();
void led(int stat);

int bsp_init();