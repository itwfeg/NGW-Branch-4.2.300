/**
 * @file NGWBootFrame.h
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Aaron Swift
 * @brief CAN Wrapper message containing source image
 * Created on: Dec 9, 2015
 *
 **/

#ifndef NGWBOOTFRAME_H_
#define NGWBOOTFRAME_H_

#include "EvolMessage.h"

class NGWBootFrame : public EvolMessage
{
	public:

	typedef enum
	{
		_CanHeaderFrame,
		_CanFrame1,
		_CanFrame2,
		_CanFrame3,
		_CanFrame4,
		_CanFrame5,
		_CanFrame6,
		_CanFrame7,
		_CanFrame8,
	}FrameID;

	#define MAX_PAYLOAD 8
	NGWBootFrame( Hobart::CEvolMsgReceiver *pHobThread = NULL, FrameID frameId = _CanHeaderFrame, UINT8 *pFrameData = NULL, int length = 0 );
	NGWBootFrame(const char *buffer);
	~NGWBootFrame();

	const char *serialize()const;
	FrameID getFrameNumber() const { return frameNumber; }

	UINT8 getFrameData( UINT8 id ) const;

	void setFrameData1(UINT8 x) { data[0] = x; }
	void setFrameData2(UINT8 x) { data[1] = x; }
	void setFrameData3(UINT8 x) { data[2] = x; }
	void setFrameData4(UINT8 x) { data[3] = x; }
	void setFrameData5(UINT8 x) { data[4] = x; }
	void setFrameData6(UINT8 x) { data[5] = x; }
	void setFrameData7(UINT8 x) { data[6] = x; }
	void setFrameData8(UINT8 x) { data[7] = x; }

	int getPayloadLength() const { return dataLength; }
	void setPayloadLength(int length) { dataLength = length; }
	void show();

	private:
	FrameID frameNumber;
	int dataLength;
	UINT8 data[MAX_PAYLOAD];
	std::string str;
};

#endif
