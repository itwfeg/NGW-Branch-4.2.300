/**
 * @file sighandler.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "sighandler.h"

#include <logmanager.h>

#include <QCoreApplication>
#include <QSocketNotifier>

#if defined( Q_OS_LINUX ) || defined( Q_OS_UNIX )
#include <unistd.h>
#include <sys/types.h>
#include <sys/signal.h>
#include <sys/socket.h>
#elif defined( Q_OS_WIN )
#include <io.h>
#endif

sigHandler *sigHandler::instance_( NULL );
QMutex sigHandler::instanceMutex_;

///////////////////////////////////////////////////////////////////////////////////////////////////
sigHandler::sigHandler( QObject *parent ) :
    _Mybase( parent )
{
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
sigHandler::~sigHandler()
{
    QMutexLocker guard( &instanceMutex_ );
    instance_ = NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
sigHandler *sigHandler::instance()
{
    LOG_CLASS_INFO( "sigHandler", "instance" );

    if ( !instance_ )
    {
        QMutexLocker guard( &instanceMutex_ );

        if ( !instance_ )
        {
            LOG_DEBUG( "creating instance...", NULL );
            instance_ = new sigHandler( qApp );
        }
    }

    return instance_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sigHandler::onSignal( int sig )
{
    LOG_CLASS_INFO( "sigHandler", "onSignal" );

    LOG_DEBUG( "handle signal %d...", sig );

    QMutexLocker guard( &m_ );

    // write to socket
    if ( !writers_.contains( sig ) )
         LOG_WARN( "failed to find writer for signal %d", sig );
    else
    {
         char a( 1 );
         ::write( writers_[sig], &a, sizeof( a ) );
    }

    LOG_DEBUG( "done", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sigHandler::onSocketActivated( int fd )
{
    LOG_CLASS_INFO( "sigHandler", "onSocketActivated" );

    LOG_DEBUG( "clearing socket...", NULL );

    char a;
    ::read( fd, &a, sizeof( a ) );

    QMutexLocker guard( &m_ );

    // sanity check
    if ( !readers_.contains( fd ) )
    {
         LOG_WARN( "failed to find fd", NULL );
         return;
    }

    int sig( readers_[fd] );

    LOG_DEBUG( "handle signal %d...", sig );

#if defined( Q_OS_LINUX ) || defined( Q_OS_UNIX )

    // handle signal
    switch ( sig )
    {
    case SIGHUP:
        Q_EMIT hup();
        break;
    case SIGTERM:
        Q_EMIT term();
        break;
    default:
        break;
    }
#endif

    LOG_DEBUG( "done", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sigHandler::initialize()
{
    LOG_CLASS_INFO( "sigHandler", "initialize" );

#if defined( Q_OS_LINUX ) || defined( Q_OS_UNIX )

    // list of signals we wish to handle
    QList<int> signalList;
    signalList.push_back( SIGHUP );
    signalList.push_back( SIGTERM );

    // install handlers for each signal
    foreach ( int sig, signalList )
    {
        int fd[2]; // write/read socket fd pair
        int rc;

        LOG_DEBUG( "creating handler for signal %d...", sig );

        // create read and write sockets
        if ( (rc = ::socketpair( AF_UNIX, SOCK_STREAM, 0, fd )) )
        {
            LOG_WARN( "error %d creating socket pair for signal %d", rc, sig );
            return;
        }

        // create notifier
        QSocketNotifier *sn( new QSocketNotifier( fd[1], QSocketNotifier::Read, this ) );

        connect( sn, SIGNAL( activated(int) ), SLOT( onSocketActivated(int) ) );

        // save socket mapping
        QMutexLocker guard( &m_ );
        readers_[ fd[1] ] = sig;
        writers_[ sig ] = fd[0];

        // install handler
        struct sigaction sa;
        sa.sa_handler = _Myt::handlerFunc;
        sigemptyset( &sa.sa_mask );
        sa.sa_flags = SA_RESTART;

        if ( 0 < (rc = ::sigaction( sig, &sa, 0 )) )
        {
            LOG_WARN( "error %d installing handler for signal %d", rc, sig );
            return;
        }
    }
#endif

    LOG_DEBUG( "done", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void sigHandler::handlerFunc( int sig )
{
    QMutexLocker guard( &instanceMutex_ );

    if ( instance_ )
        instance_->onSignal( sig );
}
