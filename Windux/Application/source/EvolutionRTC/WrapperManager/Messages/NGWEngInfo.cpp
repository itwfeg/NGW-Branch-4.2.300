/**
 * @file NGWEngInfo.h
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Aaron Swift
 * @brief CAN Wrapper message containing source image
 * Created on: Mar 9, 2018
 *
 **/
#include "NGWEngInfo.h"
#include "WrapperEvent.h"
#include "Debug.h"
#include "Integer.h"
#include "Hex.h"

NGWEngInfo::NGWEngInfo( Hobart::CEvolMsgReceiver *pHobThread, EngInfoType infoType ) : EvolMessage( Wrapper, WrNGWEngInfoN1, pHobThread )
{
	type = infoType;
	memset( &info[0], '0', sizeof(info) );
}

NGWEngInfo::NGWEngInfo(const char *buffer) : EvolMessage(buffer)
{

	EvolMessage::serialize();

	int j = getSerializedLength();

	// Now, for the derived class items
	char *pChar = (char *) &type;
	for (unsigned int i = 0; i < sizeof(type); i++)
	{
		*pChar++ = buffer[j++];
	}

	for (unsigned int i = 0; i < EngInfoSize; i++)
	{
		info[i] = buffer[j++];
	}
}

NGWEngInfo::~NGWEngInfo()
{
}

void NGWEngInfo::setEngInfo( unsigned char *pData, int length )
{
	if( length < EngInfoSize ) {
		for( int i = 0; i < length; i++ ) {
			info[i] = *pData++;
		}
	} else {
		Debug::trace( "NGWEngInfo", "Info data to large! "  + Integer::toString(length) );
	}
}

std::string NGWEngInfo::getEngInfo()
{
	std::string engInfo;


	switch(type)
	{
		case _NGW_ENG_INFO0:
		case _NGW_ENG_INFO1:{		// just text
			engInfo = "Camera board diagnostics " + Integer::toString(type) + " ";
			char *pC = (char *)&info[0];
			for(int i = 0; i < EngInfoSize; i++ ) {

				engInfo.append( 1,  pC[i] );
			}
			break;
		}
		case _NGW_ENG_INFO2:{		//text and 1 value
			engInfo = "Camera board diagnostics " + Integer::toString(type) + " ";

			//search for the end of our string
			int strLen = 0;
			for( int i = 0; i < EngInfoSize; i++  ){

				if( info[i] == '/' ) {
					break;
				} else {
					strLen++;
				}
			}
			//text
			char *pC = (char *)&info[0];
			for(int i = 0; i < strLen; i++ ) {

				engInfo.append( 1,  pC[i] );
			}
			//sub in space for marker
			engInfo += ' ';
			strLen++;

			unsigned short x = info[strLen];
			x <<= 8;
			x |= info[strLen + 1];

			engInfo +=Integer::toString( x );
			break;
		}
		case _NGW_ENG_INFO3: {		//text, value, text, text, text, text, value, value, value,
			engInfo = "Camera board diagnostics " + Integer::toString(type) + " ";
			char *pC = (char *)&info[0];
			//text
			engInfo.append( 1,  pC[0] );
			//sub in space for marker
			engInfo += ':';
			//value
			unsigned short x = info[2];
			x <<= 8;
			x |= info[3];
			engInfo +=Integer::toString( x );
			engInfo += ' ';

			//text
			engInfo.append( 1,  pC[4] );
			engInfo.append( 1,  pC[5] );
			engInfo.append( 1,  pC[6] );
			//sub in space for marker
			engInfo += ':';
			//value
			x = info[8];
			x <<= 8;
			x |= info[9];
			engInfo +=Integer::toString( x );

			//next value
			engInfo += ',';

			x = info[10];
			x <<= 8;
			x |= info[11];
			engInfo +=Integer::toString( x );

			//next value
			engInfo += ',';

			//value
			x = info[12];
			x <<= 8;
			x |= info[13];
			engInfo +=Integer::toString( x );
			break;
		}
		case _NGW_ENG_INFO4:{
			break;
		}
		case _NGW_ENG_INFO5: {
			break;
		}
		case _NGW_ENG_INFO6: {
			break;
		}
		case _NGW_ENG_INFO7: {
			break;
		}
		case _NGW_ENG_INFO8:{
			break;
		}
		case _NGW_ENG_INFO9:{
			break;
		}
		case _NGW_ENG_INFO10:{
			break;
		}
		case _NGW_ENG_INFO11:{
			break;
		}
		case _NGW_ENG_INFO12:{
			break;
		}
		case _NGW_ENG_INFO13:{
			break;
		}
		case _NGW_ENG_INFO14:{
			break;
		}
		case _NGW_ENG_INFO15:{
			break;
		}
		case _NGW_ENG_INFO16:{
			break;
		}
		case _NGW_ENG_INFO17:{
			break;
		}
		case _NGW_ENG_INFO18:{
			break;
		}
		case _NGW_ENG_INFO19:{
			break;
		}
		case _NGW_ENG_INFO20:{
			break;
		}
		default: {
			Debug::trace( "NGWEngInfo", "Unknown type!" );
		}
	}
	return engInfo;
}

const char *NGWEngInfo::serialize() const
{
	EvolMessage::serialize();

	// Now, for the derived class items
	char *pChar = (char *) &type;
	for (unsigned int i = 0; i < sizeof(type); i++)
	{
		serializedData.append(1, pChar[i]);
	}


	pChar = (char *)&info[0];
	for (unsigned int i = 0; i < EngInfoSize; i++)
	{
		serializedData.append(1, pChar[i] );
	}

	return serializedData.data();

}






