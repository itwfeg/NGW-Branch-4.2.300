/**
 * @file main.cpp
 * Application entry point.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <appinit.h>
#include <filesystem.h>
#include <logmanager.h>
#include <platform.h>

#include <sessionhooks.h>
#include <sessionmainwindow.h>
#include <touchscreenstyle.h>

#include <HobSocket.h>
#include <Session.h>

#include <QApplication>
#include <QDesktopWidget>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QTime>
#include <QThread>

#include <iostream>

#if defined( Q_OS_WIN )
#include <windows.h>
#endif

#if defined( Q_OS_LINUX )
#include <sys/time.h>
#include <sys/resource.h>
#endif

#include <CkGlobal.h>
#include <CkSshTunnel.h>

// Allow Squish to attach to a running process using the built-in hook
#include "../../shared/squish/qtbuiltinhook.h"

// this variable can be used by various modules to check if we are in demo mode.
bool isDemoMode = false;

// constants for palette colors
static const struct {
    unsigned int active;
    unsigned int disabled;
    unsigned int inactive;
} PALETTE_COLORS[20] = {
    {0xff000000, 0xff787878, 0xff000000}, // WindowText
    {0xfff0f0f0, 0xfff0f0f0, 0xfff0f0f0}, // Button
    {0xffffffff, 0xffffffff, 0xffffffff}, // Light
    {0xffe3e3e3, 0xfff7f7f7, 0xffe3e3e3}, // Midlight
    {0xffa0a0a0, 0xffa0a0a0, 0xffa0a0a0}, // Dark
    {0xffa0a0a0, 0xffa0a0a0, 0xffa0a0a0}, // Mid
    {0xff000000, 0xff787878, 0xff000000}, // Text
    {0xffffffff, 0xffffffff, 0xffffffff}, // BrightText
    {0xff000000, 0xff787878, 0xff000000}, // ButtonText
    {0xffffffff, 0xfff0f0f0, 0xffffffff}, // Base
    {0xfff0f0f0, 0xfff0f0f0, 0xfff0f0f0}, // Window
    {0xff696969, 0xff000000, 0xff696969}, // Shadow
    {0xff3399ff, 0xff3399ff, 0xfff0f0f0}, // Highlight
    {0xffffffff, 0xffffffff, 0xff000000}, // HighlightedText
    {0xff0000ff, 0xff0000ff, 0xff0000ff}, // Link
    {0xffff00ff, 0xffff00ff, 0xffff00ff}, // LinkVisited
    {0xfff6f6f6, 0xfff6f6f6, 0xfff6f6f6}, // AlternateBase
    {0xff000000, 0xff000000, 0xff000000}, // NoRole
    {0xffffffdc, 0xffffffdc, 0xffffffdc}, // ToolTipBase
    {0xff000000, 0xff000000, 0xff000000}  // ToolTipText
};

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Log palette colors
 */
void logApplicationPalette()
{
    LOG_CLASS_INFO( "", "logApplicationPalette" );

    LOG_DEBUG( "application palette:", NULL );
    const QPalette p( QApplication::palette() );

    for ( int cr = 0; cr < QPalette::NColorRoles; ++cr )
    {
        const QColor active( p.color( QPalette::Active, (QPalette::ColorRole) cr ) );
        const QColor disabled( p.color( QPalette::Disabled, (QPalette::ColorRole) cr ) );
        const QColor inactive( p.color( QPalette::Inactive, (QPalette::ColorRole) cr ) );

        LOG_DEBUG( "0x%02x: 0x%08x 0x%08x 0x%08x", cr, active.rgba(), disabled.rgba(), inactive.rgba() );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Create socket connection to scale backend.
 */
HobSocket *connectToScale( const QString& address, quint16 port )
{
    LOG_CLASS_INFO( "", "connectToScale" );

    QTime timeoutTime( QTime::currentTime().addSecs( 30 ) );
    HobSocket *socket = NULL;

    for ( ;; )
    {
        // create socket connection
        if ( isDemoMode )
        {
            LOG_INFO( "creating demo socket...", NULL );
            socket = new HobSocket();
        }
        else
        {
            LOG_INFO( "connecting to '%s' on port %d...", address.toLatin1().constData(), port );
            socket = new HobSocket( address, port );
        }

        // check socket
        if ( socket->isConnected() )
        {
            LOG_INFO( "connected!", NULL );
            break;
        }
        else if ( isDemoMode )
        {
            LOG_INFO( "demo mode socket", NULL );
            break;
        }

        // delete existing socket
        if ( socket )
        {
            LOG_TRACE( "delete bad socket...", NULL );

            delete socket;
            socket = NULL;
        }

        if ( timeoutTime <= QTime::currentTime() )
        {
            LOG_ERROR( "no connection... timeout!", NULL );
            break;
        }

        LOG_WARN( "no connection... sleeping...", NULL );

        QTime sleep( QTime::currentTime().addSecs( 5 ) );

        do
        {
            QApplication::processEvents();

        } while ( QTime::currentTime() < sleep );
    }

    return socket;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Output help information.
 */
void displayHelp()
{
    std::cout << "SCALEUI" << std::endl;
    std::cout << "Copyright (c) 2013 Hobart Corporation" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: scaleui [[-demo] | [[-address ip] [-port p] [-primary | -secondary]]]" << std::endl;
    std::cout << "               [[-user u] -pwd s [-tunnel]]" << std::endl;
    std::cout << "               [-nologging | -error | -warn | -info | -debug | -trace]" << std::endl;
    std::cout << "               [-useospalette]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "    -demo           Launch in demo mode with no backend connection" << std::endl;
    std::cout << std::endl;
    std::cout << "    -address ip     Use ip as backend instead of 127.0.0.1" << std::endl;
    std::cout << "    -port p         Use port p for backend connection" << std::endl;
    std::cout << "    -primary        Connect as primary session" << std::endl;
    std::cout << "    -secondary      Connect as secondary session" << std::endl;
    std::cout << std::endl;
    std::cout << "    -full           Show fullscreen" << std::endl;
    std::cout << "    -windowed       Show windowed" << std::endl;
    std::cout << "    -ww w           Use window width of w" << std::endl;
    std::cout << "    -wh h           Use window height of h" << std::endl;
    std::cout << std::endl;
    std::cout << "    -user u         Use username u when connecting" << std::endl;
    std::cout << "    -pwd s          Use password s when connecting" << std::endl;
    std::cout << "    -tunnel         Connect using SSH tunnel on port 22" << std::endl;
    std::cout << std::endl;
    std::cout << "    -nologging      No logging" << std::endl;
    std::cout << "    -error          Log only errors" << std::endl;
    std::cout << "    -warn           Log errors and warnings" << std::endl;
    std::cout << "    -info           Log informational messages also" << std::endl;
    std::cout << "    -debug          Log debug messages also" << std::endl;
    std::cout << "    -trace          Enable trace logging" << std::endl;
    std::cout << std::endl;
    std::cout << "    -useospalette   Use the OS palette instead of scale palette" << std::endl;
#ifdef Q_OS_WIN
    std::cout << "    -killexplorer   Kill (or attempt to kill) explorer" << std::endl;
#endif
    std::cout << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Show dialog that we could not connect.
 */
void displayConnectionFailed()
{
    QMessageBox::warning(
        NULL,
        QObject::tr( "Connection Failed" ),
        QObject::tr( "Failed to connect to remote system.\n\nPlease check that the system is online and verify your login credentials." ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Application entry point.
 *
 * Initialize the application and show dialog.
 *
 * @param[in] argc  argument count
 * @param[in] argv  arguments passed to application
 * @returns  exit code
 */
int main( int argc, char *argv[] )
{
    LOG_CLASS_INFO( "", "main" );

    // initialize resources
    Q_INIT_RESOURCE( sessionhandlerwidgets );

    QApplication a( argc, argv );
    QStringList arguments( a.arguments() );

    // Allow Squish to attach to a running process using the built-in hook
    Squish::allowAttaching( 11233 );

    // ----------------------------------------------------
    // initialize
    // ----------------------------------------------------

    // Init Application
    appInitialize( "scaleui" );

#if defined( Q_OS_LINUX )

    // allow core dumps
    struct rlimit core_limit;

    core_limit.rlim_cur = core_limit.rlim_max = RLIM_INFINITY;

    if ( 0 == setrlimit( RLIMIT_CORE, &core_limit ) )
        LOG_INFO( "core dumps enabled", NULL );

    // allow 16mb stack
    struct rlimit stack_limit;

    if ( 0 == getrlimit( RLIMIT_STACK, &stack_limit ) )
    {
        stack_limit.rlim_cur = 16 * 1024 * 1024;

        if (( RLIM_INFINITY != stack_limit.rlim_max ) && ( stack_limit.rlim_max < stack_limit.rlim_cur ))
            stack_limit.rlim_max = stack_limit.rlim_cur;

        if ( 0 == setrlimit( RLIMIT_STACK, &stack_limit ) )
            LOG_INFO( "stack is %d %d", stack_limit.rlim_cur, stack_limit.rlim_max );
    }
#endif

    // ----------------------------------------------------
    // check for help
    // ----------------------------------------------------

    if ( arguments.contains( "-help" ) )
    {
        LOG_INFO( "showing help!", NULL );

        // display help information
        displayHelp();

        return 0;
    }

    // ----------------------------------------------------
    // unlock chilkat
    // ----------------------------------------------------

    static const QString chilkat_unlock_code( "SE9CQVJULkNCWDExMjAyMl9mZ3NuWTZXSjdnOVo=" ); // HOBART.CBX112022_fgsnY6WJ7g9Z (in base64)

    CkGlobal glob;

    if (( !glob.UnlockBundle( QByteArray::fromBase64( chilkat_unlock_code.toLatin1() ) ) ) ||
        ( 2 != glob.get_UnlockStatus() ))
    {
        LOG_WARN( "failed to unlock ck bundle", NULL );
    }

    // ----------------------------------------------------
    // palette
    // ----------------------------------------------------

    // create application palette
    if ( !arguments.contains( "-useospalette" ) )
    {
        QPalette p( QApplication::palette() );

        for ( int cr = 0; cr < QPalette::NColorRoles; ++cr )
        {
            p.setColor( QPalette::Active, (QPalette::ColorRole) cr, QColor( PALETTE_COLORS[cr].active ) );
            p.setColor( QPalette::Disabled, (QPalette::ColorRole) cr, QColor( PALETTE_COLORS[cr].disabled ) );
            p.setColor( QPalette::Inactive, (QPalette::ColorRole) cr, QColor( PALETTE_COLORS[cr].inactive ) );
        }

        QApplication::setPalette( p );
    }

    // log application palette
    logApplicationPalette();

    // ----------------------------------------------------
    // library paths
    // ----------------------------------------------------
    // ----------------------------------------------------

#ifdef Q_OS_WIN
    QDir rootFolder( filesystem::folder( folderType::RootFolder ) );

    // set working folder
    QDir::setCurrent( rootFolder.absolutePath() );

    // append qt plugin library path
    QDir plugins( rootFolder );
    plugins.cd( "qt4_plugins" );

    if ( plugins.exists() )
        qApp->addLibraryPath( plugins.absolutePath() );

    plugins.cdUp();
    plugins.cd( "qt_plugins" );

    if ( plugins.exists() )
        qApp->addLibraryPath( plugins.absolutePath() );
#endif

    QStringList libraryPaths( qApp->libraryPaths() );

    foreach ( const QString& p, libraryPaths )
        LOG_INFO( "library path '%s'", qPrintable( p ) );

#ifdef Q_OS_WIN
    static const QString adobeFlashPluginFolder( "C:\\Windows\\SysWOW64\\Macromed\\Flash" );

    // check for adobe flash plugin
    if ( QDir( adobeFlashPluginFolder ).exists() )
        qputenv( "QTWEBKIT_PLUGIN_PATH", adobeFlashPluginFolder.toLatin1() );
#endif

    // ----------------------------------------------------
    // check for fullscreen
    // ----------------------------------------------------

    bool showFullscreen( false );
    QSize size( 1024, 600 );

    // check desktop dimensions
    QDesktopWidget *desktop( a.desktop() );

    if ( desktop )
    {
        const QRect desktopRect( desktop->screenGeometry( 0 ) );

        LOG_INFO( "primary screen size is (%d, %d)", desktopRect.width(), desktopRect.height() );

        // check for larger display(s)
        if (( 1920 <= desktopRect.width() ) && ( 1080 <= desktopRect.height() ))
            size = QSize( 1920, 1080 );
        else if (( 1366 <= desktopRect.width() ) && ( 768 <= desktopRect.height() ))
            size = QSize( 1366, 768 );

        if (( desktopRect.width() <= size.width() ) || ( desktopRect.height() <= size.height() ))
            showFullscreen = true;
    }

    // ----------------------------------------------------
    // create socket
    // ----------------------------------------------------

    SessionType sessionType = PrimarySessionType;

    QString address = "127.0.0.1";
    quint16 port = 6000;

    QString user = "hobart";
    QString pwd;

    bool isTunnelRunning( false );
    CkSshTunnel tunnel;

    int forceWidth( 1024 );
    int forceHeight( 600 );

    // check for address and port
    for ( int i = 0; i < arguments.size(); ++i )
    {
        if (( "-address" == arguments[i] ) || ( "-ip" == arguments[i] ))
            if ( (i+1) < arguments.size() )
            {
                address = arguments.at( i+1 );

                // if we are not connecting to the local scale service then assume we are connecting
                // to a remote scale as a secondary session (unless they override below of course)
                if (( "127.0.0.1" != address ) && ( "localhost" != address ))
                    sessionType = SecondarySessionType;
            }

        if ( "-port" == arguments[i] )
            if ( (i+1) < arguments.size() )
                port = arguments.at( i+1 ).toUShort();

        if (( "-user" == arguments[i] ) || ( "-u" == arguments[i] ))
            if ( (i+1) < arguments.size() )
                user = arguments.at( i+1 );

        if (( "-pwd" == arguments[i] ) || ( "-p" == arguments[i] ))
            if ( (i+1) < arguments.size() )
                pwd = arguments.at( i+1 );

        if ( "-ww" == arguments[i] )
            if ( (i+1) < arguments.size() )
            {
                forceWidth = arguments.at( i+1 ).toInt();

                if ( forceWidth < 1024 )
                    forceWidth = 1024;
            }

        if ( "-wh" == arguments[i] )
            if ( (i+1) < arguments.size() )
            {
                forceHeight = arguments.at( i+1 ).toInt();

                if ( forceHeight < 600 )
                    forceHeight = 600;
            }
    }

    // check for session type
    if (( arguments.contains( "-primary" ) ) || ( arguments.contains( "-1" ) ))
        sessionType = PrimarySessionType;
    else if (( arguments.contains( "-secondary" ) ) || ( arguments.contains( "-2" ) ))
        sessionType = SecondarySessionType;

    // check for demo flag
    if ( arguments.contains( "-demo" ) )
    {
        // force secondary session type
        sessionType = SecondarySessionType;

        isDemoMode = true;
    }

    // check for fullscreen mode
    if ( arguments.contains( "-full" ) )
        showFullscreen = true;
    else if ( arguments.contains( "-windowed" ) )
    {
        showFullscreen = false;

        size.setWidth( forceWidth );
        size.setHeight( forceHeight );
    }
    else if ( arguments.contains( "-developer10" ) )
    {
        showFullscreen = false;

        size.setWidth( 1024 );
        size.setHeight( 600 );
    }
    else if ( arguments.contains( "-developer14" ) )
    {
        showFullscreen = false;

        size.setWidth( 1366 );
        size.setHeight( 768 );
    }
    // check for SSH tunnel flag
    if ( arguments.contains( "-tunnel" ) )
    {
        // no username/password supplied, use canned one
        if (( !user.length() ) || ( !pwd.length() ))
        {
#ifdef QT_DEBUG
            static const QString tunnelUser( "cm9vdA==" ); // root (in base64)
            static const QString tunnelPwd( "aG8xYmFydDI=" ); // ho1bart2 (in base64)
#else
            static const QString tunnelUser( "aG9iYXJ0" ); // hobart (in base64)
            static const QString tunnelPwd( "YWRtaW44NzU4" ); // admin8758 (in base64)
#endif

            pwd = QString( QByteArray::fromBase64( tunnelPwd.toLatin1() ) );
            user = QString( QByteArray::fromBase64( tunnelUser.toLatin1() ) );
        }

        // connection info for scale service
        tunnel.put_DestHostname( "localhost" );
        tunnel.put_DestPort( port );

        LOG_DEBUG( "connecting...", NULL );

        // connect
        if ( !tunnel.Connect( address.toLatin1(), 22 ) )
            LOG_WARN( "error connecting to ssh client: '%s'", tunnel.lastErrorText() );

        // authenticate
        else if ( !tunnel.AuthenticatePw( user.toLatin1(), pwd.toLatin1() ) )
            LOG_WARN( "error authenticating to ssh client: '%s'", tunnel.lastErrorText() );

        // open tunnel!
        else if ( !tunnel.BeginAccepting( 6000 ) )
            LOG_WARN( "error opening tunnel: '%s'", tunnel.lastErrorText() );
        else
        {
            isTunnelRunning = true;

            address = "127.0.0.1";
            port = 6000;
        }

        // are we ready?
        if ( !isTunnelRunning )
        {
            displayConnectionFailed();
            return -1;
        }
    }

    // connect!
    HobSocket *socket = connectToScale( address, port );

    if ( !socket )
    {
        LOG_ERROR( "no connection... aborting!", NULL );

        // for secondary sessions lets pop a nice message to help them out
        if ( SecondarySessionType == sessionType )
            displayConnectionFailed();

        return -1;
    }

    // ----------------------------------------------------
    // explorer
    // ----------------------------------------------------

#ifdef Q_OS_WIN
#ifndef QT_DEBUG

    // kill explorer process
    //
    // only do this when:
    // 1) windows machine
    // 2) release mode
    // 3) we are a primary session
    if (( arguments.contains( "-killexplorer" ) ) && ( PrimarySessionType == sessionType ))
    {
        LOG_INFO( "Need to kill explorer", NULL );

        int rval = -1;
        int num_tries = 0;

        // HW-1: workaround to fix killing of explorer
        //
        // Python startup sequence seems to be running in a different session... when it attempts
        // to kill explorer it cannot find the executable. Yet when I run it manually from a command
        // prompt it finds it just fine. I am moving the task killer to this process as the UI is
        // running in the same session.
        while( rval != 0 && num_tries < 5)
        {
            rval = system( "taskkill /IM explorer.exe /F" );
            LOG_INFO( "rval from killing explorer=%d", rval );

            //-----------------------------------------------------------//
            // rval will be 0 if the system call succeeds                //
            // rval will be 128 if the process to kill doesn't exist yet //
            //-----------------------------------------------------------//
            if( rval != 0)
                // Sleep for 500 milliseconds and try again
                QThread::msleep( 500 );

            ++ num_tries;
        }
    }
#endif
#endif

    // ----------------------------------------------------
    // show user interface
    // ----------------------------------------------------

    SessionMainWindow::RegisterSessionResult regrc;
    int rc = -2;

    // set application style
    QApplication::setStyle( new TouchScreenStyle );

    if ( PrimarySessionType == sessionType )
        LOG_INFO( "about to launch ui as primary session type...", NULL );
    else if ( SecondarySessionType == sessionType )
        LOG_INFO( "about to launch ui as secondary session type...", NULL );

    // open the session main window
    SessionMainWindow *w = new SessionMainWindow( socket, sessionType, size, user, pwd );

    // setup window
    w->setWindowTitle( "Hobart Scale UI" );
    w->setFixedSize( size );

    if ( showFullscreen )
    {
        w->setWindowFlags( Qt::FramelessWindowHint );
        w->showFullScreen();
    }
    else
    {
        // for demo mode show no frame
        if ( isDemoMode )
            w->setWindowFlags( Qt::FramelessWindowHint );
        else
            w->setWindowFlags( Qt::Window );

        w->show();
    }

    // install hooks
    sessionHooks::instance()->setSession( w->session() );

    // session registration
    if ( SessionMainWindow::RegisterSessionResultOkay != (regrc = w->registerSession()) )
    {
        LOG_ERROR( "session failed to register %d!", regrc );

        // for secondary sessions lets pop a nice message to help them out
        if ( SecondarySessionType == sessionType )
            if (( SessionMainWindow::RegisterSessionResultNoFilesystem == regrc ) ||
                ( SessionMainWindow::RegisterSessionResultTimeout == regrc ))
            {
                displayConnectionFailed();
            }
    }
    else
    {
        // Note:
        //
        // In release mode we have no window manager (openbox) on the HTi system. When the
        // application starts up there is no focus. Here we force the system to focus our
        // application.
        //
        QApplication::setActiveWindow( w );

        if ( w != QApplication::activeWindow() )
            w->setWindowState( Qt::WindowActive );

        if ( w != QApplication::activeWindow() )
        {
            w->setWindowState( Qt::WindowMinimized );
            w->setWindowState( Qt::WindowActive );
        }

        LOG_DEBUG( "entering application loop...", NULL );
        rc = a.exec();

        LOG_INFO( "application complete %d!", rc );
    }

    // cleanup
    w->deleteLater();
    socket->deleteLater();

    LOG_DEBUG( "closing anything still open..", NULL );
    a.closeAllWindows();

    LOG_DEBUG( "cleaning up...", NULL );
    a.sendPostedEvents();
    a.flush();

    a.processEvents();

    if ( isTunnelRunning )
    {
        LOG_DEBUG( "stopping tunnel...", NULL );

        if ( !tunnel.StopAccepting( true ) )
            LOG_WARN( "error stopping tunnel listener: '%s'", tunnel.lastErrorText() );

        else if ( !tunnel.DisconnectAllClients( true ) )
            LOG_WARN( "error stopping tunnel: '%s'", tunnel.lastErrorText() );
    }

    ///////////////////////////////////////////////////////
    // finalize logging

    appFinalize();

    return rc;
}
