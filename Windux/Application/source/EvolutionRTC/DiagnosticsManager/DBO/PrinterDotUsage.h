/**
 * @file PrinterDotUsage.h
 * @brief PrinterDotUsage table support.
 *
 * @copyright Copyright (c) 2019, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef DBOPRINTERDOTUSAGE_H
#define DBOPRINTERDOTUSAGE_H

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

/// Printer dot usage row.
struct PrinterDotUsageRow
{
	/// Timestamp type.
	typedef util::Timestamp timestamp_type;

	timestamp_type timestamp;						///< Timestamp (ms since epoch).
	int printerNum;									///< Printer number.
	int dotNum;										///< Head dot number (0 = top of head).
	int printed;									///< Number of dots printed.
	int removed;									///< Number of dots removed from label (due to code checker).

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

/// Printer dot usage.
class PrinterDotUsage : public TableBase<PrinterDotUsageRow>
{
	typedef PrinterDotUsage _Myt;
	typedef TableBase<PrinterDotUsageRow> _Mybase;

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
	PrinterDotUsage( sqlite3 *db );

	/// Destructor.
	virtual ~PrinterDotUsage();

	// ========================================================================
	// Methods
	// ========================================================================

	/// Create record.
	/**
	 * @param[in] timestamp  timestamp created
	 * @param[in] printerNum  printer number
	 * @param[in] dotNum  head dot number
	 * @param[in] printed  number of dots printed
	 * @param[in] removed  number of dots removed from label
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool create( const timestamp_type &timestamp, int printerNum, int dotNum, int printed, int removed );

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
	PrinterDotUsage();

	// not implemented
	PrinterDotUsage( const _Myt& );

	// not implemented
	_Myt &operator = ( const _Myt& );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace

#endif // DBOPRINTERDOTUSAGE_H
