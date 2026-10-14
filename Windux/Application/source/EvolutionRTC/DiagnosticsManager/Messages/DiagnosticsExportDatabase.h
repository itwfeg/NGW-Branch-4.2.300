/**
 * @file DiagnosticsExportDatabase.h
 * @brief Diagnostics export database message.
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef DIAGNOSTICSEXPORTDATABASE_H
#define DIAGNOSTICSEXPORTDATABASE_H

#include "DiagnosticsMessage.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Diagnostics export database message.
class DiagnosticsExportDatabase : public DiagnosticsMessage
{
	typedef DiagnosticsExportDatabase _Myt;
	typedef DiagnosticsMessage _Mybase;

public:

	std::string filename_;

	// ========================================================================
	// CTOR / DTOR
	// ========================================================================

	/// Constructor.
	/**
	 * @param[in] thread  receiver thread
	 * @param[in] seqNumber  sequence number
	 */
	DiagnosticsExportDatabase( Hobart::CEvolMsgReceiver *thread = NULL, unsigned int seqNumber = 0 ) :
		_Mybase( DiagExportDatabase, thread, seqNumber )
	{
		initialize();
	}

	/// Constructor.
	/**
	 * @param[in] buffer  message buffer
	 */
	DiagnosticsExportDatabase( const char *buffer ) :
		_Mybase( DiagExportDatabase )
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
	DiagnosticsExportDatabase( const char *buffer, size_t bufferLen, int format ) :
		_Mybase( DiagExportDatabase )
	{
		initialize();
		deserialize( buffer, bufferLen, format );
	}

	/// Destructor.
	virtual ~DiagnosticsExportDatabase() {}

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

		APPEND_FIELD( filename_, std::string() )
	}

private:

	/// Initialize.
	void initialize()
	{
		SET_FIELDS
	}

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Diagnostics export database complete message.
class DiagnosticsExportDatabaseComplete : public DiagnosticsMessage
{
	typedef DiagnosticsExportDatabaseComplete _Myt;
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
	DiagnosticsExportDatabaseComplete( Hobart::CEvolMsgReceiver *thread = NULL, unsigned int seqNumber = 0 ) :
		_Mybase( DiagExportDatabaseComplete, thread, seqNumber )
	{
		initialize();
	}

	/// Constructor.
	/**
	 * @param[in] buffer  message buffer
	 */
	DiagnosticsExportDatabaseComplete( const char *buffer ) :
		_Mybase( DiagExportDatabaseComplete )
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
	DiagnosticsExportDatabaseComplete( const char *buffer, size_t bufferLen, int format ) :
		_Mybase( DiagExportDatabaseComplete )
	{
		initialize();
		deserialize( buffer, bufferLen, format );
	}

	/// Destructor.
	virtual ~DiagnosticsExportDatabaseComplete() {}

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

#endif // DIAGNOSTICSEXPORTDATABASE_H
