/**
 * @file Config.h
 * @brief Config table support.
 *
 * @copyright Copyright (c) 2016, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef DBOCONFIG_H
#define DBOCONFIG_H

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

/// Configuration row.
struct ConfigRow
{
	/// Timestamp type.
	typedef util::Timestamp timestamp_type;

	/// Value types.
	enum valueType
	{
		vtInteger = 0,								///< Integer value (32 bit).
		vtString,									///< String value.
		vtReal,										///< Decimal value.
		vtLongLong,									///< Integer value (64 bit).
	};

	std::string name;
	valueType type;

	int value_integer;
	std::string value_string;
	double value_real;
	long long value_longlong;

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

/// Configuration.
class Config : public TableBase<ConfigRow>
{
	typedef Config _Myt;
	typedef TableBase<ConfigRow> _Mybase;

public:

	/// Timestamp type.
	typedef typename ConfigRow::timestamp_type timestamp_type;

	// ========================================================================
	// CTOR / DTOR
	// ========================================================================

	/// Constructor.
	/**
	 * @param[in] db  pointer to database
	 */
	Config( sqlite3 *db );

	/// Destructor.
	virtual ~Config();

	// ========================================================================
	// Properties
	// ========================================================================

	/// Retrieve created.
	/**
	 * @return  created stamp (ms since epoch)
	 */
	virtual timestamp_type created() const;

	/// Retrieve edge device init time.
	/**
	 * @return  time (seconds)
	 */
	virtual unsigned int edgeInit() const;

	/// Retrieve edge device power up time.
	/**
	 * @return  time (seconds)
	 */
	virtual unsigned int edgePowerUp() const;

	/// Retrieve edge device status check timer.
	/**
	 * @return  time (seconds)
	 */
	virtual unsigned int edgeStatusCheckTimer() const;

	/// Retrieve housekeeping.
	/**
	 * @return  housekeeping (days)
	 */
	virtual unsigned int housekeeping() const;

	/// Check if database is latest version.
	/**
	 * @return  @c true if latest version, @c false otherwise
	 */
	virtual bool isLatestVersion() const;

	/// Set lower film width.
	/**
	 * @param[in] value  film width
	 */
	virtual void setWrapperLowerFilmWidth( unsigned int value );

	/// Set upper film width.
	/**
	 * @param[in] value  film width
	 */
	virtual void setWrapperUpperFilmWidth( unsigned int value );

	/// Retrieve version.
	/**
	 * @return  version
	 */
	virtual unsigned int version() const;

	/// Retrieve wrapper lower film width.
	/**
	 * @return  film width
	 */
	virtual unsigned int wrapperLowerFilmWidth() const;

	/// Retrieve wrapper upper film width.
	/**
	 * @return  upper width
	 */
	virtual unsigned int wrapperUpperFilmWidth() const;

	// ========================================================================
	// Methods
	// ========================================================================

	/// Add amount to value max savings total.
	/**
	 * @param[in] savings  savings to add
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool addValueMaxSavings( double savings );

protected:

	// ========================================================================
	// Methods
	// ========================================================================

	/// Retrieve row.
	/**
	 * @param[in] name  config name
	 * @param[out] result  event
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool retrieve( const std::string &name, row &result ) const;

private:

#ifdef USE_ACCOUNTANT
	static int dataType_;
	static std::string dataTypeName_;
#endif

	// not implemented
	Config();

	// not implemented
	Config( const _Myt& );

	// not implemented
	_Myt &operator = ( const _Myt& );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace

#endif // DBOCONFIG_H
