/**
 * @file main.cpp
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */

#include "commclient.h"
#include "inputwidgethook.h"
#include "mainwindow.h"
#include "touchscreenstyle.h"

#ifdef Q_OS_WIN
#include <direct.h>
#else
#include <signal.h>
#include <unistd.h>
#endif

#include <string>

#include <QApplication>
#include <QDebug>

bool _expert( false );

///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef Q_OS_WIN
void sig_handler( int /*unused*/ )
{
    qWarning() << "exiting...";
    QCoreApplication::instance()->quit();
}
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
int main( int argc, char *argv[] )
{
    int result( -1 );

#ifndef Q_OS_WIN
    // register ctrl-c interrupt handler
    signal( SIGINT, sig_handler );

    // register term signal handler
    signal( SIGTERM, sig_handler );
#endif

    // ---- //

    // set current working dir to application working dir
    std::string pwd( argv[0] );

    size_t len( 0 );
    size_t idx_bs( pwd.find_last_of( '\\' ) );
    size_t idx_fs( pwd.find_last_of( '/' ) );

    if ( std::string::npos != idx_bs )
        if ( len < idx_bs )
            len = idx_bs;

    if ( std::string::npos != idx_fs )
        if ( len < idx_fs )
            len = idx_fs;

    if ( len )
    {
        std::string new_pwd( pwd.substr( 0, len ) );
        chdir( new_pwd.c_str() );
    }

    // ---- //

    bool fullscreen( true );

    QApplication a( argc, argv );
    a.setStyle( new touchScreenStyle() );

    // virtual keyboard
    inputWidgetHook virtualInputDevice;

    // connect to remote
    commClient *client( commClient::instance() );

    QString hostName( "127.0.0.1" );
    quint16 port( 5100 );

    foreach ( const QString& arg, a.arguments() )
    {
        if ( 3 == arg.count( '.' ) )
            hostName = arg;

        if (( "-windowed" == arg ) || ( "-w" == arg ))
            fullscreen = false;

        if ( "-expert" == arg )
            _expert = true;
    }

    client->connectToHost( hostName, port );

    if ( !client->waitForConnected( 30000 ) )
        qWarning() << "failed to connect to remote" << hostName << port << client->error() << client->errorString();
    else
    {
        QObject::connect( client, SIGNAL(disconnected()), &a, SLOT(quit()) );

        mainWindow w;

        if ( fullscreen )
            w.showFullScreen();
        else
        {
            QSize size( 1024, 768 );
            w.setFixedSize( size );

            w.show();
        }

        // start event loop
        result = a.exec();
    }

    // delete client
    commClient::destroy();

    return result;
}
