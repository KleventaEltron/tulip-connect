/* TODO:  Include other files here if needed. */
#include <xc.h>
#include <string.h>
#include <stdbool.h>                    // Defines true
#include "definitions.h"
#include "shiftregisters.h"

uint8_t shiftRegisterOutputStates[SHIFT_REGISTERS_AMOUNT] = {0,0};
// [0] is eerste Shift Register
// [1] is tweede Shift Register
// etc...

// LSB is eerste uitgang (QA), MSB is laatste uitgang (QH)

void InitShiftRegisters(void)
{
    ShiftRegistersNotOutputEnable_Clear();
    ShiftRegistersNotMasterReclear_Set();
    
    ShiftRegistersData_Clear();
    ShiftRegistersClock_Clear();
    ShiftRegistersLatch_Clear();
}

void smallDelay(uint8_t amountOfNop)
{
    for (int i = 0; i < amountOfNop; i++)
        asm("nop");
}

void shiftBit(bool data) 
{
    ShiftRegistersClock_Clear();
    
    smallDelay(SMALL_DELAY_NOP_AMOUNT);
    
    if (data == true)
        ShiftRegistersData_Set();
    else
        ShiftRegistersData_Clear();
    
    smallDelay(SMALL_DELAY_NOP_AMOUNT);
    
    ShiftRegistersClock_Set();
    
    smallDelay(SMALL_DELAY_NOP_AMOUNT);
    
    ShiftRegistersClock_Clear();
}

void updateShiftRegisters(void) 
{
    ShiftRegistersLatch_Clear();
    
    smallDelay(SMALL_DELAY_NOP_AMOUNT);
    
    ShiftRegistersLatch_Set();
    
    smallDelay(SMALL_DELAY_NOP_AMOUNT);
    
    ShiftRegistersLatch_Clear();
}

void SetOutput(uint16_t outputNumber, bool state) 
{
    uint16_t registerIndex = outputNumber / BITS_PER_SHIFT_REGISTER;
    uint16_t bitIndex = outputNumber % BITS_PER_SHIFT_REGISTER;

    if (outputNumber >= 0 && outputNumber < TOTAL_OUTPUTS) 
    {
        // Update de huidige status in de array zonder andere uitgangen te beïnvloeden
        shiftRegisterOutputStates[registerIndex] &= ~(1 << bitIndex); // Clear bit
        shiftRegisterOutputStates[registerIndex] |= (state << bitIndex); // Set bit

        // Update het shift register met de nieuwe status
        for (int i = SHIFT_REGISTERS_AMOUNT - 1; i >= 0; i--) 
        {
            for (int j = BITS_PER_SHIFT_REGISTER - 1; j >= 0; j--) 
            {
                shiftBit((shiftRegisterOutputStates[i] >> j) & 1);
            }
        }

        updateShiftRegisters();
    }
}

void ResetShiftRegisters(void)
{
    ShiftRegistersNotMasterReclear_Clear();
    smallDelay(DELAY_NOP_AMOUNT);
    ShiftRegistersNotMasterReclear_Set();
}