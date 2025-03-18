#include <xc.h>
#include <string.h>
#include <stdbool.h>                 
#include "definitions.h"
#include "system/fs/sys_fs.h"

#include "sdcard.h"
#include "../config/default/user.h"
#include "modbus/heatpump_parameters.h"
#include "eeprom.h"
#include "modbus/display.h"
#include "time_counters.h"


bool SD_CARD_MOUNT_FLAG = false;
SYS_FS_HANDLE fileHandle;

char parameterCSV   [100];
char hardwareId     [50];



/*
 * Writes a string to the logfiles.
 */
void write_String_To_File(SYS_FS_HANDLE fileHandle, char buffer[]) {
    if(SYS_FS_FileStringPut(fileHandle, buffer) == -1) {
        SYS_FS_FileClose(fileHandle);
        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "----- Write FileStringPut was not succesfull -----\r\n");
    }
    return;    
}



/*
 * Writes a value to a logfile after first converting it to a file.
 */
void write_String_To_File_Value(SYS_FS_HANDLE fileHandle, int value) {
    char buffer[30];
    sprintf(buffer, "%i,", value);
    
    if(SYS_FS_FileStringPut(fileHandle, buffer) == -1) {
        SYS_FS_FileClose(fileHandle);
        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "----- Write FileStringPut was not succesfull -----\r\n");
    }
    return;    
}



/*
 * Handles the mounting and unmounting of the SD card. Sets the SD_CARD_MOUNT_FLAG appropriatly.
 */
void SYSTEM_SDCARD_MOUNT_HANDLER(SYS_FS_EVENT event, void* eventData,uintptr_t context)
{
    switch(event)
    {
        /* If the event is mount then check if SDCARD media has been mounted */
        case SYS_FS_EVENT_MOUNT:
            if(strcmp((const char *)eventData, SDCARD_MOUNT_NAME) == 0)
            {
                SD_CARD_MOUNT_FLAG = true;
            }
            break;

        /* If the event is unmount then check if SDCARD media has been unmount */
        case SYS_FS_EVENT_UNMOUNT:
            if(strcmp((const char *)eventData, SDCARD_MOUNT_NAME) == 0)
            {
                SD_CARD_MOUNT_FLAG = false;
            }
            break;

        case SYS_FS_EVENT_ERROR:
            break;
            
        default:
            break;
    }
    return;
}



/*
 * Returns true or false based on the existance of the directory.
 */
bool checkIfDirectoryExists(char directoryName[]) 
{  
    bool ret = false;
    if(SYS_FS_DirectoryMake(directoryName) == SYS_FS_RES_FAILURE) {
        ret = true;
    } 
    return ret;
}


void resetWatchdogTimer() {
    WDT_Clear(); 
}


/*
 * Opens the JSON logfile. Prints an error if the file could not be opened. 
 */
bool openFileJson( char fileName[] ) {
    char pathToFile[80];
    strcpy(pathToFile, SDCARD_MOUNT_NAME);
    strcat(pathToFile, SDCARD_LOGGING_DIRECTORY);
    strcat(pathToFile, fileName);
            
    fileHandle = SYS_FS_FileOpen(pathToFile, (SYS_FS_FILE_OPEN_APPEND));
    if(fileHandle == SYS_FS_HANDLE_INVALID)
    {
        SYS_FS_FileClose(fileHandle);
        SYS_DEBUG_PRINT(SYS_ERROR_ERROR, "----- Error opening bootloader logfile ----- \r\n"); 
        return false;
    }    
    
    return true;
}



/*
 * Opens a CSV logfile and throws an error if it does not exist.
 * In case the file is empty, we first write the header row to the file
 * linked to the specific device type.
 */
bool openFileCSV(char fileName[], DEVICE_TYPE dvt) {
    char pathToFile[80];
    strcpy(pathToFile, SDCARD_MOUNT_NAME);
    strcat(pathToFile, SDCARD_LOGGING_DIRECTORY);
    strcat(pathToFile, fileName);
            
    fileHandle = SYS_FS_FileOpen(pathToFile, (SYS_FS_FILE_OPEN_APPEND));
    if(fileHandle == SYS_FS_HANDLE_INVALID)
    {
        SYS_FS_FileClose(fileHandle);
        SYS_DEBUG_PRINT(SYS_ERROR_ERROR, "----- Error opening bootloader logfile ----- \r\n"); 
        return false;
    } 
    
    if(SYS_FS_FileSize(fileHandle) <= 0) {
        switch (dvt) {
            case TULIP_PRINT:{
                write_String_To_File(fileHandle, "TL1,TL2,TL3,TL4,TL5,TL6,TL7,TL8,TL9,TL10,TL11,TL12,TL13,TL14,TL15,TL16,TL17,TL18,TL19,TL20,"
                        "TL21,TL22,TL23,TL24,TL25,TL26,TL27,TL28,TL29,TL30,TL31,TL32,TL33,TL34,TL35,TL36,TL37,TL38,TL39,TL40,TL41,TL42\n"); 
                break;
            }

            case HEATPUMP:{
                write_String_To_File(fileHandle, "WL1,WL2,WL3,WL4,WL5,WL6,WL7,WL8,WL9,WL10,WL11,WL12,WL13,WL14,WL15,WL16,WL17,WL18,WL19,"
                        "WL20,WL21,WL22,WL23,WL24,WL25,WL26,WL27,WL28,WL29,WL30,WL31,WL32,WL33,WL34,WL35,WL36,WL37,WL38,WL39,WL40,WL41,"
                        "WL42,WL43,WL44,WL45,WL46,WL47,WL48,WL49,WL50,WL51,WL52,WL53,WL54,WL55,WL56,WL57,WL58,WL59,WL60,WL61,WL62,WL63\n"); 
                break;
            }

            case BATTERY:{
                break;
            }

            case SMART_METER:{
                break;
            }

            case INVERTER:{
                break;
            }

            case HEATPUMP_BOILER:{
                break;
            }

            /* ZOU NOOIT GECALLED MOETEN WORDEN */
            default:{
                break;
            }
        }    
    }
    return true;
}



/*
 * Returns the linecount for a given input file. This count is later used for FIFO file writing 
 * This count is based on a given interval. For example: 20 sec per log for 2 weeks of data
 * Results in ~ 60.000 logs in a file at a given time.
 */
int getFileLineCount( char fileName[] ) {
    char pathToFile[80];
    strcpy(pathToFile, SDCARD_MOUNT_NAME);
    strcat(pathToFile, SDCARD_LOGGING_DIRECTORY);
    strcat(pathToFile, fileName);
    
    SYS_FS_HANDLE fileHandle = SYS_FS_FileOpen(pathToFile, SYS_FS_FILE_OPEN_READ);
    if (fileHandle == SYS_FS_HANDLE_INVALID)
    {
        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "----- Could not obtain line count for file -----\r\n");
        return 0;
    }

    //char buffer;
    char buffer[8192];  // Use a larger buffer to read in chunks
    int lineCount = 0;
    int bytesRead;
    // Read the file character by character
    while ((bytesRead = SYS_FS_FileRead(fileHandle, buffer, sizeof(buffer))) > 0) {        
        UpdateCounters();
        for (int i = 0; i < bytesRead; i++) {
            if (buffer[i] == '\n') {
                lineCount++;
            }
        }
    }
    // Close the file
    SYS_FS_FileClose(fileHandle);    
    return lineCount;
}



/*
 * Gets called when the linecount exceeds the maximum allowed limit of a given file.
 * Opens the file, and a temporary file. It writes the header, skips the first data row
 * and writes the rest of the data into the temporary file. After this it deletes the old file
 * and renames the temporary file into the fileName of the old file. Works based
 * on the FIFO (First In First Out) mechanics. 
 */
void fifoFileWriter( char fileName[], int lineCount ) {
    char pathToFile[80];
    char tempfilePath[100];
    
    // Old file path
    strcpy(pathToFile, SDCARD_MOUNT_NAME);
    strcat(pathToFile, SDCARD_LOGGING_DIRECTORY);
    strcat(pathToFile, fileName);
    
    // New file path
    strcpy(tempfilePath, SDCARD_MOUNT_NAME);
    strcat(tempfilePath, SDCARD_LOGGING_DIRECTORY);
    strcat(tempfilePath, TEMP_FILE_NAME);
    
    // Open the old file
    SYS_FS_HANDLE fileHandleCurrent = SYS_FS_FileOpen(pathToFile, SYS_FS_FILE_OPEN_READ);
    if (fileHandleCurrent == SYS_FS_HANDLE_INVALID)
    {
        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "----- File filehandle open was not succesfull -----\r\n");
    }
    
    // Open the new file
    SYS_FS_HANDLE TempfileHandle = SYS_FS_FileOpen(tempfilePath, SYS_FS_FILE_OPEN_WRITE);
    if (TempfileHandle == SYS_FS_HANDLE_INVALID)
    {
        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "----- File TEMP filehandle open was not succesfull  -----\r\n");
    }
    
    char single_Char_buffer;
    // Writes the header of the CSV file to the temporary file
    while (SYS_FS_FileRead(fileHandleCurrent, &single_Char_buffer, 1) > 0) {
        if(SYS_FS_FileCharacterPut(TempfileHandle, single_Char_buffer) == -1) {
            SYS_FS_FileClose(fileHandleCurrent);
            SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "----- Write write character was not succesfull -----\r\n");
        }
        if (single_Char_buffer == '\n'){break;}
    }
    
    int currentLine = 0;
    while (SYS_FS_FileRead(fileHandleCurrent, &single_Char_buffer, 1) > 0) {          
        if (currentLine < lineCount - MAX_FILE_LINE_COUNT){
            if (single_Char_buffer == '\n'){
                currentLine++;
            }
            continue;
        }
         if (single_Char_buffer == '\n'){break;}
    }
    
    char buffer[8192];
    int bytesRead = 0;
    while ((bytesRead = SYS_FS_FileRead(fileHandleCurrent, buffer, sizeof(buffer))) > 0) {
            if (SYS_FS_FileWrite(TempfileHandle, buffer, bytesRead) != bytesRead) {
                SYS_FS_FileClose(fileHandleCurrent);
                SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "----- Write operation failed -----\r\n");
                break;
            }
            UpdateCounters();
            resetWatchdogTimer();   
    }      
    
    SYS_FS_FileClose(fileHandleCurrent);
    SYS_FS_FileClose(TempfileHandle);
    
    if(SYS_FS_FileDirectoryRemove(pathToFile) == SYS_FS_RES_FAILURE){
        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "OOPSIE WOOPSIE!\r\n");
    }
    
    if(SYS_FS_FileDirectoryRenameMove(tempfilePath, pathToFile) == SYS_FS_RES_FAILURE){
        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "Failed to replace file!\r\n");
    }

    return;
}




/*
 * Writes a single complete log entry into a given CSV file based on the passed DEVICE_TYPE.
 */
void writeCsvLogToFile(DEVICE_TYPE dvt) {
    
    switch(dvt){
        case TULIP_PRINT:{
            setHardwareId( TULIP_PRINT, hardwareId, 1 );
            sprintf(parameterCSV, "%s,", hardwareId);
            write_String_To_File(fileHandle, parameterCSV);
            memset(parameterCSV, 0, sizeof (parameterCSV));
                
            write_String_To_File(fileHandle, "TEST,");
              
            sprintf(parameterCSV, "%d-%d-%d,", (int)((THIS_FIRMWARE_VERSION / 1000000)), (int)((THIS_FIRMWARE_VERSION / 1000) % 1000), (int)(THIS_FIRMWARE_VERSION % 1000));
            write_String_To_File(fileHandle, parameterCSV);
            memset(parameterCSV, 0, sizeof (parameterCSV));
                 
            uint32_t BOOTLOADER_FIRMWARE_VERSION = ReadSmartEeprom32(BOOTLOADER_SOFTWARE_VERSION);
            sprintf(parameterCSV, "%d-%d-%d,", (int)((BOOTLOADER_FIRMWARE_VERSION / 1000000)), (int)((BOOTLOADER_FIRMWARE_VERSION / 1000) % 1000), (int)(BOOTLOADER_FIRMWARE_VERSION % 1000));
            write_String_To_File(fileHandle, parameterCSV);
            memset(parameterCSV, 0, sizeof (parameterCSV));
                
            /* For both time logs */
            sprintf(parameterCSV, "%s,", NTP_TIME_BUFFER);
            write_String_To_File(fileHandle, parameterCSV);
            write_String_To_File(fileHandle, parameterCSV);
            memset(parameterCSV, 0, sizeof (parameterCSV));
                
            write_String_To_File_Value(fileHandle, 0);
            write_String_To_File_Value(fileHandle, 0);
            write_String_To_File_Value(fileHandle, 0);
            write_String_To_File_Value(fileHandle, 0);
            write_String_To_File_Value(fileHandle, 0);
            write_String_To_File_Value(fileHandle, GetThermostatContact());
            write_String_To_File_Value(fileHandle, GetDigitalInput2());
                
            write_String_To_File_Value(fileHandle, GetDigitalInput3());
            write_String_To_File_Value(fileHandle, getStatusHeatingElementHeatingBuffer());
            write_String_To_File_Value(fileHandle, getStatusHeatingElementHotWaterBuffer());
            write_String_To_File_Value(fileHandle, false);
            write_String_To_File_Value(fileHandle, false);
            write_String_To_File_Value(fileHandle, false);
            write_String_To_File_Value(fileHandle, getStatusCirculationPump());
            write_String_To_File_Value(fileHandle, getStatus3WayValve());
            write_String_To_File_Value(fileHandle, false);
            write_String_To_File_Value(fileHandle, false);      
                
            write_String_To_File_Value(fileHandle, true);
            write_String_To_File_Value(fileHandle, GetDip1());
            write_String_To_File_Value(fileHandle, GetDip2());
            write_String_To_File_Value(fileHandle, GetDip3());
            write_String_To_File_Value(fileHandle, GetDip4());
            write_String_To_File_Value(fileHandle, false);
            write_String_To_File(fileHandle, "N,");   
            write_String_To_File_Value(fileHandle, 0);
            write_String_To_File_Value(fileHandle, true);
            write_String_To_File_Value(fileHandle, 0);     
                
            write_String_To_File(fileHandle, "N,");
            write_String_To_File(fileHandle, "N,");
            write_String_To_File_Value(fileHandle, 0);  
            write_String_To_File_Value(fileHandle, 0);  
            write_String_To_File_Value(fileHandle, false);
            write_String_To_File_Value(fileHandle, false);
            write_String_To_File_Value(fileHandle, true);
            write_String_To_File_Value(fileHandle, true);
            write_String_To_File_Value(fileHandle, 0);                 

            write_String_To_File(fileHandle, "\n");            
            break;
        }

        case HEATPUMP:{
            write_String_To_File_Value(fileHandle, getDataFromMemoryCallable(ADDRESS_RUNNING_STATUS_1));
            write_String_To_File_Value(fileHandle, getDataFromMemoryCallable(ADDRESS_RUNNING_STATUS_2));
            write_String_To_File_Value(fileHandle, getDataFromMemoryCallable(ADDRESS_FAULT_STATE_1));
            write_String_To_File_Value(fileHandle, getDataFromMemoryCallable(ADDRESS_FAULT_STATE_2));
            write_String_To_File_Value(fileHandle, getDataFromMemoryCallable(ADDRESS_FAULT_STATE_3));
            write_String_To_File_Value(fileHandle, getDataFromMemoryCallable(ADDRESS_SYSTEM_1_FAULT_STATE_1));
            write_String_To_File_Value(fileHandle, getDataFromMemoryCallable(ADDRESS_SYSTEM_1_FAULT_STATE_2));
            write_String_To_File_Value(fileHandle, getDataFromMemoryCallable(ADDRESS_SYSTEM_1_DRIVE_FAULT_STATE_1));
            write_String_To_File_Value(fileHandle, getDataFromMemoryCallable(ADDRESS_SYSTEM_1_DRIVE_FAULT_STATE_2));
            write_String_To_File_Value(fileHandle, getDataFromMemoryCallable(ADDRESS_SYSTEM_1_DRIVE_FAULT_STATE_3));

            write_String_To_File_Value(fileHandle, getDataFromMemoryCallable(ADDRESS_RELAY_OUTPUT_STATUS_1));
            write_String_To_File_Value(fileHandle, getDataFromMemoryCallable(ADDRESS_RELAY_OUTPUT_STATUS_2));
            write_String_To_File_Value(fileHandle, getDataFromMemoryCallable(ADDRESS_RELAY_OUTPUT_STATUS_3));
            write_String_To_File_Value(fileHandle, getDataFromMemoryCallable(ADDRESS_RELAY_OUTPUT_STATUS_4));
            write_String_To_File_Value(fileHandle, getDataFromMemoryCallable(ADDRESS_SWITCH_PORT_STATE_1));
            write_String_To_File_Value(fileHandle, getDataFromMemoryCallable(ADDRESS_SWITCH_PORT_STATE_2));
            write_String_To_File_Value(fileHandle, getDataFromMemoryCallable(ADDRESS_SWITCH_PORT_STATE_3));
            write_String_To_File_Value(fileHandle, getDataFromMemoryCallable(ADDRESS_SWITCH_PORT_STATE_4));
            write_String_To_File_Value(fileHandle, getDataFromMemoryCallable(ADDRESS_CURRENT_UNIT_TOOLING_NO));
            write_String_To_File_Value(fileHandle, (int16_t)getDataFromMemoryCallable(ADDRESS_COMPRESSOR_1_TARGET_FREQUENCY));

            write_String_To_File_Value(fileHandle, (int16_t)getDataFromMemoryCallable(ADDRESS_COMPRESSOR_OPERATING_FREQUENCY));
            write_String_To_File_Value(fileHandle, (int16_t)getDataFromMemoryCallable(ADDRESS_FAN_OPERATING_FREQUENCY_ROTATIONAL_SPEED));
            write_String_To_File_Value(fileHandle, (int16_t)getDataFromMemoryCallable(ADDRESS_ELECTRONIC_EXPANSION_VALVE_STEPS_COUNT));
            write_String_To_File_Value(fileHandle, (int16_t)getDataFromMemoryCallable(ADDRESS_NUMBER_OF_EVI_VALVE_STEPS));
            write_String_To_File_Value(fileHandle, (int16_t)getDataFromMemoryCallable(ADDRESS_AC_INPUT_VOLTAGE));
            write_String_To_File_Value(fileHandle, (int16_t)getDataFromMemoryCallable(ADDRESS_AC_INPUT_CURRENT));
            write_String_To_File_Value(fileHandle, (int16_t)getDataFromMemoryCallable(ADDRESS_COMPRESSOR_PHASE_CURRENT));
            write_String_To_File_Value(fileHandle, (int16_t)getDataFromMemoryCallable(ADDRESS_COMPRESSOR_IPM_TEMPERATURE));
            write_String_To_File_Value(fileHandle, (int16_t)getDataFromMemoryCallable(ADDRESS_HIGH_PRESSURE_SATURATION_TEMPERATURE));
            write_String_To_File_Value(fileHandle, (int16_t)getDataFromMemoryCallable(ADDRESS_LOW_PRESSURE_SATURATION_TEMPERATURE));

            write_String_To_File_Value(fileHandle, (int16_t)getDataFromMemoryCallable(ADDRESS_EXTERNAL_AMBIENT_TEMPERATURE_T1));
            write_String_To_File_Value(fileHandle, (int16_t)getDataFromMemoryCallable(ADDRESS_EXTERNAL_COIL_TUBE_FIN_T2));
            write_String_To_File_Value(fileHandle, (int16_t)getDataFromMemoryCallable(ADDRESS_INTERNAL_COIL_TUBE_PLATE_REPLACEMENT_T3));
            write_String_To_File_Value(fileHandle, (int16_t)getDataFromMemoryCallable(ADDRESS_RETURN_AIR_TEMPERATURE_T4));
            write_String_To_File_Value(fileHandle, (int16_t)getDataFromMemoryCallable(ADDRESS_EXHAUST_TEMPERATURE_T5));
            write_String_To_File_Value(fileHandle, (int16_t)getDataFromMemoryCallable(ADDRESS_RETURN_WATER_TEMPERATURE_T6));
            write_String_To_File_Value(fileHandle, (int16_t)getDataFromMemoryCallable(ADDRESS_WATER_OUTLET_TEMPERATURE_T7));
            write_String_To_File_Value(fileHandle, (int16_t)getDataFromMemoryCallable(ADDRESS_ECONOMIZER_INLET_TUBE_T8));
            write_String_To_File_Value(fileHandle, (int16_t)getDataFromMemoryCallable(ADDRESS_ECONOMIZER_OUTLET_TUBE_T9));
            write_String_To_File_Value(fileHandle, (int16_t)getDataFromMemoryCallable(ADDRESS_WATER_TANK_TEMPERATURE));

            write_String_To_File_Value(fileHandle, (int16_t)getDataFromMemoryCallable(ADDRESS_FLUORINE_OUTLET_TEMPERATURE_OF_PLATE_HEAT_EXCHANGER));
            write_String_To_File_Value(fileHandle, (int16_t)getDataFromMemoryCallable(ADDRESS_DRIVE_MANUFACTURER));
            write_String_To_File_Value(fileHandle, (int16_t)getDataFromMemoryCallable(ADDRESS_PUMP_SPEED_PWM));
            write_String_To_File_Value(fileHandle, (int16_t)getDataFromMemoryCallable(ADDRESS_WATER_FLOW));
            write_String_To_File_Value(fileHandle, (int16_t)getDataFromMemoryCallable(ADDRESS_USER_RETURN_WATER_TEMPERATURE));
            write_String_To_File_Value(fileHandle, (int16_t)getDataFromMemoryCallable(ADDRESS_DEVICE_INPUT_VOLTAGE));
            write_String_To_File_Value(fileHandle, (int16_t)getDataFromMemoryCallable(ADDRESS_DEVICE_INPUT_CURRENT));
            write_String_To_File_Value(fileHandle, (int16_t)getDataFromMemoryCallable(ADDRESS_DEVICE_INPUT_POWER_KW));
            write_String_To_File_Value(fileHandle, (int16_t)getDataFromMemoryCallable(ADDRESS_TOTAL_UNIT_ELECTRICITY_CONSUMPTION_KWH));
            write_String_To_File_Value(fileHandle, (int16_t)getDataFromMemoryCallable(ADDRESS_AUXILIARY_HEATING_SOURCE_HOT_WATER_TEMPERATURE_VALUE));

            write_String_To_File_Value(fileHandle, (int16_t)getDataFromMemoryCallable(ADDRESS_AUXILIARY_HEATING_SOURCE_HEATING_TEMPERATURE_VALUE));
            write_String_To_File_Value(fileHandle, (int16_t)getDataFromMemoryCallable(ADDRESS_BUFFER_TANK_FOR_HEATING_TEMPERATURE_VALUE));
            write_String_To_File_Value(fileHandle, (int16_t)getDataFromMemoryCallable(ADDRESS_MAIN_OUTLET_WATER_TEMPERATURE_VALUE));
            write_String_To_File_Value(fileHandle, (int16_t)getDataFromMemoryCallable(ADDRESS_WATER_INLET_TEMPERATURE));
            write_String_To_File_Value(fileHandle, (int16_t)getDataFromMemoryCallable(ADDRESS_WATER_OUTLET_TEMPERATURE));
            write_String_To_File_Value(fileHandle, (int16_t)getDataFromMemoryCallable(ADDRESS_EXTERNAL_ENVIRONMENT_TEMPERATURE));
            write_String_To_File_Value(fileHandle, (int16_t)getDataFromMemoryCallable(ADDRESS_WATER_TANK_TEMPERATURE_2));
            write_String_To_File_Value(fileHandle, (int16_t)getDataFromMemoryCallable(ADDRESS_SET_THE_UPPER_LIMIT_OF_FLOOR_HEATING_HEATING_HEATING_TEMPERATURE));
            write_String_To_File_Value(fileHandle, (int16_t)getDataFromMemoryCallable(ADDRESS_SET_THE_LOWER_LIMIT_OF_FLOOR_HEATING_HEATING_HEATING_TEMPERATURE));
            write_String_To_File_Value(fileHandle, (int16_t)getDataFromMemoryCallable(ADDRESS_SET_THE_UPPER_LIMIT_OF_HOT_WATER_TEMPERATURE));

            write_String_To_File_Value(fileHandle, (int16_t)getDataFromMemoryCallable(ADDRESS_SET_THE_LOWER_LIMIT_OF_HOT_WATER_TEMPERATURE));
            write_String_To_File_Value(fileHandle, (int16_t)getDataFromMemoryCallable(ADDRESS_SET_THE_UPPER_LIMIT_OF_REFRIGERATING_TEMPERATURE));
            write_String_To_File_Value(fileHandle, (int16_t)getDataFromMemoryCallable(ADDRESS_SET_THE_LOWER_LIMIT_OF_REFRIGERATING_TEMPERATURE));     
              
            write_String_To_File(fileHandle, "\n");            
            break;
        }

        case BATTERY:{
            break;
        }

        case SMART_METER:{
            break;
        }

        case INVERTER:{
            break;
        }

        case HEATPUMP_BOILER:{
            break;
        }        
        
        
        default: {
            break;
        }
    }
    return;
}
