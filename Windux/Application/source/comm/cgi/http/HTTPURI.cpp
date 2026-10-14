/**
 * @file HTTPURI.cpp
 * HTTP URI class.
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Santos Juan
 * @author Randy Blankley
 */

#include "PAL_platformheader.h"

#include "HTTPURI.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"
#endif

namespace comm
{

const std::string HTTPURI::_HTTPScheme_("http");

const std::string HTTPURI::_validSchemeMarks_("+-.");
const std::string HTTPURI::_validPathMarks_("-_.!~*'():@&+$,;");
const std::string HTTPURI::_validQueryMarks_("-_.!~*'();/?:@&=+$,");
const std::string HTTPURI::_validFragmentMarks_("-_.!~*'();/?:@&=+$,");

const std::string HTTPURI::_emptyParameter_("");

#ifdef USE_ACCOUNTANT
int HTTPURI::_dataType_ = 0;
std::string HTTPURI::_dataTypeName_("comm::HTTPURI");
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
HTTPURI::HTTPURI()
{
	initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
HTTPURI::HTTPURI(const std::string& uri)
{
	initialize();
	parseAbsoluteURI(uri);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
HTTPURI::HTTPURI(const _Myt& other)
{
	initialize();
	copy(other);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
HTTPURI::~HTTPURI()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
HTTPURI& HTTPURI::operator = ( const _Myt& rhs )
{
	copy(rhs);
	return *this;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void HTTPURI::setQueryParameter(const std::string& param, const std::string& value)
{
	if ( queryNeedsParsing_ )
		parseQuery();

	if ( queryParameters_.find(param) == queryParameters_.end() )
		queryParameterNames_.push_back(param);
	queryParameters_[param] = value;

	rebuildQuery();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
Util::CSmartPtr<std::string> HTTPURI::formatURI() const
{
	Util::CSmartPtr<std::string> uri;

	if ( !isValid() )
		return uri;

	uri = new std::string();

	uri->append(_HTTPScheme_).append(1, ':').append(2, '/');
	uri->append(getHost());

	if ( getPort().size() )
		uri->append(1, ':').append(getPort());

	uri->append(getAbsolutePath());

	if ( getQuery().size() )
		uri->append(1, '?').append(getQuery());

	if ( getFragment().size() )
		uri->append(1, '#').append(getFragment());

	return uri;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void HTTPURI::read( std::istream &is )
{
	std::string uri;
	char c;

	// read until NULL
	for ( ;; )
	{
		is.read( &c, 1 );

		if ( !c )
			break;

		uri.push_back( c );
	}

	initialize();
	parseAbsoluteURI(uri);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void HTTPURI::write( std::ostream &os ) const
{
	Util::CSmartPtr<std::string> uri( formatURI() );

	// write out uri
	os.write( uri->c_str(), uri->length()+1 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void HTTPURI::initialize()
{
	reset();

#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void HTTPURI::reset()
{
	host_.clear();
	port_.clear();
	path_.clear();
	query_.clear();
	fragment_.clear();
	valid_ = false;
	ipHost_ = false;
	queryNeedsParsing_ = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void HTTPURI::copy(const _Myt& other)
{
	host_ = other.host_;
	port_ = other.port_;
	path_ = other.path_;
	query_ = other.query_;
	fragment_ = other.fragment_;
	valid_ = other.valid_;

	ipHost_ = other.ipHost_;

	queryNeedsParsing_ = other.queryNeedsParsing_;

	queryParameters_ = other.queryParameters_;
	queryParameterNames_ = other.queryParameterNames_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void HTTPURI::parseAbsoluteURI(const std::string& u)
{
	bool isHTTPScheme = false;
	bool authorityPresent = false;
	std::string uri(u);
	std::string::size_type separator, length = uri.size();

	reset();

	if ( length == 0 )
		return;


	/* If there is a fragment, take note of it and remove it. */
	separator = uri.find('#');
	if ( separator != std::string::npos )
	{
		std::string::size_type fragmentPosition = separator + 1;
		if ( fragmentPosition < length )
		{
			fragment_.assign(uri, fragmentPosition, length - fragmentPosition);

			/* Validate fragment characters */
			if ( !validateURIFragment(fragment_) )
				return;
		}

		uri.erase(separator);
		length = uri.size();
	}


	/* If there is a query, take note of it and remove it. */
	separator = uri.find('?');
	if ( separator != std::string::npos )
	{
		std::string::size_type queryPosition = separator + 1;
		if ( queryPosition < length )
		{
			std::string query(uri, queryPosition, length - queryPosition);

			/* Validate query characters */
			if ( !validateURIQuery(query) )
				return;

			setQuery(query);
		}

		uri.erase(separator);
		length = uri.size();
	}


	/* When parsing the URL location, first isolate the scheme. */
	separator = uri.find(':');
	if ( separator != std::string::npos )
	{
		if ( separator == _HTTPScheme_.size() )
		{
			std::string::size_type p = 0;
			for ( ; p < separator; ++p )
			{
				if ( tolower(uri[p]) != _HTTPScheme_[p] )
					break;
			}

			if ( p >= separator )
			{
				isHTTPScheme = true;
				++separator;
			}
		}
	}

	if ( !isHTTPScheme )
		return;


	/* The authority comes after the scheme. */
	std::string authority;
	if ( separator + 1 < length )
	{
		if ( uri[separator] == '/' && uri[separator + 1] == '/' )
		{
			std::string::size_type authorityPos = separator + 2;

			if ( authorityPos < length )
			{
				separator = uri.find('/', authorityPos);
				if ( separator == std::string::npos )
					separator = length;

				if ( authorityPos < separator )
				{
					authority.assign(uri, authorityPos, separator - authorityPos);
					authorityPresent = true;
				}
			}
		}
	}

	if ( authorityPresent )
	{
		std::string::size_type separator2 = authority.find(':');
		if ( separator2 != std::string::npos )
		{
			std::string::size_type portPos = separator2 + 1;
			if ( portPos < authority.size() )
				port_.assign(authority, portPos, authority.size() - portPos);

			/* Validate port characters */
			if ( !validateURIPort(port_) )
				return;
		}
		else
			separator2 = authority.size();

		host_.assign(authority, 0, separator2);
		if ( !validateURIHost(host_) )
			return;
	}
	else
		return;


	/* The path, if present, comes after the authority. */
	if ( separator < length )
	{
		path_.assign(uri, separator, length - separator);
		if ( !validateURIPath(path_) )
			return;
	}
	else
		path_.assign("/");

	valid_ = true;

	return;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void HTTPURI::parseQuery() const
{
	std::string::size_type length = query_.size();

	queryParameters_.clear();
	queryParameterNames_.clear();

	if ( !length )
		return;

	std::string::size_type currentPos, separatorPos, equalPos;
	std::string paramName, paramValue;
	for ( currentPos = 0; currentPos < length; currentPos = separatorPos + 1 )
	{
		separatorPos = query_.find('&', currentPos);
		if ( separatorPos == std::string::npos )
			separatorPos = length;

		equalPos = query_.find('=', currentPos);
		if ( equalPos == std::string::npos || equalPos > separatorPos )
			equalPos = separatorPos;

		if ( currentPos < equalPos )
		{
			paramName = query_.substr(currentPos, equalPos - currentPos);

			++equalPos;
			if ( equalPos < separatorPos )
				paramValue = query_.substr(equalPos, separatorPos - equalPos);
			else
				paramValue.clear();

			if ( queryParameters_.find(paramName) == queryParameters_.end() )
				queryParameterNames_.push_back(paramName);
			queryParameters_[paramName] = paramValue;
		}
	}

	queryNeedsParsing_ = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void HTTPURI::rebuildQuery()
{
	query_.clear();

	if ( queryParameterNames_.size() )
	{
		parameterNames_iterator iter = queryParameterNames_.begin();

		query_.append(*iter).append(1, '=').append(queryParameters_[*iter]);
		while ( ++iter != queryParameterNames_.end() )
			query_.append(1, '&').append(*iter).append(1, '=').append(queryParameters_[*iter]);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool HTTPURI::validateURIHost(const std::string& host)
{
	std::string::size_type length = host.size();

	if ( !length || !isalnum(host[0]) )
		return false;

	/* Check if it is an IPv4 address. */
	int group = 0;
	std::string::size_type currentPos, dotPosition;
	for ( currentPos = 0; group < 4 && currentPos < length; ++group, ++currentPos )
	{
		dotPosition = host.find('.', currentPos);
		if ( dotPosition == std::string::npos )
		{
			if ( group == 3 )
				dotPosition = length;
			else
				break;
		}

		if ( currentPos < dotPosition )
		{
			for ( ; currentPos < dotPosition; ++currentPos )
			{
				if ( !isdigit(host[currentPos]) )
					break;
			}
			if ( currentPos < dotPosition )
				break;
		}
		else break;
	}

	if ( group == 4 && currentPos > length )
	{
		/* It is an IPv4 address. */
		ipHost_ = true;
		return true;
	}


	/* If not IPv4 address try hostname. */
	currentPos = 0;
	while ( currentPos < length )
	{
		dotPosition = host.find('.', currentPos);
		if ( dotPosition == std::string::npos )
			dotPosition = length;

		if ( currentPos < dotPosition )
		{
			/* The last label ('toplabel') must start with an alphabetic character. */
			if ( dotPosition >= length - 1 && !isalpha(host[currentPos]) )
				break;

			/* Any label must start and end with an alpha numeric character. */
			if ( !isalnum(host[currentPos++]) || !isalnum(host[--dotPosition]) )
				break;

			for ( ; currentPos < dotPosition; ++currentPos )
			{
				if ( !(isalnum(host[currentPos]) || host[currentPos] == '-') )
					break;
			}
			if ( currentPos < dotPosition )
				break;
			else
				++currentPos;
		}
		else break;

		++currentPos;
	}

	if ( currentPos < length )
		return false;

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool HTTPURI::validateURIPort(const std::string& port)
{
	int length = (int)port.size();

	for ( int p = 0; p < length; ++p )
	{
		if ( !isdigit(port[p]) )
			return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool HTTPURI::validateURIPath(const std::string& path, bool absolute)
{
	std::string::size_type currentPos = 0;
	std::string::size_type length = path.size();

	if ( length == 0 )
		return false;

	if ( absolute && path[0] != '/' )
		return false;

	if ( path[currentPos] == '/' )
		++currentPos;
	while ( currentPos < length )
	{
		std::string::size_type separator = path.find('/', currentPos);
		if ( separator == std::string::npos )
			separator = length;

		if ( currentPos < separator )
		{
			if ( path[currentPos] == ';' )
				break;

			for ( ; currentPos < separator; ++currentPos )
			{
				if ( isValidEscapeSequence(path, currentPos, separator) )
					currentPos += 2;
				else
				{
					if ( !(isalnum(path[currentPos]) || _validPathMarks_.find(path[currentPos]) != std::string::npos) )
						break;
				}
			}

			if ( currentPos < separator )
				break;
		}
		else
			break;

		++currentPos;
	}

	if ( currentPos < length )
		return false;

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool HTTPURI::validateURIQuery(const std::string& query)
{
	std::string::size_type currentPos = 0;
	std::string::size_type length = query.size();

	for ( ; currentPos < length; ++currentPos )
	{
		if ( isValidEscapeSequence(query, currentPos, length) )
			currentPos += 2;
		else
		{
			if ( !(isalnum(query[currentPos]) || _validQueryMarks_.find(query[currentPos]) != std::string::npos) )
				return false;
		}
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool HTTPURI::validateURIFragment(const std::string& fragment)
{
	std::string::size_type currentPos = 0;
	std::string::size_type length = fragment.size();

	for ( ; currentPos < length; ++currentPos )
	{
		if ( isValidEscapeSequence(fragment, currentPos, length) )
			currentPos += 2;
		else
		{
			if ( !(isalnum(fragment[currentPos]) || _validFragmentMarks_.find(fragment[currentPos]) != std::string::npos) )
				return false;
		}
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool HTTPURI::isValidEscapeSequence(const std::string& s, std::string::size_type off, std::string::size_type end)
{
	if ( off + 2 < end && s[off] == '%' )
	{
		if ( isxdigit(s[off + 1]) && isxdigit(s[off + 2]) )
			return true;
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // comm
