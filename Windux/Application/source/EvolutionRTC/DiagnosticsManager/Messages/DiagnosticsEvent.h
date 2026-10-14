/**
 * @file DiagnosticsEvent.h
 * @brief Diagnostic event types.
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef DIAGNOSTICSEVENT_H
#define DIAGNOSTICSEVENT_H

#include "Event.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Diagnostics events.
typedef enum DiagEvent
{
	// configuration messages
	DiagConfigurationReq = 32,
	DiagConfiguration,

	// status messages
	DiagStatusReq,
	DiagStatus,

	// retrieve audit log records
	DiagAuditLogReq = 48,
	DiagAuditLog,

	// retrieve audit record
	DiagAuditLogRecordReq,
	DiagAuditLogRecord,

	// record service visit to audit log
	DiagAuditLogServiceVisitStart,
	DiagAuditLogServiceVisitStop,
	DiagAuditLogServiceVisitStatus,

	// check for diagnostics error handler
	DiagCheckErrorHandlerReq = 64,
	DiagCheckErrorHandler,

	// start diagnostics
	DiagStartErrorHandler,
	DiagStartErrorHandlerFromMaintRecord,

	DiagErrorHandlerFailed,

	// show diagnostic wizard
	DiagShowWizard = 80,

	// initialize diagnostic wizard
	DiagInitWizardReq,
	DiagInitWizard,

	// perform diagnostics
	DiagPerformWizardTestingReq,
	DiagPerformWizardTesting,

	// exit diagnostic wizard
	DiagExitWizard,

	// retrieve service maintenance records
	DiagServiceMaintLogReq = 96,
	DiagServiceMaintLog,

	// clear service maintenance log record
	DiagClearServiceMaintRecord,

	// print service maintenance log
	DiagPrintServiceMaintLog,

	// retrieve service actions
	DiagServiceActionsReq,
	DiagServiceActions,

	// export options for diagnostics
	DiagExportDatabase = 112,
	DiagExportDatabaseComplete,

	// send options for diagnostics
	DiagSendDatabaseEmail,
	DiagSendDatabaseGatewayDevice,
	DiagSendDatabaseComplete,

	// printer statistics
	DiagPrintLabelStats = 144,

	// value max savings
	DiagValueMax = 152,

	// reporting
	DiagReport = 224


} DiagEvent_t;

/// Diagnostics errors.
typedef enum DiagnosticsErrors
{
	DiagnosticsErrUnknownError = Diagnostics * DEVICE_ERROR_OFFSET,
} DiagnosticsErrors_t;

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // DIAGNOSTICSEVENT_H
