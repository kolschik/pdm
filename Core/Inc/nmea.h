#pragma once
#include "stdint.h"
#include "can.h"



typedef enum {
    N2kOnOff_Off=0,         ///< No, Off, Disabled
    N2kOnOff_On=1,          ///< Yes, On, Enabled
    N2kOnOff_Error=2,       ///< Error
    N2kOnOff_Unavailable=3  ///< Unavailable
}tN2kOnOff;
  
__attribute__((aligned(4)))typedef struct {
    unsigned char Priority;
    unsigned long PGN;
    unsigned char Source;
    unsigned char Destination;
    int DataLen;
    unsigned char Data[223];
    unsigned long MsgTime;
} tN2kMsg_t;