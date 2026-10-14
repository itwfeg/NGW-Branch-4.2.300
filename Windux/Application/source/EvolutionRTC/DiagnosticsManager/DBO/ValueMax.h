/**
 * @file ValueMax.h
 * @brief ValueMax table support.
 *
 * @copyright Copyright (c) 2019, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef DBOVALUEMAX_H
#define DBOVALUEMAX_H

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

/// Value Max item row.
struct ValueMaxRow
{
	/// Timestamp type.
	typedef util::Timestamp timestamp_type;

	timestamp_type timestamp;						///< Timestamp (ms since epoch).
	int transNum;									///< Transaction Number.
	int productNum;									///< Product Number.
	double unitPrice;								///< Item Unit Price.
	int quantity;									///< Quantity Sold.
	double netWeight;								///< Net Weight of product sold.
	double tare;									///< Tare of product sold.
	double totalPrice;								///< Item total price.
	int counts;										///< Zeroed Weigher counts.
	int scaleFactor;								///< Scale factor.
	double tiltX;									///< X Axis tilt when product sold.
	double tiltY;									///< Y Axis tilt when product sold.
	double adjNetWeight;							///< Adjusted net weight for scale out of level.
	double adjTotalPrice;							///< Adjusted total price for scale out of level.
	double savings;									///< Savings gained from Value Max enabled.

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

/// Value Max.
class ValueMax : public TableBase<ValueMaxRow>
{
	typedef ValueMax _Myt;
	typedef TableBase<ValueMaxRow> _Mybase;

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
	ValueMax( sqlite3 *db );

	/// Destructor.
	virtual ~ValueMax();

	// ========================================================================
	// Methods
	// ========================================================================

	/// Create record.
	/**
	 * @param[in] timestamp  timestamp created
	 * @param[in] transNum  transaction number
	 * @param[in] productNum  product number
	 * @param[in] unitPrice  item unit price
	 * @param[in] quantity  quantity sold
	 * @param[in] netWeight  net weight of product sold
	 * @param[in] tare  tare of product sold
	 * @param[in] totalPrice  item total price
	 * @param[in] counts  zeroed weigher counts
	 * @param[in] scaleFactor  scale factor
	 * @param[in] tiltX  x axis tilt when product sold
	 * @param[in] tiltY  x axis tilt when product sold
	 * @param[in] adjNetWeight  adjusted net weight for scale out of level
	 * @param[in] adjTotalPrice  adjusted total price for scale out of level
	 * @param[in] savings  savings gained from value max enabled
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool create( const timestamp_type &timestamp, int transNum, int productNum, double unitPrice, int quantity, double netWeight, double tare, double totalPrice, int counts, int scaleFactor, double tiltX, double tiltY, double adjNetWeight, double adjTotalPrice, double savings );

	/// Retrieve totals savings from Value Max.
	/**
	 * @param[out] savings  savings gained from value max enabled
	 * @return  @c true upon success, @c false otherwise
	 */
	virtual bool totalSavings( double& savings ) const;

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
	ValueMax();

	// not implemented
	ValueMax( const _Myt& );

	// not implemented
	_Myt &operator = ( const _Myt& );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace

#endif // DBOVALUEMAX_H
