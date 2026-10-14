/**
 * @file DiagnosticsServiceMaintLog.h
 * @brief Diagnostics service maint log message.
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef DIAGNOSTICSSERVICEMAINTLOG_H
#define DIAGNOSTICSSERVICEMAINTLOG_H

#include "DiagnosticsMessage.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Diagnostics service maint log request message.
class DiagnosticsServiceMaintLogReq : public DiagnosticsMessage
{
	typedef DiagnosticsServiceMaintLogReq _Myt;
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
	DiagnosticsServiceMaintLogReq( Hobart::CEvolMsgReceiver *thread = NULL, unsigned int seqNumber = 0 ) :
		_Mybase( DiagServiceMaintLogReq, thread, seqNumber )
	{
		initialize();
	}

	/// Constructor.
	/**
	 * @param[in] buffer  message buffer
	 */
	DiagnosticsServiceMaintLogReq( const char *buffer ) :
		_Mybase( DiagServiceMaintLogReq )
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
	DiagnosticsServiceMaintLogReq( const char *buffer, size_t bufferLen, int format ) :
		_Mybase( DiagServiceMaintLogReq )
	{
		initialize();
		deserialize( buffer, bufferLen, format );
	}

	/// Destructor.
	virtual ~DiagnosticsServiceMaintLogReq() {}

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

/// Diagnostics service maint log message.
class DiagnosticsServiceMaintLog : public DiagnosticsMessage
{
	typedef DiagnosticsServiceMaintLog _Myt;
	typedef DiagnosticsMessage _Mybase;

public:

	int count_;

	std::string maintRecords_;

	// ========================================================================
	// CTOR / DTOR
	// ========================================================================

	/// Constructor.
	/**
	 * @param[in] thread  receiver thread
	 * @param[in] seqNumber  sequence number
	 */
	DiagnosticsServiceMaintLog( Hobart::CEvolMsgReceiver *thread = NULL, unsigned int seqNumber = 0 ) :
		_Mybase( DiagServiceMaintLog, thread, seqNumber )
	{
		initialize();
	}

	/// Constructor.
	/**
	 * @param[in] buffer  message buffer
	 */
	DiagnosticsServiceMaintLog( const char *buffer ) :
		_Mybase( DiagServiceMaintLog )
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
	DiagnosticsServiceMaintLog( const char *buffer, size_t bufferLen, int format ) :
		_Mybase( DiagServiceMaintLog )
	{
		initialize();
		deserialize( buffer, bufferLen, format );
	}

	/// Destructor.
	virtual ~DiagnosticsServiceMaintLog() {}

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

		APPEND_FIELD( count_, 0 )
		APPEND_FIELD( maintRecords_, std::string() )
	}

private:

	/// Initialize.
	void initialize()
	{
		SET_FIELDS
	}

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // DIAGNOSTICSSERVICEMAINTLOG_H
