#ifndef ULTIMAWRAPPERMESSAGES_h
#define ULTIMAWRAPPERMESSAGES_h

#include "Wrapper.h"

#ifdef __cplusplus
	#pragma pack(1)
#endif

	/* Generic */
    typedef struct
	{
        UINT8	wrEvent;
	    UINT8  	pad0;
	} UltimaWrGeneric;

	/* Wakeup */
	typedef struct
	{
	    UINT8	wrEvent;
	    UINT8	pad0;
		UINT8	pad1; // BOOL (word field) on wrapper
		UINT8	wrCfgStatus;
	} UltimaWrWakeup;

	/* Inquire */
	typedef struct
	{
	    UINT8		wrEvent;
	    UINT8		pad0;
	    UINT16		configType;
	    UltimaWrCfg	config;
	} UltimaWrInquire;

	/* Config */
	typedef struct
	{
	    UINT8		wrEvent;
	    UINT8		pad0;
	    UINT16		configType;
	    UltimaWrCfg	config;
	} UltimaWrConfig;

	/* Mode */
	typedef struct
	{
	    UINT8	wrEvent;
	    UINT8	pad0;
	    UINT8	mode;
	    UINT8	pad1;
	} UltimaWrMode;

	/* Test */
	typedef struct
	{
	    UINT8	wrEvent;
	    UINT8	pad0;
	    UINT8	test;
	    UINT8	pad1;
		UINT16	data;
	} UltimaWrTest;

	/* Wrap */
	typedef struct
	{
	    UINT8	wrEvent;
	    UINT8	intakeMode;
	    UINT8	packageId; 
	    UINT8	pullFilm;  
	    UINT8	packCode;
		UINT8	pad0;
	} UltimaWrWrap;

	/* Cycle */
	typedef struct
	{
	    UINT8	wrEvent;
	    UINT8	pad0;
	    UINT8	packageId; 
	    UINT8	pullFilm;  
	    UINT8	packCode;
		UINT8	pad1;
	} UltimaWrCycle, UltimaWrJog;

	/* Control */
	typedef struct
	{
	    UINT8	wrEvent;
	    UINT8	pad0;
	    UINT16	heaterSetting; 
	    UINT8	heaterEnable;   
	    UINT8	filmTension;   
	    UINT8	packWidth;    
	    UINT8	pad1;
	} UltimaWrControl;

	/* Sense */
	typedef struct
	{
	    UINT8	wrEvent;
	    UINT8	pad0;
		UINT16	heaterReading;	
	    UINT16	encoderCount;    
	    UINT16	intakeCount; 

		// MSB -- Most significant Byte (transmitted from the UWS)
		// byte 0 lsb..msb
		UINT32	stopSwitch			:1;	 // lsb -- least significant bit
		UINT32	undefined0			:7;

		// byte 1 lsb..msb
		UINT32	narrowFilmSwitch	:1;	 // lsb -- least significant bit
		UINT32	wideFilmSwitch		:1;	
		UINT32	packageJamSwitch	:1;	
		UINT32	pusherHomeSwitch	:1;	
		UINT32	packageWidthSwitch	:1;	
		UINT32	packageSenseSwitch	:1;	
		UINT32	packageLengthSwitch	:1;	
		UINT32	startSwitch			:1;	// msb -- most significant bit

		// byte 2 lsb..msb
		UINT32	dischargeMotorState	:1;	 // lsb -- least significant bit
		UINT32	mainMotorState		:1;	
		UINT32	compressorState		:1;	
		UINT32	heaterState			:1;	
		UINT32	slideBlockState		:1;	
		UINT32	undefined1			:3;	// ms 3 bits
			
		// LSB -- Least Significant Byte  (transmitted from the UWS)
		// byte 3 lsb..msb
		UINT32	gripperState		:1;	 // lsb -- least significant byte
		UINT32	rightSideClampstate	:1;	
		UINT32	leftSideClampState	:1;	
		UINT32	knifeState			:1;	

		// 0 = 0x00 1 = 0x10 2 = 0x20 3 = 0x30
		UINT32	filmWidthState		:2;	

		// 0 = 0x00 1 = 0x40 2 = 0x80 3 = 0xC0
		UINT32	filmLengthState		:2;	// ms 2 bits
	} UltimaWrSensors;
	/* Film Auto Load */

	typedef struct
	{
	    UINT8	wrEvent;
	    UINT8	pad0;
	    UINT8	filmWidth; 
	    UINT8	pad1;
	} UltimaWrLoad;

	typedef struct
	{
	    UINT8				wrEvent;
	    UINT8				pad0;
		WrErrPkgHistoryData	data[MaxErrorHistoryBuffers];	
	} UltimaWrErrorHistory;

	typedef struct
	{
	    UINT8			wrEvent;
	    UINT8			pad0;
	    WrStatusField	status;
	    WrErrorField	error;
	    WrErrorField	history;
	    UINT8			mode;
		UINT8			pad1;
	    UINT16			event;  
	    WrHeaterField	heater;
	    WrPackageField	package[NumWrapperPositions];
	} UltimaWrStatus;

#ifdef __cplusplus
	#pragma pack()
#endif

#endif // ULTIMAWRAPPERMESSAGES_h
