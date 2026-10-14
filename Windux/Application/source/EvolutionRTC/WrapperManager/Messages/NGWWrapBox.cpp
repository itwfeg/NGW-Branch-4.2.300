/*
 * NGWWrapBox.cpp
 *
 *  Created on: Oct 29, 2015
 *      Author: aaron
 */
#include "NGWWrapBox.h"

NGWWrapBox::NGWWrapBox( Hobart::CEvolMsgReceiver *pHobThread ) : WrapperGeneric( WrWrapBox, pHobThread )
{
	_wrapBoxIndex = 0;
	_boxName[0] = 0;
	_length = 0;
	_width = 0;
	_lip = 0;
	_height = 0;
	_plu = 0;
}

NGWWrapBox::NGWWrapBox( const char *buffer ) : WrapperGeneric( buffer )
{

	WrapperGeneric::serialize();
	int j = getSerializedLength();

	char *pChar = (char *) &_wrapBoxIndex;
	for (unsigned int i = 0; i < sizeof(_wrapBoxIndex); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &_boxName[0];
	for (unsigned int i = 0; i < _MAX_NAME_SIZE; i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &_length;
	for (unsigned int i = 0; i <  sizeof(_length); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &_width;
	for (unsigned int i = 0; i <  sizeof(_width); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &_lip;
	for (unsigned int i = 0; i < sizeof(_lip); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &_height;
	for (unsigned int i = 0; i < sizeof(_height); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &_plu;
	for (unsigned int i = 0; i < sizeof(_plu); i++)
	{
		*pChar++ = buffer[j++];
	}

}

NGWWrapBox::~NGWWrapBox()
{

}

const char *NGWWrapBox::serialize() const
{
	WrapperGeneric::serialize();

	char *pChar = (char *) &_wrapBoxIndex;
	for (unsigned int i = 0; i < sizeof(_wrapBoxIndex); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	pChar = (char *) &_boxName[0];
	for (unsigned int i = 0; i < _MAX_NAME_SIZE; i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &_length;
	for (unsigned int i = 0; i < sizeof(_length); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &_width;
	for (unsigned int i = 0; i <  sizeof(_width); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &_lip;
	for (unsigned int i = 0; i < sizeof(_lip); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &_height;
	for (unsigned int i = 0; i < sizeof(_height); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &_plu;
	for (unsigned int i = 0; i < sizeof(_plu); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}



