/**
 * @file main.cpp
 * Hobart Hands Free Pager Device (HFPD) Daemon Process.
 *
 * @copyright Copyright (c) 2018 Hobart Corporation
 * @author Randy Blankley
 */

#include "blescanneragent.h"
#include "commserver.h"
#include "commclient.h"
#include "hciscanneragent.h"
#include "hobarttagmanager.h"
#include "otaupgrademanager.h"
#include "scanneragentwatchdog.h"

#include <QCoreApplication>
#include <QDebug>

#include <signal.h>
#include <unistd.h>

bool _rssiLogging( false );

///////////////////////////////////////////////////////////////////////////////////////////////////
void sig_handler( int /*unused*/ )
{
    qWarning() << "exiting...";
    QCoreApplication::instance()->quit();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int main( int argc, char *argv[] )
{
    int result( -1 );

    // register ctrl-c interrupt handler
    signal( SIGINT, sig_handler );

    // register term signal handler
    signal( SIGTERM, sig_handler );

    // ---- //

    QCoreApplication a( argc, argv );

    const QStringList args( a.arguments() );

    if ( args.contains( "-rssilogging" ) )
        _rssiLogging = true;

    // create ble agent
    scannerAgent *agent( new hciScannerAgent() );
    agent->startScanning();

    if ( !agent->isScanningActive() )
        qWarning() << "scanner agent is not active: " << qPrintable( agent->errorString() );
    else
    {
        scannerAgentWatchdog watchdog( agent );
        commServer server;

        qDebug() << "scanner agent active!";

        // start server
        QHostAddress address( QHostAddress::LocalHost );
        quint16 port( 5100 );

        if ( args.contains( "-any" ) )
            address = QHostAddress( QHostAddress::Any );
        else if ( args.contains( "-any4" ) )
            address = QHostAddress( QHostAddress::AnyIPv4 );
        else if ( args.contains( "-any6" ) )
            address = QHostAddress( QHostAddress::AnyIPv6 );

        if ( server.listen( address, port ) )
            qDebug() << "listening for connections...";
        else
        {
            qWarning() << "comm server failed to start";
        }

        // start event loop
        result = a.exec();

        agent->stopScanning();
    }

    // delete agent
    delete agent;

    // delete managers
    hobartTagManager::destroy();
    otaUpgradeManager::destroy();

    return result;
}
