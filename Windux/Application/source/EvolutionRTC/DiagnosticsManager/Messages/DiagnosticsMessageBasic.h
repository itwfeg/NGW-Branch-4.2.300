/**
 * @file DiagnosticsMessageBasic.h
 * @brief Diagnostics message with no parameters.
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef DIAGNOSTICSMESSAGEBASIC_H
#define DIAGNOSTICSMESSAGEBASIC_H

#include "DiagnosticsMessage.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Diagnostics check error handler message.
template <DiagEvent_t _Ev>
class DiagnosticsMessageBasic : public DiagnosticsMessage
{
	typedef DiagnosticsMessageBasic<_Ev> _Myt;
	typedef DiagnosticsMessage _Mybase;

public:

	// ========================================================================
	// CTOR / DTOR
	// ========================================================================

	/// Constructor.
	/**
	 * @param[in] thread  receiver thread
	 * @param[in] seqNumber  sequence number
	 */
	DiagnosticsMessageBasic( Hobart::CEvolMsgReceiver *thread = NULL, unsigned int seqNumber = 0 ) :
		_Mybase( _Ev, thread, seqNumber )
	{
	}

	/// Constructor.
	/**
	 * @param[in] buffer  message buffer
	 */
	DiagnosticsMessageBasic( const char *buffer ) :
		_Mybase( _Ev )
	{
		deserialize( buffer );
	}

	/// Constructor.
	/**
	 * @param[in] buffer  message buffer
	 * @param[in] bufferLen  message buffer length
	 * @param[in] format  buffer format
	 */
	DiagnosticsMessageBasic( const char *buffer, size_t bufferLen, int format ) :
		_Mybase( _Ev )
	{
		deserialize( buffer, bufferLen, format );
	}

	/// Destructor.
	virtual ~DiagnosticsMessageBasic() {}

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Diagnostics error handler failed message.
typedef DiagnosticsMessageBasic<DiagErrorHandlerFailed> DiagnosticsErrorHandlerFailed;

/// Diagnostics exit wizard message.
typedef DiagnosticsMessageBasic<DiagExitWizard> DiagnosticsExitWizard;

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // DIAGNOSTICSMESSAGEBASIC_H
