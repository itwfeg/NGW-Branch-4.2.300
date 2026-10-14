/**
 * @file DiagnosticsAuditLogServiceVisit.h
 * @brief Diagnostics audit log service visit message.
 *
 * @copyright Copyright (c) 2017, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef DIAGNOSTICSAUDITLOGSERVICEVISIT_H
#define DIAGNOSTICSAUDITLOGSERVICEVISIT_H

#include "DiagnosticsMessage.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Diagnostics audit log service visit start message.
class DiagnosticsAuditLogServiceVisitStart : public DiagnosticsMessage
{
	typedef DiagnosticsAuditLogServiceVisitStart _Myt;
	typedef DiagnosticsMessage _Mybase;

public:

	std::string description_;						///< Visit description.

	// ========================================================================
	// CTOR / DTOR
	// ========================================================================

	/// Constructor.
	/**
	 * @param[in] thread  receiver thread
	 * @param[in] seqNumber  sequence number
	 */
	DiagnosticsAuditLogServiceVisitStart( Hobart::CEvolMsgReceiver *thread = NULL, unsigned int seqNumber = 0 ) :
		_Mybase( DiagAuditLogServiceVisitStart, thread, seqNumber )
	{
		initialize();
	}

	/// Constructor.
	/**
	 * @param[in] buffer  message buffer
	 */
	DiagnosticsAuditLogServiceVisitStart( const char *buffer ) :
		_Mybase( DiagAuditLogServiceVisitStart )
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
	DiagnosticsAuditLogServiceVisitStart( const char *buffer, size_t bufferLen, int format ) :
		_Mybase( DiagAuditLogServiceVisitStart )
	{
		initialize();
		deserialize( buffer, bufferLen, format );
	}

	/// Destructor.
	virtual ~DiagnosticsAuditLogServiceVisitStart() {}

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

		APPEND_FIELD( description_, std::string() )
	}

private:

	/// Initialize.
	void initialize()
	{
		SET_FIELDS
	}

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Diagnostics audit log service visit stop message.
class DiagnosticsAuditLogServiceVisitStop : public DiagnosticsMessage
{
	typedef DiagnosticsAuditLogServiceVisitStop _Myt;
	typedef DiagnosticsMessage _Mybase;

public:

	std::string description_;						///< Visit description.

	// ========================================================================
	// CTOR / DTOR
	// ========================================================================

	/// Constructor.
	/**
	 * @param[in] thread  receiver thread
	 * @param[in] seqNumber  sequence number
	 */
	DiagnosticsAuditLogServiceVisitStop( Hobart::CEvolMsgReceiver *thread = NULL, unsigned int seqNumber = 0 ) :
		_Mybase( DiagAuditLogServiceVisitStop, thread, seqNumber )
	{
		initialize();
	}

	/// Constructor.
	/**
	 * @param[in] buffer  message buffer
	 */
	DiagnosticsAuditLogServiceVisitStop( const char *buffer ) :
		_Mybase( DiagAuditLogServiceVisitStop )
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
	DiagnosticsAuditLogServiceVisitStop( const char *buffer, size_t bufferLen, int format ) :
		_Mybase( DiagAuditLogServiceVisitStop )
	{
		initialize();
		deserialize( buffer, bufferLen, format );
	}

	/// Destructor.
	virtual ~DiagnosticsAuditLogServiceVisitStop() {}

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

		APPEND_FIELD( description_, std::string() )
	}

private:

	/// Initialize.
	void initialize()
	{
		SET_FIELDS
	}

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Diagnostics audit log service visit stop message.
class DiagnosticsAuditLogServiceVisitStatus : public DiagnosticsMessage
{
	typedef DiagnosticsAuditLogServiceVisitStatus _Myt;
	typedef DiagnosticsMessage _Mybase;

public:

	bool start_;									///< Start allowed.
	bool stop_;										///< Stop allowed.

	// ========================================================================
	// CTOR / DTOR
	// ========================================================================

	/// Constructor.
	/**
	 * @param[in] thread  receiver thread
	 * @param[in] seqNumber  sequence number
	 */
	DiagnosticsAuditLogServiceVisitStatus( Hobart::CEvolMsgReceiver *thread = NULL, unsigned int seqNumber = 0 ) :
		_Mybase( DiagAuditLogServiceVisitStatus, thread, seqNumber )
	{
		initialize();
	}

	/// Constructor.
	/**
	 * @param[in] buffer  message buffer
	 */
	DiagnosticsAuditLogServiceVisitStatus( const char *buffer ) :
		_Mybase( DiagAuditLogServiceVisitStatus )
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
	DiagnosticsAuditLogServiceVisitStatus( const char *buffer, size_t bufferLen, int format ) :
		_Mybase( DiagAuditLogServiceVisitStatus )
	{
		initialize();
		deserialize( buffer, bufferLen, format );
	}

	/// Destructor.
	virtual ~DiagnosticsAuditLogServiceVisitStatus() {}

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

		APPEND_FIELD( start_, false )
		APPEND_FIELD( stop_, false )
	}

private:

	/// Initialize.
	void initialize()
	{
		SET_FIELDS
	}

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // DIAGNOSTICSAUDITLOGSERVICEVISIT_H
