#ifndef DESCRIPTOR_H
#define DESCRIPTOR_H

/* #include <assert.h> */
#include "usb.h"

	#define HOBART_VENDOR_ID	     0x128C
	#define HOBART_LSMN_ID		     0x0001 
	#define HOBART_LSMN_REL	     	     0x0100

        #define HOBART_EVO_PRINTER_ID        0x0200
        #define HOBART_ESS_PRINTER_ID        0x0201
        #define HOBART_ESC_PRINTER_ID        0x0203
        #define ACCESS_PERIPHERAL_CONTROLLER 0x0204
        #define EVOL_WEIGHER_ID              0x0400

	enum USB_Descriptors {
		DEVICE_DESCRIPTOR = 1,
		CONFIGURATION_DESCRIPTOR = 2,
		STRING_DESCRIPTOR = 3,
		INTERFACE_DESCRIPTOR = 4,
		ENDPOINT_DESCRIPTOR = 5,
		DEVICE_QUALIFIER_DESCRIPTOR = 6,
		OTHER_SPEED_CONFIGURATION_DESCRIPTOR = 7,
		INTERFACE_POWER_DESCRIPTOR = 8
	};

        /* size of descriptor blocks per USB spec */
        #define DEVICE_DESCRIPTOR_SIZE 18
        #define CONFIGURATION_DESCRIPTOR_SIZE 9
        #define INTERFACE_DESCRIPTOR_SIZE 9
        #define ENDPOINT_DESCRIPTOR_SIZE 7
	
	/* English United States Only */
	/* for others, consult: http://www.usb.org/developers/data/USB_LANGIDs.pdf */
	#define USB_ENGLISH_US	0x0409

	/* The total number of items allowing in the lists */

	/* Some, hopefully, reasonable numbers */
	#define USB_MAX_LANGUAGE_ID	10
	#define USB_MAX_STRING_DESCRIPTORS	25
	#define	USB_MAX_ENDPOINTS_INPUT		15	/* defined by usb spec */
	#define	USB_MAX_ENDPOINTS_OUTPUT	15	/* defined by usb spec */
	#define USB_MAX_INTERFACES			10
	#define USB_MAX_CONFIGURATIONS		10

	/* device & endpoint descriptors */
	#define USB_VENDOR_SPECIFIC	0xFF

	/*device descriptor */
	#define USB_MAX_POWER	500		/* 500 mA is max power for a USB device */

	/* for configuration descriptors */
	#define USB_NO_REMOTE_WAKEUP	0x00	/* Bit 5 0FF */
	#define USB_REMOTE_WAKEUP		0x20	/* Bit 5 0NN */
	#define USB_BUS_POWERED			0x00	/* Bit 6 0FF */
	#define USB_SELF_POWERED		0x40	/* Bit 6 0N */

	/* Here are the definitions necessary for Endpoints */

	/* Endpoint Types */

	#define USB_EP_CONTROL		0x00
	#define USB_EP_ISOCHRONOUS	0x01
	#define USB_EP_BULK			0x02
	#define USB_EP_INTERRUPT	0x03

	/* Endpoint Directions */
	#define USB_EP_OUT	0x00
	#define USB_EP_IN	0x80

	/* Isochronous Endpoints Usage Types */
	#define USB_EP_DATA					0x00
	#define USB_EP_FEEDBACK				0x10
	#define USB_EP_IMPLICIT_FEEDBACK	0x20

	/* Isochronous Endpoints Synch Type */
	#define USB_EP_NO_SYNCH	0x00
	#define USB_EP_ASYNC	0x04
	#define USB_EP_ADAPTIVE	0x08
	#define USB_EP_SYNC		0x0A

#endif
