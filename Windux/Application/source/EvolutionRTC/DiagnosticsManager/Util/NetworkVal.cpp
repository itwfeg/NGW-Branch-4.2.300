/**
 * @file NetworkVal.cpp
 * @brief Utility methods for byte conversion.
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#include "NetworkVal.h"

#include <cstring>

#if defined( _MSC_VER )
#include <winsock2.h>
#elif defined( __GNUC__ )
#include <netinet/in.h>
#endif

/// Util objects namespace.
namespace util
{

///////////////////////////////////////////////////////////////////////////////////////////////////
void NetworkVal::setValueString( char *field, size_t fieldSize, const char *value )
{
	std::memset( field, 0, fieldSize );

	if ( value )
	{
		size_t len( std::strlen( value ) );

		if ( fieldSize < len )
			len = fieldSize;

		std::memcpy( field, value, len );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
uint16_t NetworkVal::hostToNet16( uint16_t value )
{
	return htons( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t NetworkVal::hostToNet32( uint32_t value )
{
	return htonl( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
uint16_t NetworkVal::netToHost16( uint16_t value )
{
	return ntohs( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t NetworkVal::netToHost32( uint32_t value )
{
	return ntohl( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace
