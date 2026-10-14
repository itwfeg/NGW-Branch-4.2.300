/*
 * NGWCycleCount.cpp
 *
 *  Created on: Aug 10, 2015
 *      Author: aaron
 */
#include "NGWCycleCount.h"

NGWCycleCount::NGWCycleCount( Hobart::CEvolMsgReceiver *pHobThread ) : WrapperGeneric(WrNgwCycleCount, pHobThread )
{
	_cycleCount = 0;
}

NGWCycleCount::NGWCycleCount( const char *buffer ) : WrapperGeneric( buffer )
{
	WrapperGeneric::serialize();
	int j = getSerializedLength();

	char *pChar = (char *) &_cycleCount;
	for (unsigned int i = 0; i < sizeof(_cycleCount); i++)
	{
		*pChar++ = buffer[j++];
	}
}

NGWCycleCount::~NGWCycleCount()
{

}

const char *NGWCycleCount::serialize()const
{
	WrapperGeneric::serialize();

	char *pChar = (char *) &_cycleCount;
	for (unsigned int i = 0; i < sizeof(_cycleCount); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	return serializedData.data();
}
