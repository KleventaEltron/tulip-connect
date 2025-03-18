
#include "logging.h"

#ifndef _SD_CARD_H    /* Guard against multiple inclusion */
#define _SD_CARD_H


/* SD CARD MOUNTING NAMES */
#define SDCARD_MOUNT_NAME           SYS_FS_MEDIA_IDX0_MOUNT_NAME_VOLUME_IDX0
#define SDCARD_DEV_NAME             SYS_FS_MEDIA_IDX0_DEVICE_NAME_VOLUME_IDX0

#define SDCARD_LOGGING_DIRECTORY    "/LOGGING"
#define TWO_WEEK_LOGFILE_NAME       "/TwoWeekLogFile.txt"

#define TEMP_FILE_NAME              "/TEMPFILE.txt"
#define TULIP_LOG_FILE              "/TULIP.txt"
#define HEATPUMP_LOG_FILE           "/HEATPUMP.txt"

#define MAX_FILE_LINE_COUNT         4032

extern bool SD_CARD_MOUNT_FLAG;
extern SYS_FS_HANDLE fileHandle;

void write_String_To_File(SYS_FS_HANDLE fileHandle, char buffer[]);
//void write_String_To_File_Value(SYS_FS_HANDLE fileHandle, int value);
void SYSTEM_SDCARD_MOUNT_HANDLER(SYS_FS_EVENT event,void* eventData,uintptr_t context);
bool checkIfDirectoryExists(char directoryName[]); 
int getFileLineCount(char fileName[]);    
bool openFileJson(char fileName[]);
bool openFileCSV(char fileName[], DEVICE_TYPE dvt);
void fifoFileWriter(char fileName[], int lineCount);
void writeCsvLogToFile(DEVICE_TYPE dvt);

#endif
