#include "app.h"
#include "adc.h"
#include "gpio.h"
#include "vn7004.h"

osThreadId CtlPDMHandle;
static uint32_t CtlPDMBuffer[ 256 ];
static osStaticThreadDef_t CtlPDMControlBlock;
static void StartCtlPDM(void const * argument);

extern adc_t adc1;
extern adc_t adc2;

extern vn7004_t vn1;
extern vn7004_t vn2;
extern vn7004_t vn3;

static pump_t pump = {
    .enable_delay = 1000,
    .disable_delay = 3000
};

static int pump_algo(int water_level);

int acc = 0;

int app_init(){
    osThreadStaticDef(CtlPDM, StartCtlPDM, osPriorityHigh, 0, sizeof(CtlPDMBuffer)/4, CtlPDMBuffer, &CtlPDMControlBlock);
    CtlPDMHandle = osThreadCreate(osThread(CtlPDM), NULL);
    return 0;
}

int get_acc(){
    return acc;
}

void StartCtlPDM(void const * argument) {
    (void)argument;
    uint32_t volt_bat = 0;
    int  over_voltage = 0, override_water = 0;
    ///HAL_ADCEx_Calibration_Start(&hadc1);
    //HAL_ADCEx_Calibration_Start(&hadc2);
        uint8_t water_level;
        start_adc();
    //__HAL_TIM_ENABLE(&htim1);
    for(;;) {
        if (ulTaskNotifyTake( pdTRUE, 100) == 0){
            continue;
        }
        vn7004_poll(&vn1);
        vn7004_poll(&vn2);        
        vn7004_poll(&vn3);

        uint16_t temper, val;
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
        }



        if (adc_get_ch(&adc1, 0, &temper) == 0){

        }
        water_level = read_pin();

        led(acc);

        
        int pump_status = pump_algo(water_level);
        if (override_water) {
            pump_status = 1;
        }
        (void) over_voltage;
        (void) pump_status; 
    }
}

void adc1_cb(){
    BaseType_t tpw;
    vTaskNotifyGiveFromISR(CtlPDMHandle, &tpw);
}

void adc2_cb(){}




int pump_algo(int water_level){


    if ((pump.disable == 1) || (get_acc() == 0)) {
        pump.water_state = 0;
        return 0;
    }

    if (pump.override){
        pump.water_state = 1;        
        return 1;
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