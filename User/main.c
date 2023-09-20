/******************************************************************************
 * @file     main.c
 * @version  V1.00
 * $Revision: 1 $
 * $Date: 14/12/16 9:51a $
 * @brief    Software Development Template.
 * @note
 * Copyright (C) 2014 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>
#include "NUC100Series.h"
#include "keyboard.h"
#include "midi_device.h"
#include "sys_timer.h"
#include "main_app.h"
#include "input.h"
#include "analog.h"
#include "keypad.h"
#include "led.h"


void SYS_Init(void)
{
	/* Enable Internal RC 22.1184 MHz clock */
	CLK_EnableXtalRC(CLK_PWRCON_OSC22M_EN_Msk);

	/* Waiting for Internal RC clock ready */
	CLK_WaitClockReady(CLK_CLKSTATUS_OSC22M_STB_Msk);

	/* Switch HCLK clock source to Internal RC and HCLK source divide 1 */
	CLK_SetHCLK(CLK_CLKSEL0_HCLK_S_HIRC, CLK_CLKDIV_HCLK(1));

	/* Enable external XTAL 12 MHz clock */
	CLK_EnableXtalRC(CLK_PWRCON_XTL12M_EN_Msk);

	/* Waiting for external XTAL clock ready */
	CLK_WaitClockReady(CLK_CLKSTATUS_XTL12M_STB_Msk);

	/* Set core clock */
	CLK_SetCoreClock(48000000);

	/* Enable module clock */
	CLK_EnableModuleClock(USBD_MODULE);

	/* Select module clock source */
	CLK_SetModuleClock(USBD_MODULE, 0, CLK_CLKDIV_USB(1));
}

int main()
{
    SYS_UnlockReg();
    SYS_Init();

    sys_timer_init();

    USBD_Open(&gsInfo, NULL, NULL);
    midi_device_init();
    USBD_Start();
    NVIC_EnableIRQ(USBD_IRQn);

    keyboard_init();
    input_init();
    analog_init();
    keypad_init();
    led_init();

    // never return...
    main_app();
}
