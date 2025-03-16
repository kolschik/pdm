#include "app.h"

osThreadId CtlPDMHandle;
static uint32_t CtlPDMBuffer[ 256 ];
static osStaticThreadDef_t CtlPDMControlBlock;
static void StartCtlPDM(void const * argument);

extern adc_t adc1;

int app_init(){
    osThreadStaticDef(CtlPDM, StartCtlPDM, osPriorityHigh, 0, sizeof(CtlPDMBuffer)/4, CtlPDMBuffer, &CtlPDMControlBlock);
    CtlPDMHandle = osThreadCreate(osThread(CtlPDM), NULL);
    return 0;
}


void StartCtlPDM(void const * argument) {


    ///HAL_ADCEx_Calibration_Start(&hadc1);
    //HAL_ADCEx_Calibration_Start(&hadc2);
        uint8_t water_level;
        start_adc();
    //__HAL_TIM_ENABLE(&htim1);
    for(;;) {
        if (ulTaskNotifyTake( pdTRUE, 100) == 0){
            continue;
        }
        uint32_t volt = ADC1->JDR4 >> 4;

        uint16_t temper;
        if (adc_get_ch(&adc1, 0, &temper) == 0){

        }
        water_level = read_pin();
    }
}

void adc1_cb(){
    BaseType_t tpw;
    vTaskNotifyGiveFromISR(CtlPDMHandle, &tpw);
}

void adc2_cb(){}