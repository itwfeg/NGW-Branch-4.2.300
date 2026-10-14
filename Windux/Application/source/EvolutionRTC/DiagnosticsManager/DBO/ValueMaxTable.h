/**
 * @file ValueMaxTable.h
 * @brief ValueMax table and column definitions.
 *
 * @copyright Copyright (c) 2019, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef DBOVALUEMAXTABLE_H
#define DBOVALUEMAXTABLE_H

#define STRINGIFY_(x) #x
#define STRINGIFY(x) STRINGIFY_(x)

///////////////////////////////////////////////////////////////////////////////////////////////////

// tables
#define TABLE_VALUEMAX "valuemax"

// columns
#define COLUMN_ADJ_NET_WEIGHT "adjNetWeight"
#define COLUMN_ADJ_TOTAL_PRICE "adjTotalPrice"
#define COLUMN_COUNTS "counts"
#define COLUMN_NET_WEIGHT "netWeight"
#define COLUMN_PRODUCT_NUM "productNum"
#define COLUMN_QUANTITY "quantity"
#define COLUMN_SAVINGS "savings"
#define COLUMN_SCALE_FACTOR "scaleFactor"
#define COLUMN_TARE "tare"
#define COLUMN_TILT_X "tiltX"
#define COLUMN_TILT_Y "tiltY"
#define COLUMN_TIMESTAMP "timestamp"
#define COLUMN_TOTAL_PRICE "totalPrice"
#define COLUMN_TRANSACTION_NUM "transNum"
#define COLUMN_UNIT_PRICE "unitPrice"

//
// create table sql
//

#define CREATE_TABLE_VALUEMAX "create table " \
    TABLE_VALUEMAX " ( " \
        COLUMN_TIMESTAMP            " integer primary key not null, " \
        COLUMN_TRANSACTION_NUM      " integer, " \
		COLUMN_PRODUCT_NUM          " integer, " \
		COLUMN_UNIT_PRICE           " real, " \
		COLUMN_QUANTITY             " integer, " \
		COLUMN_NET_WEIGHT           " real, " \
		COLUMN_TARE                 " real, " \
		COLUMN_TOTAL_PRICE          " real, " \
		COLUMN_COUNTS               " integer, " \
		COLUMN_SCALE_FACTOR         " integer, " \
		COLUMN_TILT_X               " real, " \
		COLUMN_TILT_Y               " real, " \
		COLUMN_ADJ_NET_WEIGHT       " real, " \
		COLUMN_ADJ_TOTAL_PRICE      " real, " \
		COLUMN_SAVINGS              " real );"

//
// export table sql
//

#define EXPORT_TABLE_VALUEMAX "insert into newdb." \
	TABLE_VALUEMAX " (" \
        COLUMN_TIMESTAMP ", " \
        COLUMN_TRANSACTION_NUM ", " \
		COLUMN_PRODUCT_NUM ", " \
		COLUMN_UNIT_PRICE ", " \
		COLUMN_QUANTITY ", " \
		COLUMN_NET_WEIGHT ", " \
		COLUMN_TARE ", " \
		COLUMN_TOTAL_PRICE ", " \
		COLUMN_COUNTS ", " \
		COLUMN_SCALE_FACTOR ", " \
		COLUMN_TILT_X ", " \
		COLUMN_TILT_Y ", " \
		COLUMN_ADJ_NET_WEIGHT ", " \
		COLUMN_ADJ_TOTAL_PRICE ", " \
        COLUMN_SAVINGS ") SELECT " \
        COLUMN_TIMESTAMP ", " \
        COLUMN_TRANSACTION_NUM ", " \
		COLUMN_PRODUCT_NUM ", " \
		COLUMN_UNIT_PRICE ", " \
		COLUMN_QUANTITY ", " \
		COLUMN_NET_WEIGHT ", " \
		COLUMN_TARE ", " \
		COLUMN_TOTAL_PRICE ", " \
		COLUMN_COUNTS ", " \
		COLUMN_SCALE_FACTOR ", " \
		COLUMN_TILT_X ", " \
		COLUMN_TILT_Y ", " \
		COLUMN_ADJ_NET_WEIGHT ", " \
		COLUMN_ADJ_TOTAL_PRICE ", " \
        COLUMN_SAVINGS " FROM " TABLE_VALUEMAX ";"

//
// insert sql
//

#define INSERT_VALUEMAX "insert into " \
	TABLE_VALUEMAX " (" \
		COLUMN_TIMESTAMP ", " \
        COLUMN_TRANSACTION_NUM ", " \
		COLUMN_PRODUCT_NUM ", " \
		COLUMN_UNIT_PRICE ", " \
		COLUMN_QUANTITY ", " \
		COLUMN_NET_WEIGHT ", " \
		COLUMN_TARE ", " \
		COLUMN_TOTAL_PRICE ", " \
		COLUMN_COUNTS ", " \
		COLUMN_SCALE_FACTOR ", " \
		COLUMN_TILT_X ", " \
		COLUMN_TILT_Y ", " \
		COLUMN_ADJ_NET_WEIGHT ", " \
		COLUMN_ADJ_TOTAL_PRICE ", " \
		COLUMN_SAVINGS ") " \
		"values (?1, ?2, ?3, ?4, ?5, ?6, ?7, ?8, ?9, ?10, ?11, ?12, ?13, ?14, ?15);"

//
// update sql
//

#define UPDATE_VALUEMAX "update " \
	TABLE_VALUEMAX " set " \
        COLUMN_TRANSACTION_NUM "=?2, " \
		COLUMN_PRODUCT_NUM "=?3, " \
		COLUMN_UNIT_PRICE "=?4, " \
		COLUMN_QUANTITY "=?5, " \
		COLUMN_NET_WEIGHT "=?6, " \
		COLUMN_TARE "=?7, " \
		COLUMN_TOTAL_PRICE "=?8, " \
		COLUMN_COUNTS "=?9, " \
		COLUMN_SCALE_FACTOR "=?10, " \
		COLUMN_TILT_X "=?11, " \
		COLUMN_TILT_Y "=?12, " \
		COLUMN_ADJ_NET_WEIGHT "=?13, " \
		COLUMN_ADJ_TOTAL_PRICE "=?14, " \
		COLUMN_SAVINGS "=?15 " \
		"where " COLUMN_TIMESTAMP "=?1;"

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // DBOVALUEMAXTABLE_H
