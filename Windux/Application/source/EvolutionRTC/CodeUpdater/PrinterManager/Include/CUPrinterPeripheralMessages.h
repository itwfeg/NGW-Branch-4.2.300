#ifndef CUPRINTERPERIPHERALMESSAGES_H
#define CUPRINTERPERIPHERALMESSAGES_H

#include "CUPrinter.h"

#include "USBEvolHeader.h"

#ifdef __cplusplus
	#pragma pack(1)
#endif

	// These are the messages that will be sent to the printer peripheral!!

	/* Generic */
	typedef struct
	{
	    UINT8	cuEvent;
	    UINT8	pad0;
	} CuPrnGenericMsg;

	typedef struct 
	{
	    UINT8	cuEvent;
	    UINT8	pad0;
	    UINT16	FlashUpdateMajorRev;
	    UINT16	FlashUpdateMinorRev;
	    UINT16	FlashUpdateDormantMajorRev; //two versions of flash updater code. Only one is active.
	    UINT16	FlashUpdateDormantMinorRev; //two versions of flash updater code. Only one is active.
	    UINT16	PrinterMajorRev;
	    UINT16	PrinterMinorRev;
	    UINT16	BootCodeMajorRev;
	    UINT16	BootCodeMinorRev;
	    UINT8	HardwareMajorRev;
	    UINT8	HardwareMinorRev;
	    UINT8	ManufacturerID;
	    UINT8	DeviceID;
		UINT32	BitError;
		UINT32  AddressError;
		UINT8	SelectedFLUD;
		UINT8	FLUD1Status;
		UINT8	FLUD2Status;
		UINT8	Configuration;
		UINT8   BootPrnFailureCnt;
		UINT8   FludFailureCnt;
	} CuPrnWakeupMsg;

	typedef struct 
	{
	    UINT8	cuEvent;
	    UINT8	pad0;
	    UINT8	command;
		UINT8	pad1;
	    UINT32 	StartAddress;
		UINT32 	EndAddress;
		UINT32 	codeSize;
	} CuPrnCommandMsg;


#ifdef __cplusplus
	#pragma pack()
#endif

#endif // CUPRINTERPERIPHERALMESSAGES_H
