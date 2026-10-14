/**
 * @file hookwalmartrfid.cpp
 *
 * @copyright Copyright (c) 2022 Hobart Corporation
 * @author Randy Blankley
 */

#include "common.h"
#include "hookwalmartrfid.h"
#include "networkaccess.h"

#include <QDateTime>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSettings>

#include <cmath>

#if defined(Q_OS_LINUX)
static const QString INTERNAL_CONFIG( "/usr/share/walmart-apphook-rfid/hobart_internal.config" );
#elif defined(Q_OS_WIN32)
static const QString INTERNAL_CONFIG( "hobart_internal.config" );
#endif

//#define USE_DEBUG_VALUES

QMutex walmartHookRFID::instanceMutex_;
walmartHookRFID *walmartHookRFID::instance_( nullptr );

///////////////////////////////////////////////////////////////////////////////////////////////////
walmartHookRFID::walmartHookRFID( QObject *parent ) :
    _Mybase( parent ),
    printingIsRemotePrint_( false ),
    serverPort_( 0 ),
    prevTrans_( 0 )
{
    LOG_DEBUG << "ctor";

    // read config
    readConfiguration();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
walmartHookRFID::~walmartHookRFID()
{
    LOG_TRACE << "done";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
walmartHookRFID *walmartHookRFID::instance()
{
    if ( !instance_ )
    {
        QMutexLocker guard( &instanceMutex_ );

        if ( !instance_ )
            instance_ = new _Myt();
    }

    return instance_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void walmartHookRFID::destroy()
{
    if ( instance_ )
    {
        QMutexLocker guard( &instanceMutex_ );

        if ( instance_ )
        {
            delete instance_;
            instance_ = nullptr;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool walmartHookRFID::initialize()
{
    QMutexLocker guard( &m_ );

    LOG_INFO << "==================================================";
    LOG_INFO << "walmart RFID customer hook";
    LOG_INFO << "build date: " << __DATE__ << " " << __TIME__;
    LOG_INFO << "==================================================";

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void walmartHookRFID::finalize()
{
    LOG_TRACE << "done";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void walmartHookRFID::configSaved()
{
    QMutexLocker guard( &m_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void walmartHookRFID::beforeOpenDatabase( const QString& dbname )
{
    QMutexLocker guard( &m_ );
    dbname_ = dbname;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void walmartHookRFID::openDatabase()
{
    QMutexLocker guard( &m_ );
    LOG_DEBUG << "open database " << qPrintable( dbname_ );

    if ( dbname_.isEmpty() )
        LOG_WARN << "empty db name!";
    else
    {
/*
        db_ = new scaleDatabase( dbname_, this );

        if ( !db_->isValid() )
            LOG_ERROR << "error opening database " << qPrintable( dbname_ );
        else if ( !db_->parent() )
        {
            // when database opened outside of parent thread an error will be thrown about
            // parenting and threads... here we try to fix the issue by moving database object
            // back to proper thread
            db_->moveToThread( thread() );
        }
*/
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void walmartHookRFID::closeDatabase()
{
    QMutexLocker guard( &m_ );
/*
    if ( db_ )
    {
        LOG_DEBUG << "close database...";

        delete db_;
        db_ = nullptr;
    }
*/
    LOG_TRACE << "done";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool walmartHookRFID::beforePluRead( const QString& plu, const QString& deptNum )
{
    QMutexLocker guard( &m_ );
    LOG_DEBUG << "request plu " << qPrintable( plu ) << " deptNum " << qPrintable( deptNum );

    LOG_TRACE << "done";
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool walmartHookRFID::beforePluTextRead( const QString& number, const QString& deptNum, int type )
{
    Q_UNUSED( number );
    Q_UNUSED( deptNum );
    Q_UNUSED( type );

    QMutexLocker guard( &m_ );

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool walmartHookRFID::beforePrint( const QString& plu, const QString& deptNum, bool isRemotePrint )
{
    QMutexLocker guard( &m_ );
    LOG_DEBUG << "before print " << qPrintable( plu ) << " " << qPrintable( deptNum ) << " " << isRemotePrint;

    // keep track of items printed
    printingPlu_ = plu;
    printingDeptNum_ = deptNum;
    printingIsRemotePrint_ = isRemotePrint;

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void walmartHookRFID::afterPrint( bool success, int numLabelsToPrint, int numLabelsPrinted, bool templateLabel )
{
    QMutexLocker guard( &m_ );
    LOG_DEBUG << "after print " << success << " " << numLabelsToPrint << " " << numLabelsPrinted << " " << templateLabel;

    // sanity check
    if ( printingPlu_.isEmpty() )
        return;

    if ( !success )
    {
        LOG_DEBUG << "after print FAILURE";
        return;
    }

    if (( serverAddr_.isEmpty() ) || ( !serverPort_ ))
    {
        LOG_WARN << "no rfid server configured";
        return;
    }

    // ---- //

    const QString SQLITE3_DRIVER( "QSQLITE" );

    bool deleteConn( false );

    if ( dbname_.isEmpty() )
        LOG_WARN << "empty db name";
    else if ( !QSqlDatabase::isDriverAvailable( SQLITE3_DRIVER ) )
        LOG_ERROR << "database driver " << qPrintable( SQLITE3_DRIVER ) << " is not available!";
    else
    {
        // create new connection
        QSqlDatabase db( QSqlDatabase::addDatabase( SQLITE3_DRIVER, connectionName() ) );

        LOG_INFO << "opening database " << qPrintable( dbname_ ) << " ...";
        db.setDatabaseName( dbname_ );

        // open database
        if ( !db.open() )
            LOG_ERROR << "failed to open db connection";
        else
        {
            deleteConn = true;

            // execute pragmas
            QStringList pragmas;
            pragmas.append( "PRAGMA foreign_keys = ON" );
            pragmas.append( "PRAGMA journal_mode = WAL" );
            pragmas.append( "PRAGMA synchronous = OFF" );
            pragmas.append( "PRAGMA temp_store = MEMORY" );

            QSqlQuery query( db );

            foreach ( const QString& pragma, pragmas )
                if ( !query.exec( pragma ) )
                {
                    const QSqlError e( query.lastError() );

                    LOG_ERROR << "error during select " << e.type() << " " << qPrintable( e.text() );
                }

            // write
            const QByteArray a( generateRFID( db ) );

            if ( !a.length() )
                LOG_ERROR << "no rfid data to write";
            else
            {
                writeRFID( a );
            }

            // close database
            db.close();
        }
    }

    // delete connection
    if ( deleteConn )
        QSqlDatabase::removeDatabase( connectionName() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void walmartHookRFID::customAction( const QString& commandString, int &result, QString& resultExtended )
{
    QMutexLocker guard( &m_ );

    LOG_DEBUG << "perform custom action " << qPrintable( commandString );
    result = -1;

    // parse out command string parameters
    const QStringList actionParams( commandString.split( "|", KEEP_EMPTY_PARTS ) );

    if ( actionParams.size() <= 0 )
        LOG_WARN << "empty action";
    else
    {
        const QString action( actionParams[0] );

        LOG_DEBUG << "action " << qPrintable( action );

        if ( action.isEmpty() )
            LOG_WARN << "empty action";
        else
        {
            // TODO
        }
    }

    LOG_TRACE << "done";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void walmartHookRFID::updatePeripheralVersion( int device, int deviceEx, int kind, const QString& swVer, const QString& hwVer )
{
    QMutexLocker guard( &m_ );

    LOG_INFO << "version info " << device << " (" << deviceEx << ")" <<
        " kind " << kind <<
        " sw " << qPrintable( swVer ) <<
        " hw " << qPrintable( hwVer );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void walmartHookRFID::updateScaleState( int type, const QString& value )
{
    QMutexLocker guard( &m_ );

    LOG_INFO << "scale state " << type << " " << qPrintable( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void walmartHookRFID::readConfiguration()
{
    QMutexLocker guard( &m_ );

    QSettings internalConfigs( INTERNAL_CONFIG, QSettings::IniFormat );

    // ---- //

    serverAddr_ = internalConfigs.value( "rfidServerAddr" ).toString();

    if ( serverAddr_.isEmpty() )
        LOG_WARN << "empty server address!";

    serverPort_ = (quint16) internalConfigs.value( "rfidServerPort" ).toUInt();

    if ( !serverPort_ )
        LOG_WARN << "empty server port!";

    LOG_INFO << "using rfid server " << qPrintable( serverAddr_ ) << " " << serverPort_;

    format_ = internalConfigs.value( "dataFormat" ).toString();

    if ( format_.isEmpty() )
        LOG_WARN << "empty data format!";

    LOG_INFO << "data format " << qPrintable( format_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QByteArray walmartHookRFID::generateRFID( QSqlDatabase& db )
{
    static const QString sql(
        "SELECT * FROM Trans WHERE (TrID) IN ("
            "SELECT MAX(TrID) FROM Trans)" );

    if ( format_.isEmpty() )
        return QByteArray();

    LOG_DEBUG << "lookup transaction...";

    // exec
    QSqlQuery query( db );
    query.setForwardOnly( true );

    if ( !query.exec( sql ) )
    {
        const QSqlError e( query.lastError() );

        LOG_ERROR << "error during select " << e.type() << " " << qPrintable( e.text() );
    }
    else if ( !query.next() )
    {
        LOG_ERROR << "no transaction found! (turn on collect transactions?)";
    }
    else
    {
        // parse record
        const QSqlRecord rec( query.record() );

        const int id( rec.value( "TrID" ).toInt() );

        const QDateTime stamp( QDateTime::fromString( rec.value( "TrDtTm" ).toString(), "yyyy-MM-dd hh:mm:ss" ) );
        const int stampDelta( abs( stamp.secsTo( QDateTime::currentDateTime() ) ) );

        LOG_DEBUG << "trans " << id << " with delta " << stampDelta;

        if ( prevTrans_ == id )
            LOG_ERROR << "duplicate transaction! (turn on collect transactions?)";
        else if ( TRANS_SECS_MAX < stampDelta )
            LOG_ERROR << "transaction too old! (turn on collect transactions?)";
        else
        {
            prevTrans_ = id;

            // ---- //

            QString data( format_ );

            LOG_TRACE << "format string " << qPrintable( data );

            // iterate all fields
            for ( int i( rec.count() ); i--; )
            {
                const QString fname( rec.fieldName( i ) );
                const QString fvalue( rec.value( i ).toString() );

                LOG_TRACE << "processing " << qPrintable( fname ) << " " << qPrintable( fvalue );

                data.replace( "%" + fname + "%", fvalue );
            }

            data.replace( "\\n", "\n" );
            data.replace( "\\r", "\r" );

            LOG_DEBUG << "data " << qPrintable( data );

            return data.toLatin1();
        }
    }

    return QByteArray();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void walmartHookRFID::writeRFID( const QByteArray& a ) const
{
    if (( serverAddr_.isEmpty() ) || ( !serverPort_ ))
        return;

    // connect
    QTcpSocket s;
    s.connectToHost( serverAddr_, serverPort_ );

    if ( !s.waitForConnected( SOCKET_TIMEOUT ) )
    {
        LOG_WARN << "could not connect to " << qPrintable( serverAddr_ ) << " " << serverPort_;
        return;
    }

    LOG_DEBUG << "==== connect ====";

    // write!
    const qint64 written( s.write( a ) );

    if ( written != a.size() )
        LOG_WARN << "error writing data to socket";
    else if ( !s.waitForBytesWritten( SOCKET_TIMEOUT ) )
        LOG_WARN << "timeout waiting for write";
    else
    {
        LOG_DEBUG << "SUCCESS (written)";
        LOG_DEBUG << qPrintable( a );
    }

    // close
    LOG_DEBUG << "close socket";
    s.close();

    LOG_TRACE << "done";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString walmartHookRFID::connectionName()
{
    static const QString CONN_NAME( "%1_%2" );

    // return unique connection name
    return CONN_NAME
        .arg( "conn" )
        .arg( (quintptr) QThread::currentThreadId() );
}
