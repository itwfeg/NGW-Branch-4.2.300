#ifndef HANDSFREEDEVICEEVENT_H
#define HANDSFREEDEVICEEVENT_H

#include "Event.h"

// Hands-Free Device Device Generated Events

typedef enum HFDEvent
{ 
	HFDGenericEvent,
	HFDSendMessage,
	HFDSendOpNameList

} HFBEvent_t;

typedef enum HandsFreeDeviceErrors
{
	HandsFreeDeviceErrUnknownError = HandsFreeDevice * DEVICE_ERROR_OFFSET
} HandsFreeDeviceErrors_t;

#endif // HANDSFREEDEVICEEVENT_H
