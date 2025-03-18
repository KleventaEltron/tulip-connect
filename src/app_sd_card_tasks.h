/*******************************************************************************
  MPLAB Harmony Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    app_sd_card_tasks.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "APP_SD_CARD_TASKS_Initialize" and "APP_SD_CARD_TASKS_Tasks" prototypes) and some of them are only used
    internally by the application (such as the "APP_SD_CARD_TASKS_STATES" definition).  Both
    are defined here for convenience.
*******************************************************************************/

#ifndef _APP_SD_CARD_TASKS_H
#define _APP_SD_CARD_TASKS_H

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

    
    
typedef enum
{
    /* Application's state machine's initial state. */
    APP_SD_CARD_TASKS_STATE_INIT=0,
    APP_SD_CARD_TASKS_IDLE,
            
    APP_SD_CARD_WAIT_FOR_LOGGING_UNLOCK,        
            
    APP_SD_CARD_TASKS_FS_MOUNT,
    APP_SD_CARD_TASKS_FS_UNMOUNT,
    APP_SD_CARD_TASKS_WAIT_FOR_FS_MOUNT,
            
    APP_SD_CARD_TASKS_CHECK_DIRECTORY_EXISTS,
            
    APP_SD_CARD_TASKS_OPEN_LOGFILE_JSON,
    APP_SD_CARD_TASKS_CLOSE_LOGFILE_JSON,
    APP_SD_CARD_TASKS_WRITE_JSON_TO_FILE,
            
    APP_SD_CARD_TASKS_OPEN_LOGFILE_CSV,
    APP_SD_CARD_TASKS_CLOSE_LOGFILE_CSV,
    APP_SD_CARD_TASKS_WRITE_CSV_TO_FILE,
            
    APP_SD_CARD_TASKS_STATE_SERVICE_TASKS,
} APP_SD_CARD_TASKS_STATES;



typedef struct
{
    /* The application's current state */
    APP_SD_CARD_TASKS_STATES state;

} APP_SD_CARD_TASKS_DATA;



void APP_SD_CARD_TASKS_Initialize ( void );
void APP_SD_CARD_TASKS_Tasks( void );
APP_SD_CARD_TASKS_STATES getSdCardState();



//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif /* _APP_SD_CARD_TASKS_H */

/*******************************************************************************
 End of File
 */

