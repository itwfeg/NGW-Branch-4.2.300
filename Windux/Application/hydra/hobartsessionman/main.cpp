/**
 * @file main.cpp
 * @brief Application entry point.
 *
 * @copyright Copyright (c) 2021, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#include "sessionmanager.h"

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
    qInfo() << "Session Manager";
    qInfo() << "==================================================";

    // register instance with DBUS
    QDBusConnection connection( QDBusConnection::sessionBus() );

    if ( !connection.registerService( SESSION_MANAGER_DBUS_SERVICE ) )
    {
        const QDBusError err( connection.lastError() );
        qWarning() << "failed to register service" << SESSION_MANAGER_DBUS_SERVICE << err.type() << err.name() << err.message();
    }

    // session manager instance
    SessionManager::instance();

    return a.exec();
}
