/******************************************************************************
 * @file     descriptors.c
 * @brief    NUC100 series USBD descriptor
 *
 * @note
 * Copyright (C) 2013 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
/*!<Includes */
#include "midi_usb_driver.h"
#include "NUC100Series.h"
#include "usbd.h"

/*----------------------------------------------------------------------------*/
/*!<USB Device Descriptor */
uint8_t gu8DeviceDescriptor[] =
{
    LEN_DEVICE,     /* bLength */
    DESC_DEVICE,    /* bDescriptorType */
    0x10, 0x01,     /* bcdUSB */
    0x00,           /* bDeviceClass */
    0x00,           /* bDeviceSubClass */
    0x00,           /* bDeviceProtocol */
    EP0_MAX_PKT_SIZE,   /* bMaxPacketSize0 */
    /* idVendor */
    USBD_VID & 0x00FF,
    (USBD_VID & 0xFF00) >> 8,
    /* idProduct */
    USBD_PID & 0x00FF,
    (USBD_PID & 0xFF00) >> 8,
    0x00, 0x00,     /* bcdDevice */
    0x01,           /* iManufacture */
    0x02,           /* iProduct */
    0x03,           /* iSerialNumber */
    0x01            /* bNumConfigurations */
};

#if MIDI_VERSION == 1

/*!<USB Configure Descriptor */
#define USBD_MIDI10_CONFIG_DESC_SIZE 97
const uint8_t gu8ConfigDescriptor[] =
{
	0x09,                               /* bLength */
	DESC_CONFIG,        /* bDescriptorType */
	USBD_MIDI10_CONFIG_DESC_SIZE,       /* wTotalLength */
	0x00,
	0x02,                               /* bNumInterfaces */
	0x01,                               /* bConfigurationValue */
	0x00,                               /* iConfiguration */
	0x80,                               /* bmAttributes */
	0xFA,                               /* bMaxPower */

	/* AUDIO CONTROL INTERFACE */
	0x09,                               /* bLength */
	DESC_INTERFACE,            /* bDescriptorType */
	0x00,                               /* bInterfaceNumber */
	0x00,                               /* bAlternateSetting */
	0x00,                               /* bNumEndpoints */
	0x01,                               /* bInterfaceClass */
	0x01,                               /* bInterfaceSubClass */
	0x00,                               /* bInterfaceProtocol */
	0x00,                               /* iInterface */

	/* AUDIO CONTROL INTERFACE HEADER */
	0x09,                               /* bLength */
	0x24,                               /* bDescriptorType */
	0x01,                               /* bDescriptorSubtype */
	0x00,                               /* bcdADC */
	0x01,
	0x09,                               /* wTotalLength */
	0x00,
	0x01,                               /* bInCollection */
	0x01,                               /* bInterfaceNr */

	/* MIDI STREAM INTERFACE */
	0x09,                               /* bLength */
	DESC_INTERFACE,            /* bDescriptorType */
	0x01,                               /* bInterfaceNumber */
	0x00,                               /* bAlternateSetting */
	0x02,                               /* bNumEndpoints */
	0x01,                               /* bInterfaceClass */
	0x03,                               /* bInterfaceSubClass */
	0x00,                               /* bInterfaceProtocol */
	0x00,                               /* iInterface */

	/* MIDI STREAM INTERFACE HEADER */
	0x07,                               /* bLength */
	0x24,                               /* bDescriptorType */
	0x01,                               /* bDescriptorSubtype */
	0x00,                               /* bcdMSC */
	0x01,
	54,                                 /* wTotalLength */
	0x00,

	/* MIDI IN JACK */
	0x06,                               /* bLength */
	0x24,                               /* bDescriptorType */
	0x02,                               /* bDescriptorSubtype */
	0x01,                               /* bJackType */
	0x01,                               /* bJackID */
	0x00,                               /* iJack */

	/* MIDI IN JACK */
	0x06,                               /* bLength */
	0x24,                               /* bDescriptorType */
	0x02,                               /* bDescriptorSubtype */
	0x02,                               /* bJackType */
	0x02,                               /* bJackID */
	0x00,                               /* iJack */

	/* MIDI OUT JACK */
	0x09,                               /* bLength */
	0x24,                               /* bDescriptorType */
	0x03,                               /* bDescriptorSubtype */
	0x01,                               /* bJackType */
	0x03,                               /* bJackID */
	0x01,                               /* bNrInputPins */
	0x02,                               /* baSourceID1 */
	0x01,                               /* BaSourcePin1 */
	0x00,                               /* iJack */

	/* MIDI OUT JACK */
	0x09,                               /* bLength */
	0x24,                               /* bDescriptorType */
	0x03,                               /* bDescriptorSubtype */
	0x02,                               /* bJackType */
	0x04,                               /* bJackID */
	0x01,                               /* bNrInputPins */
	0x01,                               /* baSourceID1 */
	0x01,                               /* BaSourcePin1 */
	0x00,                               /* iJack */

	/* OUT ENDPOINT */
	0x07,                               /* bLength */
	DESC_ENDPOINT,             /* bDescriptorType */
	USBD_MIDI_EP_OUT_ADDR,            /* bEndpointAddress */
	0x02,                               /* bmAttributes */
	EP3_MAX_PKT_SIZE,     /* wMaxPacketSize */
	0x00,
	0x00,                               /* bInterval */

	/* OUT ENDPOINT MS */
	0x05,                               /* bLength */
	0x25,                               /* bDescriptorType */
	0x01,                               /* bDescriptorSubtype */
	0x01,                               /* bNumEmbMIDIJack */
	0x01,                               /* baAssocJackID */

	/* IN ENDPOINT */
	0x07,                               /* bLength */
	DESC_ENDPOINT,             /* bDescriptorType */
	(USBD_MIDI_EP_IN_ADDR | EP_INPUT),             /* bEndpointAddress */
	0x02,                               /* bmAttributes */
	EP2_MAX_PKT_SIZE,     /* wMaxPacketSize */
	0x00,
	0x00,                               /* bInterval */

	/* IN ENDPOINT MS */
	0x05,                               /* bLength */
	0x25,                               /* bDescriptorType */
	0x01,                               /* bDescriptorSubtype */
	0x01,                               /* bNumEmbMIDIJack */
	0x03,                               /* baAssocJackID */
};

#elif MIDI_VERSION == 2

#define USBD_MIDI20_CONFIG_DESC_SIZE 139
const uint8_t gu8ConfigDescriptor[] =
{
    0x09,                               /* bLength */
	DESC_CONFIG,        /* bDescriptorType */
    USBD_MIDI20_CONFIG_DESC_SIZE,       /* wTotalLength */
    0x00,
    0x02,                               /* bNumInterfaces */
    0x01,                               /* bConfigurationValue */
    0x00,                               /* iConfiguration */
    0x80,                               /* bmAttributes */
    0xFA,                               /* bMaxPower */

    /* AUDIO CONTROL INTERFACE */
    0x09,                               /* bLength */
	DESC_INTERFACE,            /* bDescriptorType */
    0x00,                               /* bInterfaceNumber */
    0x00,                               /* bAlternateSetting */
    0x00,                               /* bNumEndpoints */
    0x01,                               /* bInterfaceClass */
    0x01,                               /* bInterfaceSubClass */
    0x00,                               /* bInterfaceProtocol */
    0x00,                               /* iInterface */

    /* AUDIO CONTROL INTERFACE HEADER */
    0x09,                               /* bLength */
    0x24,                               /* bDescriptorType */
    0x01,                               /* bDescriptorSubtype */
    0x00,                               /* bcdADC */
    0x01,
    0x09,                               /* wTotalLength */
    0x00,
    0x01,                               /* bInCollection */
    0x01,                               /* bInterfaceNr */

	/* MIDI STREAM INTERFACE */
	0x09,                               /* bLength */
	DESC_INTERFACE,            /* bDescriptorType */
	0x01,                               /* bInterfaceNumber */
	0x00,                               /* bAlternateSetting */
	0x02,                               /* bNumEndpoints */
	0x01,                               /* bInterfaceClass */
	0x03,                               /* bInterfaceSubClass */
	0x00,                               /* bInterfaceProtocol */
	0x00,                               /* iInterface */

	/* MIDI 1.0 STREAM INTERFACE HEADER */
	0x07,                               /* bLength */
	0x24,                               /* bDescriptorType */
	0x01,                               /* bDescriptorSubtype */
	0x00,                               /* bcdMSC */
	0x01,
	54,                                 /* wTotalLength */
	0x00,

	/* MIDI IN JACK */
	0x06,                               /* bLength */
	0x24,                               /* bDescriptorType */
	0x02,                               /* bDescriptorSubtype */
	0x01,                               /* bJackType */
	0x01,                               /* bJackID */
	0x00,                               /* iJack */

	/* MIDI IN JACK */
	0x06,                               /* bLength */
	0x24,                               /* bDescriptorType */
	0x02,                               /* bDescriptorSubtype */
	0x02,                               /* bJackType */
	0x02,                               /* bJackID */
	0x00,                               /* iJack */

	/* MIDI OUT JACK */
	0x09,                               /* bLength */
	0x24,                               /* bDescriptorType */
	0x03,                               /* bDescriptorSubtype */
	0x01,                               /* bJackType */
	0x03,                               /* bJackID */
	0x01,                               /* bNrInputPins */
	0x02,                               /* baSourceID1 */
	0x01,                               /* BaSourcePin1 */
	0x00,                               /* iJack */

	/* MIDI OUT JACK */
	0x09,                               /* bLength */
	0x24,                               /* bDescriptorType */
	0x03,                               /* bDescriptorSubtype */
	0x02,                               /* bJackType */
	0x04,                               /* bJackID */
	0x01,                               /* bNrInputPins */
	0x01,                               /* baSourceID1 */
	0x01,                               /* BaSourcePin1 */
	0x00,                               /* iJack */

	/* OUT ENDPOINT */
	0x07,                               /* bLength */
	DESC_ENDPOINT,             /* bDescriptorType */
	USBD_MIDI_EP_OUT_ADDR,            /* bEndpointAddress */
	0x02,                               /* bmAttributes */
	EP3_MAX_PKT_SIZE,     /* wMaxPacketSize */
	0x00,
	0x00,                               /* bInterval */

	/* OUT ENDPOINT MS */
	0x05,                               /* bLength */
	0x25,                               /* bDescriptorType */
	0x01,                               /* bDescriptorSubtype */
	0x01,                               /* bNumEmbMIDIJack */
	0x01,                               /* baAssocJackID */

	/* IN ENDPOINT */
	0x07,                               /* bLength */
	DESC_ENDPOINT,             /* bDescriptorType */
	(USBD_MIDI_EP_IN_ADDR | EP_INPUT),             /* bEndpointAddress */
	0x02,                               /* bmAttributes */
	EP2_MAX_PKT_SIZE,     /* wMaxPacketSize */
	0x00,
	0x00,                               /* bInterval */

	/* IN ENDPOINT MS */
	0x05,                               /* bLength */
	0x25,                               /* bDescriptorType */
	0x01,                               /* bDescriptorSubtype */
	0x01,                               /* bNumEmbMIDIJack */
	0x03,                               /* baAssocJackID */

    /* MIDI 2.0 STREAM INTERFACE */
    0x09,                               /* bLength */
	DESC_INTERFACE,            /* bDescriptorType */
    0x01,                               /* bInterfaceNumber */
    0x01,                               /* bAlternateSetting */
    0x02,                               /* bNumEndpoints */
    0x01,                               /* bInterfaceClass */
    0x03,                               /* bInterfaceSubClass */
    0x00,                               /* bInterfaceProtocol */
    0x00,                               /* iInterface */

    /* MIDI STREAM INTERFACE HEADER */
    0x07,                               /* bLength */
    0x24,                               /* bDescriptorType */
    0x01,                               /* bDescriptorSubtype */
    0x00,                               /* bcdMSC */
    0x02,
    0x07,                                 /* wTotalLength */
    0x00,

	/* OUT ENDPOINT */
	0x07,                               /* bLength */
	DESC_ENDPOINT,             /* bDescriptorType */
	USBD_MIDI_EP_OUT_ADDR,            /* bEndpointAddress */
	0x02,                               /* bmAttributes */
	EP3_MAX_PKT_SIZE,     /* wMaxPacketSize */
	0x00,
	0x00,                               /* bInterval */

	/* OUT ENDPOINT MS */
	0x06,                               /* bLength */
	0x25,                               /* bDescriptorType */
	0x02,                               /* bDescriptorSubtype */
	0x02,                               /* bNumGrpTrmBlock */
	0x01,                               /* baAssoGrpTrmBlkID */
	0x02,                               /* baAssoGrpTrmBlkID */

    /* IN ENDPOINT */
    0x07,                               /* bLength */
	DESC_ENDPOINT,             /* bDescriptorType */
	(USBD_MIDI_EP_IN_ADDR | EP_INPUT),             /* bEndpointAddress */
    0x02,                               /* bmAttributes */
	EP2_MAX_PKT_SIZE,     /* wMaxPacketSize */
    0x00,
    0x00,                               /* bInterval */

    /* IN ENDPOINT MS */
    0x06,                               /* bLength */
    0x25,                               /* bDescriptorType */
    0x02,                               /* bDescriptorSubtype */
    0x02,                               /* bNumGrpTrmBlock */
    0x01,                               /* baAssoGrpTrmBlkID */
	0x02,                               /* baAssoGrpTrmBlkID */
};

/* MS 2.0 terminal block descriptor */
#define USBD_MIDI20_TERM_BLOCK_DESC_SIZE 31
uint8_t USBD_MIDI20_TERM_BLOCK_DESC[] =
{
    0x05,                               /* bLength */
    0x26,                               /* bDescriptorType */
    0x01,                               /* bDescriptorSubtype */
    USBD_MIDI20_TERM_BLOCK_DESC_SIZE,   /* wTotalLength */
    0x00,

    /* TERMINAL BLOCK 1 - MAIN */
    0x0D,                               /* bLength */
    0x26,                               /* bDescriptorType */
    0x02,                               /* bDescriptorSubtype */
    0x01,                               /* bGrpTrmBlkID */
    0x00,                               /* bGrpTrmBlkType */
    0x00,                               /* nGroupTrm */
    0x01,                               /* nNumGroupTrm */
    0x00,                               /* iBlockItem */
    0x11,                               /* bMIDIProtocol */
    0x00,                               /* wMaxInputBandwidth */
    0x00,
    0x00,                               /* wMaxOutputBandwidth */
    0x00,

	/* TERMINAL BLOCK 1 - OUT PORT */
	0x0D,                               /* bLength */
	0x26,                               /* bDescriptorType */
	0x02,                               /* bDescriptorSubtype */
	0x02,                               /* bGrpTrmBlkID */
	0x00,                               /* bGrpTrmBlkType */
	0x01,                               /* nGroupTrm */
	0x01,                               /* nNumGroupTrm */
	0x00,                               /* iBlockItem */
	0x03,                               /* bMIDIProtocol */
	0x01,                               /* wMaxInputBandwidth */
	0x00,
	0x01,                               /* wMaxOutputBandwidth */
	0x00,
};

#endif

/*!<USB Language String Descriptor */
uint8_t gu8StringLang[4] =
{
    4,              /* bLength */
    DESC_STRING,    /* bDescriptorType */
    0x09, 0x04
};

/*!<USB Vendor String Descriptor */
uint8_t gu8VendorStringDesc[] =
{
    14,
    DESC_STRING,
    'A', 0, 'l', 0, 'e', 0, 's', 0, 'i', 0, 's', 0
};

/*!<USB Product String Descriptor */
uint8_t gu8ProductStringDesc[] =
{
    22,
    DESC_STRING,
    'A', 0, 'l', 0, 'e', 0, 's', 0, 'i', 0, 's', 0, ' ', 0, 'Q', 0, '8', 0, '8', 0
};


const uint8_t gu8StringSerial[26] =
{
    22,             // bLength
    DESC_STRING,    // bDescriptorType
    '0', 0, '1', 0, '2', 0, '3', 0, '4', 0, '5', 0, '6', 0, '7', 0, '8', 0, '9', 0
};

const uint8_t *gpu8UsbString[4] =
{
    gu8StringLang,
    gu8VendorStringDesc,
    gu8ProductStringDesc,
    gu8StringSerial
};

const S_USBD_INFO_T gsInfo =
{
    gu8DeviceDescriptor,
    gu8ConfigDescriptor,
    gpu8UsbString,
    NULL
};


