/**
 * @file WrPPPRequest.cpp
 * @copyright Copyright (c) 2021 Hobart Corporation
 * @author Aaron Swift
 * @brief Printer plate position request message
 * Created on: Mar 9, 2021
 */
#include "WrPPPRequest.h"
#include "Integer.h"
#include "Debug.h"

PPPRequest::PPPRequest( Hobart::CEvolMsgReceiver *pHobThread, PPPReq request, unsigned short data ) : WrapperGeneric( WrPPPRequest, pHobThread )
{
    request_ = request;
	data_ = data;
}

PPPRequest::PPPRequest( const char *buffer ) : WrapperGeneric( buffer )
{

	WrapperGeneric::serialize();
	int j = getSerializedLength();

	char *pChar = (char *) &request_;
	for (unsigned int i = 0; i < sizeof(request_); i++)
	{
		*pChar++ = buffer[j++];
	}
	
	pChar = (char *) &data_;
	for (unsigned int i = 0; i < sizeof(data_); i++)
	{
		*pChar++ = buffer[j++];
	}
}

PPPRequest::~PPPRequest()
{

}

const char *PPPRequest::serialize() const
{
	WrapperGeneric::serialize();

	char *pChar = (char *) &request_;
	for (unsigned int i = 0; i < sizeof(request_); i++)
	{
		serializedData.append(1, pChar[i]);
	}	
	pChar = (char *) &data_;
	for (unsigned int i = 0; i < sizeof(data_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void PPPRequest::show()
{
    Debug::trace("PPPRequest","request_: " + Integer::toString(request_) );    
}

