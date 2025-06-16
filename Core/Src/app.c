#include "app.h"
#include "adc.h"
#include "gpio.h"
#include "vn7004.h"
#include "nmea.h"
#include "string.h"
#include "stm32f1xx_ll_rtc.h"

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

int acc = 0;
int acc_last = 0;

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
    int light = 0;
    uint32_t acc_off_time = 0;
    ///HAL_ADCEx_Calibration_Start(&hadc1);
    //HAL_ADCEx_Calibration_Start(&hadc2);
        uint8_t water_level;
        start_adc();
    //__HAL_TIM_ENABLE(&htim1);
    for(;;) {
        if (ulTaskNotifyTake( pdTRUE, 100) == 0){
            // todo register error

            continue;
        }
        extern vn7004_t vn1;
        extern vn7004_t vn2;
        extern vn7004_t vn3;
        extern vn7004_t vn4;        
        vn7004_poll(&vn1);
        vn7004_poll(&vn2);        
        vn7004_poll(&vn3);
        vn7004_poll(&vn4);

        uint16_t val;
        if (adc_get_ch(&adc2, 0, &val) == 0){
            volt_bat = adc_convert(val, 3300, 20, 3, 0);
            if (volt_bat > 10000) {
                permit_sleep = 0;                
                acc = 1;
            }
            if (volt_bat < 8000) {
                acc = 0;
            }
            over_voltage = 0;
            if (volt_bat > 16500){
                acc = 0;
                over_voltage = 1;
            }
            pdm->batt_volt = volt_bat;
        }


        uint32_t tick = xTaskGetTickCount();
        if (acc ^ acc_last) {
            if (acc == 0){
                light = 0;
                acc_off_time = tick;
            } else if ((tick - acc_off_time) < 1000){
                light = 1;
            }
        }
        acc_last = acc;

        uint16_t temper;
        if (adc_get_ch(&adc1, 0, &temper) == 0){

        }
        water_level = read_pin() * acc;

        led(acc);
        
        pump._auto = 1;
        if ((pdm->sw[0].status == 1) && (pdm->sw[1].status != -1) && 
            (tick - pdm->sw[0].update < 5000) && (tick - pdm->sw[1].update < 5000)){
                pump.enable = pdm->sw[1].status * acc;
                pump._auto = 0;
        }
        int pump_status = pump_algo(water_level) * acc;
        (void) over_voltage;

        vn7004_ctl(&vn1.ic[0], pump_status);
        vn7004_ctl(&vn2.ic[0], acc);
        vn7004_ctl(&vn3.ic[0], 0);
        vn7004_ctl(&vn3.ic[1], 0);
        vn7004_ctl(&vn4.ic[0], 1);
        vn7004_ctl(&vn4.ic[1], light * acc);

        pdm->pump_ch.current = vn7004_get_cur(&vn1.ic[0]);
        pdm->acc_ch.current = vn7004_get_cur(&vn1.ic[1]);

        pdm->can_ch.current = vn7004_get_cur(&vn4.ic[0]);
        pdm->light_ch.current = vn7004_get_cur(&vn4.ic[1]);

        if (((tick - acc_off_time) > 5000) && (acc == 0)){
            permit_sleep = 1;
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
    uint32_t send_stat = 0;    
    pdm_t * pdm = (pdm_t *)argument;
    while(1){
        can_fifo_t rx_fifo;
        if (xQueueReceive(RxQueueHandle, &rx_fifo, 5) == pdTRUE) {
            tN2kMsg_t rx_msg;
            uint32_t tick = xTaskGetTickCount();
            CanIdToN2k(rx_fifo.id, &rx_msg);
            if ((rx_msg.PGN == 127502L) && (rx_msg.Source == ('K' ^ 'E' ^ 'Y' ^ 'P' ^ 'A' ^ 'D'))){
                memcpy (rx_msg.Data, rx_fifo.data8, 8); 
                tN2kOnOff sw[28];
                uint8_t bank;
                ParseN2kPGN127502(&rx_msg, sw, &bank);
                for (uint32_t i=0; i<(sizeof(pdm->sw) / sizeof(pdm->sw[0])); i++){
                    pdm->sw[i].status = sw[i] < N2kOnOff_Error ? sw[i] : -1;
                    pdm->sw[i].update = tick;                    
                }
            }
        }
        
        static uint32_t handler_cnt = 0;
        extern nmea_send_handler send_handler[3];
        if (handler_cnt >= sizeof(send_handler)/sizeof(send_handler[0])){
            handler_cnt = 0;
        }
        tN2kMsg_t msg;        
        if (send_handler[handler_cnt++](&msg, pdm)){
            can_fifo_t tx_fifo;
            packN2k(&msg, &tx_fifo);
            if (can_tx(tx_fifo, 10)) {
                send_stat++;
            }            
        }
    }
}


void can_rx_cb (can_fifo_t *fifo){
    xQueueSendFromISR(RxQueueHandle, fifo, 0);
}

void can_tx_cb(uint8_t *tx_slot){
    (void)tx_slot;
    BaseType_t not = 0;
    vTaskNotifyGiveFromISR(CANTaskHandle, &not);
}

void vApplicationIdleHook( void ){
  CLEAR_BIT(SCB->SCR, ((uint32_t)SCB_SCR_SLEEPDEEP_Msk));
    __WFI();
}

void sleep(){
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
}

