/**
 * @file DiagnosticsSendDatabase.h
 * @brief Diagnostics send database message.
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef DIAGNOSTICSSENDDATABASE_H
#define DIAGNOSTICSSENDDATABASE_H

#include "DiagnosticsMessageBasic.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Diagnostics messsage to email database.
typedef DiagnosticsMessageBasic<DiagSendDatabaseEmail> DiagnosticsSendDatabaseEmail;

/// Diagnostics messsage to send database through gateway device.
typedef DiagnosticsMessageBasic<DiagSendDatabaseGatewayDevice> DiagnosticsSendDatabaseGatewayDevice;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Diagnostics send database complate message.
class DiagnosticsSendDatabaseComplete : public DiagnosticsMessage
{
	typedef DiagnosticsSendDatabaseComplete _Myt;
	typedef DiagnosticsMessage _Mybase;

public:

	bool success_;

	// ========================================================================
	// CTOR / DTOR
	// ========================================================================

	/// Constructor.
	/**
	 * @param[in] thread  receiver thread
	 * @param[in] seqNumber  sequence number
	 */
	DiagnosticsSendDatabaseComplete( Hobart::CEvolMsgReceiver *thread = NULL, unsigned int seqNumber = 0 ) :
		_Mybase( DiagSendDatabaseComplete, thread, seqNumber )
	{
		initialize();
	}

	/// Constructor.
	/**
	 * @param[in] buffer  message buffer
	 */
	DiagnosticsSendDatabaseComplete( const char *buffer ) :
		_Mybase( DiagSendDatabaseComplete )
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
	DiagnosticsSendDatabaseComplete( const char *buffer, size_t bufferLen, int format ) :
		_Mybase( DiagSendDatabaseComplete )
	{
		initialize();
		deserialize( buffer, bufferLen, format );
	}

	/// Destructor.
	virtual ~DiagnosticsSendDatabaseComplete() {}

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

		APPEND_FIELD( success_, false )
	}

private:

	/// Initialize.
	void initialize()
	{
		SET_FIELDS
	}

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // DIAGNOSTICSSENDDATABASE_H
