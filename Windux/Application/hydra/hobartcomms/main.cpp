/**
 * @file main.cpp
 * @brief Application entry point.
 *
 * @copyright Copyright (c) 2021, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#include "commmanager.h"

#include <devicetype.h>

#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusError>
#include <QDebug>

///////////////////////////////////////////////////////////////////////////////////////////////////
int main( int argc, char *argv[] )
{
    QCoreApplication a( argc, argv );

    qInfo() << "==================================================";
    qInfo() << "Comms";
    qInfo() << "==================================================";

    // register instance with DBUS
    QDBusConnection connection( QDBusConnection::sessionBus() );

    if ( !connection.registerService( COMM_MANAGER_DBUS_SERVICE ) )
    {
        const QDBusError err( connection.lastError() );
        qWarning() << "failed to register service" << COMM_MANAGER_DBUS_SERVICE << err.type() << err.name() << err.message();
    }

    // comm manager instance
    CommManager::instance();

    return a.exec();
}
