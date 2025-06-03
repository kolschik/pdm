#include "NMEA2000.h"


class nmea_node : public tNMEA2000 {
public:
    nmea_node(){}
    int init();
    bool CANOpen();
    bool CANGetFrame(unsigned long &id, unsigned char &len, unsigned char *buf);
    bool CANSendFrame(unsigned long id, unsigned char len, const unsigned char *buf, bool wait_sent);
};

bool nmea_node::CANOpen(){
    return false;
}

bool nmea_node::CANGetFrame(unsigned long &id, unsigned char &len, unsigned char *buf){
    (void)id;
    (void)len;
    (void)buf;
    return false;
}

bool nmea_node::CANSendFrame(unsigned long id, unsigned char len, const unsigned char *buf, bool wait_sent){
    (void)id;
    (void)len;
    (void)buf;
    (void)wait_sent;
    return false;
}

uint32_t millis(){
    return 0;
}

nmea_node nmea;

int nmea_node::init(){

    SetN2kCANSendFrameBufSize(250);
    // Set Product information
    SetProductInformation("00000001", // Manufacturer's Model serial code
                                    100, // Manufacturer's product code
                                    "Message sender example",  // Manufacturer's Model ID
                                    "1.1.2.35 (2022-10-01)",  // Manufacturer's Software version code
                                    "1.1.2.0 (2022-10-01)" // Manufacturer's Model version
                                    );
    // Set device information
    SetDeviceInformation(1, // Unique number. Use e.g. Serial number.
                                    132, // Device function=Analog to NMEA 2000 Gateway. See codes on https://web.archive.org/web/20190531120557/https://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                                    25, // Device class=Inter/Intranetwork Device. See codes on  https://web.archive.org/web/20190531120557/https://www.nmea.org/Assets/20120726%20nmea%202000%20class%20&%20function%20codes%20v%202.00.pdf
                                    2046 // Just choosen free from code list on https://web.archive.org/web/20190529161431/http://www.nmea.org/Assets/20121020%20nmea%202000%20registration%20list.pdf
                                );
    // Uncomment 3 rows below to see, what device will send to bus                           
 //   SetForwardStream(&Serial);  // PC output to default serial port
    SetForwardType(tNMEA2000::fwdt_Text); // Show in clear text. Leave uncommented for default Actisense format.
    SetForwardOwnMessages(false); // Do not print own messages.

    // We act as real node on bus. Some devices does not show messages, if they can not request information.
    SetMode(tNMEA2000::N2km_ListenAndNode,22);
    //NMEA2000.SetDebugMode(tNMEA2000::dm_ClearText); // Uncomment this, so you can test code without CAN bus chips on Arduino Mega
    //EnableForward(EnableForward); // Disable all msg forwarding to USB (=Serial)
    //SetOnOpen(OnN2kOpen);
    Open();

    return 0;
}


extern "C"{
    int init_nmea(){
        return nmea.init();
        
    }
}
