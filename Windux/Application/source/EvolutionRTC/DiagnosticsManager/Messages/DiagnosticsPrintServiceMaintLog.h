/**
 * @file DiagnosticsPrintServiceMaintLog.h
 * @brief Diagnostics print service maint log message.
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef DIAGNOSTICSPRINTSERVICEMAINTLOG_H
#define DIAGNOSTICSPRINTSERVICEMAINTLOG_H

#include "DiagnosticsMessage.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Diagnostics print service maint log message.
class DiagnosticsPrintServiceMaintLog : public DiagnosticsMessage
{
	typedef DiagnosticsPrintServiceMaintLog _Myt;
	typedef DiagnosticsMessage _Mybase;

public:

	/// Filter options.
	enum Filter
	{
		OpenRecordsOnly = 0,						///< Only open log records.
		ResolvedRecordsOnly,						///< Only resolved log records.
		AllRecords									///< Pull all records.
	};

	int filter_;									///< Filter option (from above)
	int age_;										///< Record age in days (0 for all)

	// ========================================================================
	// CTOR / DTOR
	// ========================================================================

	/// Constructor.
	/**
	 * @param[in] thread  receiver thread
	 * @param[in] seqNumber  sequence number
	 */
	DiagnosticsPrintServiceMaintLog( Hobart::CEvolMsgReceiver *thread = NULL, unsigned int seqNumber = 0 ) :
		_Mybase( DiagPrintServiceMaintLog, thread, seqNumber )
	{
		initialize();
	}

	/// Constructor.
	/**
	 * @param[in] buffer  message buffer
	 */
	DiagnosticsPrintServiceMaintLog( const char *buffer ) :
		_Mybase( DiagPrintServiceMaintLog )
	{
		initialize();
		deserialize( buffer );
	}

	/// Constructor.
	/**
	 * @param[in] buffer  message buffer
	 * @param[in] bufferLen  message buffer length
	 * @param[in] format  buffer format
	 */
	DiagnosticsPrintServiceMaintLog( const char *buffer, size_t bufferLen, int format ) :
		_Mybase( DiagPrintServiceMaintLog )
	{
		initialize();
		deserialize( buffer, bufferLen, format );
	}

	/// Destructor.
	virtual ~DiagnosticsPrintServiceMaintLog() {}

protected:

	// ========================================================================
	// Methods
	// ========================================================================

	/// Append message specific fields.
	/**
	 * @param[in,out] fields  message fields
	 */
	virtual void appendFields( fieldVector &fields )
	{
		_Mybase::appendFields( fields );

		APPEND_FIELD( filter_, OpenRecordsOnly )
		APPEND_FIELD( age_, 0 )
	}

private:

	/// Initialize.
	void initialize()
	{
		SET_FIELDS
	}

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // DIAGNOSTICSPRINTSERVICEMAINTLOG_H
