#ifndef _HEATING_AND_HOT_WATER_TOOLS_H    /* Guard against multiple inclusion */
#define _HEATING_AND_HOT_WATER_TOOLS_H

int16_t DetermineCorrectSetpoint(APP_HEATING_AND_HOT_WATER_STATES currentState, int16_t setpointHotWater, int16_t setpointHeating, int16_t setpointHotWaterOffset, int16_t setpointSterilization, int16_t setpointSterilizationOffset);
bool CheckIfCompressorIsRunning(uint16_t compressorFrequency);
bool CheckIfTemperatureIsBelowSetpointMinusDelta(int16_t setpoint, int16_t currentTemperature, int16_t delta);
bool CheckIfSetpointReached(int16_t setpoint, int16_t currentTemperature);
bool CheckIfSetpointInHeatpumpIsUpToDate(int16_t currentSetpointInDisplay, uint16_t currentSetpointInHeatpump);
bool TemperatureRisedEnough(uint32_t* secondCounter, int16_t currentTemp, int16_t* initialTemp, uint16_t tempThreshold, uint16_t intervalSeconds);
APP_HEATING_AND_HOT_WATER_STATES DetermineWhatHasToBeDone(void);
bool CheckIfRetourTemperatureReachedSetpoint(int16_t retourTemperature, int16_t setpoint);
bool CheckIfDefrostModeActive(void);
bool IsSoftwareResetPossible(APP_HEATING_AND_HOT_WATER_STATES appState, uint32_t secondCounterLegionella);
bool IsSterilizationRequired(uint16_t sterilizationFunction, 
        uint16_t sterilizationIntervalDays, 
        uint16_t sterilizationStartTime, 
        int16_t currentHotWaterTemperature,
        uint8_t currentDisplayTimeHours,
        uint8_t currentDisplayTimeMinutes,
        uint16_t dayCounter,
        uint32_t secondCounterLegionella,
        uint16_t maxTimeOutOfSterilizationMode,
        bool heatingElementStatus);
bool IsSterilizationRunningInBackground(uint32_t secondCounterLegionella, bool heatingElementStatus , APP_HEATING_AND_HOT_WATER_STATES appState);
char * BooleanToOnOffString(bool boolean);

#endif 