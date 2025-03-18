/* TODO:  Include other files here if needed. */
#include <xc.h>
#include <string.h>
#include <stdbool.h>                    // Defines true
#include "definitions.h"
#include "hardware_rev.h"

#include "files\delay.h"

uint8_t RevNum = 0;

uint8_t calculateRevision(uint8_t pin0, uint8_t pin1, uint8_t pin2) 
{
    return (uint8_t)((pin2 * 9) + (pin1 * 3) + (pin0 * 1) + 1);
}

void DetermineHardwareRev(void)
{
    uint8_t pin0;
    uint8_t pin1;
    uint8_t pin2;
    
    HW_REV_PIN0_SET_PULLUP();    // Set the internal pullup for pin0
    HW_REV_PIN1_SET_PULLUP();    // Set the internal pullup for pin1
    HW_REV_PIN2_SET_PULLUP();    // Set the internal pullup for pin2
    delayMS(1);
    
    if(HW_REV_PIN0_GET_STATUS() == true)
    {   // Pin is high
        HW_REV_PIN0_SET_PULLDOWN();  // Set the internal pulldown
        delayMS(1);
        if(HW_REV_PIN0_GET_STATUS() == true)
        {   // Pin still high, so there must be an external pullup
            pin0 = PIN_STATUS_HIGH;
        } 
        else 
        {   // pin is low right now, so the internal pulldown does its work, so must be HIGH Z 
            pin0 = PIN_STATUS_HIGH_IMPEDANCE;
        }
    } 
    else 
    {   // Pin keeps low with internal pullup enabled, so there must be an external pulldown
        pin0 = PIN_STATUS_LOW;
    }
    
    if(HW_REV_PIN1_GET_STATUS() == true)
    {   // Pin is high
        HW_REV_PIN1_SET_PULLDOWN();  // Set the internal pulldown
        delayMS(1);
        if(HW_REV_PIN1_GET_STATUS() == true)
        {   // Pin still high, so there must be an external pullup
            pin1 = PIN_STATUS_HIGH;
        } 
        else 
        {   // pin is low right now, so the internal pulldown does its work, so must be HIGH Z 
            pin1 = PIN_STATUS_HIGH_IMPEDANCE;
        }
    } 
    else 
    {   // Pin keeps low with internal pullup enabled, so there must be an external pulldown
        pin1 = PIN_STATUS_LOW;
    }
    
    if(HW_REV_PIN2_GET_STATUS() == true)
    {   // Pin is high
        HW_REV_PIN2_SET_PULLDOWN();  // Set the internal pulldown
        delayMS(1);
        if(HW_REV_PIN2_GET_STATUS() == true)
        {   // Pin still high, so there must be an external pullup
            pin2 = PIN_STATUS_HIGH;
        } 
        else 
        {   // pin is low right now, so the internal pulldown does its work, so must be HIGH Z 
            pin2 = PIN_STATUS_HIGH_IMPEDANCE;
        }
    } 
    else 
    {   // Pin keeps low with internal pullup enabled, so there must be an external pulldown
        pin2 = PIN_STATUS_LOW;
    }
    
    RevNum = calculateRevision(pin0, pin1, pin2);
    
    //SYS_DEBUG_PRINT(SYS_ERROR_ERROR, "\r\n Revision >> %i\r\n", RevNum);
}


