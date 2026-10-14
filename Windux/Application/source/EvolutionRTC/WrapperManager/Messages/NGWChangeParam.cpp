/*
 * NGWChangeParam.cpp
 *
 *  Created on: Oct 30, 2015
 *      Author: aaron
 */
#include "NGWChangeParam.h"
NGWChangeParam::NGWChangeParam(Hobart::CEvolMsgReceiver *pHobThread) : WrapperGeneric(WrChangeParam, pHobThread)
{
	_wrapBoxIndex = 0;
	_heightBoxIndex = 0;
	_sec_distance = 0;
	_gripper_pull = 0;
	_sideclamp_pos = 0;
	_intake_speed = 0;
	_discharge_speed = 0;
}


NGWChangeParam::NGWChangeParam(const char *buffer)
		: WrapperGeneric(buffer)
{

	WrapperGeneric::serialize();
	int j = getSerializedLength();

	char *pChar = (char *) &_wrapBoxIndex;
	for (unsigned int i = 0; i < sizeof(_wrapBoxIndex); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &_heightBoxIndex;
	for (unsigned int i = 0; i < sizeof(_heightBoxIndex); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &_sec_distance;
	for (unsigned int i = 0; i <  sizeof(_sec_distance); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &_gripper_pull;
	for (unsigned int i = 0; i <  sizeof(_gripper_pull); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &_sideclamp_pos;
	for (unsigned int i = 0; i < sizeof(_sideclamp_pos); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &_intake_speed;
	for (unsigned int i = 0; i < sizeof(_intake_speed); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &_discharge_speed;
	for (unsigned int i = 0; i < sizeof(_discharge_speed); i++)
	{
		*pChar++ = buffer[j++];
	}

}

NGWChangeParam::~NGWChangeParam(){}

const char *NGWChangeParam::serialize() const
{
	WrapperGeneric::serialize();

	char *pChar = (char *) &_wrapBoxIndex;
	for (unsigned int i = 0; i < sizeof(_wrapBoxIndex); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	pChar = (char *) &_heightBoxIndex;
	for (unsigned int i = 0; i < sizeof(_heightBoxIndex); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &_sec_distance;
	for (unsigned int i = 0; i < sizeof(_sec_distance); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &_gripper_pull;
	for (unsigned int i = 0; i <  sizeof(_gripper_pull); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &_sideclamp_pos;
	for (unsigned int i = 0; i < sizeof(_sideclamp_pos); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &_intake_speed;
	for (unsigned int i = 0; i < sizeof(_intake_speed); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &_discharge_speed;
	for (unsigned int i = 0; i < sizeof(_discharge_speed); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}





