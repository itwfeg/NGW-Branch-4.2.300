/*
 * NGWError.cpp
 *
 *  Created on: Aug 10, 2015
 *      Author: aaron
 */
#include "NGWError.h"

NGWError::NGWError( Hobart::CEvolMsgReceiver *pHobThread ) : WrapperGeneric( WrNgwError, pHobThread )
{
	_mechanism = 0;
	_error = 0;
}

NGWError::NGWError( const char *buffer ) : WrapperGeneric( buffer )
{
	WrapperGeneric::serialize();

	int j = getSerializedLength();

	char *pChar = (char *) &_mechanism;
	for (unsigned int i = 0; i < sizeof(_mechanism); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &_error;
	for (unsigned int i = 0; i < sizeof(_error); i++)
	{
		*pChar++ = buffer[j++];
	}

}

NGWError::~NGWError(){}

const char *NGWError::serialize() const
{
	WrapperGeneric::serialize();


	char *pChar = (char *) &_mechanism;
	for (unsigned int i = 0; i < sizeof(_mechanism); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &_error;
	for (unsigned int i = 0; i < sizeof(_error); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

