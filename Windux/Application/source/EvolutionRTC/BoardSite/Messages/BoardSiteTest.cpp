/**
 * @file BoardSiteTest.cpp
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Aaron Swift
 * @brief Message for functional testing of ARK board and KPC.
 * Created on: Feb 4, 2020
 */
#include "BoardSiteTest.h"
#include "BoardSiteEvent.h"
#include "Debug.h"
#include "Integer.h"

BoardSiteTest::BoardSiteTest(Hobart::CEvolMsgReceiver *pHobThread, BoardSiteTests code) : EvolMessage(BoardSite, BrdSiteBoardTest, pHobThread)
{
    testCode_ = code;
}

BoardSiteTest::BoardSiteTest(Hobart::CEvolMsgReceiver *pHobThread, BoardSiteTests code,
    unsigned char testParam1, unsigned char testParam2, unsigned char testParam3,
    unsigned char testParam4, unsigned char testParam5, unsigned char testParam6, 
    unsigned char testParam7 ) : EvolMessage(BoardSite, BrdSiteBoardTest, pHobThread)
{
    testCode_ = code;
    testParam1_ = testParam1;
    testParam2_ = testParam2;
    testParam3_ = testParam3;
    testParam4_ = testParam4;
    testParam5_ = testParam5;
    testParam6_ = testParam6;
    testParam7_ = testParam7;
}     

BoardSiteTest::BoardSiteTest(const char *buffer) : EvolMessage(buffer)
{
	EvolMessage::serialize();

	int j = getSerializedLength();

	char *pChar = (char *) &testCode_;
	for (unsigned int i = 0; i < sizeof(testCode_); i++)
	{
		*pChar++ = buffer[j++];
	}

    pChar = (char *) &testParam1_;
	for (unsigned int i = 0; i < sizeof(testParam1_); i++)
	{
		*pChar++ = buffer[j++];
	}

    pChar = (char *) &testParam2_;
	for (unsigned int i = 0; i < sizeof(testParam2_); i++)
	{
		*pChar++ = buffer[j++];
	}

    pChar = (char *) &testParam3_;
	for (unsigned int i = 0; i < sizeof(testParam3_); i++)
	{
		*pChar++ = buffer[j++];
	}

    pChar = (char *) &testParam4_;
	for (unsigned int i = 0; i < sizeof(testParam4_); i++)
	{
		*pChar++ = buffer[j++];
	}

    pChar = (char *) &testParam5_;
	for (unsigned int i = 0; i < sizeof(testParam5_); i++)
	{
		*pChar++ = buffer[j++];
	}

    pChar = (char *) &testParam6_;
	for (unsigned int i = 0; i < sizeof(testParam6_); i++)
	{
		*pChar++ = buffer[j++];
	}

    pChar = (char *) &testParam7_;
	for (unsigned int i = 0; i < sizeof(testParam7_); i++)
	{
		*pChar++ = buffer[j++];
	}
}

BoardSiteTest::~BoardSiteTest()
{}

const char *BoardSiteTest::serialize() const
{
	EvolMessage::serialize();

	char *pChar = (char *) &testCode_;
	for (unsigned int i = 0; i < sizeof(testCode_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

    pChar = (char *) &testParam1_;
	for (unsigned int i = 0; i < sizeof(testParam1_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

    pChar = (char *) &testParam2_;
	for (unsigned int i = 0; i < sizeof(testParam2_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

    pChar = (char *) &testParam3_;
	for (unsigned int i = 0; i < sizeof(testParam3_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

    pChar = (char *) &testParam4_;
	for (unsigned int i = 0; i < sizeof(testParam4_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

    pChar = (char *) &testParam5_;
	for (unsigned int i = 0; i < sizeof(testParam5_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

    pChar = (char *) &testParam6_;
	for (unsigned int i = 0; i < sizeof(testParam6_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

    pChar = (char *) &testParam7_;
	for (unsigned int i = 0; i < sizeof(testParam7_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}


