/**
 * @file PrinterDotWear.h
 * @brief PrinterDotWear table support.
 *
 * @copyright Copyright (c) 2019, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef DBOPRINTERDOTWEAR_H
#define DBOPRINTERDOTWEAR_H

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

/// Printer dot wear row.
struct PrinterDotWearRow
{
	/// Timestamp type.
	typedef util::Timestamp timestamp_type;

	timestamp_type timestamp;						///< Timestamp (ms since epoch).
	int dotNum;										///< Head dot number (0 = top of head).
	int status;										///< Dot status.
	int value;										///< Dot resistance value.

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

/// Printer dot wear.
class PrinterDotWear : public TableBase<PrinterDotWearRow>
{
	typedef PrinterDotWear _Myt;
	typedef TableBase<PrinterDotWearRow> _Mybase;

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
	PrinterDotWear( sqlite3 *db );

	/// Destructor.
	virtual ~PrinterDotWear();

	// ========================================================================
	// Methods
	// ========================================================================

	/// Create record.
	/**
	 * @param[in] timestamp  timestamp created
	 * @param[in] dotNum  printer head dot number
	 * @param[in] status  dot status
	 * @param[in] value  dot resistance value
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool create( const timestamp_type &timestamp, int dotNum, int status, int value );

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
	PrinterDotWear();

	// not implemented
	PrinterDotWear( const _Myt& );

	// not implemented
	_Myt &operator = ( const _Myt& );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace

#endif // DBOPRINTERDOTWEAR_H
