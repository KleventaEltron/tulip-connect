#ifndef _MODBUS_H    /* Guard against multiple inclusion */
#define _MODBUS_H

// ERVANUITGAANDE DAT WE EEN SLAVE ZIJN!
#define THIS_DEVICE_ADDRESS 0x01

// Modbus receive defines:
#define MODBUS_ADDRESS_INDEX            0
#define MODBUS_COMMAND_INDEX            1
#define MODBUS_REG_ADDRESS_MSB_INDEX    2
#define MODBUS_REG_ADDRESS_LSB_INDEX    3
#define MODBUS_REG_AMOUNT_MSB_INDEX     4
#define MODBUS_REG_AMOUNT_LSB_INDEX     5
#define MODBUS_MOD_DATA_MSB_INDEX     4
#define MODBUS_MOD_DATA_LSB_INDEX     5
#define MODBUS_CHECKSUM_LSB_INDEX     6
#define MODBUS_CHECKSUM_MSB_INDEX     7

// Modbus Transmit defines
//#define MODBUS_ADDRESS_INDEX            0
//#define MODBUS_COMMAND_INDEX            1
#define MODBUS_BYTES_RETURNED_INDEX     2
#define MODBUS_DATA_START_INDEX         3


//Function Codes
enum {
    MB_FC_READ_COILS       = 0x01, // Read Coils (Output) Status 0xxxx
    MB_FC_READ_INPUT_STAT  = 0x02, // Read Input Status (Discrete Inputs) 1xxxx
    MB_FC_READ_REGS        = 0x03, // Read Holding Registers 4xxxx
    MB_FC_READ_INPUT_REGS  = 0x04, // Read Input Registers 3xxxx
    MB_FC_WRITE_COIL       = 0x05, // Write Single Coil (Output) 0xxxx
    MB_FC_WRITE_REG        = 0x06, // Preset Single Register 4xxxx
    MB_FC_WRITE_COILS      = 0x0F, // Write Multiple Coils (Outputs) 0xxxx
    MB_FC_WRITE_REGS       = 0x10, // Write block of contiguous registers 4xxxx
};

/*
struct CommFromDisplay 
{
  uint8_t ModbusAddress;
  uint8_t ModbusCommand;
  uint16_t ModbusStartRegister;
  uint8_t ModbusReadAmount;
  uint16_t ModbusWriteData;
  uint16_t ModbusCrc;
  uint8_t ModbusData[240];
  //char myString[30];  // String
};
*/

//uint8_t ModbusDataHandling(uint8_t* receiveBuffer, uint8_t* transmitBuffer, uint8_t sizeOfReceiveBuffer);
//void ToggleOnOffHeatpump(void);
//uint8_t DetermineSizeOfBuffer(uint8_t* receiveBuffer);
uint16_t calculateCRC16 (const uint8_t *nData, uint16_t wLength);
bool ChecksumCheck(uint8_t* rxBuffer, uint8_t rxBufferLenght);
//void ParseDisplayData(uint8_t* rxBuffer);
//uint8_t FillTransmitBuffer(uint8_t* txBuffer, uint8_t* rxHeatpump);
uint8_t CalculateModbusBufferSize(uint8_t* buffer);
//void FillTransmitBufferWithOwnData(uint8_t* txBuffer);

#endif /* _MODBUS_H */
