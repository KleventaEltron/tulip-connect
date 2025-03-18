#ifndef _MAC_H    /* Guard against multiple inclusion */
#define _MAC_H

// Device addresses (different for EEPROM and EUI)
#define AT24MAC_DEVICE_ADDR_EEPROM          0x0056 // 0b1010110 // RW 
#define AT24MAC_DEVICE_ADDR_EUI             0x005E // 0b1011110 

#define AT24MAC_ACK_DATA_LENGTH             1

//#define APP_AT24MAC_MEMORY_ADDR             0x00
//#define APP_TRANSMIT_DATA_LENGTH            5

#define AT24MAC_RECEIVE_DUMMY_WRITE_LENGTH      1
#define AT24MAC_RECEIVE_DATA_LENGTH             4
    
#define APP_RECEIVE_DATA_LENGTH_EUI         6

#define EUI_48_BYTES_LENGTH 6
#define EUI_64_BYTES_LENGTH 8

typedef enum
{
    MAC_STATE_EEPROM_STATUS_VERIFY,
    MAC_STATE_EEPROM_WRITE,
    MAC_STATE_EEPROM_WAIT_WRITE_COMPLETE,
    MAC_STATE_EEPROM_CHECK_INTERNAL_WRITE_STATUS,
    MAC_STATE_EEPROM_READ,
    MAC_STATE_EEPROM_WAIT_READ_COMPLETE,
    MAC_STATE_VERIFY,
    MAC_STATE_IDLE,
    MAC_STATE_XFER_SUCCESSFUL,
    MAC_STATE_XFER_ERROR

} MAC_STATES;

extern MAC_STATES macState;

extern uint8_t ackData;
extern uint8_t addressEui48;
extern uint8_t addressEui64;

extern bool eui64Filled;
extern uint8_t eui48[EUI_48_BYTES_LENGTH];
extern uint8_t eui64[EUI_64_BYTES_LENGTH];

#endif 