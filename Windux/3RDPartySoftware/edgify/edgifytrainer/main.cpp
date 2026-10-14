/**
 * @file main.cpp
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#include "devicediscovery.h"
#include "inputwidgethook.h"
#include "mainwindow.h"
#include "touchscreenstyle.h"

#include <HobartCustomerHook.h>

#include <string>

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QStringList>

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
    QStringList files;
    files.append( "app.config" );
    files.append( configFile );

    // load settings from each config file
    foreach ( const QString& conf, files )
    {
        QFile f( conf );

        // load settings
        if ( !f.open( QIODevice::ReadOnly | QIODevice::Text ) )
            qWarning() << "error loading config: " << conf;
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
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString findScaleDatabase()
{
    static const QString letters( "abcdefghijklmnopqrstuvwxyz" );
    static const QString dbnametemplate( "dbft%1.db" );

#if defined(Q_OS_LINUX)
    static const QString path( "/usr/local/hobart/db/scale/dbfiles/" );
#elif defined(Q_OS_WIN32)
    static const QString path;
#endif

    foreach ( const QChar& c, letters )
    {
        const QString f( path + dbnametemplate.arg( c ) );

        if ( QFile::exists( f ) )
            return f;
    }

    return QString();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int main( int argc, char *argv[] )
{
    QApplication a( argc, argv );
    a.setStyle( new TouchScreenStyle() );

#if !defined( QT_DEBUG )
    a.setOverrideCursor( QCursor( Qt::BlankCursor ) );
#endif

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
    params["allowkeyboard"] = "0";
    params["allowclose"] = "1";
    params["allowminimize"] = "0";
    params["server"] = "127.0.0.1";
    params["port"] = "6000";
    params["appname"] = "Edgify Trainer";
    params["logo"] = "logo.png";
    params["logosvg"] = "";
    params["title"] = "";
    params["background"] = "#000000";
    params["bgimage"] = "";
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
    const bool allowKeyboard( "1" == params["allowkeyboard"] );

    InputWidgetHook virtualInputDevice;

    // startup device discovery
    DeviceDiscovery::instance();

    // init hook
    hobart_cust_hook_init();

    hobart_cust_hook_before_open_database( findScaleDatabase().toLatin1() );
    hobart_cust_hook_open_database();

    // launch!
    static const QString windowTitle( params["appname"] );

    MainWindow w( params );
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

    if ( !allowKeyboard )
        virtualInputDevice.setDisabled( true );
    else
    {
        QApplication::connect( &w, SIGNAL(hideInputDevice()), &virtualInputDevice, SLOT(dismiss()) );
        QApplication::connect( &w, SIGNAL(showInputDevice(QWidget*,bool)), &virtualInputDevice, SLOT(invoke(QWidget*,bool)) );
    }

    const int rc( a.exec() );

    // finalize hook
    hobart_cust_hook_finalize();

    return rc;
}
