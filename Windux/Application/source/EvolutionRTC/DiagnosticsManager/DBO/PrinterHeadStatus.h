/**
 * @file PrinterHeadStatus.h
 * @brief PrinterHeadStatus table support.
 *
 * @copyright Copyright (c) 2019, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef DBOPRINTERHEADSTATUS_H
#define DBOPRINTERHEADSTATUS_H

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

/// Printer head status row.
struct PrinterHeadStatusRow
{
	/// Timestamp type.
	typedef util::Timestamp timestamp_type;

	timestamp_type timestamp;						///< Timestamp (ms since epoch).
	int printerNum;									///< Printer number.
	int counts;										///< Label counts.
	int status;										///< Head status.

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

/// Printer head status.
class PrinterHeadStatus : public TableBase<PrinterHeadStatusRow>
{
	typedef PrinterHeadStatus _Myt;
	typedef TableBase<PrinterHeadStatusRow> _Mybase;

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
	PrinterHeadStatus( sqlite3 *db );

	/// Destructor.
	virtual ~PrinterHeadStatus();

	// ========================================================================
	// Methods
	// ========================================================================

	/// Create record.
	/**
	 * @param[in] timestamp  timestamp created
	 * @param[in] printerNum  printer number
	 * @param[in] counts  label counts
	 * @param[in] status  head status
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool create( const timestamp_type &timestamp, int printerNum, int counts, int status );

	/// Retrieve records.
	/**
	 * @param[in] age  record age (in seconds)
	 * @param[in] printerNum  printer number
	 * @param[out] results  query results
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool queryByAge( unsigned int age, int printerNum, rows &results );

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
	PrinterHeadStatus();

	// not implemented
	PrinterHeadStatus( const _Myt& );

	// not implemented
	_Myt &operator = ( const _Myt& );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace

#endif // DBOPRINTERHEADSTATUS_H
