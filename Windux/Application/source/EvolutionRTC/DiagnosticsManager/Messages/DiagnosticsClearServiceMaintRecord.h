/**
 * @file DiagnosticsClearServiceMaintRecord.h
 * @brief Diagnostics clear service maint record message.
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef DIAGNOSTICSCLEARSERVICEMAINTRECORD_H
#define DIAGNOSTICSCLEARSERVICEMAINTRECORD_H

#include "DiagnosticsMessage.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Diagnostics clear service maint record message.
class DiagnosticsClearServiceMaintRecord : public DiagnosticsMessage
{
	typedef DiagnosticsClearServiceMaintRecord _Myt;
	typedef DiagnosticsMessage _Mybase;

public:

	std::string maintRecord_;

	int reason_;

	int serviceMethod1_;
	int servicePart1_;

	int serviceMethod2_;
	int servicePart2_;

	int serviceMethod3_;
	int servicePart3_;

	int serviceMethod4_;
	int servicePart4_;

	std::string serviceNotes_;

	// ========================================================================
	// CTOR / DTOR
	// ========================================================================

	/// Constructor.
	/**
	 * @param[in] thread  receiver thread
	 * @param[in] seqNumber  sequence number
	 */
	DiagnosticsClearServiceMaintRecord( Hobart::CEvolMsgReceiver *thread = NULL, unsigned int seqNumber = 0 ) :
		_Mybase( DiagClearServiceMaintRecord, thread, seqNumber )
	{
		initialize();
	}

	/// Constructor.
	/**
	 * @param[in] buffer  message buffer
	 */
	DiagnosticsClearServiceMaintRecord( const char *buffer ) :
		_Mybase( DiagClearServiceMaintRecord )
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
	DiagnosticsClearServiceMaintRecord( const char *buffer, size_t bufferLen, int format ) :
		_Mybase( DiagClearServiceMaintRecord )
	{
		initialize();
		deserialize( buffer, bufferLen, format );
	}

	/// Destructor.
	virtual ~DiagnosticsClearServiceMaintRecord() {}

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
		APPEND_FIELD( reason_, 0 )
		APPEND_FIELD( serviceMethod1_, 0 )
		APPEND_FIELD( servicePart1_, 0 )
		APPEND_FIELD( serviceMethod2_, 0 )
		APPEND_FIELD( servicePart2_, 0 )
		APPEND_FIELD( serviceMethod3_, 0 )
		APPEND_FIELD( servicePart3_, 0 )
		APPEND_FIELD( serviceMethod4_, 0 )
		APPEND_FIELD( servicePart4_, 0 )
		APPEND_FIELD( serviceNotes_, std::string() )
	}

private:

	/// Initialize.
	void initialize()
	{
		SET_FIELDS
	}

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // DIAGNOSTICSCLEARSERVICEMAINTRECORD_H
