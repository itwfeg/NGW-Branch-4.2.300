/**
 * @file MtdScaleHwdCfg.cpp
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Aaron Swift
 * @brief HTi serial flash hardware configuration section
 * Created on: Oct 31, 2012
 */

#include "MtdScaleHwCfg.h"
#include "Bool.h"
#include "Integer.h"
#include "Debug.h"
#include "Hex.h"

MtdScaleHwCfg::MtdScaleHwCfg()
{
	init();
}

MtdScaleHwCfg::~MtdScaleHwCfg()
{

}

MtdScaleHwCfg::MtdScaleHwCfg( const std::string &raw )
{
	std::string startTag( "<ScaleHWSection>" );
	std::string endTag( "</ScaleHWSection>" );

	init();

	// calculate the crc value of the section data
	std::string s( raw );

	const std::size_t crcPos( raw.find( startTag ) );

	if ( crcPos != std::string::npos )
	{
		std::string y( raw.substr( crcPos ) );

		std::size_t crcEndPos( y.find( endTag ) );

		if ( crcEndPos != std::string::npos )
		{
			crcEndPos += endTag.size();
			unsigned char *pSbuffer = (unsigned char *)y.data();

			crc_ = 0;
			for ( int i = 0; i < crcEndPos; i++ )
				crc_ += *pSbuffer++;

			y.resize( crcEndPos );
			s = y;
		}
	}

	for( int i = 0; i < MtdScaleHwCfg::XML_HWENDELEMENTS; i++ )
	{
		switch(i)
		{
			case MtdScaleHwCfg::XML_PRIDISPLAYTYPE:
			{
				startTag = 	"<PriDspType>";
				endTag = 	"</PriDspType>";
				break;
			}
			case MtdScaleHwCfg::XML_PRIDISPLAYSIZE:
			{
				startTag = 	"<PriDspSize>";
				endTag = 	"</PriDspSize>";
				break;
			}
			case MtdScaleHwCfg::XML_PRI_INTERFACE:
			{
				startTag = 	"<PriInter>";
				endTag = 	"</PriInter>";
				break;
			}
			case MtdScaleHwCfg::XML_SECDISPLAYTYPE:
			{
				startTag = 	"<SecDspType>";
				endTag = 	"</SecDspType>";
				break;
			}
			case MtdScaleHwCfg::XML_SECDISPLAYSIZE:
			{
				startTag = 	"<SecDspSize>";
				endTag = 	"</SecDspSize>";
				break;
			}
			case MtdScaleHwCfg::XML_SEC_INTERFACE:
			{
				startTag = 	"<SecInter>";
				endTag = 	"</SecInter>";
				break;
			}
			case MtdScaleHwCfg::XML_SATADRIVEINS:
			{
				startTag = 	"<SataIns>";
				endTag = 	"</SataIns>";
				break;
			}
			case MtdScaleHwCfg::XML_WIFIINS:
			{
				startTag = 	"<WifiIns>";
				endTag = 	"</WifiIns>";
				break;
			}
			case MtdScaleHwCfg::XML_BLUETOOTH_INS:
			{
				startTag = 	"<BlueIns>";
				endTag = 	"</BlueIns>";
				break;
			}
			case MtdScaleHwCfg::XML_POWERHOLD_INS:
			{
				startTag = 	"<PowerH>";
				endTag = 	"</PowerH>";
				break;
			}
			case MtdScaleHwCfg::XML_STOUCH_INS:
			{
				startTag = 	"<STouch>";
				endTag = 	"</STouch>";
				break;
			}
			case MtdScaleHwCfg::XML_PCIE_INS:
			{
				startTag = 	"<Pcie>";
				endTag = 	"</Pcie>";
				break;
			}
			case MtdScaleHwCfg::XML_DOM_INS:
			{
				startTag = 	"<DomIns>";
				endTag = 	"</DomIns>";
				break;
			}
			case MtdScaleHwCfg::XML_RFID_INS:
			{
				startTag = 	"<RfidIns>";
				endTag = 	"</RfidIns>";
				break;
			}
			case MtdScaleHwCfg::XML_POWERHOLD_TIME:
			{
				startTag = 	"<PTime>";
				endTag = 	"</PTime>";
				break;
			}
			case MtdScaleHwCfg::XML_SCAN_TYPE:
			{
				startTag = 	"<ScanType>";
				endTag = 	"</ScanType>";
				break;
			}
			case MtdScaleHwCfg::XML_SCAN_SUPPORT:
			{
				startTag = 	"<SSup>";
				endTag = 	"</SSup>";
				break;
			}
			case MtdScaleHwCfg::XML_SCAN_INS:
			{
				startTag = 	"<ScanIns>";
				endTag = 	"</ScanIns>";
				break;
			}
			case MtdScaleHwCfg::XML_HCAPWEIGHER_INS:
			{
				startTag = 	"<HCWIns>";
				endTag = 	"</HCWIns>";
				break;
			}
			case MtdScaleHwCfg::XML_SYSTEMBOARDVERSION:
			{
				startTag = 	"<SYSBDVERSION>";
				endTag = 	"</SYSBDVERSION>";
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
			// parse the xml string
			std::size_t spos( s.find(startTag) );
			std::size_t epos( s.find(endTag) );

			if (( std::string::npos != spos ) && ( std::string::npos != epos ))
			{
				spos += startTag.size();

				if ( spos < epos )
				{
					const std::string t( s.substr(spos, epos-spos));

					if( startTag == "<PriDspType>" )
						primaryDisplayType_	 = t;
					else if( startTag == "<PriDspSize>" )
						primaryDisplaySize_ = t;
					else if(startTag == "<PriInter>")
						primaryInterface_ = (MtdScaleHwCfg::HW_DISP_INTERFACE)strtol(t.data(), NULL, 10);
					else if(startTag == "<SecDspType>")
						secondaryDisplayType_ = t;
					else if(startTag == "<SecDspSize>")
						secondaryDisplaySize_ = t;
					else if(startTag == "<SecInter>")
						secondaryInterface_ = (MtdScaleHwCfg::HW_DISP_INTERFACE)strtol(t.data(), NULL, 10);
					else if(startTag == "<WifiIns>")
						wifiInstalled_ = (bool)strtol(t.data(), NULL, 10);
					else if(startTag == "<SataIns>")
						sataDriveInstalled_ = (bool)strtol(t.data(), NULL, 10);
					else if(startTag == "<BlueIns>")
						bluetoothInstalled_ = (bool)strtol(t.data(), NULL, 10);
					else if(startTag == "<PowerH>")
						powerHoldupInstalled_ = (bool)strtol(t.data(), NULL, 10);
					else if(startTag == "<STouch>")
						secondaryTouchInstalled_ = (bool)strtol(t.data(), NULL, 10);
					else if(startTag == "<Pcie>")
						pcieModuleInstalled_ = (bool)strtol(t.data(), NULL, 10);
					else if(startTag == "<DomIns>")
						diskOnModuleInstalled_ = (bool)strtol(t.data(), NULL, 10);
					else if(startTag == "<RfidIns>")
						rfidInstalled_ = (bool)strtol(t.data(), NULL, 10);
					else if(startTag == "<PTime>")
						powerHoldupTime_= strtol(t.data(), NULL, 10);
					else if(startTag == "<ScanIns>")
						scannerInstalled_ = (bool)strtol(t.data(), NULL, 10);
					else if(startTag == "<ScanType>")
						scannerType_ = (MtdScaleHwCfg::HW_SCANNER_TYPES)strtol(t.data(), NULL, 10);
					else if(startTag == "<SSup>")
						scannerImageSupport_ = (MtdScaleHwCfg::SCAN_IMAGE_TYPE)strtol(t.data(), NULL, 10);
					else if(startTag == "<HCWIns>")
						highCapWeigherInstalled_ = (bool)strtol(t.data(), NULL, 10);
					else if(startTag == "<SYSBDVERSION>")
						motherBoardHWVersion_=  t;
				}
			}
			else
			{
				Debug::trace("MtdScaleHwCfg", "Starting tag '" + startTag + "' could not be found! " + s);
			}
		}
		else
		{
			Debug::trace("MtdScaleHwCfg", "Unrecognized case!");
		}
	}
}

MtdScaleHwCfg::MtdScaleHwCfg(MtdScaleHwCfg *pMtdScaleHwCfg)
{
	primaryDisplayType_ 		= pMtdScaleHwCfg->getPrimaryDisplayType();
	primaryDisplaySize_ 		= pMtdScaleHwCfg->getPrimaryDisplaySize();
	primaryInterface_ 			= pMtdScaleHwCfg->getPrimaryDisplayInterface();

	secondaryDisplayType_ 		= pMtdScaleHwCfg->getSecondaryDisplayType();
	secondaryDisplaySize_ 		= pMtdScaleHwCfg->getSecondaryDisplaySize();
	secondaryInterface_ 		= pMtdScaleHwCfg->getSecondaryDisplayInterface();

	sataDriveInstalled_ 		= pMtdScaleHwCfg->isSataDriveInstalled();
	wifiInstalled_ 				= pMtdScaleHwCfg->isWifiInstalled();
	bluetoothInstalled_ 		= pMtdScaleHwCfg->isBlueToothInstalled();
	powerHoldupInstalled_ 		= pMtdScaleHwCfg->isPowerHoldUpInstalled();
	secondaryTouchInstalled_ 	= pMtdScaleHwCfg->isSecondaryTouchInstalled();
	pcieModuleInstalled_ 		= pMtdScaleHwCfg->isPcieModuleInstalled();
	diskOnModuleInstalled_ 		= pMtdScaleHwCfg->isDiskOnModuleInstalled();
	rfidInstalled_ 				= pMtdScaleHwCfg->isRfidInstalled();

	powerHoldupTime_ 			= pMtdScaleHwCfg->getPowerHoldUpTime();

	scannerInstalled_ 			= pMtdScaleHwCfg->isScannerInstalled();
	scannerType_ 				= pMtdScaleHwCfg->getScannerType();
	scannerImageSupport_ 		= pMtdScaleHwCfg->getScannerImageSupport();

	highCapWeigherInstalled_ 	= pMtdScaleHwCfg->isHighCapWeigherInstalled();
	motherBoardHWVersion_		= pMtdScaleHwCfg->getSystemBoardHWVersion();

	crc_ 						= pMtdScaleHwCfg->getCrc();
}

void MtdScaleHwCfg::init()
{
	primaryInterface_ = LVDS_0;
	secondaryInterface_ = LVDS_1;

	sataDriveInstalled_ =wifiInstalled_ = bluetoothInstalled_ = powerHoldupInstalled_ =
		secondaryTouchInstalled_ = pcieModuleInstalled_ = diskOnModuleInstalled_ = rfidInstalled_ = false;

	powerHoldupTime_ = 0;

	scannerInstalled_= false;
	scannerType_ = NO_SCANNER;
	scannerImageSupport_ = NO_IMAGE_SUPPORT;

	highCapWeigherInstalled_ = false;

	crc_ = 0;
}

void  MtdScaleHwCfg::setScaleHardwareDefaults()
{
	/*!setup defaults based on basic scale hardware configuration*/
	primaryDisplayType_ = "LDB-WSVGA";
	primaryDisplaySize_ = "10.2";
	primaryInterface_ = LVDS_0;

	secondaryDisplayType_ = "CLAA-WVGA";
	secondaryDisplaySize_ = "7.0";
	secondaryInterface_ = PRGB;

	sataDriveInstalled_ 		= false;
	wifiInstalled_ 				= false;
	bluetoothInstalled_ 		= false;
	powerHoldupInstalled_ 		= false;
	secondaryTouchInstalled_ 	= false;
	pcieModuleInstalled_ 		= false;
	diskOnModuleInstalled_ 		= false;
	rfidInstalled_ 				= false;

	powerHoldupTime_ = 0;

	scannerInstalled_ = 0;
	scannerType_ = NO_SCANNER;
	scannerImageSupport_ = NO_IMAGE_SUPPORT;

	highCapWeigherInstalled_ 	= false;
	motherBoardHWVersion_ = "";
}


unsigned long MtdScaleHwCfg::calculateCrc()
{
	crc_ = 0;
	serializeXml();
	return crc_;
}

const std::string MtdScaleHwCfg::serializeXml() const
{
	serializedData_.clear();
	crc_ = 0;

	serializedData_ = "<ScaleHWSection>"
		"<PriDspType>" + primaryDisplayType_ + "</PriDspType>"
		"<PriDspSize>" + primaryDisplaySize_ + "</PriDspSize>"
		"<PriInter>" + Integer::toString(primaryInterface_) + "</PriInter>"
		"<SecDspType>" + secondaryDisplayType_ + "</SecDspType>"
		"<SecDspSize>" + secondaryDisplaySize_ +  "</SecDspSize>"
		"<SecInter>" + Integer::toString(secondaryInterface_) + "</SecInter>"
		"<WifiIns>" + Bool::toString(wifiInstalled_) + "</WifiIns>"
		"<SataIns>" + Bool::toString(sataDriveInstalled_) + "</SataIns>"
		"<BlueIns>" +  Bool::toString(bluetoothInstalled_) + "</BlueIns>"
		"<PowerH>" + Bool::toString(powerHoldupInstalled_) + "</PowerH>"
		"<STouch>" + Bool::toString(secondaryTouchInstalled_) + "</STouch>"
		"<Pcie>" +  Bool::toString(pcieModuleInstalled_) + "</Pcie>"
		"<DomIns>" + Bool::toString(diskOnModuleInstalled_) + "</DomIns>"
		"<RfidIns>" + Bool::toString(rfidInstalled_) + "</RfidIns>"
		"<PTime>" + Integer::toString(powerHoldupTime_) + "</PTime>"
		"<ScanIns>" + Bool::toString(scannerInstalled_) + "</ScanIns>"
		"<ScanType>" + Integer::toString(scannerType_) + "</ScanType>"
		"<SSup>" + Integer::toString(scannerImageSupport_) + "</SSup>"
		"<HCWIns>" + Bool::toString(highCapWeigherInstalled_) + "</HCWIns>"
		"<SYSBDVERSION>" + motherBoardHWVersion_ + "</SYSBDVERSION>"
		"</ScaleHWSection>";

	/*calculate our crc value for the serialized data*/
	const unsigned char *pSbuffer = (const unsigned char *)serializedData_.data();
	for( int i = 0; i < (int)serializedData_.size(); i++ )
	{
		crc_ +=	*pSbuffer++;
	}
	Debug::trace("MtdScaleHwCfg", "software HW section Crc value to flash: " + Integer::toString(crc_));
	return serializedData_;
}
