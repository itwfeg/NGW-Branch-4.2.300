/**
 * @file EventMsg.h
 * @brief Event message for gateway communication.
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef EDGECOMMEVENTMSG_H
#define EDGECOMMEVENTMSG_H

#include "Header.h"

#include <cstring>

/// Edge Comms namespace.
namespace edgecomm
{

/// Message flags.
class eventMsg
{
public:

	/// Message flags.
	enum flags
	{
		diagnosticsOffered = 0x01,
		diagnosticsForced = 0x02
	};
};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Diagnostics message.
struct eventMsg_v1 : public header
{
	typedef eventMsg_v1 _Myt;
	typedef header _Mybase;

	uint32_t cycleCount_;

	uint32_t errorCode_;
	uint16_t severity_;

	uint16_t flags_;

	// ========================================================================
	// CTOR
	// ========================================================================

	/// Constructor.
	eventMsg_v1() :
		_Mybase( EVENT_MESSAGE, 1 ) {initialize();}

	/// Constructor.
	/**
	 * @param[in] serialNumber  serial number
	 * @param[in] modelNumber  model number
	 */
	eventMsg_v1( const std::string &serialNumber, const std::string &modelNumber ) :
		_Mybase( EVENT_MESSAGE, 1, serialNumber, modelNumber ) {initialize();}

	// ========================================================================
	// Properties
	// ========================================================================

	/// Retrieve message offsets.
	/**
	 * @param[in,out] list  list of offsets
	 */
	void offsets( offsetList &list ) const {
		_Mybase::offsets( list );
		list.push_back( offsetList::value_type( &cycleCount_, sizeof(cycleCount_) ) );
		list.push_back( offsetList::value_type( &errorCode_, sizeof(errorCode_) ) );
		list.push_back( offsetList::value_type( &severity_, sizeof(severity_) ) );
		list.push_back( offsetList::value_type( &flags_, sizeof(flags_) ) );
	}

private:

	void initialize() {
		memset( &((char*) this)[sizeof(_Mybase)], 0, sizeof(_Myt) - sizeof(_Mybase) );
	}

};

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace

#endif // EDGECOMMEVENTMSG_H
