#ifndef MARQUEEEVENT_H
#define MARQUEEEVENT_H

#include "Event.h"

// Marquee Device Generated Events

typedef enum MqEvent
{ 
	MqWakeup, 
	MqConfig, 
	MqInquire, 
	MqReset, 
	MqStatus,
	MqEnable, 
	MqDisable, 
	MqMode, 
	MqTest,
	MqUpdate,
	MqClear,
	MqSysReqStatus = 13,
	MqReqWakeup = 32,
	MqReqInquire,
	MqKind,
	MqOperateMode


} MqEvent_t;

typedef enum MarqueeErrors
{
	MarqueeErrUnknownError = Marquee * DEVICE_ERROR_OFFSET,
} MarqueeErrors_t;

#endif // MARQUEEEVENT_H
