/*******************************************************************************
  User Configuration Header

  File Name:
    user.h

  Summary:
    Build-time configuration header for the user defined by this project.

  Description:
    An MPLAB Project may have multiple configurations.  This file defines the
    build-time options for a single configuration.

  Remarks:
    It only provides macro definitions for build-time configuration options

*******************************************************************************/

#ifndef USER_H
#define USER_H

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: User Configuration macros
// *****************************************************************************
// *****************************************************************************
    
// General settings:   
#define TEMPERATURE_ALARM_VALUE -9999    
    
//     
#define THIS_FIRMWARE_VERSION 1001006 // 1.1.6
    
#define SECONDS_IN_DAY 86400 // 1 day
//#define SECONDS_IN_DAY 60 // 1 minute
//#define SECONDS_IN_DAY 300 // 5 minutes
    
// 230V relays    
#define TurnOnCirculationPump()     RelayLine1_Set()
#define TurnOffCirculationPump()    RelayLine1_Clear()
#define getStatusCirculationPump()  RelayLine1_Get()

#define Switch3WayValveToHotWater()    RelayLine2_Set()
#define Switch3WayValveToHeating()     RelayLine2_Clear()
#define getStatus3WayValve()           RelayLine2_Get() 
    #define VALVE_IS_ON_HOT_WATER_CIRCUIT   1      
    #define VALVE_IS_ON_HEATING_CIRCUIT     0
#define getReserved230VRelay()          RelayLine3_Get()    
    
// External relays    
#define TurnOnHeatingElementHotWaterBuffer()     RelayExtern2_Set()     
#define TurnOffHeatingElementHotWaterBuffer()    RelayExtern2_Clear()
#define getStatusHeatingElementHotWaterBuffer()  RelayExtern2_Get()  
     
#define TurnOnHeatingElementHeatingBuffer()     RelayExtern1_Set()     
#define TurnOffHeatingElementHeatingBuffer()    RelayExtern1_Clear()
#define getStatusHeatingElementHeatingBuffer()  RelayExtern1_Get() 
    
#define getStatusConnectRelay1()                RelayPotfree1_Get()
#define getStatusConnectRelay2()                RelayPotfree2_Get()
#define getStatusConnectRelay3()                RelayPotfree3_Get()    
    
    
// Digital inputs
#define GetThermostatContact()    InputExt1_Get()  
#define GetDigitalInput2()        InputExt2_Get() 
#define GetDigitalInput3()        InputExt3_Get() 
    
// Dip switches:
#define GetDip1()   ((NotInputDip1_Get()) ? false : true) 
#define GetDip2()   ((NotInputDip2_Get()) ? false : true)
#define GetDip3()   ((NotInputDip3_Get()) ? false : true) 
#define GetDip4()   ((NotInputDip4_Get()) ? false : true)     

#define DebugDipSwitch()    GetDip4()
    
#define getPowerFailStatus()                    NotPwrPowerFail_Get()
#define getSupercapFaultStatus()                NotPwrCapFault_Get()   
#define getSystemGoodIndicator()                PwrSysGood_Get()
#define getSupercapacitorPowerGoodIndicator()   PwrCapGood_Get()

        
#define getInputSlideSwitch()                NotInputSw1_Get()        
        
//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // USER_H
/*******************************************************************************
 End of File
*/
