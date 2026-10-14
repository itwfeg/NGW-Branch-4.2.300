/**
 * @file NGWDimensionOverrides.cpp
 * @copyright Copyright (c) 2021 Hobart Corporation
 * @author Aaron Swift
 * @brief Wrapper to ignore camera package dimensions.
 * Created on: Oct 21, 2021
 * 
 */
#include "NGWDimensionOverrides.h"
NGWDimensionOverrides::NGWDimensionOverrides( Hobart::CEvolMsgReceiver *pHobThread ) : EvolMessage( Wrapper, WrNGWDimensionOverrides, pHobThread )
{

}

NGWDimensionOverrides::NGWDimensionOverrides(const char *buffer)  : EvolMessage(buffer)
{
	EvolMessage::serialize();
	int j = getSerializedLength();

	char *pChar = (char *) &length_;
	for (unsigned int i = 0; i < sizeof(length_); i++)
	{
		*pChar++ = buffer[j++];
	}
	pChar = (char *) &width_;
	for (unsigned int i = 0; i < sizeof(width_); i++)
	{
		*pChar++ = buffer[j++];
	}
	pChar = (char *) &lipHeight_;
	for (unsigned int i = 0; i < sizeof(lipHeight_); i++)
	{
		*pChar++ = buffer[j++];
	}
}

NGWDimensionOverrides::~NGWDimensionOverrides()
{

}

const char *NGWDimensionOverrides::serialize()const
{
	EvolMessage::serialize();

	char *pChar = (char *) &length_;
	for (unsigned int i = 0; i < sizeof(length_); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	pChar = (char *) &width_;
	for (unsigned int i = 0; i < sizeof(width_); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	pChar = (char *) &lipHeight_;
	for (unsigned int i = 0; i < sizeof(lipHeight_); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	return serializedData.data();
}
