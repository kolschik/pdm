#include "bsp.h"
#include "app.h"
#include "can.h"
#if !defined PDM_BOOT
#include "app.h"
#endif

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
#if !defined PDM_BOOT    
    .rx_cmpl = can_rx_cb,
    .tx_cmpl = can_tx_cb
#endif
};


#if !defined PDM_BOOT
#include "main.h"
#include "vn7004.h"
#include "vn_double.h"
#include "nmea2k.h"
#include "stm32f1xx_ll_rtc.h"
#include "led.h"



#define TERM_P &gpio_a[0]
#define OUT1_EN_P &gpio_a[1]
#define OUT1_SEN_P &gpio_a[2]
#define OUT1_CS_P &gpio_a[3]
#define OUT7_CS_P &gpio_a[4]
#define OUT5_CS_P &gpio_a[5]
#define OUT3_CS_P &gpio_a[6]
#define OUT2_CS_P &gpio_a[7]
#define OUT5_SEN_P &gpio_a[8]
#define OUT5_EN_P &gpio_a[9]
#define OUT4_SEN_P &gpio_a[12]

#define LED_ERR_P &gpio_c[0]
#define LED_ACC_P &gpio_c[1]

const gpio_t gpio_a[] = {
    {
        .port = GPIOA,
        .pin = LL_GPIO_PIN_0,
        .cfg = {
            .mode = LL_GPIO_MODE_FLOATING,
            .freq = LL_GPIO_SPEED_FREQ_HIGH, 
        }
    },
    {
        .port = OUT1_H_GPIO_Port,
        .pin = OUT1_H_Pin,
        .cfg = {
            .mode = LL_GPIO_MODE_OUTPUT,
            .open_drain = LL_GPIO_OUTPUT_PUSHPULL,
            .freq = LL_GPIO_SPEED_FREQ_HIGH, 
            .pull = LL_GPIO_PULL_DOWN,
            .init_val = 0
        }
    },    
    {
        .port = OUT1_SEN_GPIO_Port,
        .pin = OUT1_SEN_Pin,
        .cfg = {
            .mode = LL_GPIO_MODE_OUTPUT,
            .open_drain = LL_GPIO_OUTPUT_PUSHPULL,
            .freq = LL_GPIO_SPEED_FREQ_HIGH, 
            .pull = LL_GPIO_PULL_DOWN,
            .init_val = 0
        }
    },
    {
        .port = OUT1_CS_GPIO_Port,
        .pin = OUT1_CS_Pin,
        .cfg = {
            .mode = LL_GPIO_MODE_FLOATING,
            .freq = LL_GPIO_SPEED_FREQ_HIGH, 
        }
    },
    {
        .port = GPIOA,
        .pin = OUT7_CS_Pin,
        .cfg = {
            .mode = LL_GPIO_MODE_FLOATING,
            .freq = LL_GPIO_SPEED_FREQ_HIGH, 
        }
    },
    {
        .port = GPIOA,
        .pin = OUT56_CS_Pin,
        .cfg = {
            .mode = LL_GPIO_MODE_FLOATING,
            .freq = LL_GPIO_SPEED_FREQ_HIGH, 
        }
    },
    {
        .port = GPIOA,
        .pin = OUT34_CS_Pin,
        .cfg = {
            .mode = LL_GPIO_MODE_FLOATING,
            .freq = LL_GPIO_SPEED_FREQ_HIGH, 
        }
    },
    {
        .port = GPIOA,
        .pin = OUT2_CS_Pin,
        .cfg = {
            .mode = LL_GPIO_MODE_FLOATING,
            .freq = LL_GPIO_SPEED_FREQ_HIGH, 
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

    // CAN RX
    {
        .port = GPIOA,
        .pin = LL_GPIO_PIN_11,
        .cfg = {
            .mode = LL_GPIO_MODE_FLOATING,
            .freq = LL_GPIO_SPEED_FREQ_HIGH, 
        }
    },
    // CAN_TX
    {
        .port = GPIOA,
        .pin = LL_GPIO_PIN_12,
        .cfg = {
            .mode = LL_GPIO_MODE_ALTERNATE,
            .open_drain = LL_GPIO_OUTPUT_PUSHPULL,
            .freq = LL_GPIO_SPEED_FREQ_HIGH, 
        }
    },
    {
        .port = OUT4_SEN_GPIO_Port,
        .pin = OUT4_SEN_Pin,
        .cfg = {
            .mode = LL_GPIO_MODE_OUTPUT,
            .open_drain = LL_GPIO_OUTPUT_PUSHPULL,
            .freq = LL_GPIO_SPEED_FREQ_HIGH, 
            .init_val = 0
        }
    },
};

#define ACC_P &gpio_b[0]
#define OUT2_EN_P &gpio_b[1]
#define OUT2_SEN_P &gpio_b[2]
#define OUT4_H_P &gpio_b[3]
#define OUT3_SEN_P &gpio_b[4]
#define OUT3_H_P &gpio_b[5]
#define OUT4_L_P &gpio_b[6]
#define OUT3_L_P &gpio_b[7]
#define CAN_EN_P &gpio_b[8]
#define LED_CON_P &gpio_b[9]
#define OUT7_EN_P &gpio_b[10]
#define OUT7_SEN_P &gpio_b[11]
#define WATER_KEY_P &gpio_b[12]
#define OUT6_EN_P &gpio_b[13]
#define OUT5_SEL_P &gpio_b[14]

const gpio_t gpio_b[] = {
    {
        .port = GPIOB,
        .pin = LL_GPIO_PIN_0,
        .cfg = {
            .mode = LL_GPIO_MODE_FLOATING,
            .freq = LL_GPIO_SPEED_FREQ_HIGH, 
        }
    },  
    {
        .port = GPIOB,
        .pin = OUT2_H_Pin,
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
        .pin = OUT2_SEN_Pin,
        .cfg = {
            .mode = LL_GPIO_MODE_OUTPUT,
            .open_drain = LL_GPIO_OUTPUT_PUSHPULL,
            .freq = LL_GPIO_SPEED_FREQ_HIGH, 
            .pull = LL_GPIO_PULL_DOWN,
            .init_val = 0
        }
    },  
    {
        .port = OUT4_H_GPIO_Port,
        .pin = OUT4_H_Pin,
        .cfg = {
            .mode = LL_GPIO_MODE_OUTPUT,
            .open_drain = LL_GPIO_OUTPUT_PUSHPULL,
            .freq = LL_GPIO_SPEED_FREQ_HIGH, 
            .pull = LL_GPIO_PULL_DOWN,
            .init_val = 0
        }
    },  
    {
        .port = OUT3_SEN_GPIO_Port,
        .pin = OUT3_SEN_Pin,
        .cfg = {
            .mode = LL_GPIO_MODE_OUTPUT,
            .open_drain = LL_GPIO_OUTPUT_PUSHPULL,
            .freq = LL_GPIO_SPEED_FREQ_HIGH, 
            .pull = LL_GPIO_PULL_DOWN,
            .init_val = 0
        }
    },  
    {
        .port = OUT3_H_GPIO_Port,
        .pin = OUT3_H_Pin,
        .cfg = {
            .mode = LL_GPIO_MODE_OUTPUT,
            .open_drain = LL_GPIO_OUTPUT_PUSHPULL,
            .freq = LL_GPIO_SPEED_FREQ_HIGH, 
            .pull = LL_GPIO_PULL_DOWN,
            .init_val = 0
        }
    },  
    {
        .port = OUT4_L_GPIO_Port,
        .pin = OUT4_L_Pin,
        .cfg = {
            .mode = LL_GPIO_MODE_OUTPUT,
            .open_drain = LL_GPIO_OUTPUT_PUSHPULL,
            .freq = LL_GPIO_SPEED_FREQ_HIGH, 
            .pull = LL_GPIO_PULL_DOWN,
            .init_val = 0
        }
    },  
    {
        .port = OUT3_L_GPIO_Port,
        .pin = OUT3_L_Pin,
        .cfg = {
            .mode = LL_GPIO_MODE_OUTPUT,
            .open_drain = LL_GPIO_OUTPUT_PUSHPULL,
            .freq = LL_GPIO_SPEED_FREQ_HIGH, 
            .pull = LL_GPIO_PULL_DOWN,
            .init_val = 0
        }
    }, 
    {
        .port = CAN_EN_GPIO_Port,
        .pin = CAN_EN_Pin,
        .cfg = {
            .mode = LL_GPIO_MODE_OUTPUT,
            .open_drain = LL_GPIO_OUTPUT_PUSHPULL,
            .freq = LL_GPIO_SPEED_FREQ_HIGH, 
            .pull = LL_GPIO_PULL_DOWN,
            .init_val = 0
        }
    }, 
    {
        .port = LED3_GPIO_Port,
        .pin = LED3_Pin,
        .cfg = {
            .mode = LL_GPIO_MODE_OUTPUT,
            .open_drain = LL_GPIO_OUTPUT_PUSHPULL,
            .freq = LL_GPIO_SPEED_FREQ_HIGH, 
            .pull = LL_GPIO_PULL_DOWN,
            .init_val = 0
        }
    }, 
    {
        .port = OUT7_H_GPIO_Port,
        .pin = OUT7_H_Pin,
        .cfg = {
            .mode = LL_GPIO_MODE_OUTPUT,
            .open_drain = LL_GPIO_OUTPUT_PUSHPULL,
            .freq = LL_GPIO_SPEED_FREQ_HIGH, 
            .pull = LL_GPIO_PULL_DOWN,
            .init_val = 0
        }
    },  
    {
        .port = OUT7_SEN_GPIO_Port,
        .pin = OUT7_SEN_Pin,
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
        .port = LED1_GPIO_Port,
        .pin = LED1_Pin,
        .cfg = {
            .mode = LL_GPIO_MODE_OUTPUT,
            .open_drain = LL_GPIO_OUTPUT_PUSHPULL,
            .freq = LL_GPIO_SPEED_FREQ_HIGH, 
            .init_val = 0
        }
    },
    {
        .port = LED2_GPIO_Port,
        .pin = LED2_Pin,
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
const uint32_t adc1_inj[] = {LL_ADC_CHANNEL_3, LL_ADC_CHANNEL_7, LL_ADC_CHANNEL_6, LL_ADC_CHANNEL_5};
const uint32_t adc2_inj[] = {LL_ADC_CHANNEL_4, LL_ADC_CHANNEL_0, LL_ADC_CHANNEL_8};

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
    .inj_channels = adc2_inj,
    .inj_channels_num = sizeof(adc2_inj) / sizeof(adc2_inj[0]),
    .inj_channels_trig = LL_ADC_INJ_TRIG_EXT_TIM1_TRGO,//LL_ADC_INJ_TRIG_EXT_TIM1_TRGO,LL_ADC_INJ_TRIG_SOFTWARE
    .cb = adc2_cb,
    .sampling_time = LL_ADC_SAMPLINGTIME_28CYCLES_5,
    .a = ADC2
};

tmr_cc_t tim1 = {
    .timer = TIM1
};

void cs1(int en){
    gpio_set(OUT1_SEN_P, en);
}
void cs2(int en){
    gpio_set(OUT2_SEN_P, en);
}
void cs7(int en){
    gpio_set(OUT7_SEN_P, en);
}

void can_ctl(int en) {
    gpio_set(CAN_EN_P, en ? 0 : 1);
}
int cs5_stat = 0;
int cs6_stat = 0;
void cs5(int en){
    cs5_stat = en;
    gpio_set(OUT5_SEN_P, cs5_stat | cs6_stat);
    if (en == 1){
        gpio_set(OUT5_SEL_P, 0);
    }
}
void cs6(int en){
    cs6_stat = en;
    gpio_set(OUT5_SEN_P, cs5_stat | cs6_stat);
    if (en == 1){
        gpio_set(OUT5_SEL_P, 1);
    }
}

uint16_t current_key[4];

vn7004_ic_t ic_group1 = {
    .en_pin = OUT1_EN_P,
    .csen = cs1,
    .current = &ADC1->JDR1,
    .max_current = 15000,
    .max_current_time = 1500,
    .ovc_lock_time = 0,

};
vn7004_ic_t ic_group2 = {
    .en_pin = OUT2_EN_P,
    .csen = cs2,
    .current = &ADC1->JDR2,
    .max_current = 15000,
    .max_current_time = 1500,
    .ovc_lock_time = 0,

};

vn_double_t vn3 = {
        .en_p2_pin = OUT3_H_P,
        .en_p_pin = OUT4_H_P,
        .sen_p2_pin = OUT3_SEN_P,
        .sen_p_pin = OUT4_SEN_P,

        .current = &ADC1->JDR3,
        .max_current = 30000,
        .max_current_time = 1500,
        .ovc_lock_time = 0,
        .current_scale = (3300*1000/4096*2*16920/1780)
};

vn7004_ic_t ic_group4[] = {
    {
        .en_pin = OUT5_EN_P,
        .csen = cs5,
        .current = &ADC1->JDR4,
        .max_current = 2500,
        .max_current_time = 1000,
        .ovc_lock_time = 0,

    },
    {
        .en_pin = OUT6_EN_P,
        .csen = cs6,
        .current = &ADC1->JDR4,
        .max_current = 2500,
        .max_current_time = 1000,
        .ovc_lock_time = 0,
    }  
};

vn7004_ic_t ic_group5 = {
    .en_pin = OUT7_EN_P,
    .csen = cs7,
    .current = &ADC2->JDR1,
    .max_current = 15000,
    .max_current_time = 1500,
    .ovc_lock_time = 0,
};

vn7004_t vn1 = {
    .ic = &ic_group1,
    .ic_count = 1,
    .current_scale = (3300*1000/4096*2*16920/1780)
};
vn7004_t vn2 = {
    .ic = &ic_group2,
    .ic_count = 1,
    .current_scale = (3300*1000/4096*2*16920/1780)
};

vn7004_t vn4 = {
    .ic = ic_group4,
    .ic_count = 2,
    .current_scale = (3300*1000/4096*2*500/1000)
};

vn7004_t vn7 = {
    .ic = &ic_group5,
    .ic_count = 1,
    .current_scale = (3300*1000/4096*2*16920/1780)
};

led_t led_acc = {
    .gpio = LED_ACC_P,
    .inv = 0
};
led_t led_conn  = {
    .gpio = LED_CON_P,
    .inv = 0    
};
led_t led_err  = {
    .gpio = LED_ERR_P,
    .inv = 0    
};
led_t *led_p[] = { &led_acc, &led_conn, &led_err};

leds_t led = {
    .led = led_p,
    .led_num = sizeof(led_p) / sizeof(led_p[0])
};

int bsp_init(){
    int rv = 0;
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOC);
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC1);
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC2);
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM1);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);


    rv |= gpio_init(gpio_a, sizeof(gpio_a) / sizeof(gpio_t));    
    rv |= gpio_init(gpio_b, sizeof(gpio_b) / sizeof(gpio_t));
    rv |= gpio_init(gpio_c, sizeof(gpio_c) / sizeof(gpio_t));

    rv |= adc_init(&adc1);
    rv |= adc_init(&adc2);
    rv |= tim_init(&tim1);
    tim_set_freq(&tim1, 500);
    tim_enable(&tim1);

    led_init(&led);
    can_init(&can1);
    can_start();

    LL_PWR_EnableBkUpAccess();
   // if (LL_RCC_IsEnabledRTC() == 0){
        LL_RCC_SetRTCClockSource(LL_RCC_RTC_CLKSOURCE_LSI);
        LL_RCC_EnableRTC();
        LL_RTC_InitTypeDef rtc_struct = {0};
        rtc_struct.AsynchPrescaler = (40000 / 10) - 1;
        rv |= LL_RTC_Init(RTC, &rtc_struct);
        LL_RTC_EnableIT_ALR(RTC);
   // }
    LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_17);
    LL_EXTI_EnableRisingTrig_0_31(LL_EXTI_LINE_17);

    nmea_init(0xff & ('p'+ 'd' + 'm'));
    //MX_CRC_Init();
    /* USER CODE BEGIN 2 */

   //vn7004_init(key, sizeof(key) / sizeof(key[0]));
    HAL_NVIC_SetPriority(RTC_Alarm_IRQn, 6, 0);
    HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);

    // NVIC_SetPriority(DMA1_Channel1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),5, 0));
    // NVIC_EnableIRQ(DMA1_Channel1_IRQn);

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
    pin_status |= gpio_read(WATER_KEY_P) << 0;
    pin_status ^= 0x01;
    return pin_status;
}



void ctl_can_bus(int stat){
    (void) stat;
} 

void start_adc(){
    LL_ADC_INJ_StartConversionExtTrig(adc1.a, LL_ADC_INJ_TRIG_EXT_RISING);
    LL_ADC_INJ_StartConversionExtTrig(adc2.a, LL_ADC_INJ_TRIG_EXT_RISING);
}
/*
HAL_GPIO_WritePin(OUT1_SEN_GPIO_Port, OUT1_SEN_Pin, GPIO_PIN_SET);
HAL_GPIO_WritePin(OUT2_SEN_GPIO_Port, OUT2_SEN_Pin, GPIO_PIN_SET);
HAL_GPIO_WritePin(OUT1_H_GPIO_Port, OUT1_H_Pin, GPIO_PIN_SET);
HAL_GPIO_WritePin(OUT2_H_GPIO_Port, OUT2_H_Pin, GPIO_PIN_SET);
*/
void rtc_handler(){
    if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_17)){
        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_17);
    }
}

#endif