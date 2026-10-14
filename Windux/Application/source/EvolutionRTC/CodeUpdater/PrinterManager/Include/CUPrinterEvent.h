#ifndef CUPRINTEREVENT_H
#define CUPRINTEREVENT_H

#include "Event.h"

// Marquee Device Generated Events

typedef enum CuPrinterEvent
{ 
	CuPrnReset, 
	CuPrnWakeup, 
	CuPrnCommand 
} CuPrinterEvent_t;

typedef enum CuPrinterErrors
{
	CuPrinterErrUnknownError = Marquee * DEVICE_ERROR_OFFSET,
} CuPrinterErrors_t;

#endif // CUPRINTEREVENT_H
