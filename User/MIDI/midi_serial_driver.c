/*
 * midi_1_0_serial_driver.c
 *
 *  Created on: Sep 22, 2023
 *      Author: matt
 */

#include "NUC100Series.h"
#include "midi_serial_driver.h"
#include "uart.h"

#define MIDI_UART UART1

#define MIDI_SERIAL_TX_QUEUE_DEPTH 64

uint8_t _midi_serial_tx_queue[MIDI_SERIAL_TX_QUEUE_DEPTH];
uint32_t _midi_serial_tx_queue_head = 0;
uint32_t _midi_serial_tx_queue_tail = 0;

void UART1_IRQHandler(void)
{
	// tx empty
	if(UART_GET_INT_FLAG(MIDI_UART, UART_ISR_THRE_IF_Msk))
	{
		if(_midi_serial_tx_queue_head != _midi_serial_tx_queue_tail)
		{
			MIDI_UART->THR = _midi_serial_tx_queue[_midi_serial_tx_queue_tail];
			_midi_serial_tx_queue_tail++;
			if(_midi_serial_tx_queue_tail >= MIDI_SERIAL_TX_QUEUE_DEPTH)
				_midi_serial_tx_queue_tail = 0;
		}
		else
		{
			UART_DisableInt(MIDI_UART, UART_IER_THRE_IEN_Msk);
		}

		UART_ClearIntFlag(MIDI_UART, UART_ISR_THRE_IF_Msk);
	}
}

void midi_serial_driver_init()
{
	CLK_EnableModuleClock(UART1_MODULE);
	CLK_SetModuleClock(UART1_MODULE, CLK_CLKSEL1_UART_S_HXT, CLK_CLKDIV_UART(1));
	SYS->GPB_MFP &= ~(SYS_GPB_MFP_PB5_Msk);
    SYS->GPB_MFP |= (SYS_GPB_MFP_PB5_UART1_TXD);

    UART_Open(MIDI_UART, 31250);
}

void midi_serial_driver_tx(uint8_t *pmessage, uint32_t length)
{
	for(uint32_t i=0; i<length; i++)
	{
		_midi_serial_tx_queue[_midi_serial_tx_queue_head] = pmessage[i];
		_midi_serial_tx_queue_head++;
		if(_midi_serial_tx_queue_head >= MIDI_SERIAL_TX_QUEUE_DEPTH)
			_midi_serial_tx_queue_head = 0;
	}

	UART_EnableInt(MIDI_UART, UART_IER_THRE_IEN_Msk);
}
