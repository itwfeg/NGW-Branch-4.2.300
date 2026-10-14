/**
 * @file HTTPRequest.h
 * HTTP request class.
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Santos Juan
 * @author Randy Blankley
 */

#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include "HTTPURI.h"

#include <string>

namespace comm
{

///////////////////////////////////////////////////////////////////////////////////////////////////

/// HTTP request class.
class HTTPRequest
{
	typedef HTTPRequest _Myt;

public:

	typedef enum
	{
		INVALID_METHOD,

		OPTIONS_METHOD,
		GET_METHOD,
		HEAD_METHOD,
		POST_METHOD,
		PUT_METHOD,
		DELETE_METHOD,
		TRACE_METHOD,
		CONNECT_METHOD,

		LAST_METHOD
	} RequestMethod;

	// ========================================================================
	// CTOR / DTOR
	// ========================================================================

	HTTPRequest();
	HTTPRequest(const std::string& uri, RequestMethod method = GET_METHOD);
	HTTPRequest(const std::string& loc, const std::string& query, RequestMethod method = GET_METHOD);
	HTTPRequest(const std::string& uri, const std::string& contentType, const std::string& content, RequestMethod method = POST_METHOD);
	HTTPRequest(const std::string& loc, const std::string& query, const std::string& contentType, const std::string& content, RequestMethod method = POST_METHOD);
	HTTPRequest(const _Myt& other);
	~HTTPRequest();

	// ========================================================================
	// Operators
	// ========================================================================

	_Myt& operator = (const _Myt& rhs);

	// ========================================================================
	// Properties
	// ========================================================================

	RequestMethod getMethod() const { return method_; }
	void setMethod(RequestMethod method) { method_ = method; changed_ = true; }

	const std::string& getMethodName() const;

	const std::string& getLocation() const { return location_; }
	void setLocation(const std::string& loc) { uri_.setLocation(loc); changed_ = true; calculateLocation(); }

	const std::string& getHost() const { return uri_.getHost(); }
	void setHost(const std::string& host) { uri_.setHost(host); changed_ = true; }

	bool isHostIPAddress() const { return uri_.isHostIPAddress(); }

	const std::string& getPort() const { return uri_.getPort(); }
	void setPort(const std::string& port) { uri_.setPort(port); changed_ = true; }

	const std::string& getAbsolutePath() const { return uri_.getAbsolutePath(); }
	void setAbsolutePath(const std::string& path) { uri_.setAbsolutePath(path); changed_ = true; }

	const std::string& getQuery() const { return uri_.getQuery(); }
	void setQuery(const std::string& query) { uri_.setQuery(query); changed_ = true; }

	const std::string& getFragment() const { return uri_.getFragment(); }
	void setFragmet(const std::string& fragment) { uri_.setFragment(fragment); changed_ = true; }

	int getMajorVersion() const { return majorVersion_; }
	void setMajorVersion(int ver) { majorVersion_ = ver; changed_ = true; }

	int getMinorVersion() const { return minorVersion_; }
	void setMinorVersion(int ver) { minorVersion_ = ver; changed_ = true; }

	const std::string& getContentType() const { return contentType_; }
	void setContentType(const std::string& contentType) { contentType_ = contentType; changed_ = true; }

	const std::string& getContent() const { return content_; }
	void setContent(const std::string& content) { content_ = content; changed_ = true; }

	const time_t& getTimestamp() const { return stamp_; }
	void setTimestamp(const time_t& stamp) { stamp_ = stamp; }

	bool isRequestEntity() const { return false; }

	bool isValid() const;

	// ========================================================================
	// Methods
	// ========================================================================

	const std::string& buildRequestBuffer() const;

	void showDebug() const;

    void read( std::istream &is );
    void write( std::ostream &os ) const;

private:

	/* --- Instance Data Members --- */
	RequestMethod method_;

	HTTPURI uri_;

	std::string contentType_;
	std::string content_;

	int majorVersion_;
	int minorVersion_;

	time_t stamp_;

	mutable bool changed_;
	mutable std::string location_;
	mutable std::string requestBuffer_;


	/* --- Instance Function Members --- */
	void initialize();
	void copy(const _Myt& other);
	void calculateLocation();


	/* --- Class Data Members --- */
	static const int _DefaultHTTPMajorVersion_ = 1;
	static const int _DefaultHTTPMinorVersion_ = 1;

	static const std::string _methodOPTIONS_;
	static const std::string _methodGET_;
	static const std::string _methodHEAD_;
	static const std::string _methodPOST_;
	static const std::string _methodPUT_;
	static const std::string _methodDELETE_;
	static const std::string _methodTRACE_;
	static const std::string _methodCONNECT_;
	static const std::string _methodUNKNOWN_;

#ifdef USE_ACCOUNTANT
	static int _dataType_;
	static std::string _dataTypeName_;
#endif

};

/// Insertion operator.
/**
 * @param[in,out] lhs  stream
 * @param[in] rhs  object
 * @return  reference to lhs
 */
inline std::ostream& operator << ( std::ostream &lhs, const HTTPRequest &rhs )
{
    rhs.write( lhs );
    return lhs;
}

/// Extraction operator.
/**
 * @param[in,out] lhs  stream
 * @param[in] rhs  object
 * @return  reference to lhs
 */
inline std::istream& operator >> ( std::istream &lhs, HTTPRequest &rhs )
{
    rhs.read( lhs );
    return lhs;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // comm

#endif // HTTPREQUEST_H
