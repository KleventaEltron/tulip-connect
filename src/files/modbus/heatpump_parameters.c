/* TODO:  Include other files here if needed. */
#include <xc.h>
#include <string.h>
#include <stdbool.h>                    // Defines true
#include "definitions.h"
#include "heatpump_parameters.h"
#include "modbus.h"

// Known parameters
uint16_t RealTimeDataStatussen  [REGISTERS_AMOUNT_REAL_TIME_DATA_STATUSSEN] [PARAMETERS_ARRAY_LENGTH]; 
uint16_t RealTimeData           [REGISTERS_AMOUNT_REAL_TIME_DATA]           [PARAMETERS_ARRAY_LENGTH]; 
uint16_t UnitSystemParameters   [REGISTERS_AMOUNT_UNIT_SYSTEM_PARAMETERS]   [PARAMETERS_ARRAY_LENGTH]; 
uint16_t UserParameters         [REGISTERS_AMOUNT_USER_PARAMETERS]          [PARAMETERS_ARRAY_LENGTH]; 
uint16_t UserOrder              [REGISTERS_AMOUNT_USER_ORDER]               [PARAMETERS_ARRAY_LENGTH]; 
uint16_t VersionInformation     [REGISTERS_AMOUNT_VERSION_INFORMATION]      [PARAMETERS_ARRAY_LENGTH]; 
uint16_t UnitSystemParameterL   [REGISTERS_AMOUNT_UNIT_SYSTEM_PARAMETER_L]  [PARAMETERS_ARRAY_LENGTH]; 
uint16_t CoilAddresses          [REGISTERS_AMOUNT_COIL_ADDRESSES]           [PARAMETERS_ARRAY_LENGTH]; 

// Unknown parameters
uint16_t UnknownParameters1     [REGISTERS_AMOUNT_UNKNOWN_PARAMTERS_1] [PARAMETERS_ARRAY_LENGTH]; 
uint16_t UnknownParameters2     [REGISTERS_AMOUNT_UNKNOWN_PARAMTERS_2] [PARAMETERS_ARRAY_LENGTH]; 
uint16_t UnknownParameters3     [REGISTERS_AMOUNT_UNKNOWN_PARAMTERS_3] [PARAMETERS_ARRAY_LENGTH]; 
uint16_t UnknownParameters4     [REGISTERS_AMOUNT_UNKNOWN_PARAMTERS_4] [PARAMETERS_ARRAY_LENGTH]; 
uint16_t UnknownParameters5     [REGISTERS_AMOUNT_UNKNOWN_PARAMTERS_5] [PARAMETERS_ARRAY_LENGTH]; 
uint16_t UnknownParameters6     [REGISTERS_AMOUNT_UNKNOWN_PARAMTERS_6] [PARAMETERS_ARRAY_LENGTH]; 
uint16_t UnknownParameters7     [REGISTERS_AMOUNT_UNKNOWN_PARAMTERS_7] [PARAMETERS_ARRAY_LENGTH]; 
uint16_t UnknownParameters8     [REGISTERS_AMOUNT_UNKNOWN_PARAMTERS_8] [PARAMETERS_ARRAY_LENGTH]; 
uint16_t UnknownParameters9     [REGISTERS_AMOUNT_UNKNOWN_PARAMTERS_9] [PARAMETERS_ARRAY_LENGTH]; 


//MANUAL_SETTING Setting = {SETTING_SEND_STATUS_IDLE, 0, 0, 0, 0, 0};

MANUAL_SETTING settings[MAX_SETTINGS];
uint8_t currentSizeSettingsArray = 0;

bool addSetting(MANUAL_SETTING newSetting) 
{
    if (currentSizeSettingsArray < MAX_SETTINGS) 
    {
        settings[currentSizeSettingsArray++] = newSetting;
        return true;
    } 
    else 
    {
        //printf("Error: The list is full.\n");
        return false;
    }
}

bool removeSetting(void) 
{   
    if (currentSizeSettingsArray == 0) 
    {   // Is size is 0, cant remove anything
        //printf("Error: The list is empty.\n");
        return false;
    }

    for (int i = 0; i < (currentSizeSettingsArray - 1); i++) 
    {   // Shift all elements one step forward
        settings[i] = settings[i + 1];
    }
    
    settings[currentSizeSettingsArray - 1] = (MANUAL_SETTING){SETTING_SEND_STATUS_EMPTY, 0, 0, 0, 0};
    
    currentSizeSettingsArray--; // Decrease the list size
    return true;
}

void ChangeFirstSettingStatus(SETTING_SEND_STATUS newStatus) 
{
    if (currentSizeSettingsArray == 0) 
    {
        //printf("Error: The list is empty. Cannot change status.\n");
        return;
    }

    settings[0].settingStatus = newStatus;
}

MANUAL_SETTING PopFirstSetting(void) 
{   // Function to return the first setting without removing it
    if (currentSizeSettingsArray == 0) {
        //printf("Error: The list is empty. Returning a default setting.\n");
        return (MANUAL_SETTING){SETTING_SEND_STATUS_EMPTY, 0, 0, 0, 0}; // Default empty setting
    }

    return settings[0]; // Return the first setting without removing it
}

void ConfirmSettingIsEchoed(uint16_t reg, uint16_t data)
{
    MANUAL_SETTING setting = (MANUAL_SETTING)PopFirstSetting();
    
    if (setting.settingStatus == SETTING_SEND_STATUS_SETTING_IS_SENT)
    {
        if (reg == setting.modbusWriteRegister && data == setting.modbusWriteData)
        {
            //setting.settingStatus = SETTING_SEND_STATUS_SETTING_IS_ECHOED;
            //setting.settingStatus = SETTING_SEND_STATUS_IDLE;
            
            removeSetting();
            
            //SetDataInArrays(reg, data);
        }
    }
}

/*
bool alreadyBusyWithSendingSetting(void)
{
    if (Setting.settingStatus == SETTING_SEND_STATUS_IDLE)
    {   // If setting status is idle, new setting can be send
        return false;
    }
    else
    {   // already busy with sending a setting
        return true;
    }
}
*/

void ChangeHeatpumpSetting(uint16_t reg, uint16_t data)
{
    MANUAL_SETTING newSetting = {SETTING_SEND_STATUS_SETTING_FILLED, THIS_DEVICE_ADDRESS, MB_FC_WRITE_REG, reg, data};
    
    addSetting(newSetting); 
            
    /*
    if (alreadyBusyWithSendingSetting() == false)
    {   // Not busy with sending a setting, so load new setting
        Setting.settingStatus = SETTING_SEND_STATUS_SETTING_FILLED;
        Setting.modbusDeviceAddress = THIS_DEVICE_ADDRESS;
        Setting.modbusCommand = MB_FC_WRITE_REG;
        Setting.modbusWriteRegister = reg;
        Setting.modbusWriteData = data;
    }
    */  
}


void SetDataInArrays(uint16_t address, uint16_t data)
{
    // Known parameters
    if ((address >= START_ADDRESS_REAL_TIME_DATA_STATUSSEN) && (address < START_ADDRESS_REAL_TIME_DATA_STATUSSEN + REGISTERS_AMOUNT_REAL_TIME_DATA_STATUSSEN))
    {
        address -= START_ADDRESS_REAL_TIME_DATA_STATUSSEN;
        RealTimeDataStatussen[address][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP] = data;
        RealTimeDataStatussen[address][PARAMETER_ARRAY_DATA_SEND_TO_DISPLAY] = data;
    }
    else if ((address >= START_ADDRESS_REAL_TIME_DATA) && (address < START_ADDRESS_REAL_TIME_DATA + REGISTERS_AMOUNT_REAL_TIME_DATA))
    {
        address -= START_ADDRESS_REAL_TIME_DATA;
        RealTimeData[address][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP] = data;
        RealTimeData[address][PARAMETER_ARRAY_DATA_SEND_TO_DISPLAY] = data;
    }
    else if ((address >= START_ADDRESS_UNIT_SYSTEM_PARAMETERS) && (address < START_ADDRESS_UNIT_SYSTEM_PARAMETERS + REGISTERS_AMOUNT_UNIT_SYSTEM_PARAMETERS))
    {
        address -= START_ADDRESS_UNIT_SYSTEM_PARAMETERS;
        UnitSystemParameters[address][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP] = data;
        UnitSystemParameters[address][PARAMETER_ARRAY_DATA_SEND_TO_DISPLAY] = data;
    }
    else if ((address >= START_ADDRESS_USER_PARAMETERS) && (address < START_ADDRESS_USER_PARAMETERS + REGISTERS_AMOUNT_USER_PARAMETERS))
    {
        address -= START_ADDRESS_USER_PARAMETERS;
        UserParameters[address][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP] = data;
        UserParameters[address][PARAMETER_ARRAY_DATA_SEND_TO_DISPLAY] = data;
    }
    else if ((address >= START_ADDRESS_USER_ORDER) && (address < START_ADDRESS_USER_ORDER + REGISTERS_AMOUNT_USER_ORDER))
    {
        address -= START_ADDRESS_USER_ORDER;
        UserOrder[address][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP] = data;
        UserOrder[address][PARAMETER_ARRAY_DATA_SEND_TO_DISPLAY] = data;
    }
    else if ((address >= START_ADDRESS_VERSION_INFORMATION) && (address < START_ADDRESS_VERSION_INFORMATION + REGISTERS_AMOUNT_VERSION_INFORMATION))
    {
        address -= START_ADDRESS_VERSION_INFORMATION;
        VersionInformation[address][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP] = data;
        VersionInformation[address][PARAMETER_ARRAY_DATA_SEND_TO_DISPLAY] = data;
    }
    else if ((address >= START_ADDRESS_UNIT_SYSTEM_PARAMETER_L) && (address < START_ADDRESS_UNIT_SYSTEM_PARAMETER_L + REGISTERS_AMOUNT_UNIT_SYSTEM_PARAMETER_L))
    {
        address -= START_ADDRESS_UNIT_SYSTEM_PARAMETER_L;
        UnitSystemParameterL[address][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP] = data;
        UnitSystemParameterL[address][PARAMETER_ARRAY_DATA_SEND_TO_DISPLAY] = data;
    }
    else if ((address >= START_ADDRESS_COIL_ADDRESSES) && (address < START_ADDRESS_COIL_ADDRESSES + REGISTERS_AMOUNT_COIL_ADDRESSES))
    {
        address -= START_ADDRESS_COIL_ADDRESSES;
        CoilAddresses[address][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP] = data;
        CoilAddresses[address][PARAMETER_ARRAY_DATA_SEND_TO_DISPLAY] = data;
    }
}

void SetDataInArraysAtStartup(void)
{
    uint16_t i;
    
    //for (i = START_ADDRESS_REAL_TIME_DATA_STATUSSEN; i < (START_ADDRESS_REAL_TIME_DATA_STATUSSEN + REGISTERS_AMOUNT_REAL_TIME_DATA_STATUSSEN); i++)
    //    SetDataInArrays(i, UINT16_MAX);
    
    for (i = START_ADDRESS_REAL_TIME_DATA; i < (START_ADDRESS_REAL_TIME_DATA + REGISTERS_AMOUNT_REAL_TIME_DATA); i++)
        SetDataInArrays(i, UINT16_MAX);
    
    for (i = START_ADDRESS_UNIT_SYSTEM_PARAMETERS; i < (START_ADDRESS_UNIT_SYSTEM_PARAMETERS + REGISTERS_AMOUNT_UNIT_SYSTEM_PARAMETERS); i++)
        SetDataInArrays(i, UINT16_MAX);
    
    for (i = START_ADDRESS_USER_PARAMETERS; i < (START_ADDRESS_USER_PARAMETERS + REGISTERS_AMOUNT_USER_PARAMETERS); i++)
        SetDataInArrays(i, UINT16_MAX);
    
    for (i = START_ADDRESS_USER_ORDER; i < (START_ADDRESS_USER_ORDER + REGISTERS_AMOUNT_USER_ORDER); i++)
        SetDataInArrays(i, UINT16_MAX);
    
    for (i = START_ADDRESS_VERSION_INFORMATION; i < (START_ADDRESS_VERSION_INFORMATION + REGISTERS_AMOUNT_VERSION_INFORMATION); i++)
        SetDataInArrays(i, UINT16_MAX);
    
    for (i = START_ADDRESS_UNIT_SYSTEM_PARAMETER_L; i < (START_ADDRESS_UNIT_SYSTEM_PARAMETER_L + REGISTERS_AMOUNT_UNIT_SYSTEM_PARAMETER_L); i++)
        SetDataInArrays(i, UINT16_MAX);
    
    for (i = START_ADDRESS_COIL_ADDRESSES; i < (START_ADDRESS_COIL_ADDRESSES + REGISTERS_AMOUNT_COIL_ADDRESSES); i++)
        SetDataInArrays(i, UINT16_MAX);
    
    
    SetDataInArrays(ADDRESS_HOT_WATER_RETURN_DIFFERENCE, TEMPERATURE_ALARM_VALUE);
    SetDataInArrays(ADDRESS_AIR_CONDITIONER_RETURN_DIFFERENCE, TEMPERATURE_ALARM_VALUE);
}

/*
void SetDataInDisplayArray(uint16_t address, uint16_t data)
{
    // Known parameters
    if ((address >= START_ADDRESS_REAL_TIME_DATA_STATUSSEN) && (address < START_ADDRESS_REAL_TIME_DATA_STATUSSEN + REGISTERS_AMOUNT_REAL_TIME_DATA_STATUSSEN))
    {
        address -= START_ADDRESS_REAL_TIME_DATA_STATUSSEN;
        RealTimeDataStatussen[address][PARAMETER_ARRAY_DATA_SEND_TO_DISPLAY] = data;
    }
    else if ((address >= START_ADDRESS_REAL_TIME_DATA) && (address < START_ADDRESS_REAL_TIME_DATA + REGISTERS_AMOUNT_REAL_TIME_DATA))
    {
        address -= START_ADDRESS_REAL_TIME_DATA;
        RealTimeData[address][PARAMETER_ARRAY_DATA_SEND_TO_DISPLAY] = data;
    }
    else if ((address >= START_ADDRESS_UNIT_SYSTEM_PARAMETERS) && (address < START_ADDRESS_UNIT_SYSTEM_PARAMETERS + REGISTERS_AMOUNT_UNIT_SYSTEM_PARAMETERS))
    {
        address -= START_ADDRESS_UNIT_SYSTEM_PARAMETERS;
        UnitSystemParameters[address][PARAMETER_ARRAY_DATA_SEND_TO_DISPLAY] = data;
    }
    else if ((address >= START_ADDRESS_USER_PARAMETERS) && (address < START_ADDRESS_USER_PARAMETERS + REGISTERS_AMOUNT_USER_PARAMETERS))
    {
        address -= START_ADDRESS_USER_PARAMETERS;
        UserParameters[address][PARAMETER_ARRAY_DATA_SEND_TO_DISPLAY] = data;
    }
    else if ((address >= START_ADDRESS_USER_ORDER) && (address < START_ADDRESS_USER_ORDER + REGISTERS_AMOUNT_USER_ORDER))
    {
        address -= START_ADDRESS_USER_ORDER;
        UserOrder[address][PARAMETER_ARRAY_DATA_SEND_TO_DISPLAY] = data;
    }
    else if ((address >= START_ADDRESS_VERSION_INFORMATION) && (address < START_ADDRESS_VERSION_INFORMATION + REGISTERS_AMOUNT_VERSION_INFORMATION))
    {
        address -= START_ADDRESS_VERSION_INFORMATION;
        VersionInformation[address][PARAMETER_ARRAY_DATA_SEND_TO_DISPLAY] = data;
    }
    else if ((address >= START_ADDRESS_UNIT_SYSTEM_PARAMETER_L) && (address < START_ADDRESS_UNIT_SYSTEM_PARAMETER_L + REGISTERS_AMOUNT_UNIT_SYSTEM_PARAMETER_L))
    {
        address -= START_ADDRESS_UNIT_SYSTEM_PARAMETER_L;
        UnitSystemParameterL[address][PARAMETER_ARRAY_DATA_SEND_TO_DISPLAY] = data;
    }
    else if ((address >= START_ADDRESS_COIL_ADDRESSES) && (address < START_ADDRESS_COIL_ADDRESSES + REGISTERS_AMOUNT_COIL_ADDRESSES))
    {
        address -= START_ADDRESS_COIL_ADDRESSES;
        CoilAddresses[address][PARAMETER_ARRAY_DATA_SEND_TO_DISPLAY] = data;
    }
}
 */