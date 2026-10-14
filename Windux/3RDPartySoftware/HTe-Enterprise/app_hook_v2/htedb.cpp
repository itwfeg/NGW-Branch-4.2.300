/**
 * @file htedb.cpp
 *
 * @copyright Copyright (c) 2022 Hobart Corporation
 * @author Larry Pevoar
 */

#include "common.h"
#include "dbutil.h"
#include "htedb.h"
#include "stringsjson.h"

#ifndef DONTUSESCALECONFIG
#include "scaleconfig.h"
#endif

#include <QDateTime>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QSettings>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlField>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlTableModel>
#include <QDir>
#include <QList>


const QString hteDatabase::HTEDBNAMEBASE = "hteapphook_";

const QString hteDatabase::FILENAME = "FileName";
const QString hteDatabase::FILEID = "FileId";
const QString hteDatabase::FILESIZE = "FileSize";
const QString hteDatabase::FILEVERSION = "FileVersion";
const QString hteDatabase::CHECKSUM = "Checksum";
const QString hteDatabase::LISTEDINCURRENTHEARTBEAT = "ListedInCurrentHeartbeat";
const QString hteDatabase::MOSTRECENTDOWNLOADDATETIME = "MostRecentDownloadDateTime";
const QString hteDatabase::STATUS = "Status";
const QString hteDatabase::DOWNLOADS_TABLE = "Downloads";

const QString hteDatabase::CONFIGNAME = "CoName";
const QString hteDatabase::CONFIGVALUE = "CoValue";
const QString hteDatabase::CONFIGS_TABLE = "Configs";

const QString hteDatabase::CFG_UUID = "uuid";
const QString hteDatabase::CFG_HEARTBEATURL = "heartbeatUrl";
const QString hteDatabase::CFG_HEARTBEATBACKUPURL = "heartbeatBackupUrl";
const QString hteDatabase::CFG_HEARTBEATENDPOINT = "heartbeatEndpoint";

// The next one should go away when we get the correct download url from the HTe server
const QString hteDatabase::CFG_HEARTBEATDOWNLOADURL = "heartbeatDownloadUrl";

// Use the following default config values if the config name was not found in the database's Configs table, nor in the "deprecated" hte.config file.
const QMap<QString, QString> hteDatabase::defaultConfigsMap = { {CFG_UUID, ""},
                                                                {CFG_HEARTBEATURL, "http://10.3.128.103:8080"},
                                                                {CFG_HEARTBEATBACKUPURL, ""},
                                                                {CFG_HEARTBEATENDPOINT, "/hte-enterprise/heartbeat/"},
                                                                {CFG_HEARTBEATDOWNLOADURL, "http://10.3.128.103:8080/hte-enterprise/filemgr/download"}
                                                              };

///////////////////////////////////////////////////////////////////////////////////////////////////
hteDatabase::hteDatabase( QObject *parent ) :
    _Mybase( parent ),
    hteDatabase_( nullptr ),
    htedbname_( "" )
{
    LOG_DEBUG << "hteDatabase - ctor";
    openDatabase();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
hteDatabase::~hteDatabase()
{
    LOG_DEBUG << "hteDatabase - dtor";
    closeDatabase();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool hteDatabase::isValid() const
{
    if ( hteDatabase_ )
        return true;

    return false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
const QJsonArray hteDatabase::getDownloadFileList()
{
    QMutexLocker guard( &m_ );

    QJsonArray fileList;

    QSqlDatabase conn( openDatabaseConnection() );

    if ( !conn.isValid() )
        LOG_DEBUG << "hte db not valid";
    else
    {
        QSqlTableModel tableModel( nullptr, conn );
        tableModel.setEditStrategy( QSqlTableModel::OnManualSubmit );
        tableModel.setTable( DOWNLOADS_TABLE );

        LOG_TRACE << "retrieve JSON array of downloaded files ...";

        // select
        if ( !tableModel.select() )
            LOG_DEBUG << "no downloaded files";
        else
        {
            const int rows( tableModel.rowCount() );

            LOG_DEBUG << "iterate " << rows << " files";

            QSqlField fileName;
            QSqlField fileId;
            QSqlField fileSize;
            QSqlField fileVersion;
            QSqlField checkSum;

            for ( int row( 0 ); row < rows; ++row )
            {
                QSqlRecord record( tableModel.record( row ) );

                if ( record.isEmpty() )
                {
                    LOG_WARN << "empty record found";
                    continue;
                }

                // parse record
                fileName = record.field( FILENAME );
                fileId = record.field( FILEID );
                fileSize = record.field( FILESIZE );
                fileVersion = record.field( FILEVERSION );
                checkSum = record.field( CHECKSUM );

                if (( !fileName.isValid() ) || ( !fileId.isValid() ) || ( !fileSize.isValid() ) || ( !fileVersion.isValid() ) || ( !checkSum.isValid() ))
                {
                    LOG_WARN << "invalid label data";
                    continue;
                }

                const QString fileNameValue( fileName.value().toString() );
                const QString checkSumValue( checkSum.value().toString() );

                QJsonObject oneFile;
                oneFile["fileName"] = fileName.value().toString();
                oneFile["fileId"] = fileId.value().toString();
                oneFile["fileSize"] = fileSize.value().toLongLong();
                oneFile["fileVersion"] = fileVersion.value().toString();
                oneFile["fileChecksum"] = checkSum.value().toString();

                /*
                {
                    "fileChecksum": "11ddd7be05950e8eada1b34bbc58e331",
                    "fileId": "",
                    "fileName": "plu_data.ht",
                    "fileSize": 702131,
                    "fileVersion": "1"
                },
                */

                LOG_DEBUG << "getDownloadFileList() - Download file: " << qPrintable( QJsonDocument(oneFile).toJson() );

                fileList.append( oneFile );
            }
        }
    }
    return fileList;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hteDatabase::markAllDownloadsReceivedFlag( bool received )
{
    QMutexLocker guard( &m_ );

    if ( !isValid() )
    {
        LOG_WARN << "db not valid for markAllDownloadsReceivedFlag()";
        return;
    }

    QSqlDatabase conn( openDatabaseConnection() );

    QSqlQuery query(conn);
    query.prepare("update " + DOWNLOADS_TABLE + " set " + LISTEDINCURRENTHEARTBEAT + "=" + ((received) ? "1" : "0") );
    if ( !query.exec() )
    {
        const QSqlError e( query.lastError() );
        LOG_ERROR << "failed to markAllDownloadsReceivedFlag() - " << e.type() << " " << qPrintable( e.text() );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void hteDatabase::deleteAllDownloadsNotReceived()
{
    QMutexLocker guard( &m_ );

    if ( !isValid() )
    {
        LOG_WARN << "db not valid for deleteAllDownloadsNotReceived()";
        return;
    }

    QSqlDatabase conn( openDatabaseConnection() );

    QSqlQuery query(conn);
    query.prepare("delete from " + DOWNLOADS_TABLE + " where " + LISTEDINCURRENTHEARTBEAT + "=0");
    if ( !query.exec() )
    {
        const QSqlError e( query.lastError() );
        LOG_ERROR << "failed to deleteAllDownloadsNotReceived() - " << e.type() << " " << qPrintable( e.text() );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool hteDatabase::shouldDownloadFile( const QJsonObject &fileObj )
{
    QMutexLocker guard( &m_ );

    bool result = true;

    /* example:
        {
            "fileChecksum": "bce412ba5932a60ed41a94dd66a07cb7d9e9d6697b06949e148423c82b17ee2c",
            "fileId": "443f5791-910e-4331-aa28-946d7a1b1bdb",
            "fileName": "store_data.ht",
            "fileSize": 16,
            "fileVersion": "1"
        }
    */

    if ( !fileObj.contains( JSON_FILENAME ) || !isValid() )
        return result;

    QSqlDatabase conn( openDatabaseConnection() );

    QSqlTableModel tableModel( nullptr, conn );
    tableModel.setTable( DOWNLOADS_TABLE );
    tableModel.setEditStrategy( QSqlTableModel::OnManualSubmit );
    const QString filter( FILENAME + "='" + fileObj[JSON_FILENAME].toString() + "'" );

    QSqlRecord rec;

    if ( dbutil::selectRecord( tableModel, filter, rec ) )
    {
        if ( fileObj.contains( JSON_FILE_CHECKSUM ) && fileObj[JSON_FILE_CHECKSUM].toString() == rec.field( CHECKSUM ).value().toString() )
        {
            result = false;

            // Also set the ListedInCurrentHeartbeat = 1, to indicate we received this filename in the heartbeat's response.
            // Only do this for when we aren't going to download the file (this conditional block of code).
            // If we need to download it, we will set this flag after we receive the file from the server.
            rec.setValue( LISTEDINCURRENTHEARTBEAT, QVariant( 1 ) );
            if ( !dbutil::saveRecordToDatabase( tableModel, DOWNLOADS_TABLE, rec, true ) )
                LOG_WARN << "Error saving ListedInCurrentHeartbeat field to Downloads record for fileName " << qPrintable( fileObj[JSON_FILENAME].toString() );

        }
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool hteDatabase::insertUpdateDownloadFile( const QJsonObject& fileObj, bool fileWasDownloaded )
{
    QMutexLocker guard( &mIU_ );

    if ( !fileObj.contains( JSON_FILENAME ) || !isValid() )
        return false;

    /* example:
        {
            "fileChecksum": "bce412ba5932a60ed41a94dd66a07cb7d9e9d6697b06949e148423c82b17ee2c",
            "fileId": "443f5791-910e-4331-aa28-946d7a1b1bdb",
            "fileName": "store_data.ht",
            "fileSize": 16,
            "fileVersion": "1"
        }
    */

    QSqlDatabase conn( openDatabaseConnection() );

    QSqlTableModel tableModel( nullptr, conn );
    tableModel.setTable( DOWNLOADS_TABLE );
    tableModel.setEditStrategy( QSqlTableModel::OnManualSubmit );
    const QString filter( FILENAME + "='" + fileObj[JSON_FILENAME].toString() + "'" );

    QSqlRecord rec;
    bool recExists(true);

    if ( !dbutil::selectRecord( tableModel, filter, rec ) )
    {
        rec = tableModel.record();
        rec.setValue( FILENAME, QVariant( fileObj[JSON_FILENAME].toString() ) );
        recExists = false;
    }

    if ( fileObj.contains( JSON_FILE_ID ) )
        rec.setValue( FILEID, QVariant( fileObj[JSON_FILE_ID].toString() ) );
    else
        rec.setNull( FILEID );

    if ( fileObj.contains( JSON_FILE_SIZE ) )
        rec.setValue( FILESIZE, QVariant( fileObj[JSON_FILE_SIZE].toVariant() ) );
    else
        rec.setNull( FILESIZE );

    if ( fileObj.contains( JSON_FILE_CHECKSUM ) )
        rec.setValue( CHECKSUM, QVariant( fileObj[JSON_FILE_CHECKSUM].toString() ) );
    else
        rec.setNull( CHECKSUM );

    if ( fileObj.contains( JSON_FILE_VERSION ) )
        rec.setValue( FILEVERSION, QVariant( fileObj[JSON_FILE_VERSION].toString() ) );
    else
        rec.setNull( FILEVERSION );

    if ( fileWasDownloaded )
    {
        rec.setValue( LISTEDINCURRENTHEARTBEAT, QVariant( 1 ) );

        const QDateTime now( QDateTime::currentDateTime() );
        QDateTime dt( now.toTimeSpec(Qt::UTC) );
        rec.setValue( MOSTRECENTDOWNLOADDATETIME, QVariant( dt.toString( Qt::ISODate ) ) );
    }

    // save!
    if ( dbutil::saveRecordToDatabase( tableModel, DOWNLOADS_TABLE, rec, recExists ) )
    {
        LOG_DEBUG << "saved Downloads record for fileName " << qPrintable( fileObj[JSON_FILENAME].toString() );
        return true;
    }
    return false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
QString hteDatabase::readConfig( const QString& name ) const
{
    QString value("");

    LOG_DEBUG << "hteDatabase::readConfig - trying to find: " << qPrintable( name );

    QMutexLocker guard( &m_ );

    if ( !isValid() )
        LOG_WARN << "hteDatabase::readConfig - not valid";
    else
    {
        QSqlDatabase conn( openDatabaseConnection() );

        QSqlTableModel model( nullptr, conn );
        model.setTable( CONFIGS_TABLE );
        model.setEditStrategy( QSqlTableModel::OnManualSubmit );
        const QString filter( CONFIGNAME + "='" + name + "'" );

        QSqlRecord rec;

       if ( dbutil::selectRecord( model, filter, rec ) )
        {
            value = ( rec.isEmpty() ) ? "" : rec.value( 1 ).toString();
            LOG_DEBUG << "hteDatabase::readConfig - read from hte db: " << qPrintable( filter ) << "  value = " << qPrintable( value );
        }
        else
        {
            bool valid(false);
#ifndef DONTUSESCALECONFIG
            value = scaleConfig::instance()->value<QString>( name, &valid );
#endif
            if (valid)
                LOG_DEBUG << "hteDatabase::readConfig - read from hte.config: " << qPrintable( name ) << "  value = " << qPrintable( value );
            else
            {
                if ( defaultConfigsMap.contains(name) )
                {
                    value = defaultConfigsMap.value(name);
                    LOG_DEBUG << "hteDatabase::readConfig - read from internal defaults: " << qPrintable( name ) << "  value = " << qPrintable( value );
                }
                else
                {
                    LOG_DEBUG << "hteDatabase::readConfig - Could not get Config named: " << qPrintable( name );
                    LOG_WARN << "hteDatabase::readConfig - Could not get Config named: " << qPrintable( name );
                }
            }
        }
    }

    return value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool hteDatabase::saveConfig( const QString& name, const QString& value ) const
{
    QMutexLocker guard( &m_ );

    if ( !isValid() )
        return false;

    QSqlDatabase conn( openDatabaseConnection() );

    QSqlTableModel model( nullptr, conn );
    model.setTable( CONFIGS_TABLE );
    model.setEditStrategy( QSqlTableModel::OnManualSubmit );
    const QString filter( CONFIGNAME + "='" + name + "'" );

    QSqlRecord rec;
    bool recExists(true);

    if ( !dbutil::selectRecord( model, filter, rec ) )
    {
        rec = model.record();
        rec.setValue( CONFIGNAME, name );
        recExists = false;
    }

    rec.setValue( CONFIGVALUE, QVariant( value ) );

    // save!
    if ( dbutil::saveRecordToDatabase( model, CONFIGS_TABLE, rec, recExists ) )
    {
        LOG_DEBUG << "saved HTe Configs record for name " << qPrintable( name ) << " to " << qPrintable( value );
        return true;
    }
    return false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
void hteDatabase::closeDatabase()
{
    if ( hteDatabase_ )
    {
        LOG_INFO << "closing database " << qPrintable( htedbname_ );
        hteDatabase_->close();

        delete hteDatabase_;
        hteDatabase_ = nullptr;
    }

    LOG_TRACE << "exit";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool hteDatabase::openDatabase()
{
    LOG_DEBUG << "hteDatabase::openDatabase()  BEGIN --------------------------------------- ";

    const QString SQLITE3_DRIVER( "QSQLITE" );

    if ( hteDatabase_ )
    {
        LOG_DEBUG << qPrintable( htedbname_ ) << " already open";
        return true;
    }

    QDir dbDir( HOBART_LOCAL_DIR + HTEDBDIR );
    QStringList filters;
    filters << "*.db";
    dbDir.setNameFilters(filters);
    dbDir.setSorting(QDir::Name | QDir::Reversed);
    QFileInfoList fileList = dbDir.entryInfoList();

    bool validDbName( false );
    if ( !fileList.empty() )
    {
        LOG_DEBUG << "hteDatabase::openDatabase() db list length " << fileList.length();
        htedbname_ = fileList.at(0).absoluteFilePath();
        if ( htedbname_.contains(HTEDBNAMEBASE) )
        {
            LOG_DEBUG << "hteDatabase::openDatabase() db found: " << qPrintable( htedbname_ );
            validDbName = true;
        }
    }
    if ( !validDbName )
    {
        htedbname_ = "";
        LOG_ERROR << "hteDatabase::openDatabase()  HTe db not found: " << qPrintable( HOBART_LOCAL_DIR + HTEDBDIR + HTEDBNAMEBASE + "*.db" );
        return false;
    }

    // If a previous db schema version exists, convert it to the newest schema, copying the data from the previous to the newest.
    if ( fileList.length() > 1 )
    {
        LOG_DEBUG << "hteDatabase::openDatabase() more than one db found ";

        QList<QFileInfo>::iterator iter = fileList.begin();
        // Using ++iter to get past the first one, which is the most current schema
        while (++iter != fileList.end())
        {
            QString previousHTeDbSchema = iter->absoluteFilePath();
            LOG_DEBUG << "hteDatabase::openDatabase() Need to convert HTe db from " << qPrintable( previousHTeDbSchema ) << " to " << qPrintable( htedbname_ );

            LOG_INFO << "Need to convert HTe db from " << qPrintable( previousHTeDbSchema ) << " to " << qPrintable( htedbname_ );
            if ( !convertDbSchemaVersion( previousHTeDbSchema, htedbname_ ) )
            {
                LOG_ERROR << "hteDatabase::openDatabase() Failed to convert HTe db from " << qPrintable( previousHTeDbSchema ) << " to " << qPrintable( htedbname_ );
                return false;
            }
        }
    }

    LOG_DEBUG << "hteDatabase::openDatabase()  " << qPrintable( htedbname_ );
    // qDebug() <<  QSqlDatabase::drivers();

    if ( !QSqlDatabase::isDriverAvailable( SQLITE3_DRIVER ) )
        LOG_ERROR << "database driver " << qPrintable( SQLITE3_DRIVER ) << " is not available!";
    else
    {
        // Need to provide a connection name for the addDatabase call below, or the scale db will
        // conflict with the HTe database, since both are open continuously
        const QString connName( "htedb_" + QString::number( (quintptr) QThread::currentThreadId() ) );
        LOG_DEBUG << "hteDatabase::openDatabase() " << qPrintable( connName );

        hteDatabase_ = new QSqlDatabase( QSqlDatabase::addDatabase( SQLITE3_DRIVER, connName ) );

        LOG_INFO << "hteDatabase::openDatabase() opening database " << qPrintable( htedbname_ ) << " ...";
        hteDatabase_->setDatabaseName( htedbname_ );

        if ( hteDatabase_->open() )
        {
            LOG_INFO << "open HTe database success";
            return true;
        }
        else
        {
            const QSqlError e( hteDatabase_->lastError() );
            LOG_ERROR << "failed to open database " << qPrintable( htedbname_ ) << " - " << e.type() << " " << qPrintable( e.text() );

            delete hteDatabase_;
            hteDatabase_ = nullptr;
        }
    }

    LOG_TRACE << "done";
    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QSqlDatabase hteDatabase::openDatabaseConnection() const
{
    const QString connName( "htedb_" + QString::number( (quintptr) QThread::currentThreadId() ) );
    LOG_DEBUG << "hteDatabase::openDatabaseConnection() " << qPrintable( connName );

    QSqlDatabase db( QSqlDatabase::database( connName ) );

    if (( !db.isOpen() ) || ( !db.isValid() ))
    {
       db = QSqlDatabase::cloneDatabase( *hteDatabase_, connName );

       if ( !db.open() )
       {
           const QSqlError e( db.lastError() );
           LOG_ERROR << "failed to open HTe database connection " << qPrintable( connName ) << " - " << e.type() << " " << qPrintable( e.text() );
       }
    }

    return db;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
bool hteDatabase::convertDbSchemaVersion( const QString previousHTeDbSchema, const QString hteDbName )
{
    bool result = true;

    LOG_DEBUG << "hteDatabase::convertDbSchemaVersion() - previous: " << qPrintable( previousHTeDbSchema ) << " current: " << qPrintable( hteDbName );

    // Open both databases.

    const QString SQLITE3_DRIVER( "QSQLITE" );
    QSqlDatabase *prevDatabase, *newDatabase;
    if ( !QSqlDatabase::isDriverAvailable( SQLITE3_DRIVER ) )
    {
        LOG_ERROR << "convertDbSchemaVersion - database driver " << qPrintable( SQLITE3_DRIVER ) << " is not available!";
        return false;
    }

    prevDatabase = new QSqlDatabase( QSqlDatabase::addDatabase( SQLITE3_DRIVER, previousHTeDbSchema ) );
    prevDatabase->setDatabaseName( previousHTeDbSchema );
    if ( prevDatabase->open() )
        LOG_INFO << "convertDbSchemaVersion - opened previous database " << qPrintable( previousHTeDbSchema ) << " ...";
    else
    {
        const QSqlError e( prevDatabase->lastError() );
        LOG_ERROR << "convertDbSchemaVersion - failed to open previous database " << qPrintable( previousHTeDbSchema ) << " - " << e.type() << " " << qPrintable( e.text() );

        delete prevDatabase;
        prevDatabase = nullptr;

        return false;
    }

    newDatabase = new QSqlDatabase( QSqlDatabase::addDatabase( SQLITE3_DRIVER, hteDbName ) );
    newDatabase->setDatabaseName( hteDbName );
    if ( newDatabase->open() )
        LOG_INFO << "convertDbSchemaVersion - opened new database " << qPrintable( hteDbName ) << " ...";
    else
    {
        const QSqlError e( newDatabase->lastError() );
        LOG_ERROR << "convertDbSchemaVersion - failed to open new database " << qPrintable( hteDbName ) << " - " << e.type() << " " << qPrintable( e.text() );

        delete newDatabase;
        newDatabase = nullptr;

        delete prevDatabase;
        prevDatabase = nullptr;

        return false;
    }


    // Determine which 'previousHTeDbSchema' version is present.
    // Read all data from the 'previousHTeDbSchema' and write it to the newest schema's database version.

    QSqlTableModel newModel( nullptr, *newDatabase );
    newModel.setTable( DOWNLOADS_TABLE );
    newModel.setEditStrategy( QSqlTableModel::OnManualSubmit );

    QSqlRecord rec;
    QSqlQuery query(*prevDatabase);
    query.prepare( "select * from " + DOWNLOADS_TABLE );
    query.exec();
    while ( query.next() )
    {
        rec = query.record();

        if ( !dbutil::saveRecordToDatabase( newModel, DOWNLOADS_TABLE, rec, false ) )
            LOG_WARN << "Could not save record to new db Downloads table";
    }


    // Close both databases.
    prevDatabase->close();
    delete prevDatabase;
    prevDatabase = nullptr;

    newDatabase->close();
    delete newDatabase;
    newDatabase = nullptr;


    // Delete the 'previousHTeDbSchema' database file(s).
    QDir dbDir( HOBART_LOCAL_DIR + HTEDBDIR );
    QStringList filters;
    QString filts (QFileInfo(previousHTeDbSchema).baseName() + ".*");
    filters << QFileInfo(previousHTeDbSchema).baseName() + ".*";
    dbDir.setNameFilters(filters);
    dbDir.setSorting(QDir::Name);
    LOG_DEBUG << "hteDatabase::convertDbSchemaVersion() - deleting previous: filts: " << qPrintable( filts );
    QFileInfoList fileList = dbDir.entryInfoList();
    LOG_DEBUG << "hteDatabase::convertDbSchemaVersion() - deleting previous: list length " << fileList.length();
    while ( !fileList.isEmpty() )
    {
        if ( !QFile::remove( fileList.at(0).absoluteFilePath() ) )
        {
            LOG_WARN << "could not remove cache file " << qPrintable( fileList.at(0).absoluteFilePath() );
            break;
        }
        fileList.removeFirst();
    }

    return result;
}
