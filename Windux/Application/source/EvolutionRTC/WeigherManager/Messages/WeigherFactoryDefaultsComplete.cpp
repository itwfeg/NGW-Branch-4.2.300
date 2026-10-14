/**
 * @file WeigherFactoryDefaultsComplete.cpp
 * @copyright Copyright (c) 2018 Hobart Corporation
 * @author Aaron Swift
 * @brief Message for signaling the completion of saving weigher configuration defaults.
 * Created on: March 1, 2018
 */
#include "WeigherFactoryDefaultsComplete.h"
#include "WeigherEvent.h"
#include "Debug.h"
#include "Integer.h"

WeigherFactoryDefaultsComplete::WeigherFactoryDefaultsComplete(Hobart::CEvolMsgReceiver *pHobThread, bool saved ) : EvolMessage(Weigher, WgResetFactoryDefaultsComplete, pHobThread)
{
	saved_ = saved;
}

WeigherFactoryDefaultsComplete::WeigherFactoryDefaultsComplete(const char *buffer) : EvolMessage(buffer)
{
	EvolMessage::serialize();

	int j = getSerializedLength();

	char *pChar = (char *) &saved_;
	for (unsigned int i = 0; i < sizeof(saved_); i++)
	{
		*pChar++ = buffer[j++];
	}
}

WeigherFactoryDefaultsComplete::~WeigherFactoryDefaultsComplete()
{}

const char *WeigherFactoryDefaultsComplete::serialize() const
{
	EvolMessage::serialize();

	char *pChar = (char *) &saved_;
	for (unsigned int i = 0; i < sizeof(saved_); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	return serializedData.data();
}


