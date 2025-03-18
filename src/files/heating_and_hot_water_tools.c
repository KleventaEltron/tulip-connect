/* TODO:  Include other files here if needed. */
#include <xc.h>
#include <string.h>
#include <stdbool.h>                    // Defines true
#include "definitions.h"
#include "heating_and_hot_water_tools.h"
#include "modbus\heatpump_parameters.h"

int16_t DetermineCorrectSetpoint(APP_HEATING_AND_HOT_WATER_STATES currentState, int16_t setpointHotWater, int16_t setpointHeating, int16_t setpointHotWaterOffset, int16_t setpointSterilization, int16_t setpointSterilizationOffset)
{
    // Setpoint in heatpump hangt af van de app state
    // Het setpoint is altijd het CV setpoint volgens de Connect tenzij hot water states actief, dan mag het setpoint van hot water in de connect naar de wartempomp gestuurd worden.
    
    if (currentState == APP_HEATING_AND_HOT_WATER_STATE_INIT_STERILIZATION ||
        currentState == APP_HEATING_AND_HOT_WATER_STATE_STERILIZATION ||
        currentState == APP_HEATING_AND_HOT_WATER_STATE_SWITCH_3_WAY_VALVE_TO_HOT_WATER_CIRCUIT_AND_GO_STERILIZATION_MODE)
    {
        return (setpointSterilization + setpointSterilizationOffset);
    }
    else if (currentState == APP_HEATING_AND_HOT_WATER_STATE_INIT_HOT_WATER ||
        currentState == APP_HEATING_AND_HOT_WATER_STATE_HOT_WATER ||
        currentState == APP_HEATING_AND_HOT_WATER_STATE_SWITCH_3_WAY_VALVE_TO_HOT_WATER_CIRCUIT_AND_GO_HOT_WATER_MODE)
    {
        return (setpointHotWater + setpointHotWaterOffset);
    }
    else
    {
        return setpointHeating;
    }  
}

bool CheckIfDefrostModeActive(void)
{
    
    if (RealTimeDataStatussen[ADDRESS_RUNNING_STATUS_1 - START_ADDRESS_REAL_TIME_DATA_STATUSSEN][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP] & (1 << RUNNING_STATUS_1_SYSTEM_DEFROST_BIT))
        return true;
    else
        return false;
    
    
    //return (bool) !NotInputSw1_Get(); // Debug only
}

bool CheckIfCompressorIsRunning(uint16_t compressorFrequency)
{
    static uint32_t counter = UINT32_MAX;
    
    if (compressorFrequency > 0)
    {   // Compressor is running
        if (counter == UINT32_MAX) // If counter is MAX value, this is start of counting sign
            counter = 0;
        else    // Add 1 to the counter
            counter++;
    }
    else
    {   // Compressor not running, reset counter value.
        counter = UINT32_MAX;
    }
    
    if ((counter > 10) && (counter < UINT32_MAX))
        return true;
    else   
        return false;
}

bool CheckIfTemperatureIsBelowSetpointMinusDelta(int16_t setpoint, int16_t currentTemperature, int16_t delta)
// 580, -9999, 0
{
    if ((currentTemperature != TEMPERATURE_ALARM_VALUE) && 
        (setpoint != TEMPERATURE_ALARM_VALUE) && 
        (delta != TEMPERATURE_ALARM_VALUE))
    {
        if (currentTemperature < (setpoint - delta))
            return true;
        else
            return false;
    }
    else
    {
        return false;
    }
}

bool CheckIfRetourTemperatureReachedSetpoint(int16_t retourTemperature, int16_t setpoint)
{
    if (retourTemperature >= setpoint)
    {   // Retour temperature is the same or greater than the setpoint
        return true;
    }
    else
    {   // Retour temperature is smaller than the setpoint
        return false;
    }
}

bool CheckIfSetpointReached(int16_t setpoint, int16_t currentTemperature)
{
    if ((currentTemperature != TEMPERATURE_ALARM_VALUE) && 
        (setpoint != TEMPERATURE_ALARM_VALUE)) 
    {
        if (currentTemperature >= setpoint)
            return true;
        else
            return false;
    }
    else
    {
        return false;
    }
}

bool CheckIfSetpointInHeatpumpIsUpToDate(int16_t currentSetpointInDisplay, uint16_t currentSetpointInHeatpump)
{
    if ((currentSetpointInHeatpump * 10) == currentSetpointInDisplay)
        return true;
    else
        return false;
}

bool TemperatureRisedEnough(uint32_t* secondCounter, int16_t currentTemp, int16_t* initialTemp, uint16_t tempThreshold, uint16_t intervalSeconds)
{
    if (currentTemp >= (*initialTemp + tempThreshold))
    {   // Current temperature has rised above the initial temp + 1 degree Celcius
        *initialTemp = currentTemp;  
        *secondCounter = 0;
        return true;
    }
    else
    {   // Current temperature not rised above the initial temp + 1 degree Celcius (yet)
        if (*secondCounter >= intervalSeconds)
        {   // An hour has passed
            return false;
        }
        else
        {   // Hour not passed
            return true;
        }
    }
    
    // Return false when temperature has not reached a degree rise in 100 minutes
    // Return true when time limit still not reached
}

bool IsSoftwareResetPossible(APP_HEATING_AND_HOT_WATER_STATES appState, uint32_t secondCounterLegionella)
{
    if ((appState == APP_HEATING_AND_HOT_WATER_STATE_IDLE) ||
        (appState == APP_HEATING_AND_HOT_WATER_STATE_INIT_HEATING) || 
        (appState == APP_HEATING_AND_HOT_WATER_STATE_HEATING) ||
        (appState > 12))
    {   // Is in idle or one of the heating modes
        if (secondCounterLegionella != UINT32_MAX)
        {   // If legionella counter is running, legionella is active on background.
            return false;
        }
        else
        {
            return true;
        }
    }
    else 
    {
        return false;
    }
}

bool IsSterilizationRequired(uint16_t sterilizationFunction, 
        uint16_t sterilizationIntervalDays, 
        uint16_t sterilizationStartTime, 
        int16_t currentHotWaterTemperature,
        uint8_t currentDisplayTimeHours,
        uint8_t currentDisplayTimeMinutes,
        uint16_t dayCounter,
        uint32_t secondCounterLegionella,
        uint16_t maxTimeOutOfSterilizationMode,
        bool heatingElementStatus)
{
    bool goToSterilizationBool = false;
    
    if (secondCounterLegionella != UINT32_MAX)
    {   // Sterilization is already running
        if ((secondCounterLegionella > maxTimeOutOfSterilizationMode) && (heatingElementStatus == true))
        {   // Sterilization hot water element was already on
            goToSterilizationBool = true;
        }
    }
    else
    {   // Sterilization not running
        if ((currentHotWaterTemperature != TEMPERATURE_ALARM_VALUE) &&
            (sterilizationFunction == STERILIZATION_FUNCTION_AUTO))
        {   // There is a hot water buffer and sterilization mode is on AUTO
            if (dayCounter >= (sterilizationIntervalDays -1))
            {   // Sterilization must be done today
                if ((currentDisplayTimeHours == sterilizationStartTime) && (currentDisplayTimeMinutes == 0))
                {   // Current time is the set time for sterilization
                    goToSterilizationBool = true;
                }
            }
        }
    }

    return goToSterilizationBool;
}

bool IsSterilizationRunningInBackground(uint32_t secondCounterLegionella, bool heatingElementStatus , APP_HEATING_AND_HOT_WATER_STATES appState)
{
    bool sterilizationInBackgroundActive = false; 
    
    if ((appState != APP_HEATING_AND_HOT_WATER_STATE_INIT_STERILIZATION) && 
            (appState != APP_HEATING_AND_HOT_WATER_STATE_STERILIZATION) &&
            (appState != APP_HEATING_AND_HOT_WATER_STATE_SWITCH_3_WAY_VALVE_TO_HOT_WATER_CIRCUIT_AND_GO_STERILIZATION_MODE))
    {   // Is not in one of the sterilization states
        if (secondCounterLegionella != UINT32_MAX)
        {   // Legionella counter is running
            if (heatingElementStatus == true)
            {   // Heating element is on
                sterilizationInBackgroundActive = true;
            }
        }
    }
    
    return sterilizationInBackgroundActive;
}

char * BooleanToOnOffString(bool boolean)
{
    if (boolean == true)
        return "ON";
    else
        return "OFF";
}