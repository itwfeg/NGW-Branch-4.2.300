//#define CLARE_TEST_ON_LAPTOP
/**
 * @file main.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "gcd.h"
#include "gcdcommserver.h"

#include <appinit.h>
#include <filesystem.h>
#include <logmanager.h>
#include <platform.h>

#include <QDesktopWidget>
#include <QDir>
#include <QString>
#include <QStringList>

// Allow Squish to attach to a running process using the built-in hook
#include "../../shared/squish/qtbuiltinhook.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Application entry point.
/**
 * Initialize the application and setup a GCD frontend and backend.
 *
 * @param[in] argc  argument count
 * @param[in] argv  arguments passed to application
 * @return  exit code
 */
int main( int argc, char *argv[] )
{
    LOG_CLASS_INFO( "", "main" );

    QApplication a( argc, argv );
    QStringList arguments( a.arguments() );

    // Allow Squish to attach to a running process using the built-in hook
    Squish::allowAttaching( 11333 );

    // ----------------------------------------------------
    // init
    // ----------------------------------------------------

    // Init Application
    appInitialize( "gcd" );

    // Set Application default background color to black
    QPalette p( a.palette() );
    p.setColor( QPalette::Window, Qt::black );
    a.setPalette( p );

    // ----------------------------------------------------
    // instantiate GCD interface
    // ----------------------------------------------------

    gcd *w( new gcd() );

    w->setWindowTitle( "Hobart GCD" );

#ifdef QT_DEBUG

    // allow developers to run windowed instead of fullscreen
    if ( arguments.contains( "-windowed" ) )
    {
        w->setWindowFlags( Qt::Window );
        w->show();
    }
    else
    {
        w->setWindowFlags( Qt::FramelessWindowHint );
        w->showFullScreen();
    }

#else

    // show fullscreen
    w->setWindowFlags( Qt::FramelessWindowHint );
    w->showFullScreen();

#endif

    QDesktopWidget *desktop = a.desktop();

    // setup correct path for layouts
    QRect screenGeometry( desktop->screenGeometry( desktop->screenNumber( w ) ) );
    LOG_INFO( "Our screen size is width: %d and height: %d", screenGeometry.width(), screenGeometry.height());

    if ( !screenGeometry.isValid() )
        LOG_INFO( "The screenGeometry is invalid!!!", NULL );
    else
    {
        QString path( "" );                          //      Default for 7" GCD

        //     Note:   As of May 2015, the 14" GCD screen does not exist.
        //                     The 10.1" screen will be available June 2015.
        if (( 1366 <= screenGeometry.width() ) && ( 768 <= screenGeometry.height() ))
        {
            path = "1366_768";                //      14" GCD
            w->resize( 1366, 768 );           //      Resize the UI to make full use of the larger display.
        }
        else if (( 1024 <= screenGeometry.width() ) && ( 600 <= screenGeometry.height() ))
        {
            path = "1024_600";                //      10.1" GCD
            w->resize( 1024, 600 );           //      Resize the UI to make full use of the larger display.
        }
        LOG_INFO( "path is [%s]", qPrintable(path) );

        QString folder( filesystem::folder( folderType::Layouts ) );
        LOG_INFO( "Current Layouts folder is now %s", qPrintable(folder) );

        folder += QDir::separator();
        folder += path;

        if ( !path.isEmpty() )
        {
             //  Check that the path exists. If not, reset back to the default 7" screens.
            QDir checkDir( folder);
            if ( !checkDir.exists() )
                folder = filesystem::folder( folderType::Layouts );    //  Set back to the main layouts path
        }

        filesystem::setFolder( folder, folderType::Layouts );
        LOG_INFO( "****  New Layouts folder is now %s", qPrintable(filesystem::folder( folderType::Layouts )) );
    }

    // for dual display we need to be on the second monitor
    if ( 1 < desktop->screenCount() )
    {
        int screenCount = desktop->screenCount();
        int screenNumber = desktop->screenNumber( w );

        int offset = INT_MIN;
        int properScreen = 0;

        LOG_INFO( "detected multiple displays (%d):", screenCount );

        for ( int i = 0; i < screenCount; ++i )
        {
            QRect geometry( desktop->screenGeometry( i ) );

            LOG_INFO( "[screen %d left %d top %d width %d height %d] %s",
                i,
                geometry.left(),
                geometry.top(),
                geometry.width(),
                geometry.height(),
                (i == screenNumber) ? "***" : "" );

            // try and detect right-most screen
            if ( offset <= geometry.left() )
            {
                offset = geometry.left();
                properScreen = i;
            }
        }

        if ( properScreen == screenNumber )
            LOG_INFO( "we are on proper screen", NULL );
        else if ( !desktop->isVirtualDesktop() )
            LOG_WARN( "cannot move window, desktop is not virtual! please restart on correct screen!!", NULL );
        else
        {
            LOG_INFO( "moving to %d...", offset );
            w->move( offset, 0 );
        }
    }

    w->setOutOfService( true );

    // ----------------------------------------------------
    // instantiate GCD backend
    // ----------------------------------------------------

    gcdCommServer *server( new gcdCommServer() );

    QObject::connect(
        server, SIGNAL( updateUserInterface(gcdUpdateUserInterfaceType::type,QString) ),
        w, SLOT( onUpdateUserInterface(gcdUpdateUserInterfaceType::type,QString) ) );

    QObject::connect(
        server, SIGNAL( controlMessage(gcdControlMessageType::type,QString ) ),
        w, SLOT( onControlMessage(gcdControlMessageType::type,QString) ) );

    QObject::connect(
        w, SIGNAL( updateStatus(uint,uint,QString,QString) ),
        server, SLOT( onUpdateStatus(uint,uint,QString,QString) ) );

    QObject::connect(
        w, SIGNAL( playlistError(QString,QString) ),
        server, SLOT( onPlaylistError(QString,QString) ) );

    // process Qt Events
    int rc = a.exec();

    // cleanup
    delete server;
    delete w;

    // ----------------------------------------------------
    // finalize
    // ----------------------------------------------------

    appFinalize();

    return rc;
}
