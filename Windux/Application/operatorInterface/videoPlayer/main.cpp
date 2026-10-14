/**
 * @file main.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "videoplayerdialog.h"

#include <appinit.h>
#include <filesystem.h>
#include <logmanager.h>
#include <sighandler.h>

#include <QApplication>
#include <QDesktopWidget>
#include <QDir>
#include <QFile>
#include <QString>
#include <QStringList>

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Application entry point.
 *
 * Initialize the application and show video player dialog.
 *
 * @param[in] argc  argument count
 * @param[in] argv  arguments passed to application
 * @returns  exit code
 */
int main( int argc, char *argv[] )
{
    LOG_CLASS_INFO( "", "main" );

    QApplication a( argc, argv );
    QStringList arguments( a.arguments() );

    // ----------------------------------------------------
    // init
    // ----------------------------------------------------

    // init application
    appInitialize( "videoPlayer" );

    // ----------------------------------------------------
    // check for video filename passed in
    // ----------------------------------------------------

    QString filename;

    for ( int i = 1; i < argc; ++i )
        if ( '-' != argv[i][0] )
        {
            QString temp( filesystem::absoluteFilePath( argv[i], folderType::Videos ) );

            // check videos path
            if ( QFile::exists( temp ) )
            {
                filename = temp;
                break;
            }

            // check working folder
            else if ( QFile::exists( argv[i] ) )
            {
                filename = temp;
                break;
            }
        }

    if ( !filename.length() )
    {
        LOG_ERROR( "no file to play!", NULL );
        return -1;
    }

    // ----------------------------------------------------
    // compute geometry for proper monitor
    // ----------------------------------------------------

    QPoint dialogPosition;
    QSize dialogSize( 0, 0 );

    int monitor( 0 );

    // use primary (left most) monitor
    if ( a.arguments().contains( "-1" ) )
        monitor = 1;
    // use secondary (right most) monitor
    else if ( a.arguments().contains( "-2" ) )
        monitor = 2;

    // relocate window to specified monitor
    if ( 0 < monitor )
    {
        QDesktopWidget *desktop = a.desktop();

        if ( !desktop->isVirtualDesktop() )
            LOG_WARN( "cannot move window, desktop is not virtual! please restart on correct screen!!", NULL );
        else if ( 1 < desktop->screenCount() )
        {
            int screenCount = desktop->screenCount();
            int offset = ((2 == monitor) ? INT_MIN : INT_MAX);

            LOG_INFO( "detected multiple displays (%d):", screenCount );

            for ( int i = 0; i < screenCount; ++i )
            {
                QRect geometry( desktop->screenGeometry( i ) );

                LOG_INFO( "[screen %d left %d top %d width %d height %d]",
                    i,
                    geometry.left(),
                    geometry.top(),
                    geometry.width(),
                    geometry.height() );

                // use the left-most screen (1) or right-most screen (2)
                if ((( 1 == monitor ) && ( geometry.left() <= offset )) ||
                    (( 2 == monitor ) && ( offset <= geometry.left() )))
                {
                    offset = geometry.left();

                    dialogPosition = geometry.topLeft();
                    dialogSize = geometry.size();
                }
            }

            // when windowed we do not want a fullscreen size
            if ( a.arguments().contains( "-windowed" ) )
            {
                LOG_INFO( "using windowed geometry", NULL );
                dialogSize = QSize();
            }

            LOG_INFO( "detected geometry %dx%d+%d+%d",
                dialogSize.width(),
                dialogSize.height(),
                dialogPosition.x(),
                dialogPosition.y() );
        }
    }

    // ----------------------------------------------------
    // init player dialog
    // ----------------------------------------------------

    videoPlayerDialog *w( new videoPlayerDialog( dialogPosition ) );

    // install signal handlers
    QObject::connect( sigHandler::instance(), SIGNAL( hup() ), w, SLOT( reject() ) );
    QObject::connect( sigHandler::instance(), SIGNAL( term() ), w, SLOT( reject() ) );

    // show player
    if ( a.arguments().contains( "-windowed" ) )
        w->show();
    else
    {
        bool disableUi( false );

        // disable user interface
        if ( a.arguments().contains( "-disableui" ) )
        {
            LOG_INFO( "player ui disabled!", NULL );

            w->disableUserInterface();
            w->setAttribute( Qt::WA_ShowWithoutActivating );
            w->setWindowFlags( w->windowFlags() | Qt::FramelessWindowHint );

            disableUi = true;
        }

        // make sure we are full screen!
        if ( dialogSize.isNull() )
            dialogSize = a.desktop()->screenGeometry( w ).size();

        w->resize( dialogSize );

        // when disabled do not show the dialog! if there is video it will show automatically,
        // otherwise it will not (i.e. for audio)...
        if ( !disableUi )
            w->show();
    }

    LOG_INFO( "player geometry %dx%d+%d+%d",
        w->geometry().width(),
        w->geometry().height(),
        w->geometry().left(),
        w->geometry().top() );

    if ( !w->play( filename ) )
    {
        LOG_ERROR( "file '%s' failed to play!", qPrintable( filename ) );
        return -1;
    }
    
    int rc = a.exec();

    LOG_DEBUG( "delete dialog...", NULL );
    delete w;

    // ----------------------------------------------------
    // finalize
    // ----------------------------------------------------

    LOG_DEBUG( "finalize...", NULL );
    appFinalize();

    LOG_INFO( "application exit %d", rc );
    return rc;
}
