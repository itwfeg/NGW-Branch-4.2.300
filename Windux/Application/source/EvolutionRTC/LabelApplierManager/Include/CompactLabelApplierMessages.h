#ifndef COMPACTLABELAPPLIERMESSAGES_H
#define COMPACTLABELAPPLIERMESSAGES_H

#include "LabelApplier.h"
#include "System.h"

//Removed 8/21 Alex Zavala
//boardInfo defines
/*#define PART_NUMBER_SIZE			10
#define REVISION_SIZE				2
#define DATE_OF_MANUFACTURE_SIZE	6
#define SERIAL_NUMBER_SIZE			6

//awsInfo defines
#define AWS_SERIAL_NUMBER_SIZE			12
#define AWS_DATE_OF_MANUFACTURE_SIZE	6
#define AWS_DATE_OF_INSTALL_SIZE		6
*/

	// SMM -- ERRATA!!!
	/* PACKING OF STRUCTS!!!
	From http://www.delorie.com/djgpp/v2faq/faq22_12.html

	Q: When I use struct ffblk from the header dir.h in a C++ program, I get garbage in some members of the structure! 



	A: There is a known bug in GCC 2.7.2: the C++ compiler effectively ignores the __attribute__((packed)) directives, so the structures end up being not packed. GCC versions 2.7.2.1 and later corrected that bug, so upgrade. As a work-around, surround the declaration of the structure that needs to be packed with #pragma pack, like this: 

	  #ifdef __cplusplus
	  #pragma pack(1)
	  #endif
	  .
	  .
	  .
	  #ifdef __cplusplus
	  #pragma pack()
	  #endif
	*/

#ifdef __cplusplus
	#pragma pack(1)
#endif

typedef enum cla_function_code
{
	CLA_WAKEUP,
	CLA_CONFIGURE,
	CLA_INQUIRE,
	CLA_RESET,
	CLA_STATUS,
	CLA_ENABLE,
	CLA_DISABLE,
	CLA_MODE,
	CLA_TEST,
	CLA_OPEN_WINDOW,
	CLA_CLOSE_WINDOW,
	CLA_LABEL_READY,
	CLA_GET_SENSOR,
	CLA_SETUP = 14,
	CLA_OFF,
	CLA_DEFINE,				/* Internal Only */
	CLA_EXECUTE,			/* Internal Only */
	CLA_UPDATE_SENSOR,		/* Internal Only */
	CLA_EMITTER,			/* Internal Only */
	CLA_ENABLE_SENSOR,		/* Internal Only */
	CLA_SYSTEM_STATUS,
	CLA_PACKAGE_READY,		/* NEEP */
	CLA_LABEL_TAKEN,		/* NEEP */
	CLA_ON,					/* NEEP */	
	CLA_PRINT_PKG = 32,
	CLA_ULTIMA_AUTO_MODE,	/*this function code forces the ultima to switch to a auto mode	*/
	CLA_TEST_COMPLETE
							/*before the window open command is sent*/
} CLA_FUNCTION_CODE;


//typedef enum _cla_station_id
//{
//	WEIGHER_STATION,
//	PRI_PRINTER_STATION,
//	SEC_PRINTER_STATION
//} CLA_STATION_ID;


	/* Generic */
	typedef struct
	{
	    UINT8	function_code;
	    UINT8	pad;
	} CompactLaGeneric;

	typedef struct
	{
		UINT16      laEvent;
		UINT16      param;
	} CompactLaGenericOneParam;

	typedef struct
	{
		UINT8	function_code;
		bool	needUpdate;
		UINT16	update;
	} CompactLaUpdateCLA;

	typedef struct
	{
		UINT8		function_code;
		UINT8		pad;
		UINT16      node1;
		UINT16      update1;
		UINT16      node3;
		UINT16      update3;
		UINT16      node4;
		UINT16      update4;
		UINT16      node5;
		UINT16      update5;
		UINT16      node6;
		UINT16      update6;
		UINT16      node2;
		UINT16      update2;
	} CompactLaUpdate;

	typedef struct _cla_wakeup_msg	//Added "_" to the beginning of struct name to remove complaint from compiler
	{
	    UINT8	function_code;
	    UINT8	type;  /* NEEP */
	    UINT8	ready_to_run;
	} CompactLaWakeup;

	/* Status */
	typedef struct
	{
	    UINT8	function_code;
	    UINT8	pad0;
	    UINT16	status;
	} CompactLaStatus;

	/* Label Applier Configuration Block */
	/* Configuration */
	typedef struct
	{
		UINT8	has_weigher;
		UINT8	has_primary_printer;
		UINT8	has_secondary_printer;
		UINT8	left_to_right;	// package direction
		UINT8	front_discharge;
//		UINT8	primary_justify_font;
//		UINT8	secondary_justify_font;
//		UINT8	transfer_station_type;
		UINT8	primary_printer_label_offset;
		UINT8	secondary_printer_label_offset;
		//Alex Zavala 8/20
		UINT8	label_placement;
		UINT8	overdrive;//Alex Zavala 11/7 new config parameter
//		UINT8	weigher_conveyor_sensitivity;//X
//		UINT8	primary_printer_conveyor_sensitivity;//X
//		UINT8	secondary_printer_conveyor_sensitivity;//X
//		UINT8	primary_printer_max_justification_distance;//X
//		UINT8	secondary_printer_max_justification_distance;//X
//		UINT8	diagonal_label_placement;//X
//		UINT8	diagonal_offset;//X
//		UINT8	ministation_label_placement;//X
//	    UINT8	device_signature[SystemDevSignatureSize];//X
	} CompactLaCfg;

	/* Inquire */
	typedef struct
	{
	    UINT8		function_code;
	    UINT8 		configType;
	    CompactLaCfg	config;
	} CompactLaInquire;

	/* Config */
	typedef struct
	{
	    UINT8		function_code;
	    UINT8		configType;
	    CompactLaCfg	config;
	} CompactLaConfig;

	/* Mode */
	typedef struct
	{
	    UINT8	function_code;
	    UINT8	mode;
	} CompactLaMode;

	typedef struct 
	{
	    UINT8	function_code;
	    UINT8	station;
		//UINT8	pad0;			// because in the Ultima World it is sixteen bits.
	    UINT8	test;
	    UINT16	result;
	    UINT16	parm1;
	    UINT16	parm2;
	} CompactLaTest;

	typedef struct
	{
	    UINT8	function_code;
	    UINT8	station;
	    UINT8	package_id;
	    UINT8	pad0;
	} CompactLaLabelReady;

	typedef struct
	{
	    UINT8	function_code;
	    UINT8	station;
	    UINT8	package_id;
	    UINT8	pad0;
	} CompactLaLabelTaken;
/*
	typedef struct
	{
	    UINT8	function_code;
	    UINT8	pad0;
	    UINT8	sensor_state[LastSensor];
	} CompactLaSensors;
*/
	/*	New CLA GET SENSOR MSG*/
	typedef struct
	{
		UINT16	Wake_up:1;
		UINT16	Weigher_entry:1;
		UINT16	CLA_entry:1;
		UINT16	CLA_pkg_edge_detect:1;
		UINT16	StartSwitch:1;
		UINT16	StopSwitch:1;
		UINT16	CLAGateSwitch:1;
		UINT16	CLAPrinterPostSwitch:1;
		UINT16	Spare1:1;
		UINT16	Spare2:1;
		UINT16	Spare3:1;
		UINT16	Spare4:1;
		UINT16	Spare5:1;
		UINT16	Spare6:1;
		UINT16	Spare7:1;
		UINT16	Spare8:1;
	} NODE_SWITCHES;

	typedef union
	{
		NODE_SWITCHES	switches;
		UINT16			generic_switches;
	} CLASwitchesAndSensors;

	typedef struct
	{
		UINT8	function_code;
		UINT8	pad;
		CLASwitchesAndSensors	switches_and_sensors;
	} CompactLaSensors;

	typedef struct
	{
	    UINT8	function_code;
	    UINT8	pad0;
	    UINT16	pri_label_size;
	    UINT16	pri_label_rotation;
	    UINT16	sec_label_size;
	    UINT16	sec_label_rotation;
	} CompactLaSetup;

	typedef struct
	{
	    UINT8	function_code;
	    UINT8	package_id;
	} CompactLaPrint;

	typedef struct
	{
	    UINT8	function_code;
	    UINT8	package_id;
	} CompactLaUltimaAuto;

	typedef struct
	{
	    UINT8	function_code;
	    UINT8	package_id;
	} CompactLaOpenWindow;

	typedef struct
	{
	    UINT8	function_code;
	    UINT8	package_id;
	} CompactLaCloseWindow;

	typedef struct
	{
	    UINT8	function_code;
	    UINT8	package_id;
	} CompactLaPackageReady;

	//Alex Zavala 8/14
	typedef struct
	{
		UINT16	major;
		UINT16	minor;
	} CLAComponentRevision;

	typedef struct
	{
		CLAComponentRevision appFlash;
		CLAComponentRevision dataFlash;
		CLAComponentRevision bootFlash;
	} CLANodeVersion;

	typedef struct
	{
		ULONG configuration_checksum;
		CLANodeVersion nodeVersion;
	} CLASysInfo;
/*
	typedef enum _aws_model
	{
		NO_MODEL,
		AWS_1RL,
		AWS_1LR,
		AWS_1MLR,
		AWS_1LR_I,
		AWS_1RL_I,
		AWS_1BWRL,
		AWS_1BWLR
	} AWSModel;
*/
	/*
	partNumber, boardRev, dateOfManufacture, and serialNumber come in as ascii values coded in hex
	boardSectionChecksum comes in as a ULONG in hex
	*/
	typedef struct
	{
		UINT8 partNumber[PART_NUMBER_SIZE];
		UINT8 boardRev[REVISION_SIZE];
		UINT8 dateOfManufacture[DATE_SIZE];
		UINT8 serialNumber[SERIAL_NUMBER_SIZE];
		ULONG boardSectionChecksum;
	} EEP93C56Board;

	/*
	awsSerialNumber, dateOfManufacture, dateOfInstall, and awsModelNumber come in as ascii values coded in hex
	cycleCount and awsSectionChecksum come in as ULONGs in hex
	*/
	typedef struct
	{
		UINT8 awsSerialNumber[AWS_SERIAL_NUMBER_SIZE];
		UINT8 dateOfManufacture[DATE_SIZE];
		UINT8 dateOfInstall[DATE_SIZE];
		UINT16 awsModelNumber;	//Ask Carlos if there is a way to keep it AWSModel awsModelNumber like it was done in the c code because enums are 4bytes
		UINT16 pad0;
		ULONG cycleCount;
		ULONG awsSectionChecksum;
	} EEP93C56AWS;

	typedef struct
	{
		EEP93C56Board boardInfo;
		EEP93C56AWS awsInfo;
	} EEP93C56Contents;
/*
	typedef struct
	{
	    UINT8	 	 	function_code;
	    UINT8		 	pad0;
	    SysStatusInfo	master_status;
	    SysStatusInfo	slave_1_status;
	    SysStatusInfo	slave_2_status;
	} CompactLaSystemStatus;
*/
	typedef struct
	{
	    UINT8	 	 		function_code;
	    UINT8		 		pad;
		UINT16				pad0;
	    CLASysInfo			system_information;
	    //EEP93C56Contents	eep_information;
	} CompactLaSysStatus;

	typedef struct
	{
		UINT8				function_code;
		UINT8				pad;
		UINT16				pad0;
		EEP93C56Contents	eep_information;
	} CompactLaSysInfo;

	//New CLA SysInfoWrite msg
	typedef struct
	{
		UINT8			function_code;
		UINT8			pad;
		UINT8			awsSerialNumber[AWS_SERIAL_NUMBER_SIZE];
		UINT8			dateOfManufacture[DATE_SIZE];
		UINT8			dateOfInstall[DATE_SIZE];
		UINT16			awsModelNumber;	//Ask Carlos if there is a way to keep it AWSModel awsModelNumber like it was done in the c code because enums are 4bytes
		ULONG			cycleCount;
		ULONG			awsSectionChecksum;
	} CompactLaSysInfoWrite;

	//Alex Zavala 8/23 new CLA_TEST_COMPLETE msg
	typedef struct
	{
		UINT16	data1;
		UINT16 data2;
	} WrShortTestData;

	typedef struct
	{
		WrShortTestData data_short;
		ULONG data_long;
	} WrTestData;

	typedef struct
	{
		UINT8 function_code;
		UINT8 pad;
		UINT8 test;
		UINT8 pad0;//pad here so UWORDs are added on the 4th byte
		WrTestData parameter;
		bool result;
	} CompactLaTestComplete;

	typedef struct
	{
		UINT8		function_code;
		UINT8		pad;
		UINT16      nodeId;
		UINT16      status;
		UINT16      fault;
		UINT16      updateFailureCount;     
		UINT16      softComponent;  
		UINT32      addr;
	} CompactLaLoaderStatus;

	typedef struct
	{
		UINT8		function_code;
		UINT8		package_id;
	} CompactLaPackageDischarge;

#ifdef __cplusplus
	#pragma pack()
#endif

#endif // COMPACTLABELAPPLIERMESSAGES_H

