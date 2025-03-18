/* TODO:  Include other files here if needed. */
#include <xc.h>
#include <string.h>
#include <stdbool.h>                    // Defines true
#include "definitions.h"
#include "mac.h"

bool eui64Filled = false;
uint8_t eui48[EUI_48_BYTES_LENGTH] = {0,0,0,0,0,0};
uint8_t eui64[EUI_64_BYTES_LENGTH] = {0,0,0,0,0,0,0,0};

uint8_t ackData = 0;    
uint8_t addressEui48 = 0x9A; // AT24MAC402 ONLY
uint8_t addressEui64 = 0x98; // AT24MAC602 ONLY

MAC_STATES macState = MAC_STATE_EEPROM_STATUS_VERIFY;
