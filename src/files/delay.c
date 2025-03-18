/* TODO:  Include other files here if needed. */
#include <xc.h>
#include <string.h>
#include <stdbool.h>                    // Defines true
#include "definitions.h"
#include "delay.h"



void delay_loops(uint32_t loops) 
{
	asm(".syntax unified");
	asm volatile 
    (
		"1: SUBS %[loops], %[loops], #1 \n"
		"   BNE 1b \n"
		: [loops] "+r"(loops)
	);
	asm(".syntax divided");
}

void delayUS(uint32_t delayUS)
{
    delay_us(delayUS);
    //SYSTICK_TimerRestart();
    //SYSTICK_DelayUs(delayUS);
    
    /*
    SYS_TIME_HANDLE timer = SYS_TIME_HANDLE_INVALID;
    //SYS_TIME_HANDLE timer;
    
    if (SYS_TIME_DelayUS(delayUS, &timer) != SYS_TIME_SUCCESS)
    {

    }
    else if (SYS_TIME_DelayIsComplete(timer) != true)
    {
        while (SYS_TIME_DelayIsComplete(timer) == false);
    }
    */
    
    //SYS_TIME_DelayUS(delayUS, &timer);
    //while(SYS_TIME_DelayIsComplete(timer) == false){};
}
void delayMS(uint32_t delayMS)
{
    delay_ms(delayMS);
    //SYSTICK_TimerRestart();
    //SYSTICK_DelayMs(delayMS);
    
    /*
    SYS_TIME_HANDLE timer = SYS_TIME_HANDLE_INVALID;
    //SYS_TIME_HANDLE timer;

    if (SYS_TIME_DelayMS(delayMS, &timer) != SYS_TIME_SUCCESS)
    {

    }
    else if (SYS_TIME_DelayIsComplete(timer) != true)
    {
        while (SYS_TIME_DelayIsComplete(timer) == false);
    }    
    */
}
