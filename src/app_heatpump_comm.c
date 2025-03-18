/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_heatpump_comm.c

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
#include "definitions.h"                // SYS function prototypes

#include "app_heatpump_comm.h"
//#include "files\modbus\display_parameters.h"
#include "files\modbus\heatpump.h"
#include "files\modbus\shiftregisters.h"
#include "files\modbus\modbus.h"
#include "files\alarms.h"
#include "files\time_counters.h"
#include "files/logging.h"

#include "files\eeprom.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************

APP_HEATPUMP_COMM_DATA app_heatpump_commData;

static uint8_t TxBuffer[TX_BUFFER_DISPLAY_SIZE];
static uint8_t RxBuffer[RX_BUFFER_DISPLAY_SIZE];

static volatile uint32_t ResponseDelay = 0;
static volatile uint32_t CommunicationWindowSecondCounter = 0;
static volatile uint32_t CommunicationTimeOutCounter = 0;

static volatile bool doFirstTimeHeatpumpCommunicationSettings = false;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

static void APP_WriteCallbackHeatpump(DMAC_TRANSFER_EVENT event, uintptr_t contextHandle)
{
    if (event == DMAC_TRANSFER_EVENT_COMPLETE)
    {
        app_heatpump_commData.commStatus = HEATPUMP_COMM_STATUS_DATA_SENT_TO_HEATPUMP;
    }
}

static void APP_ReadCallbackHeatpump(DMAC_TRANSFER_EVENT event, uintptr_t contextHandle)
{    
    if (event == DMAC_TRANSFER_EVENT_COMPLETE)
    {
        if(SERCOM7_USART_ErrorGet() == USART_ERROR_NONE)
        {   // ErrorGet clears errors, set error flag to notify console 
            switch (app_heatpump_commData.commStatus)
            {
                case HEATPUMP_COMM_STATUS_WAITING_FOR_DATA_FROM_HEATPUMP:
                {   // Eerste byte ontvangen, kijken wat dit is
                    if (RxBuffer[0] == THIS_DEVICE_ADDRESS)
                    {   // Als het het address is van de slave, vraag om de 7 andere bytes die hierna komen.
                        app_heatpump_commData.commStatus = HEATPUMP_COMM_STATUS_DEVICE_ADDRESS_RECEIVED;
                    
                        DMAC_ChannelTransfer(DMAC_CHANNEL_3, \
                            (const void *)&SERCOM7_REGS->USART_INT.SERCOM_DATA, \
                            &RxBuffer[1], 7);
                    }
                    else
                    {   // Wacht op volgende eerste byte
                        DMAC_ChannelTransfer(DMAC_CHANNEL_3, \
                            (const void *)&SERCOM7_REGS->USART_INT.SERCOM_DATA, \
                            &RxBuffer[0], 1);
                    }
                    break;
                }
                case HEATPUMP_COMM_STATUS_DEVICE_ADDRESS_RECEIVED:
                {
                    if (RxBuffer[MODBUS_COMMAND_INDEX] == MB_FC_WRITE_REG)
                    {   // Alle data ontvangen
                        app_heatpump_commData.commStatus = HEATPUMP_COMM_STATUS_DATA_RECEIVED_FROM_HEATPUMP;
                    }
                    else if (RxBuffer[MODBUS_COMMAND_INDEX] == MB_FC_READ_REGS)
                    {   // Vraag wat nog komt
                        app_heatpump_commData.commStatus = HEATPUMP_COMM_STATUS_FIRST_8_BYTES_RECEIVED;
                        DMAC_ChannelTransfer(DMAC_CHANNEL_3, \
                            (const void *)&SERCOM7_REGS->USART_INT.SERCOM_DATA, \
                            &RxBuffer[8], (RxBuffer[MODBUS_BYTES_RETURNED_INDEX] - 3));
                    }
                    else{}
                    
                    break;
                }
                case HEATPUMP_COMM_STATUS_FIRST_8_BYTES_RECEIVED:
                {
                    app_heatpump_commData.commStatus = HEATPUMP_COMM_STATUS_DATA_RECEIVED_FROM_HEATPUMP;
                    break;
                }
                default:
                {
                    break;
                }   
            }      
        }
        else{}
    }
}
/*
static void TC4_Callback_InterruptHandler(TC_TIMER_STATUS status, uintptr_t context)
{    
    if (ResponseDelay < UINT32_MAX)
        ResponseDelay++;    
    
    if (CommunicationWindowSecondCounter < UINT32_MAX)
        CommunicationWindowSecondCounter++;
    
    if (CommunicationTimeOutCounter < UINT32_MAX)
        CommunicationTimeOutCounter++;
}
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************

void StartReceivingDataFromHeatpump(void)
{
    SetOutput(LED_RX_HEATPUMP, true);
    
    app_heatpump_commData.commStatus = HEATPUMP_COMM_STATUS_WAITING_FOR_DATA_FROM_HEATPUMP;
    
    DMAC_ChannelTransfer(DMAC_CHANNEL_3, \
        (const void *)&SERCOM7_REGS->USART_INT.SERCOM_DATA, \
        &RxBuffer[0], 1);
}

void StartTransmittingDataToHeatpump(uint8_t * txBufferHeatpump)
{
    SetOutput(LED_TX_HEATPUMP, true);
    
    app_heatpump_commData.commStatus = HEATPUMP_COMM_STATUS_SENDING_DATA_TO_HEATPUMP;
    
    if (txBufferHeatpump[0] == 104)
    {
        DMAC_ChannelTransfer(DMAC_CHANNEL_0, &TxBuffer[0], \
            (const void *)&SERCOM7_REGS->USART_INT.SERCOM_DATA, 16);
    }
    else
    {
        DMAC_ChannelTransfer(DMAC_CHANNEL_0, &TxBuffer[0], \
            (const void *)&SERCOM7_REGS->USART_INT.SERCOM_DATA, 8);
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_HEATPUMP_COMM_Initialize ( void )

  Remarks:
    See prototype in app_heatpump_comm.h.
 */

void APP_HEATPUMP_COMM_Initialize ( void )
{
    SmartEepromInit();
    
    DMAC_ChannelCallbackRegister(DMAC_CHANNEL_3, APP_ReadCallbackHeatpump, 0);
    DMAC_ChannelCallbackRegister(DMAC_CHANNEL_0, APP_WriteCallbackHeatpump, 0);
    
    //TC4_TimerCallbackRegister(TC4_Callback_InterruptHandler, (uintptr_t)NULL);
    //TC4_TimerStart();

    InitShiftRegisters();
    SetOutput(LED_RX_HEATPUMP, false);
    SetOutput(LED_TX_HEATPUMP, false);
    
    memset(TxBuffer, 0, TX_BUFFER_HEATPUMP_SIZE);
    memset(RxBuffer, 0, RX_BUFFER_HEATPUMP_SIZE);
    
    // Change password setting at startup:
    //if (Setting.settingStatus == SETTING_SEND_STATUS_IDLE)
    //{
    //    ChangeHeatpumpSetting(ADDRESS_PARAMETER_PASSWORD_SETTING, 255); // Set parameters password to 255
    //}
    
    if (ReadSmartEeprom8(SEEP_ADDR_CONNECT_HAS_EVER_CONTACTED_HEATPUMP) == false)
    {   // Connect has never communicated with an heatpump before
        doFirstTimeHeatpumpCommunicationSettings = true;
    }
    
    app_heatpump_commData.commStatus = HEATPUMP_COMM_STATUS_IDLE;
    app_heatpump_commData.state = APP_HEATPUMP_COMM_STATE_INIT;
}


/******************************************************************************
  Function:
    void APP_HEATPUMP_COMM_Tasks ( void )

  Remarks:
    See prototype in app_heatpump_comm.h.
 */

void APP_HEATPUMP_COMM_Tasks ( void )
{
    static uint8_t startupSettingsCounter = 0;
    
    if (CommunicationWindowSecondCounter >= RESET_AFTER_NO_COMMUNICATION_SECONDS)
    {   // Tijd lang geen communicatie geweest
        CommunicationWindowSecondCounter = 0;
        //ManualToInitCounter++;
        
        //while(true); // Laatste redmiddel, zorg ervoor dat watchdog ingaat en print gereset wordt
        app_heatpump_commData.state = APP_HEATPUMP_COMM_STATE_INIT;
        //printf("\r\n Error! Manual to APP_STATE_INIT counter: %d\r\n", ManualToInitCounter);
    } 
    
    if (CommunicationTimeOutCounter > RESET_AFTER_NO_COMMUNICATION_SECONDS)
    {
        SetOrClearAlarm(ALARM_HEATPUMP_COMMUNICATION, SET_ALARM);
    }
    else
    {
        SetOrClearAlarm(ALARM_HEATPUMP_COMMUNICATION, CLEAR_ALARM);
    }
    
    if (HeatpumpCommunicationTimerExpired() == true)
    {
        if (ResponseDelay < UINT32_MAX) {
            ResponseDelay++;    
        }
    
        if (CommunicationWindowSecondCounter < UINT32_MAX) {
            CommunicationWindowSecondCounter++;
        }

        if (CommunicationTimeOutCounter < UINT32_MAX) {
            CommunicationTimeOutCounter++;
        }
    }
    
    /* Check the application's current state. */
    switch ( app_heatpump_commData.state )
    {
        // 1: Init
        case APP_HEATPUMP_COMM_STATE_INIT:
        {
            //if(setLoggingLock()){
                memset(TxBuffer, 0, TX_BUFFER_HEATPUMP_SIZE);
                memset(RxBuffer, 0, RX_BUFFER_HEATPUMP_SIZE);
            
                 //LedStatus_Set();
            
                SetOutput(LED_RX_HEATPUMP, false);
                SetOutput(LED_TX_HEATPUMP, false);
            
                startupSettingsCounter = FillBufferWithStartupSettings(doFirstTimeHeatpumpCommunicationSettings);
                //while(!releaseLoggingLock());
                app_heatpump_commData.commStatus = DISPLAY_COMM_STATUS_IDLE;
                app_heatpump_commData.state = APP_HEATPUMP_COMM_STATE_SEND_DATA;
            //}	
            break;
        }
        // 2: State start sending data
        case APP_HEATPUMP_COMM_STATE_SEND_DATA:
        {            
            //memcpy(&TxBuffer[0], testArray, 8);
            
            //if(setLoggingLock()){
            // Fill buffer with setting to send or with reading data request
                FillTxBuffer(&TxBuffer[0]);
                StartTransmittingDataToHeatpump(&TxBuffer[0]);
                //while(!releaseLoggingLock());
                app_heatpump_commData.state = APP_HEATPUMP_COMM_STATE_WAIT_FOR_DATA_SENT;
            //}
            
            break;
        }
        // 3: State wachten tot data verzonden is
        case APP_HEATPUMP_COMM_STATE_WAIT_FOR_DATA_SENT:
        {          
            if (app_heatpump_commData.commStatus == HEATPUMP_COMM_STATUS_DATA_SENT_TO_HEATPUMP)
            {
                SetOutput(LED_TX_HEATPUMP, false);
                app_heatpump_commData.state = APP_HEATPUMP_COMM_STATE_RECEIVE_DATA;
            }
            else{}
            break;
        }
        // 4: Start met ontvangen data
        case APP_HEATPUMP_COMM_STATE_RECEIVE_DATA:
        {            
            StartReceivingDataFromHeatpump();
            app_heatpump_commData.state = APP_HEATPUMP_COMM_STATE_WAIT_FOR_DATA_RECEIVED;
            break;
        }
        // 5: Wachten tot data ontvangen is
        case APP_HEATPUMP_COMM_STATE_WAIT_FOR_DATA_RECEIVED:
        {            
            if (app_heatpump_commData.commStatus == HEATPUMP_COMM_STATUS_DATA_RECEIVED_FROM_HEATPUMP)
            {
                SetOutput(LED_RX_HEATPUMP, false);
                
                app_heatpump_commData.state = APP_HEATPUMP_COMM_STATE_CHECKSUM_CHECK;
            }
            else{}
            break;
        }
        // 6: Checksum check
        case APP_HEATPUMP_COMM_STATE_CHECKSUM_CHECK:
        {            
            if (ChecksumCheck(&RxBuffer[0], CalculateModbusBufferSize(&RxBuffer[0])) == true)
            {
                app_heatpump_commData.state = APP_HEATPUMP_COMM_STATE_PARSE_DATA;
            }
            else
            {
                app_heatpump_commData.state = APP_HEATPUMP_COMM_STATE_INIT;
            }
            break;
        }
        // 7: Parse data
        case APP_HEATPUMP_COMM_STATE_PARSE_DATA:
        {            
            //if(setLoggingLock()){
                ParseHeatpumpData(&TxBuffer[0], &RxBuffer[0]);
                
                ResponseDelay = 0;
                CommunicationWindowSecondCounter = 0;
                CommunicationTimeOutCounter = 0;
                
                if ((doFirstTimeHeatpumpCommunicationSettings == true) && (startupSettingsCounter == UINT8_MAX))
                {
                    
                    doFirstTimeHeatpumpCommunicationSettings = false;
                    WriteSmartEeprom8(SEEP_ADDR_CONNECT_HAS_EVER_CONTACTED_HEATPUMP, true);
                }
                //while(!releaseLoggingLock()){}

                app_heatpump_commData.state = APP_HEATPUMP_COMM_STATE_DELAY;
            //}
            
            break;
        }
        // 8: Delay
        case APP_HEATPUMP_COMM_STATE_DELAY:
        {            
            if (ResponseDelay >= 5)
            {
                app_heatpump_commData.state = APP_HEATPUMP_COMM_STATE_INIT;
            }
            
            break;
        }
        /* TODO: implement your application state machine.*/

        /* The default state should never be executed. */
        default:
        {
            app_heatpump_commData.state = APP_HEATPUMP_COMM_STATE_INIT;
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}


/*******************************************************************************
 End of File
 */
