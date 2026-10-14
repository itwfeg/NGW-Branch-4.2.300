/**
 * @file WRPPPWAKEUP.cpp
 * @copyright Copyright (c) 2021 Hobart Corporation
 * @author Aaron Swift
 * @brief Printer plate position wakeup message
 * Created on: Mar 9, 2021
 */
#include "WrPPPWakeup.h"
#include "Integer.h"
#include "Debug.h"

PPPWakeup::PPPWakeup( Hobart::CEvolMsgReceiver *pHobThread ) : WrapperGeneric( WrPPPWakeup, pHobThread )
{
    productId_ = 0;
    rate_ = 0;       
    swMajor_ = 0;
    swMinor_ = 0;
    swbuild_ = 0;
    swVer_ = "";
    hwMajor_ = 0;
    hwMinor_ = 0;
    hwVer_ = "";
}

PPPWakeup::PPPWakeup( const char *buffer ) : WrapperGeneric( buffer )
{

	WrapperGeneric::serialize();
	int j = getSerializedLength();

	char *pChar = (char *) &productId_;
	for (unsigned int i = 0; i < sizeof(productId_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &rate_;
	for (unsigned int i = 0; i < sizeof(rate_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &swMajor_;
	for (unsigned int i = 0; i < sizeof(swMajor_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &swMinor_;
	for (unsigned int i = 0; i < sizeof(swMinor_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &swbuild_;
	for (unsigned int i = 0; i < sizeof(swbuild_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &hwMajor_;
	for (unsigned int i = 0; i < sizeof(hwMajor_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &hwMinor_;
	for (unsigned int i = 0; i < sizeof(hwMinor_); i++)
	{
		*pChar++ = buffer[j++];
	}

    //build the version strings
    swVer_ = Integer::toString(swMajor_) + "." + Integer::toString(swMinor_) + "." +  Integer::toString(swbuild_);
    hwVer_ = Integer::toString(hwMajor_) + "." + Integer::toString(hwMinor_);

}

PPPWakeup::~PPPWakeup()
{

}

const char *PPPWakeup::serialize() const
{
	WrapperGeneric::serialize();

	char *pChar = (char *) &productId_;
	for (unsigned int i = 0; i < sizeof(productId_); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	pChar = (char *) &rate_;
	for (unsigned int i = 0; i < sizeof(rate_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &swMajor_;
	for (unsigned int i = 0; i < sizeof(swMajor_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &swMinor_;
	for (unsigned int i = 0; i < sizeof(swMinor_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &swbuild_;
	for (unsigned int i = 0; i < sizeof(swbuild_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &hwMajor_;
	for (unsigned int i = 0; i < sizeof(hwMajor_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &hwMinor_;
	for (unsigned int i = 0; i < sizeof(hwMinor_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void PPPWakeup::show()
{
    Debug::trace("PPPWakeup","ProductId: " + Integer::toString(productId_) );    
    Debug::trace("PPPWakeup","rate: " + Integer::toString(rate_) );    
    Debug::trace("PPPWakeup","swMajor: " + Integer::toString(swMajor_) );    
    Debug::trace("PPPWakeup","swMinor: " + Integer::toString(swMinor_) );    
    Debug::trace("PPPWakeup","swbuild: " + Integer::toString(swbuild_) );    
    Debug::trace("PPPWakeup","hwMajor: " + Integer::toString(hwMajor_) );    
    Debug::trace("PPPWakeup","hwMinor: " + Integer::toString(hwMinor_) );    
    Debug::trace("PPPWakeup","Software version: " + swVer_);    
    Debug::trace("PPPWakeup","Hardware version: " + hwVer_ );    
}

