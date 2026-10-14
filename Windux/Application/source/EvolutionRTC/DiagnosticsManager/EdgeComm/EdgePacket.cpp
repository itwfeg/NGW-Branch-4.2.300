/**
 * @file EdgePacket.cpp
 * @brief Edge device packet for gateway communication.
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#include "EdgePacket.h"

#include <Debug.h>

#include <e_string/utilstring.h>

#include <cstring>

/// Edge Comms namespace.
namespace edgecomm
{

#define TRACE( logline ) Debug::trace( "DiagnosticsManager.edgedevice.edgepacket", (logline) )

///////////////////////////////////////////////////////////////////////////////////////////////////
void EdgePacket::update( size_t offset, const void *buffer, size_t bufferLen )
{
	size_t newWriteOffset( offset + bufferLen );

	// sanity check
	if ( MAX_PACKET_SIZE < (newWriteOffset + LENGTH_PREFIX_SIZE) )
	{
		TRACE( "updated buffer too large " + util_string::longlongtostring( offset ) + " " + util_string::longlongtostring( bufferLen ) );
		return;
	}

	// write!
	memcpy( &buffer_[offset + LENGTH_PREFIX_SIZE], buffer, bufferLen );

	if ( writeOffset_ < newWriteOffset )
		util::NetworkVal::setValueInt( *reinterpret_cast<uint16_t*>( &buffer_[0] ), (writeOffset_ = newWriteOffset) );

	TRACE( "added " + util_string::longlongtostring( bufferLen ) + " bytes new write offset " + util_string::longlongtostring( writeOffset_ ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void EdgePacket::initialize()
{
	memset( buffer_, 0, sizeof(buffer_) );
	writeOffset_ = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void EdgePacket::copy( const _Myt &other )
{
	memcpy( buffer_, other.buffer_, (writeOffset_ = other.writeOffset_) + LENGTH_PREFIX_SIZE );
}

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace
