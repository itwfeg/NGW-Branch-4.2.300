/**
 * @file main.cpp
 * DeviceWise Configuration Utility.
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */

#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>
#include <netinet/in.h>

#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>

///////////////////////////////////////////////////////////////////////////////////////////////////
QString generateHardwareId()
{
    QString result;

    int sock( socket( PF_INET, SOCK_DGRAM, 0 ) );

    // open socket
    if ( sock < 0 )
        qDebug() << "Error opening socket!";
    else
    {
        struct if_nameindex *names( if_nameindex() );

        // iterate interface names
        if ( names )
        {
            struct if_nameindex *n( names );

            while ( n->if_name )
            {
                struct ifreq ifr;
                strcpy( ifr.ifr_name, n->if_name );

                // retrieve adapter info
                if ( 0 != ioctl( sock, SIOCGIFFLAGS, &ifr ) )
                    qDebug() << "Error getting flags for adapter" << ifr.ifr_name;

                // ignore loopback adapter
                else if ( !(IFF_LOOPBACK & ifr.ifr_flags) )
                {
                    if ( 0 != ioctl( sock, SIOCGIFHWADDR, &ifr ) )
                        qDebug() << "Error getting hwaddr for adapter" << ifr.ifr_name;
                    else
                    {
                        char mac[32];

                        sprintf( mac, "%02X%02X%02X%02X%02X%02X",
                            ifr.ifr_addr.sa_data[0],
                            ifr.ifr_addr.sa_data[1],
                            ifr.ifr_addr.sa_data[2],
                            ifr.ifr_addr.sa_data[3],
                            ifr.ifr_addr.sa_data[4],
                            ifr.ifr_addr.sa_data[5] );

                        qDebug() << ifr.ifr_name << mac;

                        // check for eth0
                        if ( 0 == strcmp( ifr.ifr_name, "eth0" ) )
                        {
                            result.append( "bb" );
                            result.append( mac );
                        }
                    }
                }

                n = &n[1];
            }

            if_freenameindex( names );
        }

        // close socket
        close( sock );
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool updateDeviceWisePersistence( const QString &hwid )
{
    const QString hwidValue( QString::fromLatin1( "\x01" ) + "Custom: " + hwid );
    const QString httpValue( QString::fromLatin1( "\x01" ) + "Stopped" );

    QSqlQuery updateQuery;

    // save hwid value
    updateQuery.prepare( "UPDATE persistence SET value=? WHERE name='registration_hwid'" );
    updateQuery.bindValue( 0, hwidValue );

    if ( !updateQuery.exec() )
    {
        qDebug() << "Error saving persistence hwid";
        return false;
    }

    // save http value
    updateQuery.prepare( "INSERT OR REPLACE INTO persistence (name, grp, value) VALUES (?,?,?)" );
    updateQuery.bindValue( 0, "state" );
    updateQuery.bindValue( 1, "http.server" );
    updateQuery.bindValue( 2, httpValue );

    if ( !updateQuery.exec() )
    {
        qDebug() << "Error saving persistence http";
        return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool updateDeviceWiseTR50( const QString &hwid )
{
    // retrieve
    QSqlQuery query;

    if ( !query.exec( "SELECT json_conn_data FROM tr50_connections WHERE conn_name='Default'" ) )
    {
        qDebug() << "Error retrieving tr50 connections";
        return false;
    }
    else if ( !query.next() )
    {
        qDebug() << "Error retrieving tr50 connections value";
        return false;
    }

    // update value
    const QString key( "\"registration_hwid\":" );

    QString value( query.value( 0 ).toString() );

    int start;
    int end( -1 );

    if ( 0 <= (start = value.indexOf( key )) )
        end = value.indexOf( ",", start );

    if (( 0 <= start ) && ( start < end ))
    {
        const int offset( start + key.length() );

        value.replace( offset, end - offset, "\"Custom: " + hwid + "\"" );
    }
    else
    {
        qDebug() << "Error updating tr50 connections value";
        return false;
    }

    query.clear();

    // save value
    QSqlQuery updateQuery;
    updateQuery.prepare( "UPDATE tr50_connections SET json_conn_data=? WHERE conn_name='Default'" );
    updateQuery.bindValue( 0, value );

    if ( !updateQuery.exec() )
    {
        qDebug() << "Error saving tr50 connections";
        return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool updateDeviceWise( const QString &path, const QString &hwid )
{
    bool result( false );

    QString connection;

    // check for existance
    if ( !QFile::exists( path ) )
        qDebug() << "Database does not exist" << path;

    else
    {
        QSqlDatabase db( QSqlDatabase::addDatabase( "QSQLITE" ) );
        db.setDatabaseName( path );

        // open db
        if ( !db.open() )
            qDebug() << "Error opening database" << path;

        // update db
        else
        {
            result = (( updateDeviceWisePersistence( hwid ) ) && ( updateDeviceWiseTR50( hwid ) ));
        }

        connection = db.connectionName();
    }

    if ( connection.length() )
        QSqlDatabase::removeDatabase( connection );

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int main( int argc, char *argv[] )
{
    Q_UNUSED( argc );
    Q_UNUSED( argv );

    const QString databaseNameCore( "/opt/dw/db/DWCORE.db" );
    const QString databaseNameUser( "/opt/dw/db/DWUSER.db" );
    const QString hwid( generateHardwareId() );

    if ( hwid.isEmpty() )
    {
        qDebug() << "Error generating hwid!";
        return -1;
    }

    if ( !updateDeviceWise( databaseNameCore, hwid ) )
    {
        qDebug() << "Error saving hwid to core!";
        return -1;
    }

    if ( !updateDeviceWise( databaseNameUser, hwid ) )
    {
        qDebug() << "Error saving hwid to user!";
        return -1;
    }

    qDebug() << "Success!" << hwid;
    return 0;
}
