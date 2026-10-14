/**
 * @file BoardSiteTestStatus.h
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Aaron Swift
 * @brief Message for reporting board site test status.
 * Created on: Feb 4, 2020
 */
#include "BoardSiteTestStatus.h"
#include "BoardSiteEvent.h"
#include "Debug.h"
#include "Integer.h"

BoardSiteTestStatus::BoardSiteTestStatus(Hobart::CEvolMsgReceiver *pHobThread, bool ready) : EvolMessage(BoardSite, BrdSiteTestStatus, pHobThread)
{
    ready_ = ready;
}

BoardSiteTestStatus::BoardSiteTestStatus(const char *buffer) : EvolMessage(buffer)
{
	EvolMessage::serialize();

	int j = getSerializedLength();

	char *pChar = (char *) &ready_;
	for (unsigned int i = 0; i < sizeof(ready_); i++)
	{
		*pChar++ = buffer[j++];
	}
}
BoardSiteTestStatus::~BoardSiteTestStatus()
{}

const char *BoardSiteTestStatus::serialize() const
{
	EvolMessage::serialize();

	char *pChar = (char *) &ready_;
	for (unsigned int i = 0; i < sizeof(ready_); i++)
	{
		serializedData.append(1, pChar[i]);
	}
    
    return serializedData.data();
}