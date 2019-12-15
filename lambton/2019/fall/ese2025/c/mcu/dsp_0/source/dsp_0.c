/*
===============================================================================
 Name        : dsp_0.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

#include <cr_section_macros.h>

// TODO: insert other include files here

// TODO: insert other definitions and declarations here
/* DAC sample rate request time */
#define DAC_TIMEOUT 0x3FF
#define DAC_TICKRATE_HZ (50000)	/* 50000 ticks per second */
#define _ADC_CHANNEL ADC_CH0
#define _LPC_ADC_ID LPC_ADC
#define _LPC_ADC_IRQ ADC_IRQn

volatile uint32_t dacval = 0; /* storage for DAC data register */
volatile uint16_t adcval; /* storage for ADC data reads */

static ADC_CLOCK_SETUP_T ADCSetup;
static volatile uint8_t Burst_Mode_Flag = 0, Interrupt_Continue_Flag;
static volatile uint8_t ADC_Interrupt_Done_Flag, channelTC, dmaChannelNum;
uint32_t DMAbuffer;

/**
 * @brief	do a DAC read using the interrupt handler for the SysTick timer
 * @return	Nothing
 */
void SysTick_Handler(void)
{
	Chip_DAC_UpdateValue(LPC_DAC, dacval);
}

/**
 * @brief	ADC0 interrupt handler sub-routine
 * @return	Nothing
 */
void ADC_IRQHandler(void)
{

	/* disable interrupt to process it --- no nested interrupt calls allowed */
	NVIC_DisableIRQ(_LPC_ADC_IRQ);
	Chip_ADC_Int_SetChannelCmd(_LPC_ADC_ID, _ADC_CHANNEL, DISABLE);

	/* read ADC data register */
	Chip_ADC_ReadValue(_LPC_ADC_ID, _ADC_CHANNEL, &adcval);

	/* reinstate interrupt to continue */
	NVIC_EnableIRQ(_LPC_ADC_IRQ);
	Chip_ADC_Int_SetChannelCmd(_LPC_ADC_ID, _ADC_CHANNEL, ENABLE);
}

int main(void)
{
	SystemCoreClockUpdate();
	Board_Init();

	/***************     DAC Initialization      **********/
	{
		/* Setup DAC pins for board and common CHIP code */
		Chip_DAC_Init(LPC_DAC);

		/* Setup DAC timeout for polled and DMA modes to 0x3FF */
		/* 175x/6x devices have a DAC divider, set it to 1 */
		Chip_Clock_SetPCLKDiv(SYSCTL_PCLK_DAC, SYSCTL_CLKDIV_1);
		Chip_DAC_SetDMATimeOut(LPC_DAC, DAC_TIMEOUT);

		/* Compute and show estimated DAC request time */
		dacClk = Chip_Clock_GetPeripheralClockRate(SYSCTL_PCLK_DAC);

		/* DEBUGOUT("DAC base clock rate = %dHz, DAC request rate = %dHz\r\n",
			dacClk, (dacClk / DAC_TIMEOUT)); */
	}

	/***************     ADC Initialization      **********/
	{
		Chip_ADC_Init(_LPC_ADC_ID, &ADCSetup);
		Chip_ADC_EnableChannel(_LPC_ADC_ID, _ADC_CHANNEL, ENABLE);
		NVIC_EnableIRQ(_LPC_ADC_IRQ); /* Enable ADC Interrupt */
		Chip_ADC_Int_SetChannelCmd(_LPC_ADC_ID, _ADC_CHANNLE, ENABLE);
		Chip_ADC_SetBurstCmd(_LPC_ADC_ID, ENABLE); /* enable Burst Mode for regular conversions */
	}

	/***************      SysTick Timer (for DAC) Initialization      ***************/
	{
		SysTick_Config(SystemCoreClock / DAC_TICKRATE_HZ);
	}


	while (!end_Flag)
	{
	}

	return 0;
}
