#include <stdio.h>
#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include <string.h>
#include "definitions.h"                // SYS function prototypes

#include "app_in_outputs.h"
#include "app_heatpump_comm.h"
#include "files\ntc.h"
#include "files\alarms.h"
#include "files\time_counters.h"

//include "files\1wire.h"
//#include "app_heating_and_hot_water.h"
#include "files\delay.h"

#include "files\hardware_rev.h"
// Test only:
//#include "files\modbus\heatpump_parameters.h"

APP_IN_OUTPUTS_DATA app_in_outputsData;
//uint8_t RevNum = 0;   

//static volatile bool isTimerExpired = false;

/*******************************************************************************
  Function:
    void APP_IN_OUTPUTS_Initialize ( void )

  Remarks:
    See prototype in app_in_outputs.h.
 */

void APP_IN_OUTPUTS_Initialize ( void )
{
    TC0_TimerStart();
    
    InitTimerCounters();
    
    DMAC_ChannelCallbackRegister(DMAC_CHANNEL_5,adc_sram_dma_callback, 0);
    DMAC_ChannelTransfer(DMAC_CHANNEL_5,(const void *)&ADC1_REGS->ADC_RESULT,(const void *)adc_res,8);
    DMAC_ChannelTransfer(DMAC_CHANNEL_4,(const void *)adc_seq_regs,(const void *)&ADC1_REGS->ADC_DSEQDATA,16);
    ADC1_Enable();
    
    ClearAlarmArray();
    
    DetermineHardwareRev();
    
    /*
    GPIO_PB17_Set();
    delayMS(1);
    
    if(GPIO_PB17_Get() == true){
        GPIO_PB17_Clear();
        delayMS(1);
        if(GPIO_PB17_Get() == true){
            RevNum = 2;
        } else {
            RevNum = 1;
        }
    } else {
        RevNum = 3;
    }
    
    SYS_DEBUG_PRINT(SYS_ERROR_ERROR, "\r\n Revision >> %i\r\n", RevNum);
    */
    //Reserved1_Clear();
    
    /*
    NVIC_INT_Disable();
    GetHwid(&Hwid[0]);
    CalculateCrc(&Hwid[0]);
    NVIC_INT_Enable();
    
    if(writeStatus == true && (GetDip4() == true))
    {
        writeStatus = false;
        memset(DebugBuffer, 0, sizeof(DebugBuffer));
        //sprintf(DebugBuffer, "Value of test = \r\n");
        sprintf(DebugBuffer, "\r\nHWID: [%x][%x][%x][%x][%x][%x][%x]\r\n", Hwid[0],Hwid[1],Hwid[2],Hwid[3],Hwid[4],Hwid[5],Hwid[6]);
        DMAC_ChannelTransfer(DMAC_CHANNEL_10, &DebugBuffer, (const void *)&SERCOM5_REGS->USART_INT.SERCOM_DATA, 250);
    }
    */        
    app_in_outputsData.state = APP_IN_OUTPUTS_STATE_INIT;
}

/******************************************************************************
  Function:
    void APP_IN_OUTPUTS_Tasks ( void )

  Remarks:
    See prototype in app_in_outputs.h.
 */

void APP_IN_OUTPUTS_Tasks ( void )
{
    UpdateCounters();
    /* Check the application's current state. */
    switch ( app_in_outputsData.state )
    {
        /* Application's initial state. */
        case APP_IN_OUTPUTS_STATE_INIT:
        {            
            app_in_outputsData.state = APP_IN_OUTPUTS_STATE_SERVICE_TASKS;
            
            break;
        }

        case APP_IN_OUTPUTS_STATE_SERVICE_TASKS:
        {            
            if (adc_dma_done) 
            {
                adc_dma_done = false;
                
                AdcVerwerk();
                
                DMAC_ChannelTransfer(DMAC_CHANNEL_5,(const void *)&ADC1_REGS->ADC_RESULT,(const void *)adc_res,8);
                DMAC_ChannelTransfer(DMAC_CHANNEL_4,(const void *)adc_seq_regs,(const void *)&ADC1_REGS->ADC_DSEQDATA,16);
            }
            
            
            if (LedsTimerExpired() == true)
            {
                LedRun_Toggle();
                
                SetOrClearAlarmLed();
                
                /*
                Reserved1_Set();
                delayMS(1);
                
                if (Reserved1_Get() == true)
                {   
                    Reserved1_Clear();
                    delayMS(1);
                    if(Reserved1_Get() == true)
                    {   // External pullup
                        RelayPotfree1_Toggle();
                    }
                    else
                    {   // HIGH Z
                        RelayPotfree2_Toggle();
                    }
                }
                else
                {   // External pulldown
                    RelayPotfree3_Toggle();
                }
                */
                
            }   // End timer 1 sec    
            
            if ((!NotInputBtn1_Get()) == true)
            {
                while(1);
            }
            
            break;
        }
        /* TODO: implement your application state machine.*/


        /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}


/*******************************************************************************
 End of File
 */
