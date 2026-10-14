/**
 * @file DiagStep.h
 * @brief DiagStep table support.
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef DBODIAGSTEP_H
#define DBODIAGSTEP_H

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

/// Diagnostics step row.
struct DiagStepRow
{
	/// Timestamp type.
	typedef util::Timestamp timestamp_type;

	/// State types.
	enum stateType
	{
		New = 0,									///< Newly created step.
		InProgress,									///< Diagnosis in progress.
		Aborted,									///< Test was aborted in the middle of screen (i.e. user powered off system).
		Cancelled,									///< Test cancelled by user.
		Pass,										///< Test passed.
		Fail,										///< Test failed.
		FailTimeout									///< Failed due to timeout.
	};

	timestamp_type timestamp;						///< Timestamp (ms since epoch).
	timestamp_type diag;							///< Diag table identifier.
	int errorCode;									///< Error code to diagnose.
	stateType state;								///< State type.
	timestamp_type diagTimestamp;					///< Timestamp when diagnosed (ms since epoch).
	timestamp_type diagCompleteTimestamp;			///< Timestamp when diagnosis completes (ms since epoch).
	int diagResult;									///< Result code from diagnosis.
	int routine;									///< Diagnostic routine executed.
	int routineExpectedResult;						///< Expected result from diagnostic routine.
	int routineActualResult;						///< Actual result from diagnostic routine.

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

/// Diagnostics step.
class DiagStep : public TableBase<DiagStepRow>
{
	typedef DiagStep _Myt;
	typedef TableBase<DiagStepRow> _Mybase;

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
	DiagStep( sqlite3 *db );

	/// Destructor.
	virtual ~DiagStep();

	// ========================================================================
	// Methods
	// ========================================================================

	/// Abort unfinished steps.
	/**
	 * This method updates state to aborted for all unfinished steps in the table.
	 *
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool abortUnfinishedSteps();

	/// Create diagnostics event.
	/**
	 * @param[in] diag  diag identifier
	 * @param[in] errorCode  error code
	 * @return  timestamp of created step, zero (0) upon error
	 */
	virtual timestamp_type create( const timestamp_type &diag, int errorCode );

	/// Retrieve events.
	/**
	 * @param[in] diag  diag table identifier
	 * @param[out] results  query results
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool queryByDiag( const timestamp_type &diag, rows &results );

	/// Retrieve row.
	/**
	 * @param[in] timestamp  event to retrieve
	 * @param[out] result  event
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool retrieve( const timestamp_type &timestamp, row &result );

	/// Update diagnostic routine info information.
	/**
	 * @param[in] timestamp  event to update
	 * @param[in] routine  diagnostic routine executed
	 * @param[in] routineExpectedResult  expected result from routine
	 * @param[in] routineActualResult  actual result from routine
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool updateRoutineInfo( const timestamp_type &timestamp, int routine, int routineExpectedResult, int routineActualResult );

	/// Update event state.
	/**
	 * @param[in] timestamp  item to update
	 * @param[in] newState  new state
	 * @param[in] diagResult  diag result
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool updateState( const timestamp_type &timestamp, row::stateType newState, int diagResult = 0 );

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
	DiagStep();

	// not implemented
	DiagStep( const _Myt& );

	// not implemented
	_Myt &operator = ( const _Myt& );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace

#endif // DBODIAGSTEP_H
