/**
 * @file HTTPURI.h
 * HTTP URI class.
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Santos Juan
 * @author Randy Blankley
 */

#ifndef HTTPURI_H
#define HTTPURI_H

#include "CSmartPtr.h"

#include <list>
#include <map>
#include <string>

namespace comm
{

///////////////////////////////////////////////////////////////////////////////////////////////////

/// HTTP URI class.
class HTTPURI
{
	typedef HTTPURI _Myt;

public:

	typedef std::map<std::string, std::string> parameterContainer_type;
	typedef parameterContainer_type::iterator parameterContainer_iterator;

	typedef std::list<std::string> parameterNamesContainer_type;
	typedef parameterNamesContainer_type::const_iterator parameterNames_iterator;

	// ========================================================================
	// CTOR / DTOR
	// ========================================================================

	HTTPURI();
	HTTPURI(const std::string& uri);
	HTTPURI(const _Myt& other);
	~HTTPURI();

	// ========================================================================
	// Operators
	// ========================================================================

	_Myt& operator = ( const _Myt& rhs );

	// ========================================================================
	// Properties
	// ========================================================================

	void setLocation(const std::string& loc) { parseAbsoluteURI(loc); }

	const std::string& getHost() const { return host_; }
	void setHost(const std::string& host) { host_ = host; }

	bool isHostIPAddress() const { return ipHost_; }

	const std::string& getPort() const { return port_; }
	void setPort(const std::string& port) { port_ = port; }

	const std::string& getAbsolutePath() const { return path_; }
	void setAbsolutePath(const std::string& path) { path_ = path; }

	const std::string& getQuery() const { return query_; }
	void setQuery(const std::string& query) { query_ = query; queryNeedsParsing_ = true; }

	const std::string& getFragment() const { return fragment_; }
	void setFragment(const std::string& fragment) { fragment_ = fragment; }

	bool isValid() const { return valid_; }

	bool isQueryParameter(const std::string& param) { return queryParameters_.find(param) != queryParameters_.end(); }
	const std::string& getQueryParameter(const std::string& param)
			{ return isQueryParameter(param) ? queryParameters_[param] : _emptyParameter_; }
	void setQueryParameter(const std::string& param, const std::string& value);

	// ========================================================================
	// Methods
	// ========================================================================

	Util::CSmartPtr<std::string> formatURI() const;

	parameterNames_iterator queryBegin() const { if ( queryNeedsParsing_ ) parseQuery(); return queryParameterNames_.begin(); }
	parameterNames_iterator queryEnd() const { return queryParameterNames_.end(); }

    void read( std::istream &is );
    void write( std::ostream &os ) const;

private:

	/* --- Instance Data Members --- */
	std::string host_;
	std::string port_;
	std::string path_;
	std::string query_;
	std::string fragment_;
	bool valid_;

	mutable bool ipHost_;

	// queryNeedsParsing_: true means that the query has been updated but the parsing
	//   has not been performed yet; false otherwise
	mutable bool queryNeedsParsing_;

	mutable parameterContainer_type queryParameters_;
	mutable parameterNamesContainer_type queryParameterNames_;


	/* --- Instance Function Members --- */
	void initialize();
	void reset();
	void copy(const _Myt& other);
	void parseAbsoluteURI(const std::string& location);
	void parseQuery() const;
	void rebuildQuery();

	bool validateURIHost(const std::string& host);
	bool validateURIPort(const std::string& port);
	bool validateURIPath(const std::string& path, bool absolute = true);
	bool validateURIQuery(const std::string& query);
	bool validateURIFragment(const std::string& fragment);
	bool isValidEscapeSequence(const std::string& s, std::string::size_type off, std::string::size_type end);


	/* --- Class Data Members --- */
	static const std::string _HTTPScheme_;

	static const std::string _validSchemeMarks_;
	static const std::string _validPathMarks_;
	static const std::string _validQueryMarks_;
	static const std::string _validFragmentMarks_;

	static const std::string _emptyParameter_;

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
inline std::ostream& operator << ( std::ostream &lhs, const HTTPURI &rhs )
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
inline std::istream& operator >> ( std::istream &lhs, HTTPURI &rhs )
{
    rhs.read( lhs );
    return lhs;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

} // comm

#endif // HTTPURI_H
