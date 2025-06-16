#pragma once
#include "stdint.h"
#include "can.h"



typedef enum {
    N2kOnOff_Off=0,         ///< No, Off, Disabled
    N2kOnOff_On=1,          ///< Yes, On, Enabled
    N2kOnOff_Error=2,       ///< Error
    N2kOnOff_Unavailable=3  ///< Unavailable
}tN2kOnOff;

typedef enum {
    N2kft_Fuel=0,             ///< fluid type is fuel
    N2kft_Water=1,            ///< fluid type is water
    N2kft_GrayWater=2,        ///< fluid type is gray water
    N2kft_LiveWell=3,         ///< fluid type is live well
    N2kft_Oil=4,              ///< fluid type is oil
    N2kft_BlackWater=5,       ///< fluid type is black water
    N2kft_FuelGasoline=6,     ///< fluid type is gasoline fuel
    N2kft_Error=14,           ///< error occurred
    N2kft_Unavailable=15      ///< unavailable
}tN2kFluidType;

__attribute__((aligned(4)))typedef struct {
    unsigned char Priority;
    unsigned long PGN;
    unsigned char Source;
    unsigned char Destination;
    int DataLen;
    unsigned char Data[223];
    unsigned long MsgTime;
} tN2kMsg_t;

int nmea_init(uint8_t src);
int packN2k(tN2kMsg_t *N2kMsg, can_fifo_t *fifo);

void SetN2kPGN127508(tN2kMsg_t *N2kMsg, uint8_t BatInst, uint16_t BatVolt, uint16_t BatCur, uint16_t BatTemp, uint8_t SID);
void SetN2kPGN127505(tN2kMsg_t *N2kMsg, uint8_t Instance, tN2kFluidType FluidType, uint16_t Level, uint32_t Capacity);
void SetN2kPGN127751(tN2kMsg_t *N2kMsg, uint8_t Instance, uint16_t Voltage, uint32_t Current, uint8_t SID);

int ParseN2kPGN127502(tN2kMsg_t *N2kMsg, tN2kOnOff *sw, uint8_t *bank);
/************************************************************************//**
 * \brief Convert a CAN Id to NMEA2000 values
 *
 * \param id    given CAN Id
 * \param prio  Priority of the N2k message
 * \param pgn   PGN of the N2k message
 * \param src   Source of the N2k message
 * \param dst   Destination of the N2k message
 */
void CanIdToN2k(uint32_t id, tN2kMsg_t *msg);


typedef void (*nmea_send_handler)(tN2kMsg_t *msg, void *argument);