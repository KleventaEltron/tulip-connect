/* TODO:  Include other files here if needed. */
#include <xc.h>
#include <string.h>
#include <stdbool.h>                    // Defines true
#include "heatpump.h"
#include "definitions.h"

#include "modbus.h"
#include "heatpump_parameters.h"
#include "files\eeprom.h"

static uint8_t CommunicationArray[17][8] = 
{
    {0x01, 0x03, 0x08, 0x00, 0x00, 0x10, 0x46, 0x66},
    {0x01, 0x03, 0x00, 0x00, 0x00, 0x5A, 0xC5, 0xF1},
    {0x01, 0x03, 0x00, 0x5A, 0x00, 0x22, 0xE5, 0xC0},
    {0x01, 0x03, 0x00, 0xE0, 0x00, 0x22, 0xC4, 0x25},
    {0x01, 0x03, 0x01, 0x00, 0x00, 0x64, 0x45, 0xDD},
    {0x01, 0x03, 0x01, 0x64, 0x00, 0x4D, 0xC5, 0xDC},
    {0x01, 0x03, 0x01, 0xB1, 0x00, 0x5F, 0x54, 0x29},
    {0x01, 0x03, 0x03, 0x00, 0x00, 0x64, 0x44, 0x65},
    // 01 03 08 00 00 10: 0x0800 tm 0x080F  (Unit System Parameter L)
    // 01 03 00 00 00 5A: 0x0000 tm 0x0059  (Real-time data)   
    // 01 03 00 5A 00 22: 0x005A tm 0x007B  (Real-time data) 
    // 01 03 00 E0 00 22: 0x00E0 tm 0x0101  (Real-time data) 
    // 01 03 01 00 00 64: 0x0100 tm 0x0163  (System parameters)
    // 01 03 01 64 00 4D: 0x0164 tm 0x01B0  (System parameters)
    // 01 03 01 B1 00 5F: 0x01B1 tm 0x020F  (System parameters)
    // 01 03 03 00 00 64: 0x0300 tm 0x0363  (User parameters)
    
    {0x01, 0x03, 0x03, 0x70, 0x00, 0x30, 0x44, 0x41},
    {0x01, 0x03, 0x04, 0x00, 0x00, 0x64, 0x45, 0x44},
    {0x01, 0x03, 0x06, 0x00, 0x00, 0x64, 0x44, 0xA9},
    {0x01, 0x03, 0x04, 0x64, 0x00, 0x4D, 0xC5, 0x10},
    {0x01, 0x03, 0x06, 0x64, 0x00, 0x4D, 0xC4, 0xA8},
    {0x01, 0x03, 0x04, 0xB1, 0x00, 0x5F, 0x54, 0xE5},
    {0x01, 0x03, 0x06, 0xB1, 0x00, 0x5F, 0x55, 0x5D},
    {0x01, 0x03, 0x08, 0x40, 0x00, 0x10, 0x47, 0xB2},
    {0x01, 0x03, 0x08, 0x80, 0x00, 0x10, 0x47, 0x8E}
    // Unknown settings:
    // 01 03 03 70 00 30: 0x0370 tm 0x039F  (Unknown settings 1)
    // 01 03 04 00 00 64: 0x0400 tm 0x0463  (Unknown settings 2)
    // 01 03 06 00 00 64: 0x0600 tm 0x0663  (Unknown settings 3)
    // 01 03 04 64 00 4D: 0x0464 tm 0x04B0  (Unknown settings 4)
    // 01 03 06 64 00 4D: 0x0664 tm 0x06B0  (Unknown settings 5)
    // 01 03 04 B1 00 5F: 0x04B1 tm 0x050F  (Unknown settings 6)
    // 01 03 06 B1 00 5F: 0x06B1 tm 0x070F  (Unknown settings 7)
    // 01 03 08 40 00 10: 0x0840 tm 0x084F  (Unknown settings 8)
    // 01 03 08 80 00 10: 0x0880 tm 0x088F  (Unknown settings 9)
};


//uint16_t CoilAddresses[ADDRESSES_COILS_ADDRESSES];

// TYPE PARAMETER:         | START ADDRESS: | END ADDRESS: | TOTAL AMOUNT: |  
// ------------------------------------------------------------------------------------------------------------------
// Real-time data          | 0x0000         | 0x00FF       | 256           | 
// System parameters       | 0x0100         | 0x02FF       | 512           |
// User parameters         | 0x0300         | 0x036F       | 112           |
// Unit System Parameter L | 0x0800         | 0x083F       | 64            |
// Coils address           | 0x1000         | 0x10FF       | 256           |
// ------------------------------------------------------------------------------------------------------------------
// TOTAL:                                                    1198


static void parseWriteReg(uint8_t * rxBuffer)
{
    uint16_t regAddress = ((uint16_t)rxBuffer[MODBUS_REG_ADDRESS_MSB_INDEX] << 8) + rxBuffer[MODBUS_REG_ADDRESS_LSB_INDEX];
    uint16_t data       = ((uint16_t)rxBuffer[MODBUS_MOD_DATA_MSB_INDEX] << 8) + rxBuffer[MODBUS_MOD_DATA_LSB_INDEX];
    
    ConfirmSettingIsEchoed(regAddress, data);
}

void saveDataToMemory(uint16_t address, uint16_t data)
{
    if ((address >= START_ADDRESS_REAL_TIME_DATA_STATUSSEN) && (address < START_ADDRESS_REAL_TIME_DATA_STATUSSEN + REGISTERS_AMOUNT_REAL_TIME_DATA_STATUSSEN))
    {
        address -= START_ADDRESS_REAL_TIME_DATA_STATUSSEN;
        RealTimeDataStatussen[address][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP] = data;
    }
    else if ((address >= START_ADDRESS_REAL_TIME_DATA) && (address < START_ADDRESS_REAL_TIME_DATA + REGISTERS_AMOUNT_REAL_TIME_DATA))
    {
        address -= START_ADDRESS_REAL_TIME_DATA;
        RealTimeData[address][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP] = data;
    }
    else if ((address >= START_ADDRESS_UNIT_SYSTEM_PARAMETERS) && (address < START_ADDRESS_UNIT_SYSTEM_PARAMETERS + REGISTERS_AMOUNT_UNIT_SYSTEM_PARAMETERS))
    {
        address -= START_ADDRESS_UNIT_SYSTEM_PARAMETERS;
        UnitSystemParameters[address][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP] = data;
    }
    else if ((address >= START_ADDRESS_USER_PARAMETERS) && (address < START_ADDRESS_USER_PARAMETERS + REGISTERS_AMOUNT_USER_PARAMETERS))
    {
        address -= START_ADDRESS_USER_PARAMETERS;
        UserParameters[address][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP] = data;
    }
    else if ((address >= START_ADDRESS_USER_ORDER) && (address < START_ADDRESS_USER_ORDER + REGISTERS_AMOUNT_USER_ORDER))
    {
        address -= START_ADDRESS_USER_ORDER;
        UserOrder[address][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP] = data;
    }
    else if ((address >= START_ADDRESS_VERSION_INFORMATION) && (address < START_ADDRESS_VERSION_INFORMATION + REGISTERS_AMOUNT_VERSION_INFORMATION))
    {
        address -= START_ADDRESS_VERSION_INFORMATION;
        VersionInformation[address][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP] = data;
    }
    else if ((address >= START_ADDRESS_UNIT_SYSTEM_PARAMETER_L) && (address < START_ADDRESS_UNIT_SYSTEM_PARAMETER_L + REGISTERS_AMOUNT_UNIT_SYSTEM_PARAMETER_L))
    {
        address -= START_ADDRESS_UNIT_SYSTEM_PARAMETER_L;
        UnitSystemParameterL[address][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP] = data;
    }
    else if ((address >= START_ADDRESS_COIL_ADDRESSES) && (address < START_ADDRESS_COIL_ADDRESSES + REGISTERS_AMOUNT_COIL_ADDRESSES))
    {
        address -= START_ADDRESS_COIL_ADDRESSES;
        CoilAddresses[address][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP] = data;
    }
    // Unknown arrays:
    else if ((address >= START_ADDRESS_UNKNOWN_PARAMTERS_1) && (address < START_ADDRESS_UNKNOWN_PARAMTERS_1 + REGISTERS_AMOUNT_UNKNOWN_PARAMTERS_1))
    {
        address -= START_ADDRESS_UNKNOWN_PARAMTERS_1;
        UnknownParameters1[address][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP] = data;
    }
    else if ((address >= START_ADDRESS_UNKNOWN_PARAMTERS_2) && (address < START_ADDRESS_UNKNOWN_PARAMTERS_2 + REGISTERS_AMOUNT_UNKNOWN_PARAMTERS_2))
    {
        address -= START_ADDRESS_UNKNOWN_PARAMTERS_2;
        UnknownParameters2[address][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP] = data;
    }
    else if ((address >= START_ADDRESS_UNKNOWN_PARAMTERS_3) && (address < START_ADDRESS_UNKNOWN_PARAMTERS_3 + REGISTERS_AMOUNT_UNKNOWN_PARAMTERS_3))
    {
        address -= START_ADDRESS_UNKNOWN_PARAMTERS_3;
        UnknownParameters3[address][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP] = data;
    }
    else if ((address >= START_ADDRESS_UNKNOWN_PARAMTERS_4) && (address < START_ADDRESS_UNKNOWN_PARAMTERS_4 + REGISTERS_AMOUNT_UNKNOWN_PARAMTERS_4))
    {
        address -= START_ADDRESS_UNKNOWN_PARAMTERS_4;
        UnknownParameters4[address][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP] = data;
    }
    else if ((address >= START_ADDRESS_UNKNOWN_PARAMTERS_5) && (address < START_ADDRESS_UNKNOWN_PARAMTERS_5 + REGISTERS_AMOUNT_UNKNOWN_PARAMTERS_5))
    {
        address -= START_ADDRESS_UNKNOWN_PARAMTERS_5;
        UnknownParameters5[address][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP] = data;
    }
    else if ((address >= START_ADDRESS_UNKNOWN_PARAMTERS_6) && (address < START_ADDRESS_UNKNOWN_PARAMTERS_6 + REGISTERS_AMOUNT_UNKNOWN_PARAMTERS_6))
    {
        address -= START_ADDRESS_UNKNOWN_PARAMTERS_6;
        UnknownParameters6[address][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP] = data;
    }
    else if ((address >= START_ADDRESS_UNKNOWN_PARAMTERS_7) && (address < START_ADDRESS_UNKNOWN_PARAMTERS_7 + REGISTERS_AMOUNT_UNKNOWN_PARAMTERS_7))
    {
        address -= START_ADDRESS_UNKNOWN_PARAMTERS_7;
        UnknownParameters7[address][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP] = data;
    }
    else if ((address >= START_ADDRESS_UNKNOWN_PARAMTERS_8) && (address < START_ADDRESS_UNKNOWN_PARAMTERS_8 + REGISTERS_AMOUNT_UNKNOWN_PARAMTERS_8))
    {
        address -= START_ADDRESS_UNKNOWN_PARAMTERS_8;
        UnknownParameters8[address][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP] = data;
    }
    else if ((address >= START_ADDRESS_UNKNOWN_PARAMTERS_9) && (address < START_ADDRESS_UNKNOWN_PARAMTERS_9 + REGISTERS_AMOUNT_UNKNOWN_PARAMTERS_9))
    {
        address -= START_ADDRESS_UNKNOWN_PARAMTERS_9;
        UnknownParameters9[address][PARAMETER_ARRAY_DATA_READ_FROM_HEATPUMP] = data;
    }
    else{}        
    //asm("nop");
}

char * getHeatpumpStateToString(APP_HEATPUMP_COMM_STATES logState) {
    switch(logState){
        case(APP_HEATPUMP_COMM_STATE_INIT): return "0, Init"; break;        
        case(APP_HEATPUMP_COMM_STATE_SEND_DATA): return "1, Send Data"; break;   
        case(APP_HEATPUMP_COMM_STATE_WAIT_FOR_DATA_SENT): return "2, Wait Send"; break;    
        case(APP_HEATPUMP_COMM_STATE_RECEIVE_DATA): return "3, Receive Data"; break;    
        case(APP_HEATPUMP_COMM_STATE_WAIT_FOR_DATA_RECEIVED): return "4, Wait Receive"; break;    
        case(APP_HEATPUMP_COMM_STATE_CHECKSUM_CHECK): return "5, Checksum"; break;      
        case(APP_HEATPUMP_COMM_STATE_PARSE_DATA):  return "6, Parse Data"; break;    
        case(APP_HEATPUMP_COMM_STATE_DELAY): return "7, Delay"; break;              
        default: return "8, Unkown"; break;
    }
}



static void parseReadRegs(uint8_t * txBuffer, uint8_t * rxBuffer)
{
    //uint8_t i;
   
    //uint16_t checksum;
    //uint16_t currentAddress;
    uint16_t data;
    uint16_t j = 3;
    uint16_t registerAddress = ((uint16_t)txBuffer[MODBUS_REG_ADDRESS_MSB_INDEX] << 8) + txBuffer[MODBUS_REG_ADDRESS_LSB_INDEX];
    uint16_t amountOfRegisters = ((uint16_t)txBuffer[MODBUS_REG_AMOUNT_MSB_INDEX] << 8) + txBuffer[MODBUS_REG_AMOUNT_LSB_INDEX]; 
    //uint8_t  amountOfBytes = rxBuffer[MODBUS_BYTES_RETURNED_INDEX]; 
     
    for (uint16_t i = registerAddress; i < (registerAddress + amountOfRegisters); i++)
    {
        data = ((uint16_t)rxBuffer[j] << 8) + rxBuffer[j + 1];
        j += 2;
        //checkWhatData(i);
        saveDataToMemory(i, data);
    }
}

void ParseHeatpumpData(uint8_t * txBuffer, uint8_t * rxBuffer)
{
    
    if (rxBuffer[MODBUS_ADDRESS_INDEX] == THIS_DEVICE_ADDRESS)
    {   // voor mij
        if (rxBuffer[MODBUS_COMMAND_INDEX] == MB_FC_WRITE_REG)
        {
            parseWriteReg(rxBuffer);
        }
        else if (rxBuffer[MODBUS_COMMAND_INDEX] == MB_FC_READ_REGS)
        {
            parseReadRegs(txBuffer, rxBuffer);
        }
    }
    else{} // Niet voor mij
    
}

void FillTxBuffer(uint8_t * txBuffer)
{
    static uint8_t i = 0;
    
    MANUAL_SETTING setting = (MANUAL_SETTING)PopFirstSetting();
    
    if (setting.settingStatus == SETTING_SEND_STATUS_SETTING_FILLED)
    {
        txBuffer[MODBUS_ADDRESS_INDEX] =            setting.modbusDeviceAddress;
        txBuffer[MODBUS_COMMAND_INDEX] =            setting.modbusCommand;
        txBuffer[MODBUS_REG_ADDRESS_MSB_INDEX] =    (uint8_t)(setting.modbusWriteRegister >> 8);
        txBuffer[MODBUS_REG_ADDRESS_LSB_INDEX] =    (uint8_t)(setting.modbusWriteRegister >> 0);
        txBuffer[MODBUS_MOD_DATA_MSB_INDEX] =       (uint8_t)(setting.modbusWriteData >> 8);
        txBuffer[MODBUS_MOD_DATA_LSB_INDEX] =       (uint8_t)(setting.modbusWriteData >> 0);

        uint16_t checksum = calculateCRC16(txBuffer, 6);
        txBuffer[MODBUS_CHECKSUM_LSB_INDEX] = (uint8_t)(checksum >> 0);
        txBuffer[MODBUS_CHECKSUM_MSB_INDEX] = (uint8_t)(checksum >> 8);
        
        ChangeFirstSettingStatus(SETTING_SEND_STATUS_SETTING_IS_SENT);
    }
    else
    {
        txBuffer[MODBUS_ADDRESS_INDEX] =            CommunicationArray[i][0];
        txBuffer[MODBUS_COMMAND_INDEX] =            CommunicationArray[i][1];
        txBuffer[MODBUS_REG_ADDRESS_MSB_INDEX] =    CommunicationArray[i][2];
        txBuffer[MODBUS_REG_ADDRESS_LSB_INDEX] =    CommunicationArray[i][3];
        txBuffer[MODBUS_REG_AMOUNT_MSB_INDEX] =     CommunicationArray[i][4];
        txBuffer[MODBUS_REG_AMOUNT_LSB_INDEX] =     CommunicationArray[i][5];

        uint16_t checksum = calculateCRC16(txBuffer, 6);
        txBuffer[MODBUS_CHECKSUM_LSB_INDEX] = (uint8_t)(checksum >> 0);
        txBuffer[MODBUS_CHECKSUM_MSB_INDEX] = (uint8_t)(checksum >> 8);

        if (i < 16)
            i++;
        else
            i = 0;
    }
}

uint8_t FillBufferWithStartupSettings(bool doFirstTimeHeatpumpCommunicationSettings)
{
    static uint8_t i = 0;
    switch (i)
    {
        case 0:
        {
            ChangeHeatpumpSetting(ADDRESS_PARAMETER_PASSWORD_SETTING, 255);
            i++;
            break;
        }
        
        case 1:
        {
            ChangeHeatpumpSetting(ADDRESS_TANK_TEMPERATURE_PROBE_ENABLED, TANK_TEMPERATURE_PROBE_DISABLED); 
            i++;
            break;
        }
        
        case 2:
        {
            ChangeHeatpumpSetting(ADDRESS_UNIT_TEMPERATURE_CONTROL_METHOD, UNIT_TEMPERATURE_CONTROL_METHOD_RETURN_WATER); 
            i++;
            break;
        }
        
        case 3:
        {
            ChangeHeatpumpSetting(ADDRESS_HIGH_TEMPERATURE_STERILIZATION_FUNCTION, 0);  
            i++;
            break;
        }
        
        case 4:
        {
            ChangeHeatpumpSetting(ADDRESS_DEVICE_REACHING_TARGET_TEMPERATURE_AND_SHUTDOWN_MODE, 1); 
            i++;
            break;
        }
        
        case 5:
        {
            if (doFirstTimeHeatpumpCommunicationSettings == true)
            {
                ChangeHeatpumpSetting(ADDRESS_AIR_CONDITIONER_RETURN_DIFFERENCE, 5); 
            }       
            i++;
            break;
        }
        
        default:
        {
            i = UINT8_MAX;
            break;
        }
    }
    return i;
}