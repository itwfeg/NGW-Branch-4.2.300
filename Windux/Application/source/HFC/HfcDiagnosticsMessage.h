/**
 * @file HfcDiagnosticsMessage.h
 * @brief HFC wrapper class for diagnostic messages.
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef HFCDIAGNOSTICSMESSAGE_H
#define HFCDIAGNOSTICSMESSAGE_H

#include "HfcObject.h"

#include <DiagnosticsAuditLog.h>
#include <DiagnosticsAuditLogServiceVisit.h>
#include <DiagnosticsCheckErrorHandler.h>
#include <DiagnosticsClearServiceMaintRecord.h>
#include <DiagnosticsConfiguration.h>
#include <DiagnosticsExportDatabase.h>
#include <DiagnosticsInitWizard.h>
#include <DiagnosticsMessageBasic.h>
#include <DiagnosticsPerformWizardTesting.h>
#include <DiagnosticsPrintLabelStats.h>
#include <DiagnosticsPrintServiceMaintLog.h>
#include <DiagnosticsReport.h>
#include <DiagnosticsSendDatabase.h>
#include <DiagnosticsServiceActions.h>
#include <DiagnosticsServiceMaintLog.h>
#include <DiagnosticsShowWizard.h>
#include <DiagnosticsStartErrorHandler.h>
#include <DiagnosticsStatus.h>
#include <DiagnosticsValueMax.h>

///////////////////////////////////////////////////////////////////////////////////////////////////

/// HFC wrapper class for diagnostic messages.
template <class T>
class HfcDiagnosticsMessage : public HfcObject, public T
{
	typedef HfcDiagnosticsMessage<T> _Myt;
	typedef HfcObject _Mybase;

	typedef T _Myimpl;

public:

	// ========================================================================
	// CTOR / DTOR
	// ========================================================================

	/// Constructor.
	/**
	 * @param[in] thread  receiver thread
	 * @param[in] seqNumber  sequence number
	 */
	HfcDiagnosticsMessage( Hobart::CEvolMsgReceiver *thread = NULL, unsigned int seqNumber = 0 ) :
		_Mybase(),
		_Myimpl( thread, seqNumber )
	{

	}

	/// Constructor.
	/**
	 * @param[in] buffer  message buffer
	 */
	HfcDiagnosticsMessage( const char *buffer ) :
		_Mybase(),
		_Myimpl( buffer )
	{

	}

	/// Constructor.
	/**
	 * @param[in] buffer  message buffer
	 * @param[in] bufferLen  message buffer length
	 * @param[in] format  buffer format
	 */
	HfcDiagnosticsMessage( const char *buffer, size_t bufferLen, int format ) :
		_Mybase(),
		_Myimpl( buffer, bufferLen, format )
	{

	}

	/// Destructor.
	virtual ~HfcDiagnosticsMessage()
	{

	}

	// ========================================================================
	// Properties
	// ========================================================================

	/// Retrieve serialized data.
	/**
	 * @return  serialized data
	 */
	virtual const HobString &getSerializedData() const
	{
		return (_Mybase::serializedData = _Myimpl::serializedData);
	}

	/// Retrieve serialized length.
	/**
	 * @return  serialized length
	 */
	virtual UINT32 getSerializedLength() const
	{
		return _Myimpl::getSerializedLength();
	}

	// ========================================================================
	// Methods
	// ========================================================================

	/// Serialize message.
	/**
	 * @return  pointer to buffer
	 */
	virtual const char *serialize() const
	{
		int format( _Mybase::getFormatType() );

		if ( UnknownFormat == format )
			format = InternalFormat;

		return _Myt::serialize( format );
	}

	/// Serialize message.
	/**
	 * @param[in] format  message format
	 * @return  pointer to buffer
	 */
	virtual const char *serialize( int format ) const
	{
		return _Myimpl::serialize( format );
	}

	/// Show message.
	virtual void show()
	{
		_Myimpl::show();
	}

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // HFCDIAGNOSTICSMESSAGE_H
