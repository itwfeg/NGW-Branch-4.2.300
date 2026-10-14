/**
 * @file DiagnosticsAuditLog.h
 * @brief Diagnostics audit log message.
 *
 * @copyright Copyright (c) 2017, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef DIAGNOSTICSAUDITLOG_H
#define DIAGNOSTICSAUDITLOG_H

#include "DiagnosticsMessage.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Diagnostics audit log request message.
class DiagnosticsAuditLogReq : public DiagnosticsMessage
{
	typedef DiagnosticsAuditLogReq _Myt;
	typedef DiagnosticsMessage _Mybase;

public:

	/// Filter options.
	enum Filter
	{
		UnknownEventsOnly = 0,						///< Unknown source.
		TimingsEventsOnly,							///< Timings event.
		FilmEventsOnly,								///< Film event.

		AllRecords = -1								///< Pull all records.
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
	DiagnosticsAuditLogReq( Hobart::CEvolMsgReceiver *thread = NULL, unsigned int seqNumber = 0 ) :
		_Mybase( DiagAuditLogReq, thread, seqNumber )
	{
		initialize();
	}

	/// Constructor.
	/**
	 * @param[in] buffer  message buffer
	 */
	DiagnosticsAuditLogReq( const char *buffer ) :
		_Mybase( DiagAuditLogReq )
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
	DiagnosticsAuditLogReq( const char *buffer, size_t bufferLen, int format ) :
		_Mybase( DiagAuditLogReq )
	{
		initialize();
		deserialize( buffer, bufferLen, format );
	}

	/// Destructor.
	virtual ~DiagnosticsAuditLogReq() {}

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

		APPEND_FIELD( filter_, AllRecords )
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

/// Diagnostics audit log message.
class DiagnosticsAuditLog : public DiagnosticsMessage
{
	typedef DiagnosticsAuditLog _Myt;
	typedef DiagnosticsMessage _Mybase;

public:

	int count_;

	std::string records_;

	// ========================================================================
	// CTOR / DTOR
	// ========================================================================

	/// Constructor.
	/**
	 * @param[in] thread  receiver thread
	 * @param[in] seqNumber  sequence number
	 */
	DiagnosticsAuditLog( Hobart::CEvolMsgReceiver *thread = NULL, unsigned int seqNumber = 0 ) :
		_Mybase( DiagAuditLog, thread, seqNumber )
	{
		initialize();
	}

	/// Constructor.
	/**
	 * @param[in] buffer  message buffer
	 */
	DiagnosticsAuditLog( const char *buffer ) :
		_Mybase( DiagAuditLog )
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
	DiagnosticsAuditLog( const char *buffer, size_t bufferLen, int format ) :
		_Mybase( DiagAuditLog )
	{
		initialize();
		deserialize( buffer, bufferLen, format );
	}

	/// Destructor.
	virtual ~DiagnosticsAuditLog() {}

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
		APPEND_FIELD( records_, std::string() )
	}

private:

	/// Initialize.
	void initialize()
	{
		SET_FIELDS
	}

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Diagnostics audit log record request.
class DiagnosticsAuditLogRecordReq : public DiagnosticsMessage
{
	typedef DiagnosticsAuditLogRecordReq _Myt;
	typedef DiagnosticsMessage _Mybase;

public:

	std::string timestamp_;

	// ========================================================================
	// CTOR / DTOR
	// ========================================================================

	/// Constructor.
	/**
	 * @param[in] thread  receiver thread
	 * @param[in] seqNumber  sequence number
	 */
	DiagnosticsAuditLogRecordReq( Hobart::CEvolMsgReceiver *thread = NULL, unsigned int seqNumber = 0 ) :
		_Mybase( DiagAuditLogRecordReq, thread, seqNumber )
	{
		initialize();
	}

	/// Constructor.
	/**
	 * @param[in] buffer  message buffer
	 */
	DiagnosticsAuditLogRecordReq( const char *buffer ) :
		_Mybase( DiagAuditLogRecordReq )
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
	DiagnosticsAuditLogRecordReq( const char *buffer, size_t bufferLen, int format ) :
		_Mybase( DiagAuditLogRecordReq )
	{
		initialize();
		deserialize( buffer, bufferLen, format );
	}

	/// Destructor.
	virtual ~DiagnosticsAuditLogRecordReq() {}

protected:

	// ========================================================================
	// CTOR
	// ========================================================================

	/// Constructor.
	/**
	 * @param[in] event  event type
	 * @param[in] thread  receiver thread
	 * @param[in] seqNumber  sequence number
	 */
	DiagnosticsAuditLogRecordReq( DiagEvent event, Hobart::CEvolMsgReceiver *thread = NULL, unsigned int seqNumber = 0 ) :
		_Mybase( event, thread, seqNumber )
	{
	}

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

		APPEND_FIELD( timestamp_, std::string() )
	}

private:

	/// Initialize.
	void initialize()
	{
		SET_FIELDS
	}

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Diagnostics audit log record.
class DiagnosticsAuditLogRecord : public DiagnosticsAuditLogRecordReq
{
	typedef DiagnosticsAuditLogRecord _Myt;
	typedef DiagnosticsAuditLogRecordReq _Mybase;

public:

	int severity;									///< Event severity.

	int source;										///< Source type.
	int sourceExt;									///< Source extended.

	std::string description;						///< Event description.

	// ========================================================================
	// CTOR / DTOR
	// ========================================================================

	/// Constructor.
	/**
	 * @param[in] thread  receiver thread
	 * @param[in] seqNumber  sequence number
	 */
	DiagnosticsAuditLogRecord( Hobart::CEvolMsgReceiver *thread = NULL, unsigned int seqNumber = 0 ) :
		_Mybase( DiagAuditLogRecord, thread, seqNumber )
	{
		initialize();
	}

	/// Constructor.
	/**
	 * @param[in] buffer  message buffer
	 */
	DiagnosticsAuditLogRecord( const char *buffer ) :
		_Mybase( DiagAuditLogRecord )
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
	DiagnosticsAuditLogRecord( const char *buffer, size_t bufferLen, int format ) :
		_Mybase( DiagAuditLogRecord )
	{
		initialize();
		deserialize( buffer, bufferLen, format );
	}

	/// Destructor.
	virtual ~DiagnosticsAuditLogRecord() {}

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

		APPEND_FIELD( severity, 0 )

		APPEND_FIELD( source, 0 )
		APPEND_FIELD( sourceExt, 0 )

		APPEND_FIELD( description, std::string() )
	}

private:

	/// Initialize.
	void initialize()
	{
		SET_FIELDS
	}

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // DIAGNOSTICSAUDITLOG_H
