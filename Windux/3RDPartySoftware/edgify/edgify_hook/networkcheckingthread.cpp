/**
 * @file networkcheckingthread.cpp
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#include "common.h"
#include "networkcheckingthread.h"

#include <QDataStream>
#include <QFile>
#include <QHostAddress>
#include <QHostInfo>
#include <QMutexLocker>
#include <QNetworkAddressEntry>
#include <QNetworkInterface>
#include <QProcess>
#include <QStringList>

#if defined(Q_OS_LINUX)
#define ONLINE_TIME_FILE "/usr/share/edgify-apphook/lastonline"
#elif defined(Q_OS_WIN32)
#define ONLINE_TIME_FILE "lastonline"
#endif

//#define USE_DEBUG_VALUES

///////////////////////////////////////////////////////////////////////////////////////////////////
networkCheckingThread::networkCheckingThread( QObject *parent ) :
    _Mybase( parent ),
    m_( QMutex::Recursive ),
    wired_( false ),
    wireless_( false ),
    checkingInterval_( DEFAULT_CHECKING_INTERVAL )
{
    // import last online time from filesystem
    importOnlineTime( lastSuccess_ );

    const QString str( lastSuccess_.toString() );
    LOG_INFO << "last success " << qPrintable( str );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
networkCheckingThread::~networkCheckingThread()
{
    // stop thread
    if ( isRunning() )
    {
        LOG_DEBUG << "request thread termination";
        requestInterruption();

        LOG_DEBUG << "wait for termination...";

        do
        {
            QThread::msleep( 50 );

        } while ( isRunning() );

        LOG_TRACE << "wait for termination... complete";
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int networkCheckingThread::checkingInterval() const
{
    QMutexLocker guard( &m_ );
    return checkingInterval_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void networkCheckingThread::setCheckingInterval( int value )
{
    QMutexLocker guard( &m_ );
    checkingInterval_ = value;

    LOG_INFO << "network checking interval is " << checkingInterval_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool networkCheckingThread::isOnline() const
{
    QMutexLocker guard( &m_ );
    return (( wired_ ) || ( wireless_ ));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QDateTime networkCheckingThread::lastOnlineTime() const
{
    QMutexLocker guard( &m_ );
    return lastSuccess_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void networkCheckingThread::setLastOnlineTime( const QDateTime& value )
{
    QMutexLocker guard( &m_ );
    lastSuccess_ = value;

    exportOnlineTime( lastSuccess_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool networkCheckingThread::forceCheckOnlineStatus()
{
    QMutexLocker guard( &m_ );
    checkOnlineStatus();

    return isOnline();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void networkCheckingThread::run()
{
    LOG_DEBUG << "network checking thread running!";

    bool exit( false );

    do
    {
        checkOnlineStatus();

        // ---- //

        LOG_TRACE << "sleeping until next checking time...";

        int nextCheckingTime( checkingInterval() );

        do
        {
            if ( currentThread()->isInterruptionRequested() )
            {
                exit = true;
                break;
            }

            msleep( IDLE_TIME );

        } while ( 0 < (nextCheckingTime -= IDLE_TIME) );

    } while ( !exit );

    LOG_DEBUG << "network checking thread exiting";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void networkCheckingThread::checkOnlineStatus()
{
    QString program( "python" );

    QStringList args;
    args.append( "getConnectionStatus.py" );
    args.append( "-F" );
    args.append( "1" );

    // ---- //

    QProcess statusCheck;
    statusCheck.setWorkingDirectory( "/usr/local/hobart/python" );

    LOG_TRACE << "checking status...";
    statusCheck.start( program, args );

    if ( !statusCheck.waitForStarted() )
        LOG_WARN << "status check process did not start!";
    else if ( !statusCheck.waitForFinished() )
        LOG_WARN << "status check process did not complete!";
    else
    {
        const QByteArray a( statusCheck.readAllStandardOutput() );

        int strength;
        ConnectionType ct;
        bool online;

        if ( !parseStatus( a, strength, ct, online ) )
        {
            LOG_WARN << "bad return value from status check";

#if defined(Q_OS_LINUX)
            LOG_DEBUG << HEX_DUMP( a.constData(), a.length() );
#elif defined(Q_OS_WIN32)
            LOG_TRACE << a;
#endif
        }
        else
        {
            const bool onlinePrev( isOnline() );

            int offlineAmount( 0 );

            LOG_INFO << "connection status " << strength << " " << ct << " " << online;

            QMutexLocker guard( &m_ );

            wired_ = false;
            wireless_ = false;

            if ( !online )
                LOG_WARN << "OFFLINE";
            else
            {
                const QDateTime now( QDateTime::currentDateTime() );

                // calculate offline amount
                offlineAmount = lastSuccess_.msecsTo( now );

                // set last online time
                setLastOnlineTime( now );

                if ( CT_WIRED == ct )
                    wired_ = true;
                else if ( CT_WIRELESS == ct )
                    wireless_ = true;
            }

            // when status changed emit signal
            if ( onlinePrev != online )
            {
                LOG_DEBUG << "online status changed...";
                emit onlineStatusChanged( online, offlineAmount );

                LOG_TRACE << "online status changed... done";
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool networkCheckingThread::parseStatus( const QString& status, int& strength, ConnectionType& ct, bool& online )
{
    // comma separated string in form:
    // wireless strengh,device type,alive
    if ( 2 != status.count( ',' ) )
        return false;

    const QStringList values( status.split( ',' ) );

    if ( values.size() < 3 )
        return false;

    const QString v0( values[0] );
    const QString v1( values[1] );
    const QString v2( values[2] );

    if (( v0.length() < 1 ) || ( v0[0] < '0' ) || ( '5' < v0[0] ))
        return false;

    if (( v1.length() < 1 ) || ( v1[0] < '0' ) || ( '1' < v1[0] ))
        return false;

    if (( v2.length() < 1 ) || ( v2[0] < '0' ) || ( '1' < v2[0] ))
        return false;

    strength = v0[0].toLatin1() - '0';

    if ( '0' == v1[0] )
        ct = CT_WIRED;
    else if ( '1' == v1[0] )
        ct = CT_WIRELESS;

    online = ('1' == v2[0]);

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void networkCheckingThread::exportOnlineTime( const QDateTime& value )
{
    QFile lastonline( ONLINE_TIME_FILE );

    if ( !lastonline.open( QIODevice::WriteOnly ) )
    {
        LOG_WARN << "could not open last online time file " << ONLINE_TIME_FILE;
        return;
    }

    QDataStream s( &lastonline );
    s << value;

    lastonline.close();

    const QString str( value.toString() );
    LOG_DEBUG << "wrote last online time as " << qPrintable( str );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void networkCheckingThread::importOnlineTime( QDateTime& value )
{
    QDateTime dt;

    if ( !QFile::exists( ONLINE_TIME_FILE ) )
    {
        LOG_WARN << "last online time file does not exist";
        return;
    }

    QFile lastonline( ONLINE_TIME_FILE );

    if ( !lastonline.open( QIODevice::ReadOnly ) )
    {
        LOG_WARN << "could not open last online time file " << ONLINE_TIME_FILE;
        return;
    }

    QDataStream s( &lastonline );
    s >> dt;

    lastonline.close();

    if (( dt.isNull() ) || ( !dt.isValid() ))
    {
        LOG_WARN << "last online time is invalid";
        return;
    }

    const QString str( dt.toString() );
    LOG_DEBUG << "read last online time as " << qPrintable( str );

    value = dt;
}
