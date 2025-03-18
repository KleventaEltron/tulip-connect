#include <stddef.h>                    
#include <stdbool.h>                   
#include <stdlib.h>                   
#include <string.h>
#include <stdio.h>
#include "definitions.h"         

#include "config/default/system/fs/sys_fs.h"
//#include "system/fs/sys_fs.h"
#include "config/default/user.h"

#include "app_sd_card_tasks.h"
#include "files/eeprom.h"
#include "files/sdcard.h"
#include "files/logging.h"
#include "files/time_counters.h"
#include "files/alarms.h"
#include "files/modbus/display.h"
#include "files/modbus/heatpump_parameters.h"
#include "files/delay.h"

APP_SD_CARD_TASKS_DATA app_sd_card_tasksData;

char requestBody[2048];

bool tulipLogged = false;

bool heatpumpPresent = false;
bool heatpumpLogged = false;


APP_SD_CARD_TASKS_STATES getSdCardState(){
    return app_sd_card_tasksData.state;
}


void clearMemoryArray() {
    memset(requestBody, 0, sizeof(requestBody));
    return;
}



void APP_SD_CARD_TASKS_Initialize ( void )
{
    initializeMutex();
    app_sd_card_tasksData.state = APP_SD_CARD_TASKS_STATE_INIT;
    /* Handler to detect mounting of SD card */
    SYS_FS_EventHandlerSet((void const*)SYSTEM_SDCARD_MOUNT_HANDLER,(uintptr_t)NULL);
}



void APP_SD_CARD_TASKS_Tasks ( void )
{

    switch ( app_sd_card_tasksData.state )
    {

        
        
        case APP_SD_CARD_TASKS_STATE_INIT:
        {
            //app_sd_card_tasksData.state = APP_SD_CARD_TASKS_IDLE;
            break;
        }

        
        
        case APP_SD_CARD_TASKS_IDLE:
        {
            if (LoggingTimerSDCardExpired()) {
                //app_sd_card_tasksData.state = APP_SD_CARD_TASKS_WAIT_FOR_FS_MOUNT;
                //break;
            }
            break;
        }        
        
        
        
        case APP_SD_CARD_TASKS_WAIT_FOR_FS_MOUNT:
        {
            /* Wait for SDCARD to be Auto Mounted */
            if(SD_CARD_MOUNT_FLAG == true)
            {
                app_sd_card_tasksData.state = APP_SD_CARD_WAIT_FOR_LOGGING_UNLOCK;
            }            
            break;
        }
        
        
        
        case APP_SD_CARD_WAIT_FOR_LOGGING_UNLOCK:
        {
            if(setLoggingLock()){
                SYS_DEBUG_PRINT(SYS_ERROR_ERROR, "***** logging sd card *****\r\n");
                app_sd_card_tasksData.state = APP_SD_CARD_TASKS_CHECK_DIRECTORY_EXISTS;
            }
            break;
        }
        
        
        
        case APP_SD_CARD_TASKS_CHECK_DIRECTORY_EXISTS:
        {
            if(!checkIfDirectoryExists(SDCARD_LOGGING_DIRECTORY)) {
                SYS_DEBUG_PRINT(SYS_ERROR_ERROR, "***** created LOGFILES directory *****\r\n");
            }
            
            app_sd_card_tasksData.state = APP_SD_CARD_TASKS_OPEN_LOGFILE_JSON;
            break;
        }
        
        
        
        case APP_SD_CARD_TASKS_OPEN_LOGFILE_JSON:
        { 
            int lineCount = getFileLineCount(TWO_WEEK_LOGFILE_NAME);    
            if(lineCount >= MAX_FILE_LINE_COUNT) {
                fifoFileWriter(TWO_WEEK_LOGFILE_NAME, lineCount);
            }
            openFileJson(TWO_WEEK_LOGFILE_NAME);
            app_sd_card_tasksData.state = APP_SD_CARD_TASKS_WRITE_JSON_TO_FILE;
            break;
        }
        
        
        
        case APP_SD_CARD_TASKS_WRITE_JSON_TO_FILE:
        {
            clearMemoryArray();
            getRealTimeDataStatussen(requestBody);
            strcat(requestBody, "\n");
            write_String_To_File(fileHandle, requestBody);
            
            app_sd_card_tasksData.state = APP_SD_CARD_TASKS_CLOSE_LOGFILE_JSON;
            break;
        }
        
        
        
        case APP_SD_CARD_TASKS_CLOSE_LOGFILE_JSON:
        {
            SYS_FS_FileClose(fileHandle);
            app_sd_card_tasksData.state = APP_SD_CARD_TASKS_OPEN_LOGFILE_CSV;
            break;
        }
        
        
        
        case APP_SD_CARD_TASKS_OPEN_LOGFILE_CSV:
        {
            app_sd_card_tasksData.state = APP_SD_CARD_TASKS_WRITE_CSV_TO_FILE;
            if (!tulipLogged) {
                //SYS_DEBUG_PRINT(SYS_ERROR_ERROR, "***** LOGGING TULIP CSV *****\r\n");
                int lineCount = getFileLineCount(TULIP_LOG_FILE);     
                if(lineCount >= MAX_FILE_LINE_COUNT){
                   fifoFileWriter(TULIP_LOG_FILE, lineCount);
                }
                openFileCSV(TULIP_LOG_FILE, TULIP_PRINT );
                break;
            }
            
            /* HEATPUMP SHOULD BE CONNECTED */
            if (!GetAlarm(ALARM_HEATPUMP_COMMUNICATION) && !heatpumpLogged) {
                //SYS_DEBUG_PRINT(SYS_ERROR_ERROR, "***** LOGGING HEATPUMP CSV *****\r\n");
                int lineCount = getFileLineCount(HEATPUMP_LOG_FILE);     
                if(lineCount >= MAX_FILE_LINE_COUNT){
                   fifoFileWriter(HEATPUMP_LOG_FILE, lineCount);
                }
                openFileCSV(HEATPUMP_LOG_FILE, HEATPUMP); 
                break;
            }
            
            tulipLogged = false;
            heatpumpLogged = false;
            
            while(!releaseLoggingLock());
            
            app_sd_card_tasksData.state = APP_SD_CARD_TASKS_IDLE;
            
            break;
        }
        
        
        
        case APP_SD_CARD_TASKS_WRITE_CSV_TO_FILE:
        {
            clearMemoryArray();
            app_sd_card_tasksData.state = APP_SD_CARD_TASKS_CLOSE_LOGFILE_CSV;
            
            if (!tulipLogged) {    
                writeCsvLogToFile(TULIP_PRINT);
                tulipLogged = true;
                break;
            }
            
            if (!heatpumpLogged) {
                writeCsvLogToFile(HEATPUMP);
                heatpumpLogged = true;
                break;
            }
            break;
        }
        
        
        
        case APP_SD_CARD_TASKS_CLOSE_LOGFILE_CSV:
        {
            SYS_DEBUG_PRINT(SYS_ERROR_ERROR, "***** closing file *****\r\n");
            SYS_FS_FileClose(fileHandle);
            
            //if (!GetAlarm(ALARM_HEATPUMP_COMMUNICATION) && !heatpumpLogged) {
            if (!heatpumpLogged) {
               app_sd_card_tasksData.state = APP_SD_CARD_TASKS_OPEN_LOGFILE_CSV;
               break;
            }
            
            /*
             * TODO: DE REST VAN DE SYSTEMEN IMPLEMENTEREN
            if (!GetAlarm(BATTERY) && !heatpumpLogged) {
               app_sd_card_tasksData.state = APP_SD_CARD_TASKS_OPEN_LOGFILE_CSV;
               break;
            }
            */
             
            tulipLogged = false;
            heatpumpLogged = false;
            while(!releaseLoggingLock());
            
            app_sd_card_tasksData.state = APP_SD_CARD_TASKS_IDLE;
            break;
        }
        
        
        
        case APP_SD_CARD_TASKS_STATE_SERVICE_TASKS:
        {
            
            break;
        }


        
        default:
        {
            break;
        }
        
    }
    
}


/*******************************************************************************
 End of File
 */
