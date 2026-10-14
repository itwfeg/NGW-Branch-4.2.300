/**
 * @file dbutil.cpp
 *
 * @copyright Copyright (c) 2021 Hobart Corporation
 * @author Randy Blankley
 */

#include "common.h"
#include "dbutil.h"

#include <QDateTime>
#include <QJsonValue>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlField>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlTableModel>
#include <QString>

///////////////////////////////////////////////////////////////////////////////////////////////////
QString dbutil::convertToString( const QJsonValue& value, bool& wasNativeString, int mult )
{
    QString result;

    wasNativeString = false;

    if ( value.isString() )
    {
        result = value.toString();
        wasNativeString = true;
    }
    else if ( value.isBool() )
    {
        result = value.toBool() ? "1" : "0";
    }
    else if ( value.isDouble() )
    {
        double decimalValue( value.toDouble() );
        int64_t integerValue( value.toInt() );

        if ( 0 < mult )
        {
            decimalValue *= mult;
            integerValue *= mult;
        }

        const double diff( decimalValue - integerValue );

        if ( 0.0 == diff )
            result = QString::number( integerValue );
        else if ( 0 < mult )
            result = QString::number( (int) decimalValue );
        else
            result = QString::number( decimalValue );
    }
    else if ( value.isNull() )
    {
        // nothing to do, return empty string
    }
    else
    {
        LOG_WARN << "unhandled value type!";
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dbutil::deleteFromTable( QSqlDatabase& conn, const QString& tableName )
{
    qint64 beginTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
    QString query("delete from ");
    QSqlQuery deleteSql(query + tableName, conn);
    qint64 endTime = QDateTime::currentDateTime().toMSecsSinceEpoch();

    QSqlError err( deleteSql.lastError() );
    LOG_DEBUG << "Result of deleting " << qPrintable( tableName )
                 << "   error type: " << err.type()
                 << "   native error code: " << qPrintable( err.nativeErrorCode() )
                 << "   error text: " << qPrintable( err.text() )
                 << "   database text: " << qPrintable( err.databaseText() )
                 << "   driver text: " << qPrintable( err.driverText() );

    LOG_DEBUG << "Elapsed time to delete " << qPrintable( tableName ) << ":  " << endTime - beginTime << " ms";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dbutil::deleteRecords( QSqlTableModel& model, const QString& filter )
{
    model.setFilter( filter );

    if ( !model.select() )
    {
        const QSqlError e( model.lastError() );

        LOG_WARN << "error during select for " << qPrintable( model.tableName() ) << " " << e.type() << " " << qPrintable( e.text() );
        return;
    }

    if ( model.rowCount() )
    {
        LOG_DEBUG << "about to remove " << model.rowCount() << " rows...";
        model.removeRows( 0, model.rowCount() );

        int attempts( 8 );

        do
        {
            if ( model.submitAll() )
            {
                LOG_DEBUG << "success";
                return;
            }

            const QSqlError err( model.lastError() );

            LOG_WARN << "Error removing records from database:\n" <<
                "   error type: " << err.type() << "\n" <<
                "   native error code: " << qPrintable( err.nativeErrorCode() ) << "\n" <<
                "   error text: " << qPrintable( err.text() ) << "\n" <<
                "   database text: " << qPrintable( err.databaseText() ) << "\n" <<
                "   driver text: " << qPrintable( err.driverText() );

        } while ( 0 < --attempts );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool dbutil::saveRecordToDatabase( QSqlTableModel& dbRM, const QString& recordName, QSqlRecord& dbRec, bool dbRecExists )
{
    // The record already existed when we started, so just update it.
    if ( dbRecExists )
    {
        if ( !dbRM.setRecord( 0, dbRec ) )
        {
            LOG_WARN << "Could not set the db record for " << qPrintable( recordName );
            return false;
        }
    }

    // The record DID NOT ALREADY exist when we started, so I need to create a new one.
    else
    {
        if ( !dbRM.insertRecord( 0, dbRec ) )
        {
            LOG_WARN << "Could not insert the db record for " << qPrintable( recordName );
            return false;
        }
     }

     if ( !saveToDatabase( dbRM, recordName ) )
         return false;

     return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool dbutil::saveToDatabase( QSqlTableModel& dbRM, const QString& recordName )
{
    bool result( false );

    int attempts( 8 );

    do
    {
        if ( dbRM.submitAll() )
        {
            LOG_DEBUG << "Submitted " << qPrintable( recordName ) << " to the database";
            result = true;
            break;
        }

        const QSqlError err( dbRM.lastError() );

        LOG_WARN << "Error submitting " << qPrintable( recordName ) << " to the database:\n" <<
            "   error type: " << err.type() << "\n" <<
            "   native error code: " << qPrintable( err.nativeErrorCode() ) << "\n" <<
            "   error text: " << qPrintable( err.text() ) << "\n" <<
            "   database text: " << qPrintable( err.databaseText() ) << "\n" <<
            "   driver text: " << qPrintable( err.driverText() );

    } while ( 0 < --attempts );

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool dbutil::selectRecord( QSqlTableModel& model, const QString& filter, QSqlRecord& rec )
{
    model.setFilter( filter );

    if ( !model.select() )
    {
        const QSqlError e( model.lastError() );

        LOG_WARN << "error during select for " << qPrintable( model.tableName() ) << " " << e.type() << " " << qPrintable( e.text() );
        return false;
    }
    else if ( 1 != model.rowCount() )
    {
        LOG_DEBUG << "no rows found for " << qPrintable( model.tableName() ) << " filter " << qPrintable( filter );
        return false;
    }

    rec = model.record( 0 );

    if ( rec.isEmpty() )
    {
        LOG_DEBUG << "found empty record for " << qPrintable( model.tableName() ) << " filter " << qPrintable( filter );
        return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool dbutil::updateField( QSqlRecord &dbRec, const QString& dbFieldName, const QString& newValue, bool forceSet, bool useLatin1 )
{
    if ( dbRec.indexOf( dbFieldName ) < 0 )
    {
        LOG_WARN << qPrintable( dbFieldName ) << " NOT FOUND in db record";
        return false;
    }

    const QSqlField field( dbRec.field( dbFieldName ) );
    QVariant fieldVar( field.value() );

    const QString prevValue( fieldVar.toString() );
    bool setIt( true );

    // check if different
    if ( prevValue.isNull() )
        setIt = !newValue.isNull();
    else
        setIt = (prevValue != newValue);

    // nothing to do
    if (( !forceSet ) && ( !setIt ))
        return false;

    // do latin1 conversion
    if ( useLatin1 )
        fieldVar = newValue.toLatin1();
    else
        fieldVar = newValue;

    // set!
    dbRec.setValue( dbFieldName, fieldVar );

    LOG_TRACE << "setting field " << qPrintable( dbFieldName ) <<
        "  prev value = " << (prevValue.isNull() ? "NULL" : qPrintable( prevValue )) <<
        "  new value = " << (newValue.isNull() ? "NULL" : qPrintable( newValue ));

    return true;
}
