/* TODO:  Include other files here if needed. */
#include <xc.h>
#include <string.h>
#include <stdbool.h>                    // Defines true
#include "definitions.h"
#include "circulation_pump.h"
#include "files\eeprom.h"
#include "logging.h"


#define TRY_OUT_VALUE_COUNTERS 90000;

CIRCULATION_PUMP_STATES circulationPumpState;

static char * StringPumpInit = "Init";
static char * StringPumpOff = "Pump off";
static char * StringPumpOn = "Pump on";
static char * StringPumpLagTime = "Pump is in lag time";
static char * StringPumpTooLongIdle = "Pump too long idle";
static char * StringPumpOffTooLowTemp = "Pump off too low temp";

static char * StringPumpUnknown = "Unknown";

void CirculationPumpInit(void)
{
    circulationPumpState = CIRCULATION_PUMP_STATE_INIT;
}

CIRCULATION_PUMP_STATES GetCirculationPumpState(void)
{
    return (circulationPumpState);
}

char * GetStringWithCirculationPumpState(CIRCULATION_PUMP_STATES pumpState)
{
    switch (pumpState)
    {
        case (CIRCULATION_PUMP_STATE_INIT):return StringPumpInit;break;
        case (CIRCULATION_PUMP_STATE_OFF):return StringPumpOff;break;
        case (CIRCULATION_PUMP_STATE_ON):return StringPumpOn;break;
        
        case (CIRCULATION_PUMP_STATE_LAG_TIME):return StringPumpLagTime;break;
        case (CIRCULATION_PUMP_STATE_TOO_LONG_IDLE):return StringPumpTooLongIdle;break;
        case (CIRCULATION_PUMP_STATE_OFF_TOO_LOW_TEMP):return StringPumpOffTooLowTemp;break;
        
        default: return StringPumpUnknown;break;
    }
}

static bool canCirculationPumpBeOnInThisAppState(APP_HEATING_AND_HOT_WATER_STATES state)
{
    if (state == APP_HEATING_AND_HOT_WATER_STATE_IDLE ||
        state == APP_HEATING_AND_HOT_WATER_STATE_INIT_HEATING ||
        state == APP_HEATING_AND_HOT_WATER_STATE_HEATING)
    {
        return true;
    }
    else 
    {
        return false;
    }
}

void CirculationPumpControl(APP_HEATING_AND_HOT_WATER_STATES appState, bool thermostatContact, bool heatingElement, uint32_t* secondCounter, int16_t heatingSetpoint, int16_t bufferTemp, bool defrostActive)
{      
    // Circulation pump must run if:
    //   App state is APP_HEATING_AND_HOT_WATER_STATE_WAIT_FOR_HEATING_SETPOINT_REACHED
    //   &&
    //   Thermostat contact has been made 
    //   && 
    //   Heating element is off
    //   ||
    //   If pump has been off for 2 hours, go on for 10 minutes
    //
    // Pump must keep running 2 minutes if pump was on and thermostat contact has been disconnected
    
    switch (circulationPumpState)
    {
        // 0: Circulation pump init
        case CIRCULATION_PUMP_STATE_INIT:
        {   // Init, only once
            *secondCounter = 0; // Start timer
            circulationPumpState = CIRCULATION_PUMP_STATE_OFF; 
            break;
        }
        
        // 1: Circulation pump OFF
        case CIRCULATION_PUMP_STATE_OFF:
        {   // Pump is off 
            //if ((appState == APP_HEATING_AND_HOT_WATER_STATE_WAIT_FOR_HEATING_SETPOINT_REACHED || appState == APP_HEATING_AND_HOT_WATER_STATE_IDLE) &&
            //    (thermostatContact == true) &&
            //    (heatingElement == false))
            if ((canCirculationPumpBeOnInThisAppState(app_Data.appState) == true) &&
                (thermostatContact == true) &&
                (heatingElement == false) && 
                (defrostActive == false))
            {   // If in heating mode or in idle mode and thermostat contact is made and heating element if off
                *secondCounter = 0;   // Reset timer
                TurnOnCirculationPump(); // Pump on
                circulationPumpState = CIRCULATION_PUMP_STATE_ON;
            }
            //else if (SecondCounterCirculationPump >= PUMP_MAXIMUM_OFF_TIME_SEC) 
            else if (*secondCounter >= ReadSmartEeprom32(SEEP_ADDR_PUMP_MAXIMUM_OFF_TIME_SEC))
            {   // Circulation pump has been off for 2 hours, so put on for 10 minutes
                *secondCounter = 0;   // Reset timer
                TurnOnCirculationPump(); // Pump on
                circulationPumpState = CIRCULATION_PUMP_STATE_TOO_LONG_IDLE;
            }
            else{} // Do nothing
            break;
        }
        
        // 2: Circulation pump ON
        case CIRCULATION_PUMP_STATE_ON:
        {   // Pump is on
            if (thermostatContact == false)
            {   // Pump is on, but thermostat contact goes to false, so go to lag time
                *secondCounter = 0; // Reset timer
                circulationPumpState = CIRCULATION_PUMP_STATE_LAG_TIME;
            }
            else if ((canCirculationPumpBeOnInThisAppState(app_Data.appState) == false) || (heatingElement == true) || (defrostActive == true))
            {   // App state is not suitable for circulation pump, or heating element has gone ON 
                *secondCounter = 0;   // Reset timer
                TurnOffCirculationPump(); // Pump off
                circulationPumpState = CIRCULATION_PUMP_STATE_OFF; 
            }
            else if (bufferTemp < (heatingSetpoint - ReadSmartEeprom16(SEEP_ADDR_PUMP_OFF_TEMP_TOO_LOW)))
            {   // Temperature dropped below minimum for the pump to be on
                *secondCounter = 0;   // Reset timer
                TurnOffCirculationPump(); // Pump off
                circulationPumpState = CIRCULATION_PUMP_STATE_OFF_TOO_LOW_TEMP; 
            }
            break;
        }
        
        // 3: Lag time: Thermostat contact has disconnected, but pump needs to run a bit longer
        case CIRCULATION_PUMP_STATE_LAG_TIME:
        {            
            //if(setLoggingLock()){
                // If state is heating mode, thermostat contact is made, and heating element is off, go back to CP on
                //if (((appState != APP_HEATING_AND_HOT_WATER_STATE_WAIT_FOR_HEATING_SETPOINT_REACHED) || (heatingElement == true)) ||
                //        (SecondCounterCirculationPump >= PUMP_LAG_TIME_AFTER_THERMOSTAT_CONTACT_DISCONNECTED_SEC))
                if ((canCirculationPumpBeOnInThisAppState(app_Data.appState) == true) &&
                    (thermostatContact == true) &&
                    (heatingElement == false))
                {   // App state is suitable for circulation pump and thermostat contact is made and heating element if off
                    *secondCounter = 0;   // Reset timer
                    TurnOnCirculationPump(); // Pump on
                    circulationPumpState = CIRCULATION_PUMP_STATE_ON;
                }

                if ((canCirculationPumpBeOnInThisAppState(app_Data.appState) == false) || (heatingElement == true) || (defrostActive == true) ||
                        (*secondCounter >= ReadSmartEeprom16(SEEP_ADDR_PUMP_LAG_TIME_AFTER_THERMOSTAT_CONTACT_DISCONNECTED_SEC)))
                {   // If time is over or app state is not suitable for circulation pump or heating element is aan, ga weer naar off
                    *secondCounter = 0;   // Reset timer  
                    TurnOffCirculationPump(); // Pump off
                    circulationPumpState = CIRCULATION_PUMP_STATE_OFF;
                }
                //while(!releaseLoggingLock());
            //}
            break;
        }
        
        // 4: Circulation pump has been off for too long
        case CIRCULATION_PUMP_STATE_TOO_LONG_IDLE:
        {   
            //if(setLoggingLock()){
                // Regardless of other situations just run
                //if (SecondCounterCirculationPump >= PUMP_ON_TIME_AFTER_OFF_TIME_REACHED_SEC)
                if (*secondCounter >= ReadSmartEeprom16(SEEP_ADDR_PUMP_ON_TIME_AFTER_OFF_TIME_REACHED_SEC))
                {   // 10 minutes over, so turn pump off and reset timer
                    *secondCounter = 0;   // Reset timer  
                    TurnOffCirculationPump(); // Pump off
                    circulationPumpState = CIRCULATION_PUMP_STATE_OFF;
                }
                //while(!releaseLoggingLock());
            //}
            break;
        }
        

        // 5: The temperature dropped below the setpoint minus the setting
        case CIRCULATION_PUMP_STATE_OFF_TOO_LOW_TEMP:
        {   
            //if(setLoggingLock()){
                if ((thermostatContact == false) || (canCirculationPumpBeOnInThisAppState(app_Data.appState) == false) || (heatingElement == true))
                {   // Thermostat contact goes to false, or App state is not suitable for circulation pump, or heating element has gone ON 
                    *secondCounter = 0; // Reset timer
                     TurnOffCirculationPump(); // Pump off
                    circulationPumpState = CIRCULATION_PUMP_STATE_OFF;
                }
                else if (bufferTemp > (heatingSetpoint - ReadSmartEeprom16(SEEP_ADDR_PUMP_OFF_TEMP_TOO_LOW) + ReadSmartEeprom16(SEEP_ADDR_PUMP_ON_TEMP_AFTER_TOO_LOW_TEMP)))
                {   // Temperature dropped below minimum for the pump to be on
                    *secondCounter = 0;   // Reset timer
                    TurnOnCirculationPump(); // Pump off
                    circulationPumpState = CIRCULATION_PUMP_STATE_ON; 
                }
                //while(!releaseLoggingLock());
            //}
            break;
        }
                
        
        // The default state should never be executed. 
        default:
        {
            // TODO: Handle error in application's state machine. 
            break;
        }
    }
}
