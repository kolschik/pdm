#include "gpio.h"
#include "stdint.h"
#include "errno.h"

static const vn7004_short_current = 3000;

typedef enum {
    vn7004_status_off,
    vn7004_status_on,
    vn7004_status_ocp,
    vn7004_status_short_gnd,
    vn7004_status_short_vcc
}vn7004_stat_t;

typedef enum {
    vn7004_state_off,
    vn7004_state_on,
    vn7004_state_on_meas,
    vn7004_state_check,
    vn7004_state_ocp,
    vn7004_state_short_gnd,
    vn7004_state_short_vcc
}vn7004_state_t;

typedef struct {
    gpio_t *const en_pin;
    gpio_t *const csen_pin;
    uint16_t *const current;

    const int cs_common;
    const uint32_t max_current;
    const uint32_t max_current_time;
    const uint32_t ovc_lock_time;

    int enable;
    int poll_index;
    vn7004_stat_t status;
    vn7004_stat_t state;  
    uint32_t counter;
    uint32_t current_ma;    
}vn7004_t;

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
    uint32_t tick = HAL_GetTickCount();
    static uint32_t poll_cnt = 0;
    poll_cnt++;

    for(uint32_t i=0; i<vn7004_count; i++){
        vn7004_t *vn = &vn_7004[i];
        int curr_valid = 0;
        if ((vn->cs_common == -1) || ((poll_cnt == 1) && (vn->poll_index == i))){
            vn_7004[vn->cs_common].poll_index = vn->cs_common;
            poll_cnt = 0;
            curr_valid = 1;
        }
        
        vn->current_ma = vn->current;

        if (vn->enable == 0){
            vn->state = vn7004_state_off;
        }
        
        switch (vn->state){
        case vn7004_state_off:
            gpio_set(vn->en_pin, 0);
            gpio_set(vn->csen_pin, 0);            
            if (vn->enable){
                if ((vn->cs_common == -1)){
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
            if ((vn->cs_common == -1)) {
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