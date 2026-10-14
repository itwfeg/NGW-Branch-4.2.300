/**
 * @file EventLog.h
 * @brief EventLog table support.
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef DBOEVENTLOG_H
#define DBOEVENTLOG_H

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

/// Event log row.
struct EventLogRow
{
	/// Timestamp type.
	typedef util::Timestamp timestamp_type;

	/// Event severities.
	enum severityType
	{
		Unknown = 0,
		Info,
		Warning,
		Error,
		Critical
	};

	timestamp_type timestamp;					///< Timestamp (ms since epoch).
	int device;									///< Device type.
	int deviceExt;								///< Extended device type (i.e. printer number, node number).
	unsigned int deviceCycles;					///< How many cycles/counts device has.
	int errorCode;								///< Error code.
	severityType severity;						///< Severity.
	bool wizard;								///< Wizard available.

	/// Bind columns to sqlite statement.
	/**
	 * @param[in] stmt  sqlite statement
	 * @return  @c true upon success, @c false otherwise
	 */
	bool bind( sqlite3_stmt *stmt ) const;

	/// Retrieve diagnostic error code.
	/**
	 * @return  diag error code
	 */
	int diagErrorCode() const {return ((device * 1000) + errorCode);}

	/// Extract column data from sqlite statement.
	/**
	 * @param[in] stmt  sqlite statement
	 * @return  @c true upon success, @c false otherwise
	 */
	bool extract( sqlite3_stmt *stmt );

	/// Check if event is severe.
	/**
	 * @return  @c true if event is severe, @c false otherwise
	 */
	bool isSevereEvent() const {return Warning < severity;}

	/// Retrieve row in dictionary format.
	/**
	 * @param[in,out] dict  row
	 */
	void retrieve( util::Dictionary &dict ) const;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Event log.
class EventLog : public TableBase<EventLogRow>
{
	typedef EventLog _Myt;
	typedef TableBase<EventLogRow> _Mybase;

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
	EventLog( sqlite3 *db );

	/// Destructor.
	virtual ~EventLog();

	// ========================================================================
	// Methods
	// ========================================================================

	/// Create diagnostics event.
	/**
	 * @param[in] device  device
	 * @param[in] deviceExt  device extended
	 * @param[in] deviceCycles  device cycles/counts
	 * @param[in] errorCode  error code
	 * @param[in] forceWizard  force wizard for this event
	 * @param[in] severity  severity
	 * @return  event upon success, zero (0) otherwise
	 */
	virtual timestamp_type create( int device, int deviceExt, unsigned int deviceCycles, int errorCode, bool forceWizard = false, row::severityType severity = row::Unknown );

	/// Retrieve events.
	/**
	 * @param[in] age  age of events to retrieve (minutes)
	 * @param[out] results  query results
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool queryByAge( unsigned int age, rows &results );

	/// Retrieve events.
	/**
	 * @param[in] min  minimum device cycle count (inclusive)
	 * @param[in] max  maximum device cycle count (inclusive)
	 * @param[out] results  query results
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool queryByDeviceCycles( unsigned int min, unsigned int max, rows &results );

	/// Retrieve event.
	/**
	 * @param[in] timestamp  event to retrieve
	 * @param[out] result  event
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
	EventLog();

	// not implemented
	EventLog( const _Myt& );

	// not implemented
	_Myt &operator = ( const _Myt& );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace

#endif // DBOEVENTLOG_H
