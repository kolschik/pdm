#include "app.h"
#include "adc.h"
#include "gpio.h"
#include "vn7004.h"
#include "nmea.h"

osThreadId CtlPDMHandle;
extern osThreadId CANTaskHandle;

static uint32_t CtlPDMBuffer[ 256 ];
static osStaticThreadDef_t CtlPDMControlBlock;
static void StartCtlPDM(void const * argument);

extern adc_t adc1;
extern adc_t adc2;

extern vn7004_t vn1;
extern vn7004_t vn2;
extern vn7004_t vn3;
extern vn7004_t vn4;

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
uint16_t curr_array[256];
uint8_t curr_i; 
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
            // todo register error

            continue;
        }
        vn7004_poll(&vn1);
        vn7004_poll(&vn2);        
        vn7004_poll(&vn3);
        vn7004_poll(&vn4);

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

        vn7004_ctl(&vn1.ic[0], acc);
        vn7004_ctl(&vn2.ic[0], pump_status);
        vn7004_ctl(&vn3.ic[0], 0);
        vn7004_ctl(&vn3.ic[1], 0);
        vn7004_ctl(&vn4.ic[0], 1);
        vn7004_ctl(&vn4.ic[1], 1);

        curr_array[curr_i++] = vn7004_get_cur(&vn4.ic[1]);
    }
}

void adc1_cb(){
    BaseType_t tpw;
    vTaskNotifyGiveFromISR(CtlPDMHandle, &tpw);
}

void adc2_cb(){}




int pump_algo(int water_level){

    int water_buf = pump.enable * get_acc();

    if (pump._auto == 0){
        pump.water_state = water_buf;
        return water_buf;
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


void nmea_sender(){
    tN2kMsg_t msg;
    can_fifo_t tx_fifo;
    static uint8_t sid1278508 = 0;  
    static uint32_t send_stat = 0;
    /*
  HAL_CAN_Start(&hcan);
  CAN_TxHeaderTypeDef can_header;
  can_header.ExtId = 1000;
  can_header.IDE = 0;
  can_header.DLC = 8;
  uint8_t data[8];
  for(;;)
  {
    data[7] = volt;
    data[0]++;
    uint32_t mailbox=0;
    HAL_CAN_AddTxMessage(&hcan, &can_header, data, &mailbox);
    osDelay(10);
  }
*/
    uint32_t volt=0, cur=0, battemp=0;
  
    for (uint32_t i=0; i<3; i++){
        switch (i){
        case 0:
            SetN2kPGN127508(&msg, 0, volt, cur, battemp, sid1278508++);
            break;
        case 1:
            SetN2kPGN127505(&msg, 0, N2kft_Water, 0, 1);
            break;
        case 2:
            break;        
        default:
            break;
        }
    }



    if (packN2k(&msg, &tx_fifo)){
        return;
    }

    if (can_tx(tx_fifo, 50)) {
        send_stat++;
    }

    return;
}


void can_rx_cb (can_fifo_t *fifo){
    (void)fifo;
}
void can_tx_cb(uint8_t *tx_slot){
    (void)tx_slot;
    BaseType_t not = 0;
    vTaskNotifyGiveFromISR(CANTaskHandle, &not);
}