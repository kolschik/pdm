#include "nmea.h"

struct {
    uint8_t source_id;
}nmea2k_t;


int nmea_init(uint8_t source){
    if (source >= N2kMaxCanBusAddress){
        return EINVAL;
    }
    nmea2k_t.source_id = source;
    return 0;
}

#include <string.h>
#include <nmea.h>


void SetN2kPGN127508(tN2kMsg_t *N2kMsg, uint8_t BatInst, uint16_t BatVolt, uint16_t BatCur, uint16_t BatTemp, uint8_t SID) {
    N2kMsg->PGN =127508L;

    N2kMsg->Data[0] = BatInst;

    N2kMsg->Data[1] = BatVolt;
    N2kMsg->Data[2] = BatVolt >> 8;

    N2kMsg->Data[3] = BatCur;
    N2kMsg->Data[4] = BatCur >> 8;

    N2kMsg->Data[5] = BatTemp;
    N2kMsg->Data[6] = BatTemp >> 8;

    N2kMsg->Data[7] = SID;

    N2kMsg->Priority=6;    
}


void SetN2kPGN127501(tN2kMsg_t *N2kMsg, uint32_t bank, tN2kOnOff *sw, const uint8_t sw_num) {
    uint64_t BankStatus = 0xffffffffffffffff;
    for (uint8_t i=0; i<sw_num; i++){
        BankStatus = (BankStatus << 2) | sw[i];
    }
    BankStatus = (BankStatus << 8) | bank;
    memcpy(N2kMsg->Data, &BankStatus, sizeof(BankStatus));
    N2kMsg->PGN = 127501L;
    N2kMsg->Priority=3;
}


void SetN2kPGN127505(tN2kMsg_t *N2kMsg, uint8_t Instance, tN2kFluidType FluidType, uint16_t Level, uint32_t Capacity) {
    N2kMsg->PGN = 127505L;

    N2kMsg->Priority=6;

    N2kMsg->Data[0] = (Instance & 0x0f) | ((FluidType & 0x0f)<<4);

    //0.004
    N2kMsg->Data[1] = Level;
    N2kMsg->Data[2] = Level >> 8;

    //0.1
    N2kMsg->Data[3] = Capacity;
    N2kMsg->Data[4] = Capacity >> 8;
    N2kMsg->Data[5] = Capacity >> 16;
    N2kMsg->Data[6] = Capacity >> 24;

    N2kMsg->Data[7] = 0xff;
}


int packN2k(tN2kMsg_t *N2kMsg, can_fifo_t *fifo){
    if (N2kMsg->PGN & 0xff){
        N2kMsg->Destination = 0xff;
    }
    N2kMsg->Source = nmea2k_t.source_id;
    // CAN bus address range is 0-251. Anyway allow ISO address claim mgs.
    if (N2kMsg->Source > N2kMaxCanBusAddress && N2kMsg->PGN != N2kPGNIsoAddressClaim) {
        return EFAULT;
    }

    uint32_t id = ( ((unsigned long)(N2kMsg->Priority & 0x7))<<26 | N2kMsg->PGN<<8 | (unsigned long)N2kMsg->Source);
    if ((uint8_t)(N2kMsg->PGN >> 8) < 240) {  // PDU1 format
        if (N2kMsg->PGN & 0xff) {
            return EFAULT;  // for PDU1 format PGN lowest byte has to be 0 for the destination.
        }
        id = ( ((unsigned long)(N2kMsg->Priority & 0x7))<<26 | N2kMsg->PGN<<8 | ((unsigned long)N2kMsg->Destination)<<8 | (unsigned long)N2kMsg->Source);
    }

    fifo->flg.dlc = 8;
    fifo->flg.rtr = 0;
    fifo->flg.ide = 1;
    memcpy(fifo->data8, N2kMsg->Data, 8);
    fifo->id = id;
    return 0;

}