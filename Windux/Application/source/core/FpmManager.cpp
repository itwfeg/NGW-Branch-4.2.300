/**
 * @file FpmManager.cpp
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Aaron Swift
 * @brief Fastrack personality module manager manages the serial flash on the HTi motherboard
 * Created on: Feb 26, 2013
 */

#include "FpmManager.h"
#include "FpmEvent.h"
#include "HfcWriteFpmManData.h"
#include "GenericHfcObject.h"
#include "HobVersion.h"
#include "Debug.h"
#include "Long.h"
#include "Integer.h"
#include "Hex.h"
#include "Bool.h"

#include "Configuration.h"
#include "PythonScripter.h"

#if defined (__linux__)
#include <fcntl.h>
#include <sys/ioctl.h>
#include "util/filesys/filename.h"
#endif

/*my instance*/
std::mutex FpmManager::pInstanceMutex;
FpmManager *FpmManager::pInstance( NULL );

///////////////////////////////////////////////////////////////////////////////////////////////////
FpmManager *FpmManager::getInstance()
{
	if ( !pInstance )
	{
		std::lock_guard<std::mutex> g( pInstanceMutex );

		if ( !pInstance )
			pInstance = new FpmManager();
	}

	return pInstance;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
FpmManager::FpmManager()
{
	/*serial flash partitions are defined in the kernel*/
	fpmMFGSection 			= "/dev/mtd3";
	fpmScaleSWSection 		= "/dev/mtd5";
	fpmScaleHWSection 		= "/dev/mtd4";
	fpmScaleKeysSection 	= "/dev/mtd6";
	fpmWrapperDataSection 	= "/dev/mtd2";		//AWS3 & NGW series
	fpmScaleDataSection		= "/dev/mtd2";		//HTi series

	versionFile 		= "/uBootVersion";
	loaderVersion 		= "";

	pScaleSWCfg_		= NULL;
	pScaleMfgData_ 		= NULL;
	pScaleHwCfg_		= NULL;
	pNGWWrapperData_	= NULL;
	pScaleData_			= NULL; // HT-397
	pBuffer_ 			= NULL;

	init();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
FpmManager::~FpmManager()
{
	Debug::trace("FpmManager","~FpmManager()");

	/*!delete all serial flash section objects*/
	if (pScaleMfgData_)
		delete pScaleMfgData_;

	if (pScaleSWCfg_)
		delete pScaleSWCfg_;

	if (pScaleHwCfg_)
		delete pScaleHwCfg_;

	if(pNGWWrapperData_)
		delete pNGWWrapperData_;

	if(pScaleData_)
		delete pScaleData_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool FpmManager::handleWriteMessage(SessionMessage *pMsg)
{
	if ( !pMsg )
	{
		Debug::trace("FpmManager.msg", "Received NULL message....");
		return false;
	}

	bool result( false );
	int wrCount;

	switch ( pMsg->getEventType() )
	{
	case WriteMfgData:
	{
		HfcWriteFpmManData *pManData = dynamic_cast<HfcWriteFpmManData *>(pMsg->getHfcObject());

		Debug::trace("FpmManager.write", "Writing Manufacturing section of serial flash");
		Debug::trace("FpmManager.write", "Scale Style: " + pManData->getScaleStyle());
		Debug::trace("FpmManager.write", "Board Assembly: " + pManData->getBoardAssembly());
		Debug::trace("FpmManager.write", "Board Rev: " + pManData->getBoardRevision());
		Debug::trace("FpmManager.write", "Date Of Man: " + pManData->getDate());
		Debug::trace("FpmManager.write", "Scale Serial Number: " + pManData->getSerial());

		pScaleMfgData_->setScaleStyle(pManData->getScaleStyle());
		pScaleMfgData_->setBoardAssembly(pManData->getBoardAssembly());
		pScaleMfgData_->setBoardRevision(pManData->getBoardRevision());
		pScaleMfgData_->setDateOfManufacturing(pManData->getDate());
		pScaleMfgData_->setSerialNumber(pManData->getSerial());

		wrCount = writeMfgScaleData();
		if (wrCount != 0)
			result = true;
		break;
	}

	case WriteScaleHwConfig:
		break;

	case WriteScaleSWConfig:
		break;

	case WriteOfScaleSWConfigMetrologicalVersion:
	{
		GenericHfcObject *gho = dynamic_cast<GenericHfcObject *>(pMsg->getHfcObject());

		if ( !pScaleSWCfg_ )
			Debug::trace("FpmManager.write", "WriteOfScaleSWConfigPoundsFor - pScaleSWCfg_ is NULL!");
		else if ( gho )
		{
			Debug::trace("FpmManager.write", std::string("WriteOfScaleSWConfigMetrologicalVersion - setting metroVer: ") + gho->getData());
			pScaleSWCfg_->setMetrologicalVersion(gho->getData());
			Debug::trace("FpmManager.write", std::string("WriteOfScaleSWConfigMetrologicalVersion - set metroVer: ") + Integer::toString(pScaleSWCfg_->getMetrologicalVersion()));
			wrCount = writeScaleSWData();
			Debug::trace("FpmManager.write", std::string("WriteOfScaleSWConfigMetrologicalVersion - wrote metroVer - count: ") + Integer::toString(wrCount));
			if (wrCount != 0)
				result = true;
		}

		break;
	}

	case WriteOfScaleSWConfigPoundsFor:
	{
		GenericHfcObject *gho = dynamic_cast<GenericHfcObject *>(pMsg->getHfcObject());

		if ( !pScaleSWCfg_ )
			Debug::trace("FpmManager.write", "WriteOfScaleSWConfigPoundsFor - pScaleSWCfg_ is NULL!");
		else if ( gho )
		{
			Debug::trace("FpmManager.write", std::string("WriteOfScaleSWConfigPoundsFor - setting PoundForEnabled: ") + gho->getData());
			pScaleSWCfg_->setPoundForEnabled(gho->getData());
			Debug::trace("FpmManager.write", std::string("WriteOfScaleSWConfigPoundsFor - set PoundForEnabled: ") + Bool::toString(pScaleSWCfg_->getPoundsForEnabled()));
			wrCount = writeScaleSWData();
			Debug::trace("FpmManager.write", std::string("WriteOfScaleSWConfigPoundsFor - wrote PoundForEnabled - count: ") + Integer::toString(wrCount));
			if (wrCount != 0)
				result = true;
		}

		break;
	}

	case WriteOfScaleSWConfigFractionalPrice:
	{
		GenericHfcObject *gho = dynamic_cast<GenericHfcObject *>(pMsg->getHfcObject());

		if ( !pScaleSWCfg_ )
			Debug::trace("FpmManager.write", "WriteOfScaleSWConfigFractionalPrice - pScaleSWCfg_ is NULL!");
		else if ( gho )
		{
			Debug::trace("FpmManager.write", std::string("WriteOfScaleSWConfigFractionalPrice - setting FractionalPrice: ") + gho->getData());
			pScaleSWCfg_->setFractionalPricingEnabled(gho->getData());
			Debug::trace("FpmManager.write", std::string("WriteOfScaleSWConfigFractionalPrice - set FractionalPrice: ") + Bool::toString(pScaleSWCfg_->getFractionalPricingEnabled()));
			wrCount = writeScaleSWData();
			Debug::trace("FpmManager.write", std::string("WriteOfScaleSWConfigFractionalPrice - wrote FractionalPrice - count: ") + Integer::toString(wrCount));
			if (wrCount != 0)
				result = true;
		}

		break;
	}

	case WriteOfPrimaryMonetarySymbol:
	{

		GenericHfcObject *gho = dynamic_cast<GenericHfcObject *>(pMsg->getHfcObject());
		
		HobString my_string(gho->getData());
		char my_char = (char)my_string[0];

		if ( !pScaleSWCfg_ )
			Debug::trace("FpmManager.write", "WriteOfPrimaryMonetarySymbol - pScaleSWCfg_ is NULL!");
		else if ( gho )
		{
			Debug::trace("FpmManager.write", std::string("WriteOfPrimaryMonetarySymbol - setting WriteOfPrimaryMonetarySymbol: ") + my_string);
			pScaleSWCfg_->setPrimaryCurrencySymbol(gho->getData());
			Debug::trace("FpmManager.write", std::string("WriteOfPrimaryMonetarySymbol - set WriteOfPrimaryMonetarySymbol: ") );
			wrCount = writeScaleSWData();
			Debug::trace("FpmManager.write", std::string("WriteOfPrimaryMonetarySymbol - wrote WriteOfPrimaryMonetarySymbol - count: ") + Long::toString(wrCount));
			
			if (wrCount != 0)
				result = true;
		}

		break;
	}

	case WriteOfUnitSymbolSeparator:
	{

		GenericHfcObject *gho = dynamic_cast<GenericHfcObject *>(pMsg->getHfcObject());
		
		HobString my_string(gho->getData());
		char my_char = (char)my_string[0];

		if ( !pScaleSWCfg_ )
			Debug::trace("FpmManager.write", "WriteOfUnitSymbolSeparator - pScaleSWCfg_ is NULL!");
		else if ( gho )
		{
			
			//Debug::trace("FpmManager.write", std::string("WriteOfUnitSymbolSeparator - setting WriteOfUnitSymbolSeparator: ") + gho->getData());
			Debug::trace("FpmManager.write", std::string("WriteOfUnitSymbolSeparator - setting WriteOfUnitSymbolSeparator: ") + my_string);
			pScaleSWCfg_->setUnitSeparator(my_char);
			Debug::trace("FpmManager.write", std::string("WriteOfUnitSymbolSeparator - set WriteOfUnitSymbolSeparator: ") );
			wrCount = writeScaleSWData();
			Debug::trace("FpmManager.write", std::string("WriteOfUnitSymbolSeparator - wrote WriteOfUnitSymbolSeparator - count: ") + Integer::toString(wrCount));
			
			if (wrCount != 0)
				result = true;
		}

		break;
	}

	case WriteOfCurrencySymbolPlacement:
	{
		GenericHfcObject *gho = dynamic_cast<GenericHfcObject *>(pMsg->getHfcObject());

		if ( !pScaleSWCfg_ )
			Debug::trace("FpmManager.write", "WriteOfCurrencySymbolPalacement - pScaleSWCfg_ is NULL!");
		else if ( gho )
		{
			Debug::trace("FpmManager.write", std::string("WriteOfCurrencySymbolPalacement - setting WriteOfCurrencySymbolPalacement: ") + gho->getData());
			pScaleSWCfg_->setCurrencySymbolPlacement(gho->getData());
			Debug::trace("FpmManager.write", std::string("WriteOfCurrencySymbolPalacement - set WriteOfCurrencySymbolPalacement: ") + Integer::toString(pScaleSWCfg_->getCurrencySymbolPlacement()));
			wrCount = writeScaleSWData();
			Debug::trace("FpmManager.write", std::string("WriteOfCurrencySymbolPalacement - wrote WriteOfCurrencySymbolPalacement - count: ") + Integer::toString(wrCount));
			if (wrCount != 0)
				result = true;
		}

		break;
	}

	case WriteOfTotalPriceDigits:
	{
		GenericHfcObject *gho = dynamic_cast<GenericHfcObject *>(pMsg->getHfcObject());

		if ( !pScaleSWCfg_ )
			Debug::trace("FpmManager.write", "WriteOfTotalPriceDigits - pScaleSWCfg_ is NULL!");
		else if ( gho )
		{
			Debug::trace("FpmManager.write", std::string("WriteOfTotalPriceDigits - setting WriteOfTotalPriceDigits: ") + gho->getData());
			pScaleSWCfg_->setTotalPriceDigits(gho->getData());
			Debug::trace("FpmManager.write", std::string("WriteOfTotalPriceDigits - set WriteOfTotalPriceDigits: ") + Integer::toString(pScaleSWCfg_->getTotalPriceDigits()));
			wrCount = writeScaleSWData();
			Debug::trace("FpmManager.write", std::string("WriteOfTotalPriceDigits - wrote WriteOfTotalPriceDigits - count: ") + Integer::toString(wrCount));
			if (wrCount != 0)
				result = true;
		}

		break;
	}

	case WriteOfUnitPriceDigits:
	{
		GenericHfcObject *gho = dynamic_cast<GenericHfcObject *>(pMsg->getHfcObject());

		if ( !pScaleSWCfg_ )
			Debug::trace("FpmManager.write", "WriteOfUnitPriceDigits - pScaleSWCfg_ is NULL!");
		else if ( gho )
		{
			Debug::trace("FpmManager.write", std::string("WriteOfUnitPriceDigits - setting WriteOfUnitPriceDigits: ") + gho->getData());
			pScaleSWCfg_->setUnitPriceDigits(gho->getData());
			Debug::trace("FpmManager.write", std::string("WriteOfUnitPriceDigits - set WriteOfUnitPriceDigits: ") + Integer::toString(pScaleSWCfg_->getUnitPriceDigits()));
			wrCount = writeScaleSWData();
			Debug::trace("FpmManager.write", std::string("WriteOfUnitPriceDigits - wrote WriteOfUnitPriceDigits - count: ") + Integer::toString(wrCount));
			if (wrCount != 0)
				result = true;
		}

		break;
	}

	case WriteOfCurrencyRoundingMethod:
	{
		GenericHfcObject *gho = dynamic_cast<GenericHfcObject *>(pMsg->getHfcObject());

		if ( !pScaleSWCfg_ )
			Debug::trace("FpmManager.write", "WriteOfCurrencyRoundingMethod - pScaleSWCfg_ is NULL!");
		else if ( gho )
		{
			Debug::trace("FpmManager.write", std::string("WriteOfCurrencyRoundingMethod - setting WriteOfCurrencyRoundingMethod: ") + gho->getData());
			pScaleSWCfg_->setCurrencyRoundingMethod(gho->getData());
			Debug::trace("FpmManager.write", std::string("WriteOfCurrencyRoundingMethod - set WriteOfCurrencyRoundingMethod: ") + Integer::toString(pScaleSWCfg_->getCurrencyRoundingMethod()));
			wrCount = writeScaleSWData();
			Debug::trace("FpmManager.write", std::string("WriteOfCurrencyRoundingMethod - wrote WriteOfCurrencyRoundingMethod - count: ") + Integer::toString(wrCount));
			if (wrCount != 0)
				result = true;
		}

		break;
	}

	case WriteOfPrimaryPrecisionDigits:
	{
		GenericHfcObject *gho = dynamic_cast<GenericHfcObject *>(pMsg->getHfcObject());

		if ( !pScaleSWCfg_ )
			Debug::trace("FpmManager.write", "WriteOfPrimaryPrecisionDigits - pScaleSWCfg_ is NULL!");
		else if ( gho )
		{
			Debug::trace("FpmManager.write", std::string("WriteOfPrimaryPrecisionDigits - setting WriteOfPrimaryPrecisionDigits: ") + gho->getData());
			pScaleSWCfg_->setPrimaryPrecisionDigits(gho->getData());
			Debug::trace("FpmManager.write", std::string("WriteOfPrimaryPrecisionDigits - set WriteOfPrimaryPrecisionDigits: ") + Integer::toString(pScaleSWCfg_->getPrimaryPrecisionDigits()));
			wrCount = writeScaleSWData();
			Debug::trace("FpmManager.write", std::string("WriteOfPrimaryPrecisionDigits - wrote WriteOfPrimaryPrecisionDigits - count: ") + Integer::toString(wrCount));
			if (wrCount != 0)
				result = true;
		}

		break;
	}

	case ClearPrinterLabelCount:
	{
		GenericHfcObject *gho = dynamic_cast<GenericHfcObject *>(pMsg->getHfcObject());
		Debug::trace( "FpmManager.write", "ClearPrinterLabelCount: " + gho->getData() );

		if (( pNGWWrapperData_ ) && ( gho ))
		{
			if( (PrinterType)Integer::valueOf( gho->getData() )  ==  FirstPrinter )
				pNGWWrapperData_->setPrimaryPrinterCounter( 0 );
			else if( (PrinterType)Integer::valueOf( gho->getData() )  ==  SecondPrinter )
				pNGWWrapperData_->setSecondaryPrinterCounter( 0 );
			else if( (PrinterType)Integer::valueOf( gho->getData() )  ==  ThirdPrinter )
				pNGWWrapperData_->setTertiaryPrinterCounter( 0 );
			wrCount = writeNGWWrapperData();
			Debug::trace("FpmManager.write", std::string("ClearPrinterLabelCount - wrote Wrapper data: ") + Integer::toString(wrCount));

			if (wrCount != 0)
				result = true;
		}
		else if (( pScaleData_ ) && ( gho ))
		{
			if( (PrinterType)Integer::valueOf( gho->getData() )  ==  FirstPrinter )
				pScaleData_->setPrimaryPrinterCounter( 0 );
			else if( (PrinterType)Integer::valueOf( gho->getData() )  ==  SecondPrinter )
				pScaleData_->setSecondaryPrinterCounter( 0 );
			else if( (PrinterType)Integer::valueOf( gho->getData() )  ==  ThirdPrinter )
				pScaleData_->setTertiaryPrinterCounter( 0 );
			wrCount = writeScaleData();
			Debug::trace("FpmManager.write", std::string("ClearPrinterLabelCount - wrote Scale data: ") + Integer::toString(wrCount));

			if (wrCount != 0)
				result = true;
		}
		else
		{
			Debug::trace("FpmManager.write", "ClearPrinterLabelCount - pNGWWrapperData_ is NULL!");
		}

		break;
	}

	case SetInstallDate:
	{
		GenericHfcObject *gho = dynamic_cast<GenericHfcObject *>(pMsg->getHfcObject());

		if (( pNGWWrapperData_ ) && ( gho ))
		{
			pNGWWrapperData_->setInstallDate( gho->getData() );
			Debug::trace( "FpmManager.write", "SetInstallDate: " + gho->getData() );
		}

		break;
	}

	default:
		Debug::trace("FpmManager.msg", "Received unknown event....");

	}

	return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void FpmManager::init()
{
	fpmManagerNotATeamPlayer_ = false;

#if defined (__linux__)

	// get the u-boot version information
	Debug::trace( "FpmManager.init", "version file opened!" );

	std::ifstream input( versionFile.data() );
	std::getline( input, loaderVersion );

	if ( loaderVersion.empty() )
		Debug::trace( "FpmManager.init", "version file missing!" );
	else
		Debug::trace( "FpmManager.init", "u-boot version: " + loaderVersion );

	// open each section of the serial flash and initialize the section object.
	// set each section to it's defaults if blank
	FpmErrorCode status;

	// SOF-2544
	// There is a small window of time where we are erasing/writing to flash where a user can
	// power off the machine and lose the flash data. This has been seen at DLM where wrapper
	// cycle counts are stored every 5 minutes or so. To guard against this we will now backup
	// the flash to disk, erase/write flash, then remove the file from disk. When we boot up
	// and fail to read from flash we can attempt to recover from disk now.
	bool recovery;

	// ---- //

	Debug::trace( "FpmManager.init", "Reading Manufacturing section of serial flash...");
	status = readSerialFlashPartition( ManufacturingData, recovery = false );

	if ( status != FPM_OK )
	{
ManufacturingDataRecovery:
		Debug::trace( "FpmManager.init", "Reading Manufacturing section recovery...");
		status = readSerialFlashPartition( ManufacturingData, recovery = true );
	}

	if ( status != FPM_OK )
	{
		Debug::trace( "FpmManager.init", "Manufacturing section blank, loading defaults " + Integer::toString(status) );
		status = loadMfgSectionDefaults();

		if ( status == FPM_OK )
		{
			Debug::trace( "FpmManager.init", "default Manufacturing written to serial flash");
			status = readSerialFlashPartition( ManufacturingData );
		}
		else
		{
			Debug::trace( "FpmManager.init", "writing default Manufacturing data failed !!!");
		}
	}
		
	if (( status == FPM_OK ) && ( pBuffer_ ))
	{
		const std::string mtdData( (char*) pBuffer_, fpmInfo_.size );

		MtdSectionStatistics sectionStatistics( mtdData );
		mfgSecCounter = sectionStatistics.getSectionWriteCount();

		MtdManufacturingData mtdManufacturingData( mtdData );

		if ( sectionStatistics.isSectionDataValid( mtdManufacturingData.getCrc() ) )
		{
			pScaleMfgData_ = new MtdManufacturingData( mtdData );
			Debug::trace( "FpmManager.init", "Manufacturing section data is valid" );
		}
		else
		{
			Debug::trace( "FpmManager.init", "Manufacturing section data is corrupt" );
			mfgSecCounter = 0;

			if ( !recovery )
				goto ManufacturingDataRecovery;

			pScaleMfgData_ = new MtdManufacturingData( mtdData );
			writeMfgScaleData();
		}
	}
	else
	{
		Debug::trace( "FpmManager.init", "Read of Manufacturing section failed!");
		mfgSecCounter = 0;

		if ( !recovery )
			goto ManufacturingDataRecovery;

		pScaleMfgData_ = new MtdManufacturingData();
		writeMfgScaleData();
	}
	
	if ( pBuffer_ )
	{
		delete [] pBuffer_;
		pBuffer_ = NULL;
	}

	// ---- //

	Debug::trace( "FpmManager.init", "Reading scale SW config section of serial flash...");
	status = readSerialFlashPartition( ScaleSWConfiguration, recovery = false );

	if ( status != FPM_OK )
	{
ScaleSWConfigurationRecovery:
		Debug::trace( "FpmManager.init", "Reading scale SW config section recovery...");
		status = readSerialFlashPartition( ScaleSWConfiguration, recovery = true );
	}

	if ( status != FPM_OK )
	{
		Debug::trace( "FpmManager.init", "ScaleSWConfiguration section blank, loading defaults " + Integer::toString(status) );
		status = loadSWSectionDefaults();

		if ( status == FPM_OK )
		{
			Debug::trace( "FpmManager.init", "default ScaleSWConfiguration written to serial flash");
			status = readSerialFlashPartition( ScaleSWConfiguration );
		}
		else
		{
			Debug::trace( "FpmManager.init", "writing default ScaleSWConfiguration data failed: " + Integer::toString(status) );
		}
	}
	
	if (( status == FPM_OK ) && ( pBuffer_ ) && ( MtdScaleSWConfig::dataOffset() < fpmInfo_.size ))
	{
		// skip over bitmap data
		const std::string mtdData( (char*) &pBuffer_[MtdScaleSWConfig::dataOffset()], MtdScaleSWConfig::dataLen() );

		MtdSectionStatistics sectionStatistics( mtdData );
		swSecCounter = sectionStatistics.getSectionWriteCount();

		MtdScaleSWConfig mtdScaleSWConfig( mtdData );

		if ( sectionStatistics.isSectionDataValid( mtdScaleSWConfig.getCrc() ) )
		{
			pScaleSWCfg_ = new MtdScaleSWConfig( &mtdScaleSWConfig );
			Debug::trace( "FpmManager.init", "scale SW config section data is valid" );
		}
		else
		{
			Debug::trace( "FpmManager.init", "scale SW config section data is corrupt" );
			swSecCounter = 0;

			if ( !recovery )
				goto ScaleSWConfigurationRecovery;

			pScaleSWCfg_ = new MtdScaleSWConfig( &mtdScaleSWConfig );
			writeScaleSWData();
		}
	}
	else
	{
		Debug::trace( "FpmManager.init", "Read of scale SW config section failed!");
		swSecCounter = 0;

		if ( !recovery )
			goto ScaleSWConfigurationRecovery;

		pScaleSWCfg_ = new MtdScaleSWConfig();
		writeScaleSWData();
	}
	
	if ( pBuffer_ )
	{
		delete [] pBuffer_;
		pBuffer_ = NULL;
	}

	// ---- //

	Debug::trace( "FpmManager.init", "Reading scale HW config section of serial flash...");
	status = readSerialFlashPartition( ScaleHWConfiguration, recovery = false );

	if ( status != FPM_OK )
	{
ScaleHWConfigurationRecovery:
		Debug::trace( "FpmManager.init", "Reading scale HW config section recovery...");
		status = readSerialFlashPartition( ScaleHWConfiguration, recovery = true );
	}

	if ( status != FPM_OK )
	{
		Debug::trace( "FpmManager.init", "ScaleHWConfiguration section blank, loading defaults " + Integer::toString(status) );
		status = loadHWSectionDefaults();

		if ( status == FPM_OK )
		{
			Debug::trace( "FpmManager.init", "default ScaleHWConfiguration written to serial flash");
			status = readSerialFlashPartition( ScaleHWConfiguration );
		}
		else
		{
			Debug::trace( "FpmManager.init", "writing default ScaleHWConfiguration data failed !!!");
		}
	}
	
	if (( status == FPM_OK ) && ( pBuffer_ ))
	{
		Debug::trace( "FpmManager.init", "Read scale HW config section");
		const std::string mtdData( (char*) pBuffer_, fpmInfo_.size );

		MtdSectionStatistics sectionStatistics( mtdData );
		hwSecCounter = sectionStatistics.getSectionWriteCount();

		MtdScaleHwCfg mtdScaleHwCfg( mtdData );

		if ( sectionStatistics.isSectionDataValid( mtdScaleHwCfg.getCrc() ) )
		{
			pScaleHwCfg_ = new MtdScaleHwCfg( &mtdScaleHwCfg );
			Debug::trace( "FpmManager.init", "scale HW config section section data is valid");
		}
		else
		{
			Debug::trace( "FpmManager.init", "scale HW config section section data is corrupt" );
			hwSecCounter = 0;

			if ( !recovery )
				goto ScaleHWConfigurationRecovery;

			pScaleHwCfg_ = new MtdScaleHwCfg( &mtdScaleHwCfg );
			writeScaleHWData();
		}
	}
	else
	{
		Debug::trace( "FpmManager.init", "Read of ScaleHWConfiguration section failed: " + Integer::toString(status) );
		hwSecCounter = 0;

		if ( !recovery )
			goto ScaleHWConfigurationRecovery;

		pScaleHwCfg_ = new MtdScaleHwCfg();
		writeScaleHWData();
	}
	
	if ( pBuffer_ )
	{
		delete [] pBuffer_;
		pBuffer_ = NULL;
	}
		
	const std::string model( pScaleMfgData_->getScaleStyle() );

	// HT
	// EPCP
	// NGW
	const bool isScale(( model.length() ) && ( 'H' == model[0] ));
	const bool isWrapper(( model.length() ) && (( 'E' == model[0] ) || ( 'N' == model[0] )));

	// ---- //

	if ( isScale )
	{
		Debug::trace( "FpmManager.init", "Reading scale data section of serial flash...");
		status = readSerialFlashPartition( ScaleData, recovery = false );

		if ( status != FPM_OK )
		{
ScaleDataRecovery:
			Debug::trace( "FpmManager.init", "Reading scale data section recovery...");
			status = readSerialFlashPartition( ScaleData, recovery = true );
		}

		if ( status != FPM_OK )
		{
			Debug::trace( "FpmManager.init", "ScaleData section blank, loading defaults " + Integer::toString(status) );
			status = loadScaleDataDefaults();

			if ( status == FPM_OK )
			{
				Debug::trace( "FpmManager.init", "default ScaleData written to serial flash");
				status = readSerialFlashPartition( ScaleData );
			}
			else
			{
				Debug::trace( "FpmManager.init", "writing default ScaleData failed: " + Integer::toString(status) );
			}
		}

		if (( status == FPM_OK ) && ( pBuffer_ ) && ( MtdScaleData::dataOffset() < fpmInfo_.size ))
		{
			Debug::trace( "FpmManager.init", "Read scale data section");

			// skip over bitmap data
			const std::string mtdData( (char*) &pBuffer_[MtdScaleData::dataOffset()], MtdScaleData::dataLen() );

			MtdSectionStatistics sectionStatistics( mtdData );
			scDataCounter = sectionStatistics.getSectionWriteCount();

			MtdScaleData mtdScaleData( mtdData );

			if ( sectionStatistics.isSectionDataValid( mtdScaleData.getCrc() ) )
			{
				pScaleData_ = new MtdScaleData( &mtdScaleData );
				Debug::trace( "FpmManager.init", "scale data section is valid");
			}
			else
			{
				Debug::trace( "FpmManager.init", "scale data section is corrupt" );
				scDataCounter = 0;

				if ( !recovery )
					goto ScaleDataRecovery;

				pScaleData_ = new MtdScaleData( &mtdScaleData );
				writeScaleData();
			}
		}
		else
		{
			Debug::trace( "FpmManager.init", "Read of MtdScaleData section failed: " + Integer::toString(status) );
			scDataCounter = 0;

			if ( !recovery )
				goto ScaleDataRecovery;

			pScaleData_ = new MtdScaleData();
			writeScaleData();
		}
	}

	// ---- //

	else if ( isWrapper )
	{
		Debug::trace( "FpmManager.init", "Reading scale Wrapper data section of serial flash...");
		status = readSerialFlashPartition( WrapperData, recovery = false );

		if ( status != FPM_OK )
		{
WrapperDataRecovery:
			Debug::trace( "FpmManager.init", "Reading scale Wrapper data section recovery...");
			status = readSerialFlashPartition( WrapperData, recovery = true );
		}

		if ( status != FPM_OK )
		{
			Debug::trace( "FpmManager.init", "Wrapper Data section blank, loading defaults " + Integer::toString(status) );
			status = loadNGWWrapperSectionDefaults();

			if ( status == FPM_OK )
			{
				Debug::trace( "FpmManager.init", "default Wrapper Data  written to serial flash");
				status = readSerialFlashPartition( WrapperData );
			}
			else
			{
				Debug::trace( "FpmManager.init", "writing default Wrapper data failed: " + Integer::toString(status) );
			}
		}

		if (( status == FPM_OK ) && ( pBuffer_ ) && ( MtdNGWWrapperData::dataOffset() < fpmInfo_.size ))
		{
			// skip over bitmap data
			const std::string mtdData( (char*) &pBuffer_[MtdNGWWrapperData::dataOffset()], MtdNGWWrapperData::dataLen() );

			MtdSectionStatistics sectionStatistics( mtdData );
			ngwWrapperCounter = sectionStatistics.getSectionWriteCount();

			MtdNGWWrapperData mtdNGWWrapperData( mtdData );

			if ( sectionStatistics.isSectionDataValid( mtdNGWWrapperData.getCrc() ) )
			{
				pNGWWrapperData_ = new MtdNGWWrapperData( &mtdNGWWrapperData );
				Debug::trace( "FpmManager.init", "scale Wrapper data section is valid");
			}
			else
			{
				Debug::trace( "FpmManager.init", "scale Wrapper data section data is corrupt" );
				ngwWrapperCounter = 0;

				if ( !recovery )
					goto WrapperDataRecovery;

				pNGWWrapperData_ = new MtdNGWWrapperData( &mtdNGWWrapperData );
				writeNGWWrapperData();
			}
		}
		else
		{
			Debug::trace( "FpmManager.init", "Read of Wrapper data section failed!");
			ngwWrapperCounter = 0;

			if ( !recovery )
				goto WrapperDataRecovery;

			pNGWWrapperData_ = new MtdNGWWrapperData();
			writeNGWWrapperData();
		}
	}

	if ( pBuffer_ )
	{
		delete [] pBuffer_;
		pBuffer_ = NULL;
	}

#else

	// loader
	if (Configuration::getInstance()->toBoolCfgI(CfgInternal::PYTHONENABLED)) {
		const std::string command(
				PythonScripter::buildPythonCall("getBiosVersion.py"));

		std::string biosInfoOutput;

		Debug::trace("FpmManager.init", "fetching bios version...");
		PAL_absclass::PAL_executeWithResults(command, biosInfoOutput);

		if (!biosInfoOutput.empty())
			loaderVersion = biosInfoOutput;

		Debug::trace("FpmManager.init", "bios version: " + loaderVersion);
	}

	// manf data
	if (Configuration::getInstance()->toBoolCfgI(CfgInternal::PYTHONENABLED)) {
		std::string command(
				PythonScripter::buildPythonCall("getSerialFlashInfo.py"));
		command.append(" --raw");

		std::string scriptOutput;

		Debug::trace("FpmManager.init", "fetching serial flash...");
		PAL_absclass::PAL_executeWithResults(command, scriptOutput);

		if (scriptOutput.empty()) {
			Debug::trace("FpmManager.init",
					"Manufacturing section blank, loading defaults");
			loadMfgSectionDefaults();
		} else {
			Debug::trace("FpmManager.init",
					"Manufacturing section section data is valid: "
							+ scriptOutput);
			std::string stats(scriptOutput);
			std::string mfgData(scriptOutput);

			MtdSectionStatistics sectionStatistics(stats);
			mfgSecCounter = sectionStatistics.getSectionWriteCount();

			pScaleMfgData_ = new MtdManufacturingData(mfgData);

			Debug::trace("FpmManager.init",
					pScaleMfgData_->getScaleStyle() + " ["
							+ pScaleMfgData_->getSerialNumber() + "]");
		}
	}

	// sw version
	if (Configuration::getInstance()->toBoolCfgI(CfgInternal::PYTHONENABLED)) {
		std::string command( PythonScripter::buildPythonCall("getSerialFlashInfo.py") );
		command.append(" --sw --raw");

		std::string scriptOutput;

		Debug::trace("FpmManager.init", "fetching serial flash software config...");
		PAL_absclass::PAL_executeWithResults(command, scriptOutput);

		if (scriptOutput.empty()) {
			Debug::trace("FpmManager.init",
					"Software section blank, loading defaults");
			loadSWSectionDefaults();
		} else {
			Debug::trace("FpmManager.init", "Software section section data is valid: " + scriptOutput);
			std::string stats(scriptOutput);
			std::string softwareData(scriptOutput);

			MtdSectionStatistics sectionStatistics(stats);
			swSecCounter = sectionStatistics.getSectionWriteCount();

			pScaleSWCfg_ = new MtdScaleSWConfig(softwareData);

			Debug::trace("FpmManager.init", string("metroVer: ") + Long::toString(pScaleSWCfg_->getMetrologicalVersion()) );
		}
	}

	// hw version
	// FLASH TODO

#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
const std::string FpmManager::getPartition(FPMSectorTypes sector) {
	std::string s;

	if (sector == ManufacturingData) {
		s = fpmMFGSection;
	} else if (sector == ScaleSWConfiguration) {
		s = fpmScaleSWSection;
	} else if (sector == ScaleHWConfiguration) {
		s = fpmScaleHWSection;
	} else if (sector == SoftwareKeys) {
		s = fpmScaleKeysSection;
	} else if (sector == WrapperData) {
		s = fpmWrapperDataSection;
	} else if (sector == ScaleData) {
		s = fpmScaleDataSection;
	}

	return s;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
const std::string FpmManager::getMFGSystemStyle()
{
	/* =========== This is the Service Scale configuration matrix ===========
	 *		HTXX-XXXX
	 *		  ^^ ^^^^
	 *		  || ||||__ Network: T = Wired Ethernet only, No value = both Wired and Wireless
	 *		  || |||__ Media Size: S = standard, H = high
	 *		  || ||__ OS: L = Linux, W = Windows
	 *		  || |__ Customer Display Size: 7 = 7", 10 = 10"
	 *		  ||__ Optional Field: P = Printer only
	 *		  |__ Processor: s = Solo ARM, i = Dual ARM, x = Quad ARM or x86, m = ARK
	 *
	 */

	/* =========== This is the Free Standing Service Scale configuration matrix ===========
	 *		FS-XXXXX
	 *		   ^^^^^
	 *		   |||||
	 *		   |||||__ Customer Specific: TBD
	 *		   ||||__ Customer Specific: TBD
	 *		   |||__ Valuemax: 0 = None, V = Valuemax installed
	 *		   ||__ Scanner: 0 = None, B = Barcode, E = Image Recognition
	 *		   |__ Size: 1 = Tall, 2 = ADA, 3 = Table Top
	 *
	 *
	 *		Ex: FS-100 ==> Self Serve Tall
	 *			FS-10V ==> Self Serve Tall Valuemax
	 *
	 * 
	 */

	/* =========== This is the PrePack configuration matrix ===========
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

	/*
	 * Model differences in CAN / USB messaging
	 * NGW1 = hybrid CAN / USB wrapper messaging	(BCM System Controller)
	 * NGW2 = full CAN wrapper messaging only		(ARA System Controller)
	 * NGW3 = full CAN wrapper messaging only		(ARK System Controller)
	 */

	/*	System style is based off of the scale style.
	 *  AWS3 is only compatible with the following scale styles.
	 *  EPCP-5I1U,EPCP-5I1T,EPCP-5I1I
	 *  EPCP-5S1U,EPCP-5S1T,EPCP-5S1I
	 */
	std::string s;

	if ( pScaleMfgData_ )
	{
		const std::string model( pScaleMfgData_->getScaleStyle() );

		Debug::trace( "FpmManager", "Model# " + model );

		if ( 4 <= model.size() )
		{
			if (( 'H' == model[0] ) && ( 'T' == model[1] ) &&
				(( 'S' == model[2] ) || ( 's' == model[2] ) ||		// HTs
				 ( 'I' == model[2] ) || ( 'i' == model[2] ) ||		// HTi
				 ( 'X' == model[2] ) || ( 'x' == model[2] )) &&		// HTx
				(( '-' == model[3] ) || ( 'P' == model[3] )))
			{
				s = "HT";

				// SOF-3014
				// detect ARK board, when ARK board we are CAN based peripheral
				if ( Configuration::getInstance()->toBoolCfgI(CfgInternal::PYTHONENABLED) )
				{
					const string command(
						PythonScripter::buildPythonCall(
							Configuration::getInstance()->toStringCfgI(CfgInternal::PYTHONGETBOARDTYPESCRIPTNAME) ) );

					string result;

					// get board type
					PAL_absclass::PAL_executeWithResults( command, result );

					// result format:
					//   X,Y,Z
					// where:
					//   X = 0 or 1 if ARA board
					//   Y = 0 or 1 if BCM board
					//   Z = 0 or 1 if ARK board
					if ( 5 <= result.length() )
						if ( '1' == result[4] )
							s = "HTK";
				}
			}
			else if ( ( 6 <= model.size() ) &&  
					( ( 'F' == model[0] ) && ( 'S' == model[1] ) ) || ( ( 'f' == model[0] ) && ( 's' == model[1] ) ) &&
					  ('0' == model[5]) ) 
			{
				s = "FSS";
			}
			else if( ( 6 <= model.size() ) && 
					 (  ( 'F' == model[0] ) && ( 'S' == model[1] ) ) || ( ( 'f' == model[0] ) && ( 's' == model[1] ) ) &&
						( ('V' == model[5]) || ('v' == model[5]) ) )
			{
				s = "FSV";	
			}
			else if (( 'H' == model[0] ) && ( 'T' == model[1] ) &&
				(( 'M' == model[2] ) || ( 'm' == model[2] )) &&		// HTm - ARK boards
				(( '-' == model[3] ) || ( 'P' == model[3] )))
			{
				s = "HTK";
			}
			else if ( 'E' == model[0] ) // EPCP
			{
				if (( 8 <= model.size() ) &&
					( 'S' == model[6] || 'I' == model[6] || 'Q' == model[6] || 'X' == model[6] ) &&
					( '1' == model[7] || '2' == model[7] || '3' == model[7] ))
				{
					s = "AWS3";
				}
				else
				{
					s = "EPCP";
				}
			}
			else if (( 8 <= model.size()) && ( 'N' == model[0] ) && ( 'G' == model[1] ) && ( 'W' ==  model[2] ) &&  ( '3' ==  model[7] ) )
			{
				s = "NGW3";
			}
			else if(( 8 <= model.size()) && ( 'N' == model[0] ) && ( 'G' == model[1] ) && ( 'W' ==  model[2] ) &&  ( '1' ==  model[7] ) )
			{
				s = "NGW1";
			}
			else if (( 8 <= model.size()) && ( 'N' == model[0] ) && ( 'G' == model[1] ) && ( 'W' ==  model[2] ) &&  ( '2' ==  model[7] ) )
			{
				s = "NGW2";
			}
		}
#if defined (__linux__)
		//check to see if we are running boardsite?
		util_filesystem::File  boardSiteFile(NAME_OF_BOARD_SITE_FILE);
		//if we are running boardsite then allow the KPC to come up so we can test it.
		if ( boardSiteFile.exists() )  {
			s = "HTK";
		}
#endif
	}

	return s;
}

const std::string FpmManager::getFullMFGSystemStyle()
{
	if (pScaleMfgData_)
		return pScaleMfgData_->getScaleStyle();

	return std::string();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void FpmManager::setMFGSystemStyle(const std::string s) {

	if(!s.empty()) {
		if( pScaleMfgData_ != NULL ){
			pScaleMfgData_->setScaleStyle(s);
		}
		writeMfgScaleData();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int FpmManager::writeMfgScaleData() {
#if defined (__linux__)

	int bytesFlashed = 0;

	/*! setup the section statistics for this section*/
	MtdSectionStatistics mfgStatistics;
	mfgStatistics.setSectionWriteCount(++mfgSecCounter);
	mfgStatistics.setSectorType(ManufacturingData);

	/*!serialize the section data*/
	std::string xmlData = pScaleMfgData_->serializeXml();

	/*! section crc value is based on section data*/
	mfgStatistics.setSectionCrc(pScaleMfgData_->getCrc());

	/*statistics first then section data */
	std::string mfgData = mfgStatistics.serialize();
	mfgData += xmlData;

	FpmErrorCode status = writeSerialFlashPartition(ManufacturingData, mfgData);
	if(status == FPM_OK)
	{
		bytesFlashed = mfgData.size();
	}
	return bytesFlashed;

#else

	// manf data
	if (!Configuration::getInstance()->toBoolCfgI(CfgInternal::PYTHONENABLED))
		Debug::trace("FpmManager.write", "python not available!");
	else if (!pScaleMfgData_)
		Debug::trace("FpmManager.write", "invalid mfg data!");
	else {
		std::string command(
				PythonScripter::buildPythonCall("setSerialFlashInfo.py"));
		command.append(" -scaleStyle " + pScaleMfgData_->getScaleStyle());
		command.append(" -boardAssembly " + pScaleMfgData_->getBoardAssembly());
		command.append(" -boardRev " + pScaleMfgData_->getBoardRevision());
		command.append(
				" -dateOfManufacture "
						+ pScaleMfgData_->getDateOfManufacturing());
		command.append(" -serialNumber " + pScaleMfgData_->getSerialNumber());

		std::string scriptOutput;

		Debug::trace("FpmManager.write", "writing serial flash...");
		PAL_absclass::PAL_executeWithResults(command, scriptOutput);

		return 1;
	}

	return 0;

#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int FpmManager::writeScaleSWData() {
#if defined (__linux__)

	int bytesFlashed = 0;
	std::string bitMaps;

	/*! setup the section statistics for this section*/
	MtdSectionStatistics swStatistics;
	swStatistics.setSectionWriteCount(++swSecCounter);
	swStatistics.setSectorType(ScaleSWConfiguration);

	/*! read and save the bitmaps */
	FpmErrorCode status = readSerialFlashPartition( ScaleSWConfiguration );
	if(( status == FPM_OK ) || ( status = SECTION_BLANK ))
	{
		if (( pBuffer_ != NULL ) && ( MtdScaleSWConfig::dataOffset() ))
		{
			bitMaps.assign((const char *)pBuffer_, MtdScaleSWConfig::dataOffset());
		}
	}
	else
	{
		Debug::trace( "FpmManager.write", "Reading the SW section failed");
	}
	/*!serialize the section data*/
	std::string xmlData = pScaleSWCfg_->serializeXml();

	/*! section crc value is based on section data*/
	swStatistics.setSectionCrc(pScaleSWCfg_->getCrc());

	/*statistics first then section data */
	std::string scaleSwCfg = bitMaps + swStatistics.serialize();
	scaleSwCfg += xmlData;

	status = writeSerialFlashPartition(ScaleSWConfiguration,
			scaleSwCfg);
	if (status == FPM_OK) {
		bytesFlashed = scaleSwCfg.size();
	}
	return bytesFlashed;

#else

	// software data
	if (!Configuration::getInstance()->toBoolCfgI(CfgInternal::PYTHONENABLED))
		Debug::trace("FpmManager.write", "python not available!");
	else if (!pScaleSWCfg_)
		Debug::trace("FpmManager.write", "invalid software data!");
	else {
		std::string command( PythonScripter::buildPythonCall("setSerialFlashInfo.py") );
		command.append(" --sw").append(" -metrologyVersion ").append( Long::toString( pScaleSWCfg_->getMetrologicalVersion() ) );
		command.append(" -fractionalPricingEnabled ").append( Bool::toString( pScaleSWCfg_->getFractionalPricingEnabled() ) );
		command.append(" -poundsForPricingEnabled ").append( Bool::toString( pScaleSWCfg_->getPoundsForEnabled() ) );

		std::string scriptOutput;

		Debug::trace("FpmManager.write", "writing serial flash software section ... " + command);
		PAL_absclass::PAL_executeWithResults(command, scriptOutput);

		return 1;
	}

	return 0;

#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int FpmManager::writeScaleHWData() {
	int bytesFlashed = 0;

	/*! setup the section statistics for this section*/
	MtdSectionStatistics hwStatistics;
	hwStatistics.setSectionWriteCount(++hwSecCounter);
	hwStatistics.setSectorType(ScaleHWConfiguration);

	std::string xmlData = pScaleHwCfg_->serializeXml();
	/*! section crc value is based on section data*/
	hwStatistics.setSectionCrc(pScaleHwCfg_->getCrc());

	/*statistics first then section data */
	std::string scaleHwCfg = hwStatistics.serialize();
	scaleHwCfg += xmlData;

	FpmErrorCode status = writeSerialFlashPartition(ScaleHWConfiguration,
			scaleHwCfg);
	if (status == FPM_OK) {
		bytesFlashed = (int) scaleHwCfg.size();
	}
	return bytesFlashed;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int FpmManager::writeNGWWrapperData()
{
	int bytesFlashed = 0;
	std::string bitMaps;

	/*! setup the section statistics for this section*/
	MtdSectionStatistics WrapperStatistics;
	WrapperStatistics.setSectionWriteCount(++ngwWrapperCounter);
	WrapperStatistics.setSectorType(WrapperData);

	FpmErrorCode status = readSerialFlashPartition( WrapperData );
	if(( status == FPM_OK ) || ( SECTION_BLANK == status))
	{
		if(( pBuffer_ != NULL ) && (MtdNGWWrapperData::dataOffset()))
		{
			bitMaps.assign((const char *)pBuffer_, MtdNGWWrapperData::dataOffset());
		}
	}
	else
	{
		Debug::trace( "FpmManager.write", "Reading the Wrapper Data section failed");
	}

	std::string xmlData = pNGWWrapperData_->serializeXml();

	/*! section crc value is based on section data*/
	WrapperStatistics.setSectionCrc(pNGWWrapperData_->getCrc());

	/*bitmap first then statistics and section data */
	std::string wrapperData = bitMaps + WrapperStatistics.serialize();
	wrapperData += xmlData;

	status = writeSerialFlashPartition(WrapperData,
			wrapperData);
	if (status == FPM_OK) {
		bytesFlashed = (int) wrapperData.size();
	}
	return bytesFlashed;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int FpmManager::writeScaleData()
{
	int bytesFlashed = 0;
	std::string bitMaps;

	/*! setup the section statistics for this section*/
	MtdSectionStatistics statistics;
	statistics.setSectionWriteCount(++scDataCounter);
	statistics.setSectorType(ScaleData);

	FpmErrorCode status = readSerialFlashPartition( ScaleData );
	if(( status == FPM_OK ) || (SECTION_BLANK == status ))
	{
		if(( pBuffer_ != NULL ) && ( MtdScaleData::dataOffset() ))
		{
			bitMaps.assign((const char *)pBuffer_, MtdScaleData::dataOffset());
		}
	}
	else
	{
		Debug::trace( "FpmManager.write", "Reading the Scale Data section failed");
	}

	std::string xmlData = pScaleData_->serializeXml();
	/*! section crc value is based on section data*/
	statistics.setSectionCrc(pScaleData_->getCrc());

	/*statistics first then section data */
	std::string scaleData_ = bitMaps + statistics.serialize();
	scaleData_ += xmlData;

	status = writeSerialFlashPartition(ScaleData,
			scaleData_);
	if (status == FPM_OK) {
		bytesFlashed = (int) scaleData_.size();
	}
	return bytesFlashed;

}

int FpmManager::isWrapperLeftToRight()
{
	std::string style = "";
	if (pScaleMfgData_)
	{
		style = pScaleMfgData_->getScaleStyle();
	}
	else
	{
		Debug::trace("FpmManager", "pScaleMfgData is null");
	}

	// Style:
		/* NGWX - X X X X XX XX
		*	 ^   ^ ^ ^ ^ ^  ^
		*	 |   | | | | |  | __ Customer variants
		*    |   | | | | | __ Discharge
		*	 |   | | | | __ Power
		*	 |   | | | __ Generation
		*	 |   | | __ Processor
		*	 |   | __ OS
		*    | __ #Rolls
		*
		* Rolls: 1,2
		* OS: L,W
		* Processor: S,I,Q,X	S = Solo ARM, I = Dual ARM, Q = Quad ARM, X = x86
		* Generation: 1,2,3	1 = BCM CAN1 USB SAM7, 2 = ACR CAN1 USB SAM7, 3 = ACR CAN2 RS485 K64
		* Power: U,T,I		U = 198v - 242v, T = 187v - 197v or 243v - 254v, I = international (Chile)
		* Discharge: LR,RL 	LR = Left to Right, RL = Right to Left
		*/
		// Example: NGW2-LS2LR <- Need to parse the LR 
		// For two printer systems this may have a customer variant and look something like
		// NGW2-LS2LR2P (not official)

	if (style.size() > 0)
	{
		if (style.substr(0, 3) != "NGW")
		{
			Debug::trace("FpmManager", "System style is not NGW");
			return -1;
		}

		Debug::trace("FpmManager", "Wrapper Style: " + style);

		std::string direction = "";
		if ( style.size() == 13 || style.size() == 11 )
		{
			// style includes power, direction in 9th position
			direction = style.substr(9, 2);
			Debug::trace("FpmManager", "Wrapper Direction: " + direction);
		}
		else if ( style.size() == 12 || style.size() == 10 )
		{
			// style includes power, direction in 8th position
			direction = style.substr(8, 2);
			Debug::trace("FpmManager", "Wrapper Direction: " + direction);
		}

		if ( direction == "LR" )
		{
			return 1;
		}
		else if ( direction == "RL" )
		{
			return 0;
		}
		else
		{
			Debug::trace("FpmManager", "Could not decipher discharge");
		}
	}
	else
	{
		Debug::trace("FpmManager", "Did not get MFG System Style");
		return -1;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool FpmManager::isWrapperTwoPrinter() 
{
	bool result = false;

	if( pScaleMfgData_ != NULL )
	{
		std::string style = pScaleMfgData_->getScaleStyle();
		
		if ( style.size() )
		{
			//Are we a Wrapper?
			if ( style.substr(0, 3) == "NGW" )
			{
				if ( style.size() == 12 )
				{
					std::string variant;
					variant = style.substr(10, 2);
					util_string::toupper( variant );
					Debug::trace("FpmManager", "Customer Variant: " + variant );
					if( ( "2P" == variant ) ||
						( "CP" == variant ) ) // SOF-4782: Costco 2nd printer will have suffix of "CP" (e.g. NGW2-LS2RLCP)
					{
						result = true;	
					}
				}
			}
			else
			{
				Debug::trace("FpmManager", "System style is not NGW");
			}
		}
	}
	else
	{
		Debug::trace("FpmManager", "pScaleMfgData_ is NULL!");
	}
	return result;
} 

///////////////////////////////////////////////////////////////////////////////////////////////////
bool FpmManager::isFreeStandingSelfService()
{
	bool result = false;

	if( pScaleMfgData_ != NULL )
	{
		std::string style = pScaleMfgData_->getScaleStyle();
		
		if ( style.size() )
		{
			//Are we a freestanding self service scale?
			if ( ( style.substr(0, 2) == "FS" ) || ( style.substr(0, 2) == "fs" ) )
			{
				result = true;	
			}
		}
	}
	return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool FpmManager::isValueMaxModelFSS()
{
	
	std::string fullStyle = getFullMFGSystemStyle();

	if (fullStyle.size())
	{

		if (fullStyle.find("V") != std::string::npos)
		{
			return isFreeStandingSelfService() && true;
		}
	}

	return false;

}


///////////////////////////////////////////////////////////////////////////////////////////////////
FpmManager::FpmErrorCode FpmManager::readSerialFlashPartition(
	FPMSectorTypes sector,
	bool recovery )
{
#if defined (__linux__)

	// SOF-2544
	// There is a small window of time where we are erasing/writing to flash where a user can
	// power off the machine and lose the flash data. This has been seen at DLM where wrapper
	// cycle counts are stored every 5 minutes or so. To guard against this we will now backup
	// the flash to disk, erase/write flash, then remove the file from disk. When we boot up
	// and fail to read from flash we can attempt to recover from disk now.

	const std::string partitionRecovery( "mtdrecovery" + Integer::toString(sector) );
	const std::string partition( getPartition( sector ) );

	if ( !partition.size() )
	{
		Debug::trace( "FpmManager.read", "Could not get partition location!" );
		return READ_FAILURE;
	}

	int fd( open( recovery ? partitionRecovery.c_str() : partition.c_str(), O_RDONLY ) );

	if ( fd < 0 )
	{
		Debug::trace( "FpmManager.read", "Could not get descriptor!" );
		return READ_FAILURE;
	}

	Debug::trace( "FpmManager.read", "Partition opened!" );
	FpmErrorCode status( FPM_OK );

	ioctl( fd, MEMGETINFO, &fpmInfo_ );

	if ( MTD_NORFLASH == fpmInfo_.type )
	{
		Debug::trace( "FpmManager.read", "Buffer size: " + Integer::toString(fpmInfo_.size) );

		// create a temporary partition buffer for our read
		if ( pBuffer_ )
			delete [] pBuffer_;

		pBuffer_ = new unsigned char[fpmInfo_.size];

		// read section statistics and if section is blank,
		// write the defaults to the section
		read( fd, pBuffer_, fpmInfo_.size );

		// Software section is the only section that contains
		// bitmaps at the start of section
		if ( ScaleSWConfiguration == sector )
		{
			const std::string data( (char*) &pBuffer_[MtdScaleSWConfig::dataOffset()], 128 );

			// skip over the bitmaps
			if ( sectionBlank == data[0] )
			{
				 // if blank prompt user
				Debug::trace( "FpmManager.read", "Software Section is blank!" );
				status = SECTION_BLANK;
			}
			else if ( std::string::npos == data.find("<ScaleCFGSection>") )
			{
				 // if invalid prompt user
				Debug::trace( "FpmManager.read", "Software Section is missing, invalid section!" );
				status = SECTION_BLANK;
			}
			else
			{
				status = FPM_OK; // read the entire partition contents of the serial flash
			}
		}

		else if ( WrapperData == sector )
		{
			const std::string data( (char*) &pBuffer_[MtdNGWWrapperData::dataOffset()], 128 );

			// skip over the bitmaps
			if ( sectionBlank == data[0] )
			{
				 // if wrapper section is blank prompt user
				Debug::trace( "FpmManager.read", "Wrapper Data is blank!" );
				status = SECTION_BLANK;
			}
			else if ( std::string::npos == data.find("<NGWSection>") )
			{
				 // if wrapper section is blank prompt user
				Debug::trace( "FpmManager.read", "Wrapper Data is missing, invalid section!" );
				status = SECTION_BLANK;
			}
			else
			{
				status = FPM_OK; // read the entire partition contents of the serial flash
			}
		}

		else if ( ScaleData == sector )
		{
			const std::string data( (char*) &pBuffer_[MtdScaleData::dataOffset()], 128 );

			// skip over the bitmaps
			if ( sectionBlank == data[0] )
			{
				 // if wrapper section is blank prompt user
				Debug::trace( "FpmManager.read", "Scale Data is blank!" );
				status = SECTION_BLANK;
			}
			else if ( std::string::npos == data.find("<SCDataSection>") )
			{
				 // if wrapper section is blank prompt user
				Debug::trace( "FpmManager.read", "Scale Data is missing, invalid section!" );
				status = SECTION_BLANK;
			}
			else
			{
				status = FPM_OK; // read the entire partition contents of the serial flash
			}
		}

		else if ( sectionBlank != pBuffer_[0] )
		{
			// read the entire partition contents of the serial flash
			status = FPM_OK;
		}
		else
		{
			status = SECTION_BLANK;
		}
	}
	else if ( MTD_ABSENT == fpmInfo_.type )
	{
		Debug::trace( "FpmManager.read", "Personality Module Missing!" );
		status = MODULE_MISSING;
	}
	else
	{
		Debug::trace( "FpmManager.read", "Unsupported device: " + Integer::toString(fpmInfo_.type) );
		status = UNSUPPORTED_DEVICE;
	}

	close( fd );

	return status;

#else

	// FLASH TODO
	return READ_FAILURE;

#endif

}

///////////////////////////////////////////////////////////////////////////////////////////////////
FpmManager::FpmErrorCode FpmManager::eraseSerialFlashPartition(
		FPMSectorTypes sector) {
	FpmErrorCode status = FPM_OK;

#if defined (__linux__)

	std::string partition = getPartition(sector);
	erase_info_t eraseInfo;

	if( partition.size() )
	{
		int fd = open( (const char *)partition.data(), O_RDWR );
		if( fd > 0 )
		{
			ioctl( fd, MEMGETINFO, &fpmInfo_ );
			if( fpmInfo_.type == MTD_NORFLASH )
			{

				int unlock = 0;
				int erase = 0;

				eraseInfo.length = fpmInfo_.erasesize;
				/*erase all subsections within section*/
				for( eraseInfo.start = 0; eraseInfo.start < fpmInfo_.size; eraseInfo.start += fpmInfo_.erasesize )
				{
					unlock = ioctl( fd, MEMUNLOCK, &eraseInfo );
					erase = ioctl( fd, MEMERASE, &eraseInfo );
					if( unlock != -1 )
					{
						if( erase == -1 )
						{
							Debug::trace( "FpmManager.erase", "Failed to erase partition sector: " + Long::toString( eraseInfo.start ) );
						}
					}
					else
					{
						//Debug::trace( "FpmManager.erase", "Failed to unlock partition sector: " + Long::toString( eraseInfo.start ));
						//break;
					}
				}

				if( ( unlock != -1 ) || (erase != -1) )
				status = FPM_OK;
			}
			else
			{
				if( fpmInfo_.type == MTD_ABSENT )
				{
					Debug::trace( "FpmManager.erase", "Personality Module Missing!" );
					status = MODULE_MISSING;
				}
				else
				{
					Debug::trace( "FpmManager.read", "Unsupported device: " + Integer::toString(fpmInfo_.type) );
					status = UNSUPPORTED_DEVICE;
				}
			}
			close(fd);
		}
		else
		{
			Debug::trace( "FpmManager.erase", "Failed to open partition: " + partition + "error code: " + Integer::toString(fd));
			status = OPEN_PARTITION;
		}
	}
	else
	{
		Debug::trace( "FpmManager.erase", "Could no get partition location!" );
		status = UNKNOWN_PARTITION;
	}

#else

	// FLASH TODO
	status = UNKNOWN_PARTITION;

#endif

	return status;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
FpmManager::FpmErrorCode FpmManager::writeSerialFlashPartition(
	FPMSectorTypes sector,
	const std::string &data )
{
	Util::CLockCriticalSection lock( writeSerialFlashLock_ );

#if defined (__linux__)

	int fd;

	// SOF-2544
	// There is a small window of time where we are erasing/writing to flash where a user can
	// power off the machine and lose the flash data. This has been seen at DLM where wrapper
	// cycle counts are stored every 5 minutes or so. To guard against this we will now backup
	// the flash to disk, erase/write flash, then remove the file from disk. When we boot up
	// and fail to read from flash we can attempt to recover from disk now.

	const std::string partitionRecovery( "mtdrecovery" + Integer::toString(sector) );

	// open file
	if ( 0 <= (fd = open( partitionRecovery.c_str(), O_WRONLY | O_CREAT | O_TRUNC )) )
	{
		const size_t wSize( write( fd, data.c_str(), data.size() ) );

		// write file
		if ( data.size() == wSize )
			Debug::trace( "FpmManager.write", "partition recovery written: " + Integer::toString(sector) );
		else
		{
			Debug::trace( "FpmManager.write", "failed to write partition recovery: " + Integer::toString(sector) +
				" error code: " + Integer::toString(errno) );
		}

		// close file
		syncfs( fd );
		close( fd );
	}
	else
	{
		Debug::trace( "FpmManager.write", "failed to open partition recovery: " + Integer::toString(sector) +
			" error code: " + Integer::toString(errno) );
	}

	// ---- //

	const std::string partition( getPartition( sector ) );

	// open device
	if ( (fd = open( partition.c_str(), O_RDWR )) < 0 )
	{
		Debug::trace( "FpmManager.write", "failed to open partition: '" + partition + "' error code: " + Integer::toString(errno) );
		return WRITE_FAILURE;
	}

	FpmErrorCode status( FPM_OK );

	ioctl( fd, MEMGETINFO, &fpmInfo_ );

	if ( data.size() <= fpmInfo_.size )
	{
		// erase the partition before writing
		erase_info_t ei;

		ei.length = fpmInfo_.erasesize; // set the erase block size

		for ( ei.start = 0; ei.start < fpmInfo_.size; ei.start += ei.length )
		{
			ioctl( fd, MEMUNLOCK, &ei );
			ioctl( fd, MEMERASE, &ei );
		}

		lseek( fd, 0, SEEK_SET ); // go to the first block

		Debug::trace( "FpmManager.write", "Partition erased!");

		// ---- //

		Debug::trace( "FpmManager.write", "Data to write: " + data );

		const size_t wSize( write( fd, data.c_str(), data.size() ) );

		if ( data.size() == wSize )
		{
			Debug::trace( "FpmManager.write", "data written to partition: " + partition );

			remove( partitionRecovery.c_str() );
		}
		else
		{
			Debug::trace( "FpmManager.write", "failed to write data to partition: " + partition +
					" error code: " + Integer::toString(errno) );
			status = WRITE_FAILURE;
		}
	}
	else
	{
		Debug::trace( "FpmManager.write", "data to large for partition: " + partition +
				" data size: " + Integer::toString(data.size()) + " partition size: " +
				Integer::toString(fpmInfo_.size) );

		status = DATA_OUTOFBOUNDS;
	}

	close( fd );

	return status;

#else

	// FLASH TODO
	return ERASE_FAILURE;

#endif

}

///////////////////////////////////////////////////////////////////////////////////////////////////
FpmManager::FpmErrorCode FpmManager::loadSerialFlashDefaults() {
	FpmErrorCode status;

	status = loadMfgSectionDefaults();
	if (status == FPM_OK) {
		Debug::trace("FpmManager.init",
				"default manufacturing written to serial flash");
	} else {
		Debug::trace("FpmManager.init",
				"writing default manufacturing data failed: "
						+ Integer::toString(status));
	}

	status = loadSWSectionDefaults();
	if (status == FPM_OK) {
		Debug::trace("FpmManager.init",
				"default ScaleSWConfiguration written to serial flash");
	} else {
		Debug::trace("FpmManager.init",
				"writing default ScaleSWConfiguration data failed: "
						+ Integer::toString(status));
	}

	status = loadHWSectionDefaults();
	if (status == FPM_OK) {
		Debug::trace("FpmManager.init",
				"default ScaleHWConfiguration written to serial flash");
	} else {
		Debug::trace("FpmManager.init",
				"writing default ScaleHWConfiguration data failed: "
						+ Integer::toString(status));
	}
	return status;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
FpmManager::FpmErrorCode FpmManager::loadMfgSectionDefaults() {
	/*! setup the section statistics for this section*/
	MtdSectionStatistics mfgStatistics;
	mfgStatistics.setSectionWriteCount(1);
	mfgStatistics.setSectorType(ManufacturingData);

	/*! setup the default data for this section*/
	MtdManufacturingData manufacturingData;
	manufacturingData.setManufacturingDataDefaults();
	std::string xmlData = manufacturingData.serializeXml();
	/*! section crc value is based on section data*/
	mfgStatistics.setSectionCrc(manufacturingData.getCrc());

	/*statistics first then section data */
	std::string mfgData = mfgStatistics.serialize();
	mfgData += xmlData;

	return writeSerialFlashPartition(ManufacturingData, mfgData);

}

///////////////////////////////////////////////////////////////////////////////////////////////////
FpmManager::FpmErrorCode FpmManager::loadSWSectionDefaults()
{
	fpmManagerNotATeamPlayer_ = false;

#if defined (__linux__)
	MtdScaleSWConfig mtdScaleSWConfig;
	mtdScaleSWConfig.setScaleSWConfigDefaults();
	mtdScaleSWConfig.show();

	const std::string xmlData( mtdScaleSWConfig.serializeXml() );
	const unsigned long crc( mtdScaleSWConfig.getCrc() );

	Debug::trace( "FpmManager.init", "loadSWSectionDefaults() crc: " + Integer::toString(crc) );

	// setup the section statistics for this section
	MtdSectionStatistics swStatistics;
	swStatistics.setSectionWriteCount( 1 );
	swStatistics.setSectorType( ScaleSWConfiguration );
	swStatistics.setSectionCrc( crc );

	const std::string swStatSerial( swStatistics.serialize() );

	// bitmaps then statistics and section data
	std::string bitmaps_;
	if ( MtdScaleSWConfig::dataOffset() )
		bitmaps_.assign( (char*) pBuffer_, MtdScaleSWConfig::dataOffset() );
	bitmaps_.append( swStatSerial );
	bitmaps_.append( xmlData );
	bitmaps_.append( std::string( fpmInfo_.size - bitmaps_.size(), sectionBlank ) );

	return writeSerialFlashPartition( ScaleSWConfiguration, bitmaps_ );
#else
	/*! setup the section statistics for this section*/
	std::string bitmaps_;					// copied from the Linux section
	MtdSectionStatistics swStatistics;
	swStatistics.setSectionWriteCount(1);
	swStatistics.setSectorType(ScaleSWConfiguration);


	MtdScaleSWConfig mtdScaleSWConfig;
	mtdScaleSWConfig.setScaleSWConfigDefaults();

	mtdScaleSWConfig.show();

	std::string xmlData = mtdScaleSWConfig.serializeXml();

	Debug::trace("FpmManager.init","loadSWSectionDefaults() crc: " + Integer::toString(mtdScaleSWConfig.getCrc()));
	/*! section crc value is based on section data*/
	swStatistics.setSectionCrc(mtdScaleSWConfig.getCrc());

	fpmManagerNotATeamPlayer_ = Configuration::getInstance()->toBoolCfgI(CfgInternal::FPMMANAGERNOTATEAMPLAYER);

	/*bitmaps then statistics and section data */
	std::string swStatSerial = swStatistics.serialize();

	if (fpmManagerNotATeamPlayer_)
	{
		bitmaps_.append(swStatSerial);
		bitmaps_.append(xmlData);
	}
	else
	{
		bitmaps_.insert(MtdScaleSWConfig::SIZEOF_UBOOTBITMAPS, swStatSerial);
		bitmaps_.insert(MtdScaleSWConfig::SIZEOF_UBOOTBITMAPS + swStatSerial.size(),
			xmlData);

		/*resize the data to fit the partition*/
		bitmaps_ = bitmaps_.erase(
			MtdScaleSWConfig::SIZEOF_UBOOTBITMAPS + swStatSerial.size()
			+ xmlData.size(), swStatSerial.size() + xmlData.size());
	}

	std::string scaleSwCfg = bitmaps_;

	return writeSerialFlashPartition(ScaleSWConfiguration, scaleSwCfg);
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
FpmManager::FpmErrorCode FpmManager::loadHWSectionDefaults()
{
	/*! setup the section statistics for this section*/
	MtdSectionStatistics hwStatistics;
	hwStatistics.setSectionWriteCount(1);
	hwStatistics.setSectorType(ScaleHWConfiguration);

	MtdScaleHwCfg mtdScaleHwCfg;
	mtdScaleHwCfg.setScaleHardwareDefaults();
	std::string xmlData = mtdScaleHwCfg.serializeXml();
	/*! section crc value is based on section data*/
	hwStatistics.setSectionCrc(mtdScaleHwCfg.getCrc());

	/*statistics first then section data */
	std::string scaleHwCfg = hwStatistics.serialize();
	scaleHwCfg += xmlData;

	return writeSerialFlashPartition(ScaleHWConfiguration, scaleHwCfg);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
FpmManager::FpmErrorCode FpmManager::loadNGWWrapperSectionDefaults()
{

#if defined (__linux__)
	MtdNGWWrapperData mtdWrapperData;
	mtdWrapperData.setWrapperDataDefaults();
	mtdWrapperData.show();

	const std::string xmlData( mtdWrapperData.serializeXml() );
	const unsigned long crc( mtdWrapperData.getCrc() );

	Debug::trace( "FpmManager.init", "loadNGWWrapperSectionDefaults() crc: " + Integer::toString(crc) );

	// setup the section statistics for this section
	MtdSectionStatistics swStatistics;
	swStatistics.setSectionWriteCount( 1 );
	swStatistics.setSectorType( WrapperData );
	swStatistics.setSectionCrc( crc );

	const std::string swStatSerial( swStatistics.serialize() );

	// bitmaps then statistics and section data
	std::string bitmaps_;
	if (MtdNGWWrapperData::dataOffset())
		bitmaps_.assign( (char*) pBuffer_, MtdNGWWrapperData::dataOffset() );
	bitmaps_.append( swStatSerial );
	bitmaps_.append( xmlData );
	bitmaps_.append( std::string( fpmInfo_.size - bitmaps_.size(), sectionBlank ) );

	return writeSerialFlashPartition( WrapperData, bitmaps_ );
#else
	return UNSUPPORTED_OS;					//  had to still return something so I added another enum since the code was all for Linux
#endif // __linux__
}

///////////////////////////////////////////////////////////////////////////////////////////////////
FpmManager::FpmErrorCode FpmManager::loadScaleDataDefaults()
{
#if defined (__linux__)
	MtdScaleData mtdScaleData;
	mtdScaleData.setScaleDataDefaults();
	mtdScaleData.show();

	const std::string xmlData( mtdScaleData.serializeXml() );
	const unsigned long crc( mtdScaleData.getCrc() );

	Debug::trace( "FpmManager.init", "loadScaleDataDefaults() crc: " + Integer::toString(crc) );

	// setup the section statistics for this section
	MtdSectionStatistics swStatistics;
	swStatistics.setSectionWriteCount( 1 );
	swStatistics.setSectorType( ScaleData );
	swStatistics.setSectionCrc( crc );

	const std::string swStatSerial( swStatistics.serialize() );

	// bitmaps then statistics and section data
	std::string bitmaps_;
	if ( MtdScaleData::dataOffset() )
		bitmaps_.assign( (char*) pBuffer_, MtdScaleData::dataOffset() );
	bitmaps_.append( swStatSerial );
	bitmaps_.append( xmlData );
	bitmaps_.append( std::string( fpmInfo_.size - bitmaps_.size(), sectionBlank ) );

	return writeSerialFlashPartition( ScaleData, bitmaps_ );
#else
	return UNSUPPORTED_OS;					//  had to still return something so I added another enum since the code was all for Linux
#endif // __linux__
}
