/**
 * @file DiagnosticsMsg.h
 * @brief Diagnostics message for gateway communication.
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef EDGECOMMDIAGNOSTICSMSG_H
#define EDGECOMMDIAGNOSTICSMSG_H

#include "Header.h"

#include <cstring>

/// Edge Comms namespace.
namespace edgecomm
{

/// Message flags.
class diagnosticsMsg
{
public:

	/// Message flags.
	enum flags
	{
		passed = 0x01,
		failed = 0x02,
		timeout = 0x04,
		runByOperator = 0x08,
		runByService = 0x10
	};
};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Diagnostics message.
struct diagnosticsMsg_v1 : public header
{
	typedef diagnosticsMsg_v1 _Myt;
	typedef header _Mybase;

	uint32_t errorCode_;
	uint32_t resultCode;

	uint32_t diagRoutine_;
	uint32_t diagRoutineExpected;
	uint32_t diagRoutineActual;

	uint16_t flags_;

	// ========================================================================
	// CTOR
	// ========================================================================

	/// Constructor.
	diagnosticsMsg_v1() :
		_Mybase( DIAGNOSTICS_MESSAGE, 1 ) {initialize();}

	/// Constructor.
	/**
	 * @param[in] serialNumber  serial number
	 * @param[in] modelNumber  model number
	 */
	diagnosticsMsg_v1( const std::string &serialNumber, const std::string &modelNumber ) :
		_Mybase( DIAGNOSTICS_MESSAGE, 1, serialNumber, modelNumber ) {initialize();}

	// ========================================================================
	// Properties
	// ========================================================================

	/// Retrieve message offsets.
	/**
	 * @param[in,out] list  list of offsets
	 */
	void offsets( offsetList &list ) const {
		_Mybase::offsets( list );
		list.push_back( offsetList::value_type( &errorCode_, sizeof(errorCode_) ) );
		list.push_back( offsetList::value_type( &resultCode, sizeof(resultCode) ) );
		list.push_back( offsetList::value_type( &diagRoutine_, sizeof(diagRoutine_) ) );
		list.push_back( offsetList::value_type( &diagRoutineExpected, sizeof(diagRoutineExpected) ) );
		list.push_back( offsetList::value_type( &diagRoutineActual, sizeof(diagRoutineActual) ) );
		list.push_back( offsetList::value_type( &flags_, sizeof(flags_) ) );
	}

private:

	void initialize() {
		memset( &((char*) this)[sizeof(_Mybase)], 0, sizeof(_Myt) - sizeof(_Mybase) );
	}

};

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace

#endif // EDGECOMMDIAGNOSTICSMSG_H
