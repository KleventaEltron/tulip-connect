#ifndef _TIMER_COUNTERS_H    /* Guard against multiple inclusion */
#define _TIMER_COUNTERS_H

#define TENTH_SECOND_COUNTER_1_SECOND           10
#define TENTH_SECOND_COUNTER_5_SECONDS          50
#define TENTH_SECOND_COUNTER_100_MILLISECOND    1
//#define TENTH_SECOND_COUNTER_1_DAY              864000 // 1 dag
#define TENTH_SECOND_COUNTER_1_MINUTE              600 // 1 minuten
#define TENTH_SEC0ND_COUNTER_5_MINUTES          3000
#define TENTH_SECOND_COUNTER_20_SECONDS         200     // 1 minuten
#define TENTH_SECOND_COUNTER_30_SECONDS         300
//#define TENTH_SECOND_COUNTER_1_DAY  36000             // 1 uur

void InitTimerCounters ( void );

void UpdateCounters ( void );

bool LedsTimerExpired ( void );
bool HeatingHotWaterTimerExpired ( void );
bool DisplayCommunicationTimerExpired ( void );
bool HeatpumpCommunicationTimerExpired ( void );
bool LoggingTimerExpired ( void );
bool LoggingTimerSDCardExpired ( void );
//bool FtpTimerExpired ( void );
//bool ResetSoftwareTimerExpired ( void );

#endif 