/**
 * @file BoardSitePercentComplete.cpp
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Aaron Swift
 * @brief Message for displaying percentage complete on the UI.
 * Created on: April 49, 2020
 */
#include "BoardSitePercentComplete.h"
#include "BoardSiteEvent.h"
BoardSitePercentComplete::BoardSitePercentComplete(Hobart::CEvolMsgReceiver *pHobThread, BSTestComplete test, int cmplt ) : EvolMessage(BoardSite, BrdSitePercentCompete, pHobThread)
{
    percent_ = cmplt;
	test_ = test;
}


BoardSitePercentComplete::BoardSitePercentComplete(const char *buffer) : EvolMessage(buffer)
{
	EvolMessage::serialize();

	int j = getSerializedLength();

	char *pChar = (char *) &percent_;
	for (unsigned int i = 0; i < sizeof(percent_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &test_;
	for (unsigned int i = 0; i < sizeof(test_); i++)
	{
		*pChar++ = buffer[j++];
	}
}

BoardSitePercentComplete::~BoardSitePercentComplete()
{
	
}

const char *BoardSitePercentComplete::serialize() const
{
	EvolMessage::serialize();

	char *pChar = (char *) &percent_;
	for (unsigned int i = 0; i < sizeof(percent_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &test_;
	for (unsigned int i = 0; i < sizeof(test_); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	return serializedData.data();
}


