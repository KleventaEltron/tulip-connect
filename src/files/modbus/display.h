#ifndef _DISPLAY_H    /* Guard against multiple inclusion */
#define _DISPLAY_H

#include "../../app_display_comm.h"

void ParseDisplayData(uint8_t * rxBuffer);
uint8_t FillTransmitBuffer(uint8_t* txBuffer, uint8_t* rxBuffer);
void GetDataFromHeatpump(void);
uint16_t getDataFromMemoryCallable(uint16_t address);
void CheckIfSettingHasChanged(void);
char * getDisplayStateToString(APP_DISPLAY_COMM_STATES displayState);

#endif 