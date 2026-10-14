/**
 * @file ServiceCat.h
 * @brief Service category table support.
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef DBOSERVICECAT_H
#define DBOSERVICECAT_H

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

/// Service category row.
struct ServiceCatRow
{
	int id;										///< Id.
	std::string description;					///< Description.

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

/// Service category.
class ServiceCat : public TableBase<ServiceCatRow>
{
	typedef ServiceCat _Myt;
	typedef TableBase<ServiceCatRow> _Mybase;

public:

	// ========================================================================
	// CTOR / DTOR
	// ========================================================================

	/// Constructor.
	/**
	 * @param[in] db  pointer to database
	 */
	ServiceCat( sqlite3 *db );

	/// Destructor.
	virtual ~ServiceCat();

	// ========================================================================
	// Methods
	// ========================================================================

	/// Retrieve record.
	/**
	 * @param[in] id  id
	 * @param[out] result  query result
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool retrieve( int id, row &result );

private:

#ifdef USE_ACCOUNTANT
	static int dataType_;
	static std::string dataTypeName_;
#endif

	// not implemented
	ServiceCat();

	// not implemented
	ServiceCat( const _Myt& );

	// not implemented
	_Myt &operator = ( const _Myt& );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace

#endif // DBOSERVICECAT_H
