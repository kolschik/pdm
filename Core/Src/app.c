#include "app.h"
#include "adc.h"
#include "gpio.h"
#include "vn7004.h"
#include "vn_double.h"
#include "nmea2k.h"
#include "string.h"
#include "stm32f1xx_ll_rtc.h"
#include "led.h"

osThreadId CtlPDMHandle;
osThreadId CANTaskHandle;

static uint32_t CANTaskBuffer[ 256 ];
static osStaticThreadDef_t CANTaskControlBlock;

static uint32_t CtlPDMBuffer[ 256 ];
static osStaticThreadDef_t CtlPDMControlBlock;

osMessageQId RxQueueHandle;
uint8_t RxQueueBuffer[ 8 * sizeof( can_fifo_t ) ];
osStaticMessageQDef_t RxQueueControlBlock;

static void StartCtlPDM(void const * argument);
static void nmea_sender(void const * argument);
void SystemClock_Config(void);
void sleep();

extern adc_t adc1;
extern adc_t adc2;



static pump_t pump = {
    ._auto = 1,
    .enable_delay = 1000,
    .disable_delay = 3000
};

static int pump_algo(int water_level);



int app_init(){
    static pdm_t pdm = {0};
    for (uint8_t i = 0; i<sizeof(pdm.sw)/sizeof(pdm.sw[0]); i++){
        pdm.sw[i].status = -1;
        pdm.sw[i].update = -1;
    }

    osThreadStaticDef(CtlPDM, StartCtlPDM, osPriorityHigh, 0, sizeof(CtlPDMBuffer)/4, CtlPDMBuffer, &CtlPDMControlBlock);
    CtlPDMHandle = osThreadCreate(osThread(CtlPDM), &pdm);


    osThreadStaticDef(CANTask, nmea_sender, osPriorityNormal, 0, 256, CANTaskBuffer, &CANTaskControlBlock);
    CANTaskHandle = osThreadCreate(osThread(CANTask), &pdm);

    osMessageQStaticDef(RxQueue, 8, can_fifo_t, RxQueueBuffer, &RxQueueControlBlock);
    RxQueueHandle = osMessageCreate(osMessageQ(RxQueue), NULL);
    return 0;
}


void StartCtlPDM(void const * argument) {
    pdm_t *pdm = (pdm_t *)argument;
    int permit_sleep = 0;  
    uint32_t volt_bat = 0;
    int  over_voltage = 0;
    int horn_last = -1;
    uint32_t horn_start = 0;
    uint32_t acc_off_time = 0;
    int led_active = 0;
    int acc = 0;
    int acc_last = 0;
    start_adc();
    int trim_ctl_last = 0;
    uint32_t trim_update = 0;
    uint32_t comm_tick_update = 0;
    static uint32_t tm_buf[8] = {0};
    static int tm_idx = 0;
    (void)comm_tick_update;
    for(;;) {
        if (ulTaskNotifyTake( pdTRUE, 100) == 0){
            // todo register error

            continue;
        }
        extern vn7004_t vn1, vn2, vn4, vn7;
        extern vn_double_t vn3;
   
        vn7004_poll(&vn1);
        vn7004_poll(&vn2);        
        vn7004_poll(&vn4);
        vn7004_poll(&vn7);
        vn_double_poll(&vn3);
        led_poll();

        uint16_t val;
        if (adc_get_ch(&adc2, 0, &val) == 0){
            volt_bat = adc_convert(val, 3300, 20, 3, 0);
            if (volt_bat > 10000) {
                permit_sleep = 0;                
                acc = 1;
            }
            if (volt_bat < 5000) {
                acc = 0;
            }
            over_voltage = 0;
            if (volt_bat > 16500){
                acc = 0;
                over_voltage = 1;
            }
            pdm->batt_volt = volt_bat;
        }

        pdm->acc = acc;

        uint32_t tick = xTaskGetTickCount();
        if (acc ^ acc_last) {
            if (acc == 0){
                acc_off_time = tick;
            }
        }
        acc_last = acc;


        uint16_t temper;
        if (adc_get_ch(&adc1, 0, &temper) == 0){

        }

        uint32_t tm_adc = (ADC2->JDR2 * 3300 + 2048) / 4096;
        tm_buf[tm_idx++ & (sizeof(tm_buf) / sizeof(tm_buf[0]) - 1)] = tm_adc;
        volatile uint32_t tm_volt = 0;
        for (uint32_t i = 0; i < sizeof(tm_buf) / sizeof(tm_buf[0]); i++){
            tm_volt += tm_buf[i];
        }
        tm_volt = (tm_volt + sizeof(tm_buf) / sizeof(tm_buf[0]) / 2) / sizeof(tm_buf) / sizeof(tm_buf[0]);
        // Секция Помпы        
        pdm->water_stat = read_pin() * acc;
         
        pump._auto = 1; // sw[2] - auto, sw[1] - man
        if ((pdm->sw[1].status != -1) && (pdm->sw[2].status != -1) && 
            (tick - pdm->sw[1].update < 5000) && (tick - pdm->sw[2].update < 5000)){
                pump.enable = pdm->sw[1].status;
                pump._auto = pdm->sw[2].status;
        }
        int pump_status = pump_algo(pdm->water_stat) * acc;

        //секция горна
        int horn = (pdm->sw[3].status == 1) && (tick - pdm->sw[3].update < 500) ? acc : 0;
        if ((horn == 1) && (horn_last == 0)) {
            horn_start = tick;
        }
        horn_last = horn;
        if ((tick - horn_start) > 10000) {
            horn = 0;
        }


        (void) over_voltage;

        // секция трима
        int trim_ctl = 0;
        if ((pdm->trim_sw[0].status == 1) && (pdm->trim_sw[1].status != -1) && 
            (tick - pdm->trim_sw[0].update < 500) && (tick - pdm->trim_sw[1].update < 500)){
            trim_ctl = 1;
        }

        if ((pdm->trim_sw[0].status != -1) && (pdm->trim_sw[1].status == 1) && 
            (tick - pdm->trim_sw[0].update < 500) && (tick - pdm->trim_sw[1].update < 500)){
            trim_ctl = -1;
        }

        if (trim_ctl != trim_ctl_last){
            trim_update = tick;
        }
        trim_ctl_last = trim_ctl;
        if ((tick - trim_update) > 20000){
            trim_ctl = 0;
        }
        trim_ctl *= acc;

        // секция света
        int light = (pdm->sw[0].status == 1) && (tick - pdm->sw[0].update < 500) ? acc : 0;



        vn7004_ctl(&vn1.ic[0], pump_status);
        vn7004_ctl(&vn2.ic[0], acc);
        vn_double_ctl(&vn3, trim_ctl);
        vn7004_ctl(&vn4.ic[0], acc);
        vn7004_ctl(&vn4.ic[1], light);
        vn7004_ctl(&vn7.ic[0], horn);

        pdm->out[0].current = vn7004_get_cur(&vn1.ic[0]);
        pdm->out[0].voltage = pump_status * pdm->batt_volt;
        pdm->out[0].status = (uint32_t)vn7004_get_status(&vn1.ic[0]);

        pdm->out[1].current = vn7004_get_cur(&vn2.ic[0]);
        pdm->out[1].voltage = acc * pdm->batt_volt;
        pdm->out[1].status = (uint32_t)vn7004_get_status(&vn2.ic[0]);

        pdm->out[2].current = vn7004_get_cur(&vn7.ic[0]);
        pdm->out[2].voltage = horn * pdm->batt_volt;
        pdm->out[2].status = (uint32_t)vn7004_get_status(&vn7.ic[0]);

        pdm->out[3].current = vn7004_get_cur(&vn4.ic[0]);
        pdm->out[3].voltage = acc * pdm->batt_volt;
        pdm->out[3].status = (uint32_t)vn7004_get_status(&vn4.ic[0]);

        pdm->out[4].current = vn7004_get_cur(&vn4.ic[1]);
        pdm->out[4].voltage = light * pdm->batt_volt;
        pdm->out[4].status = (uint32_t)vn7004_get_status(&vn4.ic[0]);

        pdm->out[5].current = vn_double_get_cur(&vn3);
        pdm->out[5].voltage = trim_ctl < 0 ? -1 : trim_ctl * pdm->batt_volt;
        pdm->out[5].status = (uint32_t)vn_double_get_status(&vn3);

        if (((tick - acc_off_time) > 5000) && (acc == 0)){
            permit_sleep = 1;
        }

        if ((tick > 1000) || led_active){
            led_active = 1;
            led_change_index(0, acc);

            // секция светодиода связи
            led_mode_t comm_led_mode = led_off;
            if (pdm->can_fault == ENODEV) {
                comm_led_mode = led_flash_l;
            } else if (pdm->can_fault == EFAULT) {
                comm_led_mode = led_blink;
            } else if ((tick - pdm->sw[0].update) < 500){
                comm_led_mode = led_on;
            }
            led_change_index(1, comm_led_mode);

            int ch_fault = 0;
            for (uint32_t i = 0; i<(sizeof(pdm->out) / sizeof(pdm->out[0])); i++) {
                if (pdm->out[0].status == (uint32_t)vn7004_state_ocp 
                                    || (pdm->out[0].status == (uint32_t)vn7004_state_short_gnd)) {
                    ch_fault = EFAULT;                    
                }
            }

            led_mode_t err_led_mode = led_off;
            if (pdm->otp) {
                err_led_mode = led_on;    
            } else if (ch_fault == EFAULT) {
                comm_led_mode = led_flash_l;
            }
            led_change_index(2, err_led_mode);
        } else {
            led_change_index(0, led_on);
            led_change_index(1, led_on);
            led_change_index(2, led_on);            
        }



        if (permit_sleep) {
            sleep();
        }

    }
}

void adc1_cb(){
    BaseType_t tpw;
    vTaskNotifyGiveFromISR(CtlPDMHandle, &tpw);
}

void adc2_cb(){

    //pdm->batt_volt = volt_bat;
}




int pump_algo(int water_level){

    if (pump._auto == 0){
        pump.water_state = pump.enable;
        return pump.enable;
    }

    uint32_t time_now = xTaskGetTickCount();
    
    int water_edge = water_level ^ pump.water_level;
    pump.water_level = water_level;

    if (water_edge) {
        pump.time = time_now;
    }

    if ((time_now - pump.time) >= (water_level ? pump.enable_delay : pump.disable_delay)){
        pump.water_state = water_level;
    }

    return pump.water_state;
}


static void nmea_sender(void const * argument){ 
    can_ctl(1);
    static uint32_t send_stat = 0;
    static uint32_t fail_stat = 0;
    uint32_t can_reg_fail_tick = 0;
    uint32_t can_reg_fail = 0;
    pdm_t * pdm = (pdm_t *)argument;
    while(1){
        can_fifo_t rx_fifo;
        if (xQueueReceive(RxQueueHandle, &rx_fifo, 5) == pdTRUE) {
            tN2kMsg_t rx_msg;
            uint32_t tick = xTaskGetTickCount();
            CanIdToN2k(rx_fifo.id, &rx_msg);
            if ((rx_msg.PGN == 127502L) && (rx_msg.Source == (0xff & ('k'+ 'e' + 'y' + 'p'+ 'a'+ 'd')))){
                memcpy (rx_msg.Data, rx_fifo.data8, 8); 
                tN2kOnOff sw[28];
                uint8_t bank;
                ParseN2kPGN127502(&rx_msg, sw, &bank);
                if (bank == KEYPAD_BANK){
                    for (uint32_t i=0; i<(sizeof(pdm->sw) / sizeof(pdm->sw[0])); i++){
                        pdm->sw[i].status = sw[i] < N2kOnOff_Error ? sw[i] : -1;
                        pdm->sw[i].update = tick;                    
                    }
                }

            }
            if ((rx_msg.PGN == 127501L) && (rx_msg.Source == 25)){
                memcpy (rx_msg.Data, rx_fifo.data8, 8); 
                tN2kOnOff sw[28];
                uint8_t bank;
                ParseN2kPGN127501(&rx_msg, sw, &bank);
                if (bank == TRIM_BANK){
                    for (uint32_t i=0; i<(sizeof(pdm->trim_sw) / sizeof(pdm->trim_sw[0])); i++){
                        pdm->trim_sw[i].status = sw[i] < N2kOnOff_Error ? sw[i] : -1;
                        pdm->trim_sw[i].update = tick;                    
                    }
                }
            }            
        }
        if (pdm->acc == 0){
            continue;
        }
        static uint32_t handler_cnt = 0;
        extern nmea_send_handler send_handler[3];
        if (handler_cnt >= sizeof(send_handler)/sizeof(send_handler[0])){
            handler_cnt = 0;
        }
        tN2kMsg_t msg; 
        uint32_t now = xTaskGetTickCount();
        int can_error = -1;
        if (send_handler[handler_cnt++](&msg, pdm)){
            can_fifo_t tx_fifo;
            packN2k(&msg, &tx_fifo);

            if (can_tx(tx_fifo, 10) == 0) {
                can_reg_fail = 0;
                send_stat++;
            } else {
                if (can_reg_fail == 0) {
                    can_reg_fail_tick = now;
                    can_reg_fail = 1;
                }

                fail_stat++;
                can_error = can_get_error();
            }
        }
        if (can_reg_fail && (now - can_reg_fail_tick > 500)){

            pdm->can_fault = can_error < 0 ? pdm->can_fault : can_error == 3 ? ENODEV : EFAULT;
        } else if (can_reg_fail == 0) {
            pdm->can_fault = 0;
        }
    }
}


void can_rx_cb (can_fifo_t *fifo){
    xQueueSendFromISR(RxQueueHandle, fifo, 0);
}

void can_tx_cb(int8_t *tx_slot){
    (void)tx_slot;
    BaseType_t not = 0;
    vTaskNotifyGiveFromISR(CANTaskHandle, &not);
}

void vApplicationIdleHook( void ){
  CLEAR_BIT(SCB->SCR, ((uint32_t)SCB_SCR_SLEEPDEEP_Msk));
    __WFI();
}

void sleep(){

    can_ctl(0);
    LL_ADC_Disable(adc1.a);
    LL_ADC_Disable(adc2.a);
    SysTick->CTRL  = 0;

    uint32_t tickstart = HAL_GetTick();
    if ((LL_RTC_IsActiveFlag_RTOF(RTC) == 0) && ((HAL_GetTick() - tickstart) > 5)){
        return;
    }

    LL_RTC_DisableWriteProtection(RTC);
    
    uint32_t time = LL_RTC_TIME_Get(RTC);

    uint16_t alarm_temp_h, alarm_temp_l;
    
    alarm_temp_h = RTC->CNTH >> 16;
    alarm_temp_l = time & 0xffff;
    RTC->ALRL = alarm_temp_l + 3;
    RTC->ALRH = alarm_temp_h;
    if ((alarm_temp_h != RTC->CNTH) | (alarm_temp_l > (0xFFFF - 3))) RTC->ALRH = alarm_temp_h + 1;
    RTC->CRL &= ~RTC_CRL_CNF;
    
    while ((RTC_CRL_RTOFF & RTC->CRL) == 0);  

    HAL_PWR_EnterSTOPMode(PWR_MAINREGULATOR_ON, PWR_STOPENTRY_WFI);
    SystemClock_Config();


    LL_ADC_Enable(adc1.a);
    LL_ADC_Enable(adc2.a);
    can_ctl(1);
}

