/* TODO:  Include other files here if needed. */
#include <xc.h>
#include <string.h>
#include <stdbool.h>                    // Defines true
#include "definitions.h"
#include "alarms.h"

uint32_t alarms[ALARM_ARRAY_SIZE];

void ClearAlarmArray(void)
{
    for (uint32_t i = 0; i < ALARM_ARRAY_SIZE; i++) 
    {
        alarms[i] = 0; // clear all alarm variables
    }
}

void SetOrClearAlarm(uint32_t alarm, bool SetOrReset)
{
    uint32_t index = alarm >> 5;     // Calculate byte in array, divide by 32
    uint32_t bitPos = alarm % 32;   // Calculate bit position, rest of 32
    
    if (index < ALARM_ARRAY_SIZE)
    {   // Check if byte is in array, otherwise it goes outside the array
        if (SetOrReset == SET_ALARM)
            alarms[index] |= (true << bitPos);
        else
            alarms[index] &= ~(true << bitPos);
    }    
}

bool GetAlarmStatus(uint32_t alarm)
{
    uint32_t index = alarm >> 5;     // Calculate byte in array, divide by 32
    uint32_t bitPos = alarm % 32;   // Calculate bit position, rest of 32
    
    if (index < ALARM_ARRAY_SIZE)
        return (bool)((alarms[index] >> bitPos) & 0x01);
    else
        return false;
}

uint32_t GetAlarm ( uint32_t index )
{
    return alarms[index];
}

static bool checkForAlarm(void)
{
    for (uint32_t i = 0; i < ALARM_ARRAY_SIZE; i++) 
    {
        if (alarms[i] != 0) 
        {
            return true; // If variable is not 0, alarm present, return true
        }
    }
    return false; // If all variables are 0, no alarm, return false
}

void SetOrClearAlarmLed(void)
{
    if (checkForAlarm() == true)
        LedAlarm_Set();
    else
        LedAlarm_Clear();
}