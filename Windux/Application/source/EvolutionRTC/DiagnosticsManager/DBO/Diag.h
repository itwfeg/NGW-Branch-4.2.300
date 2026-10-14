/**
 * @file Diag.h
 * @brief Diag table support.
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef DBODIAG_H
#define DBODIAG_H

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

/// Diagnostics row.
struct DiagRow
{
	/// Timestamp type.
	typedef util::Timestamp timestamp_type;

	/// Method types.
	enum methodType
	{
		Ignored = 0,								///< Event that was ignored by user (i.e. they did not press the button)
		Manual,										///< Event that was accepted by user from error prompt.
		Automatic,									///< Event that triggered automatic diagnosis (i.e. 3 times rule).
		OperatorInitiated,							///< Diagnostics by operator from log entry.
		ServiceInitiated							///< Diagnostics by service tech from log entry.
	};

	timestamp_type timestamp;					///< Timestamp (ms since epoch).
	timestamp_type eventLog;					///< Event log identifier.
	methodType method;							///< State type.
	std::string hardwareModel;					///< Hardware model name.
	std::string hardwareSerial;					///< Hardware serial number.
	unsigned int hardwareCycles;				///< Hardware cycle count.

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

/// Diagnostics.
class Diag : public TableBase<DiagRow>
{
	typedef Diag _Myt;
	typedef TableBase<DiagRow> _Mybase;

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
	Diag( sqlite3 *db );

	/// Destructor.
	virtual ~Diag();

	// ========================================================================
	// Methods
	// ========================================================================

	/// Create diagnostics event with initial step.
	/**
	 * @param[in] eventLog  event log of initial event
	 * @param[in] method  diagnostic method
	 * @return  diag identifier upon success, zero (0) upon error
	 */
	virtual timestamp_type create( const timestamp_type &eventLog, row::methodType method = row::Ignored );

	/// Retrieve events.
	/**
	 * @param[in] eventLog  event log identifier
	 * @param[out] results  query results
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool queryByEventLog( const timestamp_type &eventLog, rows &results );

	/// Retrieve row.
	/**
	 * @param[in] timestamp  event to retrieve
	 * @param[out] result  event
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool retrieve( const timestamp_type &timestamp, row &result );

	/// Update diagnostic method.
	/**
	 * @param[in] timestamp  event to update
	 * @param[in] method  diagnostic method
	 * @param[in] errorCode  error code
	 * @param[out] diagStep  diagnostic step identifier
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool updateMethod( const timestamp_type &timestamp, row::methodType method, int errorCode, timestamp_type &diagStep );

	/// Update hardware information.
	/**
	 * @param[in] timestamp  event to update
	 * @param[in] hardwareModel  hardware model name
	 * @param[in] hardwareSerial  hardware serial number
	 * @param[in] hardwareCycles  hardware cycle count
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool updateHardwareInfo( const timestamp_type &timestamp, const std::string &hardwareModel, const std::string &hardwareSerial, unsigned int hardwareCycles );

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
	Diag();

	// not implemented
	Diag( const _Myt& );

	// not implemented
	_Myt &operator = ( const _Myt& );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace

#endif // DBODIAG_H
