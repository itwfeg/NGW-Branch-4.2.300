/**
 * @file DiagnosticsStartErrorHandler.h
 * @brief Diagnostics start error handler message.
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef DIAGNOSTICSSTARTERRORHANDLER_H
#define DIAGNOSTICSSTARTERRORHANDLER_H

#include "DiagnosticsMessage.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Diagnostics start error handler message.
class DiagnosticsStartErrorHandler : public DiagnosticsMessage
{
	typedef DiagnosticsStartErrorHandler _Myt;
	typedef DiagnosticsMessage _Mybase;

public:

	std::string event_;
	bool auto_;

	// ========================================================================
	// CTOR / DTOR
	// ========================================================================

	/// Constructor.
	/**
	 * @param[in] thread  receiver thread
	 * @param[in] seqNumber  sequence number
	 */
	DiagnosticsStartErrorHandler( Hobart::CEvolMsgReceiver *thread = NULL, unsigned int seqNumber = 0 ) :
		_Mybase( DiagStartErrorHandler, thread, seqNumber )
	{
		initialize();
	}

	/// Constructor.
	/**
	 * @param[in] buffer  message buffer
	 */
	DiagnosticsStartErrorHandler( const char *buffer ) :
		_Mybase( DiagStartErrorHandler )
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
	DiagnosticsStartErrorHandler( const char *buffer, size_t bufferLen, int format ) :
		_Mybase( DiagStartErrorHandler )
	{
		initialize();
		deserialize( buffer, bufferLen, format );
	}

	/// Destructor.
	virtual ~DiagnosticsStartErrorHandler() {}

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

		APPEND_FIELD( event_, std::string() )
		APPEND_FIELD( auto_, false )
	}

private:

	/// Initialize.
	void initialize()
	{
		SET_FIELDS
	}

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Diagnostics start error handler from maint record message.
class DiagnosticsStartErrorHandlerFromMaintRecord : public DiagnosticsMessage
{
	typedef DiagnosticsStartErrorHandlerFromMaintRecord _Myt;
	typedef DiagnosticsMessage _Mybase;

public:

	std::string maintRecord_;
	bool service_;

	// ========================================================================
	// CTOR / DTOR
	// ========================================================================

	/// Constructor.
	/**
	 * @param[in] thread  receiver thread
	 * @param[in] seqNumber  sequence number
	 */
	DiagnosticsStartErrorHandlerFromMaintRecord( Hobart::CEvolMsgReceiver *thread = NULL, unsigned int seqNumber = 0 ) :
		_Mybase( DiagStartErrorHandlerFromMaintRecord, thread, seqNumber )
	{
		initialize();
	}

	/// Constructor.
	/**
	 * @param[in] buffer  message buffer
	 */
	DiagnosticsStartErrorHandlerFromMaintRecord( const char *buffer ) :
		_Mybase( DiagStartErrorHandlerFromMaintRecord )
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
	DiagnosticsStartErrorHandlerFromMaintRecord( const char *buffer, size_t bufferLen, int format ) :
		_Mybase( DiagStartErrorHandlerFromMaintRecord )
	{
		initialize();
		deserialize( buffer, bufferLen, format );
	}

	/// Destructor.
	virtual ~DiagnosticsStartErrorHandlerFromMaintRecord() {}

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

		APPEND_FIELD( maintRecord_, std::string() )
		APPEND_FIELD( service_, false )
	}

private:

	/// Initialize.
	void initialize()
	{
		SET_FIELDS
	}

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // DIAGNOSTICSSTARTERRORHANDLER_H
