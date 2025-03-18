#ifndef _HARDWARE_REV_H    /* Guard against multiple inclusion */
#define _HARDWARE_REV_H

#include "definitions.h"

#define PIN_STATUS_HIGH_IMPEDANCE   0
#define PIN_STATUS_HIGH             1
#define PIN_STATUS_LOW              2

#define HW_REV_PIN0_GET_STATUS()    HwRev0_Get()
#define HW_REV_PIN0_SET_PULLUP()    HwRev0_Set()
#define HW_REV_PIN0_SET_PULLDOWN()  HwRev0_Clear()

#define HW_REV_PIN1_GET_STATUS()    HwRev1_Get()
#define HW_REV_PIN1_SET_PULLUP()    HwRev1_Set()
#define HW_REV_PIN1_SET_PULLDOWN()  HwRev1_Clear()

#define HW_REV_PIN2_GET_STATUS()    HwRev2_Get()
#define HW_REV_PIN2_SET_PULLUP()    HwRev2_Set()
#define HW_REV_PIN2_SET_PULLDOWN()  HwRev2_Clear()

//extern uint32_t  adc_seq_regs[4];
extern uint8_t RevNum;

void DetermineHardwareRev(void);

#endif 