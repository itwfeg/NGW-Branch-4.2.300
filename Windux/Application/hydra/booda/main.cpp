/**
 * @file main.cpp
 * @brief Application entry point.
 *
 * @copyright Copyright (c) 2021, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#include "oodbadapter.h"

#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusError>
#include <QDebug>

static const QString DBUS_SERVICE( "com.hobartcorp.BOODA" );

///////////////////////////////////////////////////////////////////////////////////////////////////
int main( int argc, char *argv[] )
{
    QCoreApplication a( argc, argv );

    QString method( "http" );
    QString server( "127.0.0.1" );
    quint16 port( 80 );

    // iterate arguments
    for ( int arg( 1 ); arg < argc; ++arg )
    {
        const QString param( argv[arg] );

        // method
        if (( "-method" == param ) && ( ++arg < argc ))
        {
            method = argv[arg];
        }

        // server
        if (( "-server" == param ) && ( ++arg < argc ))
        {
            server = argv[arg];
        }

        // port
        else if (( "-port" == param ) && ( ++arg < argc ))
        {
            const QString portStr( argv[arg] );

            port = portStr.toUShort();
        }
    }

    QString address( method + "://" + server );

    if ( 80 != port )
        address.append( ":" + QString::number( port ) );

    qInfo() << "==================================================";
    qInfo() << "Blackbox Online Ordering Data Adapter";
    qInfo() << "==================================================";

    qInfo() << "using blackbox address" << address;

    // register instance with DBUS
    QDBusConnection connection( QDBusConnection::sessionBus() );

    if ( !connection.registerService( DBUS_SERVICE ) )
    {
        const QDBusError err( connection.lastError() );
        qWarning() << "failed to register service" << DBUS_SERVICE << err.type() << err.name() << err.message();
    }

    // online ordering db adapter instance
    OnlineOrderingDatabaseAdapter::instance()->setServerAddress( address );

    return a.exec();
}
