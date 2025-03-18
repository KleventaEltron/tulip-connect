/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_i2c_tasks.c

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
#include <stdio.h>
#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include <string.h>
#include "definitions.h"                // SYS function prototypes
#include "app_i2c_tasks.h"

#include "files/i2c/mac.h"
#include "files\hardware_rev.h"

#include "tcpip/tcpip.h"
#include "files/logging.h"

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
    This structure should be initialized by the APP_I2C_TASKS_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

APP_I2C_TASKS_DATA app_i2c_tasksData;

volatile APP_TRANSFER_STATUS_EEPROM transferStatus = APP_TRANSFER_STATUS_ERROR;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

void APP_I2CCallback(uintptr_t context )
{
    APP_TRANSFER_STATUS_EEPROM* transferStatus = (APP_TRANSFER_STATUS_EEPROM*)context;

    if(SERCOM3_I2C_ErrorGet() == SERCOM_I2C_ERROR_NONE)
    {
        if (transferStatus)
        {
            *transferStatus = APP_TRANSFER_STATUS_SUCCESS;
        }
    }
    else
    {
        if (transferStatus)
        {
            *transferStatus = APP_TRANSFER_STATUS_ERROR;
        }
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************


/* TODO:  Add any necessary local functions.
*/


// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_I2C_TASKS_Initialize ( void )

  Remarks:
    See prototype in app_i2c_tasks.h.
 */

void APP_I2C_TASKS_Initialize ( void )
{
    SERCOM3_I2C_CallbackRegister( APP_I2CCallback, (uintptr_t)&transferStatus );
    
    // Place the App state machine in its initial state.    
    app_i2c_tasksData.state = APP_I2C_TASKS_STATE_INIT;
}


/******************************************************************************
  Function:
    void APP_I2C_TASKS_Tasks ( void )

  Remarks:
    See prototype in app_i2c_tasks.h.
 */

void APP_I2C_TASKS_Tasks ( void )
{

    /* Check the application's current state. */
    switch ( app_i2c_tasksData.state )
    {
        /* Application's initial state. */
        case APP_I2C_TASKS_STATE_INIT:
        {
            bool appInitialized = true;


            if (appInitialized)
            {

                app_i2c_tasksData.state = APP_I2C_TASKS_STATE_GET_EUI;
            }
            break;
        }

        case APP_I2C_TASKS_STATE_GET_EUI:
        {
            switch (macState)
            {
                case MAC_STATE_EEPROM_STATUS_VERIFY:
                {
                   // Verify if EEPROM is ready to accept new requests 
                    transferStatus = APP_TRANSFER_STATUS_IN_PROGRESS;
                    SERCOM3_I2C_Write(AT24MAC_DEVICE_ADDR_EEPROM, &ackData, AT24MAC_ACK_DATA_LENGTH);
                    macState = MAC_STATE_EEPROM_WRITE;
                    break;
                }
                case MAC_STATE_EEPROM_WRITE:
                {
                    if (transferStatus == APP_TRANSFER_STATUS_SUCCESS)
                    {
                        // Read data 
                        macState = MAC_STATE_EEPROM_READ;
                    }
                    else if (transferStatus == APP_TRANSFER_STATUS_ERROR)
                    {
                        // EEPROM is not ready to accept new requests. 
                        // Keep checking until the EEPROM becomes ready. 
                        macState = MAC_STATE_EEPROM_STATUS_VERIFY;
                    }
                    break;
                }
                case MAC_STATE_EEPROM_READ:
                {
                    transferStatus = APP_TRANSFER_STATUS_IN_PROGRESS;
                    // Read the data from the page written earlier 
                    if (RevNum == 1) {
                        SERCOM3_I2C_WriteRead(AT24MAC_DEVICE_ADDR_EUI, &addressEui48, AT24MAC_RECEIVE_DUMMY_WRITE_LENGTH, &eui48[0], EUI_48_BYTES_LENGTH);
                    } else {
                        SERCOM3_I2C_WriteRead(AT24MAC_DEVICE_ADDR_EUI, &addressEui64, AT24MAC_RECEIVE_DUMMY_WRITE_LENGTH, &eui64[0], EUI_64_BYTES_LENGTH);
                    }
                    macState = MAC_STATE_EEPROM_WAIT_READ_COMPLETE;
                    break;
                }
                case MAC_STATE_EEPROM_WAIT_READ_COMPLETE:
                {
                    if (transferStatus == APP_TRANSFER_STATUS_SUCCESS)
                    {
                        macState = MAC_STATE_VERIFY;
                    }
                    else if (transferStatus == APP_TRANSFER_STATUS_ERROR)
                    {
                        macState = MAC_STATE_XFER_ERROR;
                    }
                    break;
                }
                case MAC_STATE_VERIFY:
                {
                    if (RevNum == 1) {
                        if (eui48[0] == 0xFC && eui48[1] == 0xC2 && eui48[2] == 0x3D)
                        {
                            for (uint8_t i = 0; i < 3; i++) 
                            {
                                eui64[i] = eui48[i];
                            }

                            eui64[3] = 0xFF;
                            eui64[4] = 0xFE;

                            for (uint8_t i = 3; i < 6; i++) 
                            {
                                eui64[i+2] = eui48[i];
                            }

                            eui64Filled = true;

                            macState = MAC_STATE_XFER_SUCCESSFUL;
                        }
                        else
                        {
                            macState = MAC_STATE_XFER_ERROR;
                        }
                    } else {
                        if (eui64[0] == 0xFC && eui64[1] == 0xC2 && eui64[2] == 0x3D) {
                            eui64Filled = true;
                            SYS_CONSOLE_PRINT("I2C stack creation \r\n");
                            macState = MAC_STATE_XFER_SUCCESSFUL;
                        } else {
                            macState = MAC_STATE_XFER_ERROR;
                        }
                    }
                    break;
                }
                case MAC_STATE_XFER_SUCCESSFUL:
                {
                    TCPIP_NET_HANDLE netH = TCPIP_STACK_NetHandleGet("eth0");
                    
                    if (netH == NULL) {
                        //SYS_CONSOLE_PRINT("Error: Network interface not found! Setting up new interface\n");
                        if(!setupNewTcpipStack()){
                            break;
                        }
                    }  
                    
                    if (!TCPIP_STACK_NetIsUp(netH)) {      
                        break;
                    }
                    
                    SYS_CONSOLE_PRINT("*** NETWORK STACK CONFIGURED ***\r\n");
                    
                   macState = MAC_STATE_XFER_ERROR;
                   break;
                }
                case MAC_STATE_XFER_ERROR:
                {
                    //LedAlarm_Set();
                    break;
                }
                default:
                    break;
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