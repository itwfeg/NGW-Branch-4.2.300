/**
 * @file NGWWakeup.cpp
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Aaron Swift
 * @brief
 * Created on: Aug 22, 2017
 *
 */

#include "WrapperEvent.h"
#include "NGWWakeup.h"

#include "Debug.h"
#include "Integer.h"
NGWWakeup::NGWWakeup(Hobart::CEvolMsgReceiver *pHobThread,
					 WrapperModel model, WrapperOp operation ) :  EvolMessage(Wrapper, WrEvolWakeup, pHobThread)
{
	model_ = model;
	operation_ = operation;
}

NGWWakeup::NGWWakeup(const char *buffer) : EvolMessage(buffer)
{
	EvolMessage::serialize();
	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &model_;
	for (unsigned int i = 0; i < sizeof(model_); i++)
	{
		*pChar++ = buffer[j++];
	}
	pChar = (char *) &operation_;
	for (unsigned int i = 0; i < sizeof(operation_); i++)
	{
		*pChar++ = buffer[j++];
	}
}

NGWWakeup::~NGWWakeup()
{

}

const char *NGWWakeup::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &model_;
	for (unsigned int i = 0; i < sizeof(model_); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	pChar = (char *) &operation_;
	for (unsigned int i = 0; i < sizeof(operation_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void NGWWakeup::show()
{
	Debug::trace( "NGWWakeup", "NGWWakeup Model Number: " + Integer::toString( model_ ) );
	Debug::trace( "NGWWakeup", "NGWWakeup Operation: " + Integer::toString( operation_ ) );
}

