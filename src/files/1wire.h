#ifndef _1_WIRE_H    /* Guard against multiple inclusion */
#define _1_WIRE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <xc.h>
#include <stdio.h>
#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include <string.h>
#include "definitions.h"                // SYS function prototypes   
    
#define FAMILY_CODE 0x01
#define READ_ROM    0x33

extern uint8_t Hwid[7]; 
    
void GetHwid(uint8_t * hwidAddress);
void CalculateCrc(uint8_t * hwidAddress);

#ifdef __cplusplus
}
#endif

#endif /* _EXAMPLE_FILE_NAME_H */


