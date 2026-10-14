#ifndef REPORTEVENT_H
#define REPORTEVENT_H



// This file contains all the events that occur in the Evolution Wrapper Project
#include "Event.h"


// Events used for the ReportManagerDev
typedef enum ReportEventType
{
	StartReport,            // tells the ReportManager to start a specific report
	CancelReport,           // tells the ReportManager to cancel operation
	TerminateReport,        // tells the session to destroy the ReportManager
	UnknownReportEvent
} ReportEventType_t;


typedef enum ReportErrors
{
	ReportErrUnknownError = ReportManagerDev * DEVICE_ERROR_OFFSET
} ReportErrors_t;


typedef enum ReportType
{
	NetworkErrorsReportType,
	ConfigurationReportType,
	GenericTextReportType,

	WrapperTotalsReportType = 10,
	WrapperSettingsReportType,
	RunTotalsReportType,			// cut test report

	WeigherAuditTrailType,			// cat3 weigher support
	
	OperatorChangesReportType,		// Operator Changes Report 
	SupervisorChangesReportType,	// Supervisor Changes Report

	UnknownReportType
} ReportType_t;



#endif // REPORTEVENT_H

