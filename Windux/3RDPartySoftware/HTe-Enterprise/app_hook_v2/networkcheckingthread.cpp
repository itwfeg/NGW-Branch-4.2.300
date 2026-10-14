/**
 * @file networkcheckingthread.cpp
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#include "common.h"
#include "errordialogmgr.h"
#include "networkcheckingthread.h"
#include "scaleconfig.h"
#include "stringsconfig.h"

#include <QDataStream>
#include <QFile>
#include <QHostAddress>
#include <QHostInfo>
#include <QMutexLocker>
#include <QNetworkAddressEntry>
#include <QNetworkInterface>
#include <QProcess>
#include <QStringList>

//#define USE_DEBUG_VALUES

///////////////////////////////////////////////////////////////////////////////////////////////////
networkCheckingThread::networkCheckingThread( QObject *parent ) :
    _Mybase( parent ),
    m_( QMutex::Recursive ),
    wired_( false ),
    wireless_( false ),
    lockout_( false ),
    checkingInterval_( DEFAULT_CHECKING_INTERVAL ),
    d_( nullptr )
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
        checkDialogTimes();

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
void networkCheckingThread::checkDialogTimes()
{
    const QDateTime now( QDateTime::currentDateTime() );

    int offlineTimerMax;

    dialogStartStopTime dialog0;
    int dialog0_freq;

    dialogStartStopTime dialog1;
    int dialog1_freq;

    if ( !lookupOfflineDialogTimes( offlineTimerMax, dialog0, dialog0_freq, dialog1, dialog1_freq ) )
    {
        LOG_WARN << "invalid or missing dialog times";
        return;
    }
    else if ( offlineTimerMax <= 0 )
        return;
    else if ( !lastSuccess_.isValid() )
        return;
    else if ( now <= lastSuccess_ )
        return;

    // online
    if ( isOnline() )
    {
        // remove all dialog
        if ( lockout_ )
        {
            LOG_INFO << "back online from lockout... terminate dialog...";
            errorDialogManager::instance()->terminate();
            lockout_ = false;

            d_ = nullptr;
        }

        else if ( d_ )
        {
            LOG_INFO << "back online... terminate dialog...";
            errorDialogManager::instance()->terminate( d_ );
            d_ = nullptr;
        }

        return;
    }

    // compute offline countdown timer
    const int offlineAmount( lastSuccess_.msecsTo( now ) );

    if ( offlineAmount <= 0 )
        return;

    const int countdownTimer( offlineTimerMax - offlineAmount );
    const int countdownTimerHours( std::ceil( (double)(countdownTimer) / (double)(60 * 60 * 1000) ) );

    LOG_DEBUG << "countdown timer at " << countdownTimer << " (" << countdownTimerHours << " hr)";

    // countdown at 0 hours
    if ( countdownTimer <= 0 )
    {
        LOG_TRACE << "lockout";
        lockout_ = true;
    }

    // SF-5.a
    // coundown in first range (between 120 and 72 hours)
    else if (( dialog0.second < dialog0.first ) && ( 0 < dialog0.first ) && ( 0 < dialog0_freq ) &&
             ( dialog0.second < countdownTimer ) && ( countdownTimer <= dialog0.first ))
    {
        if (( !lastOfflineDialog_.isValid() ) || ( now < lastOfflineDialog_ ) || ( dialog0_freq <= lastOfflineDialog_.msecsTo( now ) ))
        {
            LOG_INFO << "showing first range continue to use dialog";
            errorDialogManager::instance()->showContinueToUse( lastSuccess_, countdownTimerHours, &d_ );
            lastOfflineDialog_ = now;
        }
    }

    // SF-5.b
    // coundown in second range (between 72 and 24 hours)
    else if (( dialog1.second < dialog1.first ) && ( 0 < dialog1.first ) && ( 0 < dialog1_freq ) &&
             ( dialog1.second < countdownTimer ) && ( countdownTimer <= dialog1.first ))
    {
        if (( !lastOfflineDialog_.isValid() ) || ( now < lastOfflineDialog_ ) || ( dialog1_freq <= lastOfflineDialog_.msecsTo( now ) ))
        {
            LOG_INFO << "showing second range continue to use dialog";
            errorDialogManager::instance()->showContinueToUse( lastSuccess_, countdownTimerHours, &d_ );
            lastOfflineDialog_ = now;
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
        LOG_WARN << "could not open last online time file " << qPrintable(ONLINE_TIME_FILE);
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
        LOG_WARN << "could not open last online time file " << qPrintable(ONLINE_TIME_FILE);
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

///////////////////////////////////////////////////////////////////////////////////////////////////
bool networkCheckingThread::lookupOfflineDialogTimes( int& max, dialogStartStopTime& d0, int& d0_freq, dialogStartStopTime& d1, int& d1_freq )
{
    static const int CONVERT_HOURS( 60 * 60 * 1000 );
    static const int CONVERT_MINS( 60 * 1000 );

#ifdef USE_DEBUG_VALUES
    // offline timer maximum
    max = 144 * CONVERT_HOURS;              // 144h

    // dialog values
    d0.first = 120 * CONVERT_HOURS;         // 120h
    d0.second = 72 * CONVERT_HOURS;         // 72h
    d0_freq = 60 * CONVERT_MINS;            // 60m

    d1.first = 72 * CONVERT_HOURS;          // 72h
    d1.second = 24 * CONVERT_HOURS;         // 24h
    d1_freq = 30 * CONVERT_MINS;            // 30m

#else
    // list of each path
    QStringList paths;
    paths.append( CONF_OFFLINE_TIMER_MAX );
    paths.append( CONF_FIRST_OFFLINE_MSG_START );
    paths.append( CONF_FIRST_OFFLINE_MSG_STOP );
    paths.append( CONF_FIRST_OFFLINE_MSG_FREQ );
    paths.append( CONF_SECOND_OFFLINE_MSG_START );
    paths.append( CONF_SECOND_OFFLINE_MSG_STOP );
    paths.append( CONF_SECOND_OFFLINE_MSG_FREQ );

    // lookup each value
    bool valid;

    foreach ( const QString& path, paths )
    {
        const int value( scaleConfig::instance()->value<int>( path, &valid ) );

        if ( !valid )
        {
            LOG_WARN << "bad or missing value for path " << qPrintable( path );
            return false;
        }

        if ( CONF_OFFLINE_TIMER_MAX == path )
            max = value * CONVERT_HOURS;
        else if ( CONF_FIRST_OFFLINE_MSG_START == path )
            d0.first = value * CONVERT_HOURS;
        else if ( CONF_FIRST_OFFLINE_MSG_STOP == path )
            d0.second = value * CONVERT_HOURS;
        else if ( CONF_FIRST_OFFLINE_MSG_FREQ == path )
            d0_freq = value * CONVERT_MINS;
        else if ( CONF_SECOND_OFFLINE_MSG_START == path )
            d1.first = value * CONVERT_HOURS;
        else if ( CONF_SECOND_OFFLINE_MSG_STOP == path )
            d1.second = value * CONVERT_HOURS;
        else if ( CONF_SECOND_OFFLINE_MSG_FREQ == path )
            d1_freq = value * CONVERT_MINS;
    }

#endif

    return true;
}
