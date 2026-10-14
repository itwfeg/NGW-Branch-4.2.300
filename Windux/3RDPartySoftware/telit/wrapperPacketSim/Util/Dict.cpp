/**
 * @file Dict.cpp
 * @brief Dictionary class.
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#include "Dict.h"

#include <iostream>
#include <sstream>

/// Util objects namespace.
namespace util
{

class DisableGrouping: public std::numpunct<char>
{
protected:
	// groups of 0 (disable)
	virtual string_type do_grouping() const { return "\000"; }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
Dictionary::Dictionary() :
	_Mybase(),
	csv_( false )
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
Dictionary::Dictionary( const _Myt &other ) :
	_Mybase( other ),
	csv_( other.csv_ )
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
Dictionary &Dictionary::operator = ( const _Myt &lhs )
{
	_Mybase::assign( lhs.begin(), lhs.end() );

	csv_ = lhs.csv_;

	return *this;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
std::string Dictionary::at( const std::string &key ) const
{
	for ( const_iterator i( begin() ); i != end(); ++i )
		if ( key == i->first )
			return i->second;

	return std::string();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Dictionary::append( const std::string &key, const std::string &value )
{
	push_back( value_type( key, value ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
std::string Dictionary::format() const
{
	std::stringstream os;
	bool first( true );

	if ( isCommaSeparated() )
		os.imbue( std::locale( os.getloc(), new DisableGrouping ) );

	for ( const_iterator i( begin() ); i != end(); ++i )
	{
		if ( first )
			first = false;
		else if ( isCommaSeparated() )
			os << ",";
		else
			os << std::endl;


		if ( isCommaSeparated() )
			os << i->second;
		else
			os << "  " << i->first << ": " << i->second;
	}

	return os.str();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
std::string Dictionary::csvValueString( const std::string &s, bool csv )
{
	static const char quote( '\"' );

	if ( !csv )
		return s;

	std::string result( s );

	for ( std::string::size_type pos( result.size() ); pos--; )
		if ( quote == result.at( pos ) )
			result.insert( pos, 1, quote );

	result.push_back( quote );
	result.insert( 0, 1, quote );

	return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
DictionaryList::DictionaryList() :
	_Mybase(),
	csv_( false )
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
DictionaryList::DictionaryList( const _Myt &other ) :
	_Mybase( other ),
	csv_( other.csv_ )
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
DictionaryList &DictionaryList::operator = ( const _Myt &lhs )
{
	_Mybase::assign( lhs.begin(), lhs.end() );

	csv_ = lhs.csv_;

	return *this;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void DictionaryList::append( const Dictionary &dict )
{
	push_back( dict );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
std::string DictionaryList::format() const
{
	std::stringstream os;
	bool first( true );

	for ( const_iterator i( begin() ); i != end(); ++i )
	{
		if ( first )
			first = false;
		else if ( isCommaSeparated() )
			os << ",";
		else
			os << std::endl;

		os << csvValue( i->format() );
	}

	return os.str();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace
