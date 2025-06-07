#include "app.h"
#include "adc.h"
#include "gpio.h"
#include "vn7004.h"
#include "nmea.h"

osThreadId CtlPDMHandle;
osThreadId CANTaskHandle;

static uint32_t CANTaskBuffer[ 256 ];
static osStaticThreadDef_t CANTaskControlBlock;

static uint32_t CtlPDMBuffer[ 256 ];
static osStaticThreadDef_t CtlPDMControlBlock;

static void StartCtlPDM(void const * argument);
static void nmea_sender(void const * argument);
void SystemClock_Config(void);

extern adc_t adc1;
extern adc_t adc2;

extern vn7004_t vn1;
extern vn7004_t vn2;
extern vn7004_t vn3;
extern vn7004_t vn4;

static pump_t pump = {
    ._auto = 1,
    .enable_delay = 1000,
    .disable_delay = 3000
};

static int pump_algo(int water_level);

int acc = 0;
int acc_last = 0;

int app_init(){
    pdm_t pdm;
    osThreadStaticDef(CtlPDM, StartCtlPDM, osPriorityHigh, 0, sizeof(CtlPDMBuffer)/4, CtlPDMBuffer, &CtlPDMControlBlock);
    CtlPDMHandle = osThreadCreate(osThread(CtlPDM), &pdm);


    osThreadStaticDef(CANTask, nmea_sender, osPriorityNormal, 0, 256, CANTaskBuffer, &CANTaskControlBlock);
    CANTaskHandle = osThreadCreate(osThread(CANTask), &pdm);
    return 0;
}


void StartCtlPDM(void const * argument) {
    pdm_t *pdm = (pdm_t *)argument;

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
        vn7004_poll(&vn1);
        vn7004_poll(&vn2);        
        vn7004_poll(&vn3);
        vn7004_poll(&vn4);

        uint16_t val;
        if (adc_get_ch(&adc2, 0, &val) == 0){
            volt_bat = adc_convert(val, 3300, 20, 3, 0);
            if (volt_bat > 10000) {
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

        int pump_status = pump_algo(water_level) * acc;

        (void) over_voltage;

        vn7004_ctl(&vn1.ic[0], pump_status);
        vn7004_ctl(&vn2.ic[0], acc);
        vn7004_ctl(&vn3.ic[0], 0);
        vn7004_ctl(&vn3.ic[1], 0);
        vn7004_ctl(&vn4.ic[0], acc);
        vn7004_ctl(&vn4.ic[1], light * acc);

        pdm->pump_ch.current = vn7004_get_cur(&vn1.ic[0]);
        pdm->acc_ch.current = vn7004_get_cur(&vn1.ic[1]);

        pdm->can_ch.current = vn7004_get_cur(&vn1.ic[0]);
        pdm->light_ch.current = vn7004_get_cur(&vn1.ic[1]);
        if (1 == 0){
              SysTick->CTRL  = 0;      
            HAL_PWR_EnterSTOPMode(PWR_MAINREGULATOR_ON, PWR_STOPENTRY_WFI);
            SystemClock_Config();
              SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
                   SysTick_CTRL_TICKINT_Msk   |
                   SysTick_CTRL_ENABLE_Msk;      
        }
    }
}

void adc1_cb(){
    BaseType_t tpw;
    vTaskNotifyGiveFromISR(CtlPDMHandle, &tpw);
}

void adc2_cb(){}




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
    uint8_t sid127508 = 0;
    uint8_t sid127751 = 0;  
    pdm_t * pdm = (pdm_t *)argument;
    while(1){
        tN2kMsg_t msg;
        can_fifo_t tx_fifo;

        uint32_t send_stat = 0;

        uint32_t  cur=0, battemp=0;
    
        for (uint32_t i=2; i<3; i++){
            switch (i){
            case 0:
                SetN2kPGN127508(&msg, 0, pdm->batt_volt, cur, battemp, sid127508++);
                break;
            case 1:
                SetN2kPGN127505(&msg, 0, N2kft_Water, 0, 1);
                break;
            case 2:
                SetN2kPGN127751(&msg, 0, pdm->batt_volt, pdm->acc_ch.current, sid127751++);
                break;        
            default:
                break;
            }

            if (packN2k(&msg, &tx_fifo)){
                continue;
            }

            if (can_tx(tx_fifo, 50)) {
                send_stat++;
            }
        }
        vTaskDelay(5);
    }
}


void can_rx_cb (can_fifo_t *fifo){
    (void)fifo;
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