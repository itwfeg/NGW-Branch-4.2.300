/**
 * @file Timestamp.h
 * @brief Timestamp class in Unix Epoch Milliseconds.
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef UTILTIMESTAMP_H
#define UTILTIMESTAMP_H

#include <iostream>
#include <string>

/// Util objects namespace.
namespace util
{

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Timestamp class in Unix Epoch Milliseconds.
struct Timestamp
{
	typedef Timestamp _Myt;

	/// Raw timestamp value.
	long long value_;

	// ========================================================================
	// CTOR
	// ========================================================================

	/// Constructor.
	Timestamp() : value_( 0 ) {}

	/// Constructor.
	/**
	 * @param[in] value  value to assign
	 */
	Timestamp( long long value ) : value_( value ) {};

	/// Constructor.
	/**
	 * @param[in] value  value to assign
	 */
	Timestamp( const std::string &value );

	/// Constructor.
	/**
	 * @param[in] other  value to assign
	 */
	Timestamp( const _Myt &other ) : value_( other.value_ ) {};

	// ========================================================================
	// Operators
	// ========================================================================

	/// Assignment operator.
	/**
	 * @param[in] value  value to assign
	 * @return  reference to this
	 */
	_Myt & operator = ( long long lhs ) {value_ = lhs; return *this;}

	/// Assignment operator.
	/**
	 * @param[in] value  value to assign
	 * @return  reference to this
	 */
	_Myt & operator = ( const std::string &lhs );

	/// Assignment operator.
	/**
	 * @param[in] value  value to assign
	 * @return  reference to this
	 */
	_Myt & operator = ( const _Myt &lhs ) {value_ = lhs.value_; return *this;}

	/// Cast operator.
	/**
	 * @return  casted value
	 */
	operator long long () const {return value_;}

	/// Cast operator.
	/**
	 * @return  value
	 */
	operator long long & () {return value_;}

	/// Cast operator.
	/**
	 * @return  value
	 */
	operator bool () const {return (value_ ? true : false);}

	// ========================================================================
	// Methods
	// ========================================================================

	/// Add seconds to timestamp.
	/**
	 * @param[in] value  amount of time to add
	 */
	void addSeconds( int value );

	/// Add minutes to timestamp.
	/**
	 * @param[in] value  amount of time to add
	 */
	void addMinutes( int value );

	/// Add hours to timestamp.
	/**
	 * @param[in] value  amount of time to add
	 */
	void addHours( int value );

	/// Add days to timestamp.
	/**
	 * @param[in] value  amount of time to add
	 */
	void addDays( int value );

	/// Convert from date time string.
	/**
	 * @param[in] value  string in MM/DD/YYYY HH:MM:SS.sss format
	 */
	void fromDateTime( const std::string &value );

	/// Format time stamp to formatted string.
	/**
	 * @return  string in MM/DD/YYYY HH:MM:SS.sss format
	 */
	std::string toDateTime() const {return formatDateTime( value_ );}

	/// Format time stamp in milliseconds to string.
	/**
	 * @return  string
	 */
	std::string toString() const;

	/// Format time as unix seconds since epoch.
	/**
	 * @return  seconds since epoch
	 */
	long toUnixEpochTime() const {long long value( value_ ); value /= 1000; return value;}

	// ========================================================================
	// Static Methods
	// ========================================================================

	/// Retrieve timestamp.
	/**
	 * @param[out] value  timestamp
	 */
	static void now( long long &value );

	/// Retrieve timestamp.
	/**
	 * @return  timestamp
	 */
	static long long now() {long long value; now( value ); return value;}

	/// Format time stamp in milliseconds to string.
	/**
	 * @param[in] stamp  stamp to format
	 * @return  string format
	 */
	static std::string formatDateTime( long long stamp );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Operator std::ostream overload.
/**
 * @param[in,out] os  output stream
 * @param[in] obj  object to output
 * @return  reference to os
 */
inline std::ostream & operator << ( std::ostream &os, const Timestamp &obj )
{
	os << (long long) obj;
	return os;
}

/// Comparison operator.
/**
 * @param[in] lhs  object to compare
 * @param[in] rhs  object to compare
 * @return  @c true if lhs < rhs, @c false otherwise
 */
inline bool operator < ( const Timestamp &lhs, const Timestamp &rhs )
{
	return lhs.value_ < rhs.value_;
}

/// Comparison operator.
/**
 * @param[in] lhs  object to compare
 * @param[in] rhs  object to compare
 * @return  @c true if lhs <= rhs, @c false otherwise
 */
inline bool operator <= ( const Timestamp &lhs, const Timestamp &rhs )
{
	return lhs.value_ <= rhs.value_;
}

/// Comparison operator.
/**
 * @param[in] lhs  object to compare
 * @param[in] rhs  object to compare
 * @return  @c true if lhs == rhs, @c false otherwise
 */
inline bool operator == ( const Timestamp &lhs, const Timestamp &rhs )
{
	return lhs.value_ == rhs.value_;
}

/// Comparison operator.
/**
 * @param[in] lhs  object to compare
 * @param[in] rhs  object to compare
 * @return  @c true if lhs >= rhs, @c false otherwise
 */
inline bool operator >= ( const Timestamp &lhs, const Timestamp &rhs )
{
	return lhs.value_ >= rhs.value_;
}

/// Comparison operator.
/**
 * @param[in] lhs  object to compare
 * @param[in] rhs  object to compare
 * @return  @c true if lhs > rhs, @c false otherwise
 */
inline bool operator > ( const Timestamp &lhs, const Timestamp &rhs )
{
	return lhs.value_ > rhs.value_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace

#endif // UTILTIMESTAMP_H
