/**
 * @file HTTPClientError.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Santos Juan
 * @author Randy Blankley
 */

#include "Debug.h"
#include "HTTPClientError.h"

#include "util/e_string/utilstring.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"
#endif

namespace comm
{

const std::string HTTPClientError::_NoErrorErrorText_("No Error");
const std::string HTTPClientError::_CannotCreateSocketErrorText_("Cannot Create Socket");
const std::string HTTPClientError::_CannotConnectSocketErrorText_("Cannot Connect Socket");
const std::string HTTPClientError::_WritingToSocketErrorText_("Error Writing To Socket");
const std::string HTTPClientError::_TimeoutReadingResponseErrorText_("Timeout Reading Response");
const std::string HTTPClientError::_InvalidCharacterResponseErrorText_("Invalid Character Response");
const std::string HTTPClientError::_ResponseLineTooLongErrorText_("Response Line Too Long");
const std::string HTTPClientError::_InvalidResponseErrorText_("Invalid Response");
const std::string HTTPClientError::_EmptyResponseErrorText_("Empty Response");
const std::string HTTPClientError::_ResponseNotSuccessErrorText_("Response Not Success");
const std::string HTTPClientError::_TimeoutReadingEntityErrorText_("Timeout Reading Entity");
const std::string HTTPClientError::_UnknownErrorText_("Unknown Error");
const std::string HTTPClientError::_ReadingFromSocketErrorText_("Error Reading From Socket");

#ifdef USE_ACCOUNTANT
int HTTPClientError::_dataType_ = 0;
std::string HTTPClientError::_dataTypeName_("comm::HTTPClientError");
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
HTTPClientError::HTTPClientError(HTTPClientEvent::HTTPError error) :
	_Mybase(ERROR_EVENT),
	errorType_(error)
{
#ifdef USE_ACCOUNTANT
	if ( !_dataType_ )
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
HTTPClientError::~HTTPClientError()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
const std::string& HTTPClientError::getErrorText() const
{
	switch ( errorType_ )
	{
	case HTTPERROR_NO_ERROR:
		return _NoErrorErrorText_;
	case HTTPERROR_CANNOT_CREATE_SOCKET:
		return _CannotCreateSocketErrorText_;
	case HTTPERROR_CANNOT_CONNECT_SOCKET:
		return _CannotConnectSocketErrorText_;
	case HTTPERROR_WRITING_TO_SOCKET:
		return _WritingToSocketErrorText_;
	case HTTPERROR_TIMEOUT_READING_RESPONSE:
		return _TimeoutReadingResponseErrorText_;
	case HTTPERROR_INVALID_CHAR_RESPONSE:
		return _InvalidCharacterResponseErrorText_;
	case HTTPERROR_RESPONSE_LINE_TOO_LONG:
		return _ResponseLineTooLongErrorText_;
	case HTTPERROR_INVALID_RESPONSE:
		return _InvalidResponseErrorText_;
	case HTTPERROR_EMPTY_RESPONSE:
		return _EmptyResponseErrorText_;
	case HTTPERROR_RESPONSE_NOT_SUCCESS:
		return _ResponseNotSuccessErrorText_;
	case HTTPERROR_TIMEOUT_READING_ENTITY:
		return _TimeoutReadingEntityErrorText_;
	case HTTPERROR_READING_FROM_SOCKET:
		return _ReadingFromSocketErrorText_;
	default:
		break;
	}

	return _UnknownErrorText_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // comm
