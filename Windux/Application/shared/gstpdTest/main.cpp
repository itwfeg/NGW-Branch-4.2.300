/**
 * @file main.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "test.h"

#include <custgstmp.h>
#include <logmanager.h>
#include <opergstmp.h>

#include <QApplication>
#include <QDBusConnection>
#include <QDBusError>

///////////////////////////////////////////////////////////////////////////////////////////////////
int main( int argc, char *argv[] )
{
    LOG_CLASS_INFO( "", "main" );

    QApplication a( argc, argv );
    QStringList arguments( a.arguments() );

    int result( 0 );

    // init logging
    logManager::initialize( QString( argv[0] ) + ".config", create_from_config );

    // validate dbus running
    QDBusConnection dbus( QDBusConnection::sessionBus() );

    if ( !dbus.isConnected() )
    {
        LOG_ERROR( "failed to connect to DBUS", NULL );
        result = -1;
    }
    else
    {
        // parse arguments
        QString path;

        QString filename;
        abstractMediaPlayer::MediaType type( abstractMediaPlayer::UnknownMediaType );

        for ( int i( 1 ); i < arguments.size(); ++i )
        {
            if ( "-cust" == arguments[i] )
                path = custgstmp::DBUS_OBJECT_PATH;
            else if ( "-oper" == arguments[i] )
                path = opergstmp::DBUS_OBJECT_PATH;
            else if ( "-a" == arguments[i] )
                type = abstractMediaPlayer::Audio;
            else if ( "-v" == arguments[i] )
                type = abstractMediaPlayer::Video;
            else
            {
                filename = arguments[i];
            }
        }

        if (( path.isEmpty() ) || ( filename.isEmpty() ) || ( abstractMediaPlayer::UnknownMediaType == type ))
        {
            LOG_ERROR( "invalid arguments", NULL );
            result = -2;
        }
        else
        {
            // create player test
            test t( path, filename, type );

            LOG_INFO( "testing...", NULL );
            a.exec();

            LOG_INFO( "result: %s", (t.isValid() ? "pass" : "FAIL") );
        }
    }

    // finalize logging
    logManager::finalize();

    return result;
}
