/**
 * @file main.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include <custgstmp.h>
#include <logmanager.h>
#include <opergstmp.h>

#include <appinit.h>

#include <signal.h>
#include <unistd.h>

#include <QApplication>
#include <QByteArray>
#include <QDBusConnection>
#include <QDBusError>
#include <QDir>
#include <QThread>

///////////////////////////////////////////////////////////////////////////////////////////////////
void sig_handler( int /*sig*/, siginfo_t* /*siginfo*/, void* /*context*/ )
{
    LOG_CLASS_INFO( "", "sig_handler" );

    LOG_INFO( "caught interrupt, exiting...", NULL );
    QCoreApplication::exit();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString pidfile( const QStringList &arguments )
{
    QDir tempFolder( QDir::temp() );
    QString pidfile;

    if ( arguments.contains( "-cust" ) )
        pidfile = tempFolder.absoluteFilePath( "gstpdcust.lock" );
    else if ( arguments.contains( "-oper" ) )
        pidfile = tempFolder.absoluteFilePath( "gstpdoper.lock" );

    return pidfile;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void create_lock( const QString &filename )
{
    LOG_CLASS_INFO( "", "create_lock" );

    // when lockfile exists there is another process running... only one process may run so lets
    // kill the other process
    if ( QFile::exists( filename ) )
    {
        QFile f( filename );
        QByteArray buffer;

        // open file and read contents
        if ( f.open( QFile::ReadOnly ) )
        {
            buffer = f.readAll();

            f.close();
        }

        // kill process
        if ( buffer.length() )
        {
            unsigned int pid( buffer.toInt() );

            if ( 0 < pid )
            {
                const QString cmd_int( QString( "kill -INT %0" ).arg( pid ) );
                const QString cmd_term( QString( "kill -9 %0" ).arg( pid ) );

                LOG_INFO( "killing pid %d", pid );

                // kill it nicely...
                LOG_DEBUG( "invoking '%s'...", qPrintable( cmd_int ) );
                system( cmd_int.toLatin1().constData() );

                // wait a moment for termination
                QThread::msleep( 500 );

                if ( QFile::exists( filename ) )
                {
                    // kill it forcefully...
                    LOG_DEBUG( "invoking '%s'...", qPrintable( cmd_term ) );
                    system( cmd_term.toLatin1().constData() );

                    QThread::msleep( 500 );

                    // remove lock
                    QFile::remove( filename );
                }
            }
        }
    }

    // generate new lock file
    QFile f( filename );

    if ( f.open( QFile::WriteOnly | QFile::Truncate ) )
    {
        QByteArray buffer;
        buffer.append( QString( "%0" ).arg( getpid() ) );

        f.write( buffer );
        f.close();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void destroy_lock( const QString &filename )
{
    // remove lock
    QFile::remove( filename );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int main( int argc, char *argv[] )
{
    LOG_CLASS_INFO( "", "main" );

    QApplication a( argc, argv );
    QStringList arguments( a.arguments() );

    int result( 0 );

    // init app
    appInitialize( "gstpd" );

    // Note:
    //
    // For linuxfb platform Qt blocks signals we rely upon...
    // https://codereview.qt-project.org/#/c/126672/
    //
    // So let's unblock these signals. This should be resolved in 5.6?
    sigset_t mask;
    sigemptyset( &mask );

    sigaddset( &mask, SIGINT );
    sigaddset( &mask, SIGTERM );

    pthread_sigmask( SIG_UNBLOCK, &mask, 0 );

    // register interrupt handlers
    struct sigaction action;

    memset( &action, 0, sizeof(action) );
    action.sa_sigaction = &sig_handler;
    action.sa_flags = SA_SIGINFO;

    if ( (result = sigaction( SIGINT, &action, NULL )) < 0 )
        LOG_ERROR( "error setting sigaction for SIGINT %d", result );

    if ( (result = sigaction( SIGTERM, &action, NULL )) < 0 )
        LOG_ERROR( "error setting sigaction for SIGTERM %d", result );

    // validate dbus running
    QDBusConnection dbus( QDBusConnection::sessionBus() );

    if ( !dbus.isConnected() )
    {
        LOG_ERROR( "failed to connect to DBUS", NULL );
        result = -1;
    }
    else
    {
        // create player
        dbusMediaPlayer *player( NULL );

        if ( arguments.contains( "-cust" ) )
            player = new custgstmp();
        else if ( arguments.contains( "-oper" ) )
            player = new opergstmp();
        else
        {
            LOG_ERROR( "display parameter missing!", NULL );
            result = -2;
        }

        if ( player )
        {
            LOG_DEBUG( "creating lock file...", NULL );
            create_lock( pidfile( arguments ) );

            LOG_INFO( "registering service '%s'...", qPrintable( player->dbusServiceName() ) );

            // register player object
            if ( !dbus.registerService( player->dbusServiceName() ) )
            {
                LOG_ERROR( "failed to register service: %s", qPrintable( dbus.lastError().message() ) );
                result = -3;
            }
            else if ( !dbus.registerObject( player->dbusObjectPath(), player, QDBusConnection::ExportAllContents ) )
            {
                LOG_ERROR( "failed to register object '%s': %s", qPrintable( player->dbusObjectPath() ), qPrintable( dbus.lastError().message() ) );
                result = -4;
            }
            else
            {
                LOG_INFO( "object interface: %s", qPrintable( player->dbusObjectInterface() ) );
                LOG_INFO( "object path: %s", qPrintable( player->dbusObjectPath() ) );

                // exec!
                LOG_DEBUG( "entering application loop...", NULL );
                a.exec();

                // cleanup
                LOG_TRACE( "unregister objects", NULL );
                dbus.unregisterObject( player->dbusObjectPath() );
                dbus.unregisterService( player->dbusServiceName() );
            }

            LOG_TRACE( "finalize", NULL );
            delete player;

            LOG_DEBUG( "destroying lock file...", NULL );
            destroy_lock( pidfile( arguments ) );
        }
    }

    // finalize
    appFinalize();

    return result;
}
