/*******************************************************************************
  SERCOM Universal Synchronous/Asynchrnous Receiver/Transmitter PLIB

  Company
    Microchip Technology Inc.

  File Name
    plib_sercom7_usart.c

  Summary
    USART peripheral library interface.

  Description
    This file defines the interface to the USART peripheral library. This
    library provides access to and control of the associated peripheral
    instance.

  Remarks:
    None.
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

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "interrupts.h"
#include "plib_sercom7_usart.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data
// *****************************************************************************
// *****************************************************************************


/* SERCOM7 USART baud value for 9600 Hz baud rate */
#define SERCOM7_USART_INT_BAUD_VALUE            (65368UL)

volatile static SERCOM_USART_OBJECT sercom7USARTObj;

// *****************************************************************************
// *****************************************************************************
// Section: SERCOM7 USART Interface Routines
// *****************************************************************************
// *****************************************************************************

void static SERCOM7_USART_ErrorClear( void )
{
    uint8_t  u8dummyData = 0U;
    USART_ERROR errorStatus = (USART_ERROR) (SERCOM7_REGS->USART_INT.SERCOM_STATUS & (uint16_t)(SERCOM_USART_INT_STATUS_PERR_Msk | SERCOM_USART_INT_STATUS_FERR_Msk | SERCOM_USART_INT_STATUS_BUFOVF_Msk ));

    if(errorStatus != USART_ERROR_NONE)
    {
        /* Clear error flag */
        SERCOM7_REGS->USART_INT.SERCOM_INTFLAG = (uint8_t)SERCOM_USART_INT_INTFLAG_ERROR_Msk;
        /* Clear all errors */
        SERCOM7_REGS->USART_INT.SERCOM_STATUS = (uint16_t)(SERCOM_USART_INT_STATUS_PERR_Msk | SERCOM_USART_INT_STATUS_FERR_Msk | SERCOM_USART_INT_STATUS_BUFOVF_Msk);

        /* Flush existing error bytes from the RX FIFO */
        while((SERCOM7_REGS->USART_INT.SERCOM_INTFLAG & (uint8_t)SERCOM_USART_INT_INTFLAG_RXC_Msk) == (uint8_t)SERCOM_USART_INT_INTFLAG_RXC_Msk)
        {
            u8dummyData = (uint8_t)SERCOM7_REGS->USART_INT.SERCOM_DATA;
        }
    }

    /* Ignore the warning */
    (void)u8dummyData;
}

void SERCOM7_USART_Initialize( void )
{
    /*
     * Configures USART Clock Mode
     * Configures TXPO and RXPO
     * Configures Data Order
     * Configures Standby Mode
     * Configures Sampling rate
     * Configures IBON
     */
    SERCOM7_REGS->USART_INT.SERCOM_CTRLA = SERCOM_USART_INT_CTRLA_MODE_USART_INT_CLK | SERCOM_USART_INT_CTRLA_RXPO(0x1UL) | SERCOM_USART_INT_CTRLA_TXPO(0x3UL) | SERCOM_USART_INT_CTRLA_DORD_Msk | SERCOM_USART_INT_CTRLA_IBON_Msk | SERCOM_USART_INT_CTRLA_FORM(0x0UL) | SERCOM_USART_INT_CTRLA_SAMPR(0UL) ;

    /* Configure Baud Rate */
    SERCOM7_REGS->USART_INT.SERCOM_BAUD = (uint16_t)SERCOM_USART_INT_BAUD_BAUD(SERCOM7_USART_INT_BAUD_VALUE);

    /*
     * Configures RXEN
     * Configures TXEN
     * Configures CHSIZE
     * Configures Parity
     * Configures Stop bits
     */
    SERCOM7_REGS->USART_INT.SERCOM_CTRLB = SERCOM_USART_INT_CTRLB_CHSIZE_8_BIT | SERCOM_USART_INT_CTRLB_SBMODE_1_BIT | SERCOM_USART_INT_CTRLB_RXEN_Msk | SERCOM_USART_INT_CTRLB_TXEN_Msk;

    /* Wait for sync */
    while((SERCOM7_REGS->USART_INT.SERCOM_SYNCBUSY) != 0U)
    {
        /* Do nothing */
    }

    /* Configures RS485 Guard Time */
    SERCOM7_REGS->USART_INT.SERCOM_CTRLC = SERCOM_USART_INT_CTRLC_GTIME(0UL);

    /* Enable the UART after the configurations */
    SERCOM7_REGS->USART_INT.SERCOM_CTRLA |= SERCOM_USART_INT_CTRLA_ENABLE_Msk;

    /* Wait for sync */
    while((SERCOM7_REGS->USART_INT.SERCOM_SYNCBUSY) != 0U)
    {
        /* Do nothing */
    }

    /* Initialize instance object */
    sercom7USARTObj.rxBuffer = NULL;
    sercom7USARTObj.rxSize = 0;
    sercom7USARTObj.rxProcessedSize = 0;
    sercom7USARTObj.rxBusyStatus = false;
    sercom7USARTObj.rxCallback = NULL;
    sercom7USARTObj.txBuffer = NULL;
    sercom7USARTObj.txSize = 0;
    sercom7USARTObj.txProcessedSize = 0;
    sercom7USARTObj.txBusyStatus = false;
    sercom7USARTObj.txCallback = NULL;
    sercom7USARTObj.errorStatus = USART_ERROR_NONE;
}

uint32_t SERCOM7_USART_FrequencyGet( void )
{
    return 60000000UL;
}

bool SERCOM7_USART_SerialSetup( USART_SERIAL_SETUP * serialSetup, uint32_t clkFrequency )
{
    bool setupStatus       = false;
    uint32_t baudValue     = 0U;
    uint32_t sampleRate    = 0U;

    bool transferProgress = sercom7USARTObj.txBusyStatus;
    transferProgress = sercom7USARTObj.rxBusyStatus || transferProgress;
    if(transferProgress)
    {
        /* Transaction is in progress, so return without updating settings */
        return setupStatus;
    }

    if((serialSetup != NULL) && (serialSetup->baudRate != 0U))
    {
        if(clkFrequency == 0U)
        {
            clkFrequency = SERCOM7_USART_FrequencyGet();
        }

        if(clkFrequency >= (16U * serialSetup->baudRate))
        {
            baudValue = 65536U - (uint32_t)(((uint64_t)65536U * 16U * serialSetup->baudRate) / clkFrequency);
            sampleRate = 0U;
        }
        else if(clkFrequency >= (8U * serialSetup->baudRate))
        {
            baudValue = 65536U - (uint32_t)(((uint64_t)65536U * 8U * serialSetup->baudRate) / clkFrequency);
            sampleRate = 2U;
        }
        else if(clkFrequency >= (3U * serialSetup->baudRate))
        {
            baudValue = 65536U - (uint32_t)(((uint64_t)65536U * 3U * serialSetup->baudRate) / clkFrequency);
            sampleRate = 4U;
        }
        else
        {
            /* Do nothing */
        }

        /* Disable the USART before configurations */
        SERCOM7_REGS->USART_INT.SERCOM_CTRLA &= ~SERCOM_USART_INT_CTRLA_ENABLE_Msk;

        /* Wait for sync */
        while((SERCOM7_REGS->USART_INT.SERCOM_SYNCBUSY) != 0U)
        {
            /* Do nothing */
        }

        /* Configure Baud Rate */
        SERCOM7_REGS->USART_INT.SERCOM_BAUD = (uint16_t)SERCOM_USART_INT_BAUD_BAUD(baudValue);

        /* Configure Parity Options */
        if(serialSetup->parity == USART_PARITY_NONE)
        {
            SERCOM7_REGS->USART_INT.SERCOM_CTRLA =  (SERCOM7_REGS->USART_INT.SERCOM_CTRLA & ~(SERCOM_USART_INT_CTRLA_SAMPR_Msk | SERCOM_USART_INT_CTRLA_FORM_Msk)) | SERCOM_USART_INT_CTRLA_FORM(0x0UL) | SERCOM_USART_INT_CTRLA_SAMPR((uint32_t)sampleRate); 
            SERCOM7_REGS->USART_INT.SERCOM_CTRLB = (SERCOM7_REGS->USART_INT.SERCOM_CTRLB & ~(SERCOM_USART_INT_CTRLB_CHSIZE_Msk | SERCOM_USART_INT_CTRLB_SBMODE_Msk)) | ((uint32_t) serialSetup->dataWidth | (uint32_t) serialSetup->stopBits);
        }
        else
        {
            SERCOM7_REGS->USART_INT.SERCOM_CTRLA =  (SERCOM7_REGS->USART_INT.SERCOM_CTRLA & ~(SERCOM_USART_INT_CTRLA_SAMPR_Msk | SERCOM_USART_INT_CTRLA_FORM_Msk)) | SERCOM_USART_INT_CTRLA_FORM(0x1UL) | SERCOM_USART_INT_CTRLA_SAMPR((uint32_t)sampleRate); 
            SERCOM7_REGS->USART_INT.SERCOM_CTRLB = (SERCOM7_REGS->USART_INT.SERCOM_CTRLB & ~(SERCOM_USART_INT_CTRLB_CHSIZE_Msk | SERCOM_USART_INT_CTRLB_SBMODE_Msk | SERCOM_USART_INT_CTRLB_PMODE_Msk)) | (uint32_t) serialSetup->dataWidth | (uint32_t) serialSetup->stopBits | (uint32_t) serialSetup->parity ;
        }

        /* Wait for sync */
        while((SERCOM7_REGS->USART_INT.SERCOM_SYNCBUSY) != 0U)
        {
            /* Do nothing */
        }

        /* Enable the USART after the configurations */
        SERCOM7_REGS->USART_INT.SERCOM_CTRLA |= SERCOM_USART_INT_CTRLA_ENABLE_Msk;

        /* Wait for sync */
        while((SERCOM7_REGS->USART_INT.SERCOM_SYNCBUSY) != 0U)
        {
            /* Do nothing */
        }

        setupStatus = true;
    }

    return setupStatus;
}

USART_ERROR SERCOM7_USART_ErrorGet( void )
{
    USART_ERROR errorStatus = sercom7USARTObj.errorStatus;

    sercom7USARTObj.errorStatus = USART_ERROR_NONE;

    return errorStatus;
}

void SERCOM7_USART_Enable( void )
{
    if((SERCOM7_REGS->USART_INT.SERCOM_CTRLA & SERCOM_USART_INT_CTRLA_ENABLE_Msk) == 0U)
    {
        SERCOM7_REGS->USART_INT.SERCOM_CTRLA |= SERCOM_USART_INT_CTRLA_ENABLE_Msk;

        /* Wait for sync */
        while((SERCOM7_REGS->USART_INT.SERCOM_SYNCBUSY) != 0U)
        {
            /* Do nothing */
        }
    }
}

void SERCOM7_USART_Disable( void )
{
    if((SERCOM7_REGS->USART_INT.SERCOM_CTRLA & SERCOM_USART_INT_CTRLA_ENABLE_Msk) != 0U)
    {
        SERCOM7_REGS->USART_INT.SERCOM_CTRLA &= ~SERCOM_USART_INT_CTRLA_ENABLE_Msk;

        /* Wait for sync */
        while((SERCOM7_REGS->USART_INT.SERCOM_SYNCBUSY) != 0U)
        {
            /* Do nothing */
        }
    }
}


void SERCOM7_USART_TransmitterEnable( void )
{
    SERCOM7_REGS->USART_INT.SERCOM_CTRLB |= SERCOM_USART_INT_CTRLB_TXEN_Msk;

    /* Wait for sync */
    while((SERCOM7_REGS->USART_INT.SERCOM_SYNCBUSY) != 0U)
    {
        /* Do nothing */
    }
}

void SERCOM7_USART_TransmitterDisable( void )
{
    SERCOM7_REGS->USART_INT.SERCOM_CTRLB &= ~SERCOM_USART_INT_CTRLB_TXEN_Msk;

    /* Wait for sync */
    while((SERCOM7_REGS->USART_INT.SERCOM_SYNCBUSY) != 0U)
    {
        /* Do nothing */
    }
}

bool SERCOM7_USART_Write( void *buffer, const size_t size )
{
    bool writeStatus      = false;
    uint32_t processedSize = 0U;

    if(buffer != NULL)
    {
        if(sercom7USARTObj.txBusyStatus == false)
        {
            sercom7USARTObj.txBuffer = buffer;
            sercom7USARTObj.txSize = size;
            sercom7USARTObj.txBusyStatus = true;

            size_t txSize = sercom7USARTObj.txSize;

            /* Initiate the transfer by sending first byte */
            while (((SERCOM7_REGS->USART_INT.SERCOM_INTFLAG & SERCOM_USART_INT_INTFLAG_DRE_Msk) == SERCOM_USART_INT_INTFLAG_DRE_Msk) &&
                    (processedSize < txSize))
            {
                if (((SERCOM7_REGS->USART_INT.SERCOM_CTRLB & SERCOM_USART_INT_CTRLB_CHSIZE_Msk) >> SERCOM_USART_INT_CTRLB_CHSIZE_Pos) != 0x01U)
                {
                    /* 8-bit mode */
                    SERCOM7_REGS->USART_INT.SERCOM_DATA = ((uint8_t*)(buffer))[processedSize];
                }
                else
                {
                    /* 9-bit mode */
                    SERCOM7_REGS->USART_INT.SERCOM_DATA = ((uint16_t*)(buffer))[processedSize];
                }
                processedSize += 1U;
            }
            sercom7USARTObj.txProcessedSize = processedSize;
            SERCOM7_REGS->USART_INT.SERCOM_INTENSET = (uint8_t)SERCOM_USART_INT_INTFLAG_DRE_Msk;

            writeStatus = true;
        }
    }

    return writeStatus;
}


bool SERCOM7_USART_WriteIsBusy( void )
{
    return sercom7USARTObj.txBusyStatus;
}

size_t SERCOM7_USART_WriteCountGet( void )
{
    return sercom7USARTObj.txProcessedSize;
}

void SERCOM7_USART_WriteCallbackRegister( SERCOM_USART_CALLBACK callback, uintptr_t context )
{
    sercom7USARTObj.txCallback = callback;

    sercom7USARTObj.txContext = context;
}


bool SERCOM7_USART_TransmitComplete( void )
{
    bool transmitComplete = false;

    if ((SERCOM7_REGS->USART_INT.SERCOM_INTFLAG & SERCOM_USART_INT_INTFLAG_TXC_Msk) == SERCOM_USART_INT_INTFLAG_TXC_Msk)
    {
        transmitComplete = true;
    }

    return transmitComplete;
}

void SERCOM7_USART_ReceiverEnable( void )
{
    SERCOM7_REGS->USART_INT.SERCOM_CTRLB |= SERCOM_USART_INT_CTRLB_RXEN_Msk;

    /* Wait for sync */
    while((SERCOM7_REGS->USART_INT.SERCOM_SYNCBUSY) != 0U)
    {
        /* Do nothing */
    }
}

void SERCOM7_USART_ReceiverDisable( void )
{
    SERCOM7_REGS->USART_INT.SERCOM_CTRLB &= ~SERCOM_USART_INT_CTRLB_RXEN_Msk;

    /* Wait for sync */
    while((SERCOM7_REGS->USART_INT.SERCOM_SYNCBUSY) != 0U)
    {
        /* Do nothing */
    }
}

bool SERCOM7_USART_Read( void *buffer, const size_t size )
{
    bool readStatus         = false;

    if(buffer != NULL)
    {
        if(sercom7USARTObj.rxBusyStatus == false)
        {
            /* Clear error flags and flush out error data that may have been received when no active request was pending */
            SERCOM7_USART_ErrorClear();

            sercom7USARTObj.rxBuffer = buffer;
            sercom7USARTObj.rxSize = size;
            sercom7USARTObj.rxProcessedSize = 0U;
            sercom7USARTObj.rxBusyStatus = true;
            sercom7USARTObj.errorStatus = USART_ERROR_NONE;

            readStatus = true;

            /* Enable receive and error interrupt */
            SERCOM7_REGS->USART_INT.SERCOM_INTENSET = (uint8_t)(SERCOM_USART_INT_INTENSET_ERROR_Msk | SERCOM_USART_INT_INTENSET_RXC_Msk);
        }
    }

    return readStatus;
}

bool SERCOM7_USART_ReadIsBusy( void )
{
    return sercom7USARTObj.rxBusyStatus;
}

size_t SERCOM7_USART_ReadCountGet( void )
{
    return sercom7USARTObj.rxProcessedSize;
}

bool SERCOM7_USART_ReadAbort(void)
{
    if (sercom7USARTObj.rxBusyStatus == true)
    {
        /* Disable receive and error interrupt */
        SERCOM7_REGS->USART_INT.SERCOM_INTENCLR = (uint8_t)(SERCOM_USART_INT_INTENCLR_ERROR_Msk | SERCOM_USART_INT_INTENCLR_RXC_Msk);

        sercom7USARTObj.rxBusyStatus = false;

        /* If required application should read the num bytes processed prior to calling the read abort API */
        sercom7USARTObj.rxSize = 0U;
        sercom7USARTObj.rxProcessedSize = 0U;
    }

    return true;
}

void SERCOM7_USART_ReadCallbackRegister( SERCOM_USART_CALLBACK callback, uintptr_t context )
{
    sercom7USARTObj.rxCallback = callback;

    sercom7USARTObj.rxContext = context;
}


void static __attribute__((used)) SERCOM7_USART_ISR_ERR_Handler( void )
{
    USART_ERROR errorStatus;

    errorStatus = (USART_ERROR) (SERCOM7_REGS->USART_INT.SERCOM_STATUS & (uint16_t)(SERCOM_USART_INT_STATUS_PERR_Msk | SERCOM_USART_INT_STATUS_FERR_Msk | SERCOM_USART_INT_STATUS_BUFOVF_Msk));

    if(errorStatus != USART_ERROR_NONE)
    {
        /* Save the error to be reported later */
        sercom7USARTObj.errorStatus = errorStatus;

        /* Clear the error flags and flush out the error bytes */
        SERCOM7_USART_ErrorClear();

        /* Disable error and receive interrupt to abort on-going transfer */
        SERCOM7_REGS->USART_INT.SERCOM_INTENCLR = (uint8_t)(SERCOM_USART_INT_INTENCLR_ERROR_Msk | SERCOM_USART_INT_INTENCLR_RXC_Msk);

        /* Clear the RX busy flag */
        sercom7USARTObj.rxBusyStatus = false;

        if(sercom7USARTObj.rxCallback != NULL)
        {
            uintptr_t rxContext = sercom7USARTObj.rxContext;

            sercom7USARTObj.rxCallback(rxContext);
        }
    }
}

void static __attribute__((used)) SERCOM7_USART_ISR_RX_Handler( void )
{
    uint16_t temp;


    if(sercom7USARTObj.rxBusyStatus == true)
    {
        size_t rxSize = sercom7USARTObj.rxSize;

        if(sercom7USARTObj.rxProcessedSize < rxSize)
        {
            uintptr_t rxContext = sercom7USARTObj.rxContext;

            temp = (uint16_t)SERCOM7_REGS->USART_INT.SERCOM_DATA;
            size_t rxProcessedSize = sercom7USARTObj.rxProcessedSize;

            if (((SERCOM7_REGS->USART_INT.SERCOM_CTRLB & SERCOM_USART_INT_CTRLB_CHSIZE_Msk) >> SERCOM_USART_INT_CTRLB_CHSIZE_Pos) != 0x01U)
            {
                /* 8-bit mode */
                ((uint8_t*)sercom7USARTObj.rxBuffer)[rxProcessedSize] = (uint8_t) (temp);
            }
            else
            {
                /* 9-bit mode */
                ((uint16_t*)sercom7USARTObj.rxBuffer)[rxProcessedSize] = temp;
            }

            /* Increment processed size */
            rxProcessedSize++;
            sercom7USARTObj.rxProcessedSize = rxProcessedSize;

            if(rxProcessedSize == sercom7USARTObj.rxSize)
            {
                sercom7USARTObj.rxBusyStatus = false;
                sercom7USARTObj.rxSize = 0U;
                SERCOM7_REGS->USART_INT.SERCOM_INTENCLR = (uint8_t)(SERCOM_USART_INT_INTENCLR_RXC_Msk | SERCOM_USART_INT_INTENCLR_ERROR_Msk);

                if(sercom7USARTObj.rxCallback != NULL)
                {
                    sercom7USARTObj.rxCallback(rxContext);
                }
            }

        }
    }
}

void static __attribute__((used)) SERCOM7_USART_ISR_TX_Handler( void )
{
    bool  dataRegisterEmpty;
    bool  dataAvailable;
    if(sercom7USARTObj.txBusyStatus == true)
    {
        size_t txProcessedSize = sercom7USARTObj.txProcessedSize;

        dataAvailable = (txProcessedSize < sercom7USARTObj.txSize);
        dataRegisterEmpty = ((SERCOM7_REGS->USART_INT.SERCOM_INTFLAG & SERCOM_USART_INT_INTFLAG_DRE_Msk) == SERCOM_USART_INT_INTFLAG_DRE_Msk);

        while(dataRegisterEmpty && dataAvailable)
        {
            if (((SERCOM7_REGS->USART_INT.SERCOM_CTRLB & SERCOM_USART_INT_CTRLB_CHSIZE_Msk) >> SERCOM_USART_INT_CTRLB_CHSIZE_Pos) != 0x01U)
            {
                /* 8-bit mode */
                SERCOM7_REGS->USART_INT.SERCOM_DATA = ((uint8_t*)sercom7USARTObj.txBuffer)[txProcessedSize];
            }
            else
            {
                /* 9-bit mode */
                SERCOM7_REGS->USART_INT.SERCOM_DATA = ((uint16_t*)sercom7USARTObj.txBuffer)[txProcessedSize];
            }
            /* Increment processed size */
            txProcessedSize++;

            dataAvailable = (txProcessedSize < sercom7USARTObj.txSize);
            dataRegisterEmpty = ((SERCOM7_REGS->USART_INT.SERCOM_INTFLAG & SERCOM_USART_INT_INTFLAG_DRE_Msk) == SERCOM_USART_INT_INTFLAG_DRE_Msk);
        }

        sercom7USARTObj.txProcessedSize = txProcessedSize;

        if(txProcessedSize >= sercom7USARTObj.txSize)
        {
            sercom7USARTObj.txBusyStatus = false;
            sercom7USARTObj.txSize = 0U;
            SERCOM7_REGS->USART_INT.SERCOM_INTENCLR = (uint8_t)SERCOM_USART_INT_INTENCLR_DRE_Msk;

            if(sercom7USARTObj.txCallback != NULL)
            {
                uintptr_t txContext = sercom7USARTObj.txContext;
                sercom7USARTObj.txCallback(txContext);
            }
        }
    }
}

void __attribute__((used)) SERCOM7_USART_InterruptHandler( void )
{
    bool testCondition;
    if(SERCOM7_REGS->USART_INT.SERCOM_INTENSET != 0U)
    {
        /* Checks for error flag */
        testCondition = ((SERCOM7_REGS->USART_INT.SERCOM_INTFLAG & SERCOM_USART_INT_INTFLAG_ERROR_Msk) == SERCOM_USART_INT_INTFLAG_ERROR_Msk);
        testCondition = ((SERCOM7_REGS->USART_INT.SERCOM_INTENSET & SERCOM_USART_INT_INTENSET_ERROR_Msk) == SERCOM_USART_INT_INTENSET_ERROR_Msk) && testCondition;
        if(testCondition)
        {
            SERCOM7_USART_ISR_ERR_Handler();
        }

        testCondition = ((SERCOM7_REGS->USART_INT.SERCOM_INTFLAG & SERCOM_USART_INT_INTFLAG_DRE_Msk) == SERCOM_USART_INT_INTFLAG_DRE_Msk);
        testCondition = ((SERCOM7_REGS->USART_INT.SERCOM_INTENSET & SERCOM_USART_INT_INTENSET_DRE_Msk) == SERCOM_USART_INT_INTENSET_DRE_Msk) && testCondition;
        /* Checks for data register empty flag */
        if(testCondition)
        {
            SERCOM7_USART_ISR_TX_Handler();
        }

        testCondition = ((SERCOM7_REGS->USART_INT.SERCOM_INTFLAG & SERCOM_USART_INT_INTFLAG_RXC_Msk) == SERCOM_USART_INT_INTFLAG_RXC_Msk);
        testCondition = ((SERCOM7_REGS->USART_INT.SERCOM_INTENSET & SERCOM_USART_INT_INTENSET_RXC_Msk) == SERCOM_USART_INT_INTENSET_RXC_Msk) && testCondition;
        /* Checks for receive complete empty flag */
        if(testCondition)
        {
            SERCOM7_USART_ISR_RX_Handler();
        }
    }
}
