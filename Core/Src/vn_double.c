#include "vn_double.h"
#include "errno.h"
#if defined (FREERTOS)
#include "cmsis_os.h"
#endif

static const uint32_t vn7004_short_current = 4096*5000/(3300*2);


void vn_double_poll(vn_double_t *ic){
#if defined (FREERTOS)
    uint32_t tick = xTaskGetTickCount();
#else
    uint32_t tick = HAL_GetTick();
#endif

    if (ic->enable == 0){
        ic->state = vn_double_state_off;
        ic->current_ma = 0;
    }

    if ((ic->state != vn_double_state_off)){
        if (ic->enable > 0){
            gpio_set(ic->sen_p_pin, 1);
        } else {
            gpio_set(ic->sen_n_pin, 1);
        }
    }

    switch (ic->state){
    case vn_double_state_off:
        gpio_set(ic->sen_p_pin, 0);
        gpio_set(ic->sen_n_pin, 0);
        gpio_set(ic->en_p_pin, 0);
        gpio_set(ic->en_n_pin, 0);                                    
        if (ic->enable != 0){            
            ic->counter = tick;
            ic->state = vn_double_state_check;
            break;
        }

        ic->status = vn_double_state_off;            
        break;

    case vn_double_state_check:
        ic->counter = tick;
        if (ic->enable > 0){
            gpio_set(ic->en_p_pin, 1);
        } else {
            gpio_set(ic->en_n_pin, 1);
        }
        ic->state = vn_double_state_on;
        break;

    case vn_double_state_on:
        ic->current_ma = *ic->current * ic->current_scale / 1000;          

        if (ic->current_ma < ic->max_current) {
            ic->counter = tick;
        }
        if (*ic->current > vn7004_short_current) {
            ic->counter = tick;
            gpio_set(ic->en_n_pin, 0);
            gpio_set(ic->en_p_pin, 0);            
            ic->state = vn_double_state_short_gnd;
        }
        if ((tick - ic->counter) > ic->max_current_time) {
            ic->counter = tick;
            gpio_set(ic->en_n_pin, 0);
            gpio_set(ic->en_p_pin, 0);    
            ic->state = vn_double_state_ocp;
        }
    break;

    case vn_double_state_short_gnd:
    case vn_double_state_ocp:       
        if (ic->ovc_lock_time == 0){
            break;
        }
        if ((ic->counter - tick) > ic->ovc_lock_time){
            ic->counter = tick;
            if (ic->enable > 0){
                gpio_set(ic->en_p_pin, 1);
            } else {
                gpio_set(ic->en_n_pin, 1);
            }
            ic->state = vn_double_state_on;
        }
        break;
    }
}

void vn_double_ctl(vn_double_t *vn_ic, int en){
    if ((vn_ic->enable == 0) || (en == 0)){
        vn_ic->enable = en;
    }
}

int vn_double_get_cur (vn_double_t *vn_ic){
    return vn_ic->current_ma;
}