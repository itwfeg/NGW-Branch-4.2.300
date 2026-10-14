/**
 * @file main.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "mainwindow.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMap>
#include <QTextStream>
#include <QString>

#include <QtDebug>

#include <appinit.h>
#include <logmanager.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
QMap<QString, QString> loadParameters( const QString& configFile )
{
    LOG_CLASS_INFO( "main", "loadParameters");

    QMap<QString, QString> params;

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
                LOG_DEBUG(" loadParameters(): line=%s", qPrintable( line ) );

                QStringList fields( line.split( "=" ) );

                if ( 2 == fields.size() )
                    params[fields[0].trimmed()] = fields[1].trimmed();
                else if ( 2 < fields.size() )
                {
                    //------------------------------------------------------------------------------------
                    // SOF-4530: Need to be able to support a parameter for a URL that contains
                    //           query string parameters in it - i.e. more than one '=' sign
                    //           (e.g. C-url=http://10.3.128.84/OnlineOrdering/kiosk/queue.html?DeptNum=0)
                    //------------------------------------------------------------------------------------

                    // Just grab everything to the right of the first '=' sign
                    int offset = line.indexOf( QChar( '=' ) );
                    QString tmpValue = line.right( line.length() - offset - 1 );
                    params[fields[0].trimmed()] = tmpValue;

                    LOG_DEBUG(" loadParameters() >2 fields: line=%s", qPrintable( line ) );
                }
            }
        }

        f.close();
    }

    return params;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int main( int argc, char *argv[] )
{
    LOG_CLASS_INFO( "main", "main");

    QApplication a( argc, argv );

    // retrieve application arguments
    QStringList args( a.arguments() );

    // set working directory to application directory
    QFileInfo appInfo( args[0] );

    QDir::setCurrent( appInfo.absolutePath() );

    // Init Application & logging
    appInitialize( "nowServing" );

    // set default config (if missing)
    QMap<QString, QString> defaultParams;
    defaultParams["server"] = "127.0.0.1";
    defaultParams["port"] = "5076";
    defaultParams["updateport"] = "5081";
    defaultParams["updatetype"] = "tcp";
    defaultParams["queue"] = "default";
    defaultParams["dept"] = "WELCOME";
    defaultParams["marquee"] = "Please wait for your number to be called.";
    defaultParams["serving"] = "Currently Serving";
    defaultParams["showqueuesize"] = "1";
    defaultParams["queuesize"] = "Number Waiting";
    defaultParams["errormsg"] = "Out of Service";
    defaultParams["showerrordetails"] = "1";
    defaultParams["playlist"] = "playlist.txt";

    defaultParams["layout"] = "1,N,P,W";

    defaultParams["A-playlist"] = "playlist-a.txt";
    defaultParams["B-playlist"] = "playlist-b.txt";
    defaultParams["C-playlist"] = "playlist-c.txt";

    defaultParams["A-url"] = "http://www.google.com";
    defaultParams["B-url"] = "http://www.yahoo.com";
    defaultParams["C-url"] = "http://www.amazon.com";

    // load params
    QString configFile( argv[0] + QString( ".config" ) );
    configFile.replace( ".exe.config", ".config" );

    QMap<QString, QString> params( loadParameters( configFile ) );

    // consolidate parameters
    LOG_DEBUG( "BEFORE assign defaultParams to params:", NULL );
    for ( QMap<QString, QString>::iterator p( defaultParams.begin() ); p != defaultParams.end(); ++p )
        if ( !params.contains( p.key() ) )
        {
            params[ p.key() ] = p.value();

            LOG_DEBUG(" Key=%s; Value=%s", qPrintable( p.key() ), qPrintable( p.value() ) );
        }

    // log out settings
    LOG_DEBUG( "AFTER assign defaultParams:", NULL );
    for ( QMap<QString, QString>::iterator p( params.begin() ); p != params.end(); ++p )
    {
        qDebug() << p.key() << ": " << p.value();

        LOG_DEBUG(" Key=%s; Value=%s", qPrintable( p.key() ), qPrintable( p.value() ) );
    }

    // show main windows
    mainWindow::modeType mode( mainWindow::printTicket );

    if ( ( args.contains( "-printTicket" ) ) || ( args.contains( "-printer" ) ) )
        mode = mainWindow::printTicket;
    else if ( ( args.contains( "-signage" ) ) || ( args.contains( "-tv" ) ) )
        mode = mainWindow::signage;
    else if ( ( args.contains( "-signage" ) ) || ( args.contains( "-oo" ) ) )
        mode = mainWindow::onlineOrdering;

    mainWindow w( mode, params );

    // check desktop dimensions
    QDesktopWidget *desktop( a.desktop() );

    if ( desktop )
    {
        const QRect desktopRect( desktop->screenGeometry( 0 ) );

        w.setFixedSize( desktopRect.size() );
    }

    w.setWindowTitle( "Now Serving" );
    w.setWindowFlags( Qt::FramelessWindowHint );
    w.showFullScreen();

    return a.exec();
}
