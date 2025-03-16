#include "bsp.h"
#include "app.h"
void can_rx_cb (can_fifo_t *fifo);
void can_tx_cb(uint8_t *tx_stot);

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
    .baudrate = &baud_250,
    .filter = &can_filters,
    .baud_num = sizeof(baud_250) / sizeof (baud_250[0]),
    .filt_num = sizeof(can_filters) / sizeof (can_filters[0]),
    .silent = 0
};

can_t can1 = {
    .can = CAN1,
    .cfg = &can_cfg,
    .no_it = 0,
    .rx_cmpl = can_rx_cb,
    .tx_cmpl = can_tx_cb
};

const gpio_t gpio_b[] = {
    {
        .port = GPIOB,
        .pin = LL_GPIO_PIN_12,
        .cfg = {
            .mode = LL_GPIO_MODE_INPUT,
            .open_drain = LL_GPIO_OUTPUT_PUSHPULL,
            .freq = LL_GPIO_SPEED_FREQ_HIGH, 
            .pull = LL_GPIO_PULL_UP,
        }
    }
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



int bsp_init(){
    int rv = 0;
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA);
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOC);
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC1);
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC2);
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM1);
    
    rv |= gpio_init(gpio_b, sizeof(gpio_b) / sizeof(gpio_t));
    rv |= gpio_init(gpio_c, sizeof(gpio_c) / sizeof(gpio_t));

    rv |= adc_init(&adc1);
    rv |= adc_init(&adc2);
    rv |= tim_init(&tim1);
    tim_set_freq(&tim1, 500);
    tim_enable(&tim1);
    //MX_CRC_Init();
    /* USER CODE BEGIN 2 */
   // can_init(&can1);
   // can_start();

   // HAL_NVIC_SetPriority(CEC_CAN_IRQn, 4, 0);
  //  HAL_NVIC_EnableIRQ(CEC_CAN_IRQn);
    return rv;
}
volatile int rx,tx;
void can_rx_cb (can_fifo_t *fifo){
    rx++;
};

uint8_t read_pin(){
    uint8_t pin_status = 0;
    pin_status |= gpio_read(&gpio_b[0]) << 0;
    pin_status ^= 0x01;
    return pin_status;
}

void start_adc(){
    LL_ADC_INJ_StartConversionExtTrig(adc1.a, LL_ADC_INJ_TRIG_EXT_RISING);
}
