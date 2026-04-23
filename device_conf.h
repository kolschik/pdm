#pragma once

#ifndef STM32F1
#define STM32F1
#endif
#ifndef STM32F103xB
#define STM32F103xB
#endif
#ifndef USE_FULL_LL_DRIVER
#define USE_FULL_LL_DRIVER
#endif

#define USE_CAN
#define USE_CAN_BOOT

#define TRIM_BANK 2
#define KEYPAD_BANK 3

#define USE_FLASH

#define MIN_FLASH 4
#define MAX_FLASH 64

#define BOOTLOADER_VERSION 0x11
#define DEVNAME "pdm"
#define BOOT_ID 0x01