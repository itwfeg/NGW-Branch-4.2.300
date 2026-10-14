/**
 * @file TableBase.h
 * @brief Base object for table and row support.
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef DBOTABLEBASE_H
#define DBOTABLEBASE_H

#include <string>
#include <vector>

struct sqlite3;

/// Database objects namespace.
namespace dbo
{

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Table row base object.
template <class _Row>
class TableBase
{
	typedef TableBase<_Row> _Myt;

public:

	/// Row type;
	typedef _Row row;

	/// Array of table rows.
	typedef std::vector<row> rows;

	// ========================================================================
	// DTOR
	// ========================================================================

	/// Destructor.
	virtual ~TableBase();

	// ========================================================================
	// Static Methods
	// ========================================================================

	/// Create database table.
	/**
	 * @param[in] db  pointer to database
	 * @return  @c true if table created, @c false otherwise
	 */
	static bool createTable( sqlite3 *db );

	/// Export database table.
	/**
	 * @param[in] db  pointer to database
	 * @return  @c true if table exported, @c false otherwise
	 */
	static bool exportTable( sqlite3 *db );

	/// Format row for printing.
	/**
	 * @param[in] value  row to format
	 * @param[in] csv  @c true for CSV format, @c false otherwise
	 * @return  formatted row
	 */
	static std::string formatRow( const row &value, bool csv = false );

protected:

	sqlite3 *db_;									///< Database.

	// ========================================================================
	// CTOR
	// ========================================================================

	/// Constructor.
	/**
	 * @param[in] db  pointer to database
	 */
	TableBase( sqlite3 *db );

	// ========================================================================
	// Methods
	// ========================================================================

	/// Insert row.
	/**
	 * @param[in] value  row to insert
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool insert( const row &value );

	/// Insert rows to database.
	/**
	 * @param[in] values  rows to insert
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool insert( const rows &values );

	/// Perform a select query.
	/**
	 * @param[in] s  select statement
	 * @param[out] result  query result
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool select( const std::string &s, row &result ) const;

	/// Perform a select query.
	/**
	 * @param[in] s  select statement
	 * @param[out] results  query results
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool select( const std::string &s, rows &results ) const;

	/// Update row.
	/**
	 * @param[in] value  row to update
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool update( const row &value );

	/// Update rows to database.
	/**
	 * @param[in] values  rows to update
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool update( const rows &values );

	// ========================================================================
	// Static Methods
	// ========================================================================

	/// Retrieve the export table sql statement.
	/**
	 * @return  export table sql statement
	 */
	static std::string exportTableStmt();

	/// Retrieve the create table sql statement.
	/**
	 * @return  create table sql statement
	 */
	static std::string createTableStmt();

	/// Retrieve the insert sql statement.
	/**
	 * @return  insert sql statement
	 */
	static std::string insertStmt();

	/// Retrieve the populate table sql statement.
	/**
	 * @return  populate table sql statement
	 */
	static std::string populateTableStmt();

	/// Retrieve the update sql statement.
	/**
	 * @return  update sql statement
	 */
	static std::string updateStmt();

private:

	// not implemented
	TableBase();

	// not implemented
	TableBase( const _Myt& );

	// not implemented
	_Myt &operator = ( const _Myt& );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace

#endif // DBOTABLEBASE_H
