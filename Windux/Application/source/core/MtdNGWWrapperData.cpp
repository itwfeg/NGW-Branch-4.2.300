/**
 * @file MtdNGWWrapperData.cpp
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Aaron Swift
 * @brief HTi serial flash new generation wrapper section
 * Created on: july 21, 2016
 */
#include "MtdNGWWrapperData.h"
#include "MessageFormat.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"
#include "Debug.h"
#include "Hex.h"
#include <iostream>


MtdNGWWrapperData::MtdNGWWrapperData()
{
	init();
}

MtdNGWWrapperData::MtdNGWWrapperData(const std::string &s)
{
	std::string startTag( "<NGWSection>" );
	std::string endTag( "</NGWSection>" );

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
	for( int i = 0; i < MtdNGWWrapperData::XML_EndManElements; i++ )
	{
		switch(i)
		{
			case MtdNGWWrapperData::XML_InstallDate:
			{
				startTag = 	"<InstallDate>";
				endTag = 	"</InstallDate>";
				break;
			}
			case MtdNGWWrapperData::XML_PrimaryPrinterCounts:
			{
				startTag = 	"<PriPrinterCounts>";
				endTag = 	"</PriPrinterCounts>";
				break;
			}
			case MtdNGWWrapperData::XML_PrimaryLabelRollCounts:
			{
				startTag = 	"<PriLabelRollCounts>";
				endTag = 	"</PriLabelRollCounts>";
				break;
			}
			case MtdNGWWrapperData::XML_SecondaryPrinterCounts:
			{
				startTag = 	"<SecPrinterCounts>";
				endTag = 	"</SecPrinterCounts>";
				break;
			}
			case MtdNGWWrapperData::XML_SecondaryLabelRollCounts:
			{
				startTag = 	"<SecLabelRollCounts>";
				endTag = 	"</SecLabelRollCounts>";
				break;
			}
			case MtdNGWWrapperData::XML_TertiaryPrinterCounts:
			{
				startTag = 	"<TertPrinterCounts>";
				endTag = 	"</TertPrinterCounts>";
				break;
			}
			case MtdNGWWrapperData::XML_TertiaryLabelRollCounts:
			{
				startTag = 	"<TertLabelRollCounts>";
				endTag = 	"</TertLabelRollCounts>";
				break;
			}
			case MtdNGWWrapperData::XML_PowerOnOffCounts:
			{
				startTag = 	"<PwrOnOffCOunts>";
				endTag = 	"</PwrOnOffCOunts>";
				break;
			}
			case MtdNGWWrapperData::XML_UpperFilmPercentUsage:
			{
				startTag = 	"<UpFilmPercUsage>";
				endTag = 	"</UpFilmPercUsage>";
				break;
			}
			case MtdNGWWrapperData::XML_LowerFilmPercentUsage:
			{
				startTag = 	"<LwFilmPercUsage>";
				endTag = 	"</LwFilmPercUsage>";
				break;
			}
			case MtdNGWWrapperData::XML_CompressorRunTime:
			{
				startTag = 	"<CompRunTime>";
				endTag = 	"</CompRunTime>";
				break;
			}
			case MtdNGWWrapperData::XML_VacumRunTime:
			{
				startTag = 	"<VacRunTime>";
				endTag = 	"</VacRunTime>";
				break;
			}
			case MtdNGWWrapperData::XML_CycleCounts:
			{
				startTag = 	"<CycleCount>";
				endTag = 	"</CycleCount>";
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
					else if ( startTag == "<PriLabelRollCounts>" )
						primaryLabelRollCounter_  = strtol(t.data(), NULL, 10);
					else if ( startTag == "<SecPrinterCounts>" )
						secondaryPrinterCounter_ = strtol(t.data(), NULL, 10);
					else if ( startTag == "<SecLabelRollCounts>" )
						secondaryLabelRollCounter_ = strtol(t.data(), NULL, 10);
					else if ( startTag == "<TertPrinterCounts>" )
						tertiaryPrinterCounter_ = strtol(t.data(), NULL, 10);
					else if ( startTag == 	"<TertLabelRollCounts>" )
						tertiaryLabelRollCounter_ = strtol(t.data(), NULL, 10);
					else if ( startTag == "<PwrOnOffCOunts>" )
						powerOnOffCounter_ = strtol(t.data(), NULL, 10);
					else if ( startTag == "<UpFilmPercUsage>" )
						upperFilmPercentUsage_ = (unsigned short)strtol(t.data(), NULL, 10);
					else if ( startTag == "<LwFilmPercUsage>")
						lowerFilmPercentUsage_ = (unsigned short)strtol(t.data(), NULL, 10);
					else if ( startTag == "<CompRunTime>" )
						compressorRunTime_ = strtol(t.data(), NULL, 10);
					else if (startTag == "<VacRunTime>" )
						VacuumRunTime_ = strtol(t.data(), NULL, 10);
					else if(startTag == "<CycleCount>")
						cycleCounter_ = strtol(t.data(), NULL, 10);
				}
			}
			else
			{
				Debug::trace("MtdNGWWrapperData", "Starting tag could not be found! " + s);
			}
		}
		else
		{
			Debug::trace("MtdNGWWrapperData", "Unrecognized case!");
		}
	}
}

MtdNGWWrapperData::~MtdNGWWrapperData()
{

}

MtdNGWWrapperData::MtdNGWWrapperData(MtdNGWWrapperData *pWrapperData)
{
	installDate_ 					= pWrapperData->getInstallDate();
	primaryPrinterCounter_ 			= pWrapperData->getPrimaryPrinterCounts();
	primaryLabelRollCounter_		= pWrapperData->getPrimaryLabelRollCounter();
	secondaryPrinterCounter_ 		= pWrapperData->getSecondaryPrinterCounts();
	secondaryLabelRollCounter_		= pWrapperData->getSecondaryLabelRollCounter();
	tertiaryPrinterCounter_ 		= pWrapperData->getTertiaryPrinterCounts();
	tertiaryLabelRollCounter_		= pWrapperData->getTertiaryLabelRollCounter();
	powerOnOffCounter_ 				= pWrapperData->getPowerOnOffCounts();
	upperFilmPercentUsage_ 			= pWrapperData->getUpperFilmPercentUsage();
	lowerFilmPercentUsage_ 			= pWrapperData->getLowerFilmPercentUsage();
	compressorRunTime_ 				= pWrapperData->getCompressorRunTime();
	VacuumRunTime_ 					= pWrapperData->getVacuumRunTime();
	cycleCounter_					= pWrapperData->getCycleCounts();
	crc_ 							= getCrc();
}

void MtdNGWWrapperData::init()
{
	installDate_ = "";
	primaryPrinterCounter_ = 0;
	primaryLabelRollCounter_ = 0;
	secondaryPrinterCounter_ = 0;
	secondaryLabelRollCounter_ = 0;
	tertiaryPrinterCounter_ = 0;
	tertiaryLabelRollCounter_ = 0;
	powerOnOffCounter_ = 0;
	upperFilmPercentUsage_ = 0;
	lowerFilmPercentUsage_ = 0;
	compressorRunTime_ = 0;
	VacuumRunTime_ = 0;
	cycleCounter_ = 0;
	crc_ = 0;
}

unsigned long MtdNGWWrapperData::calculateCrc()
{
	crc_ = 0;
	serializeXml();
	return crc_;
}

const std::string MtdNGWWrapperData::serializeXml() const
{
	serializedData_.clear();
	crc_ = 0;

	serializedData_ = "<NGWSection>";

	serializedData_	+= "<InstallDate>";
	serializedData_	+= installDate_;
	serializedData_	+= "</InstallDate>";

	serializedData_	+= "<PriPrinterCounts>";
	serializedData_	+= Integer::toString64(primaryPrinterCounter_);
	serializedData_	+= "</PriPrinterCounts>";

	serializedData_	+= "<PriLabelRollCounts>";
	serializedData_	+= Integer::toString64(primaryLabelRollCounter_);
	serializedData_	+= "</PriLabelRollCounts>";

	serializedData_	+= "<SecPrinterCounts>";
	serializedData_ += Integer::toString64(secondaryPrinterCounter_);
	serializedData_	+= "</SecPrinterCounts>";

	serializedData_	+= "<SecLabelRollCounts>";
	serializedData_	+= Integer::toString64(secondaryLabelRollCounter_);
	serializedData_	+= "</SecLabelRollCounts>";

	serializedData_	+= "<TertPrinterCounts>";
	serializedData_	+= Integer::toString64(tertiaryPrinterCounter_);
	serializedData_	+= "</TertPrinterCounts>";

	serializedData_	+= "<TertLabelRollCounts>";
	serializedData_	+= Integer::toString64(tertiaryLabelRollCounter_);
	serializedData_	+= "</TertLabelRollCounts>";

	serializedData_	+= "<PwrOnOffCOunts>";
	serializedData_	+= Integer::toString64(powerOnOffCounter_);
	serializedData_	+=  "</PwrOnOffCOunts>";

	serializedData_	+= "<UpFilmPercUsage>";
	serializedData_ += Integer::toString(upperFilmPercentUsage_);
	serializedData_	+= "</UpFilmPercUsage>";

	serializedData_	+= "<LwFilmPercUsage>";
	serializedData_ += Integer::toString(lowerFilmPercentUsage_);
	serializedData_	+= "</LwFilmPercUsage>";

	serializedData_	+= "<CompRunTime>";
	serializedData_ += Integer::toString64(compressorRunTime_);
	serializedData_	+= "</CompRunTime>";

	serializedData_	+= "<VacRunTime>";
	serializedData_ += Integer::toString64(VacuumRunTime_);
	serializedData_	+= "</VacRunTime>";

	serializedData_	+= "<CycleCount>";
	serializedData_ += Integer::toString64(cycleCounter_);
	serializedData_	+= "</CycleCount>";

	serializedData_	+= "</NGWSection>";

	Debug::trace("MtdNGWWrapperData", "Serialized: " + serializedData_);

	/*calculate our crc value for the serialized data*/
	const unsigned char *pSbuffer = (const unsigned char *)serializedData_.data();
	for( int i = 0; i < (int)serializedData_.size(); i++ )
	{
		crc_ +=	*pSbuffer++;
	}
	Debug::trace("MtdNGWWrapperData", "Crc value to flash: " + Integer::toString(crc_));
	Debug::trace("MtdNGWWrapperData", "Crc string size: " + Integer::toString(serializedData_.size()));
	return serializedData_;

}

void MtdNGWWrapperData::setWrapperDataDefaults()
{
	installDate_ = "";
	primaryPrinterCounter_ = 0;
	primaryLabelRollCounter_ = 0;
	secondaryPrinterCounter_ = 0;
	secondaryLabelRollCounter_ = 0;
	tertiaryPrinterCounter_ = 0;
	tertiaryLabelRollCounter_ = 0;
	powerOnOffCounter_ = 0;
	upperFilmPercentUsage_ = 0;
	lowerFilmPercentUsage_ = 0;
	compressorRunTime_ = 0;
	VacuumRunTime_ = 0;
	cycleCounter_ = 0;
	crc_ = 0;
}

void MtdNGWWrapperData::show()
{
	Debug::trace("MtdNGWWrapperData", "installDate_ " + installDate_);
	Debug::trace("MtdNGWWrapperData", "primaryPrinterCounter_ " + Integer::toString(primaryPrinterCounter_));
	Debug::trace("MtdNGWWrapperData", "primaryLabelRollCounter_" + Integer::toString(primaryLabelRollCounter_));
	Debug::trace("MtdNGWWrapperData", "secondaryPrinterCounter_ " + Integer::toString(secondaryPrinterCounter_));
	Debug::trace("MtdNGWWrapperData", "secondaryLabelRollCounter_" + Integer::toString(secondaryLabelRollCounter_));
	Debug::trace("MtdNGWWrapperData", "tertiaryPrinterCounter_ " +  Integer::toString(tertiaryPrinterCounter_));
	Debug::trace("MtdNGWWrapperData", "tertiaryLabelRollCounter_" +  Integer::toString(tertiaryLabelRollCounter_));
	Debug::trace("MtdNGWWrapperData", "powerOnOffCounter_ " +  Integer::toString(powerOnOffCounter_));
	Debug::trace("MtdNGWWrapperData", "upperFilmPercentUsage_ " +  Integer::toString(upperFilmPercentUsage_));
	Debug::trace("MtdNGWWrapperData", "lowerFilmPercentUsage_ " +  Integer::toString(lowerFilmPercentUsage_));
	Debug::trace("MtdNGWWrapperData", "compressorRunTime_ " +  Integer::toString(compressorRunTime_));
	Debug::trace("MtdNGWWrapperData", "VacuumRunTime_ " +  Integer::toString(VacuumRunTime_));
	Debug::trace("MtdNGWWrapperData", "cycleCounter_ " +  Integer::toString(cycleCounter_));
}

int MtdNGWWrapperData::dataOffset()
{
	struct stat buffer;

	// ARK
	// we have dedicated partition in ARK so check for exist
	if ( 0 == stat( "/dev/mtd9", &buffer ) )
		return 0;

	return SIZEOF_UBOOTBITMAPS;
}

