/**
 * @file NetworkVal.h
 * @brief Utility methods for byte conversion.
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef UTILNETWORKVAL_H
#define UTILNETWORKVAL_H

#include <stdint.h>

#include <string>

/// Util objects namespace.
namespace util
{

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Utility methods for byte conversion.
class NetworkVal
{
	typedef NetworkVal _Myt;

public:

	/// Set integer value.
	/**
	 * @param[in,out] field  field to set
	 * @param[in] value  value to set
	 */
	template <typename T, typename U>
	static void setValueInt( T &field, U value ) {
		switch ( sizeof(T) )
		{
		case 1:	field = value; break;
		case 2: field = hostToNet16( value ); break;
		case 4: field = hostToNet32( value ); break;
		}
	}

	/// Set string value.
	/**
	 * @param[in,out] field  field to set
	 * @param[in] fieldSize  field size
	 * @param[in] value  value to set
	 */
	static void setValueString( char *field, size_t fieldSize, const char *value );

	/// Set string value.
	/**
	 * @param[in,out] field  field to set
	 * @param[in] fieldSize  field size
	 * @param[in] value  value to set
	 */
	static void setValueString( char *field, size_t fieldSize, const std::string &value ) {
		setValueString( field, fieldSize, value.c_str() );
	}

	/// Retrieve integer value.
	/**
	 * @param[in] field  field to retrieve
	 * @return  value
	 */
	template <typename T>
	static T valueInt( const T &field ) {
		switch ( sizeof(T) )
		{
		case 1:	return field;
		case 2: return netToHost16( field );
		case 4: return netToHost32( field );
		}

		return 0;
	}

	/// Retrieve string value.
	/**
	 * @param[in] field  field to retrieve
	 * @param[in] fieldSize  field size
	 * @return  value
	 */
	static std::string valueString( const char *field, size_t fieldSize ) {
		return std::string( field, fieldSize );
	}

private:

	// not implemented
	NetworkVal();

	// not implemented
	_Myt &operator = ( const _Myt& );

	static uint16_t hostToNet16( uint16_t value );
	static uint32_t hostToNet32( uint32_t value );

	static uint16_t netToHost16( uint16_t value );
	static uint32_t netToHost32( uint32_t value );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace

#endif // UTILNETWORKVAL_H
