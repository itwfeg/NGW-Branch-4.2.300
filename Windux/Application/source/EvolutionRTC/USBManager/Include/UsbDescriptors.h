#ifndef UsbDescriptors_h
#define UsbDescriptors_h

#include "usb100.h"

struct UsbDescriptors
{
	char str[255]; //holder of string descriptor

	PUSB_DEVICE_DESCRIPTOR			deviceDescriptor;
	PUSB_CONFIGURATION_DESCRIPTOR	configurationDescriptor;
	PUSB_INTERFACE_DESCRIPTOR		interfaceDescriptor;
};

#endif 