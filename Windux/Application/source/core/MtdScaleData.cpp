/**
 * @file MtdScaleData.cpp
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Aaron Swift
 * @brief HTi serial flash scale data section
 * Created on: Feb 2, 2017
 */
#include "MtdScaleData.h"
#include "MessageFormat.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"
#include "Debug.h"
#include "Hex.h"
#include <iostream>

///////////////////////////////////////////////////////////////////////////////////////////////////
MtdScaleData::MtdScaleData()
{
	init();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
MtdScaleData::MtdScaleData(const std::string &s)
{
	std::string startTag( "<SCDataSection>" );
	std::string endTag( "</SCDataSection>" );

	init();

	// calculate the crc value of the section data
	std::size_t crcPos;

	if ( std::string::npos != (crcPos = s.find( startTag )) )
	{
		std::size_t crcEndPos( s.find( endTag, crcPos ) );

		if ( std::string::npos != crcEndPos )
		{
			const unsigned char *pSbuffer( &reinterpret_cast<const unsigned char*>( s.c_str() )[crcPos] );

			crcEndPos += endTag.size(); // include the section end tag into our calculation
			crcEndPos -= crcPos;

			crc_ = 0;

			while ( crcEndPos-- )
				crc_ += *pSbuffer++;
		}
	}

	// parse the xml data from the serial flash
	for( int i = 0; i < MtdScaleData::XML_EndSCElements; i++ )
	{
		switch(i)
		{
			case MtdScaleData::XML_SC_InstallDate:
			{
				startTag = 	"<InstallDate>";
				endTag = 	"</InstallDate>";
				break;
			}
			case MtdScaleData::XML_SC_PrimaryPrinterCounts:
			{
				startTag = 	"<PriPrinterCounts>";
				endTag = 	"</PriPrinterCounts>";
				break;
			}
			case MtdScaleData::XML_SC_SecondaryPrinterCounts:
			{
				startTag = 	"<SecPrinterCounts>";
				endTag = 	"</SecPrinterCounts>";
				break;
			}
			case MtdScaleData::XML_SC_TertiaryPrinterCounts:
			{
				startTag = 	"<TertPrinterCounts>";
				endTag = 	"</TertPrinterCounts>";
				break;
			}
			default:
			{
				startTag = "";
				break;
			}
		}

		if(startTag.size())
		{
			// find the start tag
			std::size_t spos( s.find(startTag) );
			std::size_t epos( s.find(endTag) );

			if (( std::string::npos != spos ) && ( std::string::npos != epos ))
			{
				spos += startTag.size();

				if ( spos < epos )
				{
					const std::string t( s.substr(spos, epos-spos));

					if (startTag == "<InstallDate>")
						installDate_ = t;
					else if ( startTag == "<PriPrinterCounts>" )
						primaryPrinterCounter_ = strtol(t.data(), NULL, 10);
					else if ( startTag == "<SecPrinterCounts>" )
						secondaryPrinterCounter_ = strtol(t.data(), NULL, 10);
					else if ( startTag == "<TertPrinterCounts>" )
						tertiaryPrinterCounter_ = strtol(t.data(), NULL, 10);
				}
			}
			else
			{
				Debug::trace("MtdScaleData", "Starting tag could not be found! " + s);
			}
		}
		else
		{
			Debug::trace("MtdScaleData", "Unrecognized case!");
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
MtdScaleData::~MtdScaleData()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
MtdScaleData::MtdScaleData(MtdScaleData *pScaleData)
{
	installDate_ 					= pScaleData->getInstallDate();
	primaryPrinterCounter_ 			= pScaleData->getPrimaryPrinterCounts();
	secondaryPrinterCounter_ 		= pScaleData->getSecondaryPrinterCounts();
	tertiaryPrinterCounter_ 		= pScaleData->getTertiaryPrinterCounts();
	crc_ 							= getCrc();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MtdScaleData::init()
{
	installDate_ = "";
	primaryPrinterCounter_ = 0;
	secondaryPrinterCounter_ = 0;
	tertiaryPrinterCounter_ = 0;
	crc_ = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
unsigned long MtdScaleData::calculateCrc()
{
	crc_ = 0;
	serializeXml();
	return crc_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
const std::string MtdScaleData::serializeXml() const
{
	serializedData_.clear();
	crc_ = 0;

	serializedData_ = "<SCDataSection>";

	serializedData_	+= "<InstallDate>";
	serializedData_	+= installDate_;
	serializedData_	+= "</InstallDate>";

	serializedData_	+= "<PriPrinterCounts>";
	serializedData_	+= Integer::toString64(primaryPrinterCounter_);

	serializedData_	+= "</PriPrinterCounts>";

	serializedData_	+= "<SecPrinterCounts>";
	serializedData_ += Integer::toString64(secondaryPrinterCounter_);
	serializedData_	+= "</SecPrinterCounts>";

	serializedData_	+= "<TertPrinterCounts>";
	serializedData_	+= Integer::toString64(tertiaryPrinterCounter_);
	serializedData_	+= "</TertPrinterCounts>";

	serializedData_	+= "</SCDataSection>";

	Debug::trace("MtdScaleData", "Serialized: " + serializedData_);

	/*calculate our crc value for the serialized data*/
	const unsigned char *pSbuffer = (const unsigned char *)serializedData_.data();
	for( int i = 0; i < (int)serializedData_.size(); i++ )
	{
		crc_ +=	*pSbuffer++;
	}
	Debug::trace("MtdScaleData", "Crc value to flash: " + Integer::toString(crc_));
	Debug::trace("MtdScaleData", "Crc string size: " + Integer::toString(serializedData_.size()));
	return serializedData_;

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MtdScaleData::setScaleDataDefaults()
{
	installDate_ = "";
	primaryPrinterCounter_ = 0;
	secondaryPrinterCounter_ = 0;
	tertiaryPrinterCounter_ = 0;
	crc_ = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MtdScaleData::show()
{
	Debug::trace("MtdScaleData", "installDate_ " + installDate_);
	Debug::trace("MtdScaleData", "primaryPrinterCounter_ " + Integer::toString(primaryPrinterCounter_));
	Debug::trace("MtdScaleData", "secondaryPrinterCounter_ " + Integer::toString(secondaryPrinterCounter_));
	Debug::trace("MtdScaleData", "tertiaryPrinterCounter_ " +  Integer::toString(tertiaryPrinterCounter_));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int MtdScaleData::dataOffset()
{
	struct stat buffer;

	// ARK
	// we have dedicated partition in ARK so check for exist
	if ( 0 == stat( "/dev/mtd9", &buffer ) )
		return 0;

	return SIZEOF_UBOOTBITMAPS;
}


