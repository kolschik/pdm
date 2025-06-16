#include "nmea.h"
#include "app.h"

int batt_handler();
int ch_handler();
int water_handler();


int batt_handler(tN2kMsg_t *msg, void *argument){
    static uint32_t tick = 0;
    static uint8_t sid127508 = 0;
    const uint32_t period = 1500;
    pdm_t * pdm = (pdm_t *)argument;    
    if ((HAL_GetTick() - tick) < period){
        return 0;
    }
    tick = HAL_GetTick();
    SetN2kPGN127508(&msg, 0, pdm->batt_volt, cur, battemp, sid127508++);
    return 1;
}

int ch_handler(tN2kMsg_t *msg, void *argument){
    static uint32_t tick = 0;
    static uint8_t sid127751 = 0;
    const uint32_t period = 100;
    pdm_t * pdm = (pdm_t *)argument;      
    SetN2kPGN127751(&msg, 0, pdm->batt_volt, pdm->acc_ch.current, sid127751++);
}

int water_handler(tN2kMsg_t *msg, void *argument){
    static uint32_t tick = 0;
    const uint32_t period = 500;
    pdm_t * pdm = (pdm_t *)argument;      
    if ((HAL_GetTick() - tick) < period){
        return 0;
    }    
    SetN2kPGN127505(&msg, 0, N2kft_Water, 0, 1);
}

nmea_send_handler send_handler;
