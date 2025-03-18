/* TODO:  Include other files here if needed. */
#include <xc.h>
#include <string.h>
#include <stdbool.h>                    // Defines true
#include "definitions.h"
#include "display.h"

#include "modbus.h"
#include "heatpump.h"
#include "heatpump_parameters.h"
#include "files\ntc.h"
#include "files\eeprom.h"
//#include "shiftregisters.h

void ParseDisplayData(uint8_t * rxBuffer)
{
    uint16_t regAddress;
    uint16_t data;
    //uint16_t checksum;
    
    if (rxBuffer[MODBUS_COMMAND_INDEX] == MB_FC_WRITE_REG)
    {
        regAddress = ((uint16_t)rxBuffer[MODBUS_REG_ADDRESS_MSB_INDEX] << 8) + rxBuffer[MODBUS_REG_ADDRESS_LSB_INDEX];
        data       = ((uint16_t)rxBuffer[MODBUS_MOD_DATA_MSB_INDEX] << 8) + rxBuffer[MODBUS_MOD_DATA_LSB_INDEX];
        
        /*
        if (Setting.settingStatus == SETTING_SEND_STATUS_IDLE)
        {
            Setting.modbusDeviceAddress = rxBuffer[MODBUS_ADDRESS_INDEX];
            Setting.modbusCommand = rxBuffer[MODBUS_COMMAND_INDEX];
            Setting.modbusWriteRegister = regAddress;
            Setting.modbusWriteData = data;
            Setting.settingStatus = SETTING_SEND_STATUS_SETTING_FILLED;
        }
        */
        
        switch (regAddress)
        {
            case ADDRESS_SET_MODE:
                WriteSmartEeprom16(SEEP_ADDR_HEATPUMP_MODE, data); // Save the mode to smart eeprom
                SetDataInArrays(regAddress, data);
                
                if (data == SET_MODE_HOT_WATER_AND_HEATING)
                {   // If the display tells the heatpump to go to Hot water and Heating, this must not be possible, because we control the hot water with the Connect. So tell to go to heating only
                    ChangeHeatpumpSetting(regAddress, SET_MODE_HEATING); 
                }
                else
                {   // All other modes can be sent to the heatpump
                    ChangeHeatpumpSetting(regAddress, data);
                }

                break;
            case ADDRESS_HEATING_SET_TEMPERATURE:
                //if (UserParameters[ADDRESS_HEATING_FLOOR_HEATING_CURVE_SETTING - START_ADDRESS_USER_PARAMETERS][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP] == HEATING_CURVE_SETTING_OFF)
                    WriteSmartEeprom16(SEEP_ADDR_HEATING_SETPOINT, data);
                //ChangeHeatpumpSetting(regAddress, data); 
                break;
            case ADDRESS_HOT_WATER_SET_TEMPERATURE:
                //SetpointHotWater = data;
                WriteSmartEeprom16(SEEP_ADDR_HOT_WATER_SETPOINT, data);
                //ChangeHeatpumpSetting(regAddress, data); 
                break;
            default:
                //SetDataInDisplayArray(regAddress, data);
                ChangeHeatpumpSetting(regAddress, data);
                SetDataInArrays(regAddress, data);
                break;
        }    
        
        //checksum = calculateCRC16(rxBuffer, 6);
        //rxBuffer[MODBUS_CHECKSUM_LSB_INDEX] = (uint8_t)(checksum >> 0);
        //rxBuffer[MODBUS_CHECKSUM_MSB_INDEX] = (uint8_t)(checksum >> 8);
    }
}

char * getDisplayStateToString(APP_DISPLAY_COMM_STATES displayState) {
    switch(displayState){
        case(APP_DISPLAY_COMM_STATE_INIT): return "0, Init"; break;        
        case(APP_DISPLAY_COMM_STATE_RECEIVE_DATA): return "1, Receive Data"; break;   
        case(APP_DISPLAY_COMM_STATE_WAIT_FOR_DATA_RECEIVED): return "2, Wait Data"; break;    
        case(APP_DISPLAY_COMM_STATE_CHECKSUM_CHECK): return "3, Checksum"; break;    
        case(APP_DISPLAY_COMM_STATE_PARSE_DATA): return "4, Parse Data"; break;    
        case(APP_DISPLAY_COMM_STATE_DELAY): return "5, Delay"; break;      
        case(APP_DISPLAY_COMM_STATE_PREPARING_DATA_TO_SENT):  return "6, Prepare Send"; break;    
        case(APP_DISPLAY_COMM_STATE_SEND_DATA): return "7, Send Data"; break;             
        case(APP_DISPLAY_COMM_STATE_WAIT_FOR_DATA_SENT): return "8, Wait Send"; break;          
        default: return "9, Unkown"; break;
    }
}



static uint16_t getDataFromMemory(uint16_t address)
{
    uint16_t returnData;
    
    // Known parameters
    if ((address >= START_ADDRESS_REAL_TIME_DATA_STATUSSEN) && (address < START_ADDRESS_REAL_TIME_DATA_STATUSSEN + REGISTERS_AMOUNT_REAL_TIME_DATA_STATUSSEN))
    {
        address -= START_ADDRESS_REAL_TIME_DATA_STATUSSEN;
        //RealTimeDataStatussen[address][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP];
        returnData = RealTimeDataStatussen[address][PARAMETER_ARRAY_DATA_SEND_TO_DISPLAY];
    }
    else if ((address >= START_ADDRESS_REAL_TIME_DATA) && (address < START_ADDRESS_REAL_TIME_DATA + REGISTERS_AMOUNT_REAL_TIME_DATA))
    {
        address -= START_ADDRESS_REAL_TIME_DATA;
        returnData = RealTimeData[address][PARAMETER_ARRAY_DATA_SEND_TO_DISPLAY];
    }
    else if ((address >= START_ADDRESS_UNIT_SYSTEM_PARAMETERS) && (address < START_ADDRESS_UNIT_SYSTEM_PARAMETERS + REGISTERS_AMOUNT_UNIT_SYSTEM_PARAMETERS))
    {
        address -= START_ADDRESS_UNIT_SYSTEM_PARAMETERS;
        returnData = UnitSystemParameters[address][PARAMETER_ARRAY_DATA_SEND_TO_DISPLAY];
    }
    else if ((address >= START_ADDRESS_USER_PARAMETERS) && (address < START_ADDRESS_USER_PARAMETERS + REGISTERS_AMOUNT_USER_PARAMETERS))
    {
        address -= START_ADDRESS_USER_PARAMETERS;
        returnData = UserParameters[address][PARAMETER_ARRAY_DATA_SEND_TO_DISPLAY];
    }
    else if ((address >= START_ADDRESS_USER_ORDER) && (address < START_ADDRESS_USER_ORDER + REGISTERS_AMOUNT_USER_ORDER))
    {
        address -= START_ADDRESS_USER_ORDER;
        returnData = UserOrder[address][PARAMETER_ARRAY_DATA_SEND_TO_DISPLAY];
    }
    else if ((address >= START_ADDRESS_VERSION_INFORMATION) && (address < START_ADDRESS_VERSION_INFORMATION + REGISTERS_AMOUNT_VERSION_INFORMATION))
    {
        address -= START_ADDRESS_VERSION_INFORMATION;
        returnData = VersionInformation[address][PARAMETER_ARRAY_DATA_SEND_TO_DISPLAY];
    }
    else if ((address >= START_ADDRESS_UNIT_SYSTEM_PARAMETER_L) && (address < START_ADDRESS_UNIT_SYSTEM_PARAMETER_L + REGISTERS_AMOUNT_UNIT_SYSTEM_PARAMETER_L))
    {
        address -= START_ADDRESS_UNIT_SYSTEM_PARAMETER_L;
        returnData = UnitSystemParameterL[address][PARAMETER_ARRAY_DATA_SEND_TO_DISPLAY];
    }
    else if ((address >= START_ADDRESS_COIL_ADDRESSES) && (address < START_ADDRESS_COIL_ADDRESSES + REGISTERS_AMOUNT_COIL_ADDRESSES))
    {
        address -= START_ADDRESS_COIL_ADDRESSES;
        returnData = CoilAddresses[address][PARAMETER_ARRAY_DATA_SEND_TO_DISPLAY];
    }
    
    // Unknown parameters
    else if ((address >= START_ADDRESS_UNKNOWN_PARAMTERS_1) && (address < START_ADDRESS_UNKNOWN_PARAMTERS_1 + REGISTERS_AMOUNT_UNKNOWN_PARAMTERS_1))
    {
        address -= START_ADDRESS_UNKNOWN_PARAMTERS_1;
        returnData = UnknownParameters1[address][PARAMETER_ARRAY_DATA_SEND_TO_DISPLAY];
    }
    else if ((address >= START_ADDRESS_UNKNOWN_PARAMTERS_2) && (address < START_ADDRESS_UNKNOWN_PARAMTERS_2 + REGISTERS_AMOUNT_UNKNOWN_PARAMTERS_2))
    {
        address -= START_ADDRESS_UNKNOWN_PARAMTERS_2;
        returnData = UnknownParameters2[address][PARAMETER_ARRAY_DATA_SEND_TO_DISPLAY];
    }
    else if ((address >= START_ADDRESS_UNKNOWN_PARAMTERS_3) && (address < START_ADDRESS_UNKNOWN_PARAMTERS_3 + REGISTERS_AMOUNT_UNKNOWN_PARAMTERS_3))
    {
        address -= START_ADDRESS_UNKNOWN_PARAMTERS_3;
        returnData = UnknownParameters3[address][PARAMETER_ARRAY_DATA_SEND_TO_DISPLAY];
    }
    else if ((address >= START_ADDRESS_UNKNOWN_PARAMTERS_4) && (address < START_ADDRESS_UNKNOWN_PARAMTERS_4 + REGISTERS_AMOUNT_UNKNOWN_PARAMTERS_4))
    {
        address -= START_ADDRESS_UNKNOWN_PARAMTERS_4;
        returnData = UnknownParameters4[address][PARAMETER_ARRAY_DATA_SEND_TO_DISPLAY];
    }
    else if ((address >= START_ADDRESS_UNKNOWN_PARAMTERS_5) && (address < START_ADDRESS_UNKNOWN_PARAMTERS_5 + REGISTERS_AMOUNT_UNKNOWN_PARAMTERS_5))
    {
        address -= START_ADDRESS_UNKNOWN_PARAMTERS_5;
        returnData = UnknownParameters5[address][PARAMETER_ARRAY_DATA_SEND_TO_DISPLAY];
    }
    else if ((address >= START_ADDRESS_UNKNOWN_PARAMTERS_6) && (address < START_ADDRESS_UNKNOWN_PARAMTERS_6 + REGISTERS_AMOUNT_UNKNOWN_PARAMTERS_6))
    {
        address -= START_ADDRESS_UNKNOWN_PARAMTERS_6;
        returnData = UnknownParameters6[address][PARAMETER_ARRAY_DATA_SEND_TO_DISPLAY];
    }
    else if ((address >= START_ADDRESS_UNKNOWN_PARAMTERS_7) && (address < START_ADDRESS_UNKNOWN_PARAMTERS_7 + REGISTERS_AMOUNT_UNKNOWN_PARAMTERS_7))
    {
        address -= START_ADDRESS_UNKNOWN_PARAMTERS_7;
        returnData = UnknownParameters7[address][PARAMETER_ARRAY_DATA_SEND_TO_DISPLAY];
    }
    else if ((address >= START_ADDRESS_UNKNOWN_PARAMTERS_8) && (address < START_ADDRESS_UNKNOWN_PARAMTERS_8 + REGISTERS_AMOUNT_UNKNOWN_PARAMTERS_8))
    {
        address -= START_ADDRESS_UNKNOWN_PARAMTERS_8;
        returnData = UnknownParameters8[address][PARAMETER_ARRAY_DATA_SEND_TO_DISPLAY];
    }
    else if ((address >= START_ADDRESS_UNKNOWN_PARAMTERS_9) && (address < START_ADDRESS_UNKNOWN_PARAMTERS_9 + REGISTERS_AMOUNT_UNKNOWN_PARAMTERS_9))
    {
        address -= START_ADDRESS_UNKNOWN_PARAMTERS_9;
        returnData = UnknownParameters9[address][PARAMETER_ARRAY_DATA_SEND_TO_DISPLAY];
    }
    else
    {
        returnData = UINT16_MAX; 
    }
    
    return returnData;
}


uint16_t getDataFromMemoryCallable(uint16_t address){
    return getDataFromMemory(address);
}


uint8_t FillTransmitBuffer(uint8_t* txBuffer, uint8_t* rxBuffer)
{
    uint8_t size = 0;
    //uint8_t i = 0;
    uint16_t checksum;
    
    if (rxBuffer[MODBUS_COMMAND_INDEX] == MB_FC_WRITE_REG)
    {
         
        /*
        if (Setting.settingStatus == SETTING_SEND_STATUS_IDLE)
        {
            Setting.modbusDeviceAddress = rxBuffer[MODBUS_ADDRESS_INDEX];
            Setting.modbusCommand = rxBuffer[MODBUS_COMMAND_INDEX];
            Setting.modbusWriteRegister = ((uint16_t)rxBuffer[MODBUS_REG_ADDRESS_MSB_INDEX] << 8) + rxBuffer[MODBUS_REG_ADDRESS_LSB_INDEX];
            Setting.modbusWriteData = ((uint16_t)rxBuffer[MODBUS_MOD_DATA_MSB_INDEX] << 8) + rxBuffer[MODBUS_MOD_DATA_LSB_INDEX];
            Setting.settingStatus = SETTING_SEND_STATUS_SETTING_FILLED;

            memcpy(&txBuffer[0], &rxBuffer[0], 8);
            size = 8;
        }
        */
        
        memcpy(&txBuffer[0], &rxBuffer[0], 8);
        size = 8;
    }
    else if (rxBuffer[MODBUS_COMMAND_INDEX] == MB_FC_READ_REGS)
    {
        uint16_t data;
        uint8_t j = 3;
        uint16_t amountOfRegisters = ((uint16_t)rxBuffer[MODBUS_REG_AMOUNT_MSB_INDEX] << 8) + rxBuffer[MODBUS_REG_AMOUNT_LSB_INDEX]; 
        uint16_t readRegisterAddress = ((uint16_t)rxBuffer[MODBUS_REG_ADDRESS_MSB_INDEX] << 8) + rxBuffer[MODBUS_REG_ADDRESS_LSB_INDEX];

        txBuffer[MODBUS_ADDRESS_INDEX]          = 0x01;
        txBuffer[MODBUS_COMMAND_INDEX]          = MB_FC_READ_REGS;
        txBuffer[MODBUS_BYTES_RETURNED_INDEX]   = (amountOfRegisters << 1);
        
        for (uint16_t i = readRegisterAddress; i < (readRegisterAddress + amountOfRegisters); i++)
        {
            data = getDataFromMemory(i);
            txBuffer[j++] = (uint8_t)(data >> 8);
            txBuffer[j++] = (uint8_t)(data >> 0);
        }               
        
        checksum = calculateCRC16(txBuffer, (txBuffer[MODBUS_BYTES_RETURNED_INDEX] + 3));
        txBuffer[txBuffer[MODBUS_BYTES_RETURNED_INDEX]+3] = (uint8_t)(checksum >> 0);
        txBuffer[txBuffer[MODBUS_BYTES_RETURNED_INDEX]+4] = (uint8_t)(checksum >> 8);
        
        size = (txBuffer[MODBUS_BYTES_RETURNED_INDEX] + 5);
    }
    else{}
    
    return (size);
}

void GetDataFromHeatpump(void)
{    
    // Known parameters
    for (uint16_t i = 0; i < REGISTERS_AMOUNT_REAL_TIME_DATA_STATUSSEN; i++)
        RealTimeDataStatussen[i][PARAMETER_ARRAY_DATA_SEND_TO_DISPLAY] = RealTimeDataStatussen[i][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP];
    
    for (uint16_t i = 0; i < REGISTERS_AMOUNT_REAL_TIME_DATA; i++)
        RealTimeData[i][PARAMETER_ARRAY_DATA_SEND_TO_DISPLAY] = RealTimeData[i][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP];
    
    for (uint16_t i = 0; i < REGISTERS_AMOUNT_UNIT_SYSTEM_PARAMETERS; i++)
        UnitSystemParameters[i][PARAMETER_ARRAY_DATA_SEND_TO_DISPLAY] = UnitSystemParameters[i][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP];
    
    for (uint16_t i = 0; i < REGISTERS_AMOUNT_USER_PARAMETERS; i++)
        UserParameters[i][PARAMETER_ARRAY_DATA_SEND_TO_DISPLAY] = UserParameters[i][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP];
    
    for (uint16_t i = 0; i < REGISTERS_AMOUNT_USER_ORDER; i++)
        UserOrder[i][PARAMETER_ARRAY_DATA_SEND_TO_DISPLAY] = UserOrder[i][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP];
    
    for (uint16_t i = 0; i < REGISTERS_AMOUNT_VERSION_INFORMATION; i++)
        VersionInformation[i][PARAMETER_ARRAY_DATA_SEND_TO_DISPLAY] = VersionInformation[i][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP];
    
    for (uint16_t i = 0; i < REGISTERS_AMOUNT_UNIT_SYSTEM_PARAMETER_L; i++)
        UnitSystemParameterL[i][PARAMETER_ARRAY_DATA_SEND_TO_DISPLAY] = UnitSystemParameterL[i][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP];
    
    for (uint16_t i = 0; i < REGISTERS_AMOUNT_COIL_ADDRESSES; i++)
        CoilAddresses[i][PARAMETER_ARRAY_DATA_SEND_TO_DISPLAY] = CoilAddresses[i][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP];
    
    // Unknown parameters
    for (uint16_t i = 0; i < REGISTERS_AMOUNT_UNKNOWN_PARAMTERS_1; i++)
        UnknownParameters1[i][PARAMETER_ARRAY_DATA_SEND_TO_DISPLAY] = UnknownParameters1[i][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP];
    
    for (uint16_t i = 0; i < REGISTERS_AMOUNT_UNKNOWN_PARAMTERS_2; i++)
        UnknownParameters2[i][PARAMETER_ARRAY_DATA_SEND_TO_DISPLAY] = UnknownParameters2[i][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP];
    
    for (uint16_t i = 0; i < REGISTERS_AMOUNT_UNKNOWN_PARAMTERS_3; i++)
        UnknownParameters3[i][PARAMETER_ARRAY_DATA_SEND_TO_DISPLAY] = UnknownParameters3[i][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP];
    
    for (uint16_t i = 0; i < REGISTERS_AMOUNT_UNKNOWN_PARAMTERS_4; i++)
        UnknownParameters4[i][PARAMETER_ARRAY_DATA_SEND_TO_DISPLAY] = UnknownParameters4[i][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP];
    
    for (uint16_t i = 0; i < REGISTERS_AMOUNT_UNKNOWN_PARAMTERS_5; i++)
        UnknownParameters5[i][PARAMETER_ARRAY_DATA_SEND_TO_DISPLAY] = UnknownParameters5[i][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP];
    
    for (uint16_t i = 0; i < REGISTERS_AMOUNT_UNKNOWN_PARAMTERS_6; i++)
        UnknownParameters6[i][PARAMETER_ARRAY_DATA_SEND_TO_DISPLAY] = UnknownParameters6[i][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP];
    
    for (uint16_t i = 0; i < REGISTERS_AMOUNT_UNKNOWN_PARAMTERS_7; i++)
        UnknownParameters7[i][PARAMETER_ARRAY_DATA_SEND_TO_DISPLAY] = UnknownParameters7[i][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP];
    
    for (uint16_t i = 0; i < REGISTERS_AMOUNT_UNKNOWN_PARAMTERS_8; i++)
        UnknownParameters8[i][PARAMETER_ARRAY_DATA_SEND_TO_DISPLAY] = UnknownParameters8[i][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP];
    
    for (uint16_t i = 0; i < REGISTERS_AMOUNT_UNKNOWN_PARAMTERS_9; i++)
        UnknownParameters9[i][PARAMETER_ARRAY_DATA_SEND_TO_DISPLAY] = UnknownParameters9[i][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP];
    
    // If smartEEprom mode is on Hot Water and Heating, and mode from heatpump is only heating, send to display Hot Water and Heating
    int16_t heatpumpMode = ReadSmartEeprom16(SEEP_ADDR_HEATPUMP_MODE);
    if ((UserParameters[ADDRESS_SET_MODE - START_ADDRESS_USER_PARAMETERS][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP] == SET_MODE_HEATING) && (heatpumpMode == SET_MODE_HOT_WATER_AND_HEATING))
        UserParameters[ADDRESS_SET_MODE - START_ADDRESS_USER_PARAMETERS][PARAMETER_ARRAY_DATA_SEND_TO_DISPLAY] = heatpumpMode; 
    
    int16_t heatingSetpoint = ReadSmartEeprom16(SEEP_ADDR_HEATING_SETPOINT);
    
    if (heatingSetpoint == TEMPERATURE_ALARM_VALUE && UserParameters[ADDRESS_HEATING_SET_TEMPERATURE - START_ADDRESS_USER_PARAMETERS][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP] != UINT16_MAX)
    {   // If heating setpoint in smartEEprom is -9999, and the heatings setpoint in the heatpumt array is not UINT16_MAX (first startup only), set setpoint from heatpump in eeprom
        WriteSmartEeprom16(SEEP_ADDR_HEATING_SETPOINT, UserParameters[ADDRESS_HEATING_SET_TEMPERATURE - START_ADDRESS_USER_PARAMETERS][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP]);
    }
    else
    {   // Other then at first startup, send the setpoint in the eeprom to the display.
        //if (UserParameters[ADDRESS_HEATING_FLOOR_HEATING_CURVE_SETTING - START_ADDRESS_USER_PARAMETERS][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP] == HEATING_CURVE_SETTING_OFF)
        UserParameters[ADDRESS_HEATING_SET_TEMPERATURE - START_ADDRESS_USER_PARAMETERS][PARAMETER_ARRAY_DATA_SEND_TO_DISPLAY] = heatingSetpoint; 
    }
    
    // If hot water setpoint in smartEEprom is -9999, wait for setpoint given from heatpump, and write it to smartEEprom
    int16_t hotWaterSetpoint = ReadSmartEeprom16(SEEP_ADDR_HOT_WATER_SETPOINT);

    if (hotWaterSetpoint == TEMPERATURE_ALARM_VALUE && UserParameters[ADDRESS_HOT_WATER_SET_TEMPERATURE - START_ADDRESS_USER_PARAMETERS][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP] != UINT16_MAX)
    {   // If hot water setpoint in smartEEprom is -9999, wait for setpoint given from heatpump, and write it to smartEEprom
        WriteSmartEeprom16(SEEP_ADDR_HOT_WATER_SETPOINT, UserParameters[ADDRESS_HOT_WATER_SET_TEMPERATURE - START_ADDRESS_USER_PARAMETERS][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP]);
    }
    else
    {
        UserParameters[ADDRESS_HOT_WATER_SET_TEMPERATURE - START_ADDRESS_USER_PARAMETERS][PARAMETER_ARRAY_DATA_SEND_TO_DISPLAY] = hotWaterSetpoint;
    }
    
    // For displaying the NTC's on the display:
    RealTimeData[ADDRESS_WATER_TANK_TEMPERATURE - START_ADDRESS_REAL_TIME_DATA][PARAMETER_ARRAY_DATA_SEND_TO_DISPLAY] = (GetNtcTemperature(NTC_HOT_WATER_BUFFER) / 10); 
    RealTimeData[ADDRESS_RETURN_WATER_TEMPERATURE_T6 - START_ADDRESS_REAL_TIME_DATA][PARAMETER_ARRAY_DATA_SEND_TO_DISPLAY] = (GetNtcTemperature(NTC_HEATING_BUFFER) / 10);
    
    //TEST
    //RealTimeData[ADDRESS_BUFFER_TANK_FOR_HEATING_TEMPERATURE_VALUE - START_ADDRESS_REAL_TIME_DATA][PARAMETER_ARRAY_DATA_SEND_TO_DISPLAY] = 25;
    //RealTimeDataStatussen[ADDRESS_SWITCH_PORT_STATE_1 - START_ADDRESS_REAL_TIME_DATA_STATUSSEN][PARAMETER_ARRAY_DATA_SEND_TO_DISPLAY] |= (1<<5);
    //RealTimeDataStatussen[ADDRESS_SWITCH_PORT_STATE_1 - START_ADDRESS_REAL_TIME_DATA_STATUSSEN][PARAMETER_ARRAY_DATA_SEND_TO_DISPLAY] &= ~(1<<4);
}
