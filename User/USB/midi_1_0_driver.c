/******************************************************************************
 * @file     hid_mouse.c
 * @brief    NUC100 series USBD HID mouse sample file
 *
 * @note
 * Copyright (C) 2013 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/

/*!<Includes */
#include <midi_1_0_driver.h>
#include <string.h>
#include "NUC100Series.h"
#include "usbd.h"

uint8_t volatile g_u8EP2Ready = 0;
uint8_t volatile g_u8Suspend = 0;
uint8_t g_u8Idle = 0;
uint8_t g_u8Protocol = 0;

void EP2_Handler(void);

void USBD_IRQHandler(void)
{
    uint32_t u32IntSts = USBD_GET_INT_FLAG();
    uint32_t u32State = USBD_GET_BUS_STATE();

//------------------------------------------------------------------
    if(u32IntSts & USBD_INTSTS_FLDET_STS_Msk)
    {
        // Floating detect
        USBD_CLR_INT_FLAG(USBD_INTSTS_FLDET_STS_Msk);

        if(USBD_IS_ATTACHED())
        {
            /* USB Plug In */
            USBD_ENABLE_USB();
        }
        else
        {
            /* USB Un-plug */
            USBD_DISABLE_USB();
        }
    }

//------------------------------------------------------------------
    if(u32IntSts & USBD_INTSTS_WAKEUP)
    {
        /* Clear event flag */
        USBD_CLR_INT_FLAG(USBD_INTSTS_WAKEUP);
    }

//------------------------------------------------------------------
    if(u32IntSts & USBD_INTSTS_BUS_STS_Msk)
    {
        /* Clear event flag */
        USBD_CLR_INT_FLAG(USBD_INTSTS_BUS_STS_Msk);

        if(u32State & USBD_ATTR_USBRST_Msk)
        {
            /* Bus reset */
            USBD_ENABLE_USB();
            USBD_SwReset();
            g_u8Suspend = 0;
        }
        if(u32State & USBD_ATTR_SUSPEND_Msk)
        {
            /* Enter power down to wait USB attached */
            g_u8Suspend = 1;

            /* Enable USB but disable PHY */
            USBD_DISABLE_PHY();
        }
        if(u32State & USBD_ATTR_RESUME_Msk)
        {
            /* Enable USB and enable PHY */
            USBD_ENABLE_USB();
            g_u8Suspend = 0;
        }
    }

//------------------------------------------------------------------
    if(u32IntSts & USBD_INTSTS_USB_STS_Msk)
    {
        // USB event
        if(u32IntSts & USBD_INTSTS_SETUP_Msk)
        {
            // Setup packet
            /* Clear event flag */
            USBD_CLR_INT_FLAG(USBD_INTSTS_SETUP_Msk);

            /* Clear the data IN/OUT ready flag of control end-points */
            USBD_STOP_TRANSACTION(EP0);
            USBD_STOP_TRANSACTION(EP1);

            USBD_ProcessSetupPacket();
        }

        // EP events
        if(u32IntSts & USBD_INTSTS_EP0)
        {
            /* Clear event flag */
            USBD_CLR_INT_FLAG(USBD_INTSTS_EP0);
            // control IN
            USBD_CtrlIn();
        }

        if(u32IntSts & USBD_INTSTS_EP1)
        {
            /* Clear event flag */
            USBD_CLR_INT_FLAG(USBD_INTSTS_EP1);

            // control OUT
            USBD_CtrlOut();
        }

        if(u32IntSts & USBD_INTSTS_EP2)
        {
            /* Clear event flag */
            USBD_CLR_INT_FLAG(USBD_INTSTS_EP2);
            // Interrupt IN
            EP2_Handler();
        }

        if(u32IntSts & USBD_INTSTS_EP3)
        {
            /* Clear event flag */
            USBD_CLR_INT_FLAG(USBD_INTSTS_EP3);
        }

        if(u32IntSts & USBD_INTSTS_EP4)
        {
            /* Clear event flag */
            USBD_CLR_INT_FLAG(USBD_INTSTS_EP4);
        }

        if(u32IntSts & USBD_INTSTS_EP5)
        {
            /* Clear event flag */
            USBD_CLR_INT_FLAG(USBD_INTSTS_EP5);
        }
    }
}

void EP2_Handler(void)  /* Interrupt IN handler */
{
    g_u8EP2Ready = 1;
}


/*--------------------------------------------------------------------------*/
/**
  * @brief  USBD Endpoint Config.
  * @param  None.
  * @retval None.
  */
void midi_1_0_driver_init(void)
{
    /* Init setup packet buffer */
    /* Buffer range for setup packet -> [0 ~ 0x7] */
    USBD->STBUFSEG = SETUP_BUF_BASE;

    /*****************************************************/
    /* EP0 ==> control IN endpoint, address 0 */
    USBD_CONFIG_EP(EP0, USBD_CFG_CSTALL | USBD_CFG_EPMODE_IN | 0);
    /* Buffer range for EP0 */
    USBD_SET_EP_BUF_ADDR(EP0, EP0_BUF_BASE);

    /* EP1 ==> control OUT endpoint, address 0 */
    USBD_CONFIG_EP(EP1, USBD_CFG_CSTALL | USBD_CFG_EPMODE_OUT | 0);
    /* Buffer range for EP1 */
    USBD_SET_EP_BUF_ADDR(EP1, EP1_BUF_BASE);

    /*****************************************************/
    /* EP2 ==> Interrupt IN endpoint, address 1 */
    USBD_CONFIG_EP(EP2, USBD_CFG_EPMODE_IN | USBD_MIDI10_EP_IN_ADDR);
    /* Buffer range for EP2 */
    USBD_SET_EP_BUF_ADDR(EP2, EP2_BUF_BASE);

    /* EP3 ==> Interrupt OUT endpoint, address 2 */
	USBD_CONFIG_EP(EP3, USBD_CFG_EPMODE_OUT | USBD_MIDI10_EP_OUT_ADDR);
	/* Buffer range for EP3 */
	USBD_SET_EP_BUF_ADDR(EP3, EP3_BUF_BASE);
	/* trigger to receive OUT data */
	USBD_SET_PAYLOAD_LEN(EP3, EP3_MAX_PKT_SIZE);

    /* start to IN data */
    g_u8EP2Ready = 1;
}

//void midi_1_0_class_request(void)
//{
//    uint8_t buf[8];
//
//    USBD_GetSetupPacket(buf);
//
//    if(buf[0] & 0x80)    /* request data transfer direction */
//    {
////        // Device to host
////        switch(buf[1])
////        {
////            case GET_REPORT:
//////            {
//////                break;
//////            }
////            case GET_IDLE:
////            {
////                USBD_SET_PAYLOAD_LEN(EP1, buf[6]);
////                /* Data stage */
////                USBD_PrepareCtrlIn(&g_u8Idle, buf[6]);
////                /* Status stage */
////                USBD_PrepareCtrlOut(0, 0);
////                break;
////            }
////            case GET_PROTOCOL:
////            {
////                USBD_SET_PAYLOAD_LEN(EP1, buf[6]);
////                /* Data stage */
////                USBD_PrepareCtrlIn(&g_u8Protocol, buf[6]);
////                /* Status stage */
////                USBD_PrepareCtrlOut(0, 0);
////                break;
////            }
////            default:
////            {
//                /* Setup error, stall the device */
//                USBD_SetStall(EP0);
//                USBD_SetStall(EP1);
////                break;
////            }
////        }
//    }
//    else
//    {
//        // Host to device
////        switch(buf[1])
////        {
////            case SET_REPORT:
////            {
////                if(buf[3] == 3)
////                {
////                    /* Request Type = Feature */
////                    USBD_SET_DATA1(EP1);
////                    USBD_SET_PAYLOAD_LEN(EP1, 0);
////                }
////                break;
////            }
////            case SET_IDLE:
////            {
////                g_u8Idle = buf[3];
////                /* Status stage */
////                USBD_SET_DATA1(EP0);
////                USBD_SET_PAYLOAD_LEN(EP0, 0);
////                break;
////            }
////            case SET_PROTOCOL:
////            {
////                g_u8Protocol = buf[2];
////                /* Status stage */
////                USBD_SET_DATA1(EP0);
////                USBD_SET_PAYLOAD_LEN(EP0, 0);
////                break;
////            }
////            default:
////            {
//                // Stall
//                /* Setup error, stall the device */
//                USBD_SetStall(EP0);
//                USBD_SetStall(EP1);
////                break;
////            }
////        }
//    }
//}

#define MIDI_1_0_TX_QUEUE_DEPTH 10

uint32_t _midi_1_0_tx_queue[MIDI_1_0_TX_QUEUE_DEPTH];
uint32_t _midi_1_0_tx_queue_head = 0;
uint32_t _midi_1_0_tx_queue_tail = 0;

void midi_1_0_driver_tx(uint32_t message)
{
	_midi_1_0_tx_queue[_midi_1_0_tx_queue_head] = message;
	_midi_1_0_tx_queue_head++;
	if(_midi_1_0_tx_queue_head >= MIDI_1_0_TX_QUEUE_DEPTH)
		_midi_1_0_tx_queue_head = 0;
}

void midi_1_0_driver_task()
{
    uint8_t *buf;

    if(g_u8EP2Ready)
    {
    	if(_midi_1_0_tx_queue_tail != _midi_1_0_tx_queue_head)
    	{
    		buf = (uint8_t *)(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP2));

    		uint32_t message = _midi_1_0_tx_queue[_midi_1_0_tx_queue_tail];
    		_midi_1_0_tx_queue_tail++;
    		if(_midi_1_0_tx_queue_tail >= MIDI_1_0_TX_QUEUE_DEPTH)
    			_midi_1_0_tx_queue_tail = 0;

    		memcpy(buf, &message, 4);

    		g_u8EP2Ready = 0;

    		/* Set transfer length and trigger IN transfer */
    		USBD_SET_PAYLOAD_LEN(EP2, 4);
    	}
    }
}


