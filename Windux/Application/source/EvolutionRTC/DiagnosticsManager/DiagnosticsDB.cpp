/**
 * @file DiagnosticsDB.cpp
 * @brief Database class for diagnostic events.
 *
 * @copyright Copyright (c) 2016-2019, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#include "DiagnosticsDB.h"

#include "DBO/Config.h"
#include "DBO/Diag.h"
#include "DBO/DiagStep.h"
#include "DBO/EventLog.h"
#include "DBO/PrinterDotUsage.h"
#include "DBO/PrinterDotWear.h"
#include "DBO/PrinterHeadStatus.h"
#include "DBO/ServiceAction.h"
#include "DBO/ServiceCat.h"
#include "DBO/ServiceMaintLog.h"
#include "DBO/ServiceMethod.h"
#include "DBO/ServicePart.h"
#include "DBO/ValueMax.h"
#include "DBO/Wizards.h"
#include "DBO/WrapperAuditLog.h"
#include "DBO/WrapperTimings.h"
#include "DBO/WrapperTrays.h"

#include <CfgInternal.h>
#include <Configuration.h>

#include <sqlite3.h>

#include <algorithm>

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"

int DiagnosticsDB::dataType_ = 0;
std::string DiagnosticsDB::dataTypeName_( "rtc.diagnosticsmanager.diagnosticsdb" );
#endif

std::string DiagnosticsDB::category_( "DiagnosticsManager.db" );

#define TRACE( logline ) Debug::trace( category_, (logline) )
#define TRACEFUNC( logline ) if (Debug::isTracing( category_ + "." + __func__ )) TRACE( (logline) )


typedef std::pair<std::string, unsigned int> trayKey;
typedef std::pair<dbo::WrapperTrays::row, dbo::WrapperTimings::rows> trayData;

typedef std::list<trayKey> trayKeyList;
trayKeyList traysOrdering_;

typedef std::map<trayKey, trayData> traysMap;
traysMap traysOld_;
traysMap traysNew_;

typedef std::map<size_t, std::string> timingParametersMap;

timingParametersMap timingParameters_;

///////////////////////////////////////////////////////////////////////////////////////////////////
DiagnosticsDB::DiagnosticsDB( const std::string &filename ) :
	db_( NULL ),
	filename_( filename ),
	version_( 0 )
{
#ifdef USE_ACCOUNTANT
	if ( !dataType )
		dataType_ = util_acc::registerDataType( dataTypeName_ );

	util_acc::addInstance( dataType_ );
#endif

	TRACE( "CTOR" );
	initialize();

	TRACE( "CTOR: done!" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
DiagnosticsDB::~DiagnosticsDB()
{
	TRACE( "DTOR" );

	finalize();

#ifdef USE_ACCOUNTANT
	util_acc::removeInstance( dataType_ );
#endif

	TRACE( "DTOR: done!" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DiagnosticsDB::edgeDeviceTimers( unsigned int &init, unsigned int &powerUp, unsigned int &statusCheck ) const
{
	if ( !db_ )
		return;

	dbo::Config config( db_ );

	init = config.edgeInit();
	powerUp = config.edgePowerUp();
	statusCheck = config.edgeStatusCheckTimer();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
unsigned int DiagnosticsDB::housekeepingDays() const
{
	if ( db_ )
	{
		dbo::Config config( db_ );

		return config.housekeeping();
	}

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DiagnosticsDB::isLatestVersion() const
{
	if ( db_ )
	{
		dbo::Config config( db_ );

		return config.isLatestVersion();
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DiagnosticsDB::isOpen() const
{
	if ( db_ )
		return true;

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DiagnosticsDB::createEvent( int device, int deviceExt, unsigned int deviceCycles, int errorCode, bool forceDiagnostics )
{
	dbo::EventLog eventLogTable( db_ );
	dbo::EventLog::rows eventLogRecs;

	util::Timestamp e;

	// create event
	if ( !(e = eventLogTable.create( device, deviceExt, deviceCycles, errorCode, forceDiagnostics )) )
	{
		TRACE( "error creating event!" );
		return false;
	}

	// retrieve records
	unsigned int minDeviceCycles( deviceCycles < 3 ? 0 : (deviceCycles - 3) );
	unsigned int maxDeviceCycles( deviceCycles );
	unsigned int count( 0 );

	if ( !eventLogTable.queryByDeviceCycles( minDeviceCycles, maxDeviceCycles, eventLogRecs ) )
	{
		TRACE( "error retrieving events!" );
		return false;
	}

	dbo::EventLog::row eventLogRec;

	// iterate results
	for ( dbo::EventLog::rows::iterator rec( eventLogRecs.begin() ); rec != eventLogRecs.end(); ++rec )
	{
		TRACEFUNC( "event (" + util_string::inttostring( count ) + "):\n" + dbo::EventLog::formatRow( *rec ) );

		// filter by device, deviceExt, and errorCode
		if (( device == rec->device ) &&
			(( 0 == deviceExt ) || ( deviceExt == rec->deviceExt )) &&
			( errorCode == rec->errorCode ))
		{
			if ( 0 == count )
				eventLogRec = (*rec);

			++count;
		}
	}

	// generate maint log record
	// event has happened 3 or more times within last 3 cycles
	//  -AND-
	// event is error or critical
	if (( 3 <= count ) && ( eventLogRec.isSevereEvent() ))
	{
		dbo::ServiceMaintLog maintLogTable( db_ );

		// create!
		if ( !maintLogTable.create( eventLogRec.timestamp, eventLogRec.diagErrorCode() ) )
		{
			TRACE( "failed to create maint entry for event log " + eventLogRec.timestamp.toString() );
			return false;
		}
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DiagnosticsDB::lookupEvent( int device, int deviceExt, int errorCode, std::string &eventId, std::string &diagId, bool &isWizardAvail, int &count )
{
	static const int lookupMinutes( 60 );

	dbo::EventLog eventLogTable( db_ );
	dbo::EventLog::rows eventLogRecs;

	TRACE( "lookup event " + util_string::inttostring( device ) + " " + util_string::inttostring( errorCode ) );

	// retrieve records
	if ( !eventLogTable.queryByAge( lookupMinutes, eventLogRecs ) )
	{
		TRACE( "error retrieving events!" );
		return false;
	}

	count = 0;

	// iterate results
	for ( dbo::EventLog::rows::iterator rec( eventLogRecs.begin() ); rec != eventLogRecs.end(); ++rec )
	{
		TRACEFUNC( "event (" + util_string::inttostring( count ) + "):\n" + dbo::EventLog::formatRow( *rec ) );

		// filter by device, deviceExt, and errorCode
		if (( device == rec->device ) &&
			(( 0 == deviceExt ) || ( deviceExt == rec->deviceExt )) &&
												// Both 745 and 567 are canopy open
			( (errorCode == rec->errorCode) || ( errorCode == 745 && rec->errorCode == 567 ) ))
		{
			dbo::Diag diagTable( db_ );
			dbo::Diag::rows diagRecs;

			// retrieve the diagnostic records
			if ( !diagTable.queryByEventLog( rec->timestamp, diagRecs ) )
			{
				TRACE( "failed to retrieve diag with event log " + rec->timestamp.toString() );
				return false;
			}

			// no diagnostic records... check what to do with this error
			else if (( 0 == diagRecs.size() ) && ( !count ))
			{
				// wizard available so create diagnostic record
				if ( rec->wizard )
				{
					util::Timestamp diag;

					dbo::Diag::row diagRec;

					// create diagnostic
					if ( !(diag = diagTable.create( rec->timestamp )) )
					{
						TRACE( "failed to create diagnostic for event log " + rec->timestamp.toString() );
						return false;
					}

					else if ( !diagTable.retrieve( diag, diagRec ) )
					{
						TRACE( "failed to retrieve diag " + diag.toString() );
						return false;
					}

					diagId = diagRec.timestamp.toString();

					diagRecs.push_back( diagRec );
				}
			}

			// diagnostics not available when no records
			if ( 0 == diagRecs.size() )
				break;
			// diagnostics was run when method is other than ignored
			else if ( dbo::DiagRow::Ignored != diagRecs[0].method )
				break;

			// first result
			// give first result to caller
			else if ( !count )
			{
				eventId = rec->timestamp.toString();
				isWizardAvail = rec->wizard;
			}

			// increment count for ignored events (i.e. no records)
			++count;
		}
	}

	return (0 < count);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
unsigned int DiagnosticsDB::lookupEventMaxCount( int device, int deviceExt, int errorCode )
{
	dbo::Wizards wizards( db_ );

	return wizards.lookupMaxCount( device, deviceExt, errorCode );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
unsigned int DiagnosticsDB::lookupEventSeverity( int device, int deviceExt, int errorCode )
{
	dbo::Wizards wizards( db_ );

	return wizards.lookupSeverity( device, deviceExt, errorCode );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DiagnosticsDB::lookupDeviceInfo( const std::string &eventId, int &device, int &deviceExt, int &errorCode )
{
	const util::Timestamp e( eventId );

	// retrieve event
	dbo::EventLog eventLogTable( db_ );
	dbo::EventLog::row eventLogResult;

	if ( !eventLogTable.retrieve( e, eventLogResult ) )
	{
		TRACE( "failed to retrieve event log " + eventId );
		return false;
	}

	// give result to caller
	device = eventLogResult.device;
	deviceExt = eventLogResult.deviceExt;
	errorCode = eventLogResult.errorCode;

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DiagnosticsDB::lookupDeviceInfoFromDiagStep( const std::string &diagStepId, int &device, int &deviceExt, int &errorCode )
{
	const util::Timestamp diagStep( diagStepId );

	dbo::DiagStep diagStepTable( db_ );
	dbo::DiagStep::row diagStepRec;

	// retrieve the diagnostic step
	if ( !diagStepTable.retrieve( diagStep, diagStepRec ) )
	{
		TRACE( "failed to retrieve diag step with id " + diagStepId );
		return false;
	}

	dbo::Diag diagTable( db_ );
	dbo::Diag::row diagRec;

	// retrieve the diagnostic record
	if ( !diagTable.retrieve( diagStepRec.diag, diagRec ) )
	{
		TRACE( "failed to retrieve diag with id " + diagStepRec.diag.toString() );
		return false;
	}

	// lookup device info
	else if ( !lookupDeviceInfo( diagRec.eventLog.toString(), device, deviceExt, errorCode ) )
	{
		TRACE( "failed to retrieve lookup device info for event log " + diagRec.eventLog.toString() );
		return false;
	}

	// override event log error code with diag step error code
	errorCode = diagStepRec.errorCode;

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DiagnosticsDB::updateHardwareInfo( const std::string &diagId, const std::string &hardwareModel, const std::string &hardwareSerial, unsigned int hardwareCycles )
{
	const util::Timestamp diag( diagId );

	dbo::Diag diagTable( db_ );

	if ( !diagTable.updateHardwareInfo( diag, hardwareModel, hardwareSerial, hardwareCycles ) )
	{
		TRACE( "error updating hardware information diag " + diagId );
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DiagnosticsDB::lookupHardwareInfo( const std::string &diagId, std::string &hardwareModel, std::string &hardwareSerial, unsigned int &hardwareCycles )
{
	const util::Timestamp diag( diagId );

	dbo::Diag diagTable( db_ );
	dbo::Diag::row diagRec;

	// lookup record
	if ( !diagTable.retrieve( diag, diagRec ) )
	{
		TRACE( "error updating hardware information diag " + diagId );
		return false;
	}

	// give caller results
	hardwareModel = diagRec.hardwareModel;
	hardwareSerial = diagRec.hardwareSerial;
	hardwareCycles = diagRec.hardwareCycles;

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DiagnosticsDB::createDiagWizard( const std::string &eventId, int method, std::string &diagId, std::string &diagStepId, int &diagErrorCode )
{
	const util::Timestamp e( eventId );

	dbo::EventLog eventLogTable( db_ );
	dbo::EventLog::row eventLogRec;

	// lookup the event
	if ( !eventLogTable.retrieve( e, eventLogRec ) )
	{
		TRACE( "failed to find event log " + e.toString() );
		return false;
	}

	dbo::Diag diagTable( db_ );
	dbo::Diag::rows diagRecs;

	// retrieve the diagnostic records
	if ( !diagTable.queryByEventLog( e, diagRecs ) )
	{
		TRACE( "failed to retrieve diag with event log " + eventId );
		return false;
	}

	// we are modifying first record in result set
	else if ( 0 == diagRecs.size() )
	{
		TRACE( "no diag found with event log " + eventId );
		return false;
	}

	// first record should be ignored method
	else if ( dbo::DiagRow::Ignored != diagRecs[0].method )
	{
		TRACE( "diag " + diagRecs[0].timestamp.toString() +	" invalid method " +
			util_string::longtostring( diagRecs[0].method ) );

		return false;
	}

	// update method
	util::Timestamp diagStep;

	diagErrorCode = eventLogRec.diagErrorCode();

	if ( method <= 0 )
	{
		TRACE( "invalid method " + util_string::longtostring( method ) );
		return false;
	}
	else if ( !diagTable.updateMethod( diagRecs[0].timestamp, (dbo::DiagRow::methodType) method, diagErrorCode, diagStep ) )
	{
		TRACE( "failed to update diag " + diagRecs[0].timestamp.toString() + " with method " +
			util_string::longtostring( method ) );

		return false;
	}
	else if ( !diagStep )
	{
		TRACE( "invalid diag step " + diagStep.toString() );
		return false;
	}

	diagId = diagRecs[0].timestamp.toString();
	diagStepId = diagStep.toString();

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DiagnosticsDB::createDiagWizardFromMaintRecord( const std::string &maintRecordId, int method, std::string &eventId, std::string &diagId, std::string &diagStepId, int &diagErrorCode )
{
	const util::Timestamp maintRecord( maintRecordId );

	dbo::ServiceMaintLog maintLogTable( db_ );
	dbo::ServiceMaintLog::row maintLogRec;

	// retrieve maint log record
	if ( !maintLogTable.retrieve( maintRecord, maintLogRec ) )
	{
		TRACE( "failed to retrieve maint record " + maintRecordId );
		return false;
	}

	// check if diagnostics run previously... if so duplicate...
	if ( maintLogRec.latestDiag )
	{
		TRACE( "diag run previously for this maint log" );

		dbo::DiagStep diagStepTable( db_ );
		dbo::DiagStep::row diagStepRec;

		// retrieve the diagnostic step
		if ( !diagStepTable.retrieve( maintLogRec.latestDiag, diagStepRec ) )
		{
			TRACE( "failed to retrieve diag step with id " + maintLogRec.latestDiag.toString() );
			return false;
		}

		dbo::Diag diagTable( db_ );
		dbo::Diag::row diagRec;

		// retrieve the diagnostic record
		if ( !diagTable.retrieve( diagStepRec.diag, diagRec ) )
		{
			TRACE( "failed to retrieve diag with id " + diagStepRec.diag.toString() );
			return false;
		}

		// create new diag event
		else if ( !diagTable.create( diagRec.eventLog ) )
		{
			TRACE( "failed to create diagnostic for event log " + diagRec.eventLog.toString() );

			return false;
		}

		// create the wizard
		return createDiagWizard(
			eventId = diagRec.eventLog.toString(),
			method,
			diagId,
			diagStepId,
			diagErrorCode );
	}

	TRACE( "no diag previously run for this maint record (strike 1)" );

	// diagnostics was not run previously... lets check if diag available
	dbo::Diag diagTable( db_ );
	dbo::Diag::row diagRec;

	if ( diagTable.retrieve( maintLogRec.latestOccurrence, diagRec ) )
	{
		TRACE( "diag exists for this maint log" );

		// create new diag event
		if ( !diagTable.create( diagRec.eventLog ) )
		{
			TRACE( "failed to create diagnostic for event log " + diagRec.eventLog.toString() );
			return false;
		}

		// create the wizard
		return createDiagWizard(
			eventId = diagRec.eventLog.toString(),
			method,
			diagId,
			diagStepId,
			diagErrorCode );
	}

	TRACE( "no diag exists for this maint record (strike 2)" );

	// diagnostics was not even offered it appears... lets check if wizard available
	dbo::EventLog eventLogTable( db_ );
	dbo::EventLog::row eventLogRec;

	if ( !eventLogTable.retrieve( maintLogRec.latestOccurrence, eventLogRec ) )
	{
		TRACE( "failed to retrieve event log with id " + maintLogRec.latestOccurrence.toString() );
		return false;
	}
	else if ( eventLogRec.wizard )
	{
		TRACE( "wizard exists for this maint log" );

		// create new diag event
		if ( !diagTable.create( eventLogRec.timestamp ) )
		{
			TRACE( "failed to create diagnostic for event log " + eventLogRec.timestamp.toString() );
			return false;
		}

		// create the wizard
		return createDiagWizard(
			eventId = eventLogRec.timestamp.toString(),
			method,
			diagId,
			diagStepId,
			diagErrorCode );
	}

	TRACE( "no wizard available for this maint log entry (strike 3, were out!)" );
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DiagnosticsDB::lookupDiagWizardMethod( const std::string &diagStepId, int &method )
{
	const util::Timestamp diagStep( diagStepId );

	// lookup diag step
	dbo::DiagStep diagStepTable( db_ );
	dbo::DiagStep::row diagStepRec;

	if ( !diagStepTable.retrieve( diagStep, diagStepRec ) )
	{
		TRACE( "failed to retrieve diag step " + diagStepId );
		return false;
	}

	// lookup diag
	dbo::Diag diagTable( db_ );
	dbo::Diag::row diagRec;

	if ( !diagTable.retrieve( diagStepRec.diag, diagRec ) )
	{
		TRACE( "failed to retrieve diag " + diagStepRec.diag.toString() );
		return false;
	}

	method = diagRec.method;
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DiagnosticsDB::startDiagWizardEventTest( const std::string &diagStepId )
{
	const util::Timestamp diagStep( diagStepId );

	dbo::DiagStep diagStepTable( db_ );

	// update state
	if ( !diagStepTable.updateState( diagStep, dbo::DiagStepRow::InProgress ) )
	{
		TRACE( "failed to update diag step " + diagStepId + " to in progress" );
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DiagnosticsDB::finishDiagWizardEventTest( const std::string &diagStepId, diagStepTestCode testCode, int result, int routine, int routineExpectedResult, int routineActualResult )
{
	const util::Timestamp diagStep( diagStepId );

	// determine new state
	dbo::DiagStepRow::stateType newState( dbo::DiagStepRow::Pass );

	if ( Fail == testCode )
		newState = dbo::DiagStepRow::Fail;
	else if ( FailTimeout == testCode )
		newState = dbo::DiagStepRow::FailTimeout;
	else if ( Pass != testCode )
	{
		TRACE( "invalid state " + util_string::longtostring( testCode ) );
		return false;
	}

	// update state
	dbo::DiagStep diagStepTable( db_ );

	if ( !diagStepTable.updateState( diagStep, newState, result ) )
	{
		TRACE( "failed to update diag step " + diagStepId +
			" to state " + util_string::longtostring( newState ) );

		return false;
	}

	// save routine info and service record info
	if ( dbo::DiagStepRow::Pass != newState )
	{
		// update diag routine information
		if ( routine )
			if ( !diagStepTable.updateRoutineInfo( diagStep, routine, routineExpectedResult, routineActualResult ) )
			{
				TRACE( "failed to update diag step " + diagStepId + " routine info" );
				return false;
			}

		// generate service record
		// 1) nothing to do when no error code
		// 2) when failure comes explicitly from device record regardless of severity
		// 3) check severity level for timeout failures
		if ( !result )
			TRACE( "not creating service record due to empty error code" );
		else if (( FailTimeout == testCode ) && ( lookupEventSeverity( result / 1000, 0, result % 1000 ) <= dbo::WizardsRow::Warning ))
			TRACE( "not creating service record due to low severity" );
		else
		{
			// retrieve record
			dbo::DiagStep::row diagStepRec;

			if ( !diagStepTable.retrieve( diagStep, diagStepRec ) )
			{
				TRACE( "failed to retrieve diag step " + diagStepId );
				return false;
			}
			else if ( !diagStepRec.diagCompleteTimestamp )
				TRACE( "diag step " + diagStepId + " does not appear to have completed" );
			else
			{
				// create/update service record
				dbo::ServiceMaintLog maintLog( db_ );

				if ( !maintLog.create( diagStep, result, diagStep ) )
				{
					TRACE( "failed to create maint rec for diag step " + diagStepId );
					return false;
				}
			}
		}
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DiagnosticsDB::retrieveActionRecords( int errorCode, util::DictionaryList &results )
{
	dbo::Wizards wizards( db_ );

	dbo::ServiceAction serviceActionTable( db_ );
	dbo::ServiceAction::rows serviceActionRecs;

	int category( wizards.lookupServiceCategory( errorCode / 1000, 0, errorCode % 1000 ) );

	// lookup actions by category
	if (( !category ) ||
		( !serviceActionTable.queryByCategory( category, serviceActionRecs ) ) ||
		( !serviceActionRecs.size() ))
	{
		// no results, switch to generic category
		if ( Configuration::getInstance()->isServiceCounterScale() )
			category = 400;
		else if ( Configuration::getInstance()->isPrepackScale() )
			category = 300;
		else
		{
			TRACE( "unknown scale type!" );
			return false;
		}
	}

	// lookup actions by generic category
	if (( !category ) ||
		( !serviceActionTable.queryByCategory( category, serviceActionRecs ) ) ||
		( !serviceActionRecs.size() ))
	{
		TRACE( "unknown error category!" );
		return false;
	}

	// iterate results
	for ( dbo::ServiceAction::rows::const_iterator i( serviceActionRecs.begin() ); i != serviceActionRecs.end(); ++i )
	{
		util::Dictionary dict;
		dict.setCommaSeparated( results.isCommaSeparated() );

		// retrieve record in dictionary format
		i->retrieve( dict );

		// append record
		results.append( dict );
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DiagnosticsDB::retrieveMaintLogRecords( int filter, int age, util::DictionaryList &results )
{
	dbo::ServiceMaintLog maintLog( db_ );
	dbo::ServiceMaintLog::rows maintLogRecs;

	bool queryResult( false );

	// pull records
	// OpenRecordsOnly = 0
	// ResolvedRecordsOnly = 1
	// AllRecords = 2
	if ( 0 == filter )
		queryResult = maintLog.queryByAge_OpenRecordsOnly( age, maintLogRecs );
	else if ( 1 == filter )
		queryResult = maintLog.queryByAge_ResolvedRecordsOnly( age, maintLogRecs );
	else if ( 2 == filter )
		queryResult = maintLog.queryByAge_AllRecords( age, maintLogRecs );
	else
	{
		TRACE( "invalid filter " + util_string::longtostring( filter ) );
		return false;
	}

	// validate response
	if ( !queryResult )
		TRACE( "no maint records returned from query " + util_string::longtostring( filter ) + " " + util_string::longtostring( age ) );
	else
	{
		for ( dbo::ServiceMaintLog::rows::const_iterator i( maintLogRecs.begin() ); i != maintLogRecs.end(); ++i )
		{
			util::Dictionary dict;
			dict.setCommaSeparated( results.isCommaSeparated() );

			// retrieve record in dictionary format
			i->retrieve( dict );

			// append severity
			std::string errorCodeStr( dict["errorCode"] );
			int severity( 0 );

			if ( errorCodeStr.length() )
			{
				const int errorCode( util_string::stringtolong( errorCodeStr ) );

				severity = lookupEventSeverity( errorCode / 1000, 0, errorCode % 1000 );
			}

			dict.append( "severity", util_string::longtostring( severity ) );

			// append record
			results.append( dict );
		}

		return true;
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DiagnosticsDB::clearMaintLogRecord( const std::string &maintRecordId, maintLogResolutionReason reason, int serviceActionCount, int *serviceMethods, int *serviceParts, const std::string &serviceNotes )
{
	const util::Timestamp maintRecord( maintRecordId );

	// compute state
	dbo::ServiceMaintLogRow::stateType newState( dbo::ServiceMaintLogRow::Ignored );

	if ( ResolvedByService == reason )
		newState = dbo::ServiceMaintLogRow::ResolvedByService;
	else if ( ResolvedByDiag == reason )
		newState = dbo::ServiceMaintLogRow::ResolvedByDiag;
	else if ( Ignored != reason )
	{
		TRACE( "invalid state " + util_string::longtostring( reason ) );

		return false;
	}

	// update state
	dbo::ServiceMaintLog maintLog( db_ );

	if ( !maintLog.updateState( maintRecord, newState ) )
	{
		TRACE( "failed to update maint log state " + maintRecordId +
			" to state " + util_string::longtostring( newState ) );

		return false;
	}

	// set service information
	if ( dbo::ServiceMaintLogRow::ResolvedByService == newState )
	{
		dbo::ServiceMaintLogRow row;

		dbo::ServiceMaintLog::serviceMethodPartList actions;

		for ( int i( 0 ); i < serviceActionCount; ++i )
			actions.push_back( dbo::ServiceMaintLog::serviceMethodPartList::value_type( serviceMethods[i], serviceParts[i] ) );

		if ( !maintLog.updateServiceInfo( maintRecord, actions, serviceNotes ) )
		{
			TRACE( "failed to update maint log service info " + maintRecordId );
			return false;
		}
		else if ( maintLog.retrieve( maintRecord, row ) )
		{
			dbo::ServiceMethod method( db_ );
			dbo::ServiceMethod::row methodRow;

			dbo::ServicePart part( db_ );
			dbo::ServicePart::row partRow;

			std::string report;

			report += "ERROR CODE: " + util_string::longtostring( row.errorCode ) + "\n";
			report += "\n";

			// method1 / part1
			if ( method.retrieve( row.serviceMethod1, methodRow ) )
				report += "METHOD: " + methodRow.description + "\n";
			else
				report += "METHOD: " + util_string::longtostring( row.serviceMethod1 ) + "\n";

			if ( part.retrieve( row.servicePart1, partRow ) )
				report += "PART: " + partRow.description + "\n";
			else
				report += "PART: " + util_string::longtostring( row.servicePart1 ) + "\n";

			// method2 / part2
			if ( row.servicePart2 )
			{
				report += "\n";

				if ( method.retrieve( row.serviceMethod2, methodRow ) )
					report += "METHOD: " + methodRow.description + "\n";
				else
					report += "METHOD: " + util_string::longtostring( row.serviceMethod2 ) + "\n";

				if ( part.retrieve( row.servicePart2, partRow ) )
					report += "PART: " + partRow.description + "\n";
				else
					report += "PART: " + util_string::longtostring( row.servicePart2 ) + "\n";
			}

			// method3 / part3
			if ( row.servicePart3 )
			{
				report += "\n";

				if ( method.retrieve( row.serviceMethod3, methodRow ) )
					report += "METHOD: " + methodRow.description + "\n";
				else
					report += "METHOD: " + util_string::longtostring( row.serviceMethod3 ) + "\n";

				if ( part.retrieve( row.servicePart3, partRow ) )
					report += "PART: " + partRow.description + "\n";
				else
					report += "PART: " + util_string::longtostring( row.servicePart3 ) + "\n";
			}

			// method4 / part4
			if ( row.servicePart4 )
			{
				report += "\n";

				if ( method.retrieve( row.serviceMethod4, methodRow ) )
					report += "METHOD: " + methodRow.description + "\n";
				else
					report += "METHOD: " + util_string::longtostring( row.serviceMethod4 ) + "\n";

				if ( part.retrieve( row.servicePart4, partRow ) )
					report += "PART: " + partRow.description + "\n";
				else
					report += "PART: " + util_string::longtostring( row.servicePart4 ) + "\n";
			}

			report += "\n";
			report += "NOTES:\n";
			report += row.serviceNotes;

			// ---- //

			dbo::WrapperAuditLog auditLog( db_ );

			auditLog.create(
				util::Timestamp::now(),
				dbo::WrapperAuditLogRow::Info,
				dbo::WrapperAuditLogRow::ServiceVisit,
				dbo::WrapperAuditLogRow::ServiceVisit_ResolveError,
				"Error resolved by Service.\n\n" + report );
		}
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DiagnosticsDB::generateMaintLogReport( int filter, int age, std::string &report )
{
	dbo::ServiceMaintLog maintLog( db_ );
	dbo::ServiceMaintLog::rows maintLogRecs;

	bool queryResult( false );

	// pull records
	// OpenRecordsOnly = 0
	// ResolvedRecordsOnly = 1
	// AllRecords = 2
	if ( 0 == filter )
		queryResult = maintLog.queryByAge_OpenRecordsOnly( age, maintLogRecs );
	else if ( 1 == filter )
		queryResult = maintLog.queryByAge_ResolvedRecordsOnly( age, maintLogRecs );
	else if ( 2 == filter )
		queryResult = maintLog.queryByAge_AllRecords( age, maintLogRecs );
	else
	{
		TRACE( "invalid filter " + util_string::longtostring( filter ) );
		return false;
	}

	// validate response
	if ( !queryResult )
		TRACE( "no maint records returned from query " + util_string::longtostring( filter ) + " " + util_string::longtostring( age ) );
	else if ( !maintLogRecs.size() )
	{
		report = "NO RECORDS";
		return true;
	}
	else
	{
		util::Timestamp timestamp( util::Timestamp::now() );

		report += timestamp.toDateTime();
		report += "\n";

		for ( dbo::ServiceMaintLog::rows::const_iterator i( maintLogRecs.begin() ); i != maintLogRecs.end(); ++i )
		{
			report += "\n";

			report += "ERROR CODE: ";
			report += util_string::longtostring( i->errorCode );
			report += "\n";

			report += "Date: ";
			report += i->latestOccurrence.toDateTime();
			report += "\n";

			dbo::Diag diagTable( db_ );
			dbo::Diag::rows diagRecs;

			if ( diagTable.queryByEventLog( i->latestOccurrence, diagRecs ) )
				if ( diagRecs.size() )
				{
					report += "Cycle Count: ";
					report += util_string::longtostring( diagRecs[0].hardwareCycles );
					report += "\n";
				}

			report += "First Occurrence: ";
			report += i->timestamp.toDateTime();
			report += "\n";

			report += "Occurrences: ";
			report += util_string::longtostring( i->count );
			report += "\n";
		}

		return true;
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DiagnosticsDB::resolveMaintLog( unsigned int deviceCycles, int &resolved )
{
	resolved = 0;

	dbo::EventLog eventLogTable( db_ );
	dbo::EventLog::rows eventLogRecs;

	// retrieve records
	unsigned int minDeviceCycles( deviceCycles < 2 ? 0 : (deviceCycles - 2) );
	unsigned int maxDeviceCycles( deviceCycles );

	if ( !eventLogTable.queryByDeviceCycles( minDeviceCycles, maxDeviceCycles, eventLogRecs ) )
	{
		TRACE( "error retrieving events!" );
		return false;
	}

	dbo::ServiceMaintLog maintLogTable( db_ );
	dbo::ServiceMaintLog::rows maintLogRecs;

	// retrieve records
	if ( !maintLogTable.queryByAge_OpenRecordsOnly( 0, maintLogRecs ) )
	{
		TRACE( "error retrieving maint log!" );
		return false;
	}

	// iterate results
	for ( dbo::ServiceMaintLog::rows::iterator rec( maintLogRecs.begin() ); rec != maintLogRecs.end(); ++rec )
	{
		bool exists( false );

		// check for event log occurences
		for ( dbo::EventLog::rows::iterator eventRec( eventLogRecs.begin() ); eventRec != eventLogRecs.end(); ++eventRec )
			if ( eventRec->diagErrorCode() == rec->errorCode )
			{
				exists = true;
				break;
			}

		if ( !exists )
		{
			if ( clearMaintLogRecord( rec->timestamp.toString(), ResolvedByDiag ) )
				++resolved;
			else
			{
				TRACE( "error clearing maint log record!" );
				return false;
			}
		}
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DiagnosticsDB::updateWrapperBoxBegin()
{
	bool result( true );

	// clear existing data
	TRACEFUNC( "clearing data..." );

	traysOrdering_.clear();

	traysOld_.clear();
	traysNew_.clear();

	// retrieve all trays
	dbo::WrapperTrays wrapperTrays( db_ );
	dbo::WrapperTrays::rows trays;

	TRACEFUNC( "retrieving trays..." );

	if ( !wrapperTrays.retrieveAll( trays ) )
		result = false;
	else
	{
		dbo::WrapperTimings wrapperTimings( db_ );
		dbo::WrapperTimings::rows trayTimings;

		// retrieve timings
		for ( dbo::WrapperTrays::rows::iterator tray( trays.begin() ); tray != trays.end(); ++tray )
		{
			trayTimings.clear();

			TRACEFUNC( "retrieving timings..." );

			if ( wrapperTimings.retrieve( tray->name, tray->plu, trayTimings ) )
				traysOld_[ trayKey( tray->name, tray->plu ) ] = trayData( *tray, trayTimings );
			else
			{
				TRACE( "missing timings data for tray '" + tray->name + "' plu exception " + util_string::longtostring( tray->plu ) );
				result = false;
			}
		}
	}

	TRACEFUNC( "done" );

	return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DiagnosticsDB::updateWrapperBox( const std::string &name, unsigned int plu, unsigned int lengthMin, unsigned int lengthMax, unsigned int widthMin, unsigned int widthMax, unsigned int lip, const timingsMap &timings )
{
	dbo::WrapperTrays::row tray;
	dbo::WrapperTimings::rows trayTimings;

	//
	// process wrapper tray
	//

	tray.name = name;
	tray.plu = plu;
	tray.lengthMin = lengthMin;
	tray.lengthMax = lengthMax;
	tray.widthMin = widthMin;
	tray.widthMax = widthMax;
	tray.lip = lip;

	//
	// process timings
	//

	dbo::WrapperTimings::row r;
	r.name = name;
	r.plu = plu;

	// generate wrapper timings record
	for ( timingsMap::const_iterator height( timings.begin() ); height != timings.end(); ++height )
	{
		r.height = height->first;

		timingsParamList::const_iterator param( height->second.begin() );
		timingsParamList::const_iterator paramEnd( height->second.end() );

		while ( param != paramEnd )
		{
			r.param = param->first;
			r.paramValue = param->second;

			trayTimings.push_back( r );
			++param;
		}
	}

	// save tray
	const trayKey key( name, plu );

	traysOrdering_.push_front( key ); // they are sent in reverse order
	traysNew_[ key ] = trayData( tray, trayTimings );

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool findParamValue( const dbo::WrapperTimings::rows &timings, unsigned int height, unsigned int param, unsigned int &paramValue )
{
	dbo::WrapperTimings::rows::const_iterator p( timings.begin() );
	dbo::WrapperTimings::rows::const_iterator pend( timings.end() );

	while ( p != pend )
	{
		if (( p->height == height ) && ( p->param == param ))
		{
			paramValue = p->paramValue;
			return true;
		}

		++p;
	}

	return false;
}

std::string trayReport( const trayData *t, bool added, bool removed, const trayData *modified )
{
	const dbo::WrapperTrays::row &tray( t->first );
	const dbo::WrapperTimings::rows &timings( t->second );

	std::string report;

	report += "TRAY '" + tray.name + "' ";

	if ( 0 != tray.plu )
		report += "PLU EXCEPTION " + util_string::longtostring( tray.plu ) + " ";

	if ( removed )
		report += "[DELETED]\n";
	else
	{
		if ( added )
			report += "[ADDED]\n";
		else
			report += "[MODIFIED]\n";

		report += "    length min: " + util_string::longtostring( tray.lengthMin ) + " ";
		if ( added )
			report += "[ADDED]\n";
		else if (( modified ) && ( tray.lengthMin != modified->first.lengthMin ))
			report += "[was " + util_string::longtostring( modified->first.lengthMin ) + "]\n";
		else
			report += "\n";

		report += "    length max: " + util_string::longtostring( tray.lengthMax ) + " ";
		if ( added )
			report += "[ADDED]\n";
		else if (( modified ) && ( tray.lengthMax != modified->first.lengthMax ))
			report += "[was " + util_string::longtostring( modified->first.lengthMax ) + "]\n";
		else
			report += "\n";

		report += "     width min: " + util_string::longtostring( tray.widthMin ) + " ";
		if ( added )
			report += "[ADDED]\n";
		else if (( modified ) && ( tray.widthMin != modified->first.widthMin ))
			report += "[was " + util_string::longtostring( modified->first.widthMin ) + "]\n";
		else
			report += "\n";

		report += "     width max: " + util_string::longtostring( tray.widthMax ) + " ";
		if ( added )
			report += "[ADDED]\n";
		else if (( modified ) && ( tray.widthMax != modified->first.widthMax ))
			report += "[was " + util_string::longtostring( modified->first.widthMax ) + "]\n";
		else
			report += "\n";

		report += "    lip height: " + util_string::longtostring( tray.lip ) + " ";
		if ( added )
			report += "[ADDED]\n";
		else if (( modified ) && ( tray.lip != modified->first.lip ))
			report += "[was " + util_string::longtostring( modified->first.lip ) + "]\n";
		else
			report += "\n";

		dbo::WrapperTimings::rows::const_iterator p( timings.begin() );
		dbo::WrapperTimings::rows::const_iterator pend( timings.end() );

		unsigned int height( -1 );
		unsigned int paramValue( -1 );

		while ( p != pend )
		{
			const bool found(( modified ) && ( findParamValue( modified->second, p->height, p->param, paramValue ) ));
			const bool a(( added ) || ( !found ));

			if ( height != p->height )
			{
				report += "\n";
				report += "  HEIGHT " + util_string::longtostring( height = p->height ) + ": ";

				if ( a )
					report += "[ADDED]\n";
				else
					report += "\n";
			}

			report += "      " + timingParameters_[ p->param ] + ": " + util_string::longtostring( p->paramValue ) + " ";

			if ( a )
				report += "[ADDED]\n";
			else if (( found ) && ( p->paramValue != paramValue ))
				report += "[was " + util_string::longtostring( paramValue ) + "]\n";
			else
				report += "\n";

			++p;
		}

		if ( modified )
		{
			p = modified->second.begin();
			pend = modified->second.end();

			height = -1;

			while ( p != pend )
			{
				if ( height != p->height )
				{
					if ( !findParamValue( timings, p->height, p->param, paramValue ) )
					{
						report += "\n";
						report += "  HEIGHT " + util_string::longtostring( height = p->height ) + ": [DELETED]\n";
					}

					height = p->height;
				}

				++p;
			}

		}
	}

	report += "\n";

	return report;
}

bool DiagnosticsDB::updateWrapperBoxEnd()
{
	const util::Timestamp now( util::Timestamp::now() );

	bool result( true );

	//
	// COMPUTE CHANGES
	//

	TRACEFUNC( "finding changes..." );

	trayKeyList added;
	trayKeyList removed;
	trayKeyList modified;

	// added or modified trays
	for ( traysMap::const_iterator tray( traysNew_.begin() ); tray != traysNew_.end(); ++tray )
	{
		const trayKey *key( &tray->first );
		const trayData *trayOld( &tray->second );

		// find new tray in old list
		traysMap::const_iterator t( traysOld_.find( *key ) );

		if ( traysOld_.end() == t )
		{
			TRACE( "ADDED TRAY '" + key->first + "' plu exception " + util_string::longtostring( key->second ) );
			added.push_back( *key );
		}
		else
		{
			const trayData *trayNew( &t->second );

			if ( (*trayOld) != (*trayNew) )
			{
				TRACE( "MODIFIED TRAY '" + key->first + "' plu exception " + util_string::longtostring( key->second ) );
				modified.push_back( *key );
			}
		}
	}

	// removed
	for ( traysMap::const_iterator tray( traysOld_.begin() ); tray != traysOld_.end(); ++tray )
	{
		const trayKey *key( &tray->first );

		// find old tray in new list
		traysMap::const_iterator t( traysNew_.find( *key ) );

		if ( traysNew_.end() == t )
		{
			TRACE( "REMOVED TRAY '" + key->first + "' plu exception " + util_string::longtostring( key->second ) );
			removed.push_back( *key );
		}
	}

	//
	// GENERATE REPORT
	//

	TRACEFUNC( "generating report..." );

	std::string report;

	for ( trayKeyList::const_iterator key( removed.begin() ); key != removed.end(); ++key )
		report += trayReport( &traysOld_[*key], false, true, NULL );

	for ( trayKeyList::const_iterator key( added.begin() ); key != added.end(); ++key )
		report += trayReport( &traysNew_[*key], true, false, NULL );

	for ( trayKeyList::const_iterator key( modified.begin() ); key != modified.end(); ++key )
		report += trayReport( &traysNew_[*key], false, false, &traysOld_[*key] );

	report += "ORDER:\n";

	for ( trayKeyList::const_iterator key( traysOrdering_.begin() ); key != traysOrdering_.end(); ++key )
	{
		report += "  TRAY '" + key->first + "' ";

		if ( 0 != key->second )
			report += "PLU EXCEPTION " + util_string::longtostring( key->second );

		report += "\n";
	}

	TRACEFUNC( "saving report..." );
	TRACEFUNC( report );

	dbo::WrapperAuditLog wrapperAuditLog( db_ );

	result = wrapperAuditLog.create(
		now,
		dbo::WrapperAuditLogRow::Info,
		dbo::WrapperAuditLogRow::Timings,
		dbo::WrapperAuditLogRow::Timings_Modified,
		"Timings were modified:\n\n" + report );

	if ( !result )
	{
		TRACE( "failed to save report into audit log!" );
		return false;
	}

	//
	// SAVE
	//

	TRACEFUNC( "clearing existing trays..." );

	dbo::WrapperTrays wrapperTrays( db_ );
	dbo::WrapperTimings wrapperTimings( db_ );

	// clear existing trays and timings
	if (( !wrapperTrays.deleteAll() ) || ( !wrapperTimings.deleteAll() ))
		return false;

	TRACEFUNC( "saving trays..." );

	// save tray data
	for ( traysMap::const_iterator tray( traysNew_.begin() ); tray != traysNew_.end(); ++tray )
	{
		const trayKey *key( &tray->first );
		const trayData *t( &tray->second );

		if ( !wrapperTrays.save( t->first ) )
		{
			TRACE( "error saving tray '" + key->first + "' plu exception " + util_string::longtostring( key->second ) );
			result = false;
		}
		else if ( !wrapperTimings.save( t->second ) )
		{
			TRACE( "error saving timings for tray '" + key->first + "' plu exception " + util_string::longtostring( key->second ) );
			result = false;
		}
	}

	TRACEFUNC( "done" );

	return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DiagnosticsDB::clearAllWrapperBoxes()
{
	const util::Timestamp now( util::Timestamp::now() );

	bool result ( true );

	//
	// record event to audit log
	//

	dbo::WrapperAuditLog wrapperAuditLog( db_ );

	result = wrapperAuditLog.create(
		now,
		dbo::WrapperAuditLogRow::Warning,
		dbo::WrapperAuditLogRow::Timings,
		dbo::WrapperAuditLogRow::Timings_Reset,
		"Timings were reset to default!" );

	if ( !result )
	{
		TRACE( "failed to save report into audit log!" );
		return false;
	}

	//
	// clear trays and timings
	//

	dbo::WrapperTrays wrapperTrays( db_ );
	dbo::WrapperTimings wrapperTimings( db_ );

	TRACE( "delete all wrapper trays..." );
	result &= wrapperTrays.deleteAll();

	TRACE( "delete all wrapper timings..." );
	result &= wrapperTimings.deleteAll();

	return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DiagnosticsDB::createUnlockTimings()
{
	const util::Timestamp now(util::Timestamp::now());

	bool result(true);

	dbo::WrapperAuditLog wrapperAuditLog(db_);

	result = wrapperAuditLog.create(
		now,
		dbo::WrapperAuditLogRow::Info,
		dbo::WrapperAuditLogRow::Timings,
		dbo::WrapperAuditLogRow::Timings_Unlocked,
		"Wrapper Timings Unlocked.");

	if (!result)
	{
		TRACE("failed to save Unlock Timings report into audit log!");
		return false;
	}
	else
		TRACE("Save Unlock Timings report into audit log!");

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DiagnosticsDB::updateFilmStatus( unsigned int upperFilmWidth, unsigned int lowerFilmWidth )
{
	dbo::Config config( db_ );

	if (( config.wrapperUpperFilmWidth() == upperFilmWidth ) && ( config.wrapperLowerFilmWidth() == lowerFilmWidth ))
		return true;

	const util::Timestamp now( util::Timestamp::now() );

	bool result( true );

	//
	// generate report
	//

	std::string report;

	report += "Film widths were modified:\n";
	report += "\n";

	report += "    Upper film width: " + util_string::longtostring( upperFilmWidth ) + " ";

	if ( config.wrapperUpperFilmWidth() != upperFilmWidth )
		report += "[was " + util_string::longtostring( config.wrapperUpperFilmWidth() ) + "]";

	report += "\n";

	report += "    Lower film width: " + util_string::longtostring( lowerFilmWidth ) + " ";

	if ( config.wrapperLowerFilmWidth() != lowerFilmWidth )
		report += "[was " + util_string::longtostring( config.wrapperLowerFilmWidth() ) + "]";

	report += "\n";

	//
	// record event to audit log
	//

	dbo::WrapperAuditLog wrapperAuditLog( db_ );

	result = wrapperAuditLog.create(
		now,
		dbo::WrapperAuditLogRow::Warning,
		dbo::WrapperAuditLogRow::Film,
		dbo::WrapperAuditLogRow::Film_RollWidthModified,
		report );

	if ( !result )
	{
		TRACE( "failed to save report into audit log!" );
		return false;
	}

	//
	// save values
	//

	config.setWrapperUpperFilmWidth( upperFilmWidth );
	config.setWrapperLowerFilmWidth( lowerFilmWidth );

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DiagnosticsDB::retrieveAuditLog( int filter, int age, util::DictionaryList &results )
{
	dbo::WrapperAuditLog auditLog( db_ );
	dbo::WrapperAuditLog::rows auditLogRecs;

	bool queryResult( false );

	// pull records
	// AllRecords = -1
	if ( -1 == filter )
		queryResult = auditLog.queryByAge_AllRecords( age, auditLogRecs );
	else
		queryResult = auditLog.queryByAge_FromSourceOnly( age, filter, auditLogRecs );

	// validate response
	if ( !queryResult )
		TRACE( "no audit records returned from query " + util_string::longtostring( filter ) + " " + util_string::longtostring( age ) );
	else
	{
		for ( dbo::WrapperAuditLog::rows::const_iterator i( auditLogRecs.begin() ); i != auditLogRecs.end(); ++i )
		{
			util::Dictionary dict;
			dict.setCommaSeparated( results.isCommaSeparated() );

			// retrieve record in dictionary format
			i->retrieve( dict );

			// remove description
			dict.remove( "description" );

			// append record
			results.append( dict );
		}

		return true;
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DiagnosticsDB::retrieveAuditRecord( const std::string &recordId, int &severity, int &source, int &sourceExt, std::string &description )
{
	const util::Timestamp record( recordId );

	dbo::WrapperAuditLog auditLog( db_ );
	dbo::WrapperAuditLog::row auditLogRec;

	if ( !auditLog.retrieve( record, auditLogRec ) )
		TRACE( "no audit record found for " + recordId );
	else
	{
		severity = auditLogRec.severity;
		source = auditLogRec.source;
		sourceExt = auditLogRec.sourceExt;
		description = auditLogRec.description;

		return true;
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DiagnosticsDB::createServiceVisit( bool start, bool stop, const std::string &description )
{
	const util::Timestamp now( util::Timestamp::now() );

	bool result( true );

	dbo::WrapperAuditLog wrapperAuditLog( db_ );

	if ( start )
	{
		result = wrapperAuditLog.create(
			now,
			dbo::WrapperAuditLogRow::Info,
			dbo::WrapperAuditLogRow::ServiceVisit,
			dbo::WrapperAuditLogRow::ServiceVisit_Start,
			"Service visit has started.\n\n" + description );
	}
	else if ( stop )
	{
		result = wrapperAuditLog.create(
			now,
			dbo::WrapperAuditLogRow::Info,
			dbo::WrapperAuditLogRow::ServiceVisit,
			dbo::WrapperAuditLogRow::ServiceVisit_Stop,
			"Service visit is complete.\n\n" + description );
	}

	if ( !result )
	{
		TRACE( "failed to save report into audit log!" );
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DiagnosticsDB::checkServiceVisit()
{
	bool result( false );

	dbo::WrapperAuditLog auditLog( db_ );
	dbo::WrapperAuditLog::rows auditLogRows;

	// check last 24h of history
	if ( auditLog.queryByAge_FromSourceOnly( 1, dbo::WrapperAuditLogRow::ServiceVisit, auditLogRows ) )
		for ( size_t i( 0 ); i < auditLogRows.size(); ++i )
		{
			// not within visit
			if ( dbo::WrapperAuditLogRow::ServiceVisit_Stop == auditLogRows[i].sourceExt )
				break;

			// we are in service visit
			else if ( dbo::WrapperAuditLogRow::ServiceVisit_Start == auditLogRows[i].sourceExt )
			{
				result = true;
				break;
			}
		}

	return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DiagnosticsDB::createHeadStatusEvent( int printerNum, unsigned int cycles, int status )
{
	const util::Timestamp now( util::Timestamp::now() );

	dbo::PrinterHeadStatus headStatus( db_ );

	const bool result(
		headStatus.create(
			now,
			printerNum,
			cycles,
			status ) );

	if ( !result )
	{
		TRACE( "failed to create printer head status!" );
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DiagnosticsDB::updateDotWear( int printerNum, const std::vector<int> &status, const std::vector<int> &value )
{
	dbo::PrinterHeadStatus headStatus( db_ );
	dbo::PrinterHeadStatus::rows headStatusRows;

	// locate head status event for this dot wear event
	if (( !headStatus.queryByAge( 8, printerNum, headStatusRows ) ) || ( 0 == headStatusRows.size() ))
	{
		TRACE( "failed to locate head status event!" );
		return false;
	}

	// generate record for each dot
	bool result( true );

	dbo::PrinterDotWear dotWear( db_ );

#if defined(min)
	int numDots( (int) min( status.size(), value.size() ) );
#else
	int numDots( (int) std::min( status.size(), value.size() ) );
#endif

	while ( numDots-- )
		result &= dotWear.create( headStatusRows[0].timestamp, numDots, status[numDots], value[numDots] );

	if ( !result )
	{
		TRACE( "failed to create one or more dot wear rows!" );
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DiagnosticsDB::updateDotUsage( int printerNum, const std::vector<int> &printed, const std::vector<int> &removed )
{
	const util::Timestamp now( util::Timestamp::now() );

	// generate record for each dot
	bool result( true );

	dbo::PrinterDotUsage dotUsage( db_ );

#if defined(min)
	int numDots( (int) min( printed.size(), removed.size() ) );
#else
	int numDots( (int) std::min( printed.size(), removed.size() ) );
#endif

	while ( numDots-- )
		if (( printed[numDots] ) || ( removed[numDots] ))
			result &= dotUsage.create( now, printerNum, numDots, printed[numDots], removed[numDots] );

	if ( !result )
	{
		TRACE( "failed to create one or more dot usage rows!" );
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DiagnosticsDB::createValueMax( int transNum, int productNum, double unitPrice, int quantity, double netWeight, double tare, double totalPrice, int counts, int scaleFactor, double tiltX, double tiltY, double adjNetWeight, double adjTotalPrice, double savings )
{
	const util::Timestamp now( util::Timestamp::now() );

	dbo::ValueMax valueMax( db_ );

	// generate record
	if ( !valueMax.create( now, transNum, productNum, unitPrice, quantity, netWeight, tare, totalPrice, counts, scaleFactor, tiltX, tiltY, adjNetWeight, adjTotalPrice, savings ) )
	{
		TRACE( "failed to create value max row!" );
		return false;
	}

	// update lifetime savings
	if ( 0.0 < savings )
	{
		dbo::Config config( db_ );

		if ( !config.addValueMaxSavings( savings ) )
		{
			TRACE( "failed to update value max savings total!" );
			return false;
		}
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DiagnosticsDB::retrieveValueMaxSavings( double& savings )
{
	dbo::ValueMax valueMax( db_ );

	// retrieve savings
	return valueMax.totalSavings( savings );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DiagnosticsDB::upgrade( _Myt &db )
{
	int result;

	sqlite3_stmt *stmt( NULL );
	std::string s;

	// process upgrade from version to version
	for ( unsigned int ver( db.version() ); ver < version(); ++ver )
	{
		TRACE( "upgrade db from " +	util_string::longtostring( ver ) + "..." );

		// v1 ==> v2
		if ( 1 == ver )
		{
			// these tables were added in v2
			if (( !dbo::WrapperTimings::createTable( db.db_ ) ) ||
				( !dbo::WrapperTrays::createTable( db.db_ ) ))
			{
				return false;
			}
		}

		// v2 ==> v3
		else if ( 2 == ver )
		{
			// this table was added in v3
			if ( !dbo::WrapperAuditLog::createTable( db.db_ ) )
			{
				return false;
			}

		}

		// v3 ==> v4: wizards table only (added wizard)
		// v4 ==> v5: wizards table only (added wizard)
		// v5 ==> v6: wizards table changed to support parameterized forced wizards
		// v6 ==> v7: added deviceCycles column to eventlog table
		else if ( 6 == ver )
		{
			s = "ALTER TABLE eventlog ADD deviceCycles integer;";
			result = sqlite3_prepare_v2( db.db_, s.c_str(), (int) s.length(), &stmt, NULL );

			if (( SQLITE_OK != result ) || ( !stmt ))
			{
				TRACE( "error during attach prepare: " + util_string::longtostring( result ) );
				goto error_handler;
			}
			else if ( SQLITE_DONE != (result = sqlite3_step( stmt )) )
			{
				TRACE( "error attach: " + util_string::longtostring( result ) );
				goto error_handler;
			}

			// finalize
			sqlite3_finalize( stmt );

			s = "UPDATE eventlog SET deviceCycles=0;";
			result = sqlite3_prepare_v2( db.db_, s.c_str(), (int) s.length(), &stmt, NULL );

			if (( SQLITE_OK != result ) || ( !stmt ))
			{
				TRACE( "error during attach prepare: " + util_string::longtostring( result ) );
				goto error_handler;
			}
			else if ( SQLITE_DONE != (result = sqlite3_step( stmt )) )
			{
				TRACE( "error attach: " + util_string::longtostring( result ) );
				goto error_handler;
			}

			// finalize
			sqlite3_finalize( stmt );
		}

		// v7 ==> v8: wizards table only (changed error severity)
		// v8 ==> v9: added tables for printer head dot wear tracking
		else if ( 8 == ver )
		{
			// these tables were added in v9
			if (( !dbo::PrinterDotWear::createTable( db.db_ ) ) ||
				( !dbo::PrinterHeadStatus::createTable( db.db_ ) ))
			{
				return false;
			}

		}

		// v9 ==> v10: added table for printer head dot usage tracking
		else if ( 9 == ver )
		{
			// these tables were added in v10
			if ( !dbo::PrinterDotUsage::createTable( db.db_ ) )
			{
				return false;
			}

		}

		// v10 ==> v11: added table for value max
		else if ( 10 == ver )
		{
			// these tables were added in v11
			if ( !dbo::ValueMax::createTable( db.db_ ) )
			{
				return false;
			}

		}

	}

	// attach new database to old database
	s = "ATTACH DATABASE '" + filename_ + "' AS newdb;";
	result = sqlite3_prepare_v2( db.db_, s.c_str(), (int) s.length(), &stmt, NULL );

	if (( SQLITE_OK != result ) || ( !stmt ))
	{
		TRACE( "error during attach prepare: " + util_string::longtostring( result ) );
		goto error_handler;
	}
	else if ( SQLITE_DONE != (result = sqlite3_step( stmt )) )
	{
		TRACE( "error attach: " + util_string::longtostring( result ) );
		goto error_handler;
	}

	// finalize
	sqlite3_finalize( stmt );

	// export upgraded tables
	if (( !dbo::Diag::exportTable( db.db_ ) ) ||
		( !dbo::DiagStep::exportTable( db.db_ ) ) ||
		( !dbo::EventLog::exportTable( db.db_ ) ) ||
		( !dbo::ServiceMaintLog::exportTable( db.db_ ) ) ||
		( !dbo::WrapperAuditLog::exportTable( db.db_ ) ) ||
		( !dbo::WrapperTimings::exportTable( db.db_ ) ) ||
		( !dbo::WrapperTrays::exportTable( db.db_ ) ))
	{
		TRACE( "failed to export one or more tables!" );
		return false;
	}

	return true;

error_handler:

	if ( stmt )
		sqlite3_finalize( stmt );

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DiagnosticsDB::housekeeping()
{
	dbo::Config config( db_ );

	// retrieve housekeeping age
	int age( config.housekeeping() );

	if ( !age )
		age = 180;

	TRACE( "performing housekeeping, keeping " + util_string::longtostring( age ) + " days worth" );

	// compute timestamp for query
	util::Timestamp timestamp( util::Timestamp::now() );
	timestamp.addDays( (age *= -1) );

	TRACE( "housekeeping stamp " + timestamp.toString() );

	// cull database
	bool result( true );
	result &= dbo::Diag::housekeeping( db_, timestamp );
	result &= dbo::DiagStep::housekeeping( db_, timestamp );
	result &= dbo::EventLog::housekeeping( db_, timestamp );
	result &= dbo::PrinterDotUsage::housekeeping( db_, timestamp );
	result &= dbo::PrinterDotWear::housekeeping( db_, timestamp );
	result &= dbo::PrinterHeadStatus::housekeeping( db_, timestamp );
	result &= dbo::ValueMax::housekeeping( db_, timestamp );
	result &= dbo::WrapperAuditLog::housekeeping( db_, timestamp );

	return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
std::string DiagnosticsDB::dbFile()
{
	return Configuration::getInstance()->toStringCfgI( CfgInternal::DIAGNOSTICSDBNAME );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
std::string DiagnosticsDB::dbBackupFile()
{
	return Configuration::getInstance()->toStringCfgI( CfgInternal::DIAGNOSTICSDBBACKUPNAME );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
std::string DiagnosticsDB::dbOldFile()
{
	return Configuration::getInstance()->toStringCfgI( CfgInternal::DIAGNOSTICSDBOLDNAME );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DiagnosticsDB::initialize()
{
	sqlite3_stmt *stmt( NULL );
	std::string s;

	int flags( SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE );
	int result;

	TRACE( "sqlite3 version: " SQLITE_VERSION );

	// open the db
	result = sqlite3_open_v2( filename_.c_str(), &db_, flags, NULL );

	if (( SQLITE_OK != result ) || ( !db_ ))
	{
		TRACE( "error opening db: " + util_string::longtostring( result ) );
		goto error_handler;
	}

	// retrieve version
	s = "SELECT name FROM sqlite_master WHERE type='table' AND name='config';";
	result = sqlite3_prepare_v2( db_, s.c_str(), (int) s.length(), &stmt, NULL );

	if (( SQLITE_OK != result ) || ( !stmt ))
	{
		TRACE( "error during prepare: " + util_string::longtostring( result ) );
		goto error_handler;
	}

	// step
	result = sqlite3_step( stmt );

	if ( SQLITE_ROW != result )
	{
		TRACE( "new database detected!" );

		if ( !createDatabase() )
		{
			TRACE( "failed to create database!!" );
			goto error_handler;
		}
	}

	// finalize
	sqlite3_finalize( stmt );

	// grab version
	updateVersionInfo();

	// abort any unfinished steps lurking in the db
	abortUnfinishedEventSteps();

	// setup wrapper timing parameters
	timingParameters_[0]  = "   wrap tension side to side";
	timingParameters_[1]  = "           knife fire height";
	timingParameters_[2]  = "                  clamp open";
	timingParameters_[3]  = "package position on elevator";
	timingParameters_[4]  = "             add tray length";
	timingParameters_[5]  = "  wrap tension front to back";
	timingParameters_[6]  = "front underfolder adjustment";
	timingParameters_[7]  = "                  wrap speed";
	timingParameters_[8]  = "             discharge speed";
	timingParameters_[9]  = "          side folder travel";
	timingParameters_[10] = "             height override";
	timingParameters_[11] = "side underfolder return home";
	timingParameters_[12] = "    gripper allow short pull";
	timingParameters_[13] = "              side clamp gap";
	timingParameters_[14] = "            film roll to use";
	timingParameters_[15] = " rear underfolder trip delay";
	timingParameters_[16] = "            film pull length";
	timingParameters_[17] = "     outside gripper release";
	timingParameters_[18] = "      inside gripper release";

	return;

error_handler:

	if ( stmt )
		sqlite3_finalize( stmt );

	finalize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DiagnosticsDB::finalize()
{
	if ( db_ )
	{
		TRACE( "closing db..." );
		int result( sqlite3_close_v2( db_ ) );

		if ( SQLITE_OK != result )
			TRACE( "error closing db: " + util_string::longtostring( result ) );

		db_ = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool DiagnosticsDB::createDatabase()
{
	bool result( true );

	result &= dbo::Config::createTable( db_ );
	result &= dbo::Diag::createTable( db_ );
	result &= dbo::DiagStep::createTable( db_ );
	result &= dbo::EventLog::createTable( db_ );
	result &= dbo::PrinterDotUsage::createTable( db_ );
	result &= dbo::PrinterDotWear::createTable( db_ );
	result &= dbo::PrinterHeadStatus::createTable( db_ );
	result &= dbo::ServiceAction::createTable( db_ );
	result &= dbo::ServiceCat::createTable( db_ );
	result &= dbo::ServiceMaintLog::createTable( db_ );
	result &= dbo::ServiceMethod::createTable( db_ );
	result &= dbo::ServicePart::createTable( db_ );
	result &= dbo::ValueMax::createTable( db_ );
	result &= dbo::Wizards::createTable( db_ );
	result &= dbo::WrapperAuditLog::createTable( db_ );
	result &= dbo::WrapperTimings::createTable( db_ );
	result &= dbo::WrapperTrays::createTable( db_ );

	return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DiagnosticsDB::updateVersionInfo()
{
	if ( db_ )
	{
		dbo::Config config( db_ );

		TRACE( "db version: " + util_string::longtostring( (version_ = config.version()) ) );
		TRACE( "created: " + util_string::longlongtostring( config.created() ) );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DiagnosticsDB::abortUnfinishedEventSteps()
{
	if ( db_ )
	{
		dbo::DiagStep diagStepTable( db_ );

		if ( !diagStepTable.abortUnfinishedSteps() )
			TRACE( "failed to abort unfinished steps" );
	}
}
