#include "PAL_platformheader.h"

#include "HTTPResponse.h"



#include "util/e_string/utilstring.h"

#include "Debug.h"

#ifdef USE_ACCOUNTANT
	#include "util/accountant/accountanti.h"
#endif



namespace comm
{


	const std::string HTTPResponse::_HTTPVersion_("HTTP/");
	const std::string HTTPResponse::_validSeparators_("()<>@,;:\\\"/[]?={} \t");

#ifdef USE_ACCOUNTANT
	int HTTPResponse::_dataType_ = 0;
	std::string HTTPResponse::_dataTypeName_("comm::HTTPResponse"); 
#endif


	HTTPResponse::HTTPResponse(HTTPRequest::RequestMethod method)
		: requestMethod_(method)
	{
	#ifdef USE_ACCOUNTANT
		if ( !_dataType_ )
			_dataType_ = util_acc::registerDataType(_dataTypeName_);

		util_acc::addInstance(_dataType_);
	#endif

		init();
	}


	HTTPResponse::~HTTPResponse()
	{
	#ifdef USE_ACCOUNTANT
		util_acc::removeInstance(_dataType_);
	#endif
	}


	void HTTPResponse::init()
	{
		majorVersion_ = minorVersion_ = statusCode_ = 0;
		reason_.clear();
		headers_.clear();
		headerNames_.clear();
	}


	bool HTTPResponse::processStatusLine(const std::string& line)
	{
		int count = 0;
		std::string::size_type versionLength = _HTTPVersion_.size(), length = line.size();
		std::string::size_type separator, lwsSeparator;

		init();

		if ( !length )
			return false;


		/* First parse the HTTP version. */
		std::string::size_type p = 0;
		for ( ; p < versionLength; ++p )
		{
			if ( toupper(line[p]) != _HTTPVersion_[p] )
				break;
		}
		if ( p < versionLength )
			return false;

		lwsSeparator = findLinearWhiteSpace(line, versionLength, &count);
		if ( lwsSeparator != std::string::npos )
		{
			// Parse the major version number
			separator = line.find('.', versionLength);
			if ( separator != std::string::npos &&
					(versionLength < separator && separator < lwsSeparator) )
			{
				std::string version = line.substr(versionLength, separator - versionLength);
				if ( validateDigitChars(version) )
					majorVersion_ = util_string::stringtoint(version);
				else
					return false;
			}
			else
				return false;

			// Parse the minor version number
			if ( separator + 1 < lwsSeparator )
			{
				std::string version = line.substr(separator + 1, lwsSeparator - (separator + 1));
				if ( validateDigitChars(version) )
					minorVersion_ = util_string::stringtoint(version);
				else
					return false;
			}
			else
				return false;
		}
		else
			return false;


		/* Then parse the status code. */
		separator = lwsSeparator + count;
		lwsSeparator = findLinearWhiteSpace(line, separator, &count);
		if ( lwsSeparator == std::string::npos )
			lwsSeparator = length;
		if ( lwsSeparator == separator + 3 )
		{
			std::string status = line.substr(separator, 3);
			if ( validateDigitChars(status) )
				statusCode_ = util_string::stringtoint(line.substr(separator, 3));
			else
				return false;
		}
		else
			return false;


		/* Finally parse the reason/phrase. */
		separator = lwsSeparator + count;
		if ( separator < length )
			reason_ = line.substr(separator, length - separator);


		return true;
	}


	bool HTTPResponse::processHeaderLine(const std::string& line)
	{
		std::string::size_type length = line.size(), separator;
		std::string fieldName, fieldValue;

		if ( !length )
			return false;


		/* First parse the field name. */
		separator = line.find(':');
		if ( separator != std::string::npos )
		{
			fieldName = line.substr(0, separator);
			if ( !validateTokenChars(fieldName) )
				return false;
		}
		else
			return false;


		/* Then parse the field value. */
		++separator;
		if ( separator < length )
			fieldValue = line.substr(separator, length - separator);


		/* Keep track of the name-value pairs. */
		if ( !headers_.insert(make_pair(fieldName, fieldValue)).second )
			headers_[fieldName] = fieldValue;
		headerNames_.push_back(fieldName);


		return true;
	}


	bool HTTPResponse::isResponseEntity() const
	{
		/* The protocol specification says that "All responses to HEAD request
		method MUST NOT include a message-body, even when the presence of
		entity-header fields might lead one to believe they do." However,
		Microsoft-ISS/4.0 includes a message-body in responses to HEAD requests. */
		return !(requestMethod_ == HTTPRequest::HEAD_METHOD ||
			(100 <= statusCode_ && statusCode_ <= 199) || statusCode_ == 204 || statusCode_ == 304);
	}


	int HTTPResponse::getContentLength() const
	{
		if ( !isResponseEntity() )
			return 0;


		headerContainer_type::const_iterator iter = headers_.find("Transfer-Encoding");
		if ( iter != headers_.end() )
		{
			/* Not supported yet. */
			return -1;
		}
		else
		{
			iter = headers_.find("Content-Length");
			if ( iter != headers_.end() )
			{
				int count = 0;
				const std::string& value = iter->second;
				std::string::size_type length = value.size();
				std::string::size_type valuePos = 0, lwsSeparator = findLinearWhiteSpace(value, 0, &count);

				if ( lwsSeparator == 0 )
				{
					valuePos = count;
					lwsSeparator = findLinearWhiteSpace(value, valuePos, &count);
				}

				if ( lwsSeparator == std::string::npos )
					lwsSeparator = length;

				if ( valuePos < lwsSeparator )
					return util_string::stringtoint(value.substr(valuePos, lwsSeparator - valuePos));
			}
		}

		return -1;
	}


	bool HTTPResponse::validateFieldName(const std::string& fieldName)
	{
		std::string::size_type currentPos = 0;
		std::string::size_type length = fieldName.size();

		for ( ; currentPos < length; ++currentPos )
		{
			if ( !(isalnum(fieldName[currentPos]) || _validSeparators_.find(fieldName[currentPos]) != std::string::npos) )
				break;
		}

		return ( currentPos >= length );
	}


	bool HTTPResponse::validateTokenChars(const std::string& token)
	{
		std::string::size_type currentPos = 0;
		std::string::size_type length = token.size();

		for ( ; currentPos < length; ++currentPos )
		{
			if ( !(32 <= token[currentPos] && token[currentPos] < 127 &&
					_validSeparators_.find(token[currentPos]) == std::string::npos) )
				break;
		}

		return ( currentPos >= length );
	}


	bool HTTPResponse::validateDigitChars(const std::string& version)
	{
		std::string::size_type currentPos = 0;
		std::string::size_type length = version.size();

		for ( ; currentPos < length; ++currentPos )
		{
			if ( !isdigit(version[currentPos]) )
				break;
		}

		return ( currentPos >= length );
	}


	std::string::size_type HTTPResponse::findLinearWhiteSpace(const std::string& line, std::string::size_type off, int* count) const
	{
		bool inLWS = false;
		std::string::size_type lwsPos = std::string::npos, length = line.size();

		if ( !count )
			return lwsPos;

		for ( *count = 0; off < length; ++off )
		{
			if ( !inLWS )
			{
				if ( line[off] == '\x20' || line[off] == '\x09' )
				{
					lwsPos = off;
					++(*count);
					inLWS = true;
				}
			}
			else
			{
				if ( line[off] == '\x20' || line[off] == '\x09' )
					++(*count);
				else
					break;
			}

		}

		return lwsPos;
	}


	std::string HTTPResponse::dumpContents()
	{
		std::string buffer;


		/* Put together the status line. */
		buffer.append(_HTTPVersion_).append(util_string::inttostring(getMajorVersion())).append(1, '.');
		buffer.append(util_string::inttostring(getMinorVersion())).append(1, ' ');
		buffer.append(util_string::inttostring(getStatusCode())).append(1, ' ');
		buffer.append(reason_).append(1, '\x0D').append(1, '\x0A');


		/* Now put together the header lines. */
		headerNameContainer_type::const_iterator iter = headerNames_.begin();
		while ( iter != headerNames_.end() )
		{
			buffer.append(*iter).append(1, ':').append(headers_[*iter]).append(1, '\x0D').append(1, '\x0A');
			++iter;
		}


		/* Now a blank line. */
		buffer.append(1, '\x0D').append(1, '\x0A');


		return buffer;
	}


	void HTTPResponse::showDebug()
	{
		Debug::trace("HTTPResponse", "HTTP Response contents:");


		/* Put together the status line. */
		Debug::trace("HTTPResponse", "\t" + _HTTPVersion_ + util_string::inttostring(getMajorVersion()) + "." +
			util_string::inttostring(getMinorVersion()) + " " + util_string::inttostring(getStatusCode()) + " " + reason_);


		/* Now put together the header lines. */
		headerNameContainer_type::const_iterator iter = headerNames_.begin();
		while ( iter != headerNames_.end() )
		{
			Debug::trace("HTTPResponse", "\t" + *iter + ":" + headers_[*iter]);
			++iter;
		}
	}


}
