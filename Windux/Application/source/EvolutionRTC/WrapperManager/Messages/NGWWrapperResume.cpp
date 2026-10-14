/**
 * @file NGWWrapperResume.h
 * @copyright Copyright (c) 2021 Hobart Corporation
 * @author Aaron Swift
 * @brief NGW Wrapper resume message. 
  * Created on: May 18, 2021
 */
#include "NGWWrapperResume.h"
#include "Integer.h"
#include "Debug.h"

NGWWrapperResume::NGWWrapperResume( Hobart::CEvolMsgReceiver *pHobThread ) : WrapperGeneric( WrPPPResume, pHobThread )
{
    pkgId_ = 0;
	ngwState_ = PAUSE;	
}

NGWWrapperResume::NGWWrapperResume( const char *buffer ) : WrapperGeneric( buffer )
{

	WrapperGeneric::serialize();
	int j = getSerializedLength();

	char *pChar = (char *) &pkgId_;
	for (unsigned int i = 0; i < sizeof(pkgId_); i++)
	{
		*pChar++ = buffer[j++];
	}
	
	pChar = (char *) &ngwState_;
	for (unsigned int i = 0; i < sizeof(ngwState_); i++)
	{
		*pChar++ = buffer[j++];
	}
}

NGWWrapperResume::~NGWWrapperResume()
{

}

const char *NGWWrapperResume::serialize() const
{
	WrapperGeneric::serialize();

	char *pChar = (char *) &pkgId_;
	for (unsigned int i = 0; i < sizeof(pkgId_); i++)
	{
		serializedData.append(1, pChar[i]);
	}	

	pChar = (char *) &ngwState_;
	for (unsigned int i = 0; i < sizeof(ngwState_); i++)
	{
		serializedData.append(1, pChar[i]);
	}	
	return serializedData.data();
}

void NGWWrapperResume::show()
{
    Debug::trace("NGWWrapperPause","ngwState_: " + Integer::toString(ngwState_) );    
}
