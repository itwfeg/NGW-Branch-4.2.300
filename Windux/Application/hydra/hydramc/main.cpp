/**
 * @file main.cpp
 * @brief Application entry point.
 *
 * @copyright Copyright (c) 2021, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#include "messagecenter.h"

#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusError>
#include <QDebug>

static const QString DBUS_SERVICE( "com.hobartcorp.MessageCenter" );

///////////////////////////////////////////////////////////////////////////////////////////////////
int main( int argc, char *argv[] )
{
    QCoreApplication a( argc, argv );

    qInfo() << "==================================================";
    qInfo() << "Hydra Message Center";
    qInfo() << "==================================================";

    // register instance with DBUS
    QDBusConnection connection( QDBusConnection::sessionBus() );

    if ( !connection.registerService( DBUS_SERVICE ) )
    {
        const QDBusError err( connection.lastError() );
        qWarning() << "failed to register service" << DBUS_SERVICE << err.type() << err.name() << err.message();
    }

    // message center instance
    MessageCenter::instance();

    return a.exec();
}
