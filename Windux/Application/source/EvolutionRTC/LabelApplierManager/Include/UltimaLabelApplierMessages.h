#ifndef ULTIMALABELAPPLIERMESSAGES_H
#define ULTIMALABELAPPLIERMESSAGES_H

#include "LabelApplier.h"
#include "System.h"

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

	/* Generic */
	typedef struct
	{
	    UINT8	function_code;
	    UINT8	pad;
	} UltimaLaGeneric;

	typedef struct cla_wakeup_msg
	{
	    UINT8	function_code;
	    UINT8	type;  /* NEEP */
	    UINT8	ready_to_run;
	} UltimaLaWakeup;

	/* Status */
	typedef struct
	{
	    UINT8	function_code;
	    UINT8	pad0;
	    UINT16	status;
	} UltimaLaStatus;

	/* Label Applier Configuration Block */
	/* Configuration */
	typedef struct
	{
		UINT8	has_weigher;
		UINT8	has_primary_printer;
		UINT8	has_secondary_printer;
		UINT8	left_to_right;	// package direction
		UINT8	front_discharge;
		UINT8	primary_justify_font;
		UINT8	secondary_justify_font;
		UINT8	transfer_station_type;
		UINT8	primary_printer_label_offset;
		UINT8	secondary_printer_label_offset;
		UINT8	weigher_conveyor_sensitivity;
		UINT8	primary_printer_conveyor_sensitivity;
		UINT8	secondary_printer_conveyor_sensitivity;
		UINT8	primary_printer_max_justification_distance;
		UINT8	secondary_printer_max_justification_distance;
		UINT8	diagonal_label_placement;
		UINT8	diagonal_offset;
		UINT8	ministation_label_placement;
	    UINT8	device_signature[SystemDevSignatureSize];
	} UltimaLaCfg;

	/* Inquire */
	typedef struct
	{
	    UINT8		function_code;
	    UINT8 		configType;
	    UltimaLaCfg	config;
	} UltimaLaInquire;

	/* Config */
	typedef struct
	{
	    UINT8		function_code;
	    UINT8		configType;
	    UltimaLaCfg	config;
	} UltimaLaConfig;

	/* Mode */
	typedef struct
	{
	    UINT8	function_code;
	    UINT8	mode;
	} UltimaLaMode;

	typedef struct 
	{
	    UINT8	function_code;
	    UINT8	station;
		UINT8	pad0;			// because in the Ultima World it is sixteen bits.
	    UINT8	test;
	    UINT16	result;
	    UINT16	parm1;
	    UINT16	parm2;
	} UltimaLaTest;

	typedef struct
	{
	    UINT8	function_code;
	    UINT8	station;
	    UINT8	package_id;
	    UINT8	pad0;
	} UltimaLaLabelReady;

	typedef struct
	{
	    UINT8	function_code;
	    UINT8	station;
	    UINT8	package_id;
	    UINT8	pad0;
	} UltimaLaLabelTaken;

	typedef struct
	{
	    UINT8	function_code;
	    UINT8	pad0;
	    UINT8	sensor_state[LastSensor];
	} UltimaLaSensors;

	typedef struct
	{
	    UINT8	function_code;
	    UINT8	pad0;
	    UINT16	pri_label_size;
	    UINT16	pri_label_rotation;
	    UINT16	sec_label_size;
	    UINT16	sec_label_rotation;
	} UltimaLaSetup;

	typedef struct
	{
	    UINT8	function_code;
	    UINT8	package_id;
	} UltimaLaPrint;

	typedef struct
	{
	    UINT8	function_code;
	    UINT8	package_id;
	} UltimaLaOpenWindow;

	typedef struct
	{
	    UINT8	function_code;
	    UINT8	package_id;
	} UltimaLaCloseWindow;

	typedef struct
	{
	    UINT8	function_code;
	    UINT8	package_id;
	} UltimaLaPackageReady;

	typedef struct
	{
	    UINT8	 	 	function_code;
	    UINT8		 	pad0;
	    SysStatusInfo	master_status;
	    SysStatusInfo	slave_1_status;
	    SysStatusInfo	slave_2_status;
	} UltimaLaSystemStatus;

#ifdef __cplusplus
	#pragma pack()
#endif

#endif // ULTIMALABELAPPLIERMESSAGES_H

