/**
 * @file scaledb.cpp
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#include "common.h"
#include "dbutil.h"
#include "scaledb.h"
#include "stringsjson.h"

#include <QDateTime>
#include <QJsonArray>
#include <QJsonObject>
#include <QSettings>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlField>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlTableModel>

///////////////////////////////////////////////////////////////////////////////////////////////////
scaleDatabase::scaleDatabase( const QString& name, QObject *parent ) :
    _Mybase( parent ),
    dbname_( name ),
    scaleDatabase_( nullptr )
{
    LOG_DEBUG << "ctor";
    openDatabase();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
scaleDatabase::~scaleDatabase()
{
    LOG_DEBUG << "dtor";
    closeDatabase();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool scaleDatabase::isValid() const
{
    if ( scaleDatabase_ )
        return true;

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString scaleDatabase::queryConfigs( const QString& configName ) const
{
    QString configValue;

    if ( !isValid() )
        LOG_WARN << "not valid";
    else
    {
        QSqlDatabase conn( openDatabaseConnection() );

        QSqlTableModel dbConfigsRM( nullptr, conn );
        dbConfigsRM.setTable( "Configs" );
        dbConfigsRM.setEditStrategy( QSqlTableModel::OnManualSubmit );

        QSqlRecord dbConfigRec;

        const QString filter( "CoName='" + configName + "'" );

        if ( dbutil::selectRecord( dbConfigsRM, filter, dbConfigRec ) )
        {
            if ( !dbConfigRec.isEmpty() )
                configValue = dbConfigRec.value( 1 ).toString();

            LOG_TRACE << "Configs read " << qPrintable( filter ) << "  value = " << qPrintable( configValue );
        }
        else
        {
            LOG_WARN << "Could not get Configs from db " << qPrintable( filter );
        }
    }

    return configValue;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void scaleDatabase::closeDatabase()
{
    if ( scaleDatabase_ )
    {
        LOG_INFO << "closing database ...";
        scaleDatabase_->close();

        delete scaleDatabase_;
        scaleDatabase_ = nullptr;
    }

    LOG_TRACE << "exit";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool scaleDatabase::openDatabase()
{
    const QString SQLITE3_DRIVER( "QSQLITE" );

    if ( scaleDatabase_ )
    {
        LOG_DEBUG << "already open";
        return true;
    }

    LOG_DEBUG << "open db " << qPrintable( dbname_ );

    if ( dbname_.isEmpty() )
        LOG_WARN << "empty db name";
    else if ( !QSqlDatabase::isDriverAvailable( SQLITE3_DRIVER ) )
        LOG_ERROR << "database driver " << qPrintable( SQLITE3_DRIVER ) << " is not available!";
    else
    {
        scaleDatabase_ = new QSqlDatabase( QSqlDatabase::addDatabase( SQLITE3_DRIVER ) );

        LOG_INFO << "opening database " << qPrintable( dbname_ ) << " ...";
        scaleDatabase_->setDatabaseName( dbname_ );

        if ( scaleDatabase_->open() )
        {
            LOG_INFO << "open database success";
            LOG_INFO << "country code " << qPrintable( queryConfigs( "primaryCountryCode" ) );
            LOG_INFO << "store " << qPrintable( queryConfigs( "storeId" ) );

            return true;
        }
        else
        {
            const QSqlError e( scaleDatabase_->lastError() );

            LOG_ERROR << "failed to open database " << e.type() << " " << qPrintable( e.text() );

            delete scaleDatabase_;
            scaleDatabase_ = nullptr;
        }
    }

    LOG_TRACE << "done";
    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QSqlDatabase scaleDatabase::openDatabaseConnection() const
{
    const QString connName( QString::number( (quintptr) QThread::currentThreadId() ) );

    QSqlDatabase db( QSqlDatabase::database( connName ) );

    if (( !db.isOpen() ) || ( !db.isValid() ))
    {
       db = QSqlDatabase::cloneDatabase( *scaleDatabase_, connName );

       if ( !db.open() )
           LOG_ERROR << "failed to open db connection";
    }

    return db;
}
