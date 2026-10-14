/**
 * @file DiagnosticsPrintLabelStats.h
 * @brief Diagnostics print label stats.
 *
 * @copyright Copyright (c) 2019, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef DIAGNOSTICSPRINTLABELSTATS_H
#define DIAGNOSTICSPRINTLABELSTATS_H

#include "DiagnosticsMessage.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Diagnostics print label stats.
class DiagnosticsPrintLabelStats : public DiagnosticsMessage
{
	typedef DiagnosticsPrintLabelStats _Myt;
	typedef DiagnosticsMessage _Mybase;

public:

	int printerType_;

	std::string pixelsPrinted_;
	std::string pixelsRemoved_;

	// ========================================================================
	// CTOR / DTOR
	// ========================================================================

	/// Constructor.
	/**
	 * @param[in] thread  receiver thread
	 * @param[in] seqNumber  sequence number
	 */
	DiagnosticsPrintLabelStats( Hobart::CEvolMsgReceiver *thread = NULL, unsigned int seqNumber = 0 ) :
		_Mybase( DiagPrintLabelStats, thread, seqNumber )
	{
		initialize();
	}

	/// Constructor.
	/**
	 * @param[in] buffer  message buffer
	 */
	DiagnosticsPrintLabelStats( const char *buffer ) :
		_Mybase( DiagPrintLabelStats )
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
	DiagnosticsPrintLabelStats( const char *buffer, size_t bufferLen, int format ) :
		_Mybase( DiagPrintLabelStats )
	{
		initialize();
		deserialize( buffer, bufferLen, format );
	}

	/// Destructor.
	virtual ~DiagnosticsPrintLabelStats() {}

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

		APPEND_FIELD( printerType_, 0 );
		APPEND_FIELD( pixelsPrinted_, std::string() );
		APPEND_FIELD( pixelsRemoved_, std::string() );
	}

private:

	/// Initialize.
	void initialize()
	{
		SET_FIELDS
	}

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // DIAGNOSTICSPRINTLABELSTATS_H
