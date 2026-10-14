/**
 * @file networkcheckingthread.cpp
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#include "customerhook.h"
#include "errordialogmgr.h"
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

#include <cmath>

#include <clio.h>

#define ONLINE_TIME_FILE "/usr/share/walmart-apphook/lastonline"

///////////////////////////////////////////////////////////////////////////////////////////////////
networkCheckingThread::networkCheckingThread( QObject *parent ) :
    _Mybase( parent ),
    m_( QMutex::Recursive ),
    wired_( false ),
    wireless_( false ),
    lockout_( false ),
    checkingInterval_( DEFAULT_CHECKING_INTERVAL ),
    offlineTimerMax_( 0 ),
    dialog0_freq_( 0 ),
    dialog1_freq_( 0 ),
    d_( nullptr )
{
    // import last online time from filesystem
    importOnlineTime( lastSuccess_ );

    const QString str( lastSuccess_.toString() );
    LOG_INFO << "last success " << str.toStdString();

    // initialize dialog values
    dialog0_.first = dialog0_.second = 0;
    dialog1_.first = dialog1_.second = 0;

    // retrieve adapter info
    queryIpAddress();

    // check online
    checkOnlineStatus();
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
    return (wired_ || wireless_);
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
void networkCheckingThread::setOfflineTimerMax( int value )
{
    QMutexLocker guard( &m_ );
    offlineTimerMax_ = value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void networkCheckingThread::setDialogTimes( int d, const QPair<int, int>& times, int freq )
{
    QMutexLocker guard( &m_ );

    if ( 0 == d )
    {
        dialog0_ = times;
        dialog0_freq_ = freq;
    }
    else if ( 1 == d )
    {
        dialog1_ = times;
        dialog1_freq_ = freq;
    }
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
        int addressTime( QUERY_IP_ADDRESS );

        do
        {
            if ( currentThread()->isInterruptionRequested() )
            {
                exit = true;
                break;
            }

            msleep( IDLE_TIME );

            if ( (addressTime -= IDLE_TIME) <= 0 )
            {
                queryIpAddress();
                addressTime = QUERY_IP_ADDRESS;
            }

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

    if ( !statusCheck.waitForFinished() )
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
            LOG_DEBUG << HEX_DUMP( a.constData(), a.length() );
        }
        else
        {
            LOG_INFO << "connection status " << strength << " " << ct << " " << online;

            QMutexLocker guard( &m_ );

            wired_ = false;
            wireless_ = false;

            if ( !online )
                LOG_WARN << "OFFLINE";
            else
            {
                setLastOnlineTime( QDateTime::currentDateTime() );

                if ( CT_WIRED == ct )
                    wired_ = true;
                else if ( CT_WIRELESS == ct )
                    wireless_ = true;
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void networkCheckingThread::checkDialogTimes()
{
    const QDateTime now( QDateTime::currentDateTime() );

    if ( offlineTimerMax_ <= 0 )
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

    const int countdownTimer( offlineTimerMax_ - offlineAmount );
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
    else if (( dialog0_.second < dialog0_.first ) && ( 0 < dialog0_.first ) && ( 0 < dialog0_freq_ ) &&
             ( dialog0_.second < countdownTimer ) && ( countdownTimer <= dialog0_.first ))
    {
        if (( !lastOfflineDialog_.isValid() ) || ( now < lastOfflineDialog_ ) || ( dialog0_freq_ <= lastOfflineDialog_.msecsTo( now ) ))
        {
            LOG_INFO << "showing first range continue to use dialog";
            errorDialogManager::instance()->showContinueToUse( lastSuccess_, countdownTimerHours, &d_ );
            lastOfflineDialog_ = now;
        }
    }

    // SF-5.b
    // coundown in second range (between 72 and 24 hours)
    else if (( dialog1_.second < dialog1_.first ) && ( 0 < dialog1_.first ) && ( 0 < dialog1_freq_ ) &&
             ( dialog1_.second < countdownTimer ) && ( countdownTimer <= dialog1_.first ))
    {
        if (( !lastOfflineDialog_.isValid() ) || ( now < lastOfflineDialog_ ) || ( dialog1_freq_ <= lastOfflineDialog_.msecsTo( now ) ))
        {
            LOG_INFO << "showing second range continue to use dialog";
            errorDialogManager::instance()->showContinueToUse( lastSuccess_, countdownTimerHours, &d_ );
            lastOfflineDialog_ = now;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void networkCheckingThread::queryIpAddress()
{
    const QString localhostname( QHostInfo::localHostName() );
    const QList<QHostAddress> addresses( QHostInfo::fromName( localhostname ).addresses() );

    QString ip;

    // loop through host addresses, find the valid ipv4 one
    foreach ( const QHostAddress& addr, addresses )
        if (( QAbstractSocket::IPv4Protocol == addr.protocol() ) && ( !addr.isLoopback() ))
        {
            ip = addr.toString();

            if ( ip.isEmpty() )
                continue;

            customerHook::instance()->setIpAddress( ip );
            break;
        }

    // loop through adapters to get mac address
    foreach ( const QNetworkInterface& networkInterface, QNetworkInterface::allInterfaces() )
        foreach ( const QNetworkAddressEntry& entry, networkInterface.addressEntries() )
            if ( ip == entry.ip().toString() )
            {
                customerHook::instance()->setMacAddress( networkInterface.hardwareAddress() );
                break;
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
    LOG_DEBUG << "wrote last online time as " << str.toStdString();
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
    LOG_DEBUG << "read last online time as " << str.toStdString();

    value = dt;
}
