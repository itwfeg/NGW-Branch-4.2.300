/**
 * @file HTTPClientError.h
 * HTTP client error event.
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Santos Juan
 * @author Randy Blankley
 */

#ifndef HTTPCLIENTERROR_H
#define HTTPCLIENTERROR_H

#include "HTTPClientEvent.h"

#include <string>

namespace comm
{

///////////////////////////////////////////////////////////////////////////////////////////////////

/// HTTP client error event.
class HTTPClientError : public HTTPClientEvent
{
	typedef HTTPClientError _Myt;
	typedef HTTPClientEvent _Mybase;

public:

	// ========================================================================
	// CTOR / DTOR
	// ========================================================================

	HTTPClientError(HTTPClientEvent::HTTPError error);
	virtual ~HTTPClientError();

	// ========================================================================
	// Properties
	// ========================================================================

	HTTPClientEvent::HTTPError getError() const { return errorType_; }

	const std::string& getErrorText() const;

private:
	/* --- Instance data members --- */
	HTTPClientEvent::HTTPError errorType_;


	/* --- Class data members --- */
	static const std::string _NoErrorErrorText_;
	static const std::string _CannotCreateSocketErrorText_;
	static const std::string _CannotConnectSocketErrorText_;
	static const std::string _WritingToSocketErrorText_;
	static const std::string _TimeoutReadingResponseErrorText_;
	static const std::string _InvalidCharacterResponseErrorText_;
	static const std::string _ResponseLineTooLongErrorText_;
	static const std::string _InvalidResponseErrorText_;
	static const std::string _EmptyResponseErrorText_;
	static const std::string _ResponseNotSuccessErrorText_;
	static const std::string _TimeoutReadingEntityErrorText_;
	static const std::string _ReadingFromSocketErrorText_;
	static const std::string _UnknownErrorText_;

#ifdef USE_ACCOUNTANT
	static int _dataType_;
	static std::string _dataTypeName_;
#endif

};

///////////////////////////////////////////////////////////////////////////////////////////////////

} // comm

#endif // HTTPCLIENTERROR_H
