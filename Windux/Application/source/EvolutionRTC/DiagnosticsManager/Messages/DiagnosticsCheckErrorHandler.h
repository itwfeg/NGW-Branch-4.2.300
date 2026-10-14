/**
 * @file DiagnosticsCheckErrorHandler.h
 * @brief Diagnostics check error handler message.
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef DIAGNOSTICSCHECKERRORHANDLER_H
#define DIAGNOSTICSCHECKERRORHANDLER_H

#include "DiagnosticsMessage.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Diagnostics check error handler message.
class DiagnosticsCheckErrorHandlerReq : public DiagnosticsMessage
{
	typedef DiagnosticsCheckErrorHandlerReq _Myt;
	typedef DiagnosticsMessage _Mybase;

public:

	int device_;
	int errorCode_;

	// ========================================================================
	// CTOR / DTOR
	// ========================================================================

	/// Constructor.
	/**
	 * @param[in] thread  receiver thread
	 * @param[in] seqNumber  sequence number
	 */
	DiagnosticsCheckErrorHandlerReq( Hobart::CEvolMsgReceiver *thread = NULL, unsigned int seqNumber = 0 ) :
		_Mybase( DiagCheckErrorHandlerReq, thread, seqNumber )
	{
		initialize();
	}

	/// Constructor.
	/**
	 * @param[in] buffer  message buffer
	 */
	DiagnosticsCheckErrorHandlerReq( const char *buffer ) :
		_Mybase( DiagCheckErrorHandlerReq )
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
	DiagnosticsCheckErrorHandlerReq( const char *buffer, size_t bufferLen, int format ) :
		_Mybase( DiagCheckErrorHandlerReq )
	{
		initialize();
		deserialize( buffer, bufferLen, format );
	}

	/// Destructor.
	virtual ~DiagnosticsCheckErrorHandlerReq() {}

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
	DiagnosticsCheckErrorHandlerReq( DiagEvent event, Hobart::CEvolMsgReceiver *thread = NULL, unsigned int seqNumber = 0 ) :
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

		APPEND_FIELD( device_, 0 )
		APPEND_FIELD( errorCode_, 0 )
	}

private:

	/// Initialize.
	void initialize()
	{
		SET_FIELDS
	}

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Diagnostics check error handler message.
class DiagnosticsCheckErrorHandler : public DiagnosticsCheckErrorHandlerReq
{
	typedef DiagnosticsCheckErrorHandler _Myt;
	typedef DiagnosticsCheckErrorHandlerReq _Mybase;

public:

	bool available_;
	bool force_;

	std::string event_;

	// ========================================================================
	// CTOR / DTOR
	// ========================================================================

	/// Constructor.
	/**
	 * @param[in] thread  receiver thread
	 * @param[in] seqNumber  sequence number
	 */
	DiagnosticsCheckErrorHandler( Hobart::CEvolMsgReceiver *thread = NULL, unsigned int seqNumber = 0 ) :
		_Mybase( DiagCheckErrorHandler, thread, seqNumber )
	{
		initialize();
	}

	/// Constructor.
	/**
	 * @param[in] buffer  message buffer
	 */
	DiagnosticsCheckErrorHandler( const char *buffer ) :
		_Mybase( DiagCheckErrorHandler )
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
	DiagnosticsCheckErrorHandler( const char *buffer, size_t bufferLen, int format ) :
		_Mybase( DiagCheckErrorHandler )
	{
		initialize();
		deserialize( buffer, bufferLen, format );
	}

	/// Destructor.
	virtual ~DiagnosticsCheckErrorHandler() {}

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

		APPEND_FIELD( available_, false )
		APPEND_FIELD( force_, false )
		APPEND_FIELD( event_, std::string() )
	}

private:

	/// Initialize.
	void initialize()
	{
		SET_FIELDS
	}

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // DIAGNOSTICSCHECKERRORHANDLER_H
