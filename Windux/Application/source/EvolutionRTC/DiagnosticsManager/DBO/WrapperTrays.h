/**
 * @file WrapperTrays.h
 * @brief Wrapper trays table support.
 *
 * @copyright Copyright (c) 2017, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef DBOWRAPPERTRAYS_H
#define DBOWRAPPERTRAYS_H

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

/// Wrapper trays row.
struct WrapperTraysRow
{
	std::string name;								///< Tray name.
	unsigned int plu;								///< PLU Exception.

	unsigned int lengthMin;							///< Min length.
	unsigned int lengthMax;							///< Max length.
	unsigned int widthMin;							///< Min width.
	unsigned int widthMax;							///< Max width.
	unsigned int lip;								///< Lip height.

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
	bool operator == ( const WrapperTraysRow &rhs ) const;

	/// Comparison operator.
	/**
	 * @param[in] rhs  object to compare to
	 * @return  @c true if not equal, @c false otherwise
	 */
	bool operator != ( const WrapperTraysRow &rhs ) const {return ((rhs == *this) ? false : true);}

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Wrapper trays.
class WrapperTrays : public TableBase<WrapperTraysRow>
{
	typedef WrapperTrays _Myt;
	typedef TableBase<WrapperTraysRow> _Mybase;

public:

	// ========================================================================
	// CTOR / DTOR
	// ========================================================================

	/// Constructor.
	/**
	 * @param[in] db  pointer to database
	 */
	WrapperTrays( sqlite3 *db );

	/// Destructor.
	virtual ~WrapperTrays();

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
	virtual bool retrieve( const std::string &name, unsigned int plu, row &result );

	/// Retrieve all records.
	/**
	 * @param[out] result  query result
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool retrieveAll( rows &result );

	/// Save record.
	/**
	 * @param[in] value  row
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool save( const row &value );

private:

#ifdef USE_ACCOUNTANT
	static int dataType_;
	static std::string dataTypeName_;
#endif

	// not implemented
	WrapperTrays();

	// not implemented
	WrapperTrays( const _Myt& );

	// not implemented
	_Myt &operator = ( const _Myt& );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace

#endif // DBOWRAPPERTRAYS_H
