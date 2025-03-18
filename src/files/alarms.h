#ifndef _ALARMS_H    /* Guard against multiple inclusion */
#define _ALARMS_H

#define CLEAR_ALARM false
#define SET_ALARM   true

#define ALARM_ARRAY_SIZE 1

void ClearAlarmArray(void);
void SetOrClearAlarm(uint32_t alarm, bool SetOrReset);
bool GetAlarmStatus(uint32_t alarm);
uint32_t GetAlarm(uint32_t index);
void SetOrClearAlarmLed(void);


typedef enum
{
    ALARM_HEATPUMP_COMMUNICATION=0, // 0
    ALARM_DISPLAY_COMMUNICATION,    // 1
            
} ALARMS;


#endif 