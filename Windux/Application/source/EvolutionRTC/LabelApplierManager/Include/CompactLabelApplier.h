/*******************************************************************************
*
*   Template Name			   Program Name				Program Assembly
*   cla.h						  CLA-II					   Unknown
*
********************************************************************************
*
*   COPYRIGHT (c) 1993, by  Hobart Corporation
*						   401 West Market Street
*						   Troy, Ohio 45374
*							(513) 332-3000
*
*   Author:	 Stan Swartzel
*			   Glen Griffiths
*   Created:	March 5, 1993
*
********************************************************************************
*
*   Description:
*	   This template contains definitions and data structures required for
*	   operation of the compact label applier software.
*
*------------------------------------------------------------------------------
*
*   Revision	Date of Change  Changed By  Description
*	--------	--------------  ----------  -----------------------------------
*	 1.00	   Mar 5, 1993	S Swartzel  Created
*
*****************************************************************************+*/
#ifndef _CLA
#define _CLA

//#include "hsystem.h"
//#ifndef M68HC11 
#define CLA_MASTER
//#endif 

//#ifdef M68HC11
//
//#define CLA_ATTACHED
//
/***********************/
/*  CLA Configuration  */
/***********************/
//#define CLA_CONFIG_EEP_START ((EEPOverlay *) EEPROM)->cla.block
//#define CLA_CONFIG_EEP_END (((EEPOverlay *) EEPROM)->cla.block + sizeof(CLA_CONFIGURATION))
//#define CLA_EEP_CHECKSUM (((EEPOverlay *) EEPROM)->cla.block + sizeof(CLA_CONFIGURATION) + 1)
//
//#endif /* M68HC11 */

typedef struct cla_config
{
	UINT8	has_weigher;
	UINT8	has_primary_printer;
	UINT8	has_secondary_printer;
	UINT8	left_to_right;		  /* package direction */
	UINT8	front_discharge;
	UINT8	primary_justify_front;
	UINT8	secondary_justify_front;
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
	UINT8	ministation_label_placement;	/* NEEP */
	UINT8   device_signature[DEVICE_SIG_NAME_LEN];  /* NEEP 041697 */
} CLA_CONFIGURATION;

/**/
/***********************/
/*  Sensor Structures  */
/***********************/
/*  Emitter Control  */
typedef enum _emitter_id
{
	WEIGHER_ENTRY_EMITTER,
	PR_1_ENTRY_EMITTER,
	PR_1_RIGHT_EMITTER,
	PR_1_LEFT_EMITTER,
	PR_2_ENTRY_EMITTER,
	PR_2_RIGHT_EMITTER,
	PR_2_LEFT_EMITTER,
	LAST_EMITTER
} EMITTER_ID;

/* Note: Do Not Change the order of the PR_1 and PR_2 sensors  */
/* Before adding a new sensor examine function ProcessLOUpdateSensor  */
/* and function BuildLOEmitter in module mcla_msg.c. */

/*  Sensor Control  */
typedef enum _sensor_id
{
	EXPECT_PACKAGE,
	WEIGHER_ENTRY,
	PR_1_ENTRY,
	PR_1_GATE,
	PR_1_LABEL_POSITION,
	PR_1_VALID,
	PR_2_ENTRY,
	PR_2_GATE,
	PR_2_LABEL_POSITION,
	PR_2_VALID,
	LAST_SENSOR
} SENSOR_ID;

typedef enum printer_arm
{
	EITHER_ARM,
	PRIMARY_ARM,
	SECONDARY_ARM
} PRINTER_ARM;


#define EXPECT_PACKAGE_BIT	  0x0001
#define WEIGHER_ENTRY_BIT	   0x0002
#define PR_1_ENTRY_BIT		  0x0004
#define PR_1_GATE_BIT		   0x0008
#define PR_1_LABEL_POSITION_BIT 0x0010
#define PR_1_VALID_BIT		  0x0020
#define PR_2_ENTRY_BIT		  0x0004
#define PR_2_GATE_BIT		   0x0008
#define PR_2_LABEL_POSITION_BIT 0x0010
#define PR_2_VALID_BIT		  0x0020


/* Transfer Station Types */
#define NSW		 0x00
#define HANDWRAPPER 0x01
#define BAKERY	  0x02
#define ESW		 0x03
#define MINI		0x04			/* NEEP */


/* Ministation Label Placement */
#define LEADING_EDGE  0x00		  /* NEEP */
#define TRAILING_EDGE 0x01		  /* NEEP */


/* Motor Direction */
#define FORWARD 0
#define REVERSE 1


#ifdef M68HC11
/* Status bits for system status and blink codes  */
#define  CLA_HIGH_VOLTAGE_OFF		   0x0001
#define  CLA_PRINTER_1_ARM_OUT		  0x0002
#define  CLA_PRINTER_2_ARM_OUT		  0x0004
#define  CLA_WEIGHER_MOTOR_FAIL		 0x0008
#define  CLA_PRINTER_1_MOTOR_FAIL	   0x0010
#define  CLA_PRINTER_2_MOTOR_FAIL	   0x0020
#define  CLA_PRINTER_1_NO_WAKEUP		0x0040
#define  CLA_PRINTER_2_NO_WAKEUP		0x0080
#define  CLA_PACKAGE_OVERRUN			0x0100
#define  CLA_SPI_ERRORS_SLAVE_1		 0x0200
#define  CLA_SPI_ERRORS_SLAVE_2		 0x0400
#define  CLA_BAD_MOTOR_CODE_WEIGHER	 0x0800
#define  CLA_BAD_MOTOR_CODE_PRINTER_1   0x1000
#define  CLA_BAD_MOTOR_CODE_PRINTER_2   0x2000
#define  CLA_MODE_OP_ERROR			  0x4000
#define  CLA_AVAILABLE				  0x8000
#endif /* M68HC11 */

/* Status bits for status message to Ultima */
#define  CLA_HIGH_VOLTAGE_ERROR		 0x0001
#define  CLA_PRINTER1_ARM_OUT_ERROR	 0x0002
#define  CLA_PRINTER2_ARM_OUT_ERROR	 0x0004
#define  CLA_PRINTER1_NO_WAKEUP_ERROR   0x0008
#define  CLA_PRINTER2_NO_WAKEUP_ERROR   0x0010
#define  CLA_WEIGHER_HARDWARE_ERROR	 0x0020
#define  CLA_PRINTER1_HARDWARE_ERROR	0x0040
#define  CLA_PRINTER2_HARDWARE_ERROR	0x0080
#define  CLA_PACKAGE_OVERRUN_ERROR	  0x0100
#define  CLA_MODE_ERROR				 0x0200
#define  CLA_RESERVED1_ERROR			0x0400
#define  CLA_RESERVED2_ERROR			0x0800


#ifdef M68HC11

typedef struct _sensor_ctrl
{
	SENSOR_ID   sensor;				 /*  What Sensor Is This  */
	UINT8		output_state;			/*  Debounced Sensor State  */
	UINT8		local_remote_flag;	  /*  Is Sensor Local To uProcessor  */
	UINT8		send_sensor_updates;	/*  Are Sensor Updates to Remote Processor Expected  */
	UINT8		interrupt_available;	/*  Is Sensor Interrupt Driven  */
	UINT8		sensor_changed;		 
	UINT8	  (*read_sensor) ();		/*  Function To Read Sensor  */

										/*  Polled Sensor Variables  */
	UINT16		high_counts;			/*  Total Counts Since Sensor Went High  */
	UINT16		low_counts;			 /*  Total Counts Since Sensor Went Low  */
	UINT8		last_sensor_reading;	/*  Last Real Sensor Reading  */
	UINT8		readings_at_state;	  /*  Consecutive Readings At This State  */
	UINT8		read_count_threshhold;  /*  Consecutive Readings Required To Change State  */
	
										/*  Interrupt Sensor Variables  */
	UINT8		holdoff_time;			/*  Holdoff Delay in Interrupt Driven System  */
	TimerEvent  timer;				  /*  Timer For Interrupt Driven Sensor  */
} SENSOR_CTRL;

#endif /* M68HC11 */

/**/
/************************************/
/*  Master/Slave Command Interface  */
/************************************/
typedef enum cla_command_id
{
	CLA_NO_OP,
	CLA_GET_PACKAGE,
	CLA_JUSTIFY_PACKAGE,
	CLA_ALIGN_PACKAGE,
	CLA_APPLY_LABEL,
	CLA_DISCHARGE_PACKAGE,
	CLA_COMPRESSOR_OFF,
	CLA_GET_LABEL,
	CLA_TEST_ROTATION_SOLONOID
} CLA_COMMAND_ID;

/* Status message response codes to a command message */
typedef enum cla_command_status
{
	CLA_IDLE,
	CLA_BUSY,
	CLA_HAS_ERROR
} CLA_COMMAND_STATUS;

typedef enum _cla_station_id
{
	WEIGHER_STATION,
	PRI_PRINTER_STATION,
	SEC_PRINTER_STATION
} CLA_STATION_ID;

typedef enum _cla_command_type
{
	CLA_LEFT,
	CLA_RIGHT,
	CLA_FRONT,
	CLA_BACK,
	CLA_MFG_TEST,
	CLA_LEFT_SIM,
	CLA_RIGHT_SIM,
	CLA_FRONT_SIM,
	CLA_BACK_SIM,
	CLA_LEFT_TRAIL,
	CLA_RIGHT_TRAIL,
	CLA_LEFT_P2,
	CLA_RIGHT_P2,
	CLA_LEFT_TRAIL_P2,
	CLA_RIGHT_TRAIL_P2,
	CLA_LEFT_FRONT,
	CLA_RIGHT_FRONT,
	CLA_RIGHT_BAKERY,
	CLA_LEFT_BAKERY,
	CLA_LEFT_ESW,
	CLA_RIGHT_ESW,
	CLA_DIAG_APPLY,
	CLA_MINI,			  /* NEEP */
	CLA_LEFT_MINI,		 /* NEEP */
	CLA_RIGHT_MINI		 /* NEEP */
} CLA_COMMAND_TYPE;

typedef struct _cla_status_info
{
	CLA_COMMAND_STATUS  weigher_command_status;
	CLA_COMMAND_STATUS  printer_command_status;
	CLA_COMMAND_ID	  weigher_command;
	CLA_COMMAND_ID	  printer_command;
	UINT32			   error_status;
} CLAStatusInfo;

typedef enum _cla_test_id
{
	CLA_NO_SERVICE_TEST,	//Alex Zavala 9/3
	CLA_TEST_ENABLE_PRINTER_MOTOR,
	CLA_TEST_ENABLE_WEIGHER_MOTOR,
/*	CLA_TEST_DISABLE_PRINTER_MOTOR,
	CLA_TEST_DISABLE_WEIGHER_MOTOR,
	CLA_TEST_BRAKE_WEIGHER_MOTOR,
	CLA_TEST_BRAKE_PRINTER_MOTOR,
	CLA_TEST_RELEASE_WEIGHER_MOTOR,
	CLA_TEST_RELEASE_PRINTER_MOTOR,
	CLA_TEST_TORQUE_TO_WEIGHER,
	CLA_TEST_TORQUE_TO_PRINTER,
*/	CLA_TEST_ENABLE_COMPRESSOR,
	CLA_TEST_DISABLE_COMPRESSOR,
	CLA_TEST_GATE_UP,
	CLA_TEST_GATE_DOWN,
	CLA_TEST_ACTIVATE_BELTS,
	CLA_TEST_ACTIVATE_ROLLERS,
	CLA_TEST_ACTIVATE_WAND,
	CLA_TEST_RELEASE_WAND,
	CLA_TEST_ACTIVATE_APPLIER,
	CLA_TEST_RELEASE_APPLIER,
/*	CLA_TEST_ROTATE_WAND,
	CLA_TEST_ENABLE_DRIVE_SIGNALS,
	CLA_TEST_DISABLE_DRIVE_SIGNALS,
	CLA_TEST_SLAVE_INPUT,
	CLA_TEST_SLAVE_OUTPUT,
*/	CLA_TEST_ENABLE_WAND_VACUUM,			/* MLA Only */
	CLA_TEST_DISABLE_WAND_VACUUM			/* MLA Only */
	,CLA_TEST_RESET_MECHS//Alex Zavala 9/3
	,CLA_LAST_SERVICE_TEST//Alex Zavala 9/3
} CLA_TEST_ID;


//	Messages to Manager and beyond.

	/* Status */
	typedef struct
	{
	    UINT8	function_code;
	    UINT8	pad0;
	    UINT16	status;
	} CLAStatus;

	/* Inquire */
	typedef struct
	{
	    UINT8		function_code;
	    UINT8 		configType;
	    UltimaLaCfg	config;
	} CLAInquire;


	typedef struct
	{
	    UINT8	function_code;
	    UINT8	pad0;
	    UINT8	sensor_state[LastSensor];
	} CLASensors;

	typedef struct
	{
	    UINT8	function_code;
	    UINT8	package_id;
	} CLAPrint;

	typedef struct
	{
	    UINT8	 	 	function_code;
	    UINT8		 	pad0;
	    SysStatusInfo	master_status;
	    SysStatusInfo	slave_1_status;
	    SysStatusInfo	slave_2_status;
	} CLASystemStatus;

#ifdef M68HC11

/**/
/*******************************/
/*  Master Control Structures  */
/*******************************/
#ifdef CLA_MASTER

#define ONESEC  1000
#define ONEANDHALFSEC   1500
#define TWOSEC  2000
#define THREESEC 3000
#define FIVESEC 5000
#define TENSEC  10000
#define FIFTEENSEC  15000
#define TWENTYSEC 20000
#define FORTYFIVESEC	45000
#define MAX_NUMBER_OF_STATIONS 4
#define MAXREQUESTS 30		  /* Max Low Order output message requests */
#define MAX_PACKAGES	6
#define WAIT_FOR_ROLLERS_DELAY 15
#define WAIT_FOR_BELTS_DELAY 5
#define TWO_AND_ONE_HALF_VOLTS 127	  /* 2.5 * 255 */
#define TWO_VOLTS 51					/* 2   * 255 */

#if 0
/* .9 inch * 152 dots per inch = 137 * 2 = 274*/
#define WAND_LABEL_OFFSET   274
#endif

#define DOTS_PER_TICK 18

#define CONVEYOR_SENSITIVITY_LOW	0x10
#define CONVEYOR_SENSITIVITY_MEDIUM 0x38
#define CONVEYOR_SENSITIVITY_HIGH   0x60
#define WEIGHER_SENSITIVITY_DEFAULT CONVEYOR_SENSITIVITY_MEDIUM
#define PRINTER_SENSITIVITY_DEFAULT CONVEYOR_SENSITIVITY_HIGH

#define MAX_CLA_LABEL_OFFSET			30
#define DEFAULT_CLA_LABEL_OFFSET		 8
#define MAX_CLA_JUSTIFY_DISTANCE	   200
#define DEFAULT_CLA_JUSTIFY_DISTANCE	75
#define MAX_CLA_DIAG_OFFSET			 15
#define DEFAULT_CLA_DIAG_OFFSET		  0

/* Discharge delay for printer in sim mode */
#define P_DISCHARGE_TIME	750 

/* Discharge delay for weigher in sim mode */
#define W_DISCHARGE_TIME	2000

/* Discharge delay for xfer in auto mode, & conveyor mode for NSW */
#define T_WAIT_2_DISCHARGE_TIME 100 

/* Discharge delay for xfer in auto mode, & conveyor mode for Hand Wrapper */
#define T_HAND_DISCHARGE_TIME 500 

/* Discharge delay for xfer in auto mode, & conveyor mode for ESW */
#define T_ESW_DISCHARGE_TIME 250 



#endif /* CLA_MASTER */
#endif /* M68HC11 */

#ifdef CLA_MASTER

/* CLA Operation Modes */
typedef enum cla_opmode
{
	MANUAL,
	SEMI_AUTO,
	AUTO,
	CONVEYOR,
	SIMULATION,
	CSERVICE
} CLA_OPMODE;

#endif /* CLA_MASTER */

#ifdef M68HC11
#ifdef CLA_MASTER

typedef enum cla_init_state
{
	RESET,
	WAIT4WAKEUPS,
	WAIT4SLAVE,
	ENABLE_P1RIGHT_EMITTER,
	READ_P1RIGHT_SENSOR,
	ENABLE_P1LEFT_EMITTER,
	READ_P1LEFT_SENSOR,
	ENABLE_P2RIGHT_EMITTER,
	READ_P2RIGHT_SENSOR,
	ENABLE_P2LEFT_EMITTER,
	READ_P2LEFT_SENSOR,
	PROGRAM_WEIGHER,
	PROGRAM_PRI_PRINTER1,
	PROGRAM_PRI_PRINTER2,
	PROGRAM_SEC_PRINTER1,
	PROGRAM_SEC_PRINTER2,
	INIT,
	RUN,
	WAITING_FOR_PRINTER_ARM_1,
	WAITING_FOR_PRINTER_ARM_2,
	SEND_P1RIGHT_OFF,
	SEND_P1LEFT_OFF,
	SEND_P2RIGHT_OFF,
	SEND_P2LEFT_OFF,
	CALIBRATE_PRIMARY_SENSORS_1ST_PASS,
	CALIBRATE_PRIMARY_SENSORS_2ND_PASS,
	CALIBRATE_SECONDARY_SENSORS_1ST_PASS,
	CALIBRATE_SECONDARY_SENSORS_2ND_PASS 
} CLA_INIT_STATE;


typedef enum request_status
{
	ACTIVE,
	INACTIVE
} REQUEST_STATUS;

/* Station Type and Attributes */
typedef struct cla_station_type
{
	UINT16	xfer_station	:1;
	UINT16	weigher_station :1;
	UINT16	printer_station :1;
	/* Attributes */
	UINT16	leading_edge	:1;
	UINT16	trailing_edge   :1;
	UINT16	side_front	  :1;
	UINT16	front_justify   :1;
	UINT16	back_justify	:1;
} CLA_STATION_TYPE;

typedef struct cla_station_flags
{
	UINT8	station_valid			:1; /* Station Valid, Slaves Sent Wakeup */
	UINT8	sensor_broken			:1; /* Next Station Entry Sensor Broken */
	UINT8	emitter_timer_expired   :1;
	UINT8	close_win_rcvd		  :1; /* Received close window from ultima */
} CLA_STATION_FLAGS;

typedef enum cla_station_state
{
	WAITING_FOR_PACKAGE,
	START_GETTING_PACKAGE,
	GETTING_PACKAGE,
	HAVE_PACKAGE,
	WEIGHING_PACKAGE,
	WAITING_TO_DISCHARGE,
	WAITING_FOR_TIMER,
	WAITING_FOR_COMMAND_COMPLETE,
	START_DISCHARGING_PACKAGE,
	DISCHARGING_PACKAGE,
	PACKAGE_DISCHARGED,

	P_START_JUSTIFYING_PACKAGE,
	P_JUSTIFYING_PACKAGE,
	P_PACKAGE_JUSTIFIED,
	P_START_ALIGNING_PACKAGE,
	P_ALIGNING_PACKAGE,
	P_PACKAGE_ALIGNED,
	P_LABEL_AVAILABLE,
	P_START_APPLYING_LABEL,
	P_APPLYING_LABEL
} CLA_STATION_STATE;

typedef struct cla_package_control
{
	VOID	*station_id;
	UINT16	p1_offset;
	UINT16	p2_offset;
	UINT16	p1_rotation;
	UINT16	p2_rotation;
	UINT8	p1_label_available;
	UINT8	p2_label_available;
	UINT8	get_p1_label_sent;
	UINT8	get_p2_label_sent;
	UINT8	force_package;
	UINT8	close_win_received;
	UINT8	package_id;
} CLA_PACKAGE_CONTROL;

/* Station Control Structure */
typedef struct cla_station_control
{
	struct cla_station_control *next_station;
	struct cla_station_control *prev_station;
	CLA_PACKAGE_CONTROL		*package;
	CLA_STATION_TYPE			type;
	UINT8						spi_address;
	CLA_STATION_FLAGS		   flags;
	CLA_STATION_STATE		   state;
	SENSOR_ID				   entry_sensor;
	UINT16						waitcounter;
} CLA_STATION_CONTROL;

typedef struct message_request
{
	struct message_request  *next;
	UINT8					slave;
	REQUEST_STATUS		  status;
	CLA_STATION_ID		  device;
	UINT8					function;
	CLA_COMMAND_ID		  command;
	UINT8					parm1;
	UINT8					parm2;
	UINT16					parm3;
	UINT16					parm4;
} MESSAGE_REQUEST;

typedef struct cla_system_events
{
	UINT8	Pr1CompressorOn		 :1;
	UINT8	Pr2CompressorOn		 :1;
	UINT8	wakeup_timer_expired	:1;
	UINT8	activity_timer_expired  :1;
} CLA_SYSTEM_EVENTS;

typedef struct cla_system_control
{
	CLA_STATION_CONTROL *station;		/* SLL to Station Control Blocks */
	CLA_STATION_CONTROL *WeighStation;  /* Saved addr of Weigher Station */
	CLA_STATION_CONTROL *Prt1Station;	/* Saved addr of Primary Printer */
	CLA_STATION_CONTROL *Prt2Station;	/* Saved addr of Secondary Printer */
	CLA_OPMODE		  mode;			/* CLA Current Mode, RUN or Other  */
	CLA_OPMODE		  last_mode;	  /* last mode message mode  */
	CLA_OPMODE		  default_mode;	/* Default mode via  activity timeout */
	CLA_INIT_STATE	  state;		  /* Current Processing State	 */
	CLA_PACKAGE_CONTROL default_package; /* Default label rotation & size */
	MESSAGE_REQUEST	 *ho_request;	/* SLL for inter module message req */
	UINT8				mfg_test;		/* State of Manufactures test jumper  */
										/* at initialization */
	MESSAGE_REQUEST	 *request;		/* SLL for internal msg man requests*/
	TimerEvent		  activity_timer; /* Timer to default to default mode */
	TimerEvent		  compressor_timer; /* compressor off after no activity */
	CLA_SYSTEM_EVENTS   events;		 /* events bit mask */
	UINT8				station_cnt;	/* number of stations */
	UINT16				mfg_rotation;	/* wand rotation in mfg test */
	UINT16				error_status;	/* system error status word */
	UINT8				ForcePackageSemaphore;  /* if > 0, force the package */
	UINT8				package_id;	 /* used to match open win,close win */
										/* & label ready to proper package */
	TimerEvent		  get_package_timer; /* prt1 get package timer */
} CLA_SYSTEM_CONTROL;

#endif /* CLA_MASTER */


/**/
/******************************/
/*  Slave Control Structures  */
/******************************/
#ifdef CLA_SLAVE

#define MAX_CLA_COMMANDS_PER_OP 8

/*******************/
/*  Motor Control  */
/*******************/

/*  Assumptions:   7FFF Internal Velocity Units ==> 5000 Electrical Revs per Minute
**				  Counter Period (7.3728MHz and Timer Prescaler of 4 ==> 2.170 uS
**				   At 5000 Electrical Revs per Minute, Minimum Hall Change Period ==> 2.0 mS
*/

#define MOTOR_VELOCITY_CONSTANT	 0x1CCFC66
#define MOTOR_CONTROL_TOC_CONSTANT  4608
#define MOTOR_POSITION_TOC_CONSTANT 3225

#define BAD_HALL_CODE_THRESHOLD 1000

#define NO_MOTOR_TICKS	  ((256/6)*0)
#define ONE_MOTOR_TICK	  ((256/6)*1)
#define TWO_MOTOR_TICKS	 ((256/6)*2)
#define THREE_MOTOR_TICKS   ((256/6)*3)
#define FOUR_MOTOR_TICKS	((256/6)*4)
#define FIVE_MOTOR_TICKS	((256/6)*5)

typedef enum _motor_command_type
{
	NO_COMMAND,
	TO_POS,
	TO_VEL,
	TO_POS_INDIRECT,
	TO_VEL_INDIRECT
} MOTOR_COMMAND_TYPE;

typedef enum _term_type
{
	TO_POSITION,
	TO_VELOCITY,
	TO_TIME,
	TO_SENSOR_BLOCKED,
	TO_SENSOR_NOT_BLOCKED,
	TO_SENSOR_BLOCKED_OR_DIST,
	TO_DIST,
	TO_COUNTDOWN,
	TO_SENSOR_NOT_BLOCKED_OR_DIST,
	TO_DIST_PARM,
	TO_SENSOR_NOT_BLOCKED_OR_DIST_PARM,
	TO_TIME_PARM
} TERM_TYPE;

typedef struct _term_cond
{
	TERM_TYPE   type;
	BYTE		target;
	UINT8		return_offset;
} TERM_COND;

typedef struct _motor_command
{
	MOTOR_COMMAND_TYPE  command_type;		  /*  Motor Command Variables  */
	TERM_TYPE		   termination_type;
	
	BYTE			  (*control_engine)();
	BYTE			  (*test_for_done)();

	WORD				 target_position;
	WORD				 target_velocity;
	WORD				 target_time;
	SENSOR_ID		   target_sensor;
	SENSOR_CTRL		 *target_sensor_addr;

	WORD				 velocity_limit;
	WORD				 acceleration_limit;
	WORD				 torque_limit;
	WORD				 torque_change_limit;
	
	WORD				 position_loop_p;
	WORD				 position_loop_i;
	WORD				 position_loop_d;
	
	WORD				 velocity_loop_p;
	WORD				 velocity_loop_i;
	WORD				 velocity_loop_d;

	UINT8				gate_on_entry;
	UINT8				table_on_entry;
	UINT8				wand_on_entry;
	UINT8				applier_on_entry;
	UINT8				compressor_on_entry;
	UINT8				rotator_on_entry;

	UINT8				gate_on_exit;
	UINT8				table_on_exit;
	UINT8				wand_on_exit;
	UINT8				applier_on_exit;
	UINT8				compressor_on_exit;
	UINT8				rotator_on_exit;
} MOTOR_COMMAND;

typedef struct _motor_control
{
	UINT8				active;
	CLAStatusInfo	   status;
	UINT8				error;
	UINT8				last_error;				 /*  Motor Feedback Variables  */
	UINT8				operation_complete;
	UINT8				motor_direction;
	UINT8				last_motor_direction;
	WORD				 motor_position;			 /*  High Byte - Rev Count  */
													/*  Low Byte  - Angular Position  */
	WORD				 motor_velocity;			 /*  0x7FFF = 7000 Electrical RPM  */
	WORD				 motor_acceleration;		 /*  0x7FFF = 7000 Electrical RPM per second  */

	WORD				 last_motor_position;		
	WORD				 last_motor_velocity;		
	WORD				 last_motor_acceleration;	

	WORD				 command_velocity;
	WORD				 command_acceleration;
	
	UINT8			  (*get_hall)();				
	UINT16			  (*get_timer)();
	VOID			  (*send_torque)();
	UINT8				last_hall_effect_pos;		
	UINT8				this_hall_effect_vel;		
	UINT8				last_hall_effect_vel;
	UINT16				velocity_sample_start_time; 
	UINT16				velocity_sample_overflow;
	UINT8				commutation_count;
	UINT16				bad_hall_count;

	UINT32				velocity_sample_time;
	UINT8				velocity_commutation_counts;
	
	CLA_COMMAND_ID	  command_id;
	CLA_COMMAND_STATUS  command_status;	
	WORD				 command_rotation;
	WORD				 command_parameter;
	MOTOR_COMMAND	  *command;
	UINT16				command_closures;

	WORD				 position_error;		 /*  Motor Loop Closure Variables  */
	WORD				 velocity_error;
	WORD				 acceleration_error;
	
	WORD				 position_int;
	WORD				 velocity_int;
	WORD				 acceleration_int;

	WORD				 high_res_torque;
	BYTE				calculated_torque;
	BYTE				commanded_torque;
	
	BYTE				countdown;
} MOTOR_CONTROL;

#define LOOPS_PER_SECOND			100

#define WEIGHER_1_IPS			   0x01F4
#define WEIGHER_1_IPSS			 (WEIGHER_1_IPS / LOOPS_PER_SECOND)
#define WEIGHER_ACCELERATION		((LONG)386 * (LONG)WEIGHER_1_IPS / (LONG) LOOPS_PER_SECOND)
#define WEIGHER_GAIN				0x0020

#define MAX_WEIGHER_VELOCITY		0x7FFF
#define MAX_WEIGHER_TORQUE_CHANGE   0x7FFF
#define MAX_WEIGHER_ACCELERATION	0x7FFF
#define MAX_WEIGHER_TORQUE		  0x7FFF

#define PRINTER_1_IPS			   0x022C
#define PRINTER_1_IPSS			 (PRINTER_1_IPS / LOOPS_PER_SECOND)
#define PRINTER_ACCELERATION	  ((LONG) 386 * (LONG) PRINTER_1_IPS / (LONG) LOOPS_PER_SECOND)
#define PRINTER_GAIN				0x0030
#define PRINTER_ROLLER_1_IPS		0x033C
#define PRINTER_ROLLER_ACCELERATION	 ((LONG) 386 * (LONG) PRINTER_ROLLER_1_IPS / (LONG) LOOPS_PER_SECOND)

#define MAX_PRINTER_VELOCITY		0x7FFF
#define MAX_PRINTER_TORQUE_CHANGE   0x7FFF
#define MAX_PRINTER_TORQUE		  0x7FFF
#define MAX_PRINTER_ACCELERATION	0x7FFF

#endif /* CLA_SLAVE */
#endif /* M68HC11 */

/**/
/******************************************************************/
/*  Message Function Codes for High Order and Low Order Messages  */
/******************************************************************/
//typedef enum cla_function_code
//{
//	CLA_WAKEUP,
//	CLA_CONFIGURE,
//	CLA_INQUIRE,
//	CLA_RESET,
//	CLA_STATUS,
//	CLA_ENABLE,
//	CLA_DISABLE,
//	CLA_MODE,
//	CLA_TEST,
//	CLA_OPEN_WINDOW,
//	CLA_CLOSE_WINDOW,
//	CLA_LABEL_READY,
//	CLA_GET_SENSOR,
//	CLA_SETUP = 14,
//	CLA_OFF,
//	CLA_DEFINE,				/* Internal Only */
//	CLA_EXECUTE,			/* Internal Only */
//	CLA_UPDATE_SENSOR,		/* Internal Only */
//	CLA_EMITTER,			/* Internal Only */
//	CLA_ENABLE_SENSOR,		/* Internal Only */
//	CLA_SYSTEM_STATUS,
//	CLA_PACKAGE_READY,		/* NEEP */
//	CLA_LABEL_TAKEN,		/* NEEP */
//	CLA_ON,					/* NEEP */	
//	CLA_PRINT_PKG = 32,
//	CLA_ULTIMA_AUTO_MODE	/*this function code forces the ultima to switch to a auto mode	*/
//							/*before the window open command is sent*/
//} CLA_FUNCTION_CODE;

/* The following messages are used for High Order & Low Order */

/* CLA System Status */

typedef struct cla_sys_status
{
	UINT8   function_code;
	UINT8			   pad0;
	SysStatusInfo   master_status;
	SysStatusInfo   slave_1_status;
	SysStatusInfo   slave_2_status;
}  CLASysStatus;

/********************************************/
/*  High Order Messages From/To The Ultima  */
/********************************************/
#ifdef CLA_MASTER

typedef struct open_window_msg
{
	UINT8   function_code;
	UINT8				package_id;
} CLAOpenWindowMsg;

typedef struct close_window_msg
{
	UINT8   function_code;
	UINT8				package_id;
} CLACloseWindowMsg;

typedef struct label_ready_msg
{
	UINT8   function_code;
	CLA_STATION_ID	  station;
	UINT8				package_id;
	UINT8				pad;
} CLALabelReadyMsg;

typedef struct get_sensor_msg
{
	UINT8   function_code;
	UINT8				pad;
	UINT8				sensor_state[LAST_SENSOR];
} CLAGetSensorMsg;

typedef struct cla_setup_msg
{
	UINT8   function_code;
	UINT8				pad;
	UINT16				pri_label_size;
	UINT16				pri_label_rotation;
	UINT16				sec_label_size;
	UINT16				sec_label_rotation;
} CLASetupMsg;

typedef struct cla_off_msg
{
	UINT8   function_code;
	UINT8				pad;
} CLAOffMsg;

typedef struct cla_print_msg
{
	UINT8   function_code;
	UINT8				package_id;
} CLAPrintMsg;

typedef struct cla_emitter_msg
{
	UINT8   function_code;
	UINT8				pad;
	EMITTER_ID		  emitter_id;
	UINT8				state;
} CLAEmitterMsg;

/* NEEP new message type */
typedef struct package_ready_msg
{
	UINT8   function_code;
	UINT8				package_id;
} CLAPackageReadyMsg;

typedef struct label_taken_msg
{
	UINT8   function_code;
	CLA_STATION_ID	  station;
	UINT8				package_id;
	UINT8				pad;
} CLALabelTakenMsg;

#endif   /* CLA_MASTER */

/**/
/*************************************************/
/*  Low Order Messages Between Master and Slave  */
/*************************************************/

#ifdef M68HC11

typedef struct cla_slave_generic_msg
{
	UINT8   function_code;
} GenericSlaveMsg;

typedef struct wakeup_slave
{
	UINT8   function_code;
	UINT8				ready_to_run;
	SysStatusInfo	   status;
} WakeupSlaveMsg;

typedef struct config_slave_msg
{
	UINT8   function_code;
	UINT8				disposition;			/* write to EEP */
	CLA_CONFIGURATION   config;
} ConfigureSlaveMsg;

typedef struct reset_slave
{
	UINT8   function_code;
} ResetSlaveMsg;

typedef struct status_slave
{
	UINT8   function_code;
	CLAStatusInfo	   status;
} SlaveStatusMsg;

typedef struct command_msg
{
	UINT8   function_code;
	CLA_COMMAND_ID	  command;
	CLA_STATION_ID	  station;
	VOID				*parameter;
} SlaveExecuteMsg;

typedef struct update_sensor_msg
{
	UINT8   function_code;
	SENSOR_ID		   sensor;
	UINT8				output_state;
	UINT16				high_counts;
	UINT16				low_counts;
} UpdateSensorMsg;

typedef struct enable_sensor_msg
{
	UINT8   function_code;
	UINT8				sensor_enables;	
} EnableSensorMsg;

typedef struct emitter_msg
{
	UINT8   function_code;
	EMITTER_ID		  emitter;
	UINT8				state;
} EmitterMsg;

typedef struct define_command_msg
{
	UINT8   function_code;
	CLA_STATION_ID	  station;
	CLA_COMMAND_ID	  command;
	CLA_COMMAND_TYPE	type;
	UINT8				p_gain;	 /* proportional gain for PID loop */
} DefineMsg;

union _cla_slave_msg
{
	GenericSlaveMsg	 generic;
	ConfigureSlaveMsg   config;
	WakeupSlaveMsg	  wakeup;
	ResetSlaveMsg	   reset;
	SlaveStatusMsg	  status;
	SlaveExecuteMsg	 execute;
	UpdateSensorMsg	 sensor;
	EmitterMsg		  emitter;
	DefineMsg		   define;
	EnableSensorMsg	 enable_sensor;
} CLA_SLAVE_MSG;

/**/
/******************************************/
/*  MASTER PROCESSOR FUNCTION PROTOTYPES  */
/******************************************/

#ifdef CLA_MASTER
/*  From mcla_mgr.c  */
CommHeader *InitializeCLAHardware(VOID);
CommHeader *CLAManager( CommHeader *message );
VOID DoInternalProcessing(VOID);
VOID ProcessStations(VOID);
VOID ProcessAutoStation( CLA_STATION_CONTROL *pStation );
VOID ProcessSemiAutoStation( CLA_STATION_CONTROL *pStation );
VOID ProcessManualStation( CLA_STATION_CONTROL *pStation );
VOID ProcessSimStation( CLA_STATION_CONTROL *pStation );
VOID ProcessConStation( CLA_STATION_CONTROL *pStation );
VOID ProcessAutoHWTransfer( CLA_STATION_CONTROL *pStation );
VOID ProcessAutoNSWTransfer( CLA_STATION_CONTROL *pStation );
VOID ProcessAutoWeigher( CLA_STATION_CONTROL *pStation );
VOID ProcessSemiAutoWeigher( CLA_STATION_CONTROL *pStation );
VOID ProcessSimWeigher( CLA_STATION_CONTROL *pStation );
VOID ProcessAutoPrinter( CLA_STATION_CONTROL *pStation );
VOID ProcessSemiAutoPrinter( CLA_STATION_CONTROL *pStation );
VOID ProcessSimPrinter( CLA_STATION_CONTROL *pStation );
VOID ProcessConTransfer( CLA_STATION_CONTROL *pStation );
VOID ProcessConWeigher( CLA_STATION_CONTROL *pStation );
VOID ProcessConPrinter( CLA_STATION_CONTROL *pStation );
BOOL SetDischargingPackage( CLA_STATION_CONTROL *pStation );
BOOL SetWaitingForPackage( CLA_STATION_CONTROL *pStation );
BOOL SetWaitingToDischarge( CLA_STATION_CONTROL *pStation );
BOOL DischargeFromPrinter( CLA_STATION_CONTROL *pStation );
VOID DoCLAStartUp(VOID);
VOID InitCLAData(VOID);
BOOL CreatePackage( CLA_STATION_CONTROL *pStation );
VOID DestructPackage( CLA_STATION_CONTROL *pStation );
VOID GetIncomingPackage( CLA_STATION_CONTROL *pStation );
BOOL WakeupTimeout(VOID);
BOOL CheckPrinterArmPos( PRINTER_ARM Arm );
MESSAGE_REQUEST *GetMessageRequest(VOID);
VOID EmitterOn( EMITTER_ID emitter );
VOID EmitterOff( EMITTER_ID emitter );
BOOL EmitterTimeout( CLA_STATION_CONTROL *pStation );
VOID ProgramSensors( UINT8 bits );
VOID ProgramWeigher(VOID);
VOID ProgramPrinterFunctions1( UINT8 slave );
VOID ProgramPrinterFunctions2( UINT8 slave );
VOID LocalSensorManager(VOID);
BOOL GetPackageTimeout(VOID);
BOOL ActivityTimeout(VOID);
VOID IdleSystem(VOID);
BOOL CompressorTimeout(VOID);
VOID SendGetLabel( CLA_STATION_CONTROL *pStation );
VOID ResetStateMachines(VOID);
BOOL CheckForLabelReady( CLA_STATION_CONTROL *pStation );
VOID SendTestMessage( UINT8 slave, CLA_TEST_ID test );
VOID SystemStatusManager(VOID);
VOID CalibrateSensors(BYTE slave);

/*  From mcla_msg.c  */
VOID InternalMessageManager(VOID);
CommHeader *OutputHOMessages(VOID);
CommHeader *InterModuleMessageManager( CommHeader *message );
VOID ProcessHOWakeup(CommHeader *message);
VOID ProcessHOConfig(CommHeader * message );
VOID ProcessHOInquire(CommHeader* message );
VOID ProcessHOReset(CommHeader *message);
VOID ProcessHOStatus( CommHeader *message );
VOID ProcessHOEnable( CommHeader *message );
VOID ProcessHODisable( CommHeader *message );
VOID ProcessHOMode( CommHeader *message );
VOID ProcessHOTest( CommHeader *message );
VOID ProcessHOCloseWindow( CommHeader *message );
VOID ProcessHOLabelReady( CommHeader *message );
VOID ProcessHOPackageReady( CommHeader *message );
VOID ProcessHOLabelTaken( CommHeader *message );
VOID ProcessHOGetSensors(CommHeader *message);
VOID ProcessHOSetup( CommHeader *message );
VOID ProcessHOSysStatus(CommHeader *message);
CommHeader *BuildHOStatus(VOID);
CommHeader *BuildHOWakeup(VOID);
CommHeader *BuildHOGetSensor(VOID);
CommHeader *BuildHOTest( MESSAGE_REQUEST *pRequest );
CommHeader *BuildHOInquire(VOID);
CommHeader *BuildHOOpen(BYTE package_id);
CommHeader *BuildHOOff(VOID);
CommHeader *BuildHOClose(VOID);
CommHeader *BuildHOPrint(VOID);
CommHeader *BuildHOSysStatus(VOID);
VOID ProcessInternalMess( CommHeader *message );
VOID ProcessLOWakeup( CommHeader *message );
VOID ProcessLOStatus( CommHeader *message );
VOID ProcessLOUpdateSensor( CommHeader *message );
VOID ProcessLOTest( CommHeader *message );
CommHeader *BuildLOWakeup( UINT8 slave );
CommHeader *BuildLOReset( UINT8 slave );
CommHeader *BuildLOStatus( UINT8 slave );
CommHeader *BuildLOCommand( MESSAGE_REQUEST *pRequest );
CommHeader *BuildLOEmitter( MESSAGE_REQUEST *pRequest );
CommHeader *BuildLODefine( MESSAGE_REQUEST *pRequest );
CommHeader *BuildLOUpdateSensor( MESSAGE_REQUEST *pRequest );
CommHeader *BuildLOEnableSensor( MESSAGE_REQUEST *pRequest );
CommHeader *BuildLOTest( MESSAGE_REQUEST *pRequest );
WORD CalcOffset( WORD size, UINT16 Rot, CLA_STATION_CONTROL *pStation);

/*  From mcla_init.c  */
CommHeader *InitializeCLA ( UINT16 init_count );

/*  From m_sensor.c  */
VOID SensorInit(VOID);
VOID UpstreamSensorISR(VOID);
VOID WeigherEntrySensorISR(VOID);
BOOL SensorTimerISR(SENSOR_CTRL *control);
VOID PollSensor(SENSOR_CTRL *sensor_ctrl);
SENSOR_ID PollSensors(VOID);
VOID UpdateRemoteSensorState(UpdateSensorMsg *message);

/*  From m_io.c  */
BOOL GetUpstreamSensor(VOID);
BOOL GetWeigherEntrySensor(VOID);
VOID ClearSensorIntFlag(SENSOR_ID sensor);
VOID SetSensorIntRising(SENSOR_ID sensor);
VOID SetSensorIntFalling(SENSOR_ID sensor);
VOID EnableSensorInt(SENSOR_ID sensor);
VOID DisableSensorInt(SENSOR_ID sensor);
VOID DisableSensorEmitters(VOID);
VOID EnableSensorEmitters(VOID);

#include "/proj/cla/include/mlib_proto.h"

#endif

/**/
/*****************************************/
/*  SLAVE PROCESSOR FUNCTION PROTOTYPES  */
/*****************************************/
#ifdef CLA_SLAVE
/*  Prototypes For Functions Contained In s_rom.c  *
EXTERN MOTOR_COMMAND WeigherGetPackageLeft[];
EXTERN MOTOR_COMMAND WeigherGetPackageRight[];
EXTERN MOTOR_COMMAND PrinterGetPackageLeft[];
EXTERN MOTOR_COMMAND PrinterGetPackageRight[];
EXTERN MOTOR_COMMAND WeigherDischargePackageLeft[];
EXTERN MOTOR_COMMAND WeigherDischargePackageRight[];
EXTERN MOTOR_COMMAND PrinterDischargePackageLeft[];
EXTERN MOTOR_COMMAND PrinterDischargePackageRight[];
EXTERN MOTOR_COMMAND PrinterDischargePackageFront[];
EXTERN MOTOR_COMMAND PrinterJustifyPackageFront[];
EXTERN MOTOR_COMMAND PrinterJustifyPackageBack[];
EXTERN MOTOR_COMMAND PrinterAlignPackageLeft[];
EXTERN MOTOR_COMMAND PrinterAlignPackageRight[];
EXTERN MOTOR_COMMAND PrinterApplyLabel[];
EXTERN MOTOR_COMMAND WeigherNoOp[];
EXTERN MOTOR_COMMAND PrinterNoOp[];				*/

/*  Prototypes For Functions Contained In s_io.c  */
VOID SignalForMaster(VOID);
VOID EndSignalForMaster(VOID);
UINT GetWeigherTimer(VOID);
UINT GetPrinterTimer(VOID);
BYTE GetWeigherHall(VOID);
BYTE GetPrinterHall(VOID);
VOID ClearOverflowFlags(VOID);
VOID ClearWeigherIntFlag(VOID);
VOID ClearPrinterIntFlag(VOID);
VOID ClearSensorIntFlag(VOID);
VOID SetSensorIntRising(VOID);
VOID SetSensorIntFalling(VOID);
VOID EnableSensorInt(VOID);
VOID DisableSensorInt(VOID);
VOID EnablePrinterMotor(VOID);
VOID EnableWeigherMotor(VOID);
VOID DisablePrinterMotor(VOID);
VOID DisableWeigherMotor(VOID);
VOID BrakeWeigherMotor(VOID);
VOID BrakePrinterMotor(VOID);
VOID RelaseWeigherMotor(VOID);
VOID ReleasePrinterMotor(VOID);
VOID TorqueToWeigher(SINT torque);
VOID TorqueToPrinter(SINT torque);
VOID EnableCompressor(VOID);
VOID DisableCompressor(VOID);
VOID GateUp(VOID);
VOID GateDown(VOID);
VOID ActivateBelts(VOID);
VOID ActivateRollers(VOID);
VOID ActivateWand(VOID);
VOID ReleaseWand(VOID);
VOID ActivateApplier(VOID);
VOID ReleaseApplier(VOID);
VOID RotateWand(UINT rotation);
VOID EnableDriveSignals(VOID);
VOID DisableDriveSignals(VOID);
BOOL GetPrinterValidSensor(VOID);
BOOL GetPrinterEntrySensor(VOID);
BOOL GetPrinterPackageSensor(VOID);
BOOL GetGateSensor(VOID);
VOID EnablePrinterStationEntrySensor(VOID);
VOID EnablePrinterStationLeftPackageSensor(VOID);
VOID EnablePrinterStationRightPackageSensor(VOID);
VOID DisablePrinterStationEntrySensor(VOID);
VOID DisablePrinterStationLeftPackageSensor(VOID);
VOID DisablePrinterStationRightPackageSensor(VOID);

/*  Prototypes For Functions Contained In s_sensor.c  */

VOID SensorInit(VOID);
VOID PrinterPackageSensorISR(VOID);
BOOL PrinterPackageSensorTimerISR(SENSOR_CTRL *control);
VOID PollSensor(SENSOR_CTRL *sensor_ctrl);
SENSOR_ID PollSensors(VOID);
VOID UpdateRemoteSensorState(UpdateSensorMsg *message);
VOID ProcessEnableSensorMessage(EnableSensorMsg *message);

/*  Prototypes For Functions Contained In s_fback.c  */

VOID FeedbackInit(MOTOR_CONTROL *control,BYTE (*get_hall)(),WORD (*get_timer)());
VOID StartFeedback(VOID);
VOID WeigherHallISR(VOID);
VOID PrinterHallISR(VOID);
VOID StopFeedback(VOID);
VOID CalculateVelocitySampleTime(MOTOR_CONTROL *control);
VOID CalculateVelocity(MOTOR_CONTROL *control);
VOID CalculateAcceleration(MOTOR_CONTROL *control);
VOID CalculatePosition(MOTOR_CONTROL *control);
VOID SetPosition(MOTOR_CONTROL *control,INT position);
BOOL GetMotorDirection(MOTOR_CONTROL *control);

/*  Prototypes For Functions Contained In s_servo.c  */

SINT to_pos_ctrl(MOTOR_CONTROL *control);
SINT to_vel_ctrl(MOTOR_CONTROL *control);
SINT to_hard_zero_ctrl(MOTOR_CONTROL *control);
BYTE CheckCommandTermination(MOTOR_CONTROL *control);
SINT to_forever(MOTOR_CONTROL *control);
SINT to_sensor_blocked(MOTOR_CONTROL *control);
SINT to_sensor_not_blocked(MOTOR_CONTROL *control);
SINT to_velocity(MOTOR_CONTROL *control);
SINT to_sensor_blocked_or_dist(MOTOR_CONTROL *control);
SINT to_sensor_not_blocked_or_dist(MOTOR_CONTROL *control);
SINT to_sensor_not_blocked_or_dist_parm(MOTOR_CONTROL *control);
SINT to_time(MOTOR_CONTROL *control);
SINT to_countdown(MOTOR_CONTROL *control);
SINT to_dist(MOTOR_CONTROL *control);
SINT to_dist_parm(MOTOR_CONTROL *control);
SINT to_dist_less_parm(MOTOR_CONTROL *control);	 /* NEEP */
SINT to_time_parm(MOTOR_CONTROL *control);

/*  Prototypes For Functions Contained In s_motor.c  */

VOID MotorControlInit(VOID);
VOID SetCommandTable(MOTOR_CONTROL *control,MOTOR_COMMAND *command_table,INT parameter);
VOID GetNextCommand(MOTOR_CONTROL *control,BYTE offset);
VOID MotorControlISR(VOID);


/*  Prototypes For Functions Contained in s_rom.c  */

VOID InitCLACommands(VOID);
VOID DefineCLACommand(CLA_STATION_ID station, CLA_COMMAND_ID command, CLA_COMMAND_TYPE type, WORD p_gain, WORD d_gain);


/*  Prototypes For Functions Contained in s_mgr.c  */

VOID InitializeCLAHardware(VOID);
CommHeader *InitializeCLA(UINT init_count);
CommHeader *CLAWakeupMessage(VOID);
CommHeader *CLAUpdateSensorMessage(SENSOR_ID sensor);
VOID ExecuteCLACommand(CLA_STATION_ID station, CLA_COMMAND_ID command, WORD parameter);
CommHeader *CLAStatusMessage(CLA_COMMAND_STATUS weigher_command_status,
							 CLA_COMMAND_STATUS printer_command_status,
							 CLA_COMMAND_ID weigher_command,
							 CLA_COMMAND_ID printer_command,
							 UINT32 error_status);
CommHeader *CLAManager(CommHeader *message);

#endif /* CLA_SLAVE */
#endif /* M68HC11 */
#endif /* _CLA */

