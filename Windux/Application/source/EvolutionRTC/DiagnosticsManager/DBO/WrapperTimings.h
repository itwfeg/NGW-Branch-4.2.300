/**
 * @file WrapperTimings.h
 * @brief Wrapper height box and timings table support.
 *
 * @copyright Copyright (c) 2017, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef DBOWRAPPERTIMINGS_H
#define DBOWRAPPERTIMINGS_H

#include "TableBase.h"

#if defined ( __linux__ )
#include <Util/Dict.h>
#else
#include "../Util/Dict.h"
#endif // __linux__


struct sqlite3_stmt;

/// Database objects namespace.
namespace dbo
{

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Wrapper timings row.
struct WrapperTimingsRow
{
	std::string name;								///< Tray name.
	unsigned int plu;								///< PLU Exception.
	unsigned int height;							///< Height.

	unsigned int param;								///< Parameter id.
	unsigned int paramValue;						///< Parameter value.

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

	/// Comparison operator.
	/**
	 * @param[in] rhs  object to compare to
	 * @return  @c true if equal, @c false otherwise
	 */
	bool operator == ( const WrapperTimingsRow &rhs ) const;

	/// Comparison operator.
	/**
	 * @param[in] rhs  object to compare to
	 * @return  @c true if not equal, @c false otherwise
	 */
	bool operator != ( const WrapperTimingsRow &rhs ) const {return ((rhs == *this) ? false : true);}

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Wrapper timings.
class WrapperTimings : public TableBase<WrapperTimingsRow>
{
	typedef WrapperTimings _Myt;
	typedef TableBase<WrapperTimingsRow> _Mybase;

public:

	// ========================================================================
	// CTOR / DTOR
	// ========================================================================

	/// Constructor.
	/**
	 * @param[in] db  pointer to database
	 */
	WrapperTimings( sqlite3 *db );

	/// Destructor.
	virtual ~WrapperTimings();

	// ========================================================================
	// Methods
	// ========================================================================

	/// Delete all records.
	/**
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool deleteAll();

	/// Retrieve record.
	/**
	 * @param[in] name  tray name
	 * @param[in] plu  plu exception
	 * @param[out] result  query result
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool retrieve( const std::string &name, unsigned int plu, rows &result );

	/// Save record.
	/**
	 * @param[in] values  rows
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool save( const rows &values );

private:

#ifdef USE_ACCOUNTANT
	static int dataType_;
	static std::string dataTypeName_;
#endif

	// not implemented
	WrapperTimings();

	// not implemented
	WrapperTimings( const _Myt& );

	// not implemented
	_Myt &operator = ( const _Myt& );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace

#endif // DBOWRAPPERTIMINGS_H
