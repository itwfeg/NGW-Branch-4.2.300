/**
 * @file StatusCheckMsg.h
 * @brief Status check message for gateway communication.
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef EDGECOMMSTATUSCHECKMSG_H
#define EDGECOMMSTATUSCHECKMSG_H

#include "Header.h"

#include <cstring>

/// Edge Comms namespace.
namespace edgecomm
{

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Diagnostics message.
struct statusCheckMsg_v1 : public header
{
	typedef statusCheckMsg_v1 _Myt;
	typedef header _Mybase;

	uint32_t cycleCount_;

	uint16_t healthInfoCount_;

	// ========================================================================
	// CTOR
	// ========================================================================

	/// Constructor.
	statusCheckMsg_v1() :
		_Mybase( STATUS_CHECK_MESSAGE, 1 ) {initialize();}

	/// Constructor.
	/**
	 * @param[in] serialNumber  serial number
	 * @param[in] modelNumber  model number
	 */
	statusCheckMsg_v1( const std::string &serialNumber, const std::string &modelNumber ) :
		_Mybase( STATUS_CHECK_MESSAGE, 1, serialNumber, modelNumber ) {initialize();}

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
		list.push_back( offsetList::value_type( &healthInfoCount_, sizeof(healthInfoCount_) ) );
	}

private:

	void initialize() {
		memset( &((char*) this)[sizeof(_Mybase)], 0, sizeof(_Myt) - sizeof(_Mybase) );
	}

};

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace

#endif // EDGECOMMSTATUSCHECKMSG_H
