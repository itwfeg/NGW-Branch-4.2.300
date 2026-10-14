#ifndef MARQUEEPERIPHERALMESSAGES_H
#define MARQUEEPERIPHERALMESSAGES_H

#include "Marquee.h"

#include "USBEvolHeader.h"

#ifdef __cplusplus
	#pragma pack(1)
#endif

	// These are the messages that will be sent to the printer peripheral!!

	/* Generic */
	typedef struct
	{
	    UINT8	mqEvent;
	    UINT8	pad0;
	} MqGenericMsg;

	typedef struct 
	{
	    UINT8	mqEvent;
	    UINT8	pad0;
	    UINT8	configuration;
	    UINT8	displayType;
	} MqWakeupMsg;

	typedef struct 
	{
	    UINT8	mqEvent;
	    UINT8	pad0;
	    UINT16	disposition;
	    UINT8	deviceName[MAX_DEVICE_NAME_STRING];
	} MqConfigMsg;

	typedef struct 
	{
	    UINT8	mqEvent;
	    UINT8	pad0;
	    UINT16	disposition;
	    UINT8	deviceName[MAX_DEVICE_NAME_STRING];
	} MqInquireMsg;

	typedef struct 
	{
	    UINT8	mqEvent;
	    UINT8	pad0;
	    UINT8	mode;
		UINT8 	pad1;
	} MqModeMsg;

	typedef struct 
	{
	    UINT8	mqEvent;
	    UINT8	pad0;
	    UINT8	test;
		UINT8 	pad1;
	} MqTestMsg;

	typedef struct 
	{
	    UINT8	mqEvent;
	    UINT8	pad0;
	    UINT8	bufferType;
		UINT8 	pad1;
		UINT8	displayText[MAX_MARQUEE_STRING + 1];
	} MqUpdateMsg;


#ifdef __cplusplus
	#pragma pack()
#endif

#endif // MARQUEEPERIPHERALMESSAGES_H
