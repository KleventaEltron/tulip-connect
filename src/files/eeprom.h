#ifndef _EEPROM_H    /* Guard against multiple inclusion */
#define _EEPROM_H

// Define the NVMCTRL_SEESBLK_MASK_BITS to extract the NVMCTRL_SEESBLK bits(35:32) from NVM User Page Mapping(0x00804000)
#define NVMCTRL_SEESBLK_MASK_BITS   0x0F

// Define the NVMCTRL_SEEPSZ_MASK_BITS to extract the NVMCTRL_SEEPSZ bits(38:36) from NVM User Page Mapping(0x00804000)
#define NVMCTRL_SEEPSZ_MASK_BITS    0x07

/* A specific byte pattern stored at the begining of SmartEEPROM data area.
 * When the application comes from a reset, if it finds this signature,
 * the assumption is that the SmartEEPROM has some valid data.
 */
#define SMEE_CUSTOM_SIG         0x5a5a5a5a      // t/m V1-0-10, daarna niet meer
//#define SMEE_CUSTOM_SIG         0x12345678

// Define the number of bytes to be read when a read request comes 
#define MAX_BUFF_SIZE          256

// This example demo assumes fuses SBLK = 1 and PSZ = 3, thus total size is 4096 bytes 
#define SEEP_FINAL_BYTE_INDEX   4095


// Eeprom addresses:
#define SEEP_ADDR_EEPROM_VERSION        0 // 4 bytes    V1-0-8 of lager
#define SEEP_ADDR_TEST                  4 // 1 byte     V1-0-8 of lager
#define SEEP_ADDR_HEATPUMP_MODE         5 // 2 bytes    V1-0-8 of lager
#define SEEP_ADDR_HEATING_SETPOINT      7 // 2 bytes    V1-0-8 of lager
#define SEEP_ADDR_HOT_WATER_SETPOINT    9 // 2 bytes    V1-0-8 of lager

// Circulation pump settings:
#define SEEP_ADDR_PUMP_MAXIMUM_OFF_TIME_SEC                                 11 // 4 bytes   V1-0-8 of lager
#define SEEP_ADDR_PUMP_ON_TIME_AFTER_OFF_TIME_REACHED_SEC                   15 // 2 bytes   V1-0-8 of lager
#define SEEP_ADDR_PUMP_LAG_TIME_AFTER_THERMOSTAT_CONTACT_DISCONNECTED_SEC   17 // 2 bytes   V1-0-8 of lager

// Hot water settings:
#define SEEP_ADDR_HOT_WATER_RUNNING_TIME_BEFORE_TURNING_ON_HEATING_ELEMENT  19 // 2 bytes   V1-0-8 of lager
#define SEEP_ADDR_HOT_WATER_MAX_TIME_HEATING_ELEMENT_ON_IN_HEATING_MODE_SEC 21 // 2 bytes   V1-0-8 of lager

// Heating settings:
#define SEEP_ADDR_HEATING_RISE_TEMP_IN_GIVEN_TIME   23 // 2 bytes   V1-0-8 of lager
#define SEEP_ADDR_HEATING_TIME_CONSTANT_SEC         25 // 2 bytes   V1-0-8 of lager

#define SEEP_ADDR_HOT_WATER_MIN_TIME_IN_HOT_WATER_MODE  27 // 2 bytes   V1-0-8 of lager
#define SEEP_ADDR_HOT_WATER_SETPOINT_OFFSET_START       29 // 2 bytes   V1-0-8 of lager
#define SEEP_ADDR_HOT_WATER_SETPOINT_OFFSET_STEPS       31 // 2 bytes   V1-0-8 of lager

// Sterilization
#define SEEP_ADDR_DAY_COUNTER_STERILIZATION                     33 // 2 bytes   V1-0-9
#define SEEP_ADDR_SECONDS_COUNTER_DAYS                          35 // 4 bytes   V1-0-9
#define SEEP_ADDR_STERILIZATION_SETPOINT_OFFSET_START           39 // 2 bytes   V1-0-9
#define SEEP_ADDR_STERILIZATION_SETPOINT_OFFSET_STEPS           41 // 2 bytes   V1-0-9
#define SEEP_ADDR_STERILIZATION_MAX_TIME_IN_STERILIZATION_MODE  43 // 2 bytes   V1-0-9
#define SEEP_ADDR_STERILIZATION_MAX_TIME_OUT_OF_STERILIZATION_MODE_WITH_ELEMENT_ON  45 // 2 bytes   V1-0-9

#define SEEP_ADDR_DEFROSTING_TEMP_FALL_BEFORE_ELEMENT_ON    47 // 2 bytes   V1-0-9
#define SEEP_ADDR_DEFROSTING_TEMP_RISE_BEFORE_ELEMENT_OFF   49 // 2 bytes   V1-0-9

#define SEEP_ADDR_RESET_COUNTER                             51 // 4 bytes   V1-0-12
#define SEEP_ADDR_CONNECT_HAS_EVER_CONTACTED_HEATPUMP       55 // 1 bytes   V1-0-12

#define SEEP_ADDR_PUMP_OFF_TEMP_TOO_LOW                     56 // 2 bytes   V1-0-12
#define SEEP_ADDR_PUMP_ON_TEMP_AFTER_TOO_LOW_TEMP           58 // 2 bytes   V1-0-12

#define BOOTLOADER_SOFTWARE_VERSION                         60 // 4 bytes V1-0-13
//.....
#define SEEP_ADDR_LAST                  4095

void WriteSmartEeprom8(uint32_t address, uint8_t data);
void WriteSmartEeprom16(uint32_t address, uint16_t data);
void WriteSmartEeprom32(uint32_t address, uint32_t data);
uint8_t ReadSmartEeprom8(uint32_t address);
uint16_t ReadSmartEeprom16(uint32_t address);
uint32_t ReadSmartEeprom32(uint32_t address);
void SmartEepromInit(void);

#endif 