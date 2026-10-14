/**
 * @file WrapBoxSize.cpp
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Aaron Swift
 * @brief CAN Wrapper message
 * Created on: Sept 5, 2017
 *
 **/
#include "WrapBoxSize.h"

WrapBoxSize::WrapBoxSize( Hobart::CEvolMsgReceiver *pHobThread, UINT16 size ) : EvolMessage( Wrapper, WrWrapBoxSizeSave, pHobThread )
{
	size_ = size;
}

WrapBoxSize::WrapBoxSize(const char *buffer) : EvolMessage(buffer)
{
	EvolMessage::serialize();
	int j = getSerializedLength();

	char *pChar = (char *) &size_;
	for (unsigned int i = 0; i < sizeof(size_); i++)
	{
		*pChar++ = buffer[j++];
	}
}

WrapBoxSize::~WrapBoxSize()
{

}

const char *WrapBoxSize::serialize()const
{
	EvolMessage::serialize();

	char *pChar = (char *) &size_;
	for (unsigned int i = 0; i < sizeof(size_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}











