/**
 * @file CGIHTTPAdapter.h
 * Adapter for HTTP CGI calls.
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Santos Juan
 * @author Randy Blankley
 */

#ifndef CGIHTTPADAPTER_H
#define CGIHTTPADAPTER_H

#include "CGICallerI.h"

#include "http/HTTPClient.h"
#include "http/HTTPRequest.h"

#include <string>

namespace comm
{

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Adapter for HTTP CGI calls.
class CGIHTTPAdapter : public CGICallerI
{
	typedef CGIHTTPAdapter _Myt;
	typedef CGICallerI _Mybase;

public:

	// ========================================================================
	// CTOR / DTOR
	// ========================================================================

	CGIHTTPAdapter();
	virtual ~CGIHTTPAdapter();

	// ========================================================================
	// Methods
	// ========================================================================

	void performCGICall(const std::string& url, const std::string& payload = std::string());
	void reportResult(bool result);

private:
	/* --- Instance Data Members --- */
	Util::CSmartPtr<HTTPClient> client_;

	HTTPRequest::RequestMethod requestMethod_;
	std::string requestContentType_;


	/* --- Class Data Members --- */
	static const bool _ReportOnlyFailedCGICalls_ = false;
	static const int _HTTPWellKnownPort_ = 80;

#ifdef USE_ACCOUNTANT
	static int _dataType_;
	static std::string _dataTypeName_;
#endif

};

///////////////////////////////////////////////////////////////////////////////////////////////////

} // comm

#endif // CGIHTTPADAPTER_H
