/**
 * @file Dict.h
 * @brief Dictionary class.
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef UTILDICT_H
#define UTILDICT_H

#include <list>
#include <map>
#include <string>

/// Util objects namespace.
namespace util
{

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Dictionary class.
class Dictionary : public std::list<std::pair<std::string, std::string> >
{
	typedef Dictionary _Myt;
	typedef std::list<std::pair<std::string, std::string> > _Mybase;

public:

	// ========================================================================
	// CTOR / DTOR
	// ========================================================================

	/// Constructor.
	Dictionary();

	/// Constructor.
	/**
	 * @param[in] other  object to copy
	 */
	Dictionary( const _Myt &other );

	// ========================================================================
	// Operators
	// ========================================================================

	/// Assignment operator.
	/**
	 * @param[in] lhs  object to copy
	 * @return  reference to this
	 */
	_Myt &operator = ( const _Myt &lhs );

	/// Indexing operator.
	/**
	 * @param[in] key  key to lookup in dictionary
	 * @return  value for key, or empty string if not found
	 */
	std::string operator [] ( const std::string &key ) const {return at( key );}

	// ========================================================================
	// Properties
	// ========================================================================

	/// Retrieve value.
	/**
	 * @param[in] key  key to lookup in dictionary
	 * @return  value for key, or empty string if not found
	 */
	std::string at( const std::string &key ) const;

	/// Retrieve if stream is comma separated.
	/**
	 * @return  @c true if comma separated, @c false otherwise
	 */
	bool isCommaSeparated() const {return csv_;}

	/// Retrieve if stream is comma separated.
	/**
	 * param[in] value  @c true if comma separated, @c false otherwise
	 */
	void setCommaSeparated( bool value ) {csv_ = value;}

	// ========================================================================
	// Methods
	// ========================================================================

	/// Append key value pair to dictionary.
	/**
	 * @param[in] key  key to append
	 * @param[in] value  value
	 */
	void append( const std::string &key, const std::string &value );

	/// Append string key value pair to dictionary.
	/**
	 * @param[in] key  key to append
	 * @param[in] value  value
	 */
	void appendString( const std::string &key, const std::string &value ) {return append( key, csvValue( value ) );}

	/// Format dictionary as string.
	/**
	 * @return  string value
	 */
	std::string format() const;

	/// Remove key value pair from dictionary.
	/**
	 * @param[in] key  key to remove
	 */
	void remove( const std::string &key );

	// ========================================================================
	// Static Methods
	// ========================================================================

	/// Convert string to csv string.
	/**
	 * This method will only perform conversion when setup for CSV.
	 *
	 * @param[in] s  string to convert
	 * @param[in] csv  @c true to convert, @c false otherwise
	 * @return  csv string
	 */
	static std::string csvValueString( const std::string &s, bool csv );

protected:

	bool csv_;										///< Comma separated or not.

	// ========================================================================
	// Methods
	// ========================================================================

	/// Convert string to csv string.
	/**
	 * This method will only perform conversion when setup for CSV.
	 *
	 * @param[in] s  string to convert
	 * @return  csv string
	 */
	std::string csvValue( const std::string &s ) const {return csvValueString( s, csv_ );}

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Dictionary list class.
class DictionaryList : public std::list<Dictionary>
{
	typedef DictionaryList _Myt;
	typedef std::list<Dictionary> _Mybase;

public:

	// ========================================================================
	// CTOR / DTOR
	// ========================================================================

	/// Constructor.
	DictionaryList();

	/// Constructor.
	/**
	 * @param[in] other  object to copy
	 */
	DictionaryList( const _Myt &other );

	// ========================================================================
	// Operators
	// ========================================================================

	/// Assignment operator.
	/**
	 * @param[in] lhs  object to copy
	 * @return  reference to this
	 */
	_Myt &operator = ( const _Myt &lhs );

	// ========================================================================
	// Properties
	// ========================================================================

	/// Retrieve if stream is comma separated.
	/**
	 * @return  @c true if comma separated, @c false otherwise
	 */
	bool isCommaSeparated() const {return csv_;}

	/// Retrieve if stream is comma separated.
	/**
	 * param[in] value  @c true if comma separated, @c false otherwise
	 */
	void setCommaSeparated( bool value ) {csv_ = value;}

	// ========================================================================
	// Methods
	// ========================================================================

	/// Append key value pair to dictionary.
	/**
	 * @param[in] key  key to append
	 * @param[in] value  value
	 */
	void append( const Dictionary &dict );

	/// Format dictionary list as string.
	/**
	 * @return  string value
	 */
	std::string format() const;

	/// Sort list by severity, most severe first.
	void sortBySeverity();

protected:

	bool csv_;										///< Comma separated or not.

	// ========================================================================
	// Methods
	// ========================================================================

	/// Convert string to csv string.
	/**
	 * This method will only perform conversion when setup for CSV.
	 *
	 * @param[in] s  string to convert
	 * @return  csv string
	 */
	std::string csvValue( const std::string &s ) const {return Dictionary::csvValueString( s, csv_ );}

};

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace

#endif // UTILDICT_H
