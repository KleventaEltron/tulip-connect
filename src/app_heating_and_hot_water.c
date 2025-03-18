/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_warm_water.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It
    implements the logic of the application's state machine and it may call
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include <string.h>
#include <stdio.h>
#include "definitions.h"                // SYS function prototypes

#include "app_heating_and_hot_water.h"
#include "files\ntc.h"
#include "files\modbus\heatpump_parameters.h"
#include "files\modbus\modbus.h"
#include "files\eeprom.h"
#include "files\i2c\mac.h"
#include "files\alarms.h"
#include "files\time_counters.h"
#include "files\heating_and_hot_water_tools.h"
#include "files\circulation_pump.h"
#include "files/logging.h"
#include "files/delay.h"
#include "files\hardware_rev.h"
#include "app_heatpump_comm.h"
#include "driver/gmac/drv_gmac.h"
#include "tcpip/tcpip.h"
#include "app_display_comm.h"
#include "app_logging_tasks.h"
#include "files/modbus/heatpump.h"
#include "files/modbus//display.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_WARM_WATER_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

APP_HEATING_AND_HOT_WATER_DATA app_Data;
extern APP_DISPLAY_COMM_DATA app_display_commData;
extern APP_HEATPUMP_COMM_DATA app_heatpump_commData;
extern APP_LOGGING_TASKS_DATA app_logging_tasksData;


//#define TRY_OUT_VALUE_COUNTERS 81000;

// Time counters, when MAX value the counter is off, when 0 counter is on
static uint32_t secondCounterHotWater   = UINT32_MAX;
static uint32_t secondCounterHeating    = UINT32_MAX;

static uint32_t secondCounterCirculationPump    = 0; 
//static uint32_t secondCounterCirculationPump    = TRY_OUT_VALUE_COUNTERS;    // Set already to 0, must always be on

static uint32_t secondCounterChangeSystem = UINT32_MAX;    
static uint32_t secondCounterResetInitSystemStuck = UINT32_MAX;    
static uint32_t secondCounterSetpointControl = 0;
static uint32_t secondCounterWaitForSensors = UINT32_MAX;  

static uint32_t secondCounterLegionella = UINT32_MAX;
//static uint32_t secondCounterLegionella = TRY_OUT_VALUE_COUNTERS;

static uint32_t secondCounterDays = 0;
//static uint32_t secondCounterDays = TRY_OUT_VALUE_COUNTERS;

static int16_t initialHeatingBufferTemp = TEMPERATURE_ALARM_VALUE;
static int16_t initialDefrostingBoilerTemp = TEMPERATURE_ALARM_VALUE;
static uint32_t sterilizationReachedTemperatureTimeStamp = UINT32_MAX;

static bool isCompressorRunning = false;

static bool dayPassedSinceLastSoftwareReset = false;

// Debug purpose:
char debugBuffer[4096];

// App state strings
static char * StringInit = "Init";
static char * StringWaitForSensors = "Wait for sensors";
static char * StringIdle = "Idle";

static char * StringInitHotWater = "Init hot water";
static char * StringHotWater = "Hot water";

static char * StringInitHeating = "Init heating";
static char * StringHeating = "Heating";

static char * StringInitSterilization = "Init sterilization";
static char * StringSterilization = "Sterilization";

static char * StringSwitch3WayValveAndGoHotWater = "Switch to hot water";
static char * StringSwitch3WayValveAndGoHeating = "Switch to heating";
static char * StringSwitch3WayValveAndGoIdle = "Switch to idle";
static char * StringSwitch3WayValveAndGoSterilization = "Switch to sterilization";

// Switch 3-way valve state strings
static char * StringSwitchValveInit = "Init";
static char * StringSwitchValveCheckIfSwitchingNeeded = "Check if valve needs to be switched";
static char * StringSwitchValveTurnOffHeatpump = "Turning off heatpump";
static char * StringSwitchValveWaitForPumpOff = "Wait for heatpump off";
static char * StringSwitchValveDelayBeforeSwitchingValve = "Delay before switching valve";
static char * StringSwitchValveSwitchValve = "Switching valve";
static char * StringSwitchValveDelayAfterSwitchingValve = "Delay after switching valve";
static char * StringSwitchValveTurnOnHeatpump = "Turn on heatpump";
static char * StringSwitchValveWaitForHeatpumpOn = "Wait for heatpump on";
static char * StringSwitchValveFinished = "Switching valve finished";

static char * StringUnknown = "Unknown";

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************

static void getAppDataVariables(void)
{

    app_Data.currentHotWaterBufferTemp = GetNtcTemperature(NTC_HOT_WATER_BUFFER);
    // if (TempNtc[NTC_HOT_WATER_BUFFER] != TEMPERATURE_ALARM_VALUE)
    //    app_Data.currentHotWaterBufferTemp = TempNtc[NTC_HOT_WATER_BUFFER]; // (*10)
    
    // Get Heating buffer temperature
    app_Data.currentHeatingBufferTemp = GetNtcTemperature(NTC_HEATING_BUFFER);
    // if (TempNtc[NTC_HEATING_BUFFER] != TEMPERATURE_ALARM_VALUE)
    //    app_Data.currentHeatingBufferTemp = TempNtc[NTC_HEATING_BUFFER]; // (*10)
    
    // Get Hot water setpoint out of smart eeprom
    int16_t setpointHotWater = ReadSmartEeprom16(SEEP_ADDR_HOT_WATER_SETPOINT);
    if (setpointHotWater != TEMPERATURE_ALARM_VALUE)
        app_Data.setpointHotWaterBufferTemp = (setpointHotWater * 10);
    
    // Get Heating setpoint out of smart eeprom
    int16_t setpointHeating = ReadSmartEeprom16(SEEP_ADDR_HEATING_SETPOINT);
    if (setpointHeating != TEMPERATURE_ALARM_VALUE)
        app_Data.setpointHeatingBufferTemp = (setpointHeating * 10);
        
    // Get delta hot water 
    if (UnitSystemParameters[ADDRESS_HOT_WATER_RETURN_DIFFERENCE - START_ADDRESS_UNIT_SYSTEM_PARAMETERS][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP] != TEMPERATURE_ALARM_VALUE)
        app_Data.deltaHotWaterBufferTemp = (UnitSystemParameters[ADDRESS_HOT_WATER_RETURN_DIFFERENCE - START_ADDRESS_UNIT_SYSTEM_PARAMETERS][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP] * 10);
    
    // Get delta heating
    if (UnitSystemParameters[ADDRESS_AIR_CONDITIONER_RETURN_DIFFERENCE - START_ADDRESS_UNIT_SYSTEM_PARAMETERS][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP] != TEMPERATURE_ALARM_VALUE)
        app_Data.deltaHeatingBufferTemp = (UnitSystemParameters[ADDRESS_AIR_CONDITIONER_RETURN_DIFFERENCE - START_ADDRESS_UNIT_SYSTEM_PARAMETERS][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP] * 10);
    
    // Get compressor operating frequency   
    app_Data.compressorOperatingFrequency = RealTimeData[ADDRESS_COMPRESSOR_OPERATING_FREQUENCY - START_ADDRESS_REAL_TIME_DATA][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP];
    
    // Get retour water temperature 
    if (RealTimeData[ADDRESS_RETURN_WATER_TEMPERATURE_T6 - START_ADDRESS_REAL_TIME_DATA][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP] != TEMPERATURE_ALARM_VALUE)
        app_Data.retourWaterTemperature = (RealTimeData[ADDRESS_RETURN_WATER_TEMPERATURE_T6 - START_ADDRESS_REAL_TIME_DATA][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP] * 10);
    
    // Sterilization Function
    app_Data.sterilizationFunction = UnitSystemParameterL[ADDRESS_HIGH_TEMPERATURE_STERILIZATION_FUNCTION - START_ADDRESS_UNIT_SYSTEM_PARAMETER_L][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP];
    
    // Sterilization Interval Days
    app_Data.sterilizationIntervalDays = UnitSystemParameterL[ADDRESS_STERILIZATION_INTERVAL_DAYS - START_ADDRESS_UNIT_SYSTEM_PARAMETER_L][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP];
    //app_Data.sterilizationIntervalDays = 2;
    
    // Sterilization Start Time
    app_Data.sterilizationStartTime = UnitSystemParameterL[ADDRESS_STERILIZATION_START_TIME - START_ADDRESS_UNIT_SYSTEM_PARAMETER_L][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP];
    
    // Sterilization Run Time
    app_Data.sterilizationRunTime = UnitSystemParameterL[ADDRESS_STERILIZATION_RUN_TIME - START_ADDRESS_UNIT_SYSTEM_PARAMETER_L][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP];
    
    // Sterilization Temperature
    if (UnitSystemParameterL[ADDRESS_STERILIZATION_TEMPERATURE_SETTING - START_ADDRESS_UNIT_SYSTEM_PARAMETER_L][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP] != TEMPERATURE_ALARM_VALUE)
        app_Data.sterilizationTemperature = (UnitSystemParameterL[ADDRESS_STERILIZATION_TEMPERATURE_SETTING - START_ADDRESS_UNIT_SYSTEM_PARAMETER_L][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP] * 10);
    
    // Current display time
    app_Data.currentDisplayTimeHours = (uint8_t)(UserParameters[ADDRESS_DISPLAY_TIME - START_ADDRESS_USER_PARAMETERS][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP] >> 8);
    app_Data.currentDisplayTimeMinutes = (uint8_t)UserParameters[ADDRESS_DISPLAY_TIME - START_ADDRESS_USER_PARAMETERS][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP];
}

static void updateTimerCounters(void)
{
    if (secondCounterHotWater >= 0 && secondCounterHotWater != UINT32_MAX)
        secondCounterHotWater++;

    if (secondCounterHeating >= 0 && secondCounterHeating != UINT32_MAX)
        secondCounterHeating++;

    if (secondCounterCirculationPump >= 0 && secondCounterCirculationPump != UINT32_MAX)
        secondCounterCirculationPump++;

    if (secondCounterChangeSystem >= 0 && secondCounterChangeSystem != UINT32_MAX)
        secondCounterChangeSystem++;
    
    if (secondCounterResetInitSystemStuck >= 0 && secondCounterResetInitSystemStuck != UINT32_MAX)
        secondCounterResetInitSystemStuck++;
    
    if (secondCounterSetpointControl >= 0 && secondCounterSetpointControl != UINT32_MAX)
        secondCounterSetpointControl++;
    
    if (secondCounterWaitForSensors >= 0 && secondCounterWaitForSensors != UINT32_MAX)
        secondCounterWaitForSensors++;
    
    if (secondCounterLegionella >= 0 && secondCounterLegionella != UINT32_MAX)
        secondCounterLegionella++;
   
}

const char * GetStringWithAppState(APP_HEATING_AND_HOT_WATER_STATES state)
{
    switch (state)
    {
        case (APP_HEATING_AND_HOT_WATER_STATE_INIT):return StringInit;break;
        case (APP_HEATING_AND_HOT_WATER_STATE_WAIT_FOR_SENSORS):return StringWaitForSensors;break;
        case (APP_HEATING_AND_HOT_WATER_STATE_IDLE):return StringIdle;break;
        
        case (APP_HEATING_AND_HOT_WATER_STATE_INIT_HOT_WATER):return StringInitHotWater;break;
        case (APP_HEATING_AND_HOT_WATER_STATE_HOT_WATER):return StringHotWater;break;
        
        case (APP_HEATING_AND_HOT_WATER_STATE_INIT_HEATING):return StringInitHeating;break;
        case (APP_HEATING_AND_HOT_WATER_STATE_HEATING):return StringHeating;break;
        
        case (APP_HEATING_AND_HOT_WATER_STATE_INIT_STERILIZATION):return StringInitSterilization;break;
        case (APP_HEATING_AND_HOT_WATER_STATE_STERILIZATION):return StringSterilization;break;
        
        case (APP_HEATING_AND_HOT_WATER_STATE_SWITCH_3_WAY_VALVE_TO_HOT_WATER_CIRCUIT_AND_GO_HOT_WATER_MODE):return StringSwitch3WayValveAndGoHotWater;break;
        case (APP_HEATING_AND_HOT_WATER_STATE_SWITCH_3_WAY_VALVE_TO_HEATING_CIRCUIT_AND_GO_HEATING_MODE):return StringSwitch3WayValveAndGoHeating;break;
        case (APP_HEATING_AND_HOT_WATER_STATE_SWITCH_3_WAY_VALVE_TO_HEATING_CIRCUIT_AND_GO_IDLE_MODE):return StringSwitch3WayValveAndGoIdle;break;
        case (APP_HEATING_AND_HOT_WATER_STATE_SWITCH_3_WAY_VALVE_TO_HOT_WATER_CIRCUIT_AND_GO_STERILIZATION_MODE):return StringSwitch3WayValveAndGoSterilization;break;
        
        default: return StringUnknown;break;
    }
}

const char * GetStringWithSwitch3WayValveState(SWITCH_3_WAY_VALVE_STATES state)
{
    switch (state)
    {
        case (SWITCH_3_WAY_VALVE_STATE_INIT):return StringSwitchValveInit;break;
        case (SWITCH_3_WAY_VALVE_STATE_CHECK_IF_SWITCHING_NEEDED):return StringSwitchValveCheckIfSwitchingNeeded;break;
        case (SWITCH_3_WAY_VALVE_STATE_TURN_OFF_HEATPUMP):return StringSwitchValveTurnOffHeatpump;break;
        case (SWITCH_3_WAY_VALVE_STATE_WAIT_FOR_PUMP_OFF):return StringSwitchValveWaitForPumpOff;break;
        case (SWITCH_3_WAY_VALVE_STATE_DELAY_BEFORE_SWITCHING_VALVE):return StringSwitchValveDelayBeforeSwitchingValve;break;
        case (SWITCH_3_WAY_VALVE_STATE_SWITCH_VALVE):return StringSwitchValveSwitchValve;break;
        case (SWITCH_3_WAY_VALVE_STATE_DELAY_AFTER_SWITCHING_VALVE):return StringSwitchValveDelayAfterSwitchingValve;break;
        case (SWITCH_3_WAY_VALVE_STATE_TURN_ON_HEATPUMP):return StringSwitchValveTurnOnHeatpump;break;
        case (SWITCH_3_WAY_VALVE_STATE_WAIT_FOR_HEATPUMP_ON):return StringSwitchValveWaitForHeatpumpOn;break;
        case (SWITCH_3_WAY_VALVE_STATE_FINISHED):return StringSwitchValveFinished;break;
        
        default: return StringUnknown;break;
    }
}

static void debugPrint(void)
{
    memset(debugBuffer, 0, sizeof(debugBuffer));
    
    
    //TCPIP_NET_HANDLE netH = TCPIP_STACK_NetHandleGet("eth0");
    //IPV4_ADDR ipAddr;
    //const uint8_t* macAddr;
    //ipAddr.Val = TCPIP_STACK_NetAddress(netH);
    //macAddr = TCPIP_STACK_NetAddressMac(netH);

    /*
    sprintf(debugBuffer, "\r\nInfo:\r\n  SN: %02x.%02x.%02x.%02x.%02x.%02x.%02x.%02x\r\n  FW: %d-%d-%d\r\n  REV:%d\r\n  RST:%d\r\n\
        \r\nHeatpump:\r\n  SP:   %d\r\n  Freq: %d\r\n  Flow: %d\r\n  Rtr:  %d\r\n\
        \r\nState: %d, %s\r\n\
        \r\nHeating:\r\n  Buffer:  %d\r\n  SP:      %d\r\n  Delta:   %d\r\n  Element: %s\r\n  Start:   %d\r\n  Counter: %d\r\n\
        \r\nHot water:\r\n  Buffer:  %d\r\n  SP:      %d\r\n  Offset:  %d\r\n  Delta:   %d\r\n  Element: %s\r\n  Counter: %d\r\n\
        \r\nCirculation pump:\r\n  State:   %d, %s\r\n  Counter: %d\r\n\
        \r\nSwitch 3-way valve:\r\n  State:   %d, %s\r\n  Counter: %d\r\n\
        \r\nLegionella:\r\n  Time:     %02d:%02d\r\n  Buffer:   %d\r\n  SP:       %d\r\n  Offset:   %d\r\n  Counter:  %d\r\n  St. cntr: %d\r\n  Day cntr: %d\r\n  Sec cntr: %d\
        \r\nDefrosting:\r\n  Status:   %s\r\n  St. temp: %d\r\n  Buffer:   %d\r\n  Element:  %s\r\n\
        \r\nLoggin:\r\n  IP ADDR: %d:%d:%d:%d\r\n  MAC ADDR:%02X:%02X:%02X:%02X:%02X:%02X\
        \r\n  TCP/IP reset count: %i\r\n  TCP/IP configured:  %s\r\n  Eth cable connected:%s\r\n  NETWORK is up:      %s\r\n\
        \r\n", 
     * 
     */
    sprintf(debugBuffer, "\r\nInfo:\r\n  SN: %02x.%02x.%02x.%02x.%02x.%02x.%02x.%02x\r\n  FW: %d-%d-%d\r\n  REV:%d\r\n  RST:%d\r\n\
        \r\nHeatpump:\r\n  SP:   %d\r\n  Freq: %d\r\n  Flow: %d\r\n  Rtr:  %d\r\n\
        \r\nHeating:\r\n  Buffer:  %d\r\n  SP:      %d\r\n  Delta:   %d\r\n  Element: %s\r\n  Start:   %d\r\n  Counter: %d\r\n\
        \r\nHot water:\r\n  Buffer:  %d\r\n  SP:      %d\r\n  Offset:  %d\r\n  Delta:   %d\r\n  Element: %s\r\n  Counter: %d\r\n\
        \r\nCirculation pump:\r\n  Counter: %d\r\n\
        \r\nSwitch 3-way valve:\r\n  Counter: %d\r\n\
        \r\nLegionella:\r\n  Time:     %02d:%02d\r\n  Buffer:   %d\r\n  SP:       %d\r\n  Offset:   %d\r\n  Counter:  %d\r\n  St. cntr: %d\r\n  Day cntr: %d\r\n  Sec cntr: %d\r\n\
        \r\nStates:\r\n HeatHowWater: %d, %s \r\n CircPump:     %d, %s \r\n 3WayValve:    %d, %s \r\n Display:      %s \r\n Heatpump:     %s \r\n Logging:      %s \r\n\
        \r\n",     
            eui64[0], eui64[1], eui64[2], eui64[3], eui64[4], eui64[5], eui64[6], eui64[7], 
            (int)((THIS_FIRMWARE_VERSION / 1000000)), (int)((THIS_FIRMWARE_VERSION / 1000) % 1000), (int)(THIS_FIRMWARE_VERSION % 1000),
            RevNum, (int)ReadSmartEeprom32(SEEP_ADDR_RESET_COUNTER),
            
            UserParameters[ADDRESS_HEATING_SET_TEMPERATURE - START_ADDRESS_USER_PARAMETERS][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP],
            app_Data.compressorOperatingFrequency,
            RealTimeData[ADDRESS_WATER_FLOW - START_ADDRESS_REAL_TIME_DATA][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP],
            app_Data.retourWaterTemperature,
            
            app_Data.currentHeatingBufferTemp,
            app_Data.setpointHeatingBufferTemp,
            app_Data.deltaHeatingBufferTemp,
            BooleanToOnOffString((int)getStatusHeatingElementHeatingBuffer()),
            initialHeatingBufferTemp,
            (int)secondCounterHeating,
            
            app_Data.currentHotWaterBufferTemp,
            app_Data.setpointHotWaterBufferTemp,
            app_Data.setpointHotWaterOffset,
            app_Data.deltaHotWaterBufferTemp,
            BooleanToOnOffString((int)getStatusHeatingElementHotWaterBuffer()),
            (int)secondCounterHotWater,
            
            (int)secondCounterCirculationPump,
            
            (int)secondCounterChangeSystem,
            
            app_Data.currentDisplayTimeHours,
            app_Data.currentDisplayTimeMinutes,
            app_Data.currentHotWaterBufferTemp,
            app_Data.sterilizationTemperature,
            app_Data.sterilizationTemperatureOffset,
            (int)secondCounterLegionella,
            (int)sterilizationReachedTemperatureTimeStamp,
            ReadSmartEeprom16(SEEP_ADDR_DAY_COUNTER_STERILIZATION),
            (int)secondCounterDays,
            
            app_Data.appState, GetStringWithAppState(app_Data.appState),
            GetCirculationPumpState(), GetStringWithCirculationPumpState(GetCirculationPumpState()),
            app_Data.switch3WayValveState, GetStringWithSwitch3WayValveState(app_Data.switch3WayValveState),
            getDisplayStateToString(app_display_commData.state),
            getHeatpumpStateToString(app_heatpump_commData.state),
            getLoggingStateToString(app_logging_tasksData.state)         
            //BooleanToOnOffString(CheckIfDefrostModeActive()),
            //initialDefrostingBoilerTemp,
            //app_Data.currentHotWaterBufferTemp,
            //BooleanToOnOffString((bool)getStatusHeatingElementHotWaterBuffer()),
            
            //ipAddr.v[0], ipAddr.v[1], ipAddr.v[2], ipAddr.v[3],
            //macAddr[0], macAddr[1], macAddr[2],macAddr[3], macAddr[4], macAddr[5],
            //getTcpIpResetCounter(),
            //((netH != NULL) ? "true" : "false"),
            //(TCPIP_STACK_NetIsLinked(netH) ? "true" : "false"),
            //(TCPIP_STACK_NetIsUp(netH) ? "true" : "false")
            //((GMAC_REGS->GMAC_NSR & GMAC_NSR_LINK_STATUS_Msk) ?  "true" : "false"),
            
            );

    SYS_DEBUG_PRINT(SYS_ERROR_ERROR, debugBuffer);
}

APP_HEATING_AND_HOT_WATER_STATES checkHotWaterStateInHeatingMode(bool heatingElementStatus, uint32_t secondCounter, int16_t setpoint, int16_t currentTemp, int16_t delta, APP_HEATING_AND_HOT_WATER_STATES currentState)
{
    // Go back to Hot water mode if:
    // - Hot water element is still on after 2 hours
    // - Hot water element is off, and buffer temp decreases under setpoint - delta
    // If need to go back to Hot water mode, turn off circulation pump and heating element heating
    // If hot water reached setpoint, turn off element
    
    if ((heatingElementStatus == true) && (secondCounter != UINT32_MAX))
    {   // Element is on and counter still running, so hot water is still working

        if (CheckIfSetpointReached(setpoint, currentTemp) == true)
        {   // Hot water checkpoint reached with heating element only, turn off element and reset timer
            app_Data.setpointHotWaterOffset = 0;
            TurnOffHeatingElementHotWaterBuffer();  // Turn off heating element hot water buffer
            secondCounterHotWater = UINT32_MAX; // Reset hot water timer
        } 
        //else if (secondCounter >= HOT_WATER_MAX_TIME_HEATING_ELEMENT_ON_IN_HEATING_MODE_SEC)
        else if (secondCounter >= ReadSmartEeprom16(SEEP_ADDR_HOT_WATER_MAX_TIME_HEATING_ELEMENT_ON_IN_HEATING_MODE_SEC))
        {   // 2 hours had passed with element on, go back to hot water mode
            currentState = APP_HEATING_AND_HOT_WATER_STATE_SWITCH_3_WAY_VALVE_TO_HOT_WATER_CIRCUIT_AND_GO_HOT_WATER_MODE; // Go back to hot water mode
        }
        else {} // Do nothing
    }
    else
    {   // Hot water is idle, check if buffer temp decreases under setpoint-delta
        
        if (CheckIfTemperatureIsBelowSetpointMinusDelta(setpoint, currentTemp, delta) == true)
        {   // Hot water is off, but current temp has decreased below current temp - delta
            currentState = APP_HEATING_AND_HOT_WATER_STATE_SWITCH_3_WAY_VALVE_TO_HOT_WATER_CIRCUIT_AND_GO_HOT_WATER_MODE; // Go back to hot water mode
        }
    } 
    
    return currentState;
}

static void HeatpumpSetpointControl(APP_HEATING_AND_HOT_WATER_STATES currentState, int16_t setpointHotWater, int16_t setpointHotWaterOffset, int16_t setpointHeatingBuffer, uint16_t setpointInHeatpump, int16_t setpointSterilization, int16_t setpointSterilizationOffset)
{
    if (secondCounterSetpointControl > 10)
    {   // 10 seconds over, check if setpoint in heatpump is the correct one for this app state.
        secondCounterSetpointControl = 0;
        
        uint16_t correctSetpoint = (uint16_t)DetermineCorrectSetpoint(currentState, setpointHotWater, setpointHeatingBuffer, setpointHotWaterOffset, setpointSterilization, setpointSterilizationOffset);
    
        if (correctSetpoint != (UserParameters[ADDRESS_HEATING_SET_TEMPERATURE - START_ADDRESS_USER_PARAMETERS][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP] * 10))
        {   // Setpoint in heatpump is not correct, send the correct one
            ChangeHeatpumpSetting(ADDRESS_HEATING_SET_TEMPERATURE, (correctSetpoint / 10));
        }
    }
}

static int16_t adjustSetpointOffsetInHeatpumpIfNeeded(int16_t hotWaterBufferTemp, int16_t actualSetpointHotWater, int16_t retourTemperature, int16_t offset)
{
    if (hotWaterBufferTemp >= actualSetpointHotWater)
    {   // NTC in hot water buffer is equal of bigger than the actual setpoint
        offset = 0; // Set offset to 0, this means the setpoint has been reached
    }
    else
    {   // NTC in hot water buffer not reached actual setpoint
        if ((retourTemperature >= (actualSetpointHotWater + offset - 20)) && (offset != 0) && (offset != TEMPERATURE_ALARM_VALUE))
        {   // Retour water temperature has come within 2 degree celcius of setpoint
            offset += ReadSmartEeprom16(SEEP_ADDR_HOT_WATER_SETPOINT_OFFSET_STEPS);  // Increase offset with 2 degree Celcius
        }
        else{} // Retour water temperature has not come within 2 degree Celcius of setpoint
    }
    
    return offset;
}

void systemSoftwareReset(uint32_t secondCounterDays)
{
    WriteSmartEeprom32(SEEP_ADDR_SECONDS_COUNTER_DAYS, secondCounterDays);
    SYS_RESET_SoftwareReset();
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_WARM_WATER_Initialize ( void )

  Remarks:
    See prototype in app_warm_water.h.
 */

void APP_HEATING_AND_HOT_WATER_Initialize ( void )
{
    //SmartEepromInit();
    //SYS_CONSOLE_PRINT("\r\n STATE >> %d, %s\r\n", app_Data.appState, GetStringWithAppState(app_Data.appState));
    
    SetDataInArraysAtStartup();
    
    app_Data.setpointHotWaterBufferTemp = TEMPERATURE_ALARM_VALUE;
    app_Data.setpointHeatingBufferTemp = TEMPERATURE_ALARM_VALUE;
    app_Data.setpointHotWaterOffset = TEMPERATURE_ALARM_VALUE;
    
    app_Data.currentHotWaterBufferTemp = TEMPERATURE_ALARM_VALUE;
    app_Data.currentHeatingBufferTemp = TEMPERATURE_ALARM_VALUE;
    
    app_Data.deltaHotWaterBufferTemp = TEMPERATURE_ALARM_VALUE;
    app_Data.deltaHeatingBufferTemp = TEMPERATURE_ALARM_VALUE;
   
    
    
    
    app_Data.appState = APP_HEATING_AND_HOT_WATER_STATE_INIT;
    //app_Data.appState = APP_HEATING_AND_HOT_WATER_STATE_SWITCH_3_WAY_VALVE_TO_HOT_WATER_CIRCUIT_AND_GO_HOT_WATER_MODE;
    
    
    
    
    app_Data.switch3WayValveState = SWITCH_3_WAY_VALVE_STATE_INIT;
    
    app_Data.compressorOperatingFrequency = UINT16_MAX;
    app_Data.retourWaterTemperature = TEMPERATURE_ALARM_VALUE;
    
    app_Data.sterilizationFunction = UINT16_MAX;
    app_Data.sterilizationIntervalDays = UINT16_MAX;
    app_Data.sterilizationStartTime = UINT16_MAX;
    app_Data.sterilizationRunTime = UINT16_MAX;
    app_Data.sterilizationTemperature = TEMPERATURE_ALARM_VALUE;
    app_Data.sterilizationTemperatureOffset = TEMPERATURE_ALARM_VALUE;
    app_Data.currentDayCount = UINT16_MAX;
    app_Data.currentDisplayTimeHours = UINT8_MAX;
    app_Data.currentDisplayTimeMinutes = UINT8_MAX;
    
    secondCounterDays = ReadSmartEeprom32(SEEP_ADDR_SECONDS_COUNTER_DAYS);
    //secondCounterDays = TRY_OUT_VALUE_COUNTERS;
    
    Switch3WayValveToHeating();
    TurnOffHeatingElementHotWaterBuffer();
    TurnOffHeatingElementHeatingBuffer();
    
    CirculationPumpInit(); 
}


/******************************************************************************
  Function:
    void APP_HEATING_AND_HOT_WATER_Tasks ( void )

  Remarks:
    See prototype in app_heating_and_hot_water.h.
 */

void APP_HEATING_AND_HOT_WATER_Tasks ( void )
{
    
    if (HeatingHotWaterTimerExpired() == true)
    {   // Every second
        
        // Get APP data variables
        //if(!setLoggingLock()){
        //    return;
        //}
        
        getAppDataVariables();
        // Update counting timers:
        updateTimerCounters();
        //while(!releaseLoggingLock());
        
        // Circulation pump control:
        CirculationPumpControl(app_Data.appState, 
            GetThermostatContact(), 
            getStatusHeatingElementHeatingBuffer(), 
            &secondCounterCirculationPump,
            app_Data.setpointHeatingBufferTemp,
            app_Data.currentHeatingBufferTemp,
            CheckIfDefrostModeActive());
        
        isCompressorRunning = CheckIfCompressorIsRunning(app_Data.compressorOperatingFrequency);
        
        if (DebugDipSwitch() == true)
        {
            //if(!setLoggingLock()){
            //    return;
            //}
            debugPrint();
            //while(!releaseLoggingLock());
        }
        

        HeatpumpSetpointControl(app_Data.appState, 
            app_Data.setpointHotWaterBufferTemp,
            app_Data.setpointHotWaterOffset,
            app_Data.setpointHeatingBufferTemp, 
            UserParameters[ADDRESS_HEATING_SET_TEMPERATURE - START_ADDRESS_USER_PARAMETERS][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP],
            app_Data.sterilizationTemperature,
            app_Data.sterilizationTemperatureOffset);
        
        if (IsSterilizationRunningInBackground(secondCounterLegionella, getStatusHeatingElementHotWaterBuffer(), app_Data.appState) == true)
        {   // Second counter legionella is running, and heating element is on, so legionella active on background
            if (app_Data.currentHotWaterBufferTemp >= app_Data.sterilizationTemperature)
            {   // Hot water buffer reached sterilization temperature

                if (sterilizationReachedTemperatureTimeStamp == UINT32_MAX)
                {   // Time not yet stamped
                    sterilizationReachedTemperatureTimeStamp = secondCounterLegionella;
                }

                if (secondCounterLegionella >= (sterilizationReachedTemperatureTimeStamp + (app_Data.sterilizationRunTime * 60)))
                {   // Time reached (sterilization finished)
                    secondCounterLegionella = UINT32_MAX;
                    TurnOffHeatingElementHotWaterBuffer();
                    sterilizationReachedTemperatureTimeStamp = UINT32_MAX;

                    WriteSmartEeprom16(SEEP_ADDR_DAY_COUNTER_STERILIZATION, 0);
                    secondCounterDays = 0;
                }
            }
        }
        
        if (secondCounterDays >= SECONDS_IN_DAY)
        {   // Day passed
            secondCounterDays = 0;
            
            // Increase day counter in eeprom
            uint16_t dayCounter = ReadSmartEeprom16(SEEP_ADDR_DAY_COUNTER_STERILIZATION);
            dayCounter++;
            WriteSmartEeprom16(SEEP_ADDR_DAY_COUNTER_STERILIZATION, dayCounter);
            
            dayPassedSinceLastSoftwareReset = true;
            
            if (IsSoftwareResetPossible(app_Data.appState, secondCounterLegionella) == true)
            {   // Can so software reset
                systemSoftwareReset(secondCounterDays);
            }   
        }
        else
        {   // No day passed
            if (secondCounterDays >= 0 && secondCounterDays != UINT32_MAX)
            {   // secondCounterDays is running
                secondCounterDays++;
            }  
        }
        
        if (dayPassedSinceLastSoftwareReset == true)
        {   // If a day has passed but the software reset was not possible at first
            if (IsSoftwareResetPossible(app_Data.appState, secondCounterLegionella) == true)
            {   // Software reset is now possible
                systemSoftwareReset(secondCounterDays);
            }
        }
    }
    
    // Statemachine:
    switch (app_Data.appState)
    {   
//       ###                
//        #  #    # # ##### 
//        #  ##   # #   #   
//        #  # #  # #   #   
//        #  #  # # #   #   
//        #  #   ## #   #   
//       ### #    # #   #  
        
        // 0: Init state, only ones
        case APP_HEATING_AND_HOT_WATER_STATE_INIT:
        { 
            secondCounterWaitForSensors = 0;
            app_Data.appState = APP_HEATING_AND_HOT_WATER_STATE_WAIT_FOR_SENSORS;
            break;
        }
        
        // 1: Init state, only ones
        case APP_HEATING_AND_HOT_WATER_STATE_WAIT_FOR_SENSORS:
        {
            
            if (secondCounterWaitForSensors < 10)
            {   // 10 seconds has nog yet passed
                if (app_Data.currentHeatingBufferTemp != TEMPERATURE_ALARM_VALUE)
                {   // Heating sensor reads normal value
                    if (app_Data.currentHotWaterBufferTemp != TEMPERATURE_ALARM_VALUE)
                    {   // Hot water sensor reads normal value, so had heating and hot water
                        //if(setLoggingLock()){
                            WriteSmartEeprom16(SEEP_ADDR_HEATPUMP_MODE, SET_MODE_HOT_WATER_AND_HEATING); 
                            //while(!releaseLoggingLock()){}
                            secondCounterWaitForSensors = UINT32_MAX;
                            app_Data.appState = APP_HEATING_AND_HOT_WATER_STATE_IDLE;
                        //}
                    }
                    else
                    {   // Hot water sensor does not read normal value, so only heating
                        //if(setLoggingLock()){
                            WriteSmartEeprom16(SEEP_ADDR_HEATPUMP_MODE, SET_MODE_HEATING); 
                            //while(!releaseLoggingLock()){}
                            secondCounterWaitForSensors = UINT32_MAX;
                            app_Data.appState = APP_HEATING_AND_HOT_WATER_STATE_IDLE;
                        //}
                    }
                }
            }
            else
            {   // 10 seconds passed, go to next state.
                secondCounterWaitForSensors = UINT32_MAX;
                app_Data.appState = APP_HEATING_AND_HOT_WATER_STATE_IDLE;
            }
            
            break;
        }
        
//      ###                      
//       #  #####  #      ###### 
//       #  #    # #      #      
//       #  #    # #      #####  
//       #  #    # #      #      
//       #  #    # #      #      
//      ### #####  ###### ###### 
                          
        // 2: Wait till something has to be done
        case APP_HEATING_AND_HOT_WATER_STATE_IDLE:
        {   
            //SYS_CONSOLE_PRINT("idle state\r\n");
            //if(!setLoggingLockNoPrint()){			
			//	break;
			//}
            
            //if (IsSterilizationActive(app_Data.appState) == false)
            //    app_Data.appState = APP_HEATING_AND_HOT_WATER_STATE_SWITCH_3_WAY_VALVE_TO_HOT_WATER_CIRCUIT_AND_GO_STERILIZATION_MODE;
            
            if (IsSterilizationRequired(app_Data.sterilizationFunction, 
                app_Data.sterilizationIntervalDays, 
                app_Data.sterilizationStartTime,
                app_Data.currentHotWaterBufferTemp,
                app_Data.currentDisplayTimeHours,
                app_Data.currentDisplayTimeMinutes,   
                ReadSmartEeprom16(SEEP_ADDR_DAY_COUNTER_STERILIZATION),
                secondCounterLegionella,
                ReadSmartEeprom16(SEEP_ADDR_STERILIZATION_MAX_TIME_OUT_OF_STERILIZATION_MODE_WITH_ELEMENT_ON),
                getStatusHeatingElementHotWaterBuffer()   
                ) == true)
            {
                app_Data.appState = APP_HEATING_AND_HOT_WATER_STATE_SWITCH_3_WAY_VALVE_TO_HOT_WATER_CIRCUIT_AND_GO_STERILIZATION_MODE;
            }
            else if ((CheckIfTemperatureIsBelowSetpointMinusDelta(app_Data.setpointHotWaterBufferTemp, app_Data.currentHotWaterBufferTemp, app_Data.deltaHotWaterBufferTemp) == true) &&
                     (IsSterilizationRunningInBackground(secondCounterLegionella, getStatusHeatingElementHotWaterBuffer(), app_Data.appState) == false))
            {   // Hot water buffer temperature had decreased below setpoint - delta, and sterilization is not active
                app_Data.appState = APP_HEATING_AND_HOT_WATER_STATE_SWITCH_3_WAY_VALVE_TO_HOT_WATER_CIRCUIT_AND_GO_HOT_WATER_MODE;
            }
            else
            {    
                if (getStatus3WayValve() == VALVE_IS_ON_HOT_WATER_CIRCUIT)
                {   // 3 Way valve is still on hot water circuit, must be on the heating circuit in idle mode.
                    app_Data.appState = APP_HEATING_AND_HOT_WATER_STATE_SWITCH_3_WAY_VALVE_TO_HEATING_CIRCUIT_AND_GO_IDLE_MODE;
                    break;
                }
                
                if ((CheckIfCompressorIsRunning(app_Data.compressorOperatingFrequency) == true) &&
                    (CheckIfDefrostModeActive() == false))
                {   // Compressor is running for some seconds, go to heating state
                    app_Data.appState = APP_HEATING_AND_HOT_WATER_STATE_SWITCH_3_WAY_VALVE_TO_HEATING_CIRCUIT_AND_GO_HEATING_MODE;
                    break;
                }
            }   
            
            //while(!releaseLoggingLockNoPrint());
            break;
        }
        
//        #     #                                                   
//        #     #  ####  #####    #    #   ##   ##### ###### #####  
//        #     # #    #   #      #    #  #  #    #   #      #    # 
//        ####### #    #   #      #    # #    #   #   #####  #    # 
//        #     # #    #   #      # ## # ######   #   #      #####  
//        #     # #    #   #      ##  ## #    #   #   #      #   #  
//        #     #  ####    #      #    # #    #   #   ###### #    #
        
        // 3: start hot water heating 
        case APP_HEATING_AND_HOT_WATER_STATE_INIT_HOT_WATER:
        {   
            if (getStatus3WayValve() != VALVE_IS_ON_HOT_WATER_CIRCUIT)
            {   // 3 Way valve is still not on hot water mode, must be on hot water circuit 
                app_Data.appState = APP_HEATING_AND_HOT_WATER_STATE_SWITCH_3_WAY_VALVE_TO_HOT_WATER_CIRCUIT_AND_GO_HOT_WATER_MODE;
                break;
            }
            
            //if(setLoggingLock()){
                TurnOffHeatingElementHotWaterBuffer();
                secondCounterHotWater = 0;
                app_Data.setpointHotWaterOffset = ReadSmartEeprom16(SEEP_ADDR_HOT_WATER_SETPOINT_OFFSET_START);   // Set offset of 5 degree to setpoint of hot water
                //while(!releaseLoggingLock()){}
                app_Data.appState = APP_HEATING_AND_HOT_WATER_STATE_HOT_WATER;
            //}
            break;
        }
        
        // 4: Heatpump is now heating the hot water buffer till checkpoint
        case APP_HEATING_AND_HOT_WATER_STATE_HOT_WATER:
        {   
            //if(!setLoggingLock()){
            //    break;
            //}

            if(CheckIfDefrostModeActive() == true)
            {   // Heatpump is doing defrosting
                if (initialDefrostingBoilerTemp == TEMPERATURE_ALARM_VALUE)
                    initialDefrostingBoilerTemp = app_Data.currentHotWaterBufferTemp;
                
                if (app_Data.currentHotWaterBufferTemp <= (initialDefrostingBoilerTemp - ReadSmartEeprom16(SEEP_ADDR_DEFROSTING_TEMP_FALL_BEFORE_ELEMENT_ON)))
                {   // Temperature falled 4 degrees Celcius beneath de defrosting starting temperature
                    TurnOnHeatingElementHotWaterBuffer();
                }
                else if (app_Data.currentHotWaterBufferTemp >= 
                        (initialDefrostingBoilerTemp - 
                        ReadSmartEeprom16(SEEP_ADDR_DEFROSTING_TEMP_FALL_BEFORE_ELEMENT_ON)+ 
                        ReadSmartEeprom16(SEEP_ADDR_DEFROSTING_TEMP_RISE_BEFORE_ELEMENT_OFF)))
                {   // Temperature rised to 1 degree beneath de defrosting starting temperature
                    TurnOffHeatingElementHotWaterBuffer();
                }
                else{}
            }
            else
            {   // Heatpump is NOT doing defrosting
                if (initialDefrostingBoilerTemp != TEMPERATURE_ALARM_VALUE)
                {   // Just came out of defrosting mode
                    if (getStatusHeatingElementHotWaterBuffer() == true)
                    {   // Hot water element is on
                        if (app_Data.currentHotWaterBufferTemp >= 
                            (initialDefrostingBoilerTemp - 
                            ReadSmartEeprom16(SEEP_ADDR_DEFROSTING_TEMP_FALL_BEFORE_ELEMENT_ON)+ 
                            ReadSmartEeprom16(SEEP_ADDR_DEFROSTING_TEMP_RISE_BEFORE_ELEMENT_OFF)))
                        {   // Temperature falled 3 degrees Celcius beneath de defrosting starting temperature
                            TurnOffHeatingElementHotWaterBuffer();
                            initialDefrostingBoilerTemp = TEMPERATURE_ALARM_VALUE;
                        }                     
                    }
                    else
                    {   // Hot water element is already off
                        initialDefrostingBoilerTemp = TEMPERATURE_ALARM_VALUE;  // Reset initial defrosting temperature
                    }
                    //while(!releaseLoggingLock());
                    return;
                }
                
                
                if (IsSterilizationRequired(app_Data.sterilizationFunction, 
                    app_Data.sterilizationIntervalDays, 
                    app_Data.sterilizationStartTime,
                    app_Data.currentHotWaterBufferTemp,
                    app_Data.currentDisplayTimeHours,
                    app_Data.currentDisplayTimeMinutes, 
                    ReadSmartEeprom16(SEEP_ADDR_DAY_COUNTER_STERILIZATION),
                    secondCounterLegionella,
                    ReadSmartEeprom16(SEEP_ADDR_STERILIZATION_MAX_TIME_OUT_OF_STERILIZATION_MODE_WITH_ELEMENT_ON),
                    getStatusHeatingElementHotWaterBuffer()   
                    ) == true)
                {
                    TurnOffHeatingElementHotWaterBuffer();  // If heating element was on, turn it off
                    secondCounterHotWater = UINT32_MAX;     // Set timer off
                    app_Data.setpointHotWaterOffset = 0;
                    initialDefrostingBoilerTemp = TEMPERATURE_ALARM_VALUE;
                    app_Data.appState = APP_HEATING_AND_HOT_WATER_STATE_SWITCH_3_WAY_VALVE_TO_HOT_WATER_CIRCUIT_AND_GO_STERILIZATION_MODE;
                    //while(!releaseLoggingLock());
                    break;
                }

                app_Data.setpointHotWaterOffset = adjustSetpointOffsetInHeatpumpIfNeeded
                        (app_Data.currentHotWaterBufferTemp, 
                        app_Data.setpointHotWaterBufferTemp, 
                        app_Data.retourWaterTemperature, 
                        app_Data.setpointHotWaterOffset);

                if (secondCounterHotWater >= ReadSmartEeprom16(SEEP_ADDR_HOT_WATER_MIN_TIME_IN_HOT_WATER_MODE))
                {
                    //if ((CheckIfRetourTemperatureReachedSetpoint(app_Data.retourWaterTemperature, app_Data.setpointHotWaterBufferTemp) == true) &&
                    //if ((CheckIfRetourTemperatureReachedSetpoint(app_Data.currentHotWaterBufferTemp, app_Data.setpointHotWaterBufferTemp) == true) &&
                    //if ((CheckIfTemperatureIsBelowSetpointMinusDelta(app_Data.setpointHotWaterBufferTemp, app_Data.retourWaterTemperature, app_Data.deltaHotWaterBufferTemp) == false) &&
                    if((CheckIfCompressorIsRunning(app_Data.compressorOperatingFrequency) == false) && 
                        (CheckIfDefrostModeActive() == false))
                    {   // Hot water setpoint reached, go back to idle mode
                        TurnOffHeatingElementHotWaterBuffer();  // If heating element was on, turn it off
                        secondCounterHotWater = UINT32_MAX;     // Set timer off
                        app_Data.setpointHotWaterOffset = 0;
                        initialDefrostingBoilerTemp = TEMPERATURE_ALARM_VALUE;
                        app_Data.appState = APP_HEATING_AND_HOT_WATER_STATE_SWITCH_3_WAY_VALVE_TO_HEATING_CIRCUIT_AND_GO_IDLE_MODE;
                        //while(!releaseLoggingLock());
                        break;
                    }
                    else
                    {   // Setpoint not (yet) reached
                        if (getStatusHeatingElementHotWaterBuffer() == false)
                        {   // Heating element is off
                            if (secondCounterHotWater >= ReadSmartEeprom16(SEEP_ADDR_HOT_WATER_RUNNING_TIME_BEFORE_TURNING_ON_HEATING_ELEMENT))
                            {   // 2 hours passed
                                secondCounterHotWater = UINT32_MAX;     // Set timer off
                                TurnOnHeatingElementHotWaterBuffer();   // Set heating element on
                            }
                            else{} // Wait till setpoint reached or 2 hours passed
                        }
                        else
                        {   // Heating element is on
                            if (GetThermostatContact() == true)
                            {   // Thermostat contact has been made
                                secondCounterHotWater = 0; // Start hot water timer
                                initialDefrostingBoilerTemp = TEMPERATURE_ALARM_VALUE;
                                app_Data.appState = APP_HEATING_AND_HOT_WATER_STATE_SWITCH_3_WAY_VALVE_TO_HEATING_CIRCUIT_AND_GO_HEATING_MODE; // Go to heating
                                //while(!releaseLoggingLock());
                                break;
                            }
                            else{} // Wait till setpoint reached
                        } 
                    }
                }
            }
            //while(!releaseLoggingLock());
            break;
        }
        
//        #     #                                     
//        #     # ######   ##   ##### # #    #  ####  
//        #     # #       #  #    #   # ##   # #    # 
//        ####### #####  #    #   #   # # #  # #      
//        #     # #      ######   #   # #  # # #  ### 
//        #     # #      #    #   #   # #   ## #    # 
//        #     # ###### #    #   #   # #    #  ####
        
        // 5: start heating heating buffer
        case APP_HEATING_AND_HOT_WATER_STATE_INIT_HEATING:
        {   
            if (getStatus3WayValve() != VALVE_IS_ON_HEATING_CIRCUIT)
            {   // 3 Way valve is still not on heating mode, must be on heating circuit 
                app_Data.appState = APP_HEATING_AND_HOT_WATER_STATE_SWITCH_3_WAY_VALVE_TO_HEATING_CIRCUIT_AND_GO_HEATING_MODE;
                break;
            }
            
            //if(setLoggingLock()){
                TurnOffHeatingElementHeatingBuffer();
                //while(!releaseLoggingLock());
                secondCounterHeating = 0;
                initialHeatingBufferTemp = app_Data.currentHeatingBufferTemp;
                app_Data.appState = APP_HEATING_AND_HOT_WATER_STATE_HEATING;
            //}
            break;
        }
        
        // 6: Check if setpoint is reached, and temperature must rise minimal 1 degree Celcius per 100 minutes
        case APP_HEATING_AND_HOT_WATER_STATE_HEATING:
        {               
            //if(!setLoggingLock()){
            //    break;
            //}
            
            if (IsSterilizationRequired(app_Data.sterilizationFunction, 
                app_Data.sterilizationIntervalDays, 
                app_Data.sterilizationStartTime,
                app_Data.currentHotWaterBufferTemp,
                app_Data.currentDisplayTimeHours,
                app_Data.currentDisplayTimeMinutes, 
                ReadSmartEeprom16(SEEP_ADDR_DAY_COUNTER_STERILIZATION),
                secondCounterLegionella,
                ReadSmartEeprom16(SEEP_ADDR_STERILIZATION_MAX_TIME_OUT_OF_STERILIZATION_MODE_WITH_ELEMENT_ON),
                getStatusHeatingElementHotWaterBuffer()   
                ) == true)
            {
                TurnOffHeatingElementHeatingBuffer();   // If heating element was on, turn it off
                TurnOffHeatingElementHotWaterBuffer();
                secondCounterHeating = UINT32_MAX;      // Set timer off
                secondCounterHotWater = UINT32_MAX;  // Reset hot water timer
                initialHeatingBufferTemp = TEMPERATURE_ALARM_VALUE;
                app_Data.appState = APP_HEATING_AND_HOT_WATER_STATE_SWITCH_3_WAY_VALVE_TO_HOT_WATER_CIRCUIT_AND_GO_STERILIZATION_MODE;
                //while(!releaseLoggingLock());
                break;
            }
            
            // Go back to Hot water mode if:
            // - Hot water element is still on after 2 hours
            // - Hot water element is off, and buffer temp decreases under setpoint - delta
            // If need to go back to Hot water mode, turn off circulation pump and heating element heating
            // If hot water reached setpoint, turn off element
            if (checkHotWaterStateInHeatingMode
                    (getStatusHeatingElementHotWaterBuffer(), 
                     secondCounterHotWater, 
                     app_Data.setpointHotWaterBufferTemp, 
                     app_Data.currentHotWaterBufferTemp, 
                     app_Data.deltaHotWaterBufferTemp,
                     app_Data.appState) == APP_HEATING_AND_HOT_WATER_STATE_SWITCH_3_WAY_VALVE_TO_HOT_WATER_CIRCUIT_AND_GO_HOT_WATER_MODE) 
            {   // Hot water must be set back on, because hot water element is still on after 2 hours, or buffer temp decreased under setpoint delta (with heating element off)
                if (IsSterilizationRunningInBackground(secondCounterLegionella, getStatusHeatingElementHotWaterBuffer(), app_Data.appState) == false)
                {
                    secondCounterHeating = UINT32_MAX;  // Reset heating timer
                    secondCounterHotWater = UINT32_MAX;  // Reset hot water timer
                    //LedAlarm_Toggle();
                    initialHeatingBufferTemp = TEMPERATURE_ALARM_VALUE; // Reset initial buffer temperature
                    TurnOffHeatingElementHeatingBuffer();   // If heating element was on, turn it off   
                    app_Data.appState = APP_HEATING_AND_HOT_WATER_STATE_SWITCH_3_WAY_VALVE_TO_HOT_WATER_CIRCUIT_AND_GO_HOT_WATER_MODE; // Go to hot water mode
                    //while(!releaseLoggingLock());
                    break;
                }
            }
            else
            {   // Stay here, 
                
            }
            
            //if ((CheckIfRetourTemperatureReachedSetpoint(app_Data.retourWaterTemperature, app_Data.setpointHeatingBufferTemp) == true) &&
            //if ((CheckIfRetourTemperatureReachedSetpoint(app_Data.currentHeatingBufferTemp, app_Data.setpointHotWaterBufferTemp) == true) &&
            //if ((CheckIfTemperatureIsBelowSetpointMinusDelta(app_Data.setpointHeatingBufferTemp, app_Data.retourWaterTemperature, app_Data.deltaHeatingBufferTemp) == false) &&
            if((CheckIfCompressorIsRunning(app_Data.compressorOperatingFrequency) == false) && 
                (CheckIfDefrostModeActive() == false))
            {   // Compressor stopped running, so checkpoint reached
                TurnOffHeatingElementHeatingBuffer();   // If heating element was on, turn it off
                secondCounterHeating = UINT32_MAX;      // Set timer off
                initialHeatingBufferTemp = TEMPERATURE_ALARM_VALUE;
                app_Data.appState = APP_HEATING_AND_HOT_WATER_STATE_SWITCH_3_WAY_VALVE_TO_HEATING_CIRCUIT_AND_GO_IDLE_MODE;
                //while(!releaseLoggingLock());
                break;
            }
            else
            {   // Compressor still running, so setpoint not reached
                if (getStatusHeatingElementHeatingBuffer() == false)
                {
                    if (TemperatureRisedEnough(&secondCounterHeating, app_Data.currentHeatingBufferTemp, &initialHeatingBufferTemp, ReadSmartEeprom16(SEEP_ADDR_HEATING_RISE_TEMP_IN_GIVEN_TIME), ReadSmartEeprom16(SEEP_ADDR_HEATING_TIME_CONSTANT_SEC)) == false)
                    {   // 100 minutes passed
                        secondCounterHeating = 0; // Reset timer
                        initialHeatingBufferTemp = app_Data.currentHeatingBufferTemp;   // Reset initial buffer temperature
                        TurnOnHeatingElementHeatingBuffer();    // Turn heating element on
                    }
                    else{} // Wait till setpoint reached (Temperature must rise minimal 1 degree Celcius per 100 mintues)
                }
                else
                {
                    if (TemperatureRisedEnough(&secondCounterHeating, app_Data.currentHeatingBufferTemp, &initialHeatingBufferTemp, ReadSmartEeprom16(SEEP_ADDR_HEATING_RISE_TEMP_IN_GIVEN_TIME), ReadSmartEeprom16(SEEP_ADDR_HEATING_TIME_CONSTANT_SEC)) == false)
                    {   // 100 minutes passed again (with heating element on)
                        secondCounterHeating = UINT32_MAX;
                        initialHeatingBufferTemp = TEMPERATURE_ALARM_VALUE;
                        TurnOffHeatingElementHeatingBuffer();

                        app_Data.appState = APP_HEATING_AND_HOT_WATER_STATE_SWITCH_3_WAY_VALVE_TO_HEATING_CIRCUIT_AND_GO_IDLE_MODE;
                        //while(!releaseLoggingLock());
                        break;
                    }
                    else{} // Wait till setpoint reached or thermostat contact has been made
                }
            }
            //while(!releaseLoggingLock());
            break;
        }
        
//       #                                                                 
//       #       ######  ####  #  ####  #    # ###### #      #        ##   
//       #       #      #    # # #    # ##   # #      #      #       #  #  
//       #       #####  #      # #    # # #  # #####  #      #      #    # 
//       #       #      #  ### # #    # #  # # #      #      #      ###### 
//       #       #      #    # # #    # #   ## #      #      #      #    # 
//       ####### ######  ####  #  ####  #    # ###### ###### ###### #    #
        // 7: start sterilization 
        case APP_HEATING_AND_HOT_WATER_STATE_INIT_STERILIZATION:
        {   
            if (getStatus3WayValve() != VALVE_IS_ON_HOT_WATER_CIRCUIT)
            {   // 3 Way valve is still not on hot water mode, must be on hot water circuit 
                app_Data.appState = APP_HEATING_AND_HOT_WATER_STATE_SWITCH_3_WAY_VALVE_TO_HOT_WATER_CIRCUIT_AND_GO_STERILIZATION_MODE;
                break;
            }
            
            //if(setLoggingLock()){
                TurnOffHeatingElementHotWaterBuffer();
                //while(!releaseLoggingLock());
                secondCounterLegionella = 0;    
                app_Data.sterilizationTemperatureOffset = ReadSmartEeprom16(SEEP_ADDR_STERILIZATION_SETPOINT_OFFSET_START);  
                app_Data.appState = APP_HEATING_AND_HOT_WATER_STATE_STERILIZATION;
            //}
            break;
        }
        
        // 8: Sterilization mode 
        case APP_HEATING_AND_HOT_WATER_STATE_STERILIZATION:
        {               
            //if(!setLoggingLock()){			
			//	break;
			//}
                        
            if(CheckIfDefrostModeActive() == true)
            {   // Heatpump is doing defrosting
                if (initialDefrostingBoilerTemp == TEMPERATURE_ALARM_VALUE)
                    initialDefrostingBoilerTemp = app_Data.currentHotWaterBufferTemp;
                
                if (app_Data.currentHotWaterBufferTemp <= (initialDefrostingBoilerTemp - ReadSmartEeprom16(SEEP_ADDR_DEFROSTING_TEMP_FALL_BEFORE_ELEMENT_ON)))
                {   // Temperature falled 4 degrees Celcius beneath de defrosting starting temperature
                    TurnOnHeatingElementHotWaterBuffer();
                }
                else if (app_Data.currentHotWaterBufferTemp >= 
                        (initialDefrostingBoilerTemp - 
                        ReadSmartEeprom16(SEEP_ADDR_DEFROSTING_TEMP_FALL_BEFORE_ELEMENT_ON)+ 
                        ReadSmartEeprom16(SEEP_ADDR_DEFROSTING_TEMP_RISE_BEFORE_ELEMENT_OFF)))
                {   // Temperature rised to 1 degree beneath de defrosting starting temperature
                    TurnOffHeatingElementHotWaterBuffer();
                }
                else{}
            }
            else
            {   // Heatpump is NOT doing defrosting
                if (initialDefrostingBoilerTemp != TEMPERATURE_ALARM_VALUE)
                {   // Just came out of defrosting mode
                    if (getStatusHeatingElementHotWaterBuffer() == true)
                    {   // Hot water element is on
                        if (app_Data.currentHotWaterBufferTemp >= 
                            (initialDefrostingBoilerTemp - 
                            ReadSmartEeprom16(SEEP_ADDR_DEFROSTING_TEMP_FALL_BEFORE_ELEMENT_ON)+ 
                            ReadSmartEeprom16(SEEP_ADDR_DEFROSTING_TEMP_RISE_BEFORE_ELEMENT_OFF)))
                        {   // Temperature falled 3 degrees Celcius beneath de defrosting starting temperature
                            TurnOffHeatingElementHotWaterBuffer();
                            initialDefrostingBoilerTemp = TEMPERATURE_ALARM_VALUE;
                        }                     
                    }
                    else
                    {   // Hot water element is already off
                        initialDefrostingBoilerTemp = TEMPERATURE_ALARM_VALUE;  // Reset initial defrosting temperature
                    }
                    //while(!releaseLoggingLock());
                    return;
                }
                
                if ((app_Data.retourWaterTemperature >= (app_Data.sterilizationTemperature + app_Data.sterilizationTemperatureOffset - 20)) && (app_Data.sterilizationTemperatureOffset != 0) && (app_Data.sterilizationTemperatureOffset != TEMPERATURE_ALARM_VALUE))
                {   // Retour water temperature has come within 2 degree celcius of setpoint
                    app_Data.sterilizationTemperatureOffset += ReadSmartEeprom16(SEEP_ADDR_STERILIZATION_SETPOINT_OFFSET_STEPS);  // Increase offset with 2 degree Celcius
                }
                else{} // Retour water temperature has not come within 2 degree Celcius of setpoint

                if (app_Data.currentHotWaterBufferTemp >= app_Data.sterilizationTemperature)
                {   // Hot water buffer reached sterilization temperature

                    if (sterilizationReachedTemperatureTimeStamp == UINT32_MAX)
                    {   // Time not yet stamped
                        sterilizationReachedTemperatureTimeStamp = secondCounterLegionella;
                    }

                    if (secondCounterLegionella >= (sterilizationReachedTemperatureTimeStamp + (app_Data.sterilizationRunTime * 60)))
                    {   
                        //if(setLoggingLock()){
                            // Time reached (sterilization finished)
                        secondCounterLegionella = UINT32_MAX;
                        sterilizationReachedTemperatureTimeStamp = UINT32_MAX;

                        WriteSmartEeprom16(SEEP_ADDR_DAY_COUNTER_STERILIZATION, 0);
                        secondCounterDays = 0;
                        app_Data.sterilizationTemperatureOffset = TEMPERATURE_ALARM_VALUE;
                        initialDefrostingBoilerTemp = TEMPERATURE_ALARM_VALUE;
                        app_Data.appState = APP_HEATING_AND_HOT_WATER_STATE_SWITCH_3_WAY_VALVE_TO_HEATING_CIRCUIT_AND_GO_IDLE_MODE;
                        //while(!releaseLoggingLock());
                        break;
                    }
                }
                else
                {   // Hot water buffer has not reached sterilization temperature
                    sterilizationReachedTemperatureTimeStamp = UINT32_MAX;        

                    if (secondCounterLegionella >= ReadSmartEeprom16(SEEP_ADDR_STERILIZATION_MAX_TIME_IN_STERILIZATION_MODE))
                    {   // Already 120 minutes in sterilization mode, and but still not finished
                        secondCounterLegionella = 0;
                        TurnOnHeatingElementHotWaterBuffer();   // Set heating element on
                        app_Data.sterilizationTemperatureOffset = TEMPERATURE_ALARM_VALUE;
                        initialDefrostingBoilerTemp = TEMPERATURE_ALARM_VALUE;
                        app_Data.appState = APP_HEATING_AND_HOT_WATER_STATE_SWITCH_3_WAY_VALVE_TO_HEATING_CIRCUIT_AND_GO_IDLE_MODE;
                        //while(!releaseLoggingLock());
                        break;
                    } 
                }
            }
            //while(!releaseLoggingLock());
            break;
        }
        
//        #####                                   #####                                                                  
//       #     # #    # # #####  ####  #    #    #     #       #    #   ##   #   #    #    #   ##   #      #    # ###### 
//       #       #    # #   #   #    # #    #          #       #    #  #  #   # #     #    #  #  #  #      #    # #      
//        #####  #    # #   #   #      ######     #####  ##### #    # #    #   #      #    # #    # #      #    # #####  
//             # # ## # #   #   #      #    #          #       # ## # ######   #      #    # ###### #      #    # #      
//       #     # ##  ## #   #   #    # #    #    #     #       ##  ## #    #   #       #  #  #    # #       #  #  #      
//        #####  #    # #   #    ####  #    #     #####        #    # #    #   #        ##   #    # ######   ##   ######
        
        // 9: Switch 3-way valve to hot water circuit if needed and go to hot water mode 
        case APP_HEATING_AND_HOT_WATER_STATE_SWITCH_3_WAY_VALVE_TO_HOT_WATER_CIRCUIT_AND_GO_HOT_WATER_MODE:
        // 10: Switch 3-way valve to heating circuit if needed and go to heating mode 
        case APP_HEATING_AND_HOT_WATER_STATE_SWITCH_3_WAY_VALVE_TO_HEATING_CIRCUIT_AND_GO_HEATING_MODE:
        // 11: Switch 3-way valve to heating circuit if needed and go to idle mode
        case APP_HEATING_AND_HOT_WATER_STATE_SWITCH_3_WAY_VALVE_TO_HEATING_CIRCUIT_AND_GO_IDLE_MODE:
        // 12: Switch 3-way valve to heating circuit if needed and go to sterilization mode
        case APP_HEATING_AND_HOT_WATER_STATE_SWITCH_3_WAY_VALVE_TO_HOT_WATER_CIRCUIT_AND_GO_STERILIZATION_MODE:
        {
            if(secondCounterResetInitSystemStuck >= 120 && secondCounterResetInitSystemStuck != UINT32_MAX) {
                secondCounterResetInitSystemStuck = UINT32_MAX;
                secondCounterChangeSystem = UINT32_MAX;
                app_Data.switch3WayValveState = SWITCH_3_WAY_VALVE_STATE_INIT;
                app_Data.appState = APP_HEATING_AND_HOT_WATER_STATE_INIT;
                systemSoftwareReset(secondCounterDays);
                break;
            }
            
            switch (app_Data.switch3WayValveState)
            {
                // 0: Init
                case SWITCH_3_WAY_VALVE_STATE_INIT:
                {
                    secondCounterResetInitSystemStuck = 0;
                    app_Data.switch3WayValveState = SWITCH_3_WAY_VALVE_STATE_CHECK_IF_SWITCHING_NEEDED;
                    break;
                }
                
                // 1: Check if switching of the 3-way valve is needed for next state
                case SWITCH_3_WAY_VALVE_STATE_CHECK_IF_SWITCHING_NEEDED:
                {                    
                    bool valvePosition = getStatus3WayValve();
                    
                    switch (app_Data.appState)
                    {
                        case (APP_HEATING_AND_HOT_WATER_STATE_SWITCH_3_WAY_VALVE_TO_HEATING_CIRCUIT_AND_GO_IDLE_MODE):
                        {   // Next app state is idle mode 
                            if (valvePosition == VALVE_IS_ON_HEATING_CIRCUIT)
                            {   // Valve is already on heating and good for idle mode
                                app_Data.switch3WayValveState = SWITCH_3_WAY_VALVE_STATE_FINISHED;
                                //app_Data.switch3WayValveState = SWITCH_3_WAY_VALVE_STATE_INIT;
                                //app_Data.appState = APP_HEATING_AND_HOT_WATER_STATE_IDLE;
                            }
                            else
                            {   // Valve is on hot water circuit and needs to go to heating circuit
                                if (RealTimeData[ADDRESS_WATER_FLOW - START_ADDRESS_REAL_TIME_DATA][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP] == 0)
                                {   // Pump is already standing still
                                    Switch3WayValveToHeating();
                                    app_Data.switch3WayValveState = SWITCH_3_WAY_VALVE_STATE_FINISHED;
                                }
                                else
                                {   // Pump is running, so turn off the heatpump and switch after that
                                    app_Data.switch3WayValveState = SWITCH_3_WAY_VALVE_STATE_TURN_OFF_HEATPUMP;
                                }
                                     
                                //app_Data.switch3WayValveState = SWITCH_3_WAY_VALVE_STATE_TURN_OFF_HEATPUMP;
                            }  
                            break;
                        }
                        case (APP_HEATING_AND_HOT_WATER_STATE_SWITCH_3_WAY_VALVE_TO_HEATING_CIRCUIT_AND_GO_HEATING_MODE):
                        {   // Next app state is heating mode 
                            if (valvePosition == VALVE_IS_ON_HEATING_CIRCUIT)
                            {   // Valve is already on heating and good for heating mode
                                app_Data.switch3WayValveState = SWITCH_3_WAY_VALVE_STATE_FINISHED;
                                //app_Data.switch3WayValveState = SWITCH_3_WAY_VALVE_STATE_INIT;
                                //app_Data.appState = APP_HEATING_AND_HOT_WATER_STATE_INIT_HEATING;
                            }
                            else
                            {   // Valve is on hot water circuit and needs to go to heating circuit
                                if (RealTimeData[ADDRESS_WATER_FLOW - START_ADDRESS_REAL_TIME_DATA][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP] == 0)
                                {   // Pump is already standing still
                                    Switch3WayValveToHeating();
                                    app_Data.switch3WayValveState = SWITCH_3_WAY_VALVE_STATE_FINISHED;
                                }
                                else
                                {   // Pump is running, so turn off the heatpump and switch after that
                                    app_Data.switch3WayValveState = SWITCH_3_WAY_VALVE_STATE_TURN_OFF_HEATPUMP;
                                }
                                
                                //app_Data.switch3WayValveState = SWITCH_3_WAY_VALVE_STATE_TURN_OFF_HEATPUMP;
                            }  
                            break;
                        }
                        case (APP_HEATING_AND_HOT_WATER_STATE_SWITCH_3_WAY_VALVE_TO_HOT_WATER_CIRCUIT_AND_GO_HOT_WATER_MODE):
                        case (APP_HEATING_AND_HOT_WATER_STATE_SWITCH_3_WAY_VALVE_TO_HOT_WATER_CIRCUIT_AND_GO_STERILIZATION_MODE):
                        {   // Next app state is hot water or sterilization mode
                            if (valvePosition == VALVE_IS_ON_HOT_WATER_CIRCUIT)
                            {   // Valve is already on hot water and good for hot water mode
                                app_Data.switch3WayValveState = SWITCH_3_WAY_VALVE_STATE_FINISHED;
                                //app_Data.switch3WayValveState = SWITCH_3_WAY_VALVE_STATE_INIT;
                                //app_Data.appState = APP_HEATING_AND_HOT_WATER_STATE_INIT_HOT_WATER;
                            }
                            else
                            {   // Valve is on heating circuit and needs to go to hot water circuit
                                if (RealTimeData[ADDRESS_WATER_FLOW - START_ADDRESS_REAL_TIME_DATA][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP] == 0)
                                {   // Pump is already standing still
                                    Switch3WayValveToHotWater();
                                    app_Data.switch3WayValveState = SWITCH_3_WAY_VALVE_STATE_FINISHED;
                                }
                                else
                                {   // Pump is running, so turn off the heatpump and switch after that
                                    app_Data.switch3WayValveState = SWITCH_3_WAY_VALVE_STATE_TURN_OFF_HEATPUMP;
                                }
                                
                                //app_Data.switch3WayValveState = SWITCH_3_WAY_VALVE_STATE_TURN_OFF_HEATPUMP;
                            }  
                            break;
                        }
                        default: break;
                    }
                    break;
                }
                
                // 2: Turning off heatpump
                case SWITCH_3_WAY_VALVE_STATE_TURN_OFF_HEATPUMP:
                {
                    ChangeHeatpumpSetting(ADDRESS_ON_OFF, SET_HEATPUMP_OFF);
                    
                    secondCounterChangeSystem = 0;
                    
                    app_Data.switch3WayValveState = SWITCH_3_WAY_VALVE_STATE_WAIT_FOR_PUMP_OFF;
                    /*
                    if (Setting.settingStatus == SETTING_SEND_STATUS_IDLE)
                    {
                        //ChangeHeatpumpSettingg(ADDRESS_ON_OFF, SET_HEATPUMP_OFF);
                        
                        secondCounterChangeSystem = 0;
                        
                        app_Data.switch3WayValveState = SWITCH_3_WAY_VALVE_STATE_WAIT_FOR_PUMP_OFF;
                    }
                    */
                    break;
                }
                
                // 3: Wait for heatpump is turned off and water pump has stopped
                case SWITCH_3_WAY_VALVE_STATE_WAIT_FOR_PUMP_OFF:
                {                            
                    if (UserParameters[ADDRESS_ON_OFF - START_ADDRESS_USER_PARAMETERS][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP] == SET_HEATPUMP_OFF)
                    {   // Setting has correct be set to OFF
                        if (RealTimeData[ADDRESS_WATER_FLOW - START_ADDRESS_REAL_TIME_DATA][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP] == 0)
                        {
                            secondCounterChangeSystem = 0;
                            app_Data.switch3WayValveState = SWITCH_3_WAY_VALVE_STATE_DELAY_BEFORE_SWITCHING_VALVE;
                        }
                    }
                    else
                    {   // Setting has nog be set to OFF yet
                        if (secondCounterChangeSystem >= 20)
                        {   // Wait 20 seconds, and retry if still not OFF
                            secondCounterChangeSystem = UINT32_MAX;
                            app_Data.switch3WayValveState = SWITCH_3_WAY_VALVE_STATE_TURN_OFF_HEATPUMP;
                        }
                    }
                    
                    
                    
                    break;
                }
                
                // 4: Delay off some seconds before switching the valve
                case SWITCH_3_WAY_VALVE_STATE_DELAY_BEFORE_SWITCHING_VALVE:
                {
                    if (secondCounterChangeSystem >= 10)
                    {
                        secondCounterChangeSystem = UINT32_MAX;
                        app_Data.switch3WayValveState = SWITCH_3_WAY_VALVE_STATE_SWITCH_VALVE;
                    }
                    else{}

                    break;
                }
                
                // 5: Switch the valve
                case SWITCH_3_WAY_VALVE_STATE_SWITCH_VALVE:
                {
                    /*
                    if (app_Data.appState == APP_HEATING_AND_HOT_WATER_STATE_SWITCH_3_WAY_VALVE_TO_HEATING_CIRCUIT_AND_GO_IDLE_MODE ||
                            app_Data.appState == APP_HEATING_AND_HOT_WATER_STATE_SWITCH_3_WAY_VALVE_TO_HEATING_CIRCUIT_AND_GO_HEATING_MODE)    
                        Switch3WayValveToHeating();
                    else if (app_Data.appState == APP_HEATING_AND_HOT_WATER_STATE_SWITCH_3_WAY_VALVE_TO_HOT_WATER_CIRCUIT_AND_GO_HOT_WATER_MODE)  
                        Switch3WayValveToHotWater();
                    else{}
                    */
                    
                    switch (app_Data.appState)
                    {
                        case APP_HEATING_AND_HOT_WATER_STATE_SWITCH_3_WAY_VALVE_TO_HEATING_CIRCUIT_AND_GO_IDLE_MODE:
                        case APP_HEATING_AND_HOT_WATER_STATE_SWITCH_3_WAY_VALVE_TO_HEATING_CIRCUIT_AND_GO_HEATING_MODE:
                        {
                            Switch3WayValveToHeating();
                            break;
                        } 
                        case APP_HEATING_AND_HOT_WATER_STATE_SWITCH_3_WAY_VALVE_TO_HOT_WATER_CIRCUIT_AND_GO_HOT_WATER_MODE:
                        case APP_HEATING_AND_HOT_WATER_STATE_SWITCH_3_WAY_VALVE_TO_HOT_WATER_CIRCUIT_AND_GO_STERILIZATION_MODE:
                        {
                            Switch3WayValveToHotWater();
                            break;
                        } 
                        default: 
                        {
                            app_Data.appState = APP_HEATING_AND_HOT_WATER_STATE_INIT;
                            break;
                        }
                    }

                    secondCounterChangeSystem = 0;
                    app_Data.switch3WayValveState = SWITCH_3_WAY_VALVE_STATE_DELAY_AFTER_SWITCHING_VALVE;
                    break;
                }
                
                // 6: Delay of some seconds after switching the valve
                case SWITCH_3_WAY_VALVE_STATE_DELAY_AFTER_SWITCHING_VALVE:
                {
                    if (secondCounterChangeSystem >= 30)
                    {
                        secondCounterChangeSystem = UINT32_MAX;
                        app_Data.switch3WayValveState = SWITCH_3_WAY_VALVE_STATE_TURN_ON_HEATPUMP;
                    }
                    else{}

                    break;
                }
                
                // 7: Turn on heatpump again
                case SWITCH_3_WAY_VALVE_STATE_TURN_ON_HEATPUMP:
                {
                    ChangeHeatpumpSetting(ADDRESS_ON_OFF, SET_HEATPUMP_ON);
                    
                    secondCounterChangeSystem = 0;
                    
                    app_Data.switch3WayValveState = SWITCH_3_WAY_VALVE_STATE_WAIT_FOR_HEATPUMP_ON;
                    
                    /*
                    if (Setting.settingStatus == SETTING_SEND_STATUS_IDLE)
                    {
                        ChangeHeatpumpSettingg(ADDRESS_ON_OFF, SET_HEATPUMP_ON);
                        
                        secondCounterChangeSystem = 0;

                        app_Data.switch3WayValveState = SWITCH_3_WAY_VALVE_STATE_WAIT_FOR_HEATPUMP_ON;
                    }
                    */
                    break;
                }
                
                // 8: Wait for heatpump to be on 
                case SWITCH_3_WAY_VALVE_STATE_WAIT_FOR_HEATPUMP_ON:
                {
                    if (UserParameters[ADDRESS_ON_OFF - START_ADDRESS_USER_PARAMETERS][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP] == SET_HEATPUMP_ON)
                    {   // Setting has correct be set to ON
                        secondCounterChangeSystem = UINT32_MAX;
                        app_Data.switch3WayValveState = SWITCH_3_WAY_VALVE_STATE_FINISHED;
                    }
                    else
                    {   // Setting has nog be set to ON yet
                        if (secondCounterChangeSystem >= 20)
                        {   // Wait 20 seconds, and retry if still not OFF
                            secondCounterChangeSystem = UINT32_MAX;
                            app_Data.switch3WayValveState = SWITCH_3_WAY_VALVE_STATE_TURN_ON_HEATPUMP;
                        }
                    }

                    break;
                }
                
                // 9: Switching 3-way valve finished
                case SWITCH_3_WAY_VALVE_STATE_FINISHED:
                {
                    secondCounterResetInitSystemStuck = UINT32_MAX;
                    if (app_Data.appState == APP_HEATING_AND_HOT_WATER_STATE_SWITCH_3_WAY_VALVE_TO_HEATING_CIRCUIT_AND_GO_IDLE_MODE)    
                        app_Data.appState = APP_HEATING_AND_HOT_WATER_STATE_IDLE;
                    else if (app_Data.appState == APP_HEATING_AND_HOT_WATER_STATE_SWITCH_3_WAY_VALVE_TO_HEATING_CIRCUIT_AND_GO_HEATING_MODE)  
                        app_Data.appState = APP_HEATING_AND_HOT_WATER_STATE_INIT_HEATING;
                    else if (app_Data.appState == APP_HEATING_AND_HOT_WATER_STATE_SWITCH_3_WAY_VALVE_TO_HOT_WATER_CIRCUIT_AND_GO_HOT_WATER_MODE)  
                        app_Data.appState = APP_HEATING_AND_HOT_WATER_STATE_INIT_HOT_WATER;
                    else if (app_Data.appState == APP_HEATING_AND_HOT_WATER_STATE_SWITCH_3_WAY_VALVE_TO_HOT_WATER_CIRCUIT_AND_GO_STERILIZATION_MODE)  
                        app_Data.appState = APP_HEATING_AND_HOT_WATER_STATE_INIT_STERILIZATION;
                    else{}     

                    app_Data.switch3WayValveState = SWITCH_3_WAY_VALVE_STATE_INIT;
                    break;
                }
                
                default: 
                {
                    secondCounterResetInitSystemStuck = UINT32_MAX;
                    secondCounterChangeSystem = UINT32_MAX;
                    app_Data.switch3WayValveState = SWITCH_3_WAY_VALVE_STATE_INIT;
                    app_Data.appState = APP_HEATING_AND_HOT_WATER_STATE_INIT;
                    break;
                }
                
            }

            break;
        }
        

        
        /*
        // 10: start legionella prevention 
        case APP_HEATING_AND_HOT_WATER_STATE_START_LEGIONELLA:
        {   // Switch to hot water and turn off heating element
            Switch3WayValveToHotWater();
            TurnOffHeatingElementHotWaterBuffer();
            app_Data.appState =  APP_HEATING_AND_HOT_WATER_STATE_CHANGE_SETPOINT_TO_LEGIONELLA_SETPOINT;
            break;
        }
        
        // 11: Change setpoint in heatpump to heatpump setpoint
        case APP_HEATING_AND_HOT_WATER_STATE_CHANGE_SETPOINT_TO_LEGIONELLA_SETPOINT:
        {   
            if (Setting.settingStatus == SETTING_SEND_STATUS_IDLE)
            {
                ChangeHeatpumpSetting(ADDRESS_HEATING_SET_TEMPERATURE, (app_Data. / 10));

                app_Data.appState = APP_HEATING_AND_HOT_WATER_STATE_WAIT_FOR_LEGIONELLA_SETPOINT_CHANGED;
                break;
            }
            break;
        }
        
        // 12: wait for setpoint is changed to legionella setpoint
        case APP_HEATING_AND_HOT_WATER_STATE_WAIT_FOR_LEGIONELLA_SETPOINT_CHANGED:
        {   
            if ((UserParameters[ADDRESS_STERILIZATION_TEMPERATURE_SETTING - START_ADDRESS_UNIT_SYSTEM_PARAMETER_L][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP] * 10) == app_Data.)
            {
                SecondCounterHotWater = 0;
                app_Data.appState = APP_HEATING_AND_HOT_WATER_STATE_WAIT_FOR_LEGIONELLA_SETPOINT_REACHED;
                break;
            }
            else{}
            
            break;
        }
        
        // 13: Heatpump is now heating the hot water buffer till legionella
        case APP_HEATING_AND_HOT_WATER_STATE_WAIT_FOR_LEGIONELLA_SETPOINT_REACHED:
        {   
            // 
            
            
            if (checkIfSetpointReached(app_Data.setpointHotWaterBufferTemp, app_Data.currentHotWaterBufferTemp) == true)
            {   // Setpoint reached
                TurnOffHeatingElementHotWaterBuffer();  // If heating element was on, turn it off
                SecondCounterHotWater = UINT32_MAX;     // Set timer off
                app_Data.appState = APP_HEATING_AND_HOT_WATER_STATE_IDLE;
                break;
            }
            else
            {   // Setpoint not (yet) reached
                if (getStatusHeatingElementHotWaterBuffer() == false)
                {   // Heating element is off
                    //if (SecondCounterHotWater >= HOT_WATER_RUNNING_TIME_BEFORE_TURNING_ON_HEATING_ELEMENT_SEC)
                    if (SecondCounterHotWater >= ReadSmartEeprom16(SEEP_ADDR_HOT_WATER_RUNNING_TIME_BEFORE_TURNING_ON_HEATING_ELEMENT))
                    {   // 2 hours passed
                        SecondCounterHotWater = UINT32_MAX;     // Set timer off
                        TurnOnHeatingElementHotWaterBuffer();   // Set heating element on
                    }
                    else{} // Wait till setpoint reached or 2 hours passed
                }
                else
                {   // Heating element is on
                    if (GetThermostatContact() == true)
                    {   // Thermostat contact has been made
                        SecondCounterHotWater = 0; // Start hot water timer
                        app_Data.appState = APP_HEATING_AND_HOT_WATER_STATE_START_HEATING_HEATING; // Go to heating
                        break;
                    }
                    else{} // Wait till setpoint reached
                } 
            }
            
            break;
        }
        */
        
        // The default state should never be executed. 
        default:
        {
            app_Data.appState = APP_HEATING_AND_HOT_WATER_STATE_INIT;
            break;
        }
    }
}

/*******************************************************************************
 End of File
 */
