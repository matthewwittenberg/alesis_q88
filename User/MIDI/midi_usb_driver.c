#include "midi_usb_driver.h"
#include <string.h>
#include <stdbool.h>
#include "NUC100Series.h"
#include "usbd.h"

#define MIDI_TX_QUEUE_DEPTH 512
#define MIDI_RX_QUEUE_DEPTH 512

uint8_t _midi_tx_queue[MIDI_TX_QUEUE_DEPTH];
uint32_t _midi_tx_queue_head = 0;
uint32_t _midi_tx_queue_tail = 0;

uint8_t _midi_rx_queue[MIDI_RX_QUEUE_DEPTH];
uint32_t _midi_rx_queue_head = 0;
uint32_t _midi_rx_queue_tail = 0;

uint8_t volatile g_u8EP2Ready = 0;
uint8_t volatile g_u8Suspend = 0;
uint8_t g_u8Idle = 0;
uint8_t g_u8Protocol = 0;

void EP2_Handler(void);
void EP3_Handler(void);

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

            EP3_Handler();
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

// in handler
void EP2_Handler(void)
{
    g_u8EP2Ready = 1;
}

// out handler
void EP3_Handler(void)
{
	uint8_t *pdata = (uint8_t*)(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP3));
	uint32_t length = USBD_GET_PAYLOAD_LEN(EP3);

	for(uint32_t i=0; i<length; i++)
	{
		_midi_rx_queue[_midi_rx_queue_head] = pdata[i];
		_midi_rx_queue_head++;
		if(_midi_rx_queue_head >= MIDI_RX_QUEUE_DEPTH)
			_midi_rx_queue_head = 0;
	}

	USBD_SET_PAYLOAD_LEN(EP3, EP3_MAX_PKT_SIZE);
}

#pragma pack(push, 1)
typedef struct
{
	uint8_t   bmRequest;
	uint8_t   bRequest;
	uint16_t  wValue;
	uint16_t  wIndex;
	uint16_t  wLength;
} USBD_SETUP_PACKET_T;
#pragma pack(pop)

extern const uint8_t USBD_MIDI20_TERM_BLOCK_DESC[18];

void midi_usb_class_request()
{
	USBD_SETUP_PACKET_T setup;
	bool handled = false;

	USBD_GetSetupPacket((uint8_t*)&setup);

	switch (setup.bmRequest & (REQ_CLASS | REQ_VENDOR))
	{
		case REQ_CLASS:
			break;

		case REQ_VENDOR:
			break;

		case REQ_STANDARD:
			if(setup.bRequest == GET_DESCRIPTOR)
			{
#if MIDI_VERSION == 2
				if(setup.wValue == 0x2601)
				{
					USBD_PrepareCtrlIn(USBD_MIDI20_TERM_BLOCK_DESC, sizeof(USBD_MIDI20_TERM_BLOCK_DESC));
					USBD_PrepareCtrlOut(0, 0);
					handled = true;
				}
#endif
			}

			break;

		default:
			break;
	}

	if(handled == false)
	{
		USBD_SetStall(EP0);
		USBD_SetStall(EP1);
	}
}


/*--------------------------------------------------------------------------*/
/**
  * @brief  USBD Endpoint Config.
  * @param  None.
  * @retval None.
  */
void midi_usb_driver_init(void)
{
	USBD_Open(&gsInfo, midi_usb_class_request, NULL);
	USBD_Start();
	NVIC_EnableIRQ(USBD_IRQn);

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
    USBD_CONFIG_EP(EP2, USBD_CFG_EPMODE_IN | USBD_MIDI_EP_IN_ADDR);
    /* Buffer range for EP2 */
    USBD_SET_EP_BUF_ADDR(EP2, EP2_BUF_BASE);

    /* EP3 ==> Interrupt OUT endpoint, address 2 */
	USBD_CONFIG_EP(EP3, USBD_CFG_EPMODE_OUT | USBD_MIDI_EP_OUT_ADDR);
	/* Buffer range for EP3 */
	USBD_SET_EP_BUF_ADDR(EP3, EP3_BUF_BASE);
	/* trigger to receive OUT data */
	USBD_SET_PAYLOAD_LEN(EP3, EP3_MAX_PKT_SIZE);

    /* start to IN data */
    g_u8EP2Ready = 1;
}

void midi_usb_driver_tx(uint8_t *pmessage, uint32_t length)
{
	uint32_t head = _midi_tx_queue_head;

	for(uint32_t i=0; i<length; i++)
	{
		_midi_tx_queue[head] = pmessage[i];
		head++;
		if(head >= MIDI_TX_QUEUE_DEPTH)
			head = 0;
	}

	_midi_tx_queue_head = head;
}

uint32_t midi_usb_driver_rx(uint8_t *pmessage, uint32_t length)
{
	uint32_t bytes_read = 0;

	while(_midi_rx_queue_tail != _midi_rx_queue_head)
	{
		pmessage[bytes_read++] = _midi_rx_queue[_midi_rx_queue_tail];
		_midi_rx_queue_tail++;
		if(_midi_rx_queue_tail >= MIDI_RX_QUEUE_DEPTH)
			_midi_rx_queue_tail = 0;

		if(bytes_read >= length)
			break;
	}

	return bytes_read;
}

void midi_usb_driver_task()
{
    uint8_t *buf;

    if(g_u8EP2Ready)
    {
    	if(_midi_tx_queue_tail != _midi_tx_queue_head)
    	{
    		uint32_t index = 0;
    		buf = (uint8_t *)(USBD_BUF_BASE + USBD_GET_EP_BUF_ADDR(EP2));

    		while(_midi_tx_queue_tail != _midi_tx_queue_head)
			{
    			buf[index++] = _midi_tx_queue[_midi_tx_queue_tail];
    			_midi_tx_queue_tail++;
				if(_midi_tx_queue_tail >= MIDI_TX_QUEUE_DEPTH)
					_midi_tx_queue_tail = 0;

				if(index >= EP2_MAX_PKT_SIZE)
					break;
			}

    		g_u8EP2Ready = 0;

    		/* Set transfer length and trigger IN transfer */
    		USBD_SET_PAYLOAD_LEN(EP2, index);
    	}
    }
}

void midi_usb_flush_rx()
{
	_midi_rx_queue_head = 0;
	_midi_rx_queue_tail = 0;
}
