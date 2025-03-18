#ifndef _NTC_H    /* Guard against multiple inclusion */
#define _NTC_H

#define ADC_VREF                (3.3f)

// NTC temperature inputs
#define NTC_HEATING_BUFFER      0
#define NTC_HOT_WATER_BUFFER    1 
#define NTC_RESERVED_1          2
#define NTC_RESERVED_2          3 

extern uint32_t  adc_seq_regs[4];
extern volatile uint16_t     adc_res[4];
extern volatile bool         adc_dma_done;
extern float input_voltage;
extern int16_t TempNtc[4];


void adc_sram_dma_callback(DMAC_TRANSFER_EVENT event, uintptr_t contextHandle);
int16_t Temp_ntc_10k (int16_t AdcWaarde, int8_t offset);
void AdcVerwerk(void);
int16_t GetNtcTemperature(uint8_t whichSensor);

#endif 