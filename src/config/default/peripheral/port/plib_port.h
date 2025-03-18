/*******************************************************************************
  PORT PLIB

  Company:
    Microchip Technology Inc.

  File Name:
    plib_port.h

  Summary:
    PORT PLIB Header File

  Description:
    This file provides an interface to control and interact with PORT-I/O
    Pin controller module.

*******************************************************************************/

/*******************************************************************************
* Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/

#ifndef PLIB_PORT_H
#define PLIB_PORT_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "device.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility
    extern "C" {
#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Data types and constants
// *****************************************************************************
// *****************************************************************************

/*** Macros for Ntc1 pin ***/
#define Ntc1_Get()               (((PORT_REGS->GROUP[2].PORT_IN >> 0U)) & 0x01U)
#define Ntc1_PIN                  PORT_PIN_PC00

/*** Macros for Ntc2 pin ***/
#define Ntc2_Get()               (((PORT_REGS->GROUP[2].PORT_IN >> 1U)) & 0x01U)
#define Ntc2_PIN                  PORT_PIN_PC01

/*** Macros for Ntc3 pin ***/
#define Ntc3_Get()               (((PORT_REGS->GROUP[2].PORT_IN >> 2U)) & 0x01U)
#define Ntc3_PIN                  PORT_PIN_PC02

/*** Macros for Ntc4 pin ***/
#define Ntc4_Get()               (((PORT_REGS->GROUP[2].PORT_IN >> 3U)) & 0x01U)
#define Ntc4_PIN                  PORT_PIN_PC03

/*** Macros for RelayExtern1 pin ***/
#define RelayExtern1_Set()               (PORT_REGS->GROUP[0].PORT_OUTSET = ((uint32_t)1U << 2U))
#define RelayExtern1_Clear()             (PORT_REGS->GROUP[0].PORT_OUTCLR = ((uint32_t)1U << 2U))
#define RelayExtern1_Toggle()            (PORT_REGS->GROUP[0].PORT_OUTTGL = ((uint32_t)1U << 2U))
#define RelayExtern1_OutputEnable()      (PORT_REGS->GROUP[0].PORT_DIRSET = ((uint32_t)1U << 2U))
#define RelayExtern1_InputEnable()       (PORT_REGS->GROUP[0].PORT_DIRCLR = ((uint32_t)1U << 2U))
#define RelayExtern1_Get()               (((PORT_REGS->GROUP[0].PORT_IN >> 2U)) & 0x01U)
#define RelayExtern1_PIN                  PORT_PIN_PA02

/*** Macros for Hwid1Wire pin ***/
#define Hwid1Wire_Set()               (PORT_REGS->GROUP[0].PORT_OUTSET = ((uint32_t)1U << 3U))
#define Hwid1Wire_Clear()             (PORT_REGS->GROUP[0].PORT_OUTCLR = ((uint32_t)1U << 3U))
#define Hwid1Wire_Toggle()            (PORT_REGS->GROUP[0].PORT_OUTTGL = ((uint32_t)1U << 3U))
#define Hwid1Wire_OutputEnable()      (PORT_REGS->GROUP[0].PORT_DIRSET = ((uint32_t)1U << 3U))
#define Hwid1Wire_InputEnable()       (PORT_REGS->GROUP[0].PORT_DIRCLR = ((uint32_t)1U << 3U))
#define Hwid1Wire_Get()               (((PORT_REGS->GROUP[0].PORT_IN >> 3U)) & 0x01U)
#define Hwid1Wire_PIN                  PORT_PIN_PA03

/*** Macros for LedRun pin ***/
#define LedRun_Set()               (PORT_REGS->GROUP[1].PORT_OUTSET = ((uint32_t)1U << 4U))
#define LedRun_Clear()             (PORT_REGS->GROUP[1].PORT_OUTCLR = ((uint32_t)1U << 4U))
#define LedRun_Toggle()            (PORT_REGS->GROUP[1].PORT_OUTTGL = ((uint32_t)1U << 4U))
#define LedRun_OutputEnable()      (PORT_REGS->GROUP[1].PORT_DIRSET = ((uint32_t)1U << 4U))
#define LedRun_InputEnable()       (PORT_REGS->GROUP[1].PORT_DIRCLR = ((uint32_t)1U << 4U))
#define LedRun_Get()               (((PORT_REGS->GROUP[1].PORT_IN >> 4U)) & 0x01U)
#define LedRun_PIN                  PORT_PIN_PB04

/*** Macros for LedAlarm pin ***/
#define LedAlarm_Set()               (PORT_REGS->GROUP[1].PORT_OUTSET = ((uint32_t)1U << 5U))
#define LedAlarm_Clear()             (PORT_REGS->GROUP[1].PORT_OUTCLR = ((uint32_t)1U << 5U))
#define LedAlarm_Toggle()            (PORT_REGS->GROUP[1].PORT_OUTTGL = ((uint32_t)1U << 5U))
#define LedAlarm_OutputEnable()      (PORT_REGS->GROUP[1].PORT_DIRSET = ((uint32_t)1U << 5U))
#define LedAlarm_InputEnable()       (PORT_REGS->GROUP[1].PORT_DIRCLR = ((uint32_t)1U << 5U))
#define LedAlarm_Get()               (((PORT_REGS->GROUP[1].PORT_IN >> 5U)) & 0x01U)
#define LedAlarm_PIN                  PORT_PIN_PB05

/*** Macros for LedStatus pin ***/
#define LedStatus_Set()               (PORT_REGS->GROUP[3].PORT_OUTSET = ((uint32_t)1U << 0U))
#define LedStatus_Clear()             (PORT_REGS->GROUP[3].PORT_OUTCLR = ((uint32_t)1U << 0U))
#define LedStatus_Toggle()            (PORT_REGS->GROUP[3].PORT_OUTTGL = ((uint32_t)1U << 0U))
#define LedStatus_OutputEnable()      (PORT_REGS->GROUP[3].PORT_DIRSET = ((uint32_t)1U << 0U))
#define LedStatus_InputEnable()       (PORT_REGS->GROUP[3].PORT_DIRCLR = ((uint32_t)1U << 0U))
#define LedStatus_Get()               (((PORT_REGS->GROUP[3].PORT_IN >> 0U)) & 0x01U)
#define LedStatus_PIN                  PORT_PIN_PD00

/*** Macros for NotPwrCapFault pin ***/
#define NotPwrCapFault_Set()               (PORT_REGS->GROUP[3].PORT_OUTSET = ((uint32_t)1U << 1U))
#define NotPwrCapFault_Clear()             (PORT_REGS->GROUP[3].PORT_OUTCLR = ((uint32_t)1U << 1U))
#define NotPwrCapFault_Toggle()            (PORT_REGS->GROUP[3].PORT_OUTTGL = ((uint32_t)1U << 1U))
#define NotPwrCapFault_OutputEnable()      (PORT_REGS->GROUP[3].PORT_DIRSET = ((uint32_t)1U << 1U))
#define NotPwrCapFault_InputEnable()       (PORT_REGS->GROUP[3].PORT_DIRCLR = ((uint32_t)1U << 1U))
#define NotPwrCapFault_Get()               (((PORT_REGS->GROUP[3].PORT_IN >> 1U)) & 0x01U)
#define NotPwrCapFault_PIN                  PORT_PIN_PD01

/*** Macros for PwrSysGood pin ***/
#define PwrSysGood_Set()               (PORT_REGS->GROUP[1].PORT_OUTSET = ((uint32_t)1U << 6U))
#define PwrSysGood_Clear()             (PORT_REGS->GROUP[1].PORT_OUTCLR = ((uint32_t)1U << 6U))
#define PwrSysGood_Toggle()            (PORT_REGS->GROUP[1].PORT_OUTTGL = ((uint32_t)1U << 6U))
#define PwrSysGood_OutputEnable()      (PORT_REGS->GROUP[1].PORT_DIRSET = ((uint32_t)1U << 6U))
#define PwrSysGood_InputEnable()       (PORT_REGS->GROUP[1].PORT_DIRCLR = ((uint32_t)1U << 6U))
#define PwrSysGood_Get()               (((PORT_REGS->GROUP[1].PORT_IN >> 6U)) & 0x01U)
#define PwrSysGood_PIN                  PORT_PIN_PB06

/*** Macros for PwrCapGood pin ***/
#define PwrCapGood_Set()               (PORT_REGS->GROUP[1].PORT_OUTSET = ((uint32_t)1U << 7U))
#define PwrCapGood_Clear()             (PORT_REGS->GROUP[1].PORT_OUTCLR = ((uint32_t)1U << 7U))
#define PwrCapGood_Toggle()            (PORT_REGS->GROUP[1].PORT_OUTTGL = ((uint32_t)1U << 7U))
#define PwrCapGood_OutputEnable()      (PORT_REGS->GROUP[1].PORT_DIRSET = ((uint32_t)1U << 7U))
#define PwrCapGood_InputEnable()       (PORT_REGS->GROUP[1].PORT_DIRCLR = ((uint32_t)1U << 7U))
#define PwrCapGood_Get()               (((PORT_REGS->GROUP[1].PORT_IN >> 7U)) & 0x01U)
#define PwrCapGood_PIN                  PORT_PIN_PB07

/*** Macros for NotPwrPowerFail pin ***/
#define NotPwrPowerFail_Set()               (PORT_REGS->GROUP[1].PORT_OUTSET = ((uint32_t)1U << 8U))
#define NotPwrPowerFail_Clear()             (PORT_REGS->GROUP[1].PORT_OUTCLR = ((uint32_t)1U << 8U))
#define NotPwrPowerFail_Toggle()            (PORT_REGS->GROUP[1].PORT_OUTTGL = ((uint32_t)1U << 8U))
#define NotPwrPowerFail_OutputEnable()      (PORT_REGS->GROUP[1].PORT_DIRSET = ((uint32_t)1U << 8U))
#define NotPwrPowerFail_InputEnable()       (PORT_REGS->GROUP[1].PORT_DIRCLR = ((uint32_t)1U << 8U))
#define NotPwrPowerFail_Get()               (((PORT_REGS->GROUP[1].PORT_IN >> 8U)) & 0x01U)
#define NotPwrPowerFail_PIN                  PORT_PIN_PB08

/*** Macros for RelayLine1 pin ***/
#define RelayLine1_Set()               (PORT_REGS->GROUP[1].PORT_OUTSET = ((uint32_t)1U << 9U))
#define RelayLine1_Clear()             (PORT_REGS->GROUP[1].PORT_OUTCLR = ((uint32_t)1U << 9U))
#define RelayLine1_Toggle()            (PORT_REGS->GROUP[1].PORT_OUTTGL = ((uint32_t)1U << 9U))
#define RelayLine1_OutputEnable()      (PORT_REGS->GROUP[1].PORT_DIRSET = ((uint32_t)1U << 9U))
#define RelayLine1_InputEnable()       (PORT_REGS->GROUP[1].PORT_DIRCLR = ((uint32_t)1U << 9U))
#define RelayLine1_Get()               (((PORT_REGS->GROUP[1].PORT_IN >> 9U)) & 0x01U)
#define RelayLine1_PIN                  PORT_PIN_PB09

/*** Macros for TxSmartEnergyMeter pin ***/
#define TxSmartEnergyMeter_Get()               (((PORT_REGS->GROUP[0].PORT_IN >> 4U)) & 0x01U)
#define TxSmartEnergyMeter_PIN                  PORT_PIN_PA04

/*** Macros for RxSmartEnergyMeter pin ***/
#define RxSmartEnergyMeter_Get()               (((PORT_REGS->GROUP[0].PORT_IN >> 5U)) & 0x01U)
#define RxSmartEnergyMeter_PIN                  PORT_PIN_PA05

/*** Macros for TeSmartEnergyMeter pin ***/
#define TeSmartEnergyMeter_Set()               (PORT_REGS->GROUP[0].PORT_OUTSET = ((uint32_t)1U << 6U))
#define TeSmartEnergyMeter_Clear()             (PORT_REGS->GROUP[0].PORT_OUTCLR = ((uint32_t)1U << 6U))
#define TeSmartEnergyMeter_Toggle()            (PORT_REGS->GROUP[0].PORT_OUTTGL = ((uint32_t)1U << 6U))
#define TeSmartEnergyMeter_OutputEnable()      (PORT_REGS->GROUP[0].PORT_DIRSET = ((uint32_t)1U << 6U))
#define TeSmartEnergyMeter_InputEnable()       (PORT_REGS->GROUP[0].PORT_DIRCLR = ((uint32_t)1U << 6U))
#define TeSmartEnergyMeter_Get()               (((PORT_REGS->GROUP[0].PORT_IN >> 6U)) & 0x01U)
#define TeSmartEnergyMeter_PIN                  PORT_PIN_PA06

/*** Macros for RelayLine2 pin ***/
#define RelayLine2_Set()               (PORT_REGS->GROUP[0].PORT_OUTSET = ((uint32_t)1U << 7U))
#define RelayLine2_Clear()             (PORT_REGS->GROUP[0].PORT_OUTCLR = ((uint32_t)1U << 7U))
#define RelayLine2_Toggle()            (PORT_REGS->GROUP[0].PORT_OUTTGL = ((uint32_t)1U << 7U))
#define RelayLine2_OutputEnable()      (PORT_REGS->GROUP[0].PORT_DIRSET = ((uint32_t)1U << 7U))
#define RelayLine2_InputEnable()       (PORT_REGS->GROUP[0].PORT_DIRCLR = ((uint32_t)1U << 7U))
#define RelayLine2_Get()               (((PORT_REGS->GROUP[0].PORT_IN >> 7U)) & 0x01U)
#define RelayLine2_PIN                  PORT_PIN_PA07

/*** Macros for TxInverter pin ***/
#define TxInverter_Get()               (((PORT_REGS->GROUP[2].PORT_IN >> 4U)) & 0x01U)
#define TxInverter_PIN                  PORT_PIN_PC04

/*** Macros for RxInverter pin ***/
#define RxInverter_Get()               (((PORT_REGS->GROUP[2].PORT_IN >> 5U)) & 0x01U)
#define RxInverter_PIN                  PORT_PIN_PC05

/*** Macros for TeInverter pin ***/
#define TeInverter_Set()               (PORT_REGS->GROUP[2].PORT_OUTSET = ((uint32_t)1U << 6U))
#define TeInverter_Clear()             (PORT_REGS->GROUP[2].PORT_OUTCLR = ((uint32_t)1U << 6U))
#define TeInverter_Toggle()            (PORT_REGS->GROUP[2].PORT_OUTTGL = ((uint32_t)1U << 6U))
#define TeInverter_OutputEnable()      (PORT_REGS->GROUP[2].PORT_DIRSET = ((uint32_t)1U << 6U))
#define TeInverter_InputEnable()       (PORT_REGS->GROUP[2].PORT_DIRCLR = ((uint32_t)1U << 6U))
#define TeInverter_Get()               (((PORT_REGS->GROUP[2].PORT_IN >> 6U)) & 0x01U)
#define TeInverter_PIN                  PORT_PIN_PC06

/*** Macros for RelayLine3 pin ***/
#define RelayLine3_Set()               (PORT_REGS->GROUP[2].PORT_OUTSET = ((uint32_t)1U << 7U))
#define RelayLine3_Clear()             (PORT_REGS->GROUP[2].PORT_OUTCLR = ((uint32_t)1U << 7U))
#define RelayLine3_Toggle()            (PORT_REGS->GROUP[2].PORT_OUTTGL = ((uint32_t)1U << 7U))
#define RelayLine3_OutputEnable()      (PORT_REGS->GROUP[2].PORT_DIRSET = ((uint32_t)1U << 7U))
#define RelayLine3_InputEnable()       (PORT_REGS->GROUP[2].PORT_DIRCLR = ((uint32_t)1U << 7U))
#define RelayLine3_Get()               (((PORT_REGS->GROUP[2].PORT_IN >> 7U)) & 0x01U)
#define RelayLine3_PIN                  PORT_PIN_PC07

/*** Macros for RelayExtern2 pin ***/
#define RelayExtern2_Set()               (PORT_REGS->GROUP[0].PORT_OUTSET = ((uint32_t)1U << 8U))
#define RelayExtern2_Clear()             (PORT_REGS->GROUP[0].PORT_OUTCLR = ((uint32_t)1U << 8U))
#define RelayExtern2_Toggle()            (PORT_REGS->GROUP[0].PORT_OUTTGL = ((uint32_t)1U << 8U))
#define RelayExtern2_OutputEnable()      (PORT_REGS->GROUP[0].PORT_DIRSET = ((uint32_t)1U << 8U))
#define RelayExtern2_InputEnable()       (PORT_REGS->GROUP[0].PORT_DIRCLR = ((uint32_t)1U << 8U))
#define RelayExtern2_Get()               (((PORT_REGS->GROUP[0].PORT_IN >> 8U)) & 0x01U)
#define RelayExtern2_PIN                  PORT_PIN_PA08

/*** Macros for RelayPotfree1 pin ***/
#define RelayPotfree1_Set()               (PORT_REGS->GROUP[0].PORT_OUTSET = ((uint32_t)1U << 9U))
#define RelayPotfree1_Clear()             (PORT_REGS->GROUP[0].PORT_OUTCLR = ((uint32_t)1U << 9U))
#define RelayPotfree1_Toggle()            (PORT_REGS->GROUP[0].PORT_OUTTGL = ((uint32_t)1U << 9U))
#define RelayPotfree1_OutputEnable()      (PORT_REGS->GROUP[0].PORT_DIRSET = ((uint32_t)1U << 9U))
#define RelayPotfree1_InputEnable()       (PORT_REGS->GROUP[0].PORT_DIRCLR = ((uint32_t)1U << 9U))
#define RelayPotfree1_Get()               (((PORT_REGS->GROUP[0].PORT_IN >> 9U)) & 0x01U)
#define RelayPotfree1_PIN                  PORT_PIN_PA09

/*** Macros for RelayPotfree2 pin ***/
#define RelayPotfree2_Set()               (PORT_REGS->GROUP[0].PORT_OUTSET = ((uint32_t)1U << 10U))
#define RelayPotfree2_Clear()             (PORT_REGS->GROUP[0].PORT_OUTCLR = ((uint32_t)1U << 10U))
#define RelayPotfree2_Toggle()            (PORT_REGS->GROUP[0].PORT_OUTTGL = ((uint32_t)1U << 10U))
#define RelayPotfree2_OutputEnable()      (PORT_REGS->GROUP[0].PORT_DIRSET = ((uint32_t)1U << 10U))
#define RelayPotfree2_InputEnable()       (PORT_REGS->GROUP[0].PORT_DIRCLR = ((uint32_t)1U << 10U))
#define RelayPotfree2_Get()               (((PORT_REGS->GROUP[0].PORT_IN >> 10U)) & 0x01U)
#define RelayPotfree2_PIN                  PORT_PIN_PA10

/*** Macros for RelayPotfree3 pin ***/
#define RelayPotfree3_Set()               (PORT_REGS->GROUP[0].PORT_OUTSET = ((uint32_t)1U << 11U))
#define RelayPotfree3_Clear()             (PORT_REGS->GROUP[0].PORT_OUTCLR = ((uint32_t)1U << 11U))
#define RelayPotfree3_Toggle()            (PORT_REGS->GROUP[0].PORT_OUTTGL = ((uint32_t)1U << 11U))
#define RelayPotfree3_OutputEnable()      (PORT_REGS->GROUP[0].PORT_DIRSET = ((uint32_t)1U << 11U))
#define RelayPotfree3_InputEnable()       (PORT_REGS->GROUP[0].PORT_DIRCLR = ((uint32_t)1U << 11U))
#define RelayPotfree3_Get()               (((PORT_REGS->GROUP[0].PORT_IN >> 11U)) & 0x01U)
#define RelayPotfree3_PIN                  PORT_PIN_PA11

/*** Macros for InputExt1 pin ***/
#define InputExt1_Set()               (PORT_REGS->GROUP[1].PORT_OUTSET = ((uint32_t)1U << 10U))
#define InputExt1_Clear()             (PORT_REGS->GROUP[1].PORT_OUTCLR = ((uint32_t)1U << 10U))
#define InputExt1_Toggle()            (PORT_REGS->GROUP[1].PORT_OUTTGL = ((uint32_t)1U << 10U))
#define InputExt1_OutputEnable()      (PORT_REGS->GROUP[1].PORT_DIRSET = ((uint32_t)1U << 10U))
#define InputExt1_InputEnable()       (PORT_REGS->GROUP[1].PORT_DIRCLR = ((uint32_t)1U << 10U))
#define InputExt1_Get()               (((PORT_REGS->GROUP[1].PORT_IN >> 10U)) & 0x01U)
#define InputExt1_PIN                  PORT_PIN_PB10

/*** Macros for InputExt2 pin ***/
#define InputExt2_Set()               (PORT_REGS->GROUP[1].PORT_OUTSET = ((uint32_t)1U << 11U))
#define InputExt2_Clear()             (PORT_REGS->GROUP[1].PORT_OUTCLR = ((uint32_t)1U << 11U))
#define InputExt2_Toggle()            (PORT_REGS->GROUP[1].PORT_OUTTGL = ((uint32_t)1U << 11U))
#define InputExt2_OutputEnable()      (PORT_REGS->GROUP[1].PORT_DIRSET = ((uint32_t)1U << 11U))
#define InputExt2_InputEnable()       (PORT_REGS->GROUP[1].PORT_DIRCLR = ((uint32_t)1U << 11U))
#define InputExt2_Get()               (((PORT_REGS->GROUP[1].PORT_IN >> 11U)) & 0x01U)
#define InputExt2_PIN                  PORT_PIN_PB11

/*** Macros for TxHeatpumpBoiler pin ***/
#define TxHeatpumpBoiler_Get()               (((PORT_REGS->GROUP[1].PORT_IN >> 12U)) & 0x01U)
#define TxHeatpumpBoiler_PIN                  PORT_PIN_PB12

/*** Macros for RxHeatpumpBoiler pin ***/
#define RxHeatpumpBoiler_Get()               (((PORT_REGS->GROUP[1].PORT_IN >> 13U)) & 0x01U)
#define RxHeatpumpBoiler_PIN                  PORT_PIN_PB13

/*** Macros for TeHeatpumpBoiler pin ***/
#define TeHeatpumpBoiler_Set()               (PORT_REGS->GROUP[1].PORT_OUTSET = ((uint32_t)1U << 14U))
#define TeHeatpumpBoiler_Clear()             (PORT_REGS->GROUP[1].PORT_OUTCLR = ((uint32_t)1U << 14U))
#define TeHeatpumpBoiler_Toggle()            (PORT_REGS->GROUP[1].PORT_OUTTGL = ((uint32_t)1U << 14U))
#define TeHeatpumpBoiler_OutputEnable()      (PORT_REGS->GROUP[1].PORT_DIRSET = ((uint32_t)1U << 14U))
#define TeHeatpumpBoiler_InputEnable()       (PORT_REGS->GROUP[1].PORT_DIRCLR = ((uint32_t)1U << 14U))
#define TeHeatpumpBoiler_Get()               (((PORT_REGS->GROUP[1].PORT_IN >> 14U)) & 0x01U)
#define TeHeatpumpBoiler_PIN                  PORT_PIN_PB14

/*** Macros for InputExt3 pin ***/
#define InputExt3_Set()               (PORT_REGS->GROUP[1].PORT_OUTSET = ((uint32_t)1U << 15U))
#define InputExt3_Clear()             (PORT_REGS->GROUP[1].PORT_OUTCLR = ((uint32_t)1U << 15U))
#define InputExt3_Toggle()            (PORT_REGS->GROUP[1].PORT_OUTTGL = ((uint32_t)1U << 15U))
#define InputExt3_OutputEnable()      (PORT_REGS->GROUP[1].PORT_DIRSET = ((uint32_t)1U << 15U))
#define InputExt3_InputEnable()       (PORT_REGS->GROUP[1].PORT_DIRCLR = ((uint32_t)1U << 15U))
#define InputExt3_Get()               (((PORT_REGS->GROUP[1].PORT_IN >> 15U)) & 0x01U)
#define InputExt3_PIN                  PORT_PIN_PB15

/*** Macros for TxHeatpump pin ***/
#define TxHeatpump_Get()               (((PORT_REGS->GROUP[3].PORT_IN >> 8U)) & 0x01U)
#define TxHeatpump_PIN                  PORT_PIN_PD08

/*** Macros for RxHeatpump pin ***/
#define RxHeatpump_Get()               (((PORT_REGS->GROUP[3].PORT_IN >> 9U)) & 0x01U)
#define RxHeatpump_PIN                  PORT_PIN_PD09

/*** Macros for TeHeatpump pin ***/
#define TeHeatpump_Get()               (((PORT_REGS->GROUP[3].PORT_IN >> 10U)) & 0x01U)
#define TeHeatpump_PIN                  PORT_PIN_PD10

/*** Macros for ShiftRegistersData pin ***/
#define ShiftRegistersData_Set()               (PORT_REGS->GROUP[3].PORT_OUTSET = ((uint32_t)1U << 11U))
#define ShiftRegistersData_Clear()             (PORT_REGS->GROUP[3].PORT_OUTCLR = ((uint32_t)1U << 11U))
#define ShiftRegistersData_Toggle()            (PORT_REGS->GROUP[3].PORT_OUTTGL = ((uint32_t)1U << 11U))
#define ShiftRegistersData_OutputEnable()      (PORT_REGS->GROUP[3].PORT_DIRSET = ((uint32_t)1U << 11U))
#define ShiftRegistersData_InputEnable()       (PORT_REGS->GROUP[3].PORT_DIRCLR = ((uint32_t)1U << 11U))
#define ShiftRegistersData_Get()               (((PORT_REGS->GROUP[3].PORT_IN >> 11U)) & 0x01U)
#define ShiftRegistersData_PIN                  PORT_PIN_PD11

/*** Macros for ShiftRegistersNotOutputEnable pin ***/
#define ShiftRegistersNotOutputEnable_Set()               (PORT_REGS->GROUP[3].PORT_OUTSET = ((uint32_t)1U << 12U))
#define ShiftRegistersNotOutputEnable_Clear()             (PORT_REGS->GROUP[3].PORT_OUTCLR = ((uint32_t)1U << 12U))
#define ShiftRegistersNotOutputEnable_Toggle()            (PORT_REGS->GROUP[3].PORT_OUTTGL = ((uint32_t)1U << 12U))
#define ShiftRegistersNotOutputEnable_OutputEnable()      (PORT_REGS->GROUP[3].PORT_DIRSET = ((uint32_t)1U << 12U))
#define ShiftRegistersNotOutputEnable_InputEnable()       (PORT_REGS->GROUP[3].PORT_DIRCLR = ((uint32_t)1U << 12U))
#define ShiftRegistersNotOutputEnable_Get()               (((PORT_REGS->GROUP[3].PORT_IN >> 12U)) & 0x01U)
#define ShiftRegistersNotOutputEnable_PIN                  PORT_PIN_PD12

/*** Macros for ShiftRegistersLatch pin ***/
#define ShiftRegistersLatch_Set()               (PORT_REGS->GROUP[2].PORT_OUTSET = ((uint32_t)1U << 10U))
#define ShiftRegistersLatch_Clear()             (PORT_REGS->GROUP[2].PORT_OUTCLR = ((uint32_t)1U << 10U))
#define ShiftRegistersLatch_Toggle()            (PORT_REGS->GROUP[2].PORT_OUTTGL = ((uint32_t)1U << 10U))
#define ShiftRegistersLatch_OutputEnable()      (PORT_REGS->GROUP[2].PORT_DIRSET = ((uint32_t)1U << 10U))
#define ShiftRegistersLatch_InputEnable()       (PORT_REGS->GROUP[2].PORT_DIRCLR = ((uint32_t)1U << 10U))
#define ShiftRegistersLatch_Get()               (((PORT_REGS->GROUP[2].PORT_IN >> 10U)) & 0x01U)
#define ShiftRegistersLatch_PIN                  PORT_PIN_PC10

/*** Macros for GMAC_GMDC pin ***/
#define GMAC_GMDC_Get()               (((PORT_REGS->GROUP[2].PORT_IN >> 11U)) & 0x01U)
#define GMAC_GMDC_PIN                  PORT_PIN_PC11

/*** Macros for ShiftRegistersClock pin ***/
#define ShiftRegistersClock_Set()               (PORT_REGS->GROUP[2].PORT_OUTSET = ((uint32_t)1U << 13U))
#define ShiftRegistersClock_Clear()             (PORT_REGS->GROUP[2].PORT_OUTCLR = ((uint32_t)1U << 13U))
#define ShiftRegistersClock_Toggle()            (PORT_REGS->GROUP[2].PORT_OUTTGL = ((uint32_t)1U << 13U))
#define ShiftRegistersClock_OutputEnable()      (PORT_REGS->GROUP[2].PORT_DIRSET = ((uint32_t)1U << 13U))
#define ShiftRegistersClock_InputEnable()       (PORT_REGS->GROUP[2].PORT_DIRCLR = ((uint32_t)1U << 13U))
#define ShiftRegistersClock_Get()               (((PORT_REGS->GROUP[2].PORT_IN >> 13U)) & 0x01U)
#define ShiftRegistersClock_PIN                  PORT_PIN_PC13

/*** Macros for ShiftRegistersNotMasterReclear pin ***/
#define ShiftRegistersNotMasterReclear_Set()               (PORT_REGS->GROUP[2].PORT_OUTSET = ((uint32_t)1U << 14U))
#define ShiftRegistersNotMasterReclear_Clear()             (PORT_REGS->GROUP[2].PORT_OUTCLR = ((uint32_t)1U << 14U))
#define ShiftRegistersNotMasterReclear_Toggle()            (PORT_REGS->GROUP[2].PORT_OUTTGL = ((uint32_t)1U << 14U))
#define ShiftRegistersNotMasterReclear_OutputEnable()      (PORT_REGS->GROUP[2].PORT_DIRSET = ((uint32_t)1U << 14U))
#define ShiftRegistersNotMasterReclear_InputEnable()       (PORT_REGS->GROUP[2].PORT_DIRCLR = ((uint32_t)1U << 14U))
#define ShiftRegistersNotMasterReclear_Get()               (((PORT_REGS->GROUP[2].PORT_IN >> 14U)) & 0x01U)
#define ShiftRegistersNotMasterReclear_PIN                  PORT_PIN_PC14

/*** Macros for PHY_NOT_RESET pin ***/
#define PHY_NOT_RESET_Set()               (PORT_REGS->GROUP[2].PORT_OUTSET = ((uint32_t)1U << 15U))
#define PHY_NOT_RESET_Clear()             (PORT_REGS->GROUP[2].PORT_OUTCLR = ((uint32_t)1U << 15U))
#define PHY_NOT_RESET_Toggle()            (PORT_REGS->GROUP[2].PORT_OUTTGL = ((uint32_t)1U << 15U))
#define PHY_NOT_RESET_OutputEnable()      (PORT_REGS->GROUP[2].PORT_DIRSET = ((uint32_t)1U << 15U))
#define PHY_NOT_RESET_InputEnable()       (PORT_REGS->GROUP[2].PORT_DIRCLR = ((uint32_t)1U << 15U))
#define PHY_NOT_RESET_Get()               (((PORT_REGS->GROUP[2].PORT_IN >> 15U)) & 0x01U)
#define PHY_NOT_RESET_PIN                  PORT_PIN_PC15

/*** Macros for GMAC_GRX1 pin ***/
#define GMAC_GRX1_Get()               (((PORT_REGS->GROUP[0].PORT_IN >> 12U)) & 0x01U)
#define GMAC_GRX1_PIN                  PORT_PIN_PA12

/*** Macros for EthIrq pin ***/
#define EthIrq_Set()               (PORT_REGS->GROUP[0].PORT_OUTSET = ((uint32_t)1U << 16U))
#define EthIrq_Clear()             (PORT_REGS->GROUP[0].PORT_OUTCLR = ((uint32_t)1U << 16U))
#define EthIrq_Toggle()            (PORT_REGS->GROUP[0].PORT_OUTTGL = ((uint32_t)1U << 16U))
#define EthIrq_OutputEnable()      (PORT_REGS->GROUP[0].PORT_DIRSET = ((uint32_t)1U << 16U))
#define EthIrq_InputEnable()       (PORT_REGS->GROUP[0].PORT_DIRCLR = ((uint32_t)1U << 16U))
#define EthIrq_Get()               (((PORT_REGS->GROUP[0].PORT_IN >> 16U)) & 0x01U)
#define EthIrq_PIN                  PORT_PIN_PA16

/*** Macros for EthLedYellow pin ***/
#define EthLedYellow_Set()               (PORT_REGS->GROUP[2].PORT_OUTSET = ((uint32_t)1U << 16U))
#define EthLedYellow_Clear()             (PORT_REGS->GROUP[2].PORT_OUTCLR = ((uint32_t)1U << 16U))
#define EthLedYellow_Toggle()            (PORT_REGS->GROUP[2].PORT_OUTTGL = ((uint32_t)1U << 16U))
#define EthLedYellow_OutputEnable()      (PORT_REGS->GROUP[2].PORT_DIRSET = ((uint32_t)1U << 16U))
#define EthLedYellow_InputEnable()       (PORT_REGS->GROUP[2].PORT_DIRCLR = ((uint32_t)1U << 16U))
#define EthLedYellow_Get()               (((PORT_REGS->GROUP[2].PORT_IN >> 16U)) & 0x01U)
#define EthLedYellow_PIN                  PORT_PIN_PC16

/*** Macros for Reserved1 pin ***/
#define Reserved1_Set()               (PORT_REGS->GROUP[2].PORT_OUTSET = ((uint32_t)1U << 18U))
#define Reserved1_Clear()             (PORT_REGS->GROUP[2].PORT_OUTCLR = ((uint32_t)1U << 18U))
#define Reserved1_Toggle()            (PORT_REGS->GROUP[2].PORT_OUTTGL = ((uint32_t)1U << 18U))
#define Reserved1_OutputEnable()      (PORT_REGS->GROUP[2].PORT_DIRSET = ((uint32_t)1U << 18U))
#define Reserved1_InputEnable()       (PORT_REGS->GROUP[2].PORT_DIRCLR = ((uint32_t)1U << 18U))
#define Reserved1_Get()               (((PORT_REGS->GROUP[2].PORT_IN >> 18U)) & 0x01U)
#define Reserved1_PIN                  PORT_PIN_PC18

/*** Macros for Reserved2 pin ***/
#define Reserved2_Set()               (PORT_REGS->GROUP[2].PORT_OUTSET = ((uint32_t)1U << 19U))
#define Reserved2_Clear()             (PORT_REGS->GROUP[2].PORT_OUTCLR = ((uint32_t)1U << 19U))
#define Reserved2_Toggle()            (PORT_REGS->GROUP[2].PORT_OUTTGL = ((uint32_t)1U << 19U))
#define Reserved2_OutputEnable()      (PORT_REGS->GROUP[2].PORT_DIRSET = ((uint32_t)1U << 19U))
#define Reserved2_InputEnable()       (PORT_REGS->GROUP[2].PORT_DIRCLR = ((uint32_t)1U << 19U))
#define Reserved2_Get()               (((PORT_REGS->GROUP[2].PORT_IN >> 19U)) & 0x01U)
#define Reserved2_PIN                  PORT_PIN_PC19

/*** Macros for TxDisplay pin ***/
#define TxDisplay_Get()               (((PORT_REGS->GROUP[2].PORT_IN >> 22U)) & 0x01U)
#define TxDisplay_PIN                  PORT_PIN_PC22

/*** Macros for RxDisplay pin ***/
#define RxDisplay_Get()               (((PORT_REGS->GROUP[2].PORT_IN >> 23U)) & 0x01U)
#define RxDisplay_PIN                  PORT_PIN_PC23

/*** Macros for TeDisplay pin ***/
#define TeDisplay_Get()               (((PORT_REGS->GROUP[3].PORT_IN >> 20U)) & 0x01U)
#define TeDisplay_PIN                  PORT_PIN_PD20

/*** Macros for NotRtccInterrupt pin ***/
#define NotRtccInterrupt_Set()               (PORT_REGS->GROUP[3].PORT_OUTSET = ((uint32_t)1U << 21U))
#define NotRtccInterrupt_Clear()             (PORT_REGS->GROUP[3].PORT_OUTCLR = ((uint32_t)1U << 21U))
#define NotRtccInterrupt_Toggle()            (PORT_REGS->GROUP[3].PORT_OUTTGL = ((uint32_t)1U << 21U))
#define NotRtccInterrupt_OutputEnable()      (PORT_REGS->GROUP[3].PORT_DIRSET = ((uint32_t)1U << 21U))
#define NotRtccInterrupt_InputEnable()       (PORT_REGS->GROUP[3].PORT_DIRCLR = ((uint32_t)1U << 21U))
#define NotRtccInterrupt_Get()               (((PORT_REGS->GROUP[3].PORT_IN >> 21U)) & 0x01U)
#define NotRtccInterrupt_PIN                  PORT_PIN_PD21

/*** Macros for HwRev0 pin ***/
#define HwRev0_Set()               (PORT_REGS->GROUP[1].PORT_OUTSET = ((uint32_t)1U << 17U))
#define HwRev0_Clear()             (PORT_REGS->GROUP[1].PORT_OUTCLR = ((uint32_t)1U << 17U))
#define HwRev0_Toggle()            (PORT_REGS->GROUP[1].PORT_OUTTGL = ((uint32_t)1U << 17U))
#define HwRev0_OutputEnable()      (PORT_REGS->GROUP[1].PORT_DIRSET = ((uint32_t)1U << 17U))
#define HwRev0_InputEnable()       (PORT_REGS->GROUP[1].PORT_DIRCLR = ((uint32_t)1U << 17U))
#define HwRev0_Get()               (((PORT_REGS->GROUP[1].PORT_IN >> 17U)) & 0x01U)
#define HwRev0_PIN                  PORT_PIN_PB17

/*** Macros for I2CScl pin ***/
#define I2CScl_Get()               (((PORT_REGS->GROUP[0].PORT_IN >> 22U)) & 0x01U)
#define I2CScl_PIN                  PORT_PIN_PA22

/*** Macros for I2CSda pin ***/
#define I2CSda_Get()               (((PORT_REGS->GROUP[0].PORT_IN >> 23U)) & 0x01U)
#define I2CSda_PIN                  PORT_PIN_PA23

/*** Macros for NotWiFiReset pin ***/
#define NotWiFiReset_Set()               (PORT_REGS->GROUP[1].PORT_OUTSET = ((uint32_t)1U << 24U))
#define NotWiFiReset_Clear()             (PORT_REGS->GROUP[1].PORT_OUTCLR = ((uint32_t)1U << 24U))
#define NotWiFiReset_Toggle()            (PORT_REGS->GROUP[1].PORT_OUTTGL = ((uint32_t)1U << 24U))
#define NotWiFiReset_OutputEnable()      (PORT_REGS->GROUP[1].PORT_DIRSET = ((uint32_t)1U << 24U))
#define NotWiFiReset_InputEnable()       (PORT_REGS->GROUP[1].PORT_DIRCLR = ((uint32_t)1U << 24U))
#define NotWiFiReset_Get()               (((PORT_REGS->GROUP[1].PORT_IN >> 24U)) & 0x01U)
#define NotWiFiReset_PIN                  PORT_PIN_PB24

/*** Macros for WiFiEnable pin ***/
#define WiFiEnable_Set()               (PORT_REGS->GROUP[1].PORT_OUTSET = ((uint32_t)1U << 25U))
#define WiFiEnable_Clear()             (PORT_REGS->GROUP[1].PORT_OUTCLR = ((uint32_t)1U << 25U))
#define WiFiEnable_Toggle()            (PORT_REGS->GROUP[1].PORT_OUTTGL = ((uint32_t)1U << 25U))
#define WiFiEnable_OutputEnable()      (PORT_REGS->GROUP[1].PORT_DIRSET = ((uint32_t)1U << 25U))
#define WiFiEnable_InputEnable()       (PORT_REGS->GROUP[1].PORT_DIRCLR = ((uint32_t)1U << 25U))
#define WiFiEnable_Get()               (((PORT_REGS->GROUP[1].PORT_IN >> 25U)) & 0x01U)
#define WiFiEnable_PIN                  PORT_PIN_PB25

/*** Macros for WiFiMosi pin ***/
#define WiFiMosi_Get()               (((PORT_REGS->GROUP[1].PORT_IN >> 26U)) & 0x01U)
#define WiFiMosi_PIN                  PORT_PIN_PB26

/*** Macros for WiFiSck pin ***/
#define WiFiSck_Get()               (((PORT_REGS->GROUP[1].PORT_IN >> 27U)) & 0x01U)
#define WiFiSck_PIN                  PORT_PIN_PB27

/*** Macros for WiFiNotSS pin ***/
#define WiFiNotSS_Get()               (((PORT_REGS->GROUP[1].PORT_IN >> 28U)) & 0x01U)
#define WiFiNotSS_PIN                  PORT_PIN_PB28

/*** Macros for WiFiMiso pin ***/
#define WiFiMiso_Get()               (((PORT_REGS->GROUP[1].PORT_IN >> 29U)) & 0x01U)
#define WiFiMiso_PIN                  PORT_PIN_PB29

/*** Macros for WiFiInterrupt pin ***/
#define WiFiInterrupt_Set()               (PORT_REGS->GROUP[2].PORT_OUTSET = ((uint32_t)1U << 24U))
#define WiFiInterrupt_Clear()             (PORT_REGS->GROUP[2].PORT_OUTCLR = ((uint32_t)1U << 24U))
#define WiFiInterrupt_Toggle()            (PORT_REGS->GROUP[2].PORT_OUTTGL = ((uint32_t)1U << 24U))
#define WiFiInterrupt_OutputEnable()      (PORT_REGS->GROUP[2].PORT_DIRSET = ((uint32_t)1U << 24U))
#define WiFiInterrupt_InputEnable()       (PORT_REGS->GROUP[2].PORT_DIRCLR = ((uint32_t)1U << 24U))
#define WiFiInterrupt_Get()               (((PORT_REGS->GROUP[2].PORT_IN >> 24U)) & 0x01U)
#define WiFiInterrupt_PIN                  PORT_PIN_PC24

/*** Macros for WiFiWake pin ***/
#define WiFiWake_Set()               (PORT_REGS->GROUP[2].PORT_OUTSET = ((uint32_t)1U << 25U))
#define WiFiWake_Clear()             (PORT_REGS->GROUP[2].PORT_OUTCLR = ((uint32_t)1U << 25U))
#define WiFiWake_Toggle()            (PORT_REGS->GROUP[2].PORT_OUTTGL = ((uint32_t)1U << 25U))
#define WiFiWake_OutputEnable()      (PORT_REGS->GROUP[2].PORT_DIRSET = ((uint32_t)1U << 25U))
#define WiFiWake_InputEnable()       (PORT_REGS->GROUP[2].PORT_DIRCLR = ((uint32_t)1U << 25U))
#define WiFiWake_Get()               (((PORT_REGS->GROUP[2].PORT_IN >> 25U)) & 0x01U)
#define WiFiWake_PIN                  PORT_PIN_PC25

/*** Macros for NotInputSw1 pin ***/
#define NotInputSw1_Set()               (PORT_REGS->GROUP[2].PORT_OUTSET = ((uint32_t)1U << 26U))
#define NotInputSw1_Clear()             (PORT_REGS->GROUP[2].PORT_OUTCLR = ((uint32_t)1U << 26U))
#define NotInputSw1_Toggle()            (PORT_REGS->GROUP[2].PORT_OUTTGL = ((uint32_t)1U << 26U))
#define NotInputSw1_OutputEnable()      (PORT_REGS->GROUP[2].PORT_DIRSET = ((uint32_t)1U << 26U))
#define NotInputSw1_InputEnable()       (PORT_REGS->GROUP[2].PORT_DIRCLR = ((uint32_t)1U << 26U))
#define NotInputSw1_Get()               (((PORT_REGS->GROUP[2].PORT_IN >> 26U)) & 0x01U)
#define NotInputSw1_PIN                  PORT_PIN_PC26

/*** Macros for NotInputBtn1 pin ***/
#define NotInputBtn1_Get()               (((PORT_REGS->GROUP[2].PORT_IN >> 27U)) & 0x01U)
#define NotInputBtn1_PIN                  PORT_PIN_PC27
 
/*** Macros for HwRev1 pin ***/
#define HwRev1_Set()               (PORT_REGS->GROUP[2].PORT_OUTSET = ((uint32_t)1U << 28U))
#define HwRev1_Clear()             (PORT_REGS->GROUP[2].PORT_OUTCLR = ((uint32_t)1U << 28U))
#define HwRev1_Toggle()            (PORT_REGS->GROUP[2].PORT_OUTTGL = ((uint32_t)1U << 28U))
#define HwRev1_OutputEnable()      (PORT_REGS->GROUP[2].PORT_DIRSET = ((uint32_t)1U << 28U))
#define HwRev1_InputEnable()       (PORT_REGS->GROUP[2].PORT_DIRCLR = ((uint32_t)1U << 28U))
#define HwRev1_Get()               (((PORT_REGS->GROUP[2].PORT_IN >> 28U)) & 0x01U)
#define HwRev1_PIN                  PORT_PIN_PC28

/*** Macros for HwRev2 pin ***/
#define HwRev2_Set()               (PORT_REGS->GROUP[0].PORT_OUTSET = ((uint32_t)1U << 27U))
#define HwRev2_Clear()             (PORT_REGS->GROUP[0].PORT_OUTCLR = ((uint32_t)1U << 27U))
#define HwRev2_Toggle()            (PORT_REGS->GROUP[0].PORT_OUTTGL = ((uint32_t)1U << 27U))
#define HwRev2_OutputEnable()      (PORT_REGS->GROUP[0].PORT_DIRSET = ((uint32_t)1U << 27U))
#define HwRev2_InputEnable()       (PORT_REGS->GROUP[0].PORT_DIRCLR = ((uint32_t)1U << 27U))
#define HwRev2_Get()               (((PORT_REGS->GROUP[0].PORT_IN >> 27U)) & 0x01U)
#define HwRev2_PIN                  PORT_PIN_PA27
        
/*** Macros for NotInputDip4 pin ***/
#define NotInputDip4_Set()               (PORT_REGS->GROUP[1].PORT_OUTSET = ((uint32_t)1U << 31U))
#define NotInputDip4_Clear()             (PORT_REGS->GROUP[1].PORT_OUTCLR = ((uint32_t)1U << 31U))
#define NotInputDip4_Toggle()            (PORT_REGS->GROUP[1].PORT_OUTTGL = ((uint32_t)1U << 31U))
#define NotInputDip4_OutputEnable()      (PORT_REGS->GROUP[1].PORT_DIRSET = ((uint32_t)1U << 31U))
#define NotInputDip4_InputEnable()       (PORT_REGS->GROUP[1].PORT_DIRCLR = ((uint32_t)1U << 31U))
#define NotInputDip4_Get()               (((PORT_REGS->GROUP[1].PORT_IN >> 31U)) & 0x01U)
#define NotInputDip4_PIN                  PORT_PIN_PB31

/*** Macros for NotInputDip3 pin ***/
#define NotInputDip3_Set()               (PORT_REGS->GROUP[2].PORT_OUTSET = ((uint32_t)1U << 30U))
#define NotInputDip3_Clear()             (PORT_REGS->GROUP[2].PORT_OUTCLR = ((uint32_t)1U << 30U))
#define NotInputDip3_Toggle()            (PORT_REGS->GROUP[2].PORT_OUTTGL = ((uint32_t)1U << 30U))
#define NotInputDip3_OutputEnable()      (PORT_REGS->GROUP[2].PORT_DIRSET = ((uint32_t)1U << 30U))
#define NotInputDip3_InputEnable()       (PORT_REGS->GROUP[2].PORT_DIRCLR = ((uint32_t)1U << 30U))
#define NotInputDip3_Get()               (((PORT_REGS->GROUP[2].PORT_IN >> 30U)) & 0x01U)
#define NotInputDip3_PIN                  PORT_PIN_PC30

/*** Macros for NotInputDip2 pin ***/
#define NotInputDip2_Set()               (PORT_REGS->GROUP[2].PORT_OUTSET = ((uint32_t)1U << 31U))
#define NotInputDip2_Clear()             (PORT_REGS->GROUP[2].PORT_OUTCLR = ((uint32_t)1U << 31U))
#define NotInputDip2_Toggle()            (PORT_REGS->GROUP[2].PORT_OUTTGL = ((uint32_t)1U << 31U))
#define NotInputDip2_OutputEnable()      (PORT_REGS->GROUP[2].PORT_DIRSET = ((uint32_t)1U << 31U))
#define NotInputDip2_InputEnable()       (PORT_REGS->GROUP[2].PORT_DIRCLR = ((uint32_t)1U << 31U))
#define NotInputDip2_Get()               (((PORT_REGS->GROUP[2].PORT_IN >> 31U)) & 0x01U)
#define NotInputDip2_PIN                  PORT_PIN_PC31

/*** Macros for NotInputDip1 pin ***/
#define NotInputDip1_Set()               (PORT_REGS->GROUP[1].PORT_OUTSET = ((uint32_t)1U << 1U))
#define NotInputDip1_Clear()             (PORT_REGS->GROUP[1].PORT_OUTCLR = ((uint32_t)1U << 1U))
#define NotInputDip1_Toggle()            (PORT_REGS->GROUP[1].PORT_OUTTGL = ((uint32_t)1U << 1U))
#define NotInputDip1_OutputEnable()      (PORT_REGS->GROUP[1].PORT_DIRSET = ((uint32_t)1U << 1U))
#define NotInputDip1_InputEnable()       (PORT_REGS->GROUP[1].PORT_DIRCLR = ((uint32_t)1U << 1U))
#define NotInputDip1_Get()               (((PORT_REGS->GROUP[1].PORT_IN >> 1U)) & 0x01U)
#define NotInputDip1_PIN                  PORT_PIN_PB01

// *****************************************************************************
/* PORT Group

  Summary:
    Identifies the port groups available on the device.

  Description:
    These macros identifies all the ports groups that are available on this
    device.

  Remarks:
    The caller should not use the constant expressions assigned to any of
    the preprocessor macros as these may vary between devices.

    Port groups shown here are the ones available on the selected device. Not
    all ports groups are implemented. Refer to the device specific datasheet
    for more details. The MHC will generate these macros with the port
    groups that are available on the device.
*/

/* Group 0 */
#define PORT_GROUP_0 (PORT_BASE_ADDRESS + (0U * 0x80U))

/* Group 1 */
#define PORT_GROUP_1 (PORT_BASE_ADDRESS + (1U * 0x80U))

/* Group 2 */
#define PORT_GROUP_2 (PORT_BASE_ADDRESS + (2U * 0x80U))

/* Group 3 */
#define PORT_GROUP_3 (PORT_BASE_ADDRESS + (3U * 0x80U))


/* Helper macros to get port information from the pin */
#define GET_PORT_GROUP(pin)  ((PORT_GROUP)(PORT_BASE_ADDRESS + (0x80U * (((uint32_t)pin) >> 5U))))
#define GET_PIN_MASK(pin)   (((uint32_t)(0x1U)) << (((uint32_t)pin) & 0x1FU))

/* Named type for port group */
typedef uint32_t PORT_GROUP;


typedef enum
{
PERIPHERAL_FUNCTION_A = 0x0,
PERIPHERAL_FUNCTION_B = 0x1,
PERIPHERAL_FUNCTION_C = 0x2,
PERIPHERAL_FUNCTION_D = 0x3,
PERIPHERAL_FUNCTION_E = 0x4,
PERIPHERAL_FUNCTION_F = 0x5,
PERIPHERAL_FUNCTION_G = 0x6,
PERIPHERAL_FUNCTION_H = 0x7,
PERIPHERAL_FUNCTION_I = 0x8,
PERIPHERAL_FUNCTION_J = 0x9,
PERIPHERAL_FUNCTION_K = 0xA,
PERIPHERAL_FUNCTION_L = 0xB,
PERIPHERAL_FUNCTION_M = 0xC,
PERIPHERAL_FUNCTION_N = 0xD,

}PERIPHERAL_FUNCTION;

// *****************************************************************************
/* PORT Pins

  Summary:
    Identifies the available Ports pins.

  Description:
    This enumeration identifies all the ports pins that are available on this
    device.

  Remarks:
    The caller should not use the constant expressions assigned to any of
    the enumeration constants as these may vary between devices.

    Port pins shown here are the ones available on the selected device. Not
    all ports pins within a port group are implemented. Refer to the device
    specific datasheet for more details.
*/

typedef enum
{
    /* PA00 pin */
    PORT_PIN_PA00 = 0U,

    /* PA01 pin */
    PORT_PIN_PA01 = 1U,

    /* PA02 pin */
    PORT_PIN_PA02 = 2U,

    /* PA03 pin */
    PORT_PIN_PA03 = 3U,

    /* PA04 pin */
    PORT_PIN_PA04 = 4U,

    /* PA05 pin */
    PORT_PIN_PA05 = 5U,

    /* PA06 pin */
    PORT_PIN_PA06 = 6U,

    /* PA07 pin */
    PORT_PIN_PA07 = 7U,

    /* PA08 pin */
    PORT_PIN_PA08 = 8U,

    /* PA09 pin */
    PORT_PIN_PA09 = 9U,

    /* PA10 pin */
    PORT_PIN_PA10 = 10U,

    /* PA11 pin */
    PORT_PIN_PA11 = 11U,

    /* PA12 pin */
    PORT_PIN_PA12 = 12U,

    /* PA13 pin */
    PORT_PIN_PA13 = 13U,

    /* PA14 pin */
    PORT_PIN_PA14 = 14U,

    /* PA15 pin */
    PORT_PIN_PA15 = 15U,

    /* PA16 pin */
    PORT_PIN_PA16 = 16U,

    /* PA17 pin */
    PORT_PIN_PA17 = 17U,

    /* PA18 pin */
    PORT_PIN_PA18 = 18U,

    /* PA19 pin */
    PORT_PIN_PA19 = 19U,

    /* PA20 pin */
    PORT_PIN_PA20 = 20U,

    /* PA21 pin */
    PORT_PIN_PA21 = 21U,

    /* PA22 pin */
    PORT_PIN_PA22 = 22U,

    /* PA23 pin */
    PORT_PIN_PA23 = 23U,

    /* PA24 pin */
    PORT_PIN_PA24 = 24U,

    /* PA25 pin */
    PORT_PIN_PA25 = 25U,

    /* PA27 pin */
    PORT_PIN_PA27 = 27U,

    /* PA30 pin */
    PORT_PIN_PA30 = 30U,

    /* PA31 pin */
    PORT_PIN_PA31 = 31U,

    /* PB00 pin */
    PORT_PIN_PB00 = 32U,

    /* PB01 pin */
    PORT_PIN_PB01 = 33U,

    /* PB02 pin */
    PORT_PIN_PB02 = 34U,

    /* PB03 pin */
    PORT_PIN_PB03 = 35U,

    /* PB04 pin */
    PORT_PIN_PB04 = 36U,

    /* PB05 pin */
    PORT_PIN_PB05 = 37U,

    /* PB06 pin */
    PORT_PIN_PB06 = 38U,

    /* PB07 pin */
    PORT_PIN_PB07 = 39U,

    /* PB08 pin */
    PORT_PIN_PB08 = 40U,

    /* PB09 pin */
    PORT_PIN_PB09 = 41U,

    /* PB10 pin */
    PORT_PIN_PB10 = 42U,

    /* PB11 pin */
    PORT_PIN_PB11 = 43U,

    /* PB12 pin */
    PORT_PIN_PB12 = 44U,

    /* PB13 pin */
    PORT_PIN_PB13 = 45U,

    /* PB14 pin */
    PORT_PIN_PB14 = 46U,

    /* PB15 pin */
    PORT_PIN_PB15 = 47U,

    /* PB16 pin */
    PORT_PIN_PB16 = 48U,

    /* PB17 pin */
    PORT_PIN_PB17 = 49U,

    /* PB18 pin */
    PORT_PIN_PB18 = 50U,

    /* PB19 pin */
    PORT_PIN_PB19 = 51U,

    /* PB20 pin */
    PORT_PIN_PB20 = 52U,

    /* PB21 pin */
    PORT_PIN_PB21 = 53U,

    /* PB22 pin */
    PORT_PIN_PB22 = 54U,

    /* PB23 pin */
    PORT_PIN_PB23 = 55U,

    /* PB24 pin */
    PORT_PIN_PB24 = 56U,

    /* PB25 pin */
    PORT_PIN_PB25 = 57U,

    /* PB26 pin */
    PORT_PIN_PB26 = 58U,

    /* PB27 pin */
    PORT_PIN_PB27 = 59U,

    /* PB28 pin */
    PORT_PIN_PB28 = 60U,

    /* PB29 pin */
    PORT_PIN_PB29 = 61U,

    /* PB30 pin */
    PORT_PIN_PB30 = 62U,

    /* PB31 pin */
    PORT_PIN_PB31 = 63U,

    /* PC00 pin */
    PORT_PIN_PC00 = 64U,

    /* PC01 pin */
    PORT_PIN_PC01 = 65U,

    /* PC02 pin */
    PORT_PIN_PC02 = 66U,

    /* PC03 pin */
    PORT_PIN_PC03 = 67U,

    /* PC04 pin */
    PORT_PIN_PC04 = 68U,

    /* PC05 pin */
    PORT_PIN_PC05 = 69U,

    /* PC06 pin */
    PORT_PIN_PC06 = 70U,

    /* PC07 pin */
    PORT_PIN_PC07 = 71U,

    /* PC10 pin */
    PORT_PIN_PC10 = 74U,

    /* PC11 pin */
    PORT_PIN_PC11 = 75U,

    /* PC12 pin */
    PORT_PIN_PC12 = 76U,

    /* PC13 pin */
    PORT_PIN_PC13 = 77U,

    /* PC14 pin */
    PORT_PIN_PC14 = 78U,

    /* PC15 pin */
    PORT_PIN_PC15 = 79U,

    /* PC16 pin */
    PORT_PIN_PC16 = 80U,

    /* PC17 pin */
    PORT_PIN_PC17 = 81U,

    /* PC18 pin */
    PORT_PIN_PC18 = 82U,

    /* PC19 pin */
    PORT_PIN_PC19 = 83U,

    /* PC20 pin */
    PORT_PIN_PC20 = 84U,

    /* PC21 pin */
    PORT_PIN_PC21 = 85U,

    /* PC22 pin */
    PORT_PIN_PC22 = 86U,

    /* PC23 pin */
    PORT_PIN_PC23 = 87U,

    /* PC24 pin */
    PORT_PIN_PC24 = 88U,

    /* PC25 pin */
    PORT_PIN_PC25 = 89U,

    /* PC26 pin */
    PORT_PIN_PC26 = 90U,

    /* PC27 pin */
    PORT_PIN_PC27 = 91U,

    /* PC28 pin */
    PORT_PIN_PC28 = 92U,

    /* PC30 pin */
    PORT_PIN_PC30 = 94U,

    /* PC31 pin */
    PORT_PIN_PC31 = 95U,

    /* PD00 pin */
    PORT_PIN_PD00 = 96U,

    /* PD01 pin */
    PORT_PIN_PD01 = 97U,

    /* PD08 pin */
    PORT_PIN_PD08 = 104U,

    /* PD09 pin */
    PORT_PIN_PD09 = 105U,

    /* PD10 pin */
    PORT_PIN_PD10 = 106U,

    /* PD11 pin */
    PORT_PIN_PD11 = 107U,

    /* PD12 pin */
    PORT_PIN_PD12 = 108U,

    /* PD20 pin */
    PORT_PIN_PD20 = 116U,

    /* PD21 pin */
    PORT_PIN_PD21 = 117U,

    /* This element should not be used in any of the PORT APIs.
     * It will be used by other modules or application to denote that none of
     * the PORT Pin is used */
    PORT_PIN_NONE = 65535U,

} PORT_PIN;

// *****************************************************************************
// *****************************************************************************
// Section: Generated API based on pin configurations done in Pin Manager
// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
/* Function:
    void PORT_Initialize(void)

  Summary:
    Initializes the PORT Library.

  Description:
    This function initializes all ports and pins as configured in the
    MHC Pin Manager.

  Precondition:
    None.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>

    PORT_Initialize();

    </code>

  Remarks:
    The function should be called once before calling any other PORTS PLIB
    functions.
*/

void PORT_Initialize(void);

// *****************************************************************************
// *****************************************************************************
// Section: PORT APIs which operates on multiple pins of a group
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    uint32_t PORT_GroupRead(PORT_GROUP group)

  Summary:
    Read all the I/O pins in the specified port group.

  Description:
    The function reads the hardware pin state of all pins in the specified group
    and returns this as a 32 bit value. Each bit in the 32 bit value represent a
    pin. For example, bit 0 in group 0 will represent pin PA0. Bit 1 will
    represent PA1 and so on. The application should only consider the value of
    the port group pins which are implemented on the device.

  Precondition:
    The PORT_Initialize() function should have been called. Input buffer
    (INEN bit in the Pin Configuration register) should be enabled in MHC.

  Parameters:
    group - One of the IO groups from the enum PORT_GROUP.

  Returns:
    A 32-bit value representing the hardware state of of all the I/O pins in the
    selected port group.

  Example:
    <code>

    uint32_t value;
    value = PORT_Read(PORT_GROUP_C);

    </code>

  Remarks:
    None.
*/

uint32_t PORT_GroupRead(PORT_GROUP group);

// *****************************************************************************
/* Function:
    uint32_t PORT_GroupLatchRead(PORT_GROUP group)

  Summary:
    Read the data driven on all the I/O pins of the selected port group.

  Description:
    The function will return a 32-bit value representing the logic levels being
    driven on the output pins within the group. The function will not sample the
    actual hardware state of the output pin. Each bit in the 32-bit return value
    will represent one of the 32 port pins within the group. The application
    should only consider the value of the pins which are available on the
    device.

  Precondition:
    The PORT_Initialize() function should have been called.

  Parameters:
    group - One of the IO groups from the enum PORT_GROUP.

  Returns:
    A 32-bit value representing the output state of of all the I/O pins in the
    selected port group.

  Example:
    <code>

    uint32_t value;
    value = PORT_GroupLatchRead(PORT_GROUP_C);

    </code>

  Remarks:
    None.
*/

uint32_t PORT_GroupLatchRead(PORT_GROUP group);

// *****************************************************************************
/* Function:
    void PORT_GroupWrite(PORT_GROUP group, uint32_t mask, uint32_t value);

  Summary:
    Write value on the masked pins of the selected port group.

  Description:
    This function writes the value contained in the value parameter to the
    port group. Port group pins which are configured for output will be updated.
    The mask parameter provides additional control on the bits in the group to
    be affected. Setting a bit to 1 in the mask will cause the corresponding
    bit in the port group to be updated. Clearing a bit in the mask will cause
    that corresponding bit in the group to stay unaffected. For example,
    setting a mask value 0xFFFFFFFF will cause all bits in the port group
    to be updated. Setting a value 0x3 will only cause port group bit 0 and
    bit 1 to be updated.

    For port pins which are not configured for output and have the pull feature
    enabled, this function will affect pull value (pull up or pull down). A bit
    value of 1 will enable the pull up. A bit value of 0 will enable the pull
    down.

  Precondition:
    The PORT_Initialize() function should have been called.

  Parameters:
    group - One of the IO groups from the enum PORT_GROUP.

    mask  - A 32 bit value in which positions of 0s and 1s decide
             which IO pins of the selected port group will be written.
             1's - Will write to corresponding IO pins.
             0's - Will remain unchanged.

    value - Value which has to be written/driven on the I/O
             lines of the selected port for which mask bits are '1'.
             Values for the corresponding mask bit '0' will be ignored.
             Refer to the function description for effect on pins
             which are not configured for output.

  Returns:
    None.

  Example:
    <code>

    PORT_GroupWrite(PORT_GROUP_C, 0x0F, 0xF563D453);

    </code>

  Remarks:
    None.
*/

void PORT_GroupWrite(PORT_GROUP group, uint32_t mask, uint32_t value);

// *****************************************************************************
/* Function:
    void PORT_GroupSet(PORT_GROUP group, uint32_t mask)

  Summary:
    Set the selected IO pins of a group.

  Description:
    This function sets (drives a logic high) on the selected output pins of a
    group. The mask parameter control the pins to be updated. A mask bit
    position with a value 1 will cause that corresponding port pin to be set. A
    mask bit position with a value 0 will cause the corresponding port pin to
    stay un-affected.

  Precondition:
    The PORT_Initialize() function should have been called.

  Parameters:
    group - One of the IO ports from the enum PORT_GROUP.
    mask - A 32 bit value in which a bit represent a pin in the group. If the
    value of the bit is 1, the corresponding port pin will driven to logic 1. If
    the value of the bit is 0. the corresponding port pin will stay un-affected.

  Returns:
    None.

  Example:
    <code>

    PORT_GroupSet(PORT_GROUP_C, 0x00A0);

    </code>

  Remarks:
    If the port pin within the the group is not configured for output and has
    the pull feature enabled, driving a logic 1 on this pin will cause the pull
    up to be enabled.
*/

void PORT_GroupSet(PORT_GROUP group, uint32_t mask);

// *****************************************************************************
/* Function:
    void PORT_GroupClear(PORT_GROUP group, uint32_t mask)

  Summary:
    Clears the selected IO pins of a group.

  Description:
    This function clears (drives a logic 0) on the selected output pins of a
    group. The mask parameter control the pins to be updated. A mask bit
    position with a value 1 will cause that corresponding port pin to be clear.
    A mask bit position with a value 0 will cause the corresponding port pin to
    stay un-affected.

  Precondition:
    The PORT_Initialize() function should have been called.

  Parameters:
    group - One of the IO ports from the enum PORT_GROUP.
    mask - A 32 bit value in which a bit represent a pin in the group. If the
    value of the bit is 1, the corresponding port pin will driven to logic 0. If
    the value of the bit is 0. the corresponding port pin will stay un-affected.

  Returns:
    None.

  Example:
    <code>

    PORT_GroupClear(PORT_GROUP_C, 0x00A0);

    </code>

  Remarks:
    If the port pin within the the group is not configured for output and has
    the pull feature enabled, driving a logic 0 on this pin will cause the pull
    down to be enabled.
*/

void PORT_GroupClear(PORT_GROUP group, uint32_t mask);

// *****************************************************************************
/* Function:
    void PORT_GroupToggle(PORT_GROUP group, uint32_t mask)

  Summary:
    Toggles the selected IO pins of a group.

  Description:
    This function toggles the selected output pins of a group. The mask
    parameter control the pins to be updated. A mask bit position with a value 1
    will cause that corresponding port pin to be toggled.  A mask bit position
    with a value 0 will cause the corresponding port pin to stay un-affected.

  Precondition:
    The PORT_Initialize() function should have been called.

  Parameters:
    group - One of the IO ports from the enum PORT_GROUP.
    mask - A 32 bit value in which a bit represent a pin in the group. If the
    value of the bit is 1, the corresponding port pin will be toggled. If the
    value of the bit is 0. the corresponding port pin will stay un-affected.

  Returns:
    None.

  Example:
    <code>

    PORT_GroupToggle(PORT_GROUP_C, 0x00A0);

    </code>

  Remarks:
    If the port pin within the the group is not configured for output and has
    the pull feature enabled, driving a logic 0 on this pin will cause the pull
    down to be enabled. Driving a logic 1 on this pin will cause the pull up to
    be enabled.
*/

void PORT_GroupToggle(PORT_GROUP group, uint32_t mask);

// *****************************************************************************
/* Function:
    void PORT_GroupInputEnable(PORT_GROUP group, uint32_t mask)

  Summary:
    Configures the selected IO pins of a group as input.

  Description:
    This function configures the selected IO pins of a group as input. The pins
    to be configured as input are selected by setting the corresponding bits in
    the mask parameter to 1.

  Precondition:
    The PORT_Initialize() function should have been called.

  Parameters:
    group - One or more of the of the IO ports from the enum PORT_GROUP.
    mask - A 32 bit value in which a bit represents a pin in the group. If the
    value of the bit is 1, the corresponding port pin will be configured as
    input. If the value of the bit is 0. the corresponding port pin will stay
    un-affected.

  Returns:
    None.

  Example:
    <code>

    PORT_GroupInputEnable(PORT_GROUP_C, 0x00A0);

    </code>

  Remarks:
   None.
*/

void PORT_GroupInputEnable(PORT_GROUP group, uint32_t mask);

// *****************************************************************************
/* Function:
    void PORT_GroupOutputEnable(PORT_GROUP group, uint32_t mask)

  Summary:
    Configures the selected IO pins of a group as output.

  Description:
    This function configures the selected IO pins of a group as output. The pins
    to be configured as output are selected by setting the corresponding bits in
    the mask parameter to 1.

  Precondition:
    The PORT_Initialize() function should have been called.

  Parameters:
    group - One or more of the of the IO ports from the enum PORT_GROUP.
    mask - A 32 bit value in which a bit represents a pin in the group. If the
    value of the bit is 1, the corresponding port pin will be configured as
    output. If the value of the bit is 0. the corresponding port pin will stay
    un-affected.

  Returns:
    None.

  Example:
    <code>

    PORT_GroupOutputEnable(PORT_GROUP_C, 0x00A0);

    </code>

  Remarks:
    None.
*/

void PORT_GroupOutputEnable(PORT_GROUP group, uint32_t mask);

// *****************************************************************************
/* Function:
    void PORT_PinPeripheralFunctionConfig(PORT_PIN pin, PERIPHERAL_FUNCTION function)

  Summary:
    Configures the peripheral function on the selected port pin

  Description:
    This function configures the selected peripheral function on the given port pin.

  Remarks:
    None
*/
void PORT_PinPeripheralFunctionConfig(PORT_PIN pin, PERIPHERAL_FUNCTION function);

// *****************************************************************************
/* Function:
    void PORT_PinGPIOConfig(PORT_PIN pin)

  Summary:
    Configures the selected pin as GPIO

  Description:
    This function configures the given pin as GPIO.

  Remarks:
    None
*/
void PORT_PinGPIOConfig(PORT_PIN pin);

// *****************************************************************************
// *****************************************************************************
// Section: PORT APIs which operates on one pin at a time
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    void PORT_PinWrite(PORT_PIN pin, bool value)

  Summary:
    Writes the specified value to the selected pin.

  Description:
    This function writes/drives the "value" on the selected I/O line/pin.

  Precondition:
    The PORT_Initialize() function should have been called once.

  Parameters:
    pin - One of the IO pins from the enum PORT_PIN.
    value - value to be written on the selected pin.
            true  = set pin to high (1).
            false = clear pin to low (0).

  Returns:
    None.

  Example:
    <code>

    bool value = true;
    PORT_PinWrite(PORT_PIN_PB3, value);

    </code>

  Remarks:
    Calling this function with an input pin with the pull-up/pull-down feature
    enabled will affect the pull-up/pull-down configuration. If the value is
    false, the pull-down will be enabled. If the value is true, the pull-up will
    be enabled.
*/

static inline void PORT_PinWrite(PORT_PIN pin, bool value)
{
    PORT_GroupWrite(GET_PORT_GROUP(pin),
                    GET_PIN_MASK(pin),
                    (value ? GET_PIN_MASK(pin) : 0U));
}


// *****************************************************************************
/* Function:
    bool PORT_PinRead(PORT_PIN pin)

  Summary:
    Read the selected pin value.

  Description:
    This function reads the present state at the selected input pin.  The
    function can also be called to read the value of an output pin if input
    sampling on the output pin is enabled in MHC. If input synchronization on
    the pin is disabled in MHC, the function will cause a 2 PORT Clock cycles
    delay. Enabling the synchronization eliminates the delay but will increase
    power consumption.

  Precondition:
    The PORT_Initialize() function should have been called. Input buffer
    (INEN bit in the Pin Configuration register) should be enabled in MHC.

  Parameters:
    pin - the port pin whose state needs to be read.

  Returns:
    true - the state at the pin is a logic high.
    false - the state at the pin is a logic low.

  Example:
    <code>

    bool value;
    value = PORT_PinRead(PORT_PIN_PB3);

    </code>

  Remarks:
    None.
*/

static inline bool PORT_PinRead(PORT_PIN pin)
{
    return ((PORT_GroupRead(GET_PORT_GROUP(pin)) & GET_PIN_MASK(pin)) != 0U);
}


// *****************************************************************************
/* Function:
    bool PORT_PinLatchRead(PORT_PIN pin)

  Summary:
    Read the value driven on the selected pin.

  Description:
    This function reads the data driven on the selected I/O line/pin. The
    function does not sample the state of the hardware pin. It only returns the
    value that is written to output register. Refer to the PORT_PinRead()
    function if the state of the output pin needs to be read.

  Precondition:
    The PORT_Initialize() function should have been called.

  Parameters:
    pin - One of the IO pins from the enum PORT_PIN.

  Returns:
    true - the present value in the output latch is a logic high.
    false - the present value in the output latch is a logic low.

  Example:
    <code>

    bool value;
    value = PORT_PinLatchRead(PORT_PIN_PB3);

    </code>

  Remarks:
    To read actual pin value, PIN_Read API should be used.
*/

static inline bool PORT_PinLatchRead(PORT_PIN pin)
{
    return ((PORT_GroupLatchRead(GET_PORT_GROUP(pin)) & GET_PIN_MASK(pin)) != 0U);
}


// *****************************************************************************
/* Function:
    void PORT_PinToggle(PORT_PIN pin)

  Summary:
    Toggles the selected pin.

  Description:
    This function toggles/inverts the present value on the selected I/O line/pin.

  Precondition:
    The PORT_Initialize() function should have been called.

  Parameters:
    pin - One of the IO pins from the enum PORT_PIN.

  Returns:
    None.

  Example:
    <code>

    PORT_PinToggle(PORT_PIN_PB3);

    </code>

  Remarks:
    None.
*/

static inline void PORT_PinToggle(PORT_PIN pin)
{
    PORT_GroupToggle(GET_PORT_GROUP(pin), GET_PIN_MASK(pin));
}


// *****************************************************************************
/* Function:
    void PORT_PinSet(PORT_PIN pin)

  Summary:
    Sets the selected pin.

  Description:
    This function drives a logic 1 on the selected I/O line/pin.

  Precondition:
    The PORT_Initialize() function should have been called.

  Parameters:
    pin - One of the IO pins from the enum PORT_PIN.

  Returns:
    None.

  Example:
    <code>

    PORT_PinSet(PORT_PIN_PB3);

    </code>

  Remarks:
    None.
*/

static inline void PORT_PinSet(PORT_PIN pin)
{
    PORT_GroupSet(GET_PORT_GROUP(pin), GET_PIN_MASK(pin));
}


// *****************************************************************************
/* Function:
    void PORT_PinClear(PORT_PIN pin)

  Summary:
    Clears the selected pin.

  Description:
    This function drives a logic 0 on the selected I/O line/pin.

  Precondition:
    The PORT_Initialize() function should have been called.

  Parameters:
    pin - One of the IO pins from the enum PORT_PIN.

  Returns:
    None.

  Example:
    <code>

    PORT_PinClear(PORT_PIN_PB3);

    </code>

  Remarks:
    None.
*/

static inline void PORT_PinClear(PORT_PIN pin)
{
    PORT_GroupClear(GET_PORT_GROUP(pin), GET_PIN_MASK(pin));
}


// *****************************************************************************
/* Function:
    void PORT_PinInputEnable(PORT_PIN pin)

  Summary:
    Configures the selected IO pin as input.

  Description:
    This function configures the selected IO pin as input. This function
    override the MHC input output pin settings.

  Precondition:
    The PORT_Initialize() function should have been called.

  Parameters:
    pin - One of the IO pins from the enum PORT_PIN.

  Returns:
    None.

  Example:
    <code>

    PORT_PinInputEnable(PORT_PIN_PB3);

    </code>

  Remarks:
    None.
*/

static inline void PORT_PinInputEnable(PORT_PIN pin)
{
    PORT_GroupInputEnable(GET_PORT_GROUP(pin), GET_PIN_MASK(pin));
}


// *****************************************************************************
/* Function:
    void PORT_PinOutputEnable(PORT_PIN pin)

  Summary:
    Enables selected IO pin as output.

  Description:
    This function enables selected IO pin as output. Calling this function will
    override the MHC input output pin configuration.

  Precondition:
    The PORT_Initialize() function should have been called.

  Parameters:
    pin - One of the IO pins from the enum PORT_PIN.

  Returns:
    None.

  Example:
    <code>

    PORT_PinOutputEnable(PORT_PIN_PB3);

    </code>

  Remarks:
    None.
*/

static inline void PORT_PinOutputEnable(PORT_PIN pin)
{
    PORT_GroupOutputEnable(GET_PORT_GROUP(pin), GET_PIN_MASK(pin));
}

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

}

#endif
// DOM-IGNORE-END
#endif // PLIB_PORT_H
