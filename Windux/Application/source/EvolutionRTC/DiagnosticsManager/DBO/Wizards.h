/**
 * @file Wizards.h
 * @brief Wizards table support.
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef DBOWIZARDS_H
#define DBOWIZARDS_H

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

/// Wizards row.
struct WizardsRow
{
	/// Event severities.
	enum severityType
	{
		Unknown = 0,
		Info,										///< Informative messages.
		Warning,									///< Problems that are just messages, possibly diagnosed.
		Error,										///< Error in the wrapper that results in degraded state.
		Critical									///< Error in the wrapper that causes machine to not function.
	};

	int device;										///< Device type.
	int deviceExt;									///< Extended device type (i.e. printer number, node number).
	int errorCode;									///< Error code.
	severityType severity;							///< Severity.
	int serviceCategory;							///< Service category.
	int wizard;										///< Wizard force count.

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

	/// Retrieve if wizard available.
	/**
	 * @return  @c true if wizard available, @c false otherwise
	 */
	bool isWizard() const {return (0 < wizard);}

	/// Retrieve row in dictionary format.
	/**
	 * @param[in,out] dict  row
	 */
	void retrieve( util::Dictionary &dict ) const;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Wizards.
class Wizards : public TableBase<WizardsRow>
{
	typedef Wizards _Myt;
	typedef TableBase<WizardsRow> _Mybase;

public:

	// ========================================================================
	// CTOR / DTOR
	// ========================================================================

	/// Constructor.
	/**
	 * @param[in] db  pointer to database
	 */
	Wizards( sqlite3 *db );

	/// Destructor.
	virtual ~Wizards();

	// ========================================================================
	// Methods
	// ========================================================================

	/// Lookup max count.
	/**
	 * @param[in] device  device
	 * @param[in] deviceExt  device extended
	 * @param[in] errorCode  error code
	 * @return  max count
	 */
	virtual int lookupMaxCount( int device, int deviceExt, int errorCode );

	/// Lookup service category.
	/**
	 * @param[in] device  device
	 * @param[in] deviceExt  device extended
	 * @param[in] errorCode  error code
	 * @return  service category
	 */
	virtual int lookupServiceCategory( int device, int deviceExt, int errorCode );

	/// Lookup severity.
	/**
	 * @param[in] device  device
	 * @param[in] deviceExt  device extended
	 * @param[in] errorCode  error code
	 * @return  severity
	 */
	virtual row::severityType lookupSeverity( int device, int deviceExt, int errorCode );

	/// Retrieve record.
	/**
	 * @param[in] device  device
	 * @param[in] deviceExt  device extended
	 * @param[in] errorCode  error code
	 * @param[out] result  query result
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool retrieve( int device, int deviceExt, int errorCode, row &result );

private:

#ifdef USE_ACCOUNTANT
	static int dataType_;
	static std::string dataTypeName_;
#endif

	// not implemented
	Wizards();

	// not implemented
	Wizards( const _Myt& );

	// not implemented
	_Myt &operator = ( const _Myt& );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace

#endif // DBOWIZARDS_H
