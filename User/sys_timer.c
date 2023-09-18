/*
 * timer.c
 *
 *  Created on: Sep 18, 2023
 *      Author: matt
 */

#include "sys_timer.h"
#include "NUC100Series.h"

#define SYS_TIMER TIMER0
#define SYS_TIMER_MODULE TMR0_MODULE

volatile uint32_t _ms = 0;

void TMR0_IRQHandler(void)
{
	TIMER_ClearIntFlag(SYS_TIMER);
	++_ms;
}

void sys_timer_init()
{
	CLK_EnableModuleClock(SYS_TIMER_MODULE);
	CLK_SetModuleClock(SYS_TIMER_MODULE, CLK_CLKSEL1_TMR0_S_HIRC, 0);

	TIMER_Open(SYS_TIMER, TIMER_PERIODIC_MODE, 1000);
	TIMER_EnableInt(SYS_TIMER);
	NVIC_EnableIRQ(TMR0_IRQn);
	TIMER_Start(SYS_TIMER);
}

uint32_t get_ms()
{
	return _ms;
}

void wait_ms(uint32_t ms)
{
	uint32_t new_ms = _ms + ms;
	while(new_ms > _ms);
}
