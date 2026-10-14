/**
 * @file bluezviewer.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "inputwidgethook.h"
#include "mainwindow.h"
#include "touchscreenstyle.h"

#include <string>

#include <QApplication>
#include <QDebug>
#include <QFile>

#ifdef Q_OS_WIN
#include <direct.h>
#else
#include <unistd.h>
#endif

#ifdef Q_OS_LINUX
#include <X11/Xlib.h>
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
void loadParameters( const QString &configFile, QMap<QString, QString> &params )
{
    QFile f( configFile );

    // load settings
    if ( !f.open( QIODevice::ReadOnly | QIODevice::Text ) )
        qWarning() << "error loading config: " << configFile;
    else
    {
        QTextStream is( &f );

        while ( !is.atEnd() )
        {
            QString line( is.readLine() );

            if (( 2 <= line.length() ) && ( ' ' != line[0] ) && ( '#' != line[0] ))
            {
                int index( line.indexOf( "=" ) );

                if ( 0 < index )
                    params[line.left(index)] = line.mid( index + 1 );
            }
        }

        f.close();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int main( int argc, char *argv[] )
{
    QApplication a( argc, argv );
    a.setStyle( new touchScreenStyle() );

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

    // set default params
    QMap<QString, QString> params;
    params["url"] = "http://www.hobartcorp.com";
    params["allowrefresh"] = "0";
    params["allowcookies"] = "0";
    params["allowkeyboard"] = "0";
    params["allowclose"] = "1";
    params["allowminimize"] = "0";
    params["sendstorenumber"] = "0";
    params["senddeptnumber"] = "0";
    params["server"] = "127.0.0.1";
    params["port"] = "6000";
    params["appname"] = "Walmart FPP";
    params["style"] = "0";
    params["logo"] = "logo.png";
    params["title"] = "";
    params["background"] = "#000000";
    params["fontcolor"] = "#FFFFFF";
    params["fontpointsize"] = "18";
    params["fontfamily"] = "Arial";

    // load params
    QString configFile( argv[0] + QString( ".config" ) );
    configFile.replace( ".exe.config", ".config" );

    loadParameters( configFile, params );

    // log out params
    for ( QMap<QString, QString>::iterator p( params.begin() ); p != params.end(); ++p )
        qDebug() << p.key() << ": " << p.value();

    // virtual keyboard
    inputWidgetHook virtualInputDevice;

    // launch!
    static const QString windowTitle( params["appname"] );

    mainWindow w( params );
    w.setWindowTitle( windowTitle );
	
#ifdef Q_OS_LINUX
    Display *disp( XOpenDisplay( NULL ) );

    if ( disp )
    {
        XStoreName( disp, w.winId(), qPrintable( windowTitle ) );
        XCloseDisplay( disp );
    }
#endif

    w.showFullScreen();

    if ( "0" == params["allowkeyboard"] )
        virtualInputDevice.setDisabled( true );
    else
    {
        QApplication::connect( &w, SIGNAL(hideInputDevice()), &virtualInputDevice, SLOT(dismiss()) );
        QApplication::connect( &w, SIGNAL(showInputDevice(QWidget*,bool)), &virtualInputDevice, SLOT(invoke(QWidget*,bool)) );
    }

    return a.exec();
}
