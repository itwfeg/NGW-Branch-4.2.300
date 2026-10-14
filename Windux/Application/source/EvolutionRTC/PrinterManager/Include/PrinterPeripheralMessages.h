#ifndef PRINTERPERIPHERALMESSAGES_H
#define PRINTERPERIPHERALMESSAGES_H

#include "Printer.h"
// #include "PrinterTeachTable.h"
#include "CommandOperations.h"
#include "USBEvolHeader.h"
#include "System.h"

#ifdef __cplusplus
	#pragma pack(1)
#endif

	// These are the messages that will be sent to the printer peripheral!!

	/* Generic */
	typedef struct
	{
	    UINT8	prEvent;
	    UINT8	pad0;
	} PrGenericMsg;

	typedef struct 
	{
	    UINT8	prEvent;
	    UINT8	pad0;
	    UINT8	printerType;
	    UINT8	labelWidth;
	    UINT16	bufferSize;
	    UINT16	printheadSize;
	    UINT8	prCfgStatus;
	} PrWakeupMsg;

	typedef struct 
	{
	    UINT8	prEvent;
	    UINT8	pad0;
	    UINT8	home;
	    UINT8	installed;
	    UINT8	interlock;
	    UINT8	jammed;
	} PrCutterStatusMsg;

	typedef struct 
	{
	   	UINT8	deviceSignature[SystemDevSignatureSize]; 
	    UINT8	printerType; 
	    UINT8	labelWidth;
	    UINT8	mediaSensorAdjustment;
	    UINT8	outOfMediaCount;	// Immutable!! Set to 200 on Ultima
	    UINT16	printContrast;		// "Print Contrast" on Ultima Configuration.
	    UINT16	configWord0;			// Used on "European" labels
	    UINT16	configWord1;			// Used on "European" labels
	    UINT16	configWord2;		// Used on "European" labels
	    UINT16	configWord3;		// "Expel Position" on Ultima Configuration.
	    UINT16	configWord4;			// Used on "European" labels
	    UINT16	configWord5;
	    UINT16	configWord6;
	    UINT16	configWord7;
	    UINT16	configWord8;
	    UINT16	configWord9;
	    UINT16	configWord10;
	    UINT16	configWord11;
	    UINT16	configWord12;
	    UINT16	configWord13;
	    UINT16	mediaSensorType;
	    UINT16	slaEjectLine;
	    UINT16	printPosition;		// "Print Position" (SKEW) on Ultima Configuration. Only valid for LabelWidth = { Narrow | UFWLabel }
	} PrConfiguration;

	/* Config */
	typedef struct
	{
	    UINT8			prEvent;
	    UINT8			pad0;
	    UINT16			prConfigType;
	    PrConfiguration	prConfiguration;
	} PrConfigMsg;

	/* Inquire */
	typedef struct
	{
	    UINT8			prEvent;
	    UINT8			pad0;
	    UINT16			prConfigType;
	    PrConfiguration	prConfiguration;
	} PrInquireMsg;

	/* Mode */
	typedef struct
	{
	    UINT8	prEvent;
	    UINT8	pad0;
	    UINT8	mode;
	} PrModeMsg;

	/* Printhead Power */
	typedef struct
	{
	    UINT8	prEvent;
	    UINT8	pad0;
	    UINT16	powerState;
		UINT16	scProtection;	// short-circuit protection 0 = Off 1 = On
	} PrPowerMsg;

	/* Printhead Sensors */
	typedef struct
	{
	    UINT8	prEvent;
	    UINT8	pad0;
	    UINT16	headupReading;
	    UINT16	labelWidthReading;
	    UINT16	regulatorTemperatureReading;
	    UINT16	headTemperatureReading;
	    UINT16	headVoltageReading;
	    UINT16	headCurrentReading;
	    UINT16	labelHighAverage; 
	    UINT16	labelLowAverage;
	    UINT16	labelThreshold; 
	    UINT16	labelReading; 
	    UINT16	mediaHighAverage;
	    UINT16	mediaLowAverage;
	    UINT16	mediaThreshold;
	    UINT16	mediaReading; 
	} PrSensorsMsg;

	/* Command */
	typedef struct
	{
	    UINT8	prEvent;
	    UINT8	pad0;
	    UINT8	printerCommandType;
	    UINT8	printerCommandOptions;
	    UINT16	printerIndirection; 
	    UINT16	value;
	} PrCommandMsg;

	typedef struct
	{
	    UINT8	prEvent;
	    UINT8	pad0;
	    UINT8	printerCalibrateType;
	} PrCalibrateMsg;

	typedef struct
	{
	    UINT8	prEvent;
	    UINT8	pad0;
	    UINT16	actualSize;
	    UINT16	measuredSize;
	} PrNomSizeMsg;

	typedef struct
	{
	    UINT8				prEvent;
	    UINT8				pad0;
	    IndirectDataItem	ram[PrNumRamLocations]; 
	} PrRamMsg;
	 
	/* Test */
	typedef struct
	{
	    UINT8	prEvent;
	    UINT8	pad0;
	    UINT8	data; 
	    UINT8	pad1;
	} PrTestMsg;

	typedef struct
	{
	    UINT8				prEvent;
	    UINT8				pad0;
	    UINT16				entries;
	    UINT8				command;
	    UINT8				pad1;
	    CommandOperationMsg	operation[MaxTeachTableEntries];
	} PrTeachMsg;

	typedef struct
	{
	    UINT8	sensorMask;
	    UINT8	userMask;
	    UINT8	sensor2Mask;
		UINT8	pad1;
	} PrMaskInfo;

	/* Mask */
	typedef struct
	{
	    UINT8		prEvent;
	    UINT8		pad0;
		PrMaskInfo	maskInfo;
	} PrMaskMsg;

	/* Status */
	typedef struct
	{
	    UINT8		prEvent;
	    UINT8		pad0;
	    UINT8		state;
	    UINT8		command;
	    UINT8		error;
	    UINT8		history;
	    UINT8		sensor;
	    UINT8		user;
	    UINT16		counter;
		PrMaskInfo	maskInfo;
	    UINT8		sla;
		UINT8		sensor2;
	} PrStatusMsg;

	typedef struct
	{
	    UINT8	prEvent;
	    UINT8	pad0;
	    UINT16	bufferSize;
	} PrReqLabelTransferMsg;

	typedef struct
	{
	    UINT8	prEvent;
	    UINT8	pad0;
	    UINT8	ioCfg;
	} VpCommCfgMsg;

	typedef struct
	{
	    UINT8			function_code;
	    UINT8 			pad0;
	    SysStatusInfo	system_status;
		UINT16			software_major_revision;
		UINT16			software_minor_revision;
		UINT16			software_eng_revision;		//PJR-301
		UINT16			software_weigher_major_revision;//weigher Major Rev
		UINT16			software_weigher_minor_revision;//weigher Minor Rev
		UINT16			software_weigher_eng_revision;	//weigher eng Rev
		UINT8			hardware_major_revision;  // Device ID
		UINT8			hardware_minor_revision;		
	} PrSysStatusMsg;

	typedef struct
	{
	    UINT8			function_code;
	    UINT8		   	pad0;
	    UINT16  		configType;
	    SysConfigInfo	system_config;
		UINT8			hardware_major_revision;  // Device ID
		UINT8			hardware_minor_revision;		
	} PrSysConfigMsg;

	typedef struct
	{
	    UINT8			function_code;
	    UINT8		   	pad0;
	    UINT8			result;
	}PrDfltComplete;
#ifdef __cplusplus
	#pragma pack()
#endif

#endif // PRINTERPERIPHERALMESSAGES_H
