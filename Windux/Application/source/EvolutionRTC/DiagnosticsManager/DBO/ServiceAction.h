/**
 * @file ServiceAction.h
 * @brief Service action table support.
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef DBOSERVICEACTION_H
#define DBOSERVICEACTION_H

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

/// Service action row.
struct ServiceActionRow
{
	int category;								///< Category.
	int method;									///< Service method.
	int part;									///< Service part.
	int order;									///< Ordering for screen.

	std::string categoryName;					///< Category name.
	std::string methodName;						///< Service method name.
	std::string partName;						///< Service part name.
	bool notesRequired;							///< Notes required when @c true.

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

/// Service action.
class ServiceAction : public TableBase<ServiceActionRow>
{
	typedef ServiceAction _Myt;
	typedef TableBase<ServiceActionRow> _Mybase;

public:

	// ========================================================================
	// CTOR / DTOR
	// ========================================================================

	/// Constructor.
	/**
	 * @param[in] db  pointer to database
	 */
	ServiceAction( sqlite3 *db );

	/// Destructor.
	virtual ~ServiceAction();

	// ========================================================================
	// Methods
	// ========================================================================

	/// Query actions by category.
	/**
	 * @param[in] category  category
	 * @param[out] result  query result
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool queryByCategory( int category, rows &results );

private:

#ifdef USE_ACCOUNTANT
	static int dataType_;
	static std::string dataTypeName_;
#endif

	// not implemented
	ServiceAction();

	// not implemented
	ServiceAction( const _Myt& );

	// not implemented
	_Myt &operator = ( const _Myt& );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace

#endif // DBOSERVICEACTION_H
