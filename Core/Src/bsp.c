#include "bsp.h"
#include "app.h"
#include "can.h"
#include "main.h"
#include "vn7004.h"
#include "nmea.h"


const can_baud_t baud_250[] = {
    {
        .com = {
          .prescaler = 6 - 1,
          .ts1 = 13-1,
          .ts2 = 2-1,
          .jump = 0
        }
    }
};
const can_filt_t can_filters[] = {
    {
        .id1 = 0,
        .id2 = 0,
        .type = 0,
        .action = 0
    }
};



const can_cfg_t can_cfg = {
    .baudrate = baud_250,
    .filter = can_filters,
    .baud_num = sizeof(baud_250) / sizeof (baud_250[0]),
    .filt_num = sizeof(can_filters) / sizeof (can_filters[0]),
    .silent = 0
};

can_t can1 = {
    .can = CAN1,
    .cfg = &can_cfg,
    .rx_cmpl = can_rx_cb,
    .tx_cmpl = can_tx_cb
};

const gpio_t gpio_a[] = {
    {
        .port = GPIOA,
        .pin = LL_GPIO_PIN_1,
        .cfg = {
            .mode = LL_GPIO_MODE_OUTPUT,
            .open_drain = LL_GPIO_OUTPUT_PUSHPULL,
            .freq = LL_GPIO_SPEED_FREQ_HIGH, 
            .pull = LL_GPIO_PULL_DOWN,
            .init_val = 0
        }
    },    
    {
        .port = GPIOA,
        .pin = LL_GPIO_PIN_2,
        .cfg = {
            .mode = LL_GPIO_MODE_OUTPUT,
            .open_drain = LL_GPIO_OUTPUT_PUSHPULL,
            .freq = LL_GPIO_SPEED_FREQ_HIGH, 
            .pull = LL_GPIO_PULL_DOWN,
            .init_val = 0
        }
    },
    {
        .port = GPIOA,
        .pin = LL_GPIO_PIN_3,
        .cfg = {
            .mode = LL_GPIO_MODE_OUTPUT,
            .open_drain = LL_GPIO_OUTPUT_PUSHPULL,
            .freq = LL_GPIO_SPEED_FREQ_HIGH, 
            .pull = LL_GPIO_PULL_DOWN,
            .init_val = 0            
        }
    },
    {
        .port = GPIOA,
        .pin = LL_GPIO_PIN_4,
        .cfg = {
            .mode = LL_GPIO_MODE_OUTPUT,
            .open_drain = LL_GPIO_OUTPUT_PUSHPULL,
            .freq = LL_GPIO_SPEED_FREQ_HIGH, 
            .pull = LL_GPIO_PULL_DOWN,
            .init_val = 0
        }
    },
    {
        .port = OUT56_SEN_GPIO_Port,
        .pin = OUT56_SEN_Pin,
        .cfg = {
            .mode = LL_GPIO_MODE_OUTPUT,
            .open_drain = LL_GPIO_OUTPUT_PUSHPULL,
            .freq = LL_GPIO_SPEED_FREQ_HIGH, 
            .pull = LL_GPIO_PULL_DOWN,
            .init_val = 0
        }
    },

    {
        .port = OUT5_EN_GPIO_Port,
        .pin = OUT5_EN_Pin,
        .cfg = {
            .mode = LL_GPIO_MODE_OUTPUT,
            .open_drain = LL_GPIO_OUTPUT_PUSHPULL,
            .freq = LL_GPIO_SPEED_FREQ_HIGH, 
            .pull = LL_GPIO_PULL_DOWN,
            .init_val = 0
        }
    },




    {
        .port = GPIOA,
        .pin = LL_GPIO_PIN_10,
        .cfg = {
            .mode = LL_GPIO_MODE_OUTPUT,
            .open_drain = LL_GPIO_OUTPUT_PUSHPULL,
            .freq = LL_GPIO_SPEED_FREQ_HIGH, 
            .pull = LL_GPIO_PULL_DOWN,
            .init_val = 0
        }
    },

    {
        .port = GPIOA,
        .pin = LL_GPIO_PIN_11,
        .cfg = {
            .mode = LL_GPIO_MODE_FLOATING,
            .freq = LL_GPIO_SPEED_FREQ_HIGH, 
        }
    },

    {
        .port = GPIOA,
        .pin = LL_GPIO_PIN_12,
        .cfg = {
            .mode = LL_GPIO_MODE_ALTERNATE,
            .open_drain = LL_GPIO_OUTPUT_PUSHPULL,
            .freq = LL_GPIO_SPEED_FREQ_HIGH, 
        }
    },
};


const gpio_t gpio_b[] = {
    {
        .port = GPIOB,
        .pin = LL_GPIO_PIN_3,
        .cfg = {
            .mode = LL_GPIO_MODE_OUTPUT,
            .open_drain = LL_GPIO_OUTPUT_PUSHPULL,
            .freq = LL_GPIO_SPEED_FREQ_HIGH, 
            .pull = LL_GPIO_PULL_DOWN,
            .init_val = 0
        }
    },  
    {
        .port = GPIOB,
        .pin = LL_GPIO_PIN_4,
        .cfg = {
            .mode = LL_GPIO_MODE_OUTPUT,
            .open_drain = LL_GPIO_OUTPUT_PUSHPULL,
            .freq = LL_GPIO_SPEED_FREQ_HIGH, 
            .pull = LL_GPIO_PULL_DOWN,
            .init_val = 0
        }
    },  
    {
        .port = GPIOB,
        .pin = LL_GPIO_PIN_5,
        .cfg = {
            .mode = LL_GPIO_MODE_OUTPUT,
            .open_drain = LL_GPIO_OUTPUT_PUSHPULL,
            .freq = LL_GPIO_SPEED_FREQ_HIGH, 
            .pull = LL_GPIO_PULL_DOWN,
            .init_val = 0
        }
    },  

    {
        .port = GPIOB,
        .pin = LL_GPIO_PIN_12,
        .cfg = {
            .mode = LL_GPIO_MODE_INPUT,
            .open_drain = LL_GPIO_OUTPUT_PUSHPULL,
            .freq = LL_GPIO_SPEED_FREQ_HIGH, 
            .pull = LL_GPIO_PULL_UP,
        }
    },
    {
        .port = GPIOB,
        .pin = OUT6_H_Pin,
        .cfg = {
            .mode = LL_GPIO_MODE_OUTPUT,
            .open_drain = LL_GPIO_OUTPUT_PUSHPULL,
            .freq = LL_GPIO_SPEED_FREQ_HIGH, 
            .init_val = 0
        }
    },
    {
        .port = OUT56_SEL_GPIO_Port,
        .pin = OUT56_SEL_Pin,
        .cfg = {
            .mode = LL_GPIO_MODE_OUTPUT,
            .open_drain = LL_GPIO_OUTPUT_PUSHPULL,
            .freq = LL_GPIO_SPEED_FREQ_HIGH, 
            .init_val = 0
        }
    },
};



const gpio_t gpio_c[] = {
    {
        .port = GPIOC,
        .pin = LL_GPIO_PIN_13,
        .cfg = {
            .mode = LL_GPIO_MODE_OUTPUT,
            .open_drain = LL_GPIO_OUTPUT_PUSHPULL,
            .freq = LL_GPIO_SPEED_FREQ_HIGH, 
            .init_val = 0
        }
    }
};


const uint32_t adc1_reg[] = {LL_ADC_CHANNEL_TEMPSENSOR};
const uint32_t adc2_reg[] = {LL_ADC_CHANNEL_8};
const uint32_t adc1_inj[] = {LL_ADC_CHANNEL_0, LL_ADC_CHANNEL_5, LL_ADC_CHANNEL_6, LL_ADC_CHANNEL_7};


const adc_t adc1 = {
    .reg_channels = adc1_reg,
    .reg_channels_num = sizeof(adc1_reg) / sizeof(adc1_reg[0]),
    .inj_channels = adc1_inj,
    .inj_channels_num = sizeof(adc1_inj) / sizeof(adc1_inj[0]),
    .inj_channels_trig = LL_ADC_INJ_TRIG_EXT_TIM1_TRGO,//LL_ADC_INJ_TRIG_EXT_TIM1_TRGO,LL_ADC_INJ_TRIG_SOFTWARE
    .cb = adc1_cb,
    .sampling_time = LL_ADC_SAMPLINGTIME_28CYCLES_5,
    .a = ADC1
};

const adc_t adc2 = {
    .reg_channels = adc2_reg,
    .reg_channels_num = sizeof(adc2_reg) / sizeof(adc2_reg[0]),
    .inj_channels_num = 0,
    .inj_channels_trig = 0,
    .cb = adc2_cb,
    .sampling_time = LL_ADC_SAMPLINGTIME_28CYCLES_5,
    .a = ADC2
};

tmr_cc_t tim1 = {
    .timer = TIM1
};

void cs1(int en){
    gpio_set(&gpio_a[0], en);
}
void cs2(int en){
    gpio_set(&gpio_a[3], en);
}
void cs3(int en){
    gpio_set(&gpio_b[1], en);
}
void cs4(int en){
    gpio_set(&gpio_a[6], en);
}

int cs5_stat = 0;
int cs6_stat = 0;
void cs5(int en){
    cs5_stat = en;
    gpio_set(&gpio_a[4], cs5_stat | cs6_stat);
    if (en == 1){
        gpio_set(&gpio_b[5], en);
    }
}
void cs6(int en){
    cs6_stat = en;
    gpio_set(&gpio_a[4], cs5_stat | cs6_stat);
    if (en == 1){
        gpio_set(&gpio_b[5], en ^ 0x01);
    }
}

uint16_t current_key[4];

vn7004_ic_t ic_group1 = {
    .en_pin = &gpio_a[1],
    .csen = cs1,
    .current = &ADC1->JDR1,
    .max_current = 15000,
    .max_current_time = 1500,
    .ovc_lock_time = 0,

};
vn7004_ic_t ic_group2 = {
    .en_pin = &gpio_a[2],
    .csen = cs2,
    .current = &ADC1->JDR2,
    .max_current = 15000,
    .max_current_time = 1500,
    .ovc_lock_time = 0,

};
vn7004_ic_t ic_group3[] = {
    {
        .en_pin = &gpio_b[2],
        .csen = cs3,
        .current = &ADC1->JDR3,
        .max_current = 20000,
        .max_current_time = 1500,
        .ovc_lock_time = 0,

    },
    {
        .en_pin = &gpio_b[0],
        .csen = cs4,
        .current = &ADC1->JDR3,
        .max_current = 20000,
        .max_current_time = 1500,
        .ovc_lock_time = 0,

    }  
};
vn7004_ic_t ic_group4[] = {
    {
        .en_pin = &gpio_a[5],
        .csen = cs5,
        .current = &ADC1->JDR4,
        .max_current = 2500,
        .max_current_time = 1000,
        .ovc_lock_time = 0,

    },
    {
        .en_pin = &gpio_b[4],
        .csen = cs6,
        .current = &ADC1->JDR4,
        .max_current = 2500,
        .max_current_time = 1000,
        .ovc_lock_time = 0,
    }  
};

vn7004_t vn1 = {
    .ic = &ic_group1,
    .ic_count = 1,
    .current_scale = (3300*1000/4096*2*16720/1780)
};
vn7004_t vn2 = {
    .ic = &ic_group2,
    .ic_count = 1,
    .current_scale = (3300*1000/4096*2*16720/1780)
};
vn7004_t vn3 = {
    .ic = ic_group3,
    .ic_count = 2,
    .current_scale = (3300*1000/4096*2*16720/1780)
};
vn7004_t vn4 = {
    .ic = ic_group4,
    .ic_count = 2,
    .current_scale = (3300*1000/4096*2*500/1000)
};

int bsp_init(){
    int rv = 0;
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOC);
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC1);
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC2);
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM1);
    
    rv |= gpio_init(gpio_a, sizeof(gpio_a) / sizeof(gpio_t));    
    rv |= gpio_init(gpio_b, sizeof(gpio_b) / sizeof(gpio_t));
    rv |= gpio_init(gpio_c, sizeof(gpio_c) / sizeof(gpio_t));

    rv |= adc_init(&adc1);
    rv |= adc_init(&adc2);
    rv |= tim_init(&tim1);
    tim_set_freq(&tim1, 500);
    tim_enable(&tim1);

    can_init(&can1);
    can_start();

    nmea_init(0xff & ('p'+ 'd' + 'm'));
    //MX_CRC_Init();
    /* USER CODE BEGIN 2 */

   //vn7004_init(key, sizeof(key) / sizeof(key[0]));
    HAL_NVIC_SetPriority(CAN1_TX_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(CAN1_TX_IRQn);
    HAL_NVIC_SetPriority(CAN1_RX0_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
    HAL_NVIC_SetPriority(ADC1_IRQn, 4, 0);
    HAL_NVIC_EnableIRQ(ADC1_IRQn);
    return rv;
}
#define             USB_HP_CAN1_TX_IRQHandler
#define CAN1_RX0_IRQHandler           USB_LP_CAN1_RX0_IRQHandler
uint8_t read_pin(){
    uint8_t pin_status = 0;
    pin_status |= gpio_read(&gpio_b[3]) << 0;
    pin_status ^= 0x01;
    return pin_status;
}

void led(int stat){
    gpio_set(&gpio_c[0], stat);
}


void ctl_can_bus(int stat){
    (void) stat;
} 

void start_adc(){
    LL_ADC_INJ_StartConversionExtTrig(adc1.a, LL_ADC_INJ_TRIG_EXT_RISING);
}
/*
HAL_GPIO_WritePin(OUT1_SEN_GPIO_Port, OUT1_SEN_Pin, GPIO_PIN_SET);
HAL_GPIO_WritePin(OUT2_SEN_GPIO_Port, OUT2_SEN_Pin, GPIO_PIN_SET);
HAL_GPIO_WritePin(OUT1_H_GPIO_Port, OUT1_H_Pin, GPIO_PIN_SET);
HAL_GPIO_WritePin(OUT2_H_GPIO_Port, OUT2_H_Pin, GPIO_PIN_SET);
*/