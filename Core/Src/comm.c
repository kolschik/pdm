#include "nmea2k.h"
#include "app.h"

int batt_handler();
int ch_handler();
int water_handler();


int batt_handler(tN2kMsg_t *msg, void *argument){
    static uint32_t tick = 0;
    static uint8_t sid = 0;
    const uint32_t period = 1500;
    pdm_t * pdm = (pdm_t *)argument;    
    if ((HAL_GetTick() - tick) < period){
        return 0;
    }
    tick = HAL_GetTick();
    SetN2kPGN127508(msg, 0, pdm->batt_volt, 0, 0, sid++);
    return 1;
}

int ch_handler(tN2kMsg_t *msg, void *argument){
    static uint32_t tick = 0;
    static uint8_t sid= 0;
    static uint8_t instance = 0;
    const uint32_t period = 100;
    pdm_t * pdm = (pdm_t *)argument;   
    if ((HAL_GetTick() - tick) < period){
        return 0;
    }

    tick = HAL_GetTick();
    SetN2kPGN127751(msg, instance, (pdm->out[instance].voltage + 50) / 100, (pdm->out[instance].current + 5) / 10, sid++);
    instance++;
    if (instance >= (sizeof(pdm->out) / sizeof(pdm->out[0]))){
        instance = 0;
    }
    return 1;    
}

int water_handler(tN2kMsg_t *msg, void *argument){
    static uint32_t tick = 0;
    const uint32_t period = 500;
    pdm_t * pdm = (pdm_t *)argument;      
    if ((HAL_GetTick() - tick) < period){
        return 0;
    }
    tick = HAL_GetTick();
    SetN2kPGN127505(msg, 0, N2kft_Water, pdm->water_stat, 1);
    return 1;      
}

nmea_send_handler send_handler[3] = {
    batt_handler, ch_handler, water_handler
};
