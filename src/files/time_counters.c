/* TODO:  Include other files here if needed. */
#include <xc.h>
#include <string.h>
#include <stdbool.h>                    // Defines true
#include "definitions.h"
#include "time_counters.h"

static volatile bool isTimerExpired = false;

static uint32_t SecondCounterLeds = UINT32_MAX;
static uint32_t SecondCounterHeatingHotWater = UINT32_MAX;
static uint32_t SecondCounterDisplayCommunication = UINT32_MAX;
static uint32_t SecondCounterHeatpumpCommunication = UINT32_MAX;
static uint32_t SecondCounterLogging = UINT32_MAX;
static uint32_t SecondCounterLoggingSDCard = UINT32_MAX;
//static uint32_t SecondCounterFtp = UINT32_MAX;
//static uint32_t SecondCounterResetSoftware = UINT32_MAX;



void TC1_Callback_InterruptHandler(TC_TIMER_STATUS status, uintptr_t context)
{
    isTimerExpired = true;
}

/*******************************************************************************
  Function:
    void InitTimerCounters ( void )

 */
void InitTimerCounters ( void )
{    
    SecondCounterLeds = 0;
    SecondCounterHeatingHotWater = 0;
    SecondCounterDisplayCommunication = 0;
    SecondCounterHeatpumpCommunication = 0;
    SecondCounterLogging = 0;
    //SecondCounterLoggingSDCard = 300;
    SecondCounterLoggingSDCard = 150;
    //SecondCounterFtp = 0;
    //SecondCounterResetSoftware = 0;
    
    TC1_TimerCallbackRegister(TC1_Callback_InterruptHandler, (uintptr_t)NULL);
    TC1_TimerStart();
}

    
/******************************************************************************
  Function:
    void UpdateCounters ( void )
 */
void UpdateCounters ( void )
{
    if (isTimerExpired == true)
    {   // Every 100 ms
        isTimerExpired = false;
        
        if (SecondCounterLeds >= 0 && SecondCounterLeds < UINT32_MAX)
            SecondCounterLeds++;
        
        if (SecondCounterHeatingHotWater >= 0 && SecondCounterHeatingHotWater < UINT32_MAX)
            SecondCounterHeatingHotWater++;
        
        if (SecondCounterDisplayCommunication >= 0 && SecondCounterDisplayCommunication < UINT32_MAX)
            SecondCounterDisplayCommunication++;
        
        if (SecondCounterHeatpumpCommunication >= 0 && SecondCounterHeatpumpCommunication < UINT32_MAX)
            SecondCounterHeatpumpCommunication++;
        
        if (SecondCounterLogging >= 0 && SecondCounterLogging < UINT32_MAX) 
            SecondCounterLogging++;
        
        if (SecondCounterLoggingSDCard >= 0 && SecondCounterLoggingSDCard < UINT32_MAX) 
            SecondCounterLoggingSDCard++;
        //if (SecondCounterFtp >= 0 && SecondCounterFtp < UINT32_MAX)
        //    SecondCounterFtp++;
        
        //if (SecondCounterResetSoftware >= 0 && SecondCounterResetSoftware < UINT32_MAX)
        //    SecondCounterResetSoftware++;
    }    
}

bool LedsTimerExpired ( void )
{
    if (SecondCounterLeds >= TENTH_SECOND_COUNTER_1_SECOND) 
    {   // Every second
        SecondCounterLeds = 0;
        return true;
    }        
    else
    {
        return false;
    }
}

bool HeatingHotWaterTimerExpired ( void )
{
    if (SecondCounterHeatingHotWater >= TENTH_SECOND_COUNTER_1_SECOND) 
    {   // Every second
        SecondCounterHeatingHotWater = 0;
        return true;
    }        
    else
    {
        return false;
    }
}

bool DisplayCommunicationTimerExpired ( void )
{
    if (SecondCounterDisplayCommunication >= TENTH_SECOND_COUNTER_100_MILLISECOND) 
    {   // Every 100 millisecond
        SecondCounterDisplayCommunication = 0;
        return true;
    }        
    else
    {
        return false;
    }
}

bool HeatpumpCommunicationTimerExpired ( void )
{
    if (SecondCounterHeatpumpCommunication >= TENTH_SECOND_COUNTER_100_MILLISECOND) 
    {   // Every 100 millisecond
        SecondCounterHeatpumpCommunication = 0;
        return true;
    }        
    else
    {
        return false;
    }
}

bool LoggingTimerExpired ( void ) {
    //if (SecondCounterLogging >= TENTH_SECOND_COUNTER_30_SECONDS) 
    //if (SecondCounterLogging >= TENTH_SECOND_COUNTER_1_MINUTE) 
    if (SecondCounterLogging >= TENTH_SEC0ND_COUNTER_5_MINUTES) 
    {   
        SecondCounterLogging = 0;
        return true;
    }        
    else
    {
        return false;
    }
}

bool LoggingTimerSDCardExpired ( void ) {
    //if (SecondCounterLoggingSDCard >= TENTH_SECOND_COUNTER_5_SECONDS) 
    //if (SecondCounterLoggingSDCard >= TENTH_SECOND_COUNTER_30_SECONDS)
    //if (SecondCounterLoggingSDCard >= TENTH_SECOND_COUNTER_1_MINUTE) 
    if (SecondCounterLoggingSDCard >= TENTH_SEC0ND_COUNTER_5_MINUTES) 
    {   
        SecondCounterLoggingSDCard = 0;
        return true;
    }        
    else
    {
        return false;
    }
}

/*
bool FtpTimerExpired ( void )
{
    if (SecondCounterFtp >= TENTH_SECOND_COUNTER_1_SECOND) 
    {   // Every 100 millisecond
        SecondCounterFtp = 0;
        return true;
    }        
    else
    {
        return false;
    }
}
*/

/*
bool ResetSoftwareTimerExpired ( void )
{
    if (SecondCounterResetSoftware >= TENTH_SECOND_COUNTER_1_DAY) 
    {   // Every second
        SecondCounterResetSoftware = 0;
        return true;
    }        
    else
    {
        return false;
    }
}
*/