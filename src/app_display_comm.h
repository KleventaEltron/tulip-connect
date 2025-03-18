/*******************************************************************************
  MPLAB Harmony Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    app_display_comm.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "APP_DISPLAY_COMM_Initialize" and "APP_DISPLAY_COMM_Tasks" prototypes) and some of them are only used
    internally by the application (such as the "APP_DISPLAY_COMM_STATES" definition).  Both
    are defined here for convenience.
*******************************************************************************/

#ifndef _APP_DISPLAY_COMM_H
#define _APP_DISPLAY_COMM_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "configuration.h"
#include "definitions.h"

#define RX_BUFFER_DISPLAY_SIZE 255
#define TX_BUFFER_DISPLAY_SIZE 255

#define THIS_DEVICE_ADDRESS 0x01

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
    DISPLAY_COMM_STATUS_IDLE,
    DISPLAY_COMM_STATUS_WAITING_FOR_DATA_FROM_DISPLAY,
    DISPLAY_COMM_STATUS_DEVICE_ADDRESS_RECEIVED,       
    DISPLAY_COMM_STATUS_104_RECEIVED, 
    DISPLAY_COMM_STATUS_255_RECEIVED,  
    DISPLAY_COMM_STATUS_FIRST_8_BYTES_RECEIVED,
    DISPLAY_COMM_STATUS_DATA_RECEIVED_FROM_DISPLAY,
    DISPLAY_COMM_STATUS_UNKNOWN_DATA_RECEIVED_FROM_DISPLAY,   
    DISPLAY_COMM_STATUS_SENDING_DATA_TO_DISPLAY,        
    DISPLAY_COMM_STATUS_DATA_SENT_TO_DISPLAY,       

} DISPLAY_COMMUNICATION_STATUS;

typedef enum
{
    APP_DISPLAY_COMM_STATE_INIT=0,
    APP_DISPLAY_COMM_STATE_RECEIVE_DATA,  
    APP_DISPLAY_COMM_STATE_WAIT_FOR_DATA_RECEIVED,        
    APP_DISPLAY_COMM_STATE_CHECKSUM_CHECK,         
    APP_DISPLAY_COMM_STATE_PARSE_DATA,        
    APP_DISPLAY_COMM_STATE_DELAY,       
    APP_DISPLAY_COMM_STATE_PREPARING_DATA_TO_SENT,        
    APP_DISPLAY_COMM_STATE_SEND_DATA, 
    APP_DISPLAY_COMM_STATE_WAIT_FOR_DATA_SENT,        
} APP_DISPLAY_COMM_STATES;


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
    /* The application's current state */
    APP_DISPLAY_COMM_STATES state;
    DISPLAY_COMMUNICATION_STATUS commStatus;
    /* TODO: Define any additional data used by the application. */

} APP_DISPLAY_COMM_DATA;

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
    void APP_DISPLAY_COMM_Initialize ( void )

  Summary:
     MPLAB Harmony application initialization routine.

  Description:
    This function initializes the Harmony application.  It places the
    application in its initial state and prepares it to run so that its
    APP_DISPLAY_COMM_Tasks function can be called.

  Precondition:
    All other system initialization routines should be called before calling
    this routine (in "SYS_Initialize").

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    APP_DISPLAY_COMM_Initialize();
    </code>

  Remarks:
    This routine must be called from the SYS_Initialize function.
*/

void APP_DISPLAY_COMM_Initialize ( void );


/*******************************************************************************
  Function:
    void APP_DISPLAY_COMM_Tasks ( void )

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
    APP_DISPLAY_COMM_Tasks();
    </code>

  Remarks:
    This routine must be called from SYS_Tasks() routine.
 */

void APP_DISPLAY_COMM_Tasks( void );

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif /* _APP_DISPLAY_COMM_H */

/*******************************************************************************
 End of File
 */

