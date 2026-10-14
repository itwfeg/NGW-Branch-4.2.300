/**
 * @file filesystem.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "filesystem.h"

#include "fileaccess.h"
//#include "remotefileaccess.h"

#include <QDir>
#include <QTime>

QMutex filesystem::mutex_;
fileAccess *filesystem::instance_ = NULL;

filesystem::instanceMap filesystem::instances_;

///////////////////////////////////////////////////////////////////////////////////////////////////
QString filesystem::folder( folderType::type type, bool system )
{
    return instance()->folder( type, system );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool filesystem::isConnected()
{
    return instance()->isConnected();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void filesystem::setFolder( const QString& value, folderType::type type, bool system )
{
    instance()->setFolder( value, type, system );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString filesystem::absoluteFilePath( const QString& filename, folderType::type type )
{
    return instance()->absoluteFilePath( filename, type );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void filesystem::connectToHost( void *cookie, const QString& host, const QString& user, const QString& pwd )
{
    QMutexLocker lock( &mutex_ );

    // look to see if exists already
    if ( instances_.contains( cookie ) )
    {
        // set active
        instance_ = instances_[cookie];
        return;
    }

    // create a new one
    fileAccess *f = NULL;
    f = new fileAccess();

/*
    // use local filesystem
    if (( host.isEmpty() ) || ( "localhost" == host ) || ( "127.0.0.1" == host ))
        f = new fileAccess();

    // use remote filesystem
    else
        f = new remoteFileAccess( host, user, pwd );

    // make active
    instances_.insert( cookie, f );
    instance_ = f;
*/
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString filesystem::fileName( const QString& fullFilename, folderType::type type )
{
    return instance()->fileName( fullFilename, type );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
fileAccess *filesystem::instance( void *cookie )
{
    QMutexLocker lock( &mutex_ );

    // check for existance
    if ( !instances_.contains( cookie ) )
        return NULL;

    return instances_[cookie];
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool filesystem::syncFolder( folderType::type type )
{
    return instance()->syncFolder( type );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
fileAccess *filesystem::instance()
{
    if ( !instance_ )
    {
        QMutexLocker lock( &mutex_ );

        if ( !instance_ )
        {
            // create base instance
            instances_.insert( 0, (instance_ = new fileAccess()) );

            // seed the rng
            QTime midnight( 0, 0, 0 );
            qsrand( midnight.secsTo( QTime::currentTime() ) );

            // clear out cache
            //remoteFileAccess::clearCache();
        }
    }

    return instance_;
}
