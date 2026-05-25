
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "bsp.h"
#include "app.h"

#if defined PDM_BOOT
#include "gpio.h"
#include "boot.h"
const gpio_t gpio_a[] = {
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
    {   .port = LED2_GPIO_Port,
        .pin = LED2_Pin,
        .cfg = {
            .mode = LL_GPIO_MODE_OUTPUT,
            .open_drain = LL_GPIO_OUTPUT_PUSHPULL,
            .freq = LL_GPIO_SPEED_FREQ_HIGH, 
            .pull = LL_GPIO_PULL_DOWN,
            .init_val = 0
        }}
};

extern can_t can1;
#else 
osThreadId InputTaskHandle;
uint32_t InputTaskBuffer[ 128 ];
osStaticThreadDef_t InputTaskControlBlock;



void StartInputTask(void const * argument);
extern uint32_t SystemCoreClock;
volatile uint32_t vtor_address ;

#endif

void SystemClock_Config(void);


int main(void) {
#if !defined PDM_BOOT
    SCB->VTOR = (volatile uint32_t)0x08001000 ;

    HAL_Init();
#endif

    SystemClock_Config();


#if defined PDM_BOOT
    __HAL_RCC_AFIO_CLK_ENABLE();
    __HAL_AFIO_REMAP_SWJ_NOJTAG();
	  __disable_irq(); // запрещаем прерывания  
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA | LL_APB2_GRP1_PERIPH_GPIOB | LL_APB2_GRP1_PERIPH_GPIOC);
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);  

    gpio_init(gpio_a, sizeof(gpio_a) / sizeof(gpio_t));

    can_init(&can1);

    can_start();
    try_boot();
#else
    __enable_irq();
    if (bsp_init()){
        while(1);
    }

    app_init();

    osThreadStaticDef(InputTask, StartInputTask, osPriorityNormal, 0, 128, InputTaskBuffer, &InputTaskControlBlock);
    InputTaskHandle = osThreadCreate(osThread(InputTask), NULL);

    osKernelStart();
#endif


    while (1)
    {

    }

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_1);
  while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_1)
  {
  }
  LL_RCC_HSE_Enable();

   /* Wait till HSE is ready */
  while(LL_RCC_HSE_IsReady() != 1)
  {

  }
  LL_RCC_LSI_Enable();

   /* Wait till LSI is ready */
  while(LL_RCC_LSI_IsReady() != 1)
  {

  }
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE_DIV_1, LL_RCC_PLL_MUL_4);
  LL_RCC_PLL_Enable();

   /* Wait till PLL is ready */
  while(LL_RCC_PLL_IsReady() != 1)
  {

  }
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {

  }
  LL_SetSystemCoreClock(48000000);
#if defined PDM_BOOT
  LL_SetSystemCoreClock(48000000);
  LL_Init1msTick(48000000);
#else
  //LL_Init1msTick(48000000);
   /* Update the time base */
  if (HAL_InitTick (TICK_INT_PRIORITY) != HAL_OK)
  {
    Error_Handler();
  }
#endif

  LL_RCC_SetADCClockSource(LL_RCC_ADC_CLKSRC_PCLK2_DIV_4);
}

/*
static void MX_IWDG_Init(void)
{

  LL_IWDG_Enable(IWDG);
  LL_IWDG_EnableWriteAccess(IWDG);
  LL_IWDG_SetPrescaler(IWDG, LL_IWDG_PRESCALER_4);
  LL_IWDG_SetReloadCounter(IWDG, 4095);
  while (LL_IWDG_IsReady(IWDG) != 1)
  {
  }

  LL_IWDG_ReloadCounter(IWDG);


}
*/

uint32_t volt;


/* USER CODE BEGIN Header_StartInputTask */
/**
* @brief Function implementing the InputTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartInputTask */
void StartInputTask(void const * argument)
{

  /* USER CODE BEGIN StartInputTask */
    (void)argument;
    /* Infinite loop */
    for(;;)
    {
      //#define WATER_DET_Pin GPIO_PIN_12
      //#define WATER_DET_GPIO_Port GPIOB

      osDelay(1000);
    }
  /* USER CODE END StartInputTask */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
