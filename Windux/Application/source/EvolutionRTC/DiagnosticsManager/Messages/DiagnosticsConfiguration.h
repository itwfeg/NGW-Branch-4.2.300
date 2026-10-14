/**
 * @file DiagnosticsConfiguration.h
 * @brief Diagnostics configuration.
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef DIAGNOSTICSCONFIGURATION_H
#define DIAGNOSTICSCONFIGURATION_H

#include "DiagnosticsMessage.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Diagnostics configuration request.
class DiagnosticsConfigurationReq : public DiagnosticsMessage
{
	typedef DiagnosticsConfigurationReq _Myt;
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
	DiagnosticsConfigurationReq( Hobart::CEvolMsgReceiver *thread = NULL, unsigned int seqNumber = 0 ) :
		_Mybase( DiagConfigurationReq, thread, seqNumber )
	{
		initialize();
	}

	/// Constructor.
	/**
	 * @param[in] buffer  message buffer
	 */
	DiagnosticsConfigurationReq( const char *buffer ) :
		_Mybase( DiagConfigurationReq )
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
	DiagnosticsConfigurationReq( const char *buffer, size_t bufferLen, int format ) :
		_Mybase( DiagConfigurationReq )
	{
		initialize();
		deserialize( buffer, bufferLen, format );
	}

	/// Destructor.
	virtual ~DiagnosticsConfigurationReq() {}

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
	DiagnosticsConfigurationReq( DiagEvent event, Hobart::CEvolMsgReceiver *thread = NULL, unsigned int seqNumber = 0 ) :
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

		//APPEND_FIELD( diagEvent_, std::string() )
	}

private:

	/// Initialize.
	void initialize()
	{
		SET_FIELDS
	}

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Diagnostics configuration.
class DiagnosticsConfiguration : public DiagnosticsConfigurationReq
{
	typedef DiagnosticsConfiguration _Myt;
	typedef DiagnosticsConfigurationReq _Mybase;

public:

	std::string serviceContactInfo_;

	std::string gatewayDeviceAddress_;
	int gatewayDeviceLogLevel_;

	bool valueMaxDiagCollectionDisabled_;

	// ========================================================================
	// CTOR / DTOR
	// ========================================================================

	/// Constructor.
	/**
	 * @param[in] thread  receiver thread
	 * @param[in] seqNumber  sequence number
	 */
	DiagnosticsConfiguration( Hobart::CEvolMsgReceiver *thread = NULL, unsigned int seqNumber = 0 ) :
		_Mybase( DiagConfiguration, thread, seqNumber )
	{
		initialize();
	}

	/// Constructor.
	/**
	 * @param[in] buffer  message buffer
	 */
	DiagnosticsConfiguration( const char *buffer ) :
		_Mybase( DiagConfiguration )
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
	DiagnosticsConfiguration( const char *buffer, size_t bufferLen, int format ) :
		_Mybase( DiagConfiguration )
	{
		initialize();
		deserialize( buffer, bufferLen, format );
	}

	/// Destructor.
	virtual ~DiagnosticsConfiguration() {}

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

		APPEND_FIELD( serviceContactInfo_, std::string() )
		APPEND_FIELD( gatewayDeviceAddress_, std::string() )
		APPEND_FIELD( gatewayDeviceLogLevel_, 0 )
		APPEND_FIELD( valueMaxDiagCollectionDisabled_, false )
	}

private:

	/// Initialize.
	void initialize()
	{
		SET_FIELDS
	}

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // DIAGNOSTICSCONFIGURATION_H
