/*
 * Sam7xUpdaterMsg.h
 *
 *  Created on: May 18, 2015
 *      Author: aaron
 */

#ifndef SAM7XUPDATERMSG_H_
#define SAM7XUPDATERMSG_H_

#if defined ( __linux__ )
#include <net/if.h>
#include <linux/can.h>
#endif // __linux__

class Sam7xUpdaterMsg
{

	public:

		typedef enum
		{
			_Empty,
			_OpenInterface,
			_ReadVendorId,
			_ReadProductId,
			_ReadMajVersion,
			_ReadMinVersion,
			_ReadNodeId,
			_WriteNodeId,
			_WriteBuffer,
			_WriteData,
			_JumpToApplication
		}Sam7xMsgType;

		Sam7xUpdaterMsg( Sam7xMsgType msg, int id);
		Sam7xUpdaterMsg( Sam7xMsgType msg, int id, unsigned char *pFrameData, int numbytes );
		~Sam7xUpdaterMsg();

#if defined ( __linux__ )
		Sam7xMsgType getMessageType() { return type; }

		can_frame *getMessage();
		const static int FRAME_DATA_SIZE = 8;
		unsigned char frameData[FRAME_DATA_SIZE];

		bool isDeviceInterfaceOpen();
		bool isHobartDevice();

		int getProductId();
		int getCommandResponse();
		void show();
#endif // __linux__


	private:

#if defined ( __linux__ )
		Sam7xMsgType type;
		can_frame frame;
		unsigned char bytesToTransfer;

		static const unsigned char GlobalNodeID = 0;
		static const unsigned char OpenInterfaceCMD = 0xff;

		/* SAM7x Commands */
		static const unsigned char IDSelectNodeCMD 		= 0;
		static const unsigned char WriteBufferCMD 		= 1;
		static const unsigned char WriteDataBufferCMD 	= 2;
		static const unsigned char ReadDataBufferCMD	= 3;
		static const unsigned char ReadWordCMD			= 4;
		static const unsigned char WriteWordCMD			= 5;
		static const unsigned char SpecialCMD			= 6;

		void buildFrame( int id );
#endif // __linux__

};

#endif
