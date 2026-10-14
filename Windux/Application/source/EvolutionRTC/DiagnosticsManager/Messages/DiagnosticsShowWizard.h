/**
 * @file DiagnosticsShowWizard.h
 * @brief Diagnostics show wizard message.
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef DIAGNOSTICSSHOWWIZARD_H
#define DIAGNOSTICSSHOWWIZARD_H

#include "DiagnosticsMessage.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Diagnostics show wizard message.
class DiagnosticsShowWizard : public DiagnosticsMessage
{
	typedef DiagnosticsShowWizard _Myt;
	typedef DiagnosticsMessage _Mybase;

public:

	std::string event_;
	std::string timestamp_;

	std::string diagEvent_;
	int diagErrorCode_;

	std::string hardwareModel_;
	std::string hardwareSerial_;
	unsigned int hardwareCycles_;

	// ========================================================================
	// CTOR / DTOR
	// ========================================================================

	/// Constructor.
	/**
	 * @param[in] thread  receiver thread
	 * @param[in] seqNumber  sequence number
	 */
	DiagnosticsShowWizard( Hobart::CEvolMsgReceiver *thread = NULL, unsigned int seqNumber = 0 ) :
		_Mybase( DiagShowWizard, thread, seqNumber )
	{
		initialize();
	}

	/// Constructor.
	/**
	 * @param[in] buffer  message buffer
	 */
	DiagnosticsShowWizard( const char *buffer ) :
		_Mybase( DiagShowWizard )
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
	DiagnosticsShowWizard( const char *buffer, size_t bufferLen, int format ) :
		_Mybase( DiagShowWizard )
	{
		initialize();
		deserialize( buffer, bufferLen, format );
	}

	/// Destructor.
	virtual ~DiagnosticsShowWizard() {}

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
		APPEND_FIELD( timestamp_, std::string() )
		APPEND_FIELD( diagEvent_, std::string() )
		APPEND_FIELD( diagErrorCode_, 0 )
		APPEND_FIELD( hardwareModel_, std::string() )
		APPEND_FIELD( hardwareSerial_, std::string() )
		APPEND_FIELD( hardwareCycles_, 0 )
	}

private:

	/// Initialize.
	void initialize()
	{
		SET_FIELDS
	}

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // DIAGNOSTICSSHOWWIZARD_H
