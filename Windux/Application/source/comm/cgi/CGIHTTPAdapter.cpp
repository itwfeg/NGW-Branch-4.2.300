/**
 * @file CGIHTTPAdapter.h
 * Adapter for HTTP CGI calls.
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Santos Juan
 * @author Randy Blankley
 */

#include "CGICallPerformedMessage.h"
#include "CGIHTTPAdapter.h"
#include "CMessageCenter.h"
#include "Configuration.h"

#ifdef USE_ACCOUNTANT
#include "util/accountant/accountanti.h"
#endif

namespace comm
{

#ifdef USE_ACCOUNTANT
int CGIHTTPAdapter::_dataType_ = 0;
std::string CGIHTTPAdapter::_dataTypeName_("comm::CGIHTTPAdapter");
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
CGIHTTPAdapter::CGIHTTPAdapter() :
	client_(0),
	requestMethod_(HTTPRequest::INVALID_METHOD)
{
#ifdef USE_ACCOUNTANT
	if (_dataType_ == 0)
		_dataType_ = util_acc::registerDataType(_dataTypeName_);

	util_acc::addInstance(_dataType_);
#endif

	requestMethod_ =
		(HTTPRequest::RequestMethod)Configuration::getInstance()->toIntCfgI(CfgInternal::HTTPREQUESTMETHOD);
	requestContentType_ =
		Configuration::getInstance()->toStringCfgI(CfgInternal::HTTPREQUESTCONTENTTYPE);

	// Initialization of the actual client
	if ( Configuration::getInstance()->toBoolCfg(fieldOffsetCfgEnableProductionPlanning) )
		if ( client_.null() )
			client_ = new HTTPClient(*this);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
CGIHTTPAdapter::~CGIHTTPAdapter()
{
#ifdef USE_ACCOUNTANT
	util_acc::removeInstance(_dataType_);
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CGIHTTPAdapter::performCGICall(const std::string& url, const std::string& payload)
{
	// Client initialization
	if ( client_.null() )
		client_ = new HTTPClient(*this);

	Util::CSmartPtr<HTTPRequest> request;

	// FT-1694: FPP request with JSON payload
	if (( HTTPRequest::POST_METHOD == requestMethod_ ) && ( requestContentType_.length() ) && ( payload.length() ))
		request = new HTTPRequest( url, requestContentType_, payload, requestMethod_ );
	else
		request = new HTTPRequest( url, requestMethod_ );

	if ( request->isValid() )
	{
		Debug::trace("CGIHTTPAdapter", "Going to add new HTTP request to HTTP client");

		client_->addHTTPRequest(request);
	}
	else
	{
		Debug::trace("CGIHTTPAdapter", "HTTP Request is not valid. Reporting the corresponding result");

		reportResult(false);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void CGIHTTPAdapter::reportResult(bool result)
{
#ifdef _WIN32
#pragma warning(disable : 4127)
#endif //_WIN32
	if ( !result || !_ReportOnlyFailedCGICalls_ )
	{
		CGICallPerformedMessage message(result);

		Hobart::CMessageCenter::instance() << &message;
	}

	Debug::trace("CGIHTTPAdapter", "Received " + std::string(result ? "successful" : "failure") + " report of CGICall");
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // comm
