#ifndef __USBD_HID_H__
#define __USBD_HID_H__

#include <stdint.h>
#include <stdbool.h>

/* Define the vendor id and product id */
#define USBD_VID        0x13B2
#define USBD_PID        0x0001

/*-------------------------------------------------------------*/
/* Define EP maximum packet size */
#define EP0_MAX_PKT_SIZE    8
#define EP1_MAX_PKT_SIZE    EP0_MAX_PKT_SIZE
#define EP2_MAX_PKT_SIZE    64
#define EP3_MAX_PKT_SIZE    64

#define SETUP_BUF_BASE  0
#define SETUP_BUF_LEN   8
#define EP0_BUF_BASE    (SETUP_BUF_BASE + SETUP_BUF_LEN)
#define EP0_BUF_LEN     EP0_MAX_PKT_SIZE
#define EP1_BUF_BASE    (SETUP_BUF_BASE + SETUP_BUF_LEN)
#define EP1_BUF_LEN     EP1_MAX_PKT_SIZE
#define EP2_BUF_BASE    (EP1_BUF_BASE + EP1_BUF_LEN)
#define EP2_BUF_LEN     EP2_MAX_PKT_SIZE
#define EP3_BUF_BASE    (EP2_BUF_BASE + EP2_BUF_LEN)
#define EP3_BUF_LEN     EP3_MAX_PKT_SIZE

/* endpoints */
#define USBD_MIDI_EP_IN_ADDR 0x01
#define USBD_MIDI_EP_OUT_ADDR 0x02

/* Define Descriptor information */
#define HID_DEFAULT_INT_IN_INTERVAL     20
#define USBD_SELF_POWERED               0
#define USBD_REMOTE_WAKEUP              0
#define USBD_MAX_POWER                  250  /* The unit is in 2mA. ex: 250 * 2mA = 500mA */

#define LEN_CONFIG_AND_SUBORDINATE      (LEN_CONFIG+LEN_INTERFACE+LEN_HID+LEN_ENDPOINT)

/*-------------------------------------------------------------*/

/*-------------------------------------------------------------*/
void midi_usb_driver_init();
void midi_usb_driver_tx(uint8_t *pmessage, uint32_t length);
uint32_t midi_usb_driver_rx(uint8_t *pmessage, uint32_t length);
void midi_usb_flush_rx();
void midi_usb_driver_task();

extern uint8_t volatile g_u8Suspend;
extern bool IS_MIDI_2_0;

#endif
