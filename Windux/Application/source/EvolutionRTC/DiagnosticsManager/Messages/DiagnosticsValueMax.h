/**
 * @file DiagnosticsValueMax.h
 * @brief Diagnostics value max savings.
 *
 * @copyright Copyright (c) 2019, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef DIAGNOSTICSVALUEMAX_H
#define DIAGNOSTICSVALUEMAX_H

#include "DiagnosticsMessage.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Diagnostics value max savings.
class DiagnosticsValueMax : public DiagnosticsMessage
{
	typedef DiagnosticsValueMax _Myt;
	typedef DiagnosticsMessage _Mybase;

public:

	int counts_;
	int scaleFactor_;

	double tiltX_;
	double tiltY_;

	int transactionNum_;

	int itemProductNum_;
	double itemUnitPrice_;
	int itemQuantity_;
	double itemWeight_;
	double itemTare_;
	double itemTotalPrice_;

	// ========================================================================
	// CTOR / DTOR
	// ========================================================================

	/// Constructor.
	/**
	 * @param[in] thread  receiver thread
	 * @param[in] seqNumber  sequence number
	 */
	DiagnosticsValueMax( Hobart::CEvolMsgReceiver *thread = NULL, unsigned int seqNumber = 0 ) :
		_Mybase( DiagValueMax, thread, seqNumber )
	{
		initialize();
	}

	/// Constructor.
	/**
	 * @param[in] buffer  message buffer
	 */
	DiagnosticsValueMax( const char *buffer ) :
		_Mybase( DiagValueMax )
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
	DiagnosticsValueMax( const char *buffer, size_t bufferLen, int format ) :
		_Mybase( DiagValueMax )
	{
		initialize();
		deserialize( buffer, bufferLen, format );
	}

	/// Destructor.
	virtual ~DiagnosticsValueMax() {}

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

		APPEND_FIELD( counts_, 0 );
		APPEND_FIELD( scaleFactor_, 0 );
		APPEND_FIELD( tiltX_, 0.0 );
		APPEND_FIELD( tiltY_, 0.0 );

		APPEND_FIELD( transactionNum_, 0 );

		APPEND_FIELD( itemProductNum_, 0 );
		APPEND_FIELD( itemUnitPrice_, 0.0 );
		APPEND_FIELD( itemQuantity_, 0 );
		APPEND_FIELD( itemWeight_, 0.0 );
		APPEND_FIELD( itemTare_, 0.0 );
		APPEND_FIELD( itemTotalPrice_, 0.0 );
	}

private:

	/// Initialize.
	void initialize()
	{
		SET_FIELDS
	}

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // DIAGNOSTICSVALUEMAX_H
