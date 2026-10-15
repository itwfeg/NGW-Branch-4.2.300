/**
 * @file DiagnosticsManager.cpp
 * @brief Manager for diagnostic related events.
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#include "DiagnosticsDB.h"
#include "DiagnosticsEdgeDevice.h"
#include "DiagnosticsManager.h"

#include "Messages/DiagnosticsAuditLog.h"
#include "Messages/DiagnosticsAuditLogServiceVisit.h"
#include "Messages/DiagnosticsCheckErrorHandler.h"
#include "Messages/DiagnosticsClearServiceMaintRecord.h"
#include "Messages/DiagnosticsConfiguration.h"
#include "Messages/DiagnosticsExportDatabase.h"
#include "Messages/DiagnosticsInitWizard.h"
#include "Messages/DiagnosticsMessageBasic.h"
#include "Messages/DiagnosticsPerformWizardTesting.h"
#include "Messages/DiagnosticsPrintLabelStats.h"
#include "Messages/DiagnosticsPrintServiceMaintLog.h"
#include "Messages/DiagnosticsReport.h"
#include "Messages/DiagnosticsSendDatabase.h"
#include "Messages/DiagnosticsServiceActions.h"
#include "Messages/DiagnosticsServiceMaintLog.h"
#include "Messages/DiagnosticsShowWizard.h"
#include "Messages/DiagnosticsStartErrorHandler.h"
#include "Messages/DiagnosticsStatus.h"
#include "Messages/DiagnosticsValueMax.h"

#include "Util/Dict.h"
#include "Util/Timestamp.h"

#include <SystemEvent.h>

#include <PrinterEvent.h>
#include <PrinterHeadDotStatus.h>
#include <PrinterHeadWear.h>
#include <PrinterLabelReady.h>
#include <PrinterStatus.h>

#include <PrinterSystemStatus.h>
#include <PrinterSystemRequestStatus.h>

#include <WeigherSystemStatus.h>
#include <WeigherSystemRequestStatus.h>
#include <UsbWeigherSystemStatus.h>

#include <EvolWrapperDiagnosticsTestResult.h>
#include <EvolWrapperDiagnosticsTest.h>
#include <EvolWrapperErrorHistory.h>
#include <EvolWrapperErrorInfo.h>
#include <EvolWrapperEventCapture.h>
#include <EvolWrapperInquire.h>
#include <EvolWrapperPackageTimings.h>
#include <EvolWrapperPackageTimingsPointer.h>
#include <EvolWrapperResetDefaultPackageTimings.h>
#include <EvolWrapperSettings.h>
#include <EvolWrapperSystemStatus.h>
#include <EvolWrapperWrap.h>

#include <WrapperEvent.h>
#include <WrapperMode.h>
#include <WrapperRequestSettings.h>
#include <WrapperSystemRequestStatus.h>

#include <SystemStatusInfo.h>

#include <FpmManager.h>

#include <Configuration.h>
#include <Event.h>
#include <OperationModeStatus.h>

#include <SessionEvent.h>
#include <SessionManagerHelper.h>

#include <utilstring.h>

#include <fstream>
#include <time.h>
#include <sys/stat.h>

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int DiagnosticsManager::dataType_ = 0;
std::string DiagnosticsManager::dataTypeName_( "rtc.diagnosticsmanager.diagnosticsmanager" );
#endif

std::string DiagnosticsManager::category_( "DiagnosticsManager" );

util_thread::Mutex<> DiagnosticsManager::instanceMutex_;
DiagnosticsManager *DiagnosticsManager::instance_( NULL );

#define TRACE( logline ) Debug::trace( category_, (logline) )
#define TRACEFUNC( logline ) if (Debug::isTracing( category_ + "." + __func__ )) TRACE( (logline) )
#define ISTRACING Debug::isTracing( category_)

///////////////////////////////////////////////////////////////////////////////////////////////////
DiagnosticsManager::DiagnosticsManager( const char *name ) :
	_Mybase( name ),
	_Myimpl(),
	db_( NULL ),
	edge_( NULL ),
	isScale_( false ),
	isWrapper_( false ),
	wrapperCycleCounter_( 0 ),
	printer1Counts_( 0 ),
	printer2Counts_( 0 ),
	printer3Counts_( 0 ),
	thread_( NULL ),
	state_( DiagStopped ),
	wizardState_( WizardIdle ),
	requestedWizardState_( WizardIdle ),
	currentDevice_( 0 ),
	currentErrorCode_( 0 )
{
#ifdef USE_ACCOUNTANT
	if ( !dataType )
		dataType_ = util_acc::registerDataType( dataTypeName_ );

	util_acc::addInstance( dataType_ );
#endif

	TRACE( "CTOR" );

	// retrieve wrapper cycle counter and printer counts
	FpmManager *pSerialFlash( FpmManager::getInstance() );

	if ( !pSerialFlash )
		TRACE( "error retrieving fpm manager!" );
	else
	{
		const std::string style( pSerialFlash->getMFGSystemStyle() );

		TRACE( "style: " + style );

		// check if scale or wrapper
		if ( 2 <= style.length() )
		{
			if (( 'H' == style[0] ) && ( 'T' == style[1] )) // HTx, HTi, HTs
				isScale_ = true;
			else if (( 'N' == style[0] ) && ( 'G' == style[1] )) // NGW, NGS
				isWrapper_ = true;
			else if (( 'A' == style[0] ) && ( 'W' == style[1] )) // AWS3
				isWrapper_ = true;
			else if (( 'E' == style[0] ) && ( 'P' == style[1] )) // EPCP
				isWrapper_ = true;
		}

		// read proper mtd section
		if ( isScale_ )
		{
			MtdScaleData *pPrinterStatistics( pSerialFlash->readScaleData() );

			if ( !pPrinterStatistics )
				TRACE( "error retrieving scale data!" );
			else
			{
				printer1Counts_ = pPrinterStatistics->getPrimaryPrinterCounts();
				printer2Counts_ = pPrinterStatistics->getSecondaryPrinterCounts();
				printer3Counts_ = pPrinterStatistics->getTertiaryPrinterCounts();
			}
        }
		else if ( isWrapper_ )
		{
			MtdNGWWrapperData *pPrinterStatisticsNGW( pSerialFlash->readWrapperData() );

			if ( !pPrinterStatisticsNGW )
				TRACE( "error retrieving wrapper data!" );
			else
			{
				wrapperCycleCounter_ = pPrinterStatisticsNGW->getCycleCounts();
				printer1Counts_ = pPrinterStatisticsNGW->getPrimaryPrinterCounts();
				printer2Counts_ = pPrinterStatisticsNGW->getSecondaryPrinterCounts();
				printer3Counts_ = pPrinterStatisticsNGW->getTertiaryPrinterCounts();
			}
		}
	}

	TRACE( "Wrapper Cycles: " + util_string::longtostring( wrapperCycleCounter_ ) );
	TRACE( "PriPrinter Counts: " + util_string::longtostring( printer1Counts_ ) );
	TRACE( "SecPrinter Counts: " + util_string::longtostring( printer2Counts_ ) );
	TRACE( "TerPrinter Counts: " + util_string::longtostring( printer3Counts_ ) );

	// generate edge device
	edge_ = new DiagnosticsEdgeDevice();
	edge_->setPrinterCounts( 0, printer1Counts_ );
	edge_->setPrinterCounts( 1, printer2Counts_ );
	edge_->setPrinterCounts( 2, printer3Counts_ );
	edge_->setWrapperCycleCount( wrapperCycleCounter_ );

	// init
	initialize();

	TRACE( "CTOR: done!" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
DiagnosticsManager::~DiagnosticsManager()
{
	TRACE( "DTOR" );

	if ( thread_ )
	{
		TRACE( "stopping manager..." );

		thread_->StopThread();
		thread_->WaitForCompletion();

		thread_ = NULL;
	}

	TRACE( "shutdown..." );
	ShutdownNicely();

	deregisterAllEvents();

	// remove the edge device
	if ( edge_ )
	{
		delete edge_;
		edge_ = NULL;
	}

#ifdef USE_ACCOUNTANT
	util_acc::removeInstance( dataType_ );
#endif

	TRACE( "DTOR: done!" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
DiagnosticsManager::State DiagnosticsManager::state() const
{
	m_.acquire();
	State s( state_ );
	m_.release();

	return s;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DiagnosticsManager::hardwareInfo( EvolDeviceType device, std::string &model, std::string &serialNumber, unsigned int &cycleCount ) const
{
	model = "Unknown";
	serialNumber = "Unknown";
	cycleCount = 0;

	switch ( device )
	{
	case LabelApplier:
	case LabelApplierSystem:
		// TODO;
		break;

	case Printer:
	case PrinterSystem:
    	// TODO
		break;

	case Weigher:
	case WeigherSystem:
    	// TODO
		break;

	case Wrapper:
	case WrapperSystem:

		model = lastWrapperSystemInfo_.getAwsModelNumber();

		if ( model.empty() )
			model = "Unknown";

		serialNumber = lastWrapperSystemInfo_.getAwsSerialNumber();

		if ( serialNumber.empty() )
			serialNumber = "Unknown";

		cycleCount = wrapperCycleCounter_;

		break;

	default:
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void DiagnosticsManager::handleMessage( DiagnosticsConfigurationReq &msg )
{
	// TODO
	DiagnosticsConfiguration resp( this );
	resp.serviceContactInfo_ = Configuration::getInstance()->toStringCfg( fieldOffsetCfgDiagServiceContactInfo );
	resp.gatewayDeviceAddress_ = Configuration::getInstance()->toStringCfg( fieldOffsetCfgDiagGatewayDeviceAddress );
	resp.gatewayDeviceLogLevel_ = Configuration::getInstance()->toIntCfg( fieldOffsetCfgDiagGatewayDeviceStatusMsgLogLevel );
	resp.valueMaxDiagCollectionDisabled_ = Configuration::getInstance()->toBoolCfg( fieldOffsetCfgValueMaxDiagCollectionDisabled );

	dispatchEvent( &resp, msg.getSourceObject() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void DiagnosticsManager::handleMessage( DiagnosticsConfiguration &msg )
{
	Configuration::getInstance()->setCfg( fieldOffsetCfgDiagServiceContactInfo, msg.serviceContactInfo_ );
	Configuration::getInstance()->setCfg( fieldOffsetCfgDiagGatewayDeviceAddress, msg.gatewayDeviceAddress_ );
	Configuration::getInstance()->setCfg( fieldOffsetCfgDiagGatewayDeviceStatusMsgLogLevel, util_string::longtostring( msg.gatewayDeviceLogLevel_ ) );
	Configuration::getInstance()->setCfg( fieldOffsetCfgValueMaxDiagCollectionDisabled, (( msg.valueMaxDiagCollectionDisabled_ ) ? "1" : "0") );

	// save configuration changes
	EvolMessage saveConfig( SessionDev, SnRequestSaveConfiguration, this );
	dispatchEvent( &saveConfig );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void DiagnosticsManager::handleMessage( DiagnosticsStatusReq &msg )
{
	std::string gateway;
	bool discovered( false );

	DiagnosticsStatus resp( this );
	resp.gatewayOnline_ = edge_->online( gateway, discovered );
	resp.gatewayDiscovered_ = discovered;
	resp.gatewayDeviceAddress_ = gateway;

	resp.queueSize_ = edge_->offlineMessages();

	dispatchEvent( &resp, msg.getSourceObject() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void DiagnosticsManager::handleMessage( DiagnosticsAuditLogReq &msg )
{
	TRACE( "Audit records request: " +
		util_string::longtostring( msg.filter_ ) + " " +
		util_string::longtostring( msg.age_ ) );

	util::DictionaryList results;
	results.setCommaSeparated( true );

	// retrieve records
	if ( !db_->retrieveAuditLog( msg.filter_, msg.age_, results ) )
		TRACE( "failed to retrieve audit log" );
	else
	{
		// send response message
		DiagnosticsAuditLog resp( this );
		resp.count_ = (int) results.size();
		resp.records_ = results.format();

		// go!
		dispatchEvent( &resp, msg.getSourceObject() );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void DiagnosticsManager::handleMessage( DiagnosticsAuditLogRecordReq &msg )
{
	TRACE( "Audit record request " + msg.timestamp_ );

	// send response message
	DiagnosticsAuditLogRecord resp( this );
	resp.timestamp_ = msg.timestamp_;

	// retrieve records
	if ( !db_->retrieveAuditRecord( resp.timestamp_, resp.severity, resp.source, resp.sourceExt, resp.description ) )
		TRACE( "failed to retrieve audit log record" );
	else
	{
		// go!
		dispatchEvent( &resp, msg.getSourceObject() );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void DiagnosticsManager::handleMessage( DiagnosticsAuditLogServiceVisitStart &msg )
{
	TRACE( "Starting service visit" );
	db_->createServiceVisit( true, false, msg.description_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void DiagnosticsManager::handleMessage( DiagnosticsAuditLogServiceVisitStop &msg )
{
	TRACE( "Stopping service visit\n\nNotes:\n" + msg.description_ );
	db_->createServiceVisit( false, true, msg.description_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void DiagnosticsManager::handleMessage( DiagnosticsAuditLogServiceVisitStatus &msg )
{
	TRACE( "Checking for active service visit" );

	// send response message
	DiagnosticsAuditLogServiceVisitStatus resp( this );
	resp.start_ = !(resp.stop_ = db_->checkServiceVisit());

	dispatchEvent( &resp, msg.getSourceObject() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void DiagnosticsManager::handleMessage( DiagnosticsCheckErrorHandlerReq &msg )
{
	std::string diag;
	int count( 0 );

	TRACE( "Checking for diagnostics handler: " +
		util_string::longtostring( msg.device_ ) + " " +
		util_string::longtostring( msg.errorCode_ ) );

	DiagnosticsCheckErrorHandler resp( this );
	resp.device_ = msg.device_;
	resp.errorCode_ = msg.errorCode_;

	// lookup error
	if ( db_->lookupEvent( resp.device_, 0, resp.errorCode_, resp.event_, diag, resp.available_, count ) )
	{
		TRACE( "found event, count at " + util_string::inttostring( count ) );

		// update hardware information
		if ( diag.length() )
		{
			std::string hardwareModel;
			std::string hardwareSerial;
			unsigned int hardwareCycles( 0 );

			// retrieve hardware info
			hardwareInfo( (EvolDeviceType) resp.device_, hardwareModel, hardwareSerial, hardwareCycles );

			// update!
			if ( !db_->updateHardwareInfo( diag, hardwareModel, hardwareSerial, hardwareCycles ) )
			{
				TRACE( "failed to update hardware info for diag " + diag );
				return;
			}
		}

		// when error happens too many times in a row force the wizard
		int maxCount( db_->lookupEventMaxCount( resp.device_, 0, resp.errorCode_ ) );

		if ( !maxCount )
			maxCount = 3;

		resp.force_ = (maxCount <= count);

		// if we are running diagnostics currently then ignore!
		if ( WizardIdle != wizardState() )
		{
			TRACE( "ignoring error because we are already running diagnostics!" );
			resp.available_ = false;
		}

		// go!
		dispatchEvent( &resp, msg.getSourceObject() );
	}

	// update edge device
	unsigned int severity;

	if ( (severity = db_->lookupEventSeverity( resp.device_, 0, resp.errorCode_ )) )
	{
		const unsigned int code( (resp.device_ * 1000) + resp.errorCode_ );

		// send event
		edge_->sendEvent( code, severity, resp.available_, resp.force_ );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void DiagnosticsManager::handleMessage( DiagnosticsStartErrorHandler &msg )
{
	TRACE( "Starting diagnostics for event " + msg.event_ );

	// 1 = manual
	// 2 = automatic
	const int method( msg.auto_ ? 2 : 1 );

	std::string diag;
	std::string diagStep;
	int diagErrorCode( 0 );

	// create diagnostic wizard
	if ( !db_->createDiagWizard( msg.event_, method, diag, diagStep, diagErrorCode ) )
		TRACE( "Failed to start diagnostics!" );
	else if ( !diagErrorCode )
		TRACE( "Invalid diagnostic code!" );
	else
	{
		DiagnosticsShowWizard resp( this );
		resp.event_ = msg.event_;
		resp.timestamp_ = util::Timestamp::formatDateTime( util_string::stringtolonglong( diag ) );
		resp.diagEvent_ = diagStep;
		resp.diagErrorCode_ = diagErrorCode;

		// retrieve hardware information
		if ( !db_->lookupHardwareInfo( diag, resp.hardwareModel_, resp.hardwareSerial_, resp.hardwareCycles_ ) )
			TRACE( "Failed to lookup hardware info for diag " + diag );

		// go!
		dispatchEvent( &resp, msg.getSourceObject() );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void DiagnosticsManager::handleMessage( DiagnosticsStartErrorHandlerFromMaintRecord &msg )
{
	TRACE( "Starting diagnostics for maint record: " + msg.maintRecord_ );

	// 3 = operator initiated
	// 4 = service initiated
	const int method( msg.service_ ? 4 : 3 );

	std::string event;
	std::string diag;
	std::string diagStep;
	int diagErrorCode( 0 );

	// create diagnostic wizard
	if ( !db_->createDiagWizardFromMaintRecord( msg.maintRecord_, method, event, diag, diagStep, diagErrorCode ) )
	{
		TRACE( "Failed to start diagnostics!" );

		DiagnosticsErrorHandlerFailed resp( this );
		dispatchEvent( &resp, msg.getSourceObject() );
	}
	else if ( !diagErrorCode )
	{
		TRACE( "Invalid diagnostic code!" );

		DiagnosticsErrorHandlerFailed resp( this );
		dispatchEvent( &resp, msg.getSourceObject() );
	}
	else
	{
		int device;
		int deviceExt;
		int errorCode;

		// lookup device information
		if ( !db_->lookupDeviceInfo( event, device, deviceExt, errorCode ) )
		{
			TRACE( "Failed to retrieve device info!" );
			return;
		}

		DiagnosticsShowWizard resp( this );
		resp.event_ = event;
		resp.timestamp_ = util::Timestamp::formatDateTime( util_string::stringtolonglong( diag ) );
		resp.diagEvent_ = diagStep;
		resp.diagErrorCode_ = diagErrorCode;

		// retrieve hardware info
		hardwareInfo( (EvolDeviceType) device, resp.hardwareModel_, resp.hardwareSerial_, resp.hardwareCycles_ );

		// update hardware info
		if ( !db_->updateHardwareInfo( diag, resp.hardwareModel_, resp.hardwareSerial_, resp.hardwareCycles_ ) )
			TRACE( "Failed to update hardware info for diag " + diag );

		// go!
		dispatchEvent( &resp, msg.getSourceObject() );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void DiagnosticsManager::handleMessage( DiagnosticsInitWizardReq &msg )
{
	TRACE( "Init diagnostics event " + msg.diagEvent_ );

	// switch to ready state
	setWizardState( msg.diagEvent_, WizardReady );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void DiagnosticsManager::handleMessage( DiagnosticsPerformWizardTestingReq &msg )
{
	TRACE( "Performing test for diagnostics event " + msg.diagEvent_ );

	// validate current state!
	if ( current_ != msg.diagEvent_ )
		TRACE( "Cannot perform diag test, diag event mismatch!" );
	else if ( WizardReady != wizardState_ )
		TRACE( "Cannot perform diag test, system not ready!" );
	else
	{
		setWizardState( msg.diagEvent_, WizardTesting );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void DiagnosticsManager::handleMessage( DiagnosticsExitWizard& /*msg*/ )
{
	TRACE( "Exiting diagnostics wizard" );

	// nothing to do
	if ( current_.empty() )
		return;
	else if ( WizardIdle == wizardState_ )
		return;

	setWizardState( current_, WizardIdle );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void DiagnosticsManager::handleMessage( DiagnosticsServiceMaintLogReq &msg )
{
	TRACE( "Maint records request: " +
		util_string::longtostring( msg.filter_ ) + " " +
		util_string::longtostring( msg.age_ ) );

	util::DictionaryList results;
	results.setCommaSeparated( true );

	// retrieve records
	if ( !db_->retrieveMaintLogRecords( msg.filter_, msg.age_, results ) )
		TRACE( "failed to retrieve maint records" );
	else
	{
		// send response message
		DiagnosticsServiceMaintLog resp( this );
		resp.count_ = (int) results.size();
		resp.maintRecords_ = results.format();

		// go!
		dispatchEvent( &resp, msg.getSourceObject() );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void DiagnosticsManager::handleMessage( DiagnosticsServiceActionsReq &msg )
{
	TRACE( "Service actions request: " + util_string::longtostring( msg.errorCode_ ) );

	util::DictionaryList results;
	results.setCommaSeparated( true );

	// retrieve records
	if ( !db_->retrieveActionRecords( msg.errorCode_, results ) )
		TRACE( "failed to retrieve actions" );
	else
	{
		// send response message
		DiagnosticsServiceActions resp( this );
		resp.count_ = (int) results.size();
		resp.actions_ = results.format();

		// go!
		dispatchEvent( &resp, msg.getSourceObject() );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void DiagnosticsManager::handleMessage( DiagnosticsClearServiceMaintRecord &msg )
{
	TRACE( "Clear maint record: " + msg.maintRecord_ + "\n"
		"    reason: " + util_string::longtostring( msg.reason_ ) + "\n"
		"    service action 1: " + util_string::longtostring( msg.serviceMethod1_ ) + " " + util_string::longtostring( msg.servicePart1_ ) + "\n"
		"    service action 2: " + util_string::longtostring( msg.serviceMethod2_ ) + " " + util_string::longtostring( msg.servicePart2_ ) + "\n"
		"    service action 3: " + util_string::longtostring( msg.serviceMethod3_ ) + " " + util_string::longtostring( msg.servicePart3_ ) + "\n"
		"    service action 4: " + util_string::longtostring( msg.serviceMethod4_ ) + " " + util_string::longtostring( msg.servicePart4_ ) + "\n"
		"    service notes: " + msg.serviceNotes_ );

	int actions( 0 );

	int methods[4];
	methods[0] = msg.serviceMethod1_;
	methods[1] = msg.serviceMethod2_;
	methods[2] = msg.serviceMethod3_;
	methods[3] = msg.serviceMethod4_;

	int parts[4];
	parts[0] = msg.servicePart1_;
	parts[1] = msg.servicePart2_;
	parts[2] = msg.servicePart3_;
	parts[3] = msg.servicePart4_;

	for ( int i( 0 ); i < 4; ++i )
	{
		if (( methods[i] ) && ( parts[i] ))
			++actions;
		else
		{
			break;
		}
	}

	if ( !db_->clearMaintLogRecord( msg.maintRecord_, (DiagnosticsDB::maintLogResolutionReason) msg.reason_, actions, methods, parts, msg.serviceNotes_ ) )
		TRACE( "failed to clear maintenance record" );
	else
	{
		util::DictionaryList dict;
		util::DictionaryList dictResolved;

		// pull open and resolved records
		db_->retrieveMaintLogRecords( 0, db_->housekeepingDays(), dict );
		dict.sortBySeverity();

		db_->retrieveMaintLogRecords( 1, 1, dictResolved );

		// some data massaging is needed here... first we must update health info with the
		// resolved record then we must remove it
		if ( dictResolved.size() )
		{
			util::DictionaryList dictPrev( dict );
			dictPrev.push_back( dictResolved.front() );
			dictPrev.sortBySeverity();

			edge_->setHealthInfo( dictPrev );
		}

		// update health info
		edge_->setHealthInfo( dict );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void DiagnosticsManager::handleMessage( DiagnosticsPrintServiceMaintLog &msg )
{
	TRACE( "Printing service maint log: " +
		util_string::longtostring( msg.filter_ ) + " " +
		util_string::longtostring( msg.age_ ) );

	// send response message
	DiagnosticsReport resp( this );
	resp.title_ = "DIAGNOSTICS LOG";

	if ( !db_->generateMaintLogReport( msg.filter_, msg.age_, resp.body_ ) )
		TRACE( "failed to generate maintenance log report" );
	else
	{
		// go!
		dispatchEvent( &resp, msg.getSourceObject() );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void DiagnosticsManager::handleMessage( DiagnosticsExportDatabase &msg )
{
	std::string filename( msg.filename_ );
	bool success( false );

	TRACE( "Exporting database to: " + filename );

	if ( !filename.size() )
		TRACE( "bad export filename" );
	else
	{
		// close the database
		closeDatabase();

		// remove filename
		std::remove( filename.c_str() );

		// copy database to filename
		std::ifstream is( DiagnosticsDB::dbFile().c_str(), std::ios::binary );
		std::ofstream os( filename.c_str(), std::ios::binary );

		os << is.rdbuf();

		is.close();
		os.close();

		// open database
		if ( !openDatabase() )
		{
			TRACE( "failed to open database after export!" );
			setState( DiagStop );
		}
		else
		{
			TRACE( "export database success" );
			success = true;
		}

#if defined ( __linux__ )
		// sync disk
		sync();
#endif // __linux__
	}

	// send response message
	DiagnosticsExportDatabaseComplete resp( this );
	resp.success_ = success;

	// go!
	dispatchEvent( &resp, msg.getSourceObject() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void DiagnosticsManager::handleMessage( DiagnosticsSendDatabaseEmail &msg )
{
	// Note:
	// This is not supported until required, for now it's just for show.

	// send response message
	DiagnosticsSendDatabaseComplete resp( this );
	resp.success_ = false;

	// go!
	dispatchEvent( &resp, msg.getSourceObject() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void DiagnosticsManager::handleMessage( DiagnosticsSendDatabaseGatewayDevice &msg )
{
	// Note:
	// This is not supported until we have a gateway device and a message to upload.

	// send response message
	DiagnosticsSendDatabaseComplete resp( this );
	resp.success_ = false;

	// go!
	dispatchEvent( &resp, msg.getSourceObject() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void DiagnosticsManager::handleMessage( DiagnosticsPrintLabelStats &msg )
{
	TRACE( "Have print label stats " + util_string::longtostring( msg.printerType_ ) );

	// validate printer number
	if (( FirstPrinter <= msg.printerType_ ) && ( msg.printerType_ < MaxPrinterTypes ))
	{
		const int printerNum( msg.printerType_ + 1 );

		// validate pixels
		if (( 0 == (msg.pixelsPrinted_.size() % sizeof(int)) ) &&
			( 0 == (msg.pixelsRemoved_.size() % sizeof(int)) ) &&
			( msg.pixelsPrinted_.size() == msg.pixelsRemoved_.size() ))
		{
			std::vector<int> pixelsPrinted;
			std::vector<int> pixelsRemoved;

			for ( size_t i( 0 ); i < msg.pixelsPrinted_.size(); i += sizeof(int) )
				pixelsPrinted.push_back( *reinterpret_cast<const int*>( &msg.pixelsPrinted_[i] ) );

			for ( size_t i( 0 ); i < msg.pixelsRemoved_.size(); i += sizeof(int) )
				pixelsRemoved.push_back( *reinterpret_cast<const int*>( &msg.pixelsRemoved_[i] ) );

			TRACE( "Num printer dots " +
				util_string::longlongtostring( pixelsPrinted.size() ) + " " +
				util_string::longlongtostring( pixelsRemoved.size() ) );

			// generate records
			db_->updateDotUsage( printerNum, pixelsPrinted, pixelsRemoved );
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void DiagnosticsManager::handleMessage( DiagnosticsValueMax &msg )
{
	TRACE( "Have value max message for trans " + util_string::longtostring( msg.transactionNum_ ) );

	if ( Configuration::getInstance()->toBoolCfg( fieldOffsetCfgValueMaxDiagCollectionDisabled ) )
	{
		TRACE( "Value max diagnostics collection is disabled" );
		return;
	}

	// DiagnosticsManager.handleMessage<DiagnosticsValueMax>
	TRACEFUNC( "ValueMax information:\n"
		"    counts:           " + util_string::longtostring( msg.counts_ ) + "\n" +
		"    scaleFactor:      " + util_string::longtostring( msg.scaleFactor_ ) + "\n" +
		"    tiltX:            " + util_string::doubletostring( msg.tiltX_, 0, 2 ) + "\n" +
		"    tiltY:            " + util_string::doubletostring( msg.tiltY_, 0, 2 ) + "\n" +
		"    transactionNum:   " + util_string::longtostring( msg.transactionNum_ ) + "\n" +
		"    itemProductNum:   " + util_string::longtostring( msg.itemProductNum_ ) + "\n" +
		"    itemUnitPrice:    " + util_string::doubletostring( msg.itemUnitPrice_, 0, 4 ) + "\n" +
		"    itemQuantity:     " + util_string::longtostring( msg.itemQuantity_ ) + "\n" +
		"    itemWeight:       " + util_string::doubletostring( msg.itemWeight_, 0, 4 ) + "\n" +
		"    itemTare:         " + util_string::doubletostring( msg.itemTare_, 0, 4 ) + "\n" +
		"    itemTotalPrice:   " + util_string::doubletostring( msg.itemTotalPrice_, 0, 4 ) );

	double calcItemNetWeight( 0.0 );
	double calcItemTotalPrice( 0.0 );
	double calcSavings( 0.0 );

	// compute monetary divisor
	double monetaryDivisor( 1.0 );

	int monetaryDigits( Configuration::getInstance()->getPrimaryMonetaryDecimalDigits() );

	while ( monetaryDigits-- )
		monetaryDivisor /= 10.0;

	// Note:
	// The root desire here is to know how much money the retailer is saving by using Value Max
	// technology. To figure this out we must back compute a weight without correction and
	// re-calculate total price with uncorrected weight. We must pay special attention to how
	// we round as that is where the retailer will make the majority of their savings... by
	// rounding the weight up to next division.

	// figure out which weigher division we are in
	const int weigherDivisionLow( msg.counts_ / msg.scaleFactor_ );
	const int weigherDivisionHigh( weigherDivisionLow + 1 );

	const int deltaLow( msg.counts_ - (weigherDivisionLow * msg.scaleFactor_) );
	const int deltaHigh( (weigherDivisionHigh * msg.scaleFactor_) - msg.counts_ );

	int weigherDivision( weigherDivisionHigh );

	if ( deltaLow < deltaHigh )
		weigherDivision = weigherDivisionLow;

	// sanity check (weight)
	double itemGrossWeight( weigherDivision * msg.scaleFactor_ );
	itemGrossWeight /= 10000.0;

	double itemNetWeight( itemGrossWeight - msg.itemTare_ );
	double itemNetWeightDelta( fabs( msg.itemWeight_ - itemNetWeight ) );

	TRACEFUNC( "Computed weights:\n"
		"    gross: " + util_string::doubletostring( itemGrossWeight, 0, 4 ) + "\n" +
		"    net:   " + util_string::doubletostring( itemNetWeight, 0, 4 ) + "\n" +
		"    delta: " + util_string::doubletostring( itemNetWeightDelta, 0, 4 ) );

	if ( 0.0001 <= itemNetWeightDelta )
	{
		TRACE( "Error computing net weight for value max " + util_string::doubletostring( itemNetWeight, 0, 4 ) );
	}
	else
	{
		// sanity check (total price)
		double itemTotalPrice( itemNetWeight * msg.itemUnitPrice_ );
		itemTotalPrice /= monetaryDivisor;
		itemTotalPrice = std::round( itemTotalPrice );
		itemTotalPrice *= monetaryDivisor;

		double itemTotalPriceDelta( fabs( msg.itemTotalPrice_ - itemTotalPrice ) );

		TRACEFUNC( "Computed price:\n"
			"    price: " + util_string::doubletostring( itemTotalPrice, 0, 4 ) + "\n" +
			"    delta: " + util_string::doubletostring( itemTotalPriceDelta, 0, 4 ) );

		if ( 0.0001 <= itemTotalPriceDelta )
		{
			TRACE( "Error computing price for value max " + util_string::doubletostring( itemTotalPrice, 0, 4 ) );
		}
		else
		{
			// Okay... at this point our math matches up with what the scale did... now lets compute
			// weight without cosine correction and see if the retailer made any money.

			// compute angle of tilt in radians
			double ax( tan( msg.tiltX_ * 0.0174533 ) );
			double ay( tan( msg.tiltY_ * 0.0174533 ) );

			double a( 1.0 / sqrt( 1.0 + ax*ax + ay*ay ) );

			double origCounts( msg.counts_ );
			origCounts *= a;
			origCounts = std::round( origCounts );

			TRACEFUNC( "Original counts:\n"
				"    a:      " + util_string::doubletostring( a, 0, 4 ) + "\n" +
				"    counts: " + util_string::doubletostring( origCounts, 0, 4 ) );

			// figure out which weigher division we are in
			const int origWeigherDivisionLow( origCounts / msg.scaleFactor_ );
			const int origWeigherDivisionHigh( origWeigherDivisionLow + 1 );

			const int origDeltaLow( origCounts - (origWeigherDivisionLow * msg.scaleFactor_) );
			const int origDeltaHigh( (origWeigherDivisionHigh * msg.scaleFactor_) - origCounts );

			int origWeigherDivision( origWeigherDivisionHigh );

			if ( origDeltaLow < origDeltaHigh )
				origWeigherDivision = origWeigherDivisionLow;

			// compute weight
			double origItemGrossWeight( origWeigherDivision * msg.scaleFactor_ );
			origItemGrossWeight /= 10000.0;

			double origItemNetWeight( origItemGrossWeight - msg.itemTare_ );

			// compute total price
			double origItemTotalPrice( origItemNetWeight * msg.itemUnitPrice_ );
			origItemTotalPrice /= monetaryDivisor;
			origItemTotalPrice = std::round( origItemTotalPrice );
			origItemTotalPrice *= monetaryDivisor;

			double savings( msg.itemTotalPrice_ - origItemTotalPrice );

			TRACEFUNC( "Original weight and total price:\n"
				"    weight:     " + util_string::doubletostring( origItemNetWeight, 0, 4 ) + "\n" +
				"    totalPrice: " + util_string::doubletostring( origItemTotalPrice, 0, 4 ) + "\n" +
				"    savings:    " + util_string::doubletostring( savings, 0, 4 ) );

			calcItemNetWeight = origItemNetWeight;
			calcItemTotalPrice = origItemTotalPrice;
			calcSavings = savings;
		}
	}

	// create!
	db_->createValueMax(
		msg.transactionNum_,
		msg.itemProductNum_,
		msg.itemUnitPrice_,
		msg.itemQuantity_,
		msg.itemWeight_,
		msg.itemTare_,
		msg.itemTotalPrice_,
		msg.counts_,
		msg.scaleFactor_,
		msg.tiltX_,
		msg.tiltY_,
		calcItemNetWeight,
		calcItemTotalPrice,
		calcSavings );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void DiagnosticsManager::handleMessage( PrinterHeadDotStatus &msg )
{
	TRACE( "Have printer head dot status " + util_string::longtostring( msg.getPrinterType() ) );

	// retrieve counts
	unsigned int counts( 0 );

	if ( FirstPrinter == msg.getPrinterType() )
		counts = printer1Counts_;
	else if ( SecondPrinter == msg.getPrinterType() )
		counts = printer2Counts_;
	else if ( ThirdPrinter == msg.getPrinterType() )
		counts = printer3Counts_;
	else
	{
		return;
	}

	// create event
	const int printerNum( msg.getPrinterType() + 1 );
	const DotWearHeadStatus dotWearStatus( (DotWearHeadStatus) msg.getStatus() );

	db_->createHeadStatusEvent( printerNum, counts, dotWearStatus );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void DiagnosticsManager::handleMessage( PrinterHeadWear &msg )
{
	TRACE( "Have printer head wear " + util_string::longtostring( msg.getPrinterType() ) );

	std::vector<int> status;
	std::vector<int> value;

	for ( int i( 0 ); i < msg.getHeadSize(); ++i )
	{
		status.push_back( msg.getDotStatus( i ) );
		value.push_back( msg.getDotCounts( i ) );
	}

	// update dot wear table
	const int printerNum( msg.getPrinterType() + 1 );

	db_->updateDotWear( printerNum, status, value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void DiagnosticsManager::handleMessage( PrinterLabelReady &msg )
{
	TRACE( "Have printer label ready " + util_string::longtostring( msg.getPrinterType() ) );

	switch ( msg.getPrinterType() )
	{
	case FirstPrinter:
		edge_->setPrinterCounts( 0, ++printer1Counts_ );
		break;
	case SecondPrinter:
		edge_->setPrinterCounts( 1, ++printer2Counts_ );
		break;
	case ThirdPrinter:
		edge_->setPrinterCounts( 2, ++printer3Counts_ );
		break;
	default:
		break;
	}

	// DiagnosticsManager.handleMessage<PrinterLabelReady>
	TRACEFUNC( "PriPrinter Counts: " + util_string::longtostring( printer1Counts_ ) );
	TRACEFUNC( "SecPrinter Counts: " + util_string::longtostring( printer2Counts_ ) );
	TRACEFUNC( "TerPrinter Counts: " + util_string::longtostring( printer3Counts_ ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void DiagnosticsManager::handleMessage( PrinterStatus &msg )
{
	TRACE( "Have printer status " + util_string::longtostring( msg.getPrinterType() ) );

	int deviceExt( msg.getPrinterType() + 1 );
	unsigned int counts( 0 );

	PrinterStatus prev;

	// save status
	if ( FirstPrinter == msg.getPrinterType() )
	{
		counts = printer1Counts_;

		prev = lastPrinter1Status_;
		lastPrinter1Status_ = msg;
	}
	else if ( SecondPrinter == msg.getPrinterType() )
	{
		counts = printer2Counts_;

		prev = lastPrinter2Status_;
		lastPrinter2Status_ = msg;
	}
	else if ( ThirdPrinter == msg.getPrinterType() )
	{
		counts = printer3Counts_;

		prev = lastPrinter3Status_;
		lastPrinter3Status_ = msg;
	}
	else
	{
		return;
	}

	// nothing to do
	if ( msg == prev )
		return;

	typedef std::pair<int, int> deviceErrorCodeType;
	typedef std::list<deviceErrorCodeType> deviceErrorList;
	deviceErrorList errors;

	// handle message
	if (( !msg.isPeripheralComm() ) && ( prev.isPeripheralComm() ))
	{
		TRACE( "Printer unresponsive" );
		errors.push_back( deviceErrorCodeType( PrinterSystem, 17 ) );
	}
	else if (( msg.isManagerDeleted() ) && ( !prev.isManagerDeleted() ))
	{
		TRACE( "Printer comm lost!" );
		errors.push_back( deviceErrorCodeType( PrinterSystem, 18 ) );
	}

	else
	{
		// priority errors
		if (( msg.isHeadDisengaged() ) && ( !prev.isHeadDisengaged() ))
			errors.push_back( deviceErrorCodeType( Printer, 101 ) );
		if (( msg.isThermalOverload() ) && ( !prev.isThermalOverload() ))
			errors.push_back( deviceErrorCodeType( Printer, 102 ) );
		if (( msg.isThermalShutdown() ) && ( !prev.isThermalShutdown() ))
			errors.push_back( deviceErrorCodeType( Printer, 103 ) );
		if (( msg.isMediaJammed() ) && ( !prev.isMediaJammed() ))
			errors.push_back( deviceErrorCodeType( Printer, 104 ) );
		if (( msg.isMediaShutdown() ) && ( !prev.isMediaShutdown() ))
			errors.push_back( deviceErrorCodeType( Printer, 105 ) );
		if (( msg.isOutOfMedia() ) && ( !prev.isOutOfMedia() ))
			errors.push_back( deviceErrorCodeType( Printer, 106 ) );
		if (( msg.isHardwareError() ) && ( !prev.isHardwareError() ))
			errors.push_back( deviceErrorCodeType( Printer, 107 ) );
		if (( msg.isCassetteMissing() ) && ( !prev.isCassetteMissing() ))
			errors.push_back( deviceErrorCodeType( Printer, 108 ) );

		// labeling errors
		if (( msg.isLabelMissing() ) && ( !prev.isLabelMissing() ))
			errors.push_back( deviceErrorCodeType( Printer, 201 ) );
		if (( msg.isLabelSizingFailed() ) && ( !prev.isLabelSizingFailed() ))
			errors.push_back( deviceErrorCodeType( Printer, 202 ) );
		if (( msg.isLabelTakenError() ) && ( !prev.isLabelTakenError() ))
			errors.push_back( deviceErrorCodeType( Printer, 203 ) );
		if (( msg.isTakeLabelError() ) && ( !prev.isTakeLabelError() ))
			errors.push_back( deviceErrorCodeType( Printer, 204 ) );
	}

	// generate events
	for ( deviceErrorList::const_iterator i( errors.begin() ); i != errors.end(); ++i )
	{
		TRACE( "Have printer error:\n"
			"    source:      " + util_string::longtostring( deviceExt ) + "\n"
			"    error:       " + util_string::longtostring( i->second ) );

		// generate event
		db_->createEvent( i->first, deviceExt, counts, i->second, false );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void DiagnosticsManager::handleMessage( PrinterSystemStatus &msg )
{
	TRACE( "Have printer system status " + util_string::longtostring( msg.getPrinterType() ) );

	if ( msg.getPrinterType() < 0 )
		return;
	else if ( 3 <= msg.getPrinterType() )
		return;

	// update version info
	HobVersion softwareVersion( msg.getSoftwareVersion() );
	HobVersion hardwareVersion( msg.getHardwareVersion() );

	std::string software( softwareVersion.getRevMajMinStr() );
	std::string hardware( hardwareVersion.getRevMajMinStr() );
	std::string checksum;

	const SystemStatusInfo *statusInfo( msg.getSystemStatusInfo() );

	if ( statusInfo )
		checksum = util_string::uinttohexstring( statusInfo->getROM_Checksum() );

	TRACE( "printer versions " + util_string::longtostring( msg.getPrinterType() ) + ":\n" +
		"    software: " + software + "\n" +
		"    hardware: " + hardware + "\n" +
		"    checksum: " + checksum );

	edge_->setPrinterSoftwareVersion( msg.getPrinterType(), software );
	edge_->setPrinterHardwareVersion( msg.getPrinterType(), hardware );
	edge_->setPrinterChecksum( msg.getPrinterType(), checksum );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void DiagnosticsManager::handleMessage( WeigherSystemStatus &msg )
{
	TRACE( "Have weigher system status" );

	std::string checksum;

	const SystemStatusInfo *statusInfo( msg.getSystemStatusInfo() );

	if ( statusInfo )
		checksum = util_string::uinttohexstring( statusInfo->getROM_Checksum() );

	edge_->setWeigherChecksum( checksum );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void DiagnosticsManager::handleMessage( UsbWeigherSystemStatus &msg )
{
	TRACE( "Have usb weigher system status" );

	// update version info
	HobVersion softwareVersion( msg.getSoftwareVersion() );
	HobVersion hardwareVersion( msg.getHardwareVersion() );

	std::string software( softwareVersion.getRevMajMinStr() );
	std::string hardware( hardwareVersion.getRevMajMinStr() );

	TRACE( "weigher versions:\n"
		"    software: " + software + "\n"
		"    hardware: " + hardware );

	edge_->setWeigherSoftwareVersion( software );
	edge_->setWeigherHardwareVersion( hardware );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void DiagnosticsManager::handleMessage( EvolWrapperDiagnosticsTestResult &msg )
{
	TRACE( "Have test result:\n"
		"                  test: " + util_string::longtostring( msg.getTest() ) + "\n"
		"            testResult: " + util_string::longtostring( msg.getTestResult() ) + "\n"
		"      openLoopOverride: " + util_string::longtostring( msg.getOpenLoopOverride() ) + "\n"
		"                  diag: " + util_string::longtostring( msg.getDiag() ) + "\n"
		"    diagExpectedResult: " + util_string::longtostring( msg.getDiagExpectedResult() ) + "\n"
		"      diagActualResult: " + util_string::longtostring( msg.getDiagActualResult() ) + "\n"
		"             errorCode: " + util_string::longtostring( msg.getErrorCode() ) );

	DiagnosticsDB::diagStepTestCode testCode( DiagnosticsDB::Fail );

	if ( msg.getTestResult() )
		testCode = DiagnosticsDB::Pass;

	// finish test
	if ( !db_->finishDiagWizardEventTest( current_, testCode, msg.getErrorCode(), msg.getDiag(), msg.getDiagExpectedResult(), msg.getDiagActualResult() ) )
		TRACE( "failed to finish diag wizard event test" );
	else
	{
		// send diag event
		int method( 0 );

		db_->lookupDiagWizardMethod( current_, method );

		edge_->sendDiagnostics(
			msg.getTest(),
			msg.getTestResult() ? DiagnosticsEdgeDevice::Pass : DiagnosticsEdgeDevice::Fail,
			msg.getErrorCode(),
			msg.getDiag(),
			msg.getDiagExpectedResult(),
			msg.getDiagActualResult(),
			(3 == method),
			(4 == method) );

		// update health info
		util::DictionaryList dict;

		db_->retrieveMaintLogRecords( 0, db_->housekeepingDays(), dict );
		dict.sortBySeverity();

		edge_->setHealthInfo( dict );
	}

	setWizardState( current_, WizardReady );

	// post result
	// note that for wrapper, open loop override is considered passed!
	DiagnosticsPerformWizardTesting resp( this );
	resp.diagEvent_ = current_;
	resp.passed_ = (msg.getTestResult() || msg.getOpenLoopOverride());
	resp.errorCode_ = msg.getErrorCode();

	// make sure there is an error code filled out if test did not pass
	if (( !resp.passed_ ) && ( !resp.errorCode_ ))
		resp.errorCode_ = msg.getTest();

	// go!
	dispatchEvent( &resp );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void DiagnosticsManager::handleMessage( EvolWrapperErrorHistory &msg )
{
	const int device( Wrapper );
	const int deviceExt( 0 );
	const int errorCode( msg.getWrapperError() + EvolWrapperErrorInfo::EvolWrapperCommLost );
	const bool forceWizard( false );

	TRACE( "Have error history:\n"
		"    error: " + util_string::longtostring( errorCode ) + "\n"
		"     tray: " + msg.getTrayName() );

	// track error
	wrapperErrorHistory_.push_back( errorCode );
	wrapperErrorHistory_.unique();

	// generate event
	db_->createEvent( device, deviceExt, wrapperCycleCounter_, errorCode, forceWizard );

	// update health info
	unsigned int severity;

	if ( (severity = db_->lookupEventSeverity( device, 0, errorCode )) )
	{
		util::DictionaryList dict;

		db_->retrieveMaintLogRecords( 0, db_->housekeepingDays(), dict );
		dict.sortBySeverity();

		edge_->setHealthInfo( dict );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void DiagnosticsManager::handleMessage( EvolWrapperEventCapture &msg )
{
	switch ( msg.getEventCaptureType() )
	{
	case UpperFilmRollEncoderReadingEvent:
		TRACE( "Upper film roll encoder: " + util_string::longtostring( msg.getValue() ) );
		break;
	case LowerFilmRollEncoderReadingEvent:
		TRACE( "Lower film roll encoder: " + util_string::longtostring( msg.getValue() ) );
		break;
	default:
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void DiagnosticsManager::handleMessage( EvolWrapperInquire &msg )
{
	EvolWrapperGeneralConfig &generalConfig( msg.getWrapperConfig() );

	WrapperFilmWidthDimensions &upperFilm( generalConfig.getTopFilmRollDimensions() );
	WrapperFilmWidthDimensions &lowerFilm( generalConfig.getBottomFilmRollDimensions() );

	TRACE(
		"upper film settings:\n"
		"       roll width: " + util_string::longtostring( upperFilm.getRollWidth() ) + "\n"
		"    roll diameter: " + util_string::longtostring( upperFilm.getRollDiameter() ) + "\n"
		"        remaining: " + util_string::longtostring( upperFilm.getRemainingOnRoll() ) );

	TRACE(
		"lower film settings:\n"
		"       roll width: " + util_string::longtostring( lowerFilm.getRollWidth() ) + "\n"
		"    roll diameter: " + util_string::longtostring( lowerFilm.getRollDiameter() ) + "\n"
		"        remaining: " + util_string::longtostring( lowerFilm.getRemainingOnRoll() ) );

	if ( !db_->updateFilmStatus( upperFilm.getRollWidth(), lowerFilm.getRollWidth() ) )
		TRACE( "Error updating film status" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void DiagnosticsManager::handleMessage( EvolWrapperResetDefaultPackageTimings &msg )
{
	TRACE( "Reset to default package timings!" );

	if ( !db_->clearAllWrapperBoxes() )
		TRACE( "Error clearing wrapper boxes" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void DiagnosticsManager::handleMessage( EvolWrapperPackageTimingsPointer &msg )
{
	const bool tracing( ISTRACING );
	std::string logline;

	EvolWrapperPackageTimings *ptimings( msg.getTimingsPointer() );

	if ( !ptimings )
	{
		TRACE( "Timings pointer is NULL" );
		return;
	}

	TRACE( "Have package timings:\n"
		"    num wrap boxes: " + util_string::longtostring( ptimings->getNumberWrapBoxes() ) + "\n"
		"       disposition: " + util_string::longtostring( ptimings->getDisposition() ) );

	if ( WRITE_PERMANENT != ptimings->getDisposition() )
		return;

	// BEGIN UPDATE
	if ( !db_->updateWrapperBoxBegin() )
		return;

	EvolWrapperPackageTimings timings( *ptimings );

	// iterate each tray
	for ( size_t trayIndex( timings.getNumberWrapBoxes() ); trayIndex--; )
	{
		const WrapBox *tray( timings.getWrapBox( (int) trayIndex ) );

		if ( !tray )
		{
			TRACE( "Tray is NULL" );
			continue;
		}

		if ( tracing )
		{
			logline = "\n"
				"TRAY '" + tray->getName() + "'\n"
				"    plu exception: " + util_string::longtostring( tray->getPLUException() ) + "\n"
				"       length min: " + util_string::longtostring( tray->getLengthLow() ) + "\n"
				"       length max: " + util_string::longtostring( tray->getLengthHigh() ) + "\n"
				"        width min: " + util_string::longtostring( tray->getWidthLow() ) + "\n"
				"        width max: " + util_string::longtostring( tray->getWidthHigh() ) + "\n"
				"       lip height: " + util_string::longtostring( tray->getLip() ) + "\n";
		}

		DiagnosticsDB::timingsMap heightBoxParam;

		// iterate each height box
		for ( WrapBox::const_iterator box( tray->begin() ); box != tray->end(); ++box )
		{
			if ( tracing )
			{
				logline += "\n"
					"  HEIGHT " + util_string::longtostring( box->getHeight() ) + ":\n";
			}

			DiagnosticsDB::timingsParamList params;

			// iterate each timing parameter
			for ( timingParametersMap::const_iterator param( timingParameters_.begin() ); param != timingParameters_.end(); ++param )
			{
				const unsigned int p( box->getParameter( (int) param->first ) );

				if ( tracing )
					logline += "      " + param->second + ": " + util_string::longtostring( p ) + "\n";

				params.push_back( DiagnosticsDB::timingsParam( (unsigned int) param->first, p ) );
			}

			heightBoxParam[ box->getHeight() ] = params;
		}

		TRACE( logline );

		db_->updateWrapperBox(
			tray->getName(),
			tray->getPLUException(),
			tray->getLengthLow(),
			tray->getLengthHigh(),
			tray->getWidthLow(),
			tray->getWidthHigh(),
			tray->getLip(),
			heightBoxParam );
	}

	// END UPDATE
	if ( !db_->updateWrapperBoxEnd() )
		TRACE( "Error saving wrapper boxes!" );

}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void DiagnosticsManager::handleMessage( EvolWrapperSettings &msg )
{
	const EvolWrapperTemperatureInfo &tempInfo( msg.getTemperatureInfo() );

	edge_->setWrapperFilmState( msg.getUpperRollWidth(), 0, msg.getLowerRollWidth(), 0 );
	edge_->setWrapperBeltState( tempInfo.getSealerBeltHeaterEnabled(), tempInfo.getSealerBeltSetting(), tempInfo.getSealerBeltTemp() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void DiagnosticsManager::handleMessage( EvolWrapperStatus &msg )
{
	EvolWrapperErrorInfo errorInfo( msg.getCurrErrorInfo() );

	// wrapper seems to send status updates about every 5s... and most of the time the error info
	// is completely unchanged.  so lets only check for errors when something changes.
	bool errorInfoChanged( !errorInfo.equals( lastWrapperStatus_.getCurrErrorInfo() ) );

	int device;
	int deviceExt;
	int errorCode;
	bool forceWizard( false );

	// save status
	lastWrapperStatus_ = msg;

	// update state
	updateWizardState();

	// handle message
	if ( !msg.isPeripheralComm() )
	{
		TRACE( "Wrapper unresponsive" );

		device = WrapperSystem;
		deviceExt = 0;
		errorCode = 17;
	}
	else if ( msg.isManagerDeleted() )
	{
		TRACE( "Wrapper comm lost!" );

		device = WrapperSystem;
		deviceExt = 0;
		errorCode = 18;
	}
	else if (( errorInfo.isEvolWrapperError() ) && ( errorInfoChanged ))
	{
		device = Wrapper;
		deviceExt = errorInfo.getEvolWrapperErrorSource();
		errorCode = errorInfo.getEvolWrapperError( (EvolWrapperErrorInfo::EvolWrapperErrorSource) deviceExt );
		forceWizard = errorInfo.getDiagnosticsSupport();

		TRACE( "Have wrapper error:\n"
			"    source:      " + util_string::longtostring( deviceExt ) + "\n"
			"    error:       " + util_string::longtostring( errorCode ) + "\n"
			"    diagnostics: " + util_string::longtostring( forceWizard ) );

		// validate error
		if ( EvolWrapperErrorInfo::NoWrapperError == errorCode )
			return;

		// track error
		wrapperStatus_.push_back( errorCode );
		wrapperStatus_.unique();

		// ignore when already handled by error history message
		if ( wrapperErrorHistory_.end() != std::find( wrapperErrorHistory_.begin(), wrapperErrorHistory_.end(), errorCode ) )
		{
			TRACE( "event already handled (ignored)" );
			return;
		}
	}
	else
	{
		// unhandled
		return;
	}

	// generate event
	db_->createEvent( device, deviceExt, wrapperCycleCounter_, errorCode, forceWizard );

	// update health info
	unsigned int severity;

	if ( (severity = db_->lookupEventSeverity( device, 0, errorCode )) )
	{
		util::DictionaryList dict;

		db_->retrieveMaintLogRecords( 0, db_->housekeepingDays(), dict );
		dict.sortBySeverity();

		edge_->setHealthInfo( dict );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void DiagnosticsManager::handleMessage( EvolWrapperSystemInfo &msg )
{
	// DiagnosticsManager.handleMessage<EvolWrapperSystemInfo>
	TRACEFUNC( "Have wrapper system info:\n" +
		util_string::ulongtostring( msg.getModelNumber() ) + " " +
		msg.getAwsSerialNumber() + " " +
		util_string::ulongtostring( msg.getAwsCycleCount() ) );

	// save system info
	lastWrapperSystemInfo_ = msg;

	// save cycle counter
	wrapperCycleCounter_ = msg.getAwsCycleCount();

	edge_->setWrapperCycleCount( msg.getAwsCycleCount() );
	edge_->setWrapperInfo( msg.getAwsModelNumber(), msg.getAwsSerialNumber() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void DiagnosticsManager::handleMessage( EvolWrapperSystemStatus &msg )
{
	TRACE( "Have wrapper system status" );

	if ( !msg.getNodeCount() )
		return;

	// build list of node versions
	std::vector<std::string> nodeAppVersions;
	nodeAppVersions.resize( msg.getNodeCount() );

	std::vector<std::string> nodeDataVersions;
	nodeDataVersions.resize( msg.getNodeCount() );

	std::vector<std::string> nodeBootVersions;
	nodeBootVersions.resize( msg.getNodeCount() );

	std::vector<std::string> nodeChecksums;
	nodeChecksums.resize( msg.getNodeCount() );

	for ( unsigned int i( 0 ); i < msg.getNodeCount(); ++i )
	{
		const EvolWrapperNode &node( msg.getNode( i ) );

		const EvolWrapperNode::EvolWrapperVersions nodeAppVersion( node.getAppFlash() );
		const EvolWrapperNode::EvolWrapperVersions nodeDataVersion( node.getDataFlash() );
		const EvolWrapperNode::EvolWrapperVersions nodeBootVersion( node.getBootFlash() );
		const uint32_t nodeChecksum( node.getWrapBoxChecksum() );

		const std::string appVersion(
			util_string::longtostring( nodeAppVersion.getMajorRevision() ) + "." +
			util_string::longtostring( nodeAppVersion.getMinorRevision() ) );

		const std::string dataVersion(
			util_string::longtostring( nodeDataVersion.getMajorRevision() ) + "." +
			util_string::longtostring( nodeDataVersion.getMinorRevision() ) );

		const std::string bootVersion(
			util_string::longtostring( nodeBootVersion.getMajorRevision() ) + "." +
			util_string::longtostring( nodeBootVersion.getMinorRevision() ) );

		const std::string checksum(
			util_string::uinttohexstring( nodeChecksum ) );

		TRACE( "wrapper node " + util_string::longtostring( node.getNodeId() ) + " versions\n"
			"    app: " + appVersion + "\n"
			"    data: " + dataVersion + "\n"
			"    boot: " + bootVersion + "\n"
			"    checksum: " + checksum );

		if ( node.getNodeId() < msg.getNodeCount() )
		{
			nodeAppVersions[node.getNodeId()] = appVersion;
			nodeDataVersions[node.getNodeId()] = dataVersion;
			nodeBootVersions[node.getNodeId()] = bootVersion;
			nodeChecksums[node.getNodeId()] = checksum;
		}
	}

	edge_->setWrapperNodeVersions( nodeAppVersions, nodeDataVersions, nodeBootVersions, nodeChecksums );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
void DiagnosticsManager::handleMessage( EvolWrapperWrap &msg )
{
	TRACE( "Have wrapper wrap" );

	edge_->setWrapperCycleCount( ++wrapperCycleCounter_ );

	// DiagnosticsManager.handleMessage<EvolWrapperWrap>
	TRACEFUNC( "Wrapper Cycles: " + util_string::longtostring( wrapperCycleCounter_ ) );

	// resolve old maintenance records
	int resolved( 0 );

	if ( !db_->resolveMaintLog( wrapperCycleCounter_, resolved ) )
		TRACE( "failed to resolve maint log records!" );
	else if ( resolved )
	{
		util::DictionaryList dict;
		util::DictionaryList dictResolved;

		// pull open and resolved records
		db_->retrieveMaintLogRecords( 0, db_->housekeepingDays(), dict );
		dict.sortBySeverity();

		db_->retrieveMaintLogRecords( 1, 1, dictResolved );

		// some data massaging is needed here... first we must update health info with the
		// resolved record then we must remove it
		if ( dictResolved.size() )
		{
			util::DictionaryList dictPrev( dict );

			// attached resolved maint records
			for ( util::DictionaryList::const_iterator i( dictResolved.begin() ); i != dictResolved.end(); ++i )
			{
				dictPrev.push_back( *i );

				if ( !(--resolved) )
					break;
			}

			dictPrev.sortBySeverity();

			edge_->setHealthInfo( dictPrev );
		}

		// update health info
		edge_->setHealthInfo( dict );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
void DiagnosticsManager::handleMessageT( const char *data )
{
	if (( db_ ) && ( db_->isOpen() ))
	{
		// acquire database
		m_.acquire();

		// handle message
		if ( DiagRunning == state_ )
		{
			// deserialize message
			T m( data );

			handleMessage( m );
		}

		// release database
		m_.release();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DiagnosticsManager::processEvent( Util::CSmartPtr<EvolMessageWrapper> &msg )
{
    if ( !msg )
    	return;

    const int device( msg->getDeviceType() );
    const int event( msg->getEventType() );

	TRACE( "process event " + util_string::longtostring( device ) + " " + util_string::longtostring( event ) );

    // handle message
	if ( Diagnostics == device )
	{
		if ( DiagConfigurationReq == event )
			handleMessageT<DiagnosticsConfigurationReq>( msg->Data() );
		else if ( DiagConfiguration == event )
			handleMessageT<DiagnosticsConfiguration>( msg->Data() );

		else if ( DiagStatusReq == event )
			handleMessageT<DiagnosticsStatusReq>( msg->Data() );

		else if ( DiagAuditLogReq == event )
			handleMessageT<DiagnosticsAuditLogReq>( msg->Data() );
		else if ( DiagAuditLogRecordReq == event )
			handleMessageT<DiagnosticsAuditLogRecordReq>( msg->Data() );

		else if ( DiagAuditLogServiceVisitStart == event )
			handleMessageT<DiagnosticsAuditLogServiceVisitStart>( msg->Data() );
		else if ( DiagAuditLogServiceVisitStop == event )
			handleMessageT<DiagnosticsAuditLogServiceVisitStop>( msg->Data() );
		else if ( DiagAuditLogServiceVisitStatus == event )
			handleMessageT<DiagnosticsAuditLogServiceVisitStatus>( msg->Data() );

		else if ( DiagCheckErrorHandlerReq == event )
			handleMessageT<DiagnosticsCheckErrorHandlerReq>( msg->Data() );
		else if ( DiagStartErrorHandler == event )
			handleMessageT<DiagnosticsStartErrorHandler>( msg->Data() );
		else if ( DiagStartErrorHandlerFromMaintRecord == event )
			handleMessageT<DiagnosticsStartErrorHandlerFromMaintRecord>( msg->Data() );

		else if ( DiagInitWizardReq == event )
			handleMessageT<DiagnosticsInitWizardReq>( msg->Data() );
		else if ( DiagPerformWizardTestingReq == event )
			handleMessageT<DiagnosticsPerformWizardTestingReq>( msg->Data() );
		else if ( DiagExitWizard == event )
			handleMessageT<DiagnosticsExitWizard>( msg->Data() );

		else if ( DiagServiceMaintLogReq == event )
			handleMessageT<DiagnosticsServiceMaintLogReq>( msg->Data() );
		else if ( DiagClearServiceMaintRecord == event )
			handleMessageT<DiagnosticsClearServiceMaintRecord>( msg->Data() );
		else if ( DiagPrintServiceMaintLog == event )
			handleMessageT<DiagnosticsPrintServiceMaintLog>( msg->Data() );

		else if ( DiagServiceActionsReq == event )
			handleMessageT<DiagnosticsServiceActionsReq>( msg->Data() );

		else if ( DiagExportDatabase == event )
			handleMessageT<DiagnosticsExportDatabase>( msg->Data() );
		else if ( DiagSendDatabaseEmail == event )
			handleMessageT<DiagnosticsSendDatabaseEmail>( msg->Data() );
		else if ( DiagSendDatabaseGatewayDevice == event )
			handleMessageT<DiagnosticsSendDatabaseGatewayDevice>( msg->Data() );

		else if ( DiagPrintLabelStats == event )
			handleMessageT<DiagnosticsPrintLabelStats>( msg->Data() );

		else if ( DiagValueMax == event )
			handleMessageT<DiagnosticsValueMax>( msg->Data() );
	}
	else if ( LabelApplier == device )
    {
    	// TODO
    }
	else if ( LabelApplierSystem == device )
    {
    	// TODO
    }
    else if ( Printer == device )
    {
    	if ( PrHeadDotStatus == event )
    		handleMessageT<PrinterHeadDotStatus>( msg->Data() );
    	else if ( PrHeadDotWear == event )
    		handleMessageT<PrinterHeadWear>( msg->Data() );
    	else if ( PrLabelReady == event )
    		handleMessageT<PrinterLabelReady>( msg->Data() );
    	else if ( PrStatus == event )
    		handleMessageT<PrinterStatus>( msg->Data() );
    }
    else if ( PrinterSystem == device )
    {
    	if ( SysStatus == event )
    		handleMessageT<PrinterSystemStatus>( msg->Data() );
    }
    else if ( Weigher == device )
    {
    	// TODO
    }
    else if ( WeigherSystem == device )
    {
		if ( SysStatus == event )
			handleMessageT<WeigherSystemStatus>( msg->Data() );
		else if ( SysUsbWgStatus == event )
    		handleMessageT<UsbWeigherSystemStatus>( msg->Data() );
    }
    else if ( Wrapper == device )
    {
    	if ( WrEvolDiagnosticsTestResult == event )
    		handleMessageT<EvolWrapperDiagnosticsTestResult>( msg->Data() );
    	else if ( WrEvolErrorHistory == event )
    		handleMessageT<EvolWrapperErrorHistory>( msg->Data() );
    	else if ( WrEvolEventCapture == event )
    		handleMessageT<EvolWrapperEventCapture>( msg->Data() );
    	else if ( WrEvolInquire == event )
    		handleMessageT<EvolWrapperInquire>( msg->Data() );
    	else if ( WrEvolResetDefaultPackageTimings == event )
    		handleMessageT<EvolWrapperResetDefaultPackageTimings>( msg->Data() );
    	else if ( WrEvolSavePackageTimings == event )
    		handleMessageT<EvolWrapperPackageTimingsPointer>( msg->Data() );
    	else if ( WrEvolSettings == event )
    		handleMessageT<EvolWrapperSettings>( msg->Data() );
    	else if ( WrEvolStatus == event )
    		handleMessageT<EvolWrapperStatus>( msg->Data() );
    	else if ( WrEvolSystemInfo == event )
    		handleMessageT<EvolWrapperSystemInfo>( msg->Data() );
    	else if ( WrEvolWrap == event )
    		handleMessageT<EvolWrapperWrap>( msg->Data() );
    }
    else if ( WrapperSystem == device )
    {
    	if ( SysUsbWrStatus == event )
    		handleMessageT<EvolWrapperSystemStatus>( msg->Data() );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DiagnosticsManager::finalize()
{
	if ( instance_ )
	{
		instanceMutex_.acquire();

		if ( instance_ )
		{
			delete instance_;
			instance_ = NULL;
		}

		instanceMutex_.release();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
DiagnosticsManager *DiagnosticsManager::instance()
{
	if ( !instance_ )
	{
		instanceMutex_.acquire();

		if ( !instance_ )
			instance_ = new _Myt();

		instanceMutex_.release();
	}

	return instance_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DiagnosticsManager::setState( State value )
{
	m_.acquire();

	// verify state change
	if ( state_ != value )
	{
		bool valid( false );

		switch ( state_ )
		{
		case DiagStopped:

			// start thread
			if ( (valid = (DiagStart == value)) )
				thread_ = new Util::CTLSThread( *this, THREAD_PRIORITY_NORMAL, 0, "DiagnosticsManager's Thread" );

			break;

		case DiagStart:
			valid = (DiagStarting == value);
			break;
		case DiagStarting:
			valid = (DiagRunning == value);
			break;
		case DiagRunning:
			valid = (DiagStop == value);
			break;
		case DiagStop:
			valid = (DiagStopping == value);
			break;
		case DiagStopping:
			valid = (DiagStopped == value);
			break;
		}

		// update state
		if ( valid )
		{
			TRACE( "New state is: " + util_string::longtostring( value ) );
			state_ = value;
		}
	}

	m_.release();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DiagnosticsManager::initialize()
{
	// register events
	registerEvent( Diagnostics, DiagConfigurationReq );
	registerEvent( Diagnostics, DiagConfiguration );

	registerEvent( Diagnostics, DiagStatusReq );

	registerEvent( Diagnostics, DiagAuditLogReq );
	registerEvent( Diagnostics, DiagAuditLogRecordReq );

	registerEvent( Diagnostics, DiagAuditLogServiceVisitStart );
	registerEvent( Diagnostics, DiagAuditLogServiceVisitStop );
	registerEvent( Diagnostics, DiagAuditLogServiceVisitStatus );

	registerEvent( Diagnostics, DiagCheckErrorHandlerReq );
	registerEvent( Diagnostics, DiagStartErrorHandler );
	registerEvent( Diagnostics, DiagStartErrorHandlerFromMaintRecord );

	registerEvent( Diagnostics, DiagInitWizardReq );
	registerEvent( Diagnostics, DiagPerformWizardTestingReq );
	registerEvent( Diagnostics, DiagExitWizard );

	registerEvent( Diagnostics, DiagServiceMaintLogReq );
	registerEvent( Diagnostics, DiagClearServiceMaintRecord );
	registerEvent( Diagnostics, DiagPrintServiceMaintLog );

	registerEvent( Diagnostics, DiagServiceActionsReq );

	registerEvent( Diagnostics, DiagExportDatabase );

	registerEvent( Diagnostics, DiagSendDatabaseEmail );
	registerEvent( Diagnostics, DiagSendDatabaseGatewayDevice );

	registerEvent( Diagnostics, DiagPrintLabelStats );

	registerEvent( Diagnostics, DiagValueMax );

	registerEvent( Printer, PrHeadDotStatus );
	registerEvent( Printer, PrHeadDotWear );
	registerEvent( Printer, PrLabelReady );
	registerEvent( Printer, PrStatus );

	registerEvent( PrinterSystem, SysStatus );

	registerEvent( WeigherSystem, SysStatus );
	registerEvent( WeigherSystem, SysUsbWgStatus );

	registerEvent( Wrapper, WrEvolDiagnosticsTestResult );
	registerEvent( Wrapper, WrEvolErrorHistory );
	registerEvent( Wrapper, WrEvolEventCapture );
	registerEvent( Wrapper, WrEvolInquire );
	registerEvent( Wrapper, WrEvolResetDefaultPackageTimings );
	registerEvent( Wrapper, WrEvolSavePackageTimings );
	registerEvent( Wrapper, WrEvolSettings );
	registerEvent( Wrapper, WrEvolStatus );
	registerEvent( Wrapper, WrEvolSystemInfo );
	registerEvent( Wrapper, WrEvolWrap );

	registerEvent( WrapperSystem, SysUsbWrStatus );

	// setup wrapper timing parameters
	timingParameters_[0]  = "             percent stretch";
	timingParameters_[1]  = "           knife fire height";
	timingParameters_[2]  = "           clamp open height";
	timingParameters_[3]  = "          elevator placement";
	timingParameters_[4]  = "             add tray length";
	timingParameters_[5]  = "     gripper pull adjustment";
	timingParameters_[6]  = "front underfolder adjustment";
	timingParameters_[7]  = "                  wrap speed";
	timingParameters_[8]  = "             discharge speed";
	timingParameters_[9]  = " side underfolder adjustment";
	timingParameters_[10] = "          height restriction";
	timingParameters_[11] = "side underfolder return home";
	timingParameters_[12] = "    gripper allow short pull";
	timingParameters_[13] = "              side clamp gap";
	timingParameters_[14] = "            film roll to use";
	timingParameters_[15] = " rear underfolder trip delay";
	timingParameters_[16] = "                brake timing";
	timingParameters_[17] = "     outside gripper release";
	timingParameters_[18] = "      inside gripper release";


	// start!
	setState( DiagStart );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DiagnosticsManager::setWizardStateLabelApplier( WizardState value )
{
	// TODO
	wizardState_ = requestedWizardState_ = value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DiagnosticsManager::setWizardStatePrinter( WizardState value )
{
	// TODO
	wizardState_ = requestedWizardState_ = value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DiagnosticsManager::setWizardStateWeigher( WizardState value )
{
	// TODO
	wizardState_ = requestedWizardState_ = value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DiagnosticsManager::setWizardStateWrapper( WizardState value )
{
	// supported states:
	// initializing
	// ready
	// wrapper need power
	// testing
	// idle

	// switching to ready state
	if ( WizardReady == value )
		if ( wizardState_ != (requestedWizardState_ = WizardReady) )
		{
			// check mode
			if ( WrServiceMode == lastWrapperStatus_.getWrapperMode() )
			{
				// send update to UI
				if (( WizardIdle == wizardState_ ) || ( WizardInitializing == wizardState_ ))
				{
					DiagnosticsInitWizard resp( this );
					resp.diagEvent_ = current_;
					resp.ready_ = true;

					// go!
					const char *raw( resp.serialize() );
					dispatchEvent( raw,  resp.getSerializedLength() );
				}

				// okay, ready
				wizardState_ = WizardReady;
			}

			// need to be in service mode
			else
			{
				WrapperMode wm( this, WrServiceMode );

				// go!
				const char *raw( wm.serialize() );
				dispatchEvent( raw,  wm.getSerializedLength() );

				// we are initializing state
				wizardState_ = WizardInitializing;
			}
		}

	// switching to testing state (must be ready first!!)
	if ( WizardTesting == value )
	{
		bool validState(
			( WizardReady == wizardState_ ) ||
			( WizardWrapperNeedPower == wizardState_ ) );

		// check current state valid
		if (( validState ) && ( wizardState_ != (requestedWizardState_ = WizardTesting) ))
		{
			EvolWrapperStatusInfo statusInfo( lastWrapperStatus_.getStatusInfo() );

			// check if high power is on, needs to be on to continue
			if ( !statusInfo.isMainPower() )
			{
				// let operator know
				SessionManagerHelper::getInstance()->addDialog(
					ErrOperationMode,
					ErrOperationMode * DEVICE_ERROR_OFFSET + OperationModeStatus::WrapperPressStart );

				wizardState_ = WizardWrapperNeedPower;
				return;
			}

			// remove message
			else if ( WizardWrapperNeedPower == wizardState_ )
			{
				// remove operator dialog
				SessionManagerHelper::getInstance()->removeDialog(
					ErrOperationMode,
					ErrOperationMode * DEVICE_ERROR_OFFSET + OperationModeStatus::WrapperPressStart );
			}

			// execute test
			if ( !db_->startDiagWizardEventTest( current_ ) )
			{
				TRACE( "failed to start diag wizard event test" );

				wizardState_ = requestedWizardState_ = WizardReady;
			}
			else
			{
				EvolWrapperDiagnosticsTest diag(
					(currentErrorCode_ % 1000) - EvolWrapperErrorInfo::EvolWrapperCommLost,
					this );

				dispatchEvent( &diag );

				// we are testing
				wizardState_ = WizardTesting;
			}
		}
	}

	// switch to idle state
	if ( WizardIdle == value )
		if ( wizardState_ != (requestedWizardState_ = WizardIdle) )
		{
			// fail any test currently in progress
			if ( WizardTesting == wizardState_ )
			{
				db_->finishDiagWizardEventTest( current_, DiagnosticsDB::FailTimeout, currentErrorCode_ );

				// send diag event
				int method( 0 );

				db_->lookupDiagWizardMethod( current_, method );

				edge_->sendDiagnostics(
					currentErrorCode_,
					DiagnosticsEdgeDevice::FailTimeout,
					currentErrorCode_,
					0,
					0,
					0,
					(3 == method),
					(4 == method) );
			}

			// need to be in operate mode
			if ( WrOperateMode != lastWrapperStatus_.getWrapperMode() )
			{
				WrapperMode wm( this, WrOperateMode );

				// go!
				const char *raw( wm.serialize() );
				dispatchEvent( raw,  wm.getSerializedLength() );
			}

			// we are initializing state
			wizardState_ = WizardIdle;
		}

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DiagnosticsManager::setWizardStateWrapperSystem( WizardState value )
{
	// supported states:
	// ready
	// testing
	// wait wrapper online
	// idle

	// switching to ready state
	if ( WizardReady == value )
		if ( wizardState_ != (requestedWizardState_ = WizardReady) )
		{
			// send update to UI
			if ( WizardIdle == wizardState_ )
			{
				DiagnosticsInitWizard resp( this );
				resp.diagEvent_ = current_;
				resp.ready_ = true;

				// go!
				const char *raw( resp.serialize() );
				dispatchEvent( raw,  resp.getSerializedLength() );
			}

			// okay, ready
			wizardState_ = WizardReady;
		}

	// switching to testing state (must be ready first!!)
	if ( WizardTesting == value )
	{
		bool validState( WizardReady == wizardState_ );

		// check current state valid
		if (( validState ) && ( wizardState_ != (requestedWizardState_ = WizardTesting) ))
		{
			// execute test
			if ( !db_->startDiagWizardEventTest( current_ ) )
			{
				TRACE( "failed to start diag wizard event test" );

				wizardState_ = requestedWizardState_ = WizardReady;
			}
			else
			{
				// we are testing
				wizardState_ = WizardTesting;

				// wait for wrapper online
				value = WizardWaitWrapperOnline;
			}
		}
	}

	// waiting for wrapper online
	if ( WizardWaitWrapperOnline == value )
	{
		bool validState( WizardTesting == wizardState_ );

		// check current state valid
		if (( validState ) && ( wizardState_ != (requestedWizardState_ = WizardWaitWrapperOnline) ))
		{
			// check for wrapper online
			if (( lastWrapperStatus_.isPeripheralComm() ) && ( !lastWrapperStatus_.isManagerDeleted() ))
			{
				// pass!
				if ( !db_->finishDiagWizardEventTest( current_, DiagnosticsDB::Pass, 0 ) )
					TRACE( "failed to finish diag wizard event test" );
				else
				{
					// send diag event
					int method( 0 );

					db_->lookupDiagWizardMethod( current_, method );

					edge_->sendDiagnostics(
						currentErrorCode_,
						DiagnosticsEdgeDevice::Pass,
						0,
						0,
						0,
						0,
						(3 == method),
						(4 == method) );

					// post result
					DiagnosticsPerformWizardTesting resp( this );
					resp.diagEvent_ = current_;
					resp.passed_ = true;
					resp.errorCode_ = 0;

					// go!
					dispatchEvent( &resp );
				}

				// set state to ready
				wizardState_ = requestedWizardState_ = WizardReady;
			}
		}
	}

	// switch to idle state
	if ( WizardIdle == value )
	{
		// fail any test currently in progress
		if ( WizardWaitWrapperOnline == requestedWizardState_ )
		{
			db_->finishDiagWizardEventTest( current_, DiagnosticsDB::FailTimeout, currentErrorCode_ );

			// send diag event
			int method( 0 );

			db_->lookupDiagWizardMethod( current_, method );

			edge_->sendDiagnostics(
				currentErrorCode_,
				DiagnosticsEdgeDevice::FailTimeout,
				currentErrorCode_,
				0,
				0,
				0,
				(3 == method),
				(4 == method) );
		}

		wizardState_ = requestedWizardState_ = value;
	}

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DiagnosticsManager::setWizardState( const std::string &diagEvent, WizardState value )
{
	bool okay( true );

	// lookup diagnostic event info
	if ( current_ != diagEvent )
	{
		int device;
		int deviceExt;
		int errorCode;

		// lookup!
		if ( !db_->lookupDeviceInfoFromDiagStep( diagEvent, device, deviceExt, errorCode ) )
		{
			TRACE( "Error looking up device info from diag step " + diagEvent );
			return;
		}

		// finalize current before processing new (only when different device)
		if (( current_.length() ) && ( currentDevice_ != device ))
			setWizardState( current_, WizardIdle );

		current_ = diagEvent;
		currentDevice_ = device;
		currentErrorCode_ = errorCode;
	}

	TRACE( "handle state change for diag event " +
		current_ + " " +
		util_string::longtostring( currentDevice_ ) + " " +
		util_string::longtostring( value ) );

	// handle device
	if ( LabelApplier == currentDevice_ )
    	setWizardStateLabelApplier( value );
    else if ( Printer == currentDevice_ )
    	setWizardStatePrinter( value );
    else if ( Weigher == currentDevice_ )
    	setWizardStateWeigher( value );
    else if ( Wrapper == currentDevice_ )
    	setWizardStateWrapper( value );
    else if ( WrapperSystem == currentDevice_ )
    	setWizardStateWrapperSystem( value );
    else
    {
    	TRACE( "unsupported device for diag wizard!" );
    	okay = false;
    }

	TRACE( "state change for diag event " + diagEvent + " complete:\n"
		"    requested: " + util_string::longtostring( requestedWizardState_ ) + "\n" +
		"       actual: " + util_string::longtostring( wizardState_ ) );

	// when going to idle force fields!
	if (( WizardIdle == value ) || ( !okay ))
	{
		wizardState_ = requestedWizardState_ = WizardIdle;

		current_.clear();
		currentDevice_ = 0;
		currentErrorCode_ = 0;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DiagnosticsManager::updateWizardState()
{
	// nothing to do
	if ( current_.empty() )
		return;
	else if ( wizardState_ == requestedWizardState_ )
		return;

	setWizardState( current_, requestedWizardState_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DiagnosticsManager::openDatabase()
{
	// open
	TRACE( "opening database..." );

	db_ = new DiagnosticsDB( DiagnosticsDB::dbFile() );

	// check open
	if ( !db_->isOpen() )
	{
		TRACE( "failed to open database!" );
		closeDatabase();

		return false;
	}
	else if ( db_->version() <= 0 )
	{
		TRACE( "database has bad version!" );
		closeDatabase();

		return false;
	}

	closeDatabase();

	// backup database
	TRACE( "backing up database..." );

	// remove old backup
	std::remove( DiagnosticsDB::dbBackupFile().c_str() );

	// copy database to backup
	std::ifstream is( DiagnosticsDB::dbFile().c_str(), std::ios::binary );
	std::ofstream os( DiagnosticsDB::dbBackupFile().c_str(), std::ios::binary );

	os << is.rdbuf();

	is.close();
	os.close();

	// open database again
	TRACE( "opening database after backup..." );

	db_ = new DiagnosticsDB( DiagnosticsDB::dbFile() );

	// check open
	if ( !db_->isOpen() )
	{
		TRACE( "failed to open database!" );
		closeDatabase();

		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DiagnosticsManager::restoreDatabase()
{
	struct stat buffer;

	if ( 0 != stat( DiagnosticsDB::dbBackupFile().c_str(), &buffer ) )
	{
		TRACE( "no database to restore!" );
		return false;
	}

	// restore
	TRACE( "restoring database..." );

	// remove database
	std::remove( DiagnosticsDB::dbFile().c_str() );

	// copy backup to database
	std::ifstream is( DiagnosticsDB::dbBackupFile().c_str(), std::ios::binary );
	std::ofstream os( DiagnosticsDB::dbFile().c_str(), std::ios::binary );

	os << is.rdbuf();

	is.close();
	os.close();

	// open database again
	return openDatabase();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DiagnosticsManager::upgradeDatabase()
{
	if ( !db_ )
	{
		TRACE( "no database to upgrade!" );
		return false;
	}
	else if ( db_->isLatestVersion() )
	{
		// nothing to do
		return true;
	}

	closeDatabase();

	// backup database
	TRACE( "upgrading database..." );

	// remove old backup
	std::remove( DiagnosticsDB::dbOldFile().c_str() );

	// move database
	std::rename( DiagnosticsDB::dbFile().c_str(), DiagnosticsDB::dbOldFile().c_str() );

	// upgrade
	if ( !openDatabase() )
	{
		TRACE( "failed to open database for upgrade!" );
		return false;
	}

	// open old database
	DiagnosticsDB dbOld( DiagnosticsDB::dbOldFile() );

	// check open
	if ( !dbOld.isOpen() )
	{
		TRACE( "failed to open old database!" );
		return false;
	}

	// go!
	bool result( db_->upgrade( dbOld ) );

	TRACE( "upgrade result: " + util_string::longtostring( result ) );
	return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DiagnosticsManager::closeDatabase()
{
	if ( db_ )
	{
		TRACE( "closing database..." );

		delete db_;
		db_ = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DiagnosticsManager::deleteDatabase()
{
	closeDatabase();

	TRACE( "deleting database..." );

	// remove database
	std::remove( DiagnosticsDB::dbFile().c_str() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
DWORD DiagnosticsManager::Run()
{
	//
	// startup
	//

	TRACE( "starting up..." );
	setState( DiagStarting );

	TRACE( "db file: " + DiagnosticsDB::dbFile() );
	TRACE( "db backup file: " + DiagnosticsDB::dbBackupFile() );
	TRACE( "db old file: " + DiagnosticsDB::dbOldFile() );

	bool success( false );

	// open and upgrade existing database
	if (( !openDatabase() ) && ( !restoreDatabase() ))
		TRACE( "failed to open or restore database!!" );
	else if ( !upgradeDatabase() )
		TRACE( "failed to upgrade database!!" );
	else
	{
		success = true;
	}

	// generate new database if needed
	if ( !success )
	{
		deleteDatabase();

		if ( !openDatabase() )
			TRACE( "failed to create new database!!" );
		else
		{
			TRACE( "generated new database!!" );
			success = true;
		}
	}

	// otherwise perform housekeeping
	else
	{
		db_->housekeeping();
	}

	// enter main loop
	if ( success )
	{
		const util::Timestamp base( util::Timestamp::now() );

#if defined ( __linux__ )
		// sync disk
		sync();
#endif // __linux__

		// retrieve config
		unsigned int edgeInit( 0 );
		unsigned int edgePowerUpTimer( 0 );
		unsigned int edgeStatusCheckTimer( 0 );
		unsigned int edgeCheckOnlineTimer( 300 );

		db_->edgeDeviceTimers( edgeInit, edgePowerUpTimer, edgeStatusCheckTimer );

		TRACE( "edge device init " + util_string::longtostring( edgeInit ) );
		TRACE( "edge device power up timer " + util_string::longtostring( edgePowerUpTimer ) );
		TRACE( "edge device status check timer " + util_string::longtostring( edgeStatusCheckTimer ) );

		// setup timers
		util::Timestamp edgeDeviceInitTime( base );
		edgeDeviceInitTime.addSeconds( edgeInit );

		util::Timestamp edgeDevicePowerUpTime( base );
		edgeDevicePowerUpTime.addSeconds( edgePowerUpTimer );

		util::Timestamp edgeDevicePingTime( base );
		edgeDevicePingTime.addSeconds( edgePowerUpTimer );

		util::Timestamp edgeCheckOnlineTime( base );
		edgeCheckOnlineTime.addSeconds( edgeCheckOnlineTimer );

		// main loop
		TRACE( "entering main loop..." );
		setState( DiagRunning );

		for ( unsigned int loops( 0 ); !ExitThread(); ++loops )
		{
			// check edge device comms
			edge_->checkComms();

			// only process every 120 loop iterations (i.e. every ~15s)
			if ( loops % 60 )
				continue;

			// force check online
			bool forceCheckOnline( false );

			const util::Timestamp now( util::Timestamp::now() );

			//
			// edge device init
			//
			if (( edgeDeviceInitTime ) && ( edgeDeviceInitTime <= now ))
			{
				// request wrapper info
				WrapperRequestSettings wrapperSettings( this );
				dispatchEvent( &wrapperSettings );

				WrapperSystemRequestStatus wrapperStatus( this );
				dispatchEvent( &wrapperStatus );

				// request weigher info
				WeigherSystemRequestStatus weigherStatus( this );
				dispatchEvent( &weigherStatus );

				// request printer info
				PrinterSystemRequestStatus printerStatus( this );
				dispatchEvent( &printerStatus );

				edgeDeviceInitTime = 0;
			}

			//
			// edge device power up timer
			//
			if (( edgeDevicePowerUpTime ) && ( edgeDevicePowerUpTime <= now ))
			{
				m_.acquire();

				// enable device
				edge_->setEnabled( true );

				if ( !edge_->enabled() )
				{
					TRACE( "edge device is disabled, cannot perform power up message" );

					// extend timer
					edgeDevicePowerUpTime.addSeconds( edgePowerUpTimer );
				}
				else
				{
					util::DictionaryList dict;

					TRACE( "performing power up message on edge device..." );

					// update health info
					db_->retrieveMaintLogRecords( 0, db_->housekeepingDays(), dict );
					dict.sortBySeverity();

					edge_->setHealthInfo( dict );

					// send!
					edge_->sendPowerUp();

					// disable message
					edgeDevicePowerUpTime = 0;

					// reset ping message timer
					edgeDevicePingTime = now;
					edgeDevicePingTime.addSeconds( edgeStatusCheckTimer );

					// force check online
					forceCheckOnline = true;
				}

				m_.release();
			}

			//
			// edge device status check timer
			//
			if ( !edgeDevicePowerUpTime )
				if ( edgeDevicePingTime <= now )
				{
					m_.acquire();

					if ( !edge_->enabled() )
						TRACE( "edge device is disabled, cannot perform status check" );
					else
					{
						TRACE( "performing status check on edge device..." );

						// we used to send status check here, however Hobart service wants us to send
						// current wrapper state more often.  In speaking with Telit they decided it
						// would be easiest if we just send the power up message again.
						edge_->sendPowerUp(); // edge_->sendStatusCheck();
					}

					edgeDevicePingTime = now;
					edgeDevicePingTime.addSeconds( edgeStatusCheckTimer );

					m_.release();
				}

			//
			// check online to edge gateway
			//
			if (( forceCheckOnline ) || ( edgeCheckOnlineTime <= now ))
			{
				TRACE( "checking if online to gateway..." );
				edge_->checkOnline();

				edgeCheckOnlineTime = now;
				edgeCheckOnlineTime.addSeconds( edgeCheckOnlineTimer );
			}

		} // loop

	}

	TRACE( "exiting..." );
	setState( DiagStopped );

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DiagnosticsManager::ExitThread( bool block )
{
	// check exit event
	if ( m_ExitEvent.IsEventSet() )
    	setState( DiagStop );

	// check state
	m_.acquire();
	bool needToStop( DiagStop == state_ );
	m_.release();

    if ( !needToStop )
    {
    	if ( block )
    	    PAL_absclass::PAL_sleep( 250 );

    	return false;
    }

    // stop
   	TRACE( "stopping..." );
   	setState( DiagStopping );

   	closeDatabase();

   	return true;
}
