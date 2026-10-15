/**
 * @file WrapperAuditLog.h
 * @brief WrapperAuditLog table support.
 *
 * @copyright Copyright (c) 2017, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef DBOWRAPPERAUDITLOG_H
#define DBOWRAPPERAUDITLOG_H

#include "TableBase.h"

#if defined ( __linux__ )
#include <Util/Dict.h>
#include <Util/Timestamp.h>
#else
#include "../Util/Dict.h"
#include "../Util/Timestamp.h"
#endif // __linux_


struct sqlite3_stmt;

/// Database objects namespace.
namespace dbo
{

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Wrapper audit log row.
struct WrapperAuditLogRow
{
	/// Timestamp type.
	typedef util::Timestamp timestamp_type;

	/// Event severities.
	enum severityType
	{
		Critical = 0,
		Error,
		Warning,
		Info
	};

	/// Entry sources.
	enum sourceType
	{
		Unknown = 0,								///< Unknown source.
		Timings,									///< Timings event.
		Film,										///< Film event.
		ServiceVisit = 99,							///< Service visit.
	};

	static const int Timings_Reset = 0;
	static const int Timings_Modified = 1;

	static const int Film_RollWidthModified = 0;
	static const int Film_RollChanged = 1;

	static const int ServiceVisit_ResolveError = 0;
	static const int ServiceVisit_Start = 1;
	static const int ServiceVisit_Stop = 2;

	timestamp_type timestamp;						///< Timestamp (ms since epoch).
	severityType severity;							///< Event severity.
	sourceType source;								///< Source type.
	int sourceExt;									///< Source extended.
	std::string description;						///< Event description.

	/// Bind columns to sqlite statement.
	/**
	 * @param[in] stmt  sqlite statement
	 * @return  @c true upon success, @c false otherwise
	 */
	bool bind( sqlite3_stmt *stmt ) const;

	/// Extract column data from sqlite statement.
	/**
	 * @param[in] stmt  sqlite statement
	 * @return  @c true upon success, @c false otherwise
	 */
	bool extract( sqlite3_stmt *stmt );

	/// Retrieve row in dictionary format.
	/**
	 * @param[in,out] dict  row
	 */
	void retrieve( util::Dictionary &dict ) const;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Wrapper audit log.
class WrapperAuditLog : public TableBase<WrapperAuditLogRow>
{
	typedef WrapperAuditLog _Myt;
	typedef TableBase<WrapperAuditLogRow> _Mybase;

public:

	/// Timestamp type.
	typedef typename row::timestamp_type timestamp_type;

	// ========================================================================
	// CTOR / DTOR
	// ========================================================================

	/// Constructor.
	/**
	 * @param[in] db  pointer to database
	 */
	WrapperAuditLog( sqlite3 *db );

	/// Destructor.
	virtual ~WrapperAuditLog();

	// ========================================================================
	// Methods
	// ========================================================================

	/// Create audit log record.
	/**
	 * @param[in] timestamp  timestamp created
	 * @param[in] severity  severity
	 * @param[in] source  event source
	 * @param[in] sourceExt  extended event source, if applicable
	 * @param[in] description  description
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool create( const timestamp_type &timestamp, int severity, int source, int sourceExt, const std::string &description );

	/// Retrieve audit records.
	/**
	 * @param[in] age  record age (in days)
	 * @param[in] source  audit source
	 * @param[out] results  query results
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool queryByAge_FromSourceOnly( unsigned int age, int source, rows &results );

	/// Retrieve audit records.
	/**
	 * @param[in] age  record age (in days)
	 * @param[out] results  query results
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool queryByAge_AllRecords( unsigned int age, rows &results );

	/// Retrieve audit event.
	/**
	 * @param[in] timestamp  record to retrieve
	 * @param[in] result  event
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool retrieve( const timestamp_type &timestamp, row &result );

	// ========================================================================
	// Static Methods
	// ========================================================================

	/// Perform housekeeping.
	/**
	 * @param[in] db  pointer to database
	 * @param[in] timestamp  cull timestamp (records older will be removed)
	 * @return  @c true upon success, @c false otherwise
	 */
	static bool housekeeping( sqlite3 *db, const timestamp_type &timestamp );

private:

#ifdef USE_ACCOUNTANT
	static int dataType_;
	static std::string dataTypeName_;
#endif

	// not implemented
	WrapperAuditLog();

	// not implemented
	WrapperAuditLog( const _Myt& );

	// not implemented
	_Myt &operator = ( const _Myt& );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace

#endif // DBOWRAPPERAUDITLOG_H
