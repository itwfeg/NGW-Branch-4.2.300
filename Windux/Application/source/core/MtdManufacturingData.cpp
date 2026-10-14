/**
 * @file MtdManufacturingData.cpp
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Aaron Swift
 * @brief HTi serial flash manufacturing section
 * Created on: Nov 2, 2012
 */

#include "MtdManufacturingData.h"
#include "MessageFormat.h"
#include "HobTokenizerCSV.h"
#include "Integer.h"
#include "Debug.h"
#include "Hex.h"

#include "Configuration.h"
#include "PythonScripter.h"

MtdManufacturingData::MtdManufacturingData()
{
	init();
}

MtdManufacturingData::MtdManufacturingData(const std::string &s)
{
	std::string startTag;
	std::string endTag;

	init();

	/*!calculate the crc value of the section data*/
	std::size_t crcPos = s.find('<');
	if( crcPos != std::string::npos )
	{
		endTag = "</MFGSection>";
		/*!index passed statistics portion of the string*/
		std::string y = s.substr( crcPos );

		std::size_t crcEndPos = y.find(endTag);	/*chop off the junk at the end of the string */
		crcEndPos += endTag.size();				/*include the section end tag into our calculation*/
		unsigned char *pSbuffer = (unsigned char *)y.data();

		crc_ = 0;
		for( int i = 0; i < crcEndPos; i++ )
		{
			crc_ += *pSbuffer++;
		}
	}

	/*! parse the xml data from the serial flash */
	for( int i = 0; i < MtdManufacturingData::XML_EndManElements; i++ )
	{
		switch(i)
		{
			case MtdManufacturingData::XML_ScaleStyle:
			{
				startTag = 	"<SStyle>";
				endTag = 	"</SStyle>";
				break;
			}
			case MtdManufacturingData::XML_BoardAssembly:
			{
				startTag = 	"<BDAssembly>";
				endTag = 	"</BDAssembly>";
				break;
			}
			case MtdManufacturingData::XML_BoardRevision:
			{
				startTag = 	"<BDRev>";
				endTag = 	"</BDRev>";
				break;
			}
			case MtdManufacturingData::XML_DateOfManufacture:
			{
				startTag = 	"<MFGDate>";
				endTag = 	"</MFGDate>";
				break;
			}
			case MtdManufacturingData::XML_SerialNumber:
			{
				startTag = 	"<Serial>";
				endTag = 	"</Serial>";
				break;
			}
			case MtdManufacturingData::XML_SystemStyle:
			{
				startTag = 	"<SystemStyle>";
				endTag = 	"</SystemStyle>";
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
			// find the start and end tag
			std::size_t spos( s.find(startTag) );
			std::size_t epos( s.find(endTag) );

			if (( std::string::npos != spos ) && ( std::string::npos != epos ))
			{
				spos += startTag.size();

				if ( spos < epos )
				{
					if (startTag == "<SStyle>")
						scaleStyle_ = s.substr(spos, epos-spos);
					else if ( startTag == "<BDAssembly>" )
						boardAssembly_ = s.substr(spos, epos-spos);
					else if ( startTag == "<BDRev>" )
						boardRevision_ = s.substr(spos, epos-spos);
					else if (startTag == "<MFGDate>")
						dateOfManufacture_ = s.substr(spos, epos-spos);
					else if (startTag == "<Serial>")
						serialNumber_ = s.substr(spos, epos-spos);
					else if (startTag == "<SystemStyle>")
						systemStyle_ = s.substr(spos, epos-spos);
				}
			}
			else
			{
				Debug::trace("MtdManufacturingData", "Starting tag could not be found! " + s);
			}
		}
		else
		{
			Debug::trace("MtdManufacturingData", "Unrecognized case!");
		}
	}
}

MtdManufacturingData::~MtdManufacturingData()
{

}

MtdManufacturingData::MtdManufacturingData(MtdManufacturingData *pMfgData)
{
	scaleStyle_			= pMfgData->getScaleStyle();
	boardAssembly_		= pMfgData->getBoardAssembly();
	boardRevision_		= pMfgData->getBoardRevision();
	dateOfManufacture_	= pMfgData->getDateOfManufacturing();
	serialNumber_		= pMfgData->getSerialNumber();
	systemStyle_		= pMfgData->getSystemStyle();
	crc_ 				= getCrc();
}

void MtdManufacturingData::init()
{
	scaleStyle_ = "EPCP-5I1U";
	boardAssembly_ 	= "";
	boardRevision_ 	= "";
	serialNumber_ 	= "";
	systemStyle_ 	= "";
	crc_ = 0;
}

const HobString MtdManufacturingData::getScaleStyle() const
{
	HobString s( scaleStyle_ );
	s.unQuote();

	return s;
}

void MtdManufacturingData::setManufacturingDataDefaults()
{
	/*
	 * Scale style determines which components the scale is attached.
	 * 		EPCP-X X X X
	 * 			 ^ ^ ^ ^
	 * 			 | | | |__ Power: U = 198v - 242v, T = 187v - 197v or 243v - 254v, I = International (Chile)
	 * 			 | | |__ Generation: 1 = BCM CAN1 USB SAM7, 2 = ACR CAN1 USB SAM7
	 * 			 | |__ Processor: S = Solo ARM, I = Dual ARM, Q = Quad ARM, X = x86
	 * 			 |__ OS: 5 = Linux, 6 = Windows
	 *
	 * 		NGWX-X X X X XX XX
	 * 		   ^ ^ ^ ^ ^ ^  ^
	 * 		   | | | | | |  |__ Customer variants
	 * 		   | | | | | |__ Discharge
	 * 		   | | | | |__ Power
	 * 		   | | | |__ Generation
	 * 		   | | |__ Processor
	 * 		   | |__ OS
	 * 		   |__ #Rolls
	 *
	 *	Rolls: 1,2
	 *	OS: L,W
	 *	Processor: S,I,Q,X	S = Solo ARM, I = Dual ARM, Q = Quad ARM, X = x86
	 *	Generation: 1,2,3	1 = BCM CAN1 USB SAM7, 2 = ACR CAN1 USB SAM7, 3 = ACR CAN2 RS485 K64
	 *	Power: U,T,I		U = 198v - 242v, T = 187v - 197v or 243v - 254v, I = international (Chile)
	 *	Discharge: LR,RL 	LR = Left to Right, RL = Right to Left
	 *
	 *	EPCP-5I1 or EPCP-5I2: support AWS3 Enhanced
	 *	NGWX-XX1 or NGWX-XX2: support NGW1 or NGW2
	 *	NGWX-XX3:			  support NGW3
	 */

	bool isArkBoard = false;

	// detect ARK board
	if (Configuration::getInstance()->toBoolCfgI(CfgInternal::PYTHONENABLED))
	{
		const string command(
			PythonScripter::buildPythonCall(
				Configuration::getInstance()->toStringCfgI(CfgInternal::PYTHONGETBOARDTYPESCRIPTNAME)));

		string result;

		// get board type
		PAL_absclass::PAL_executeWithResults(command, result);

		// result format:
		//   X,Y,Z
		// where:
		//   X = 0 or 1 if ARA board
		//   Y = 0 or 1 if BCM board
		//   Z = 0 or 1 if ARK board
		if (5 <= result.length())
			if ('1' == result[4])
				isArkBoard = true;
	}

	if (isArkBoard)
	{
		scaleStyle_ = "HTm-7LS";
		systemStyle_ = "HTm";
	}
	else
	{
		scaleStyle_ = "HTi-7LS";
		systemStyle_ = "HTi";
	}

	boardAssembly_ = "45-1084-982";
	boardRevision_ = "1.0";
	dateOfManufacture_ = "01312017";
	serialNumber_ = "1234567";
}

unsigned long MtdManufacturingData::calculateCrc()
{
	crc_ = 0;
	serializeXml();
	return crc_;
}

const std::string MtdManufacturingData::serializeXml() const
{
	serializedData_.clear();
	crc_ = 0;

	serializedData_ = "<MFGSection>";

	serializedData_	+= "<SStyle>";
	serializedData_	+= scaleStyle_;
	serializedData_	+= "</SStyle>";
	serializedData_	+= "<BDAssembly>";
	serializedData_	+= boardAssembly_ ;
	serializedData_	+= "</BDAssembly>";
	serializedData_	+= "<BDRev>";
	serializedData_ += boardRevision_;
	serializedData_	+= "</BDRev>";
	serializedData_	+= "<MFGDate>";
	serializedData_	+= dateOfManufacture_;
	serializedData_	+= "</MFGDate>";
	serializedData_	+= "<Serial>";
	serializedData_	+= serialNumber_;
	serializedData_	+=  "</Serial>";
	serializedData_	+= "<SystemStyle>";
	serializedData_	+= systemStyle_;
	serializedData_	+=  "</SystemStyle>";

	serializedData_	+= "</MFGSection>";

	Debug::trace("MtdManufacturingData", "Serialized: " + serializedData_);

	/*calculate our crc value for the serialized data*/
	const unsigned char *pSbuffer = (const unsigned char *)serializedData_.data();
	for( int i = 0; i < (int)serializedData_.size(); i++ )
	{
		crc_ +=	*pSbuffer++;
	}
	Debug::trace("MtdManufacturingData", "Crc value to flash: " + Integer::toString(crc_));
	Debug::trace("MtdManufacturingData", "Crc string size: " + Integer::toString(serializedData_.size()));
	return serializedData_;

}

void MtdManufacturingData::show()
{
	Debug::trace("MtdManufacturingData", "scaleStyle_: " + scaleStyle_);
	Debug::trace("MtdManufacturingData", "boardAssembly_: " + boardAssembly_);
	Debug::trace("MtdManufacturingData", "boardRevision_: " + boardRevision_);
	Debug::trace("MtdManufacturingData", "dateOfManufacture_: " + dateOfManufacture_);
	Debug::trace("MtdManufacturingData", "serialNumber_: " + serialNumber_);
	Debug::trace("MtdManufacturingData", "systemStyle_: " + systemStyle_);
}
