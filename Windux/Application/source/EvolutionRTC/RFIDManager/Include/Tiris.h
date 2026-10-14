/* Evolution Scale Project
 * COPYRIGHT (c) 2001.  All Rights Reserved.
 * Hobart Corporation. Troy, Ohio  USA
 */

#ifndef TIRIS_H
#define TIRIS_H

	#define MAX_TIRIS_BODY 41
	#define MAX_TIRIS_PACKET MAX_TIRIS_BODY + 3
	#define SIZE_ID_BYTES 8
	#define SIZE_TIRIS_SW_VER_BYTES 2

	#define TIRIS_READ_TIMEOUT_MS 5000
	#define MAX_TIRIS_READ_PACKET_SIZE 13

	#define START_MARK 0x01
	#define CMD_LENGTH_2 0x02
	#define BURST_MODE_50MS 0x32 
	#define CMD1_MODE_SINGLE 0x00
	#define CMD1_MODE_CONTINUOUS_NORMAL 0x01
	#define CMD1_MODE_CONTINUOUS_LINE 0x02
	#define CMD1_MODE_REQUEST_VERSION 0x03
	#define CMD1_FBCC_CALC 0x04
	#define CMD1_POW_BURST1 0x08
	#define CMD1_POW_PAUSE_DUR 0x10
	#define CMD1_POW_BURST2 0x20
	#define CMD1_DATA 0x40
	#define CMD1_EXPANSION 0x80

	#define CMD2_SPECIAL_WRITE_TIMING 0x01
	#define CMD2_WIRELESS_SYNCH 0x02
	#define CMD2_DBCC_CALC 0x04

	#define STATUS_TRANSPONDER_RO 0x00
	#define STATUS_TRANSPONDER_RW 0x01
	#define STATUS_TRANSPONDER_MPT 0x02
	#define STATUS_TRANSPONDER_OTHER 0x03
	#define STATUS_STARTBYTE_DETECTED 0x04
	#define STATUS_DBCC_OK 0x08
	#define STATUS_FBCC_OK 0x10
	#define STATUS_VERSION 0x20

	typedef struct
	{
		UINT8	startMark;
		UINT8	length;
		UINT8	status;
	} ReadBasePacket;

	typedef struct
	{
		ReadBasePacket bp;
		UINT8	BCC;
	} NoIDPacket;

	typedef struct
	{
		ReadBasePacket bp;
		UINT8	version;
		UINT8	BCC;
	} ReceiveVersion;


	typedef struct
	{
		ReadBasePacket bp;
		UINT8	id[SIZE_ID_BYTES];
		UINT8	BCC;
	} ValidIDPacket;

	typedef struct
	{
		UINT8	startMark;
		UINT8	length;
		UINT8	command1;
	} WriteBasePacket;

	typedef struct
	{
		WriteBasePacket bp;
		UINT8	dataField;
		UINT8	BCC;
	} RequestReadRO;



#endif //TIRIS_H




