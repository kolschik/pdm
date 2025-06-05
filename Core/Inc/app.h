#pragma once

#include "bsp.h"
#include "can.h"
#include "cmsis_os.h"

typedef struct
{
    uint32_t time;
    int water_level;
    int _auto;
    int enable;
    int water_state;
    uint32_t enable_delay;
    uint32_t disable_delay;

} pump_t;


void nmea_sender();
int app_init();
void adc1_cb();
void adc2_cb();

void can_rx_cb (can_fifo_t *fifo);
void can_tx_cb(uint8_t *tx_slot);
