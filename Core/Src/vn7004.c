#include "vn7004.h"
#include "errno.h"
#if defined (FREERTOS)
#include "cmsis_os.h"
#endif

static const uint32_t vn7004_short_current = 3000;


typedef enum {
    vn7004_state_off,
    vn7004_state_on,
    vn7004_state_on_meas,
    vn7004_state_check,
    vn7004_state_ocp,
    vn7004_state_short_gnd,
    vn7004_state_short_vcc
}vn7004_state_t;


static vn7004_t *vn_7004;
static uint32_t vn7004_count;

int vn7004_init(vn7004_t *vn_7004_p, uint32_t cnt) {
    if (vn_7004_p == 0) {
        return EINVAL;
    }
    vn_7004 = vn_7004_p;
    vn7004_count = cnt;
    return 0;
}

void vn7004_poll(){
#if defined (FREERTOS)
    uint32_t tick = xTaskGetTickCount();
#else
    uint32_t tick = HAL_GetTickCount();
#endif

    static uint32_t poll_cnt = 0;
    poll_cnt++;

    for(uint32_t i=0; i<vn7004_count; i++){
        vn7004_t *vn = &vn_7004[i];
        int curr_valid = 1;

        int complementare_pair = i;
        if (vn->cs_common != -1) {
            complementare_pair = vn_7004[vn->cs_common].cs_common > vn->cs_common ? vn_7004[vn->cs_common].cs_common : vn->cs_common;            
            if (poll_cnt & 0x02) {
                complementare_pair = vn_7004[vn->cs_common].cs_common < vn->cs_common ? vn_7004[vn->cs_common].cs_common : vn->cs_common;
            }

            if ((i == complementare_pair) || ((poll_cnt & 0x01) == 0)) {
                curr_valid = 0;
            }
        }

        if (vn->enable == 0){
            vn->state = vn7004_state_off;
        }
        
        switch (vn->state){
        case vn7004_state_off:
            gpio_set(vn->en_pin, 0);
            gpio_set(vn->csen_pin, 0);            
            if (vn->enable){
                if ((i == complementare_pair) && (poll_cnt & 0x01) == 0) {
                    gpio_set(vn->csen_pin, 1);                    
                    vn->counter = tick;
                    vn->state = vn7004_state_check;
                }

                break;
            }

            vn->status = vn7004_state_off;            
            break;

        case vn7004_state_check:
            if ((tick - vn->counter) >= 10){
                // TODO check short to vcc
                vn->counter = tick;
                gpio_set(vn->en_pin, 1);
                vn->state = vn7004_state_on;
            }
            break;

        case vn7004_state_on:
            if (curr_valid) {
                vn->current_ma = vn->current;                
                if (vn->current_ma < vn->max_current) {
                    vn->counter = tick;
                }
                if (vn->current_ma > vn7004_short_current) {
                    vn->counter = tick;
                    gpio_set(vn->en_pin, 0);    
                    vn->state = vn7004_state_short_gnd;
                }
                if ((vn->counter - tick) > vn->max_current_time) {
                    vn->counter = tick;
                    gpio_set(vn->en_pin, 0);    
                    vn->state = vn7004_state_ocp;
                }
                break;
            }


        case vn7004_state_short_gnd:
        case vn7004_state_ocp:       
            if (vn->ovc_lock_time == 0){
                break;
            }
            if ((vn->counter - tick) > vn->ovc_lock_time){
                vn->counter = tick;
                gpio_set(vn->en_pin, 1);
                vn->state = vn7004_state_on;
            }
        break;

        case vn7004_state_short_vcc:
        break;


        }
    }
}