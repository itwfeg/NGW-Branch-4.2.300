#ifndef RENDEREREVENT_H
#define RENDEREREVENT_H

	#include "Event.h"

	typedef enum RdEvent
	{
		RdReqSession,		// to,		none
		RdSessionId,		// from, 	UINT8   		Response to RdReqOpenSession 
		RdSetSource,		// to,		UINT8, *UINT8
		RdSetDest,			// to,		UINT8, *UINT8
		RdReqRendering,		// to,		UINT8
		RdRenderStart,		// from		none			Response to RdReqRendering
		RdRenderComplete,	// from		none			Data is valid for high speed pipe 
		RdCloseSession,		// to,		UINT8
		RdInitDotHeadWear,
		RdNewDotHeadWear
	} RdEvent_t;


#endif // RENDEREREVENT_H
