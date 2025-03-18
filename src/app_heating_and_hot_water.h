/*******************************************************************************
  MPLAB Harmony Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    app_warm_water.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "APP_WARM_WATER_Initialize" and "APP_WARM_WATER_Tasks" prototypes) and some of them are only used
    internally by the application (such as the "APP_WARM_WATER_STATES" definition).  Both
    are defined here for convenience.
*******************************************************************************/

#ifndef _APP_HEATING_AND_HOT_WATER_H
#define _APP_HEATING_AND_HOT_WATER_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "configuration.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Type Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Application states

  Summary:
    Application states enumeration

  Description:
    This enumeration defines the valid application states.  These states
    determine the behavior of the application at various times.
*/

typedef enum
{
    /* Application's state machine's initial state. */
    APP_HEATING_AND_HOT_WATER_STATE_INIT=0,             // 0
    APP_HEATING_AND_HOT_WATER_STATE_WAIT_FOR_SENSORS,   // 1    
    APP_HEATING_AND_HOT_WATER_STATE_IDLE,               // 2
    
    APP_HEATING_AND_HOT_WATER_STATE_INIT_HOT_WATER,     // 3
    APP_HEATING_AND_HOT_WATER_STATE_HOT_WATER,          // 4
            
    APP_HEATING_AND_HOT_WATER_STATE_INIT_HEATING,       // 5
    APP_HEATING_AND_HOT_WATER_STATE_HEATING,            // 6
            
    APP_HEATING_AND_HOT_WATER_STATE_INIT_STERILIZATION, // 7       
    APP_HEATING_AND_HOT_WATER_STATE_STERILIZATION,      // 8
            
    APP_HEATING_AND_HOT_WATER_STATE_SWITCH_3_WAY_VALVE_TO_HOT_WATER_CIRCUIT_AND_GO_HOT_WATER_MODE,      // 9    
    APP_HEATING_AND_HOT_WATER_STATE_SWITCH_3_WAY_VALVE_TO_HEATING_CIRCUIT_AND_GO_HEATING_MODE,          // 10
    APP_HEATING_AND_HOT_WATER_STATE_SWITCH_3_WAY_VALVE_TO_HEATING_CIRCUIT_AND_GO_IDLE_MODE,             // 11
    APP_HEATING_AND_HOT_WATER_STATE_SWITCH_3_WAY_VALVE_TO_HOT_WATER_CIRCUIT_AND_GO_STERILIZATION_MODE,  // 12
                
    // Hot water states:        
    //APP_HEATING_AND_HOT_WATER_STATE_START_HOT_WATER_HEATING,
    //APP_HEATING_AND_HOT_WATER_STATE_PREPARE_SYSTEM_FOR_HOT_WATER,
    //APP_HEATING_AND_HOT_WATER_STATE_PREPARE_SYSTEM_FOR_HOT_WATER_FINISHED, 
    //APP_HEATING_AND_HOT_WATER_STATE_WAIT_FOR_HOT_WATER_SETPOINT_REACHED,             
            
    // Heating states:        
    //APP_HEATING_AND_HOT_WATER_STATE_START_HEATING_HEATING,
    //APP_HEATING_AND_HOT_WATER_STATE_PREPARE_SYSTEM_FOR_HEATING,
    //APP_HEATING_AND_HOT_WATER_STATE_PREPARE_SYSTEM_FOR_HEATING_FINISHED,
    //APP_HEATING_AND_HOT_WATER_STATE_WAIT_FOR_HEATING_SETPOINT_REACHED, 
            
    // Legionella prevention states:        
    //APP_HEATING_AND_HOT_WATER_STATE_START_LEGIONELLA,
    //APP_HEATING_AND_HOT_WATER_STATE_CHANGE_SETPOINT_TO_LEGIONELLA_SETPOINT,
    //APP_HEATING_AND_HOT_WATER_STATE_WAIT_FOR_LEGIONELLA_SETPOINT_CHANGED,   
    //APP_HEATING_AND_HOT_WATER_STATE_WAIT_FOR_LEGIONELLA_SETPOINT_REACHED,               

} APP_HEATING_AND_HOT_WATER_STATES;
        
typedef enum
{
    SWITCH_3_WAY_VALVE_STATE_INIT=0,
    SWITCH_3_WAY_VALVE_STATE_CHECK_IF_SWITCHING_NEEDED,       
    SWITCH_3_WAY_VALVE_STATE_TURN_OFF_HEATPUMP,
    SWITCH_3_WAY_VALVE_STATE_WAIT_FOR_PUMP_OFF,
    SWITCH_3_WAY_VALVE_STATE_DELAY_BEFORE_SWITCHING_VALVE,
    SWITCH_3_WAY_VALVE_STATE_SWITCH_VALVE,
    SWITCH_3_WAY_VALVE_STATE_DELAY_AFTER_SWITCHING_VALVE,
    SWITCH_3_WAY_VALVE_STATE_TURN_ON_HEATPUMP,
    SWITCH_3_WAY_VALVE_STATE_WAIT_FOR_HEATPUMP_ON,   
    SWITCH_3_WAY_VALVE_STATE_FINISHED,  
            
} SWITCH_3_WAY_VALVE_STATES;
        
// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    Application strings and buffers are be defined outside this structure.
 */

typedef struct
{
    // The application's current state 
    APP_HEATING_AND_HOT_WATER_STATES appState;
    SWITCH_3_WAY_VALVE_STATES switch3WayValveState;

    int16_t  setpointHotWaterBufferTemp;
    int16_t  setpointHeatingBufferTemp;
    int16_t  setpointHotWaterOffset;
    
    int16_t  currentHotWaterBufferTemp;
    int16_t  currentHeatingBufferTemp;
    
    int16_t  deltaHotWaterBufferTemp;
    int16_t  deltaHeatingBufferTemp;
    
    uint16_t compressorOperatingFrequency;
    int16_t  retourWaterTemperature;
    
    uint16_t sterilizationFunction;
    uint16_t sterilizationIntervalDays;
    uint16_t sterilizationStartTime;
    uint16_t sterilizationRunTime;
    int16_t sterilizationTemperature;
    int16_t sterilizationTemperatureOffset;
    uint16_t currentDayCount;
    uint8_t currentDisplayTimeHours;
    uint8_t currentDisplayTimeMinutes;

} APP_HEATING_AND_HOT_WATER_DATA;

extern APP_HEATING_AND_HOT_WATER_DATA app_Data;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Routines
// *****************************************************************************
// *****************************************************************************
/* These routines are called by drivers when certain events occur.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************


/*******************************************************************************
  Function:
    void APP_WARM_WATER_Initialize ( void )

  Summary:
     MPLAB Harmony application initialization routine.

  Description:
    This function initializes the Harmony application.  It places the
    application in its initial state and prepares it to run so that its
    APP_WARM_WATER_Tasks function can be called.

  Precondition:
    All other system initialization routines should be called before calling
    this routine (in "SYS_Initialize").

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    APP_WARM_WATER_Initialize();
    </code>

  Remarks:
    This routine must be called from the SYS_Initialize function.
*/

void APP_HEATING_AND_HOT_WATER_Initialize ( void );


/*******************************************************************************
  Function:
    void APP_WARM_WATER_Tasks ( void )

  Summary:
    MPLAB Harmony Demo application tasks function

  Description:
    This routine is the Harmony Demo application's tasks function.  It
    defines the application's state machine and core logic.

  Precondition:
    The system and application initialization ("SYS_Initialize") should be
    called before calling this.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    APP_WARM_WATER_Tasks();
    </code>

  Remarks:
    This routine must be called from SYS_Tasks() routine.
 */

void APP_HEATING_AND_HOT_WATER_Tasks( void );

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif /* _APP_WARM_WATER_H */

/*******************************************************************************
 End of File
 */

