#include "vn7004.h"
#include "errno.h"
#if defined (FREERTOS)
#include "cmsis_os.h"
#endif

static const uint32_t vn7004_short_current = 4096*5000/(3300*2);






int vn7004_init(vn7004_t *vn_7004_p) {
    if (vn_7004_p == 0) {
        return EINVAL;
    }
    return 0;
}

void vn7004_poll(vn7004_t *vn_p){
#if defined (FREERTOS)
    uint32_t tick = xTaskGetTickCount();
#else
    uint32_t tick = HAL_GetTick();
#endif
    const uint32_t stage = vn_p->counter % STAGE_COUNT;


    for(uint32_t i=0; i<vn_p->ic_count; i++){
        vn7004_ic_t *ic = &vn_p->ic[i];

        if (ic->enable == 0){
            ic->state = vn7004_state_off;
            ic->current_ma = 0;
        }

        uint32_t curr_valid = 0;
        if ((i == (vn_p->counter / STAGE_COUNT)) && (ic->state != vn7004_state_off)){
            if (stage == 0){
                ic->csen(1); 
            } else {
                curr_valid = 1;
            }
        }

        if ((vn_p->ic_count == 1) && (ic->state != vn7004_state_off)){
            ic->csen(1);
            curr_valid = 1;
        }

        switch (ic->state){
        case vn7004_state_off:
            gpio_set(ic->en_pin, 0);
            ic->csen(0);          
            if (ic->enable){
                if (stage == 0){               
                    ic->counter = tick;
                    ic->state = vn7004_state_check;
                }

                break;
            }

            ic->status = vn7004_state_off;            
            break;

        case vn7004_state_check:
            if (curr_valid){
                // TODO check short to vcc
                ic->counter = tick;
                gpio_set(ic->en_pin, 1);
                ic->state = vn7004_state_on;
            }
            break;

        case vn7004_state_on:
            if (curr_valid) {
                ic->current_ma = *ic->current * vn_p->current_scale / 1000;          

                if (ic->current_ma < ic->max_current) {
                    ic->counter = tick;
                }
                if (*ic->current > vn7004_short_current) {
                    ic->counter = tick;
                    gpio_set(ic->en_pin, 0);    
                    ic->state = vn7004_state_short_gnd;
                }
                if ((tick - ic->counter) > ic->max_current_time) {
                    ic->counter = tick;
                    gpio_set(ic->en_pin, 0);    
                    ic->state = vn7004_state_ocp;
                }
                break;
            }
            break;

        case vn7004_state_short_gnd:
        case vn7004_state_ocp:       
            if (ic->ovc_lock_time == 0){
                break;
            }
            if ((ic->counter - tick) > ic->ovc_lock_time){
                ic->counter = tick;
                gpio_set(ic->en_pin, 1);
                ic->state = vn7004_state_on;
            }
        break;

        case vn7004_state_short_vcc:
        break;


        }

        if ((stage == (STAGE_COUNT - 1)) && (vn_p->ic_count != 1) ){
            ic->csen(0);  
        }

    }



    if (++vn_p->counter >= (vn_p->ic_count * STAGE_COUNT)){
        vn_p->counter = 0;
    }

}