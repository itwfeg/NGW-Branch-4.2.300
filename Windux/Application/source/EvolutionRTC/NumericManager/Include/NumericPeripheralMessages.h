#ifndef NUMERICPERIPHERALMESSAGES_H
#define NUMERICPERIPHERALMESSAGES_H

#include "Numeric.h"

#include "USBEvolHeader.h"

#ifdef __cplusplus
	#pragma pack(1)
#endif

	// These are the messages that will be sent to the printer peripheral!!

	/* Generic */
	typedef struct
	{
	    UINT8	nmEvent;
	    UINT8	pad0;
	} NmGenericMsg;

	typedef struct 
	{
	    UINT8	nmEvent;
	    UINT8	pad0;
	    UINT8	configuration;
	    UINT8	displayType;
	} NmWakeupMsg;

	typedef struct 
	{
	    UINT8	nmEvent;
	    UINT8	pad0;
	    UINT16	disposition;
	    UINT8	deviceName[MAX_DEVICE_NAME_STRING];
	} NmConfigMsg;

	typedef struct 
	{
	    UINT8	nmEvent;
	    UINT8	pad0;
	    UINT16	disposition;
	    UINT8	deviceName[MAX_DEVICE_NAME_STRING];
	} NmInquireMsg;

	typedef struct 
	{
	    UINT8	nmEvent;
	    UINT8	pad0;
	    UINT8	mode;
		UINT8 	pad1;
	} NmModeMsg;

	typedef struct 
	{
	    UINT8	nmEvent;
	    UINT8	pad0;
	    UINT8	test;
		UINT8 	pad1;
	} NmTestMsg;

	typedef struct 
	{
	    UINT8	nmEvent;
	    UINT8	pad0;
	    UINT8	field;
		UINT8 	pad1;
		UINT8	fieldData[MAX_NUMERIC_STRING];
	} NmFieldMsg;

	typedef struct 
	{
	    UINT8	nmEvent;
	    UINT8	pad0;
	    UINT8	index;
		UINT8	pad1;
	    UINT8	state;
		UINT8	pad2;
	} NmAnnuncicatorMsg;

#ifdef __cplusplus
	#pragma pack()
#endif

#endif // NUMERICPERIPHERALMESSAGES_H
