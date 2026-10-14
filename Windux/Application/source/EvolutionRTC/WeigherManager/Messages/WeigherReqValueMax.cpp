/**
 * @file WeigherReqValueMax.cpp
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Aaron Swift
 * @brief value max requests
 * Created on: Sept 20, 2017
 *
 **/
#include "WeigherReqValueMax.h"
#include "WeigherEvent.h"
#include "Debug.h"
WeigherReqValueMax::WeigherReqValueMax( ValueMaxReq req, Hobart::CEvolMsgReceiver *pHobThread ) : EvolMessage( Weigher, WgReqValueMax, pHobThread )
{
	request = req;
}

WeigherReqValueMax::WeigherReqValueMax( const char *buffer ) : EvolMessage(buffer)
{
	EvolMessage::serialize();
	int j = getSerializedLength();

	char *pChar = (char *) &request;
	for (unsigned int i = 0; i < sizeof(request); i++)
	{
		*pChar++ = buffer[j++];
	}
}

WeigherReqValueMax::~WeigherReqValueMax()
{
}

const char *WeigherReqValueMax::serialize()const
{
	EvolMessage::serialize();

	char *pChar = (char *) &request;
	for (unsigned int i = 0; i < sizeof(request); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

