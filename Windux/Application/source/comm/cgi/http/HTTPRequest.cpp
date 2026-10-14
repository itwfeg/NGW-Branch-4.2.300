/**
 * @file HTTPRequest.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Santos Juan
 * @author Randy Blankley
 */

#include "PAL_platformheader.h"

#include "Debug.h"
#include "HTTPRequest.h"

#include "util/e_string/utilstring.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"
#endif

namespace comm
{

const std::string HTTPRequest::_methodOPTIONS_("OPTIONS");
const std::string HTTPRequest::_methodGET_("GET");
const std::string HTTPRequest::_methodHEAD_("HEAD");
const std::string HTTPRequest::_methodPOST_("POST");
const std::string HTTPRequest::_methodPUT_("PUT");
const std::string HTTPRequest::_methodDELETE_("DELETE");
const std::string HTTPRequest::_methodTRACE_("TRACE");
const std::string HTTPRequest::_methodCONNECT_("CONNECT");
const std::string HTTPRequest::_methodUNKNOWN_("UNKNOWN");

#ifdef USE_ACCOUNTANT
int HTTPRequest::_dataType_ = 0;
std::string HTTPRequest::_dataTypeName_("comm::HTTPRequest");
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
HTTPRequest::HTTPRequest() :
	method_(INVALID_METHOD)
{
	// init
	initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
HTTPRequest::HTTPRequest(const std::string& uri, RequestMethod method) :
	method_(method),
	uri_(uri)
{
	// init
	initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
HTTPRequest::HTTPRequest(const std::string& loc, const std::string& query, RequestMethod method) :
	method_(method),
	uri_(loc + query)
{
	// init
	initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
HTTPRequest::HTTPRequest(const std::string& uri, const std::string& contentType, const std::string& content, RequestMethod method) :
	method_(method),
	uri_(uri),
	contentType_(contentType),
	content_(content)
{
	// init
	initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
HTTPRequest::HTTPRequest(const std::string& loc, const std::string& query, const std::string& contentType, const std::string& content, RequestMethod method) :
	method_(method),
	uri_(loc + query),
	contentType_(contentType),
	content_(content)
{
	// init
	initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
HTTPRequest::HTTPRequest(const _Myt& other)
{
	// init
	initialize();
	copy(other);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
HTTPRequest::~HTTPRequest()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
HTTPRequest& HTTPRequest::operator = ( const _Myt& rhs )
{
	copy(rhs);
	return *this;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
const std::string& HTTPRequest::getMethodName() const
{
	switch ( method_ )
	{
	case OPTIONS_METHOD:
		return _methodOPTIONS_;
	case GET_METHOD:
		return _methodGET_;
	case HEAD_METHOD:
		return _methodHEAD_;
	case POST_METHOD:
		return _methodPOST_;
	case PUT_METHOD:
		return _methodPUT_;
	case DELETE_METHOD:
		return _methodDELETE_;
	case TRACE_METHOD:
		return _methodTRACE_;
	case CONNECT_METHOD:
		return _methodCONNECT_;
	default:
		break;
	}

	return _methodUNKNOWN_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool HTTPRequest::isValid() const
{
	if ( method_ <= INVALID_METHOD || LAST_METHOD <= method_ )
		return false;

	if ( contentType_.empty() ^ content_.empty() )
		return false;

	return uri_.isValid();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
const std::string& HTTPRequest::buildRequestBuffer() const
{
	if ( changed_ )
	{
		std::string line;

		/* First put together the request line. */
		line.append(getMethodName()).append(1, ' ');

		line.append(getAbsolutePath());

		if ( getQuery().size() )
			line.append(1, '?').append(getQuery());

		if ( getFragment().size() )
			line.append(1, '#').append(getFragment());
		line.append(1, ' ');

		line.append("HTTP/").append(util_string::inttostring(getMajorVersion()));
		line.append(1, '.').append(util_string::inttostring(getMinorVersion()));
		line.append(1, '\x0D').append(1, '\x0A');

		requestBuffer_.append(line);


		/* Now add any necessary headers. */
		line.clear();
		line.append("Host: ").append(getHost());
		if ( getPort().size() )
			line.append(1, ':').append(getPort());
		line.append(1, '\x0D').append(1, '\x0A');

		requestBuffer_.append(line);


		/* Add header for content (if present). */
		if (( contentType_.length() ) && ( content_.length() ))
		{
			line.clear();
			line.append("Content-Type: ").append(getContentType());
			line.append(1, '\x0D').append(1, '\x0A');

			requestBuffer_.append(line);


			line.clear();
			line.append("Content-Length: ").append(util_string::inttostring(getContent().length()));
			line.append(1, '\x0D').append(1, '\x0A');

			requestBuffer_.append(line);
		}


		/* Now add the final empty line. */
		line.clear();
		line.append(1, '\x0D').append(1, '\x0A');

		requestBuffer_.append(line);


		/* Add content (if present). */
		if (( contentType_.length() ) && ( content_.length() ))
		{
			requestBuffer_.append(getContent());
		}


		changed_ = false;
	}

	return requestBuffer_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void HTTPRequest::showDebug() const
{
	Debug::trace("HTTPClient.request", buildRequestBuffer().c_str());
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void HTTPRequest::read( std::istream &is )
{
	size_t len;
	char c;

	contentType_.clear();
	content_.clear();

	is.read( (char*) &method_, sizeof(method_) );

	is >> uri_;

	// read until NULL
	for ( ;; )
	{
		is.read( &c, 1 );

		if ( !c )
			break;

		contentType_.push_back( c );
	}

	is.read( (char*) &len, sizeof(len) );

	while ( len-- )
	{
		is.read( &c, 1 );
		content_.push_back( c );
	}

	is.read( (char*) &stamp_, sizeof(stamp_) );

	changed_ = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void HTTPRequest::write( std::ostream &os ) const
{
	size_t len;

	os.write( (char*) &method_, sizeof(method_) );

	os << uri_;

	os.write( contentType_.c_str(), contentType_.length()+1 );

	len = content_.length();
	os.write( (char*) &len, sizeof(len) );
	os.write( content_.c_str(), len );

	os.write( (char*) &stamp_, sizeof(stamp_) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void HTTPRequest::initialize()
{
	majorVersion_ = _DefaultHTTPMajorVersion_;
	minorVersion_ = _DefaultHTTPMinorVersion_;

	stamp_ = time( NULL );

	changed_ = true;

#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void HTTPRequest::copy(const _Myt& other)
{
	method_ = other.method_;

	uri_ = other.uri_;

	contentType_ = other.contentType_;
	content_ = other.content_;

	majorVersion_ = other.majorVersion_;
	minorVersion_ = other.minorVersion_;

	changed_ = other.changed_;
	location_ = other.location_;
	requestBuffer_ = other.requestBuffer_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // comm
