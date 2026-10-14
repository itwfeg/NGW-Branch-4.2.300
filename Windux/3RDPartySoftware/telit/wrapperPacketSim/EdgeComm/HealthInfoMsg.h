/**
 * @file HealthInfoMsg.h
 * @brief Health info message for gateway communication.
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef EDGECOMMHEALTHINFOMSG_H
#define EDGECOMMHEALTHINFOMSG_H

#include "Header.h"

#include <cstring>

/// Edge Comms namespace.
namespace edgecomm
{

/// Message flags.
class healthInfoMsg
{
public:

	/// Message flags.
	enum flags
	{
		addToLog = 0x01,
		removeFromLog = 0x02,
		generatedByDiagnostics = 0x04
	};
};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Diagnostics message.
struct healthInfoMsg_v1 : public header
{
	typedef healthInfoMsg_v1 _Myt;
	typedef header _Mybase;

	uint32_t errorCode_;
	uint16_t severity_;

	uint16_t flags_;

	uint16_t serviceNotesOffset_;
	uint16_t serviceNotesLength_;

	uint16_t serviceActions_[4][2];

	// ========================================================================
	// CTOR
	// ========================================================================

	/// Constructor.
	healthInfoMsg_v1() :
		_Mybase( HEALTH_INFO_MESSAGE, 1 ) {initialize();}

	/// Constructor.
	/**
	 * @param[in] serialNumber  serial number
	 * @param[in] modelNumber  model number
	 */
	healthInfoMsg_v1( const std::string &serialNumber, const std::string &modelNumber ) :
		_Mybase( HEALTH_INFO_MESSAGE, 1, serialNumber, modelNumber ) {initialize();}

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
		list.push_back( offsetList::value_type( &severity_, sizeof(severity_) ) );
		list.push_back( offsetList::value_type( &flags_, sizeof(flags_) ) );
		list.push_back( offsetList::value_type( &serviceNotesOffset_, sizeof(serviceNotesOffset_) ) );
		list.push_back( offsetList::value_type( &serviceNotesLength_, sizeof(serviceNotesLength_) ) );
		list.push_back( offsetList::value_type( &serviceActions_, sizeof(serviceActions_) ) );
	}

private:

	void initialize() {
		memset( &((char*) this)[sizeof(_Mybase)], 0, sizeof(_Myt) - sizeof(_Mybase) );
	}

};

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace

#endif // EDGECOMMHEALTHINFOMSG_H
