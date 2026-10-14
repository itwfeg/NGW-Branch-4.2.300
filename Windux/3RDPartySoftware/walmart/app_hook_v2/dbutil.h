/**
 * @file dbutil.h
 * Scale database utility methods
 *
 * @copyright Copyright (c) 2021 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef DBUTIL_H
#define DBUTIL_H

class QJsonValue;
class QSqlDatabase;
class QSqlRecord;
class QSqlTableModel;
class QString;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Database utility methods.
class dbutil
{
    typedef dbutil _Myt;

public:

    // ========================================================================
    // Static Methods
    // ========================================================================

    /// Convert Json value to string.
    /**
     * @param[in] value  value to convert
     * @param[out] wasNativeString  @c true if value was string, @c false otherwise
     * @param[in] mult  convert double to integer, using multiplier
     * @return  string value
     */
    static QString convertToString( const QJsonValue& value, bool& wasNativeString, int mult = 0 );

    /// Delete all records from table.
    /**
     * @param[in,out] db  database
     * @param[in] tableName  table name
     */
    static void deleteFromTable( QSqlDatabase& db, const QString& tableName );

    /// Delete records from table.
    /**
     * @param[in,out] model  table model
     * @param[in] filter  table select statement
     */
    static void deleteRecords( QSqlTableModel& model, const QString& filter );

    /// Save record to database.
    /**
     * @param[in,out] model table
     * @param[in] recordName  record name
     * @param[in] rec  record
     * @param[in] recExists  @c true if record existed, @c false otherwise
     * @return  @c true upon success, @c false otherwise
     */
    static bool saveRecordToDatabase( QSqlTableModel& model, const QString& recordName, QSqlRecord& rec, bool recExists = false );

    /// Save table.
    /**
     * @param[in,out] model table
     * @param[in] recordName  record name
     * @return  @c true upon success, @c false otherwise
     */
    static bool saveToDatabase( QSqlTableModel& model, const QString& recordName );

    /// Select record from table.
    /**
     * @param[in,out] model  table model
     * @param[in] filter  table select statement
     * @param[out] rec  record
     * @return  @c true if record found, @c false otherwise
     */
    static bool selectRecord( QSqlTableModel& model, const QString& filter, QSqlRecord& rec );

    /**
     * @param[in,out] rec  record to update
     * @param[in] fieldName  field name
     * @param[in] newValue  new value
     * @param[in] forceSet  @c true to force update, @c false to check existing value
     * @param[in] useLatin1  @c true to use latin1 encoding, @c false to not
     * @return  @c true upon success, @c false otherwise
     */
    static bool updateField( QSqlRecord& rec, const QString& fieldName, const QString& newValue, bool forceSet = false, bool useLatin1 = false );

private:

    // not implemented
    dbutil() = delete;

    // not implemented
    dbutil( const _Myt& ) = delete;

    // not implemented
    _Myt & operator = ( const _Myt& ) = delete;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // DBUTIL_H
