/**
 * @file ServiceMaintLog.h
 * @brief ServiceMaintLog table support.
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef DBOSERVICEMAINTLOG_H
#define DBOSERVICEMAINTLOG_H

#include "TableBase.h"

#if defined ( __linux__ )
#include <Util/Dict.h>
#include <Util/Timestamp.h>
#else
#include "../Util/Dict.h"
#include "../Util/Timestamp.h"
#endif // __linux__


struct sqlite3_stmt;

/// Database objects namespace.
namespace dbo
{

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Service maint log row.
struct ServiceMaintLogRow
{
	/// Timestamp type.
	typedef util::Timestamp timestamp_type;

	/// States.
	enum stateType
	{
		Open = 0,									///< Open maintenance item.
		Ignored,									///< Ignored maintenance item.
		ResolvedByService,							///< Resolved maintenance item.
		ResolvedByDiag,								///< Resolved by diagnostics over time.
		Archived									///< Archived due to age.
	};

	timestamp_type timestamp;						///< Timestamp (ms since epoch).
	int errorCode;									///< Error code to diagnose.
	stateType state;								///< State type.
	int count;										///< Number of times error has occurred.
	timestamp_type latestOccurrence;				///< Timestamp of last occurrence (ms since epoch).
	timestamp_type latestDiag;						///< Timestamp of last diagnostic execution (ms since epoch).
	timestamp_type resolved;						///< Timestamp when resolved/ignored/etc. (ms since epoch).
	int serviceMethod1;								///< Service method.
	int servicePart1;								///< Service part.
	int serviceMethod2;								///< Service method.
	int servicePart2;								///< Service part.
	int serviceMethod3;								///< Service method.
	int servicePart3;								///< Service part.
	int serviceMethod4;								///< Service method.
	int servicePart4;								///< Service part.
	std::string serviceNotes;						///< Service notes.

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

/// Service maint log.
class ServiceMaintLog : public TableBase<ServiceMaintLogRow>
{
	typedef ServiceMaintLog _Myt;
	typedef TableBase<ServiceMaintLogRow> _Mybase;

public:

	/// Timestamp type.
	typedef typename row::timestamp_type timestamp_type;

	/// Service method and part.
	typedef std::pair<int, int> serviceMethodPart;

	/// List of service methods and parts.
	typedef std::list<serviceMethodPart> serviceMethodPartList;

	// ========================================================================
	// CTOR / DTOR
	// ========================================================================

	/// Constructor.
	/**
	 * @param[in] db  pointer to database
	 */
	ServiceMaintLog( sqlite3 *db );

	/// Destructor.
	virtual ~ServiceMaintLog();

	// ========================================================================
	// Methods
	// ========================================================================

	/// Create maintenance record.
	/**
	 * @param[in] timestamp  timestamp created
	 * @param[in] errorCode  error code
	 * @param[in] diagStep  diagnostic step, if applicable
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool create( const timestamp_type &timestamp, int errorCode, const timestamp_type &diagStep = timestamp_type() );

	/// Retrieve maint records.
	/**
	 * @param[in] age  record age (in days)
	 * @param[out] results  query results
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool queryByAge_OpenRecordsOnly( unsigned int age, rows &results );

	/// Retrieve maint records.
	/**
	 * @param[in] age  record age (in days)
	 * @param[out] results  query results
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool queryByAge_ResolvedRecordsOnly( unsigned int age, rows &results );

	/// Retrieve maint records.
	/**
	 * @param[in] age  record age (in days)
	 * @param[out] results  query results
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool queryByAge_AllRecords( unsigned int age, rows &results );

	/// Retrieve maint records.
	/**
	 * @param[in] errorCode  error code
	 * @param[out] results  query results
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool queryByErrorCode( int errorCode, rows &results );

	/// Retrieve event.
	/**
	 * @param[in] timestamp  record to retrieve
	 * @param[in] result  event
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool retrieve( const timestamp_type &timestamp, row &result );

	/// Update record with service information.
	/**
	 * @param[in] timestamp  record to update
	 * @param[in] serviceActions  service actions for resolution
	 * @param[in] serviceNotes  service notes
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool updateServiceInfo( const timestamp_type &timestamp, const serviceMethodPartList &serviceActions, const std::string &serviceNotes );

	/// Update record state.
	/**
	 * @param[in] timestamp  item to update
	 * @param[in] newState  new state
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool updateState( const timestamp_type &timestamp, row::stateType newState );

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
	ServiceMaintLog();

	// not implemented
	ServiceMaintLog( const _Myt& );

	// not implemented
	_Myt &operator = ( const _Myt& );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace

#endif // DBOSERVICEMAINTLOG_H
