#ifndef _DELAY_H    /* Guard against multiple inclusion */
#define _DELAY_H

#define F_CPU CPU_CLOCK_FREQUENCY

#define DELAY_LOOP_CYCLES 3.0 // cycles per loop

#define DELAY_US_LOOPS(US) ((uint32_t)((double)(US) * F_CPU / DELAY_LOOP_CYCLES / 1000000.0))
#define DELAY_MS_LOOPS(MS) ((uint32_t)((double)(MS) * F_CPU / DELAY_LOOP_CYCLES / 1000.0))
#define DELAY_S_LOOPS (S)  ((uint32_t)((double)(S)  * F_CPU / DELAY_LOOP_CYCLES))
#define delay_us( US ) delay_loops( DELAY_US_LOOPS(US) )
#define delay_ms( MS ) delay_loops( DELAY_MS_LOOPS(MS) )
#define delay_s ( S )  delay_loops( DELAY_S_LOOPS(S) )

void delayUS(uint32_t delayUS);
void delayMS(uint32_t delayMS);

#endif 