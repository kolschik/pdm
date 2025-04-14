#include "gpio.h"
#include "stdint.h"

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
    vn7004_stat_t status;
    vn7004_stat_t state;    
}vn7004_t;

static vn7004_t *vn_7004;
static vn7004_count;

void vn7004_poll(){
    static uint32_t iter = 0;
    for(uint32_t i=0; i<vn7004_count; i++){
        vn7004_t *vn = &vn_7004[i];
        if (vn->enable == 0){
            vn->state = vn7004_state_off;
        }
        switch (vn->state){
        case vn7004_state_off:
            gpio_set(vn->en_pin, 0);
            if (vn->enable){
                if ((vn->cs_common == -1) || ((iter & 0x3) == (i & 0x1))){
                    vn->state = vn7004_state_check;
                }

                break;
            }
  
            if (vn->cs_common != -1){
                gpio_set(vn->csen_pin, 0);
            }
            vn->status = vn7004_state_off;            
            break;
        case vn7004_state_check:

            break;

        case vn7004_state_on:
            break;

        case vn7004_state_ocp:
        break;

        case vn7004_state_short_gnd:
        break;

        case vn7004_state_short_vcc:
        break;


        }
        
        vn_7004[i].enable
    }

}