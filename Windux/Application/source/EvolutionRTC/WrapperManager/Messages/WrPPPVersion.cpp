/**
 * @file WrPPPVersion.cpp
 * @copyright Copyright (c) 2021 Hobart Corporation
 * @author Aaron Swift
 * @brief Printer plate position version message
 * Created on: Mar 17, 2021
 */
#include "WrPPPVersion.h"
#include "Integer.h"
#include "Hex.h"
#include "Debug.h"

PPPVersion::PPPVersion( Hobart::CEvolMsgReceiver *pHobThread ) : WrapperGeneric( WrPPPVersion, pHobThread )
{      
    pid_     = 0;
    swMajor_ = 0;
    swMinor_ = 0;
    swbuild_ = 0;

    hwMajor_ = 0;
    hwMinor_ = 0;

	blMajor_ = 0;
	blMinor_ = 0;
	blBuild_ = 0;
}

PPPVersion::PPPVersion( const char *buffer ) : WrapperGeneric( buffer )
{

	WrapperGeneric::serialize();
	int j = getSerializedLength();

	char *pChar = (char *) &pid_;
	for (unsigned int i = 0; i < sizeof(pid_); i++)
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

	pChar = (char *) &blMajor_;
	for (unsigned int i = 0; i < sizeof(blMajor_); ++i)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &blMinor_;
	for (unsigned int i = 0; i < sizeof(blMinor_); ++i)
	{
		*pChar++ = buffer[j++];
	}

	pChar = (char *) &blBuild_;
	for (unsigned int i = 0; i < sizeof(blBuild_); ++i)
	{
		*pChar++ = buffer[j++];
	}
}

PPPVersion::~PPPVersion()
{

}
std::string PPPVersion::getSoftwareVersion() const
{
	std::string str;
	str = Integer::toString(swMajor_);
	str += ".";
	str += Integer::toString(swMinor_);
	str += ".";
	str += Integer::toString(swbuild_);
	return str;
}

std::string PPPVersion::getHardwareVersion() const
{
	std::string str;
	str = Integer::toString(hwMajor_);
	str += ".";
	str += Integer::toString(hwMinor_);
	return str;
}

std::string PPPVersion::getBootVersion() const
{
	std::string str;
	str = Integer::toString(blMajor_);
	str += ".";
	str += Integer::toString(blMinor_);
	str += ".";
	str += Integer::toString(blBuild_);
	return str;
}

const char *PPPVersion::serialize() const
{
	WrapperGeneric::serialize();

    char * pChar = (char *) &pid_;
	for (unsigned int i = 0; i < sizeof(pid_); i++)
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

	pChar = (char *) &blMajor_;
	for(unsigned int i = 0; i < sizeof(blMajor_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &blMinor_;
	for(unsigned int i = 0; i < sizeof(blMinor_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	pChar = (char *) &blBuild_;
	for(unsigned int i = 0; i < sizeof(blBuild_); i++)
	{
		serializedData.append(1, pChar[i]);
	}

	return serializedData.data();
}

void PPPVersion::show()
{
    Debug::trace("PPPVersion","pid: " + Hex::toString(pid_) );    
    Debug::trace("PPPVersion","swMajor: " + Integer::toString(swMajor_) );    
    Debug::trace("PPPVersion","swMinor: " + Integer::toString(swMinor_) );    
    Debug::trace("PPPVersion","swbuild: " + Integer::toString(swbuild_) );    
    Debug::trace("PPPVersion","hwMajor: " + Integer::toString(hwMajor_) );    
    Debug::trace("PPPVersion","hwMinor: " + Integer::toString(hwMinor_) );
	Debug::trace("PPPVersion","blMajor: " + Integer::toString(blMajor_) );
	Debug::trace("PPPVersion","blMinor: " + Integer::toString(blMinor_) );
	Debug::trace("PPPVersion","blBuild: " + Integer::toString(blBuild_) );
}

