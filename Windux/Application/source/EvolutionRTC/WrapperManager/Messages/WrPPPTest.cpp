/**
 * @file WrPPPTest.cpp
 * @copyright Copyright (c) 2021 Hobart Corporation
 * @author Aaron Swift
 * @brief Printer plate test message
 * Created on: Mar 26, 2021
 */
#include "WrPPPTest.h"
#include "Integer.h"
#include "Debug.h"

PPPTest::PPPTest( Hobart::CEvolMsgReceiver *pHobThread, bool start ) : WrapperGeneric( WrPPPTest, pHobThread )
{
    startTest_ = start;
}

PPPTest::PPPTest( const char *buffer ) : WrapperGeneric( buffer )
{

	WrapperGeneric::serialize();
	int j = getSerializedLength();

	char *pChar = (char *) &startTest_;
	for (unsigned int i = 0; i < sizeof(startTest_); i++)
	{
		*pChar++ = buffer[j++];
	}
}

PPPTest::~PPPTest()
{

}

const char *PPPTest::serialize() const
{
	WrapperGeneric::serialize();

	char *pChar = (char *) &startTest_;
	for (unsigned int i = 0; i < sizeof(startTest_); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	return serializedData.data();
}

void PPPTest::show()
{
    Debug::trace("PPPStatus","startTest_: " + Integer::toString(startTest_) );    
}

