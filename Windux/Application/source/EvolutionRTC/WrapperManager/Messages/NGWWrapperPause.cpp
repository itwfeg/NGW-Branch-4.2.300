/**
 * @file NGWWrapperPause.cpp
 * @copyright Copyright (c) 2021 Hobart Corporation
 * @author Aaron Swift
 * @brief NGW Wrapper pause message.
 *         Wrapper pauses to determine if printer plates are in
 *        the correct position to apply labels 
 * Created on: Mar 30, 2021
 */
#include "NGWWrapperPause.h"
#include "Integer.h"
#include "Debug.h"

NGWWrapperPause::NGWWrapperPause( Hobart::CEvolMsgReceiver *pHobThread ) : WrapperGeneric( WrWrapperPause, pHobThread )
{
    ngwState_ = PAUSE;
    priPlatePosition_       = 0;
    reqPriPlatePosition_    = 0;
    secPlatePosition_       = 0;
    reqSecPlatePosition_    = 0;
}

NGWWrapperPause::NGWWrapperPause( const char *buffer ) : WrapperGeneric( buffer )
{

	WrapperGeneric::serialize();
	int j = getSerializedLength();

	char *pChar = (char *) &ngwState_;
	for (unsigned int i = 0; i < sizeof(ngwState_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &pkgId_;
	for (unsigned int i = 0; i < sizeof(pkgId_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &priPlatePosition_;
	for (unsigned int i = 0; i < sizeof(priPlatePosition_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &reqPriPlatePosition_;
	for (unsigned int i = 0; i < sizeof(reqPriPlatePosition_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &secPlatePosition_;
	for (unsigned int i = 0; i < sizeof(secPlatePosition_); i++)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &reqSecPlatePosition_;
	for (unsigned int i = 0; i < sizeof(reqSecPlatePosition_); i++)
	{
		*pChar++ = buffer[j++];
	}

}

NGWWrapperPause::~NGWWrapperPause()
{

}

const char *NGWWrapperPause::serialize() const
{
	WrapperGeneric::serialize();

	char *pChar = (char *) &ngwState_;
	for (unsigned int i = 0; i < sizeof(ngwState_); i++)
	{
		serializedData.append(1, pChar[i]);
	}
	
    pChar = (char *) &pkgId_;
	for (unsigned int i = 0; i < sizeof(pkgId_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

    pChar = (char *) &priPlatePosition_;
	for (unsigned int i = 0; i < sizeof(priPlatePosition_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &reqPriPlatePosition_;
	for (unsigned int i = 0; i < sizeof(reqPriPlatePosition_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &secPlatePosition_;
	for (unsigned int i = 0; i < sizeof(secPlatePosition_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &reqSecPlatePosition_;
	for (unsigned int i = 0; i < sizeof(reqSecPlatePosition_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void NGWWrapperPause::show()
{
    Debug::trace("NGWWrapperPause","ngwState_: " + Integer::toString(ngwState_) );    
    Debug::trace("NGWWrapperPause","priPlatePosition_: " + Integer::toString(priPlatePosition_) );    
    Debug::trace("NGWWrapperPause","reqPriPlatePosition_: " + Integer::toString(reqPriPlatePosition_) );    
    Debug::trace("NGWWrapperPause","secPlatePosition_: " + Integer::toString(secPlatePosition_) );    
    Debug::trace("NGWWrapperPause","reqSecPlatePosition_: " + Integer::toString(reqSecPlatePosition_) );    
}
