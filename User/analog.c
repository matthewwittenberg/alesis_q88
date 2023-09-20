/*
 * adc.c
 *
 *  Created on: Sep 20, 2023
 *      Author: matt
 */

#include "analog.h"
#include "NUC100Series.h"
#include "adc.h"

void analog_init()
{
	CLK_EnableModuleClock(ADC_MODULE);
	CLK_SetModuleClock(ADC_MODULE, CLK_CLKSEL1_ADC_S_HIRC, CLK_CLKDIV_ADC(7));

	/* Disable the GPA0 - GPA3 digital input path to avoid the leakage current. */
	GPIO_DISABLE_DIGITAL_PATH(PA, 0xF);

	/* Configure the GPA0 - GPA3 ADC analog input pins */
	SYS->GPA_MFP &= ~(SYS_GPA_MFP_PA0_Msk | SYS_GPA_MFP_PA1_Msk | SYS_GPA_MFP_PA2_Msk | SYS_GPA_MFP_PA3_Msk) ;
	SYS->GPA_MFP |= SYS_GPA_MFP_PA0_ADC0 | SYS_GPA_MFP_PA1_ADC1 | SYS_GPA_MFP_PA2_ADC2 | SYS_GPA_MFP_PA3_ADC3 ;

	ADC_POWER_ON(ADC);
	ADC_Open(ADC, ADC_ADCR_DIFFEN_SINGLE_END, ADC_ADCR_ADMD_CONTINUOUS, 0xF);
}

void analog_get(int16_t *pmodulation, int16_t *ppitch, int16_t *pvolume, int16_t *pexpression)
{
	ADC_CLR_INT_FLAG(ADC, ADC_ADF_INT);
	ADC_START_CONV(ADC);
	while(!ADC_GET_INT_FLAG(ADC, ADC_ADF_INT));
	ADC_CLR_INT_FLAG(ADC, ADC_ADF_INT);

	*pvolume = ADC_GET_CONVERSION_DATA(ADC, 0);
	*ppitch = ADC_GET_CONVERSION_DATA(ADC, 1);
	*pmodulation = ADC_GET_CONVERSION_DATA(ADC, 2);
	*pexpression = ADC_GET_CONVERSION_DATA(ADC, 3);

	ADC_STOP_CONV(ADC);
}
