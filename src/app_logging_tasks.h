
#ifndef _APP_LOGGING_TASKS_H
#define _APP_LOGGING_TASKS_H

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
    APP_LOGGING_TASKS_STATE_INIT=0,
            
    APP_LOGGING_TASKS_IDLE,
            
    APP_LOGGING_TASKS_WAIT_FOR_TCPIP_INIT,
    
    APP_LOGGING_TASKS_WAIT_FOR_IP,
            
    APP_LOGGING_TASKS_WAIT_FOR_SNTP,
            
    APP_LOGGING_TASKS_PARSE_STRING_TO_IP,
            
    APP_LOGGING_TASKS_WAIT_ON_DNS,
            
    APP_LOGGING_TASKS_START_CONNECTION,
            
    APP_LOGGING_TASKS_WAIT_FOR_CONNECTION,
            
    APP_LOGGING_TASKS_WAIT_FOR_SSL_CONNECT,
            
    APP_LOGGING_TASKS_SEND_REQUEST_SSL,
            
    APP_LOGGING_TASKS_WAIT_FOR_RESPONSE_SSL,
            
    APP_LOGGING_TASKS_CLOSE_CONNECTION,
            
    APP_LOGGING_TASKS_WAIT_FOR_LOGGING_UNLOCK,  
            
} APP_LOGGING_TASKS_STATES;



typedef struct
{
    APP_LOGGING_TASKS_STATES state;
} APP_LOGGING_TASKS_DATA;




void APP_LOGGING_TASKS_Initialize ( void );
void APP_LOGGING_TASKS_Tasks( void );





//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif /* _APP_LOGGING_TASKS_H */

/*******************************************************************************
 End of File
 */

