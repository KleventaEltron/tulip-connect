#include <xc.h>
#include <stdio.h>
#include <stddef.h>                     // Defines NULL
#include <stdbool.h>                    // Defines true
#include <stdlib.h>                     // Defines EXIT_FAILURE
#include <string.h>
#include "definitions.h"                // SYS function prototypes

#include "1wire.h"
#include "delay.h"

uint8_t Hwid[7]={0,0,0,0,0,0,0}; 
uint8_t CalculatedCrc = 0;

bool w1_init(void)
{
    // Configure for output
    Hwid1Wire_Set();
    Hwid1Wire_OutputEnable();
    
    // Pull low for > 480uS (master reset pulse)
    Hwid1Wire_Clear();
    delayUS(480);
    
    // Configure for input
    Hwid1Wire_InputEnable();
    delayUS(70);
    
    // Look for the line pulled low by a slave
    uint8_t result = Hwid1Wire_Get();

    // Wait for the presence pulse to finish
    // This should be less than 240uS, but the master is expected to stay
    // in Rx mode for a minimum of 480uS in total
    delayUS(460);
    
    if (result == 0)
        return true;
    else
        return false;
    
    //return result == 0;
}

void w1_write_bit(uint8_t bit)
{
    if (bit != 0) 
    { // Write high
        // Pull low for less than 15uS to write a high
        Hwid1Wire_Clear();
        delayUS(5);
        Hwid1Wire_Set();

        // Wait for the rest of the minimum slot time
        delayUS(55);
    } else 
    { // Write low
        // Pull low for 60 - 120uS to write a low
        Hwid1Wire_Clear();
        delayUS(55);

        // Stop pulling down line
        Hwid1Wire_Set();

        // Recovery time between slots
        delayUS(5);
    }
}

void w1_write(uint8_t data)
{
     // Configure for output
    Hwid1Wire_Set();
    Hwid1Wire_OutputEnable();

    for (uint8_t i = 8; i != 0; --i) 
    {
        w1_write_bit(data & 0x1);

        // Next bit (LSB first)
        data >>= 1;
    }
}

static uint8_t w1_read_bit(void)
{
    // Pull the 1-wire bus low for >1uS to generate a read slot
    Hwid1Wire_Clear();
    Hwid1Wire_OutputEnable();
    
    delayUS(1);

    // Configure for reading (releases the line)
    Hwid1Wire_InputEnable();

    // Wait for value to stabilise (bit must be read within 15uS of read slot)
    delayUS(10);
    uint8_t result = Hwid1Wire_Get() != 0;

    // Wait for the end of the read slot
    delayUS(50);
    
    return result;
}

uint8_t w1_read(void)
{
    uint8_t buffer = 0;

    // Configure for input
    Hwid1Wire_InputEnable();

    // Read 8 bits (LSB first)
    for (uint8_t whichBit = 0x01; whichBit; whichBit <<= 1) 
    {
        // Copy read bit to least significant bit of buffer
        if (w1_read_bit()) 
        {
            buffer |= whichBit;
        }
    }

    return buffer;
}

void GetHwid(uint8_t * hwidAddress)
{
	uint8_t i = 0;
	
	if (w1_init() == false)       // als geen 1-wire app. is terug gaan
		return;
    
    delayUS(10);
	
    w1_write(READ_ROM);    // nummer opvragen van DS2401    
    delayUS(5);
    
	if (w1_read() == FAMILY_CODE)  // als data afkomstig is van ds2401
	{
        delayUS(5);
        
        do
		{
			*(hwidAddress++) = w1_read(); // 6 bytes wegschrijven in array waar de verkregen
            delayUS(5);
		}	
		while (++i < 6);               // pointer naar wijst

		*(hwidAddress++) = w1_read();         // check sum ophalen en op positie 7 wegschrijven
	}
	
    
    delayMS(10);
	return;
}

void doCRC(uint8_t data) 
{
    // Lookup table for Maxim 1-Wire CRC
    const uint8_t table[256] = {
        0, 94, 188, 226, 97, 63, 221, 131, 194, 156, 126, 32, 163, 253, 31, 65,
        157, 195, 33, 127, 252, 162, 64, 30, 95, 1, 227, 189, 62, 96, 130, 220,
        35, 125, 159, 193, 66, 28, 254, 160, 225, 191, 93, 3, 128, 222, 60, 98,
        190, 224, 2, 92, 223, 129, 99, 61, 124, 34, 192, 158, 29, 67, 161, 255,
        70, 24, 250, 164, 39, 121, 155, 197, 132, 218, 56, 102, 229, 187, 89, 7,
        219, 133, 103, 57, 186, 228, 6, 88, 25, 71, 165, 251, 120, 38, 196, 154,
        101, 59, 217, 135, 4, 90, 184, 230, 167, 249, 27, 69, 198, 152, 122, 36,
        248, 166, 68, 26, 153, 199, 37, 123, 58, 100, 134, 216, 91, 5, 231, 185,
        140, 210, 48, 110, 237, 179, 81, 15, 78, 16, 242, 172, 47, 113, 147, 205,
        17, 79, 173, 243, 112, 46, 204, 146, 211, 141, 111, 49, 178, 236, 14, 80,
        175, 241, 19, 77, 206, 144, 114, 44, 109, 51, 209, 143, 12, 82, 176, 238,
        50, 108, 142, 208, 83, 13, 239, 177, 240, 174, 76, 18, 145, 207, 45, 115,
        202, 148, 118, 40, 171, 245, 23, 73, 8, 86, 180, 234, 105, 55, 213, 139,
        87, 9, 235, 181, 54, 104, 138, 212, 149, 203, 41, 119, 244, 170, 72, 22,
        233, 183, 85, 11, 136, 214, 52, 106, 43, 117, 151, 201, 74, 20, 246, 168,
        116, 42, 200, 150, 21, 75, 169, 247, 182, 232, 10, 84, 215, 137, 107, 53
    };

    // Update CRC value using the lookup table
    CalculatedCrc = table[CalculatedCrc ^ data];
}

void CalculateCrc(uint8_t * hwidAddress)
{
    for (uint8_t i = 0; i < 6; i++) 
    {
        doCRC(hwidAddress[i]);
    }
}