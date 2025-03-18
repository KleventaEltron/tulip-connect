#ifndef _CIRCULATION_PUMP_H    /* Guard against multiple inclusion */
#define _CIRCULATION_PUMP_H

typedef enum
{
    /* Application's state machine's initial state. */
    CIRCULATION_PUMP_STATE_INIT=0,
    CIRCULATION_PUMP_STATE_OFF, 
    CIRCULATION_PUMP_STATE_ON,
    CIRCULATION_PUMP_STATE_LAG_TIME,
    CIRCULATION_PUMP_STATE_TOO_LONG_IDLE,       
    CIRCULATION_PUMP_STATE_OFF_TOO_LOW_TEMP,  

} CIRCULATION_PUMP_STATES;

void CirculationPumpInit(void);
char * GetStringWithCirculationPumpState(CIRCULATION_PUMP_STATES state);
CIRCULATION_PUMP_STATES GetCirculationPumpState(void);
void CirculationPumpControl(APP_HEATING_AND_HOT_WATER_STATES appState, bool thermostatContact, bool heatingElement, uint32_t* secondCounter, int16_t heatingSetpoint, int16_t bufferTemp, bool defrostActive);

#endif 