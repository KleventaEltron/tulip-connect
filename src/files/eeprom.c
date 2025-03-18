/* TODO:  Include other files here if needed. */
#include <xc.h>
#include <string.h>
#include <stdbool.h>                    // Defines true
#include "definitions.h"
#include "eeprom.h"
#include "logging.h"

// Define a pointer to access SmartEEPROM as bytes 
uint8_t *SmartEEPROM8       = (uint8_t *)SEEPROM_ADDR;

// Define a pointer to access SmartEEPROM as int (16-bits)
uint16_t *SmartEEPROM16     = (uint16_t *)SEEPROM_ADDR;

// Define a pointer to access SmartEEPROM as words (32-bits) 
uint32_t *SmartEEPROM32     = (uint32_t *)SEEPROM_ADDR;

uint8_t eeprom_data_buffer[MAX_BUFF_SIZE] = {0};

void WriteSmartEeprom8(uint32_t address, uint8_t data)
{ 
    if (address <= SEEP_FINAL_BYTE_INDEX) 
    {
        while (NVMCTRL_SmartEEPROM_IsBusy());
        SmartEEPROM8[address] = data;
    }  
}

void WriteSmartEeprom16(uint32_t address, uint16_t data)
{  
    if ((address + 1) <= SEEP_FINAL_BYTE_INDEX)  
    {
        while (NVMCTRL_SmartEEPROM_IsBusy());
        SmartEEPROM8[address + 0] = (uint8_t)(data >> 0);
        while (NVMCTRL_SmartEEPROM_IsBusy());
        SmartEEPROM8[address + 1] = (uint8_t)(data >> 8);
    }  
}

void WriteSmartEeprom32(uint32_t address, uint32_t data)
{
    if ((address + 3) <= SEEP_FINAL_BYTE_INDEX)  
    {
        
        while (NVMCTRL_SmartEEPROM_IsBusy());
        SmartEEPROM8[address + 0] = (uint8_t)(data >> 0);
        while (NVMCTRL_SmartEEPROM_IsBusy());
        SmartEEPROM8[address + 1] = (uint8_t)(data >> 8);
        while (NVMCTRL_SmartEEPROM_IsBusy());
        SmartEEPROM8[address + 2] = (uint8_t)(data >> 16);
        while (NVMCTRL_SmartEEPROM_IsBusy());
        SmartEEPROM8[address + 3] = (uint8_t)(data >> 24);
    }  
}

uint8_t ReadSmartEeprom8(uint32_t address)
{
    if (address <= SEEP_FINAL_BYTE_INDEX) 
    {   
        // Wait till the SmartEEPROM is free 
        while (NVMCTRL_SmartEEPROM_IsBusy());
        return SmartEEPROM8[address];
    }
    else
    {
        return UINT8_MAX;
    }
}

uint16_t ReadSmartEeprom16(uint32_t address)
{
    uint16_t returnValue = 0;
    
    if (address <= SEEP_FINAL_BYTE_INDEX) 
    {   
        // Wait till the SmartEEPROM is free 
        while (NVMCTRL_SmartEEPROM_IsBusy());
        returnValue += (uint16_t)(SmartEEPROM8[address + 0] << 0);
        while (NVMCTRL_SmartEEPROM_IsBusy());
        returnValue += (uint16_t)(SmartEEPROM8[address + 1] << 8);
        
        return returnValue;
    }
    else
    {
        return UINT16_MAX;
    }
}

uint32_t ReadSmartEeprom32(uint32_t address)
{
    uint32_t returnValue = 0;
    
    if (address <= SEEP_FINAL_BYTE_INDEX) 
    {   
        // Wait till the SmartEEPROM is free 
        while (NVMCTRL_SmartEEPROM_IsBusy());
        returnValue += (uint32_t)(SmartEEPROM8[address + 0] << 0);
        while (NVMCTRL_SmartEEPROM_IsBusy());
        returnValue += (uint32_t)(SmartEEPROM8[address + 1] << 8);
        while (NVMCTRL_SmartEEPROM_IsBusy());
        returnValue += (uint32_t)(SmartEEPROM8[address + 2] << 16);
        while (NVMCTRL_SmartEEPROM_IsBusy());
        returnValue += (uint32_t)(SmartEEPROM8[address + 3] << 24);
        
        return returnValue;
    }
    else
    {
        return UINT32_MAX;
    }
}


/**
 * \brief Print a buffer as hex values
 *
 * Print a given array as a hex values
 */
/*
void print_hex_array(void *mem, uint16_t len)
{
	unsigned char *p = (unsigned char *)mem;

	for(uint32_t i = 0; i < len; i++)
    {
		if ((i != 0) && (!(i & 0x7)))
        {
			printf("\r\n");
        }
		printf("%03d ", p[i]);
	}
	printf("\r\n");
}
*/

/**
 * \brief Verify the custom data in SmartEEPROM
 *
 * Verify the custom data at initial 4 bytes of SmartEEPROM
 */
/*
int8_t verify_seep_signature(void)
{
    uint32_t        NVMCTRL_SEESBLK_FuseConfig    = ((*(uint32_t *)(USER_PAGE_ADDR + 4)) >> 0) & NVMCTRL_SEESBLK_MASK_BITS;
	int8_t          ret_val                       = 0;

	// If SBLK fuse is not configured, inform the user and wait here
	if (!NVMCTRL_SEESBLK_FuseConfig)
    {
		//printf("\r\nPlease configure SBLK fuse to allocate SmartEEPROM area\r\n");
        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "Please configure SBLK fuse to allocate SmartEEPROM area");
		while (1);
	}

    // CHANGED FROM CUSTOM SIGNATURE TO FIRMWARE VERSION!
	//if (SMEE_CUSTOM_SIG != SmartEEPROM32[0])
    if(THIS_FIRMWARE_VERSION != SmartEEPROM32[0])
    {
		ret_val = 0x4;
	}

	return ret_val;
}
*/

/**
 * \brief Invert a byte in SmartEEPROM
 *
 * To invert the data at the given index in SmartEEPROM
 */
/*
uint8_t invert_seep_byte(uint8_t index)
{
	// Wait till the SmartEEPROM is free
	while (NVMCTRL_SmartEEPROM_IsBusy());

	// Read the data, invert it, and write it back
	uint8_t data_8              = SmartEEPROM8[index];
	//printf("\r\nData at test address %d is = %d\r\n", index, (int)data_8);
	SmartEEPROM8[index] = !data_8;
	//printf("\r\nInverted the data at test address and written\r\n");
    SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "Inverted the data at test address and written");
    
    return data_8;
}
*/


void restoreEepromValuesToDefault(void)
{
    WriteSmartEeprom32(SEEP_ADDR_EEPROM_VERSION, THIS_FIRMWARE_VERSION);
    WriteSmartEeprom8 (SEEP_ADDR_TEST, 0);
    WriteSmartEeprom16(SEEP_ADDR_HEATPUMP_MODE, UINT16_MAX);
    WriteSmartEeprom16(SEEP_ADDR_HEATING_SETPOINT, TEMPERATURE_ALARM_VALUE);
    WriteSmartEeprom16(SEEP_ADDR_HOT_WATER_SETPOINT, TEMPERATURE_ALARM_VALUE);

    WriteSmartEeprom32(SEEP_ADDR_PUMP_MAXIMUM_OFF_TIME_SEC, 86400);
    WriteSmartEeprom16(SEEP_ADDR_PUMP_ON_TIME_AFTER_OFF_TIME_REACHED_SEC, 10);
    WriteSmartEeprom16(SEEP_ADDR_PUMP_LAG_TIME_AFTER_THERMOSTAT_CONTACT_DISCONNECTED_SEC, 60);

    WriteSmartEeprom16(SEEP_ADDR_HOT_WATER_RUNNING_TIME_BEFORE_TURNING_ON_HEATING_ELEMENT, 7200);   
    WriteSmartEeprom16(SEEP_ADDR_HOT_WATER_MAX_TIME_HEATING_ELEMENT_ON_IN_HEATING_MODE_SEC, 7200);  

    WriteSmartEeprom16(SEEP_ADDR_HEATING_RISE_TEMP_IN_GIVEN_TIME, 10);
    WriteSmartEeprom16(SEEP_ADDR_HEATING_TIME_CONSTANT_SEC, 6000); 

    WriteSmartEeprom16(SEEP_ADDR_HOT_WATER_MIN_TIME_IN_HOT_WATER_MODE, 600);
    WriteSmartEeprom16(SEEP_ADDR_HOT_WATER_SETPOINT_OFFSET_START, 50);
    WriteSmartEeprom16(SEEP_ADDR_HOT_WATER_SETPOINT_OFFSET_STEPS, 20);

    WriteSmartEeprom16(SEEP_ADDR_DAY_COUNTER_STERILIZATION, 0);
    WriteSmartEeprom32(SEEP_ADDR_SECONDS_COUNTER_DAYS, 0);
    WriteSmartEeprom16(SEEP_ADDR_STERILIZATION_SETPOINT_OFFSET_START, 50);
    WriteSmartEeprom16(SEEP_ADDR_STERILIZATION_SETPOINT_OFFSET_STEPS, 20);
    WriteSmartEeprom16(SEEP_ADDR_STERILIZATION_MAX_TIME_IN_STERILIZATION_MODE, 7200);
    WriteSmartEeprom16(SEEP_ADDR_STERILIZATION_MAX_TIME_OUT_OF_STERILIZATION_MODE_WITH_ELEMENT_ON, 7200);
    WriteSmartEeprom16(SEEP_ADDR_DEFROSTING_TEMP_FALL_BEFORE_ELEMENT_ON, 40);
    WriteSmartEeprom16(SEEP_ADDR_DEFROSTING_TEMP_RISE_BEFORE_ELEMENT_OFF, 30);

    WriteSmartEeprom32(SEEP_ADDR_RESET_COUNTER, 0);
    WriteSmartEeprom8 (SEEP_ADDR_CONNECT_HAS_EVER_CONTACTED_HEATPUMP, false);
    
    WriteSmartEeprom16(SEEP_ADDR_PUMP_OFF_TEMP_TOO_LOW, 100);
    WriteSmartEeprom16(SEEP_ADDR_PUMP_ON_TEMP_AFTER_TOO_LOW_TEMP, 20);
}

void SmartEepromInit(void)
{    
    //uint32_t    NVMCTRL_SEESBLK_FuseConfig  = ((*(uint32_t *)(USER_PAGE_ADDR + 4)) >> 0) & NVMCTRL_SEESBLK_MASK_BITS;
    //uint32_t    NVMCTRL_SEEPSZ_FuseConfig   = ((*(uint32_t *)(USER_PAGE_ADDR + 4)) >> 4) & NVMCTRL_SEEPSZ_MASK_BITS;
    
    //if (GetDip4() == true)
    //    printf("\r\n\r\n=============SmartEEPROM Example=============\r\n");
    
    //uint32_t    NVMCTRL_SEESBLK_FuseConfig  = ((*(uint32_t *)(USER_PAGE_ADDR + 4)) >> 0) & NVMCTRL_SEESBLK_MASK_BITS;
    //uint32_t    NVMCTRL_SEEPSZ_FuseConfig   = ((*(uint32_t *)(USER_PAGE_ADDR + 4)) >> 4) & NVMCTRL_SEEPSZ_MASK_BITS;
    
    uint32_t thisEepromVersion = ReadSmartEeprom32(SEEP_ADDR_EEPROM_VERSION);
    
    if (thisEepromVersion < 1000012) // 1.0.12
    {   // Current eeprom version is lower than 1.0.12
        // Put Smart Eeprom variables added in 1.0.12 here:
        WriteSmartEeprom32(SEEP_ADDR_RESET_COUNTER, 0);
        WriteSmartEeprom8 (SEEP_ADDR_CONNECT_HAS_EVER_CONTACTED_HEATPUMP, true);
        
        WriteSmartEeprom16(SEEP_ADDR_PUMP_OFF_TEMP_TOO_LOW, 100);
        WriteSmartEeprom16(SEEP_ADDR_PUMP_ON_TEMP_AFTER_TOO_LOW_TEMP, 20);
    }

    //if (thisEepromVersion < 1000013) // 1.0.13
    //{   // Current eeprom version is lower than 1.0.13
    //    // Put Smart Eeprom variables added in 1.0.13 here:
    //    
    //}
    
    if (thisEepromVersion < THIS_FIRMWARE_VERSION)
    {   // Update the eeprom version in the eeprom
        WriteSmartEeprom32(SEEP_ADDR_EEPROM_VERSION, THIS_FIRMWARE_VERSION);
    }
    else if (thisEepromVersion > THIS_FIRMWARE_VERSION)
    {   // Should be impossible, so when this occures, reset eeprom.
        restoreEepromValuesToDefault();
    }
    
    uint32_t resetCounter = ReadSmartEeprom32(SEEP_ADDR_RESET_COUNTER);
    resetCounter++;
    WriteSmartEeprom32(SEEP_ADDR_RESET_COUNTER, resetCounter);
    
    /*
    if (verify_seep_signature() == 0)
    {   // Eeprom already initialized
        if (GetDip4() == true)
        {
            //printf("\r\nSmartEEPROM contains valid data \r\n");
            SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "SmartEEPROM contains valid data");
        }
    }
    else
    {   // Eeprom not initialized or some fault
        if (GetDip4() == true)
        {
            //printf("\r\nStoring signature to SmartEEPROM address 0x00 to 0x03\r\n");
            SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "Storing signature to SmartEEPROM address 0x00 to 0x03");
        }
    }
    */
    /*
    if (GetDip4() == true)
    {
        for (uint32_t  i = 0; i < MAX_BUFF_SIZE; i++)
        {
            eeprom_data_buffer[i]   = SmartEEPROM8[i];
        }

        //print_hex_array(eeprom_data_buffer, MAX_BUFF_SIZE);
    }
    */
    
    /*
    if (GetDip4() == true)
    {
        printf("\r\nFuse values for SBLK = %d, PSZ = %d. See the table 'SmartEEPROM Virtual \
        Size in Bytes' in the Datasheet to calculate total available bytes \r\n",
       (int)NVMCTRL_SEESBLK_FuseConfig,
       (int)NVMCTRL_SEEPSZ_FuseConfig);
    }
    */    
    /*
    // Toggle a SmartEEPROM byte and give indication with LED0 on SAM E54 Xpro 
    invert_seep_byte(SEEP_ADDR_TEST);

    // Check the data at test address and show indication on LED0 
    if (SmartEEPROM8[SEEP_ADDR_TEST])
    {
        LedStatus_Set();
    }
    else
    {
        LedStatus_Clear();
    }
    */    
}

/*
void SmartEepromInit(void)
{
    uint8_t buffer[MAX_BUFF_SIZE] = {0};
    
    uint32_t    NVMCTRL_SEESBLK_FuseConfig  = ((*(uint32_t *)(USER_PAGE_ADDR + 4)) >> 0) & NVMCTRL_SEESBLK_MASK_BITS;
    uint32_t    NVMCTRL_SEEPSZ_FuseConfig   = ((*(uint32_t *)(USER_PAGE_ADDR + 4)) >> 4) & NVMCTRL_SEEPSZ_MASK_BITS;
    
    printf("\r\nFuse values for SBLK = %d, PSZ = %d. See the table 'SmartEEPROM Virtual \
	Size in Bytes' in the Datasheet to calculate total available bytes \r\n",
	       (int)NVMCTRL_SEESBLK_FuseConfig,
	       (int)NVMCTRL_SEEPSZ_FuseConfig);
    
    if (verify_seep_signature() == 0)
    {
		printf("\r\nSmartEEPROM contains valid data \r\n");
	}
    else
    {
		printf("\r\nStoring signature to SmartEEPROM address 0x00 to 0x03\r\n");
        // Wait till the SmartEEPROM is free 
        while (NVMCTRL_SmartEEPROM_IsBusy())
        {
            ;
        }

		SmartEEPROM32[0] = SMEE_CUSTOM_SIG;
	}
    
    uint8_t data = invert_seep_byte(SEEP_ADDR_TEST);
    
    if (data)
    {
        LedStatus_Set();
	}
    else
    {
        LedStatus_Clear();
	}
    
    if (GetDip4() == true)
    {   // Print debug eeprom data
        for (uint32_t  i = 0; i < MAX_BUFF_SIZE; i++)
        {
            buffer[i]   = SmartEEPROM8[i];
        }

        print_hex_array(buffer, MAX_BUFF_SIZE);
    }
}
*/
        
/*
void WriteSmartEeprom16(uint32_t address, uint16_t data)
{
    if ((address + 1) > SEEP_FINAL_BYTE_INDEX) 
    {
        if (GetDip4() == true)
            printf("\r\nERROR: Address invalid. Try again \r\n");
    }
    else
    {
        WriteSmartEeprom8(address, (uint8_t)(data >> 0));
        WriteSmartEeprom8((address+1), (uint8_t)(data >> 8));
    }
}
*/


/**
 * \brief Invert a byte in SmartEEPROM
 *
 * To invert the data at the given index in SmartEEPROM
 */
/*
void invert_seep_byte(uint8_t index)
{
	// Wait till the SmartEEPROM is free 
	while (NVMCTRL_SmartEEPROM_IsBusy());

	// Read the data, invert it, and write it back 
	data_8              = SmartEEPROM8[index];
    
    if (GetDip4() == true)
    {
        printf("\r\nData at test address %d is = %d\r\n", index, (int)data_8);
    }
        
    
	SmartEEPROM8[index] = !data_8;
    
    if (GetDip4() == true)
    {
        printf("\r\nInverted the data at test address and written\r\n");
    }
        
}
*/
/*
void ReadSmartEeprom(uint16_t address, uint16_t length)
{
    if (address > SEEP_FINAL_BYTE_INDEX) 
    {   
        printf("\r\nERROR: Address invalid. Try again \r\n");
        //return UINT32_MAX;
    }
    
    if (length > MAX_BUFF_SIZE)
    {
        printf("\r\nERROR: In this Demo, at a time demo can able to print 256 bytes. Try again \r\n");
        //return UINT32_MAX;
    }

    
    if((address + length) > SEEP_FINAL_BYTE_INDEX)
    {
        length    = (SEEP_FINAL_BYTE_INDEX + 1 - address);
    }
    
    for (uint32_t  i = 0; i < length; i++)
    {
        eeprom_data_buffer[i]   = SmartEEPROM8[address + i];
    }
    
    printf("\r\nEEPROM Data from location: %d to location: %d: \r\n", (int)address, (int)(address + length - 1));
    print_hex_array(eeprom_data_buffer, length);

}
*/

