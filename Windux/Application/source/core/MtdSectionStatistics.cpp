/**
 * @file MtdSectionStatistics.cpp
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Aaron Swift
 * @brief HTi serial flash statistics section
 * Created on: Dec 3, 2012
 */

#include "MtdSectionStatistics.h"
#include "Integer.h"
#include "Long.h"
#include "Debug.h"

MtdSectionStatistics::MtdSectionStatistics()
{
	sectionMarker_ = 0;
	sectorType_ = 0;
	writeCounter_ = 0;
	crc_ = 0;
}

MtdSectionStatistics::MtdSectionStatistics(const std::string &s)
{
	std::string value;

	std::size_t pos( 0 );
	std::size_t posEnd;

	if ( std::string::npos == (posEnd = s.find( ",", pos )) )
	{
		Debug::trace( "FpmManager.read", "Section Statistics parse error");
		return;
	}

	value = s.substr( pos, posEnd - pos );
	sectionMarker_ = (unsigned int) strtol( value.c_str(), NULL, 10 );

	pos = posEnd + 1;

	if ( std::string::npos == (posEnd = s.find( ",", pos )) )
	{
		Debug::trace( "FpmManager.read", "Section Statistics parse error");
		return;
	}

	value = s.substr( pos, posEnd - pos );
	sectorType_ = (unsigned int) strtol( value.c_str(), NULL, 10 );

	pos = posEnd + 1;

	if ( std::string::npos == (posEnd = s.find( ",", pos )) )
	{
		Debug::trace( "FpmManager.read", "Section Statistics parse error");
		return;
	}

	value = s.substr( pos, posEnd - pos );
	writeCounter_ = (unsigned int) strtol( value.c_str(), NULL, 10 );

	pos = posEnd + 1;
	if ( std::string::npos == (posEnd = s.find( "<", pos )) )
	{
		Debug::trace( "FpmManager.read", "Section Statistics parse error");
		return;
	}

	value = s.substr( pos, posEnd - pos );
	crc_ = (unsigned int) strtol( value.c_str(), NULL, 10 );
}

MtdSectionStatistics::~MtdSectionStatistics()
{

}

bool MtdSectionStatistics::isSectionBlank()
{
	if(sectionMarker_ == 0)
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool MtdSectionStatistics::isSectionDataValid(unsigned long dataCrc)
{
	bool valid = false;
	if(dataCrc == crc_)
	{
		valid = true;
	}
	else
	{
		Debug::trace( "FpmManager.read", "Section Statistics calculated crc: " + Integer::toString(dataCrc) + " saved crc: " + Integer::toString(crc_) );
	}
	return valid;
}


const std::string MtdSectionStatistics::serialize() const
{
	serializedData_.clear();
	serializedData_ = Integer::toString(sectionMarker_);
	serializedData_ += ",";
	serializedData_ += Integer::toString(sectorType_);
	serializedData_ += ",";
	serializedData_ += Long::toString(writeCounter_);
	serializedData_ += ",";
	serializedData_ += Long::toString(crc_);

	return serializedData_;
}
