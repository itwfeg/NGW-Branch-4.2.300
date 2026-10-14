/**
 * @file fileaccess.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "fileaccess.h"

#include <QDir>
#include <QString>

///////////////////////////////////////////////////////////////////////////////////////////////////
fileAccess::fileAccess()
{
#ifdef Q_OS_LINUX
    // for linux use harde coded firmware folder
    setFolder( "/root/firmware", folderType::Firmware );
#endif

    loadFromFile( configFileName() );
    loadFromEnvironment();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
fileAccess::~fileAccess()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString fileAccess::absoluteFilePath( const QString& filename, folderType::type type ) const
{
    // nothing to do
    if ( isQualifiedFilename( filename ) )
        return filename;

    // check user folder
    QDir userDir( folder( type ) );

    if ( userDir.exists( filename ) )
        return userDir.absoluteFilePath( filename );

    // use system folder
    QDir systemDir( folder( type, true ) );

    if ( systemDir.exists( filename ) )
        return systemDir.absoluteFilePath( filename );

    // the file does not exist in the user dir or the system dir... what do we do?

    // as per Scott Davis, use the user folder as the default
    return userDir.absoluteFilePath( filename );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString fileAccess::fileName( const QString& fullFilename, folderType::type type ) const
{
    // nothing to do
    if ( isQualifiedFilename( fullFilename ) )
    {
        QString path;

        // check user folder
        path = folder( type );

        if ( path == fullFilename.left( path.length() ) )
            return fullFilename.right( fullFilename.length() - path.length() - 1 );

        // use system folder
        path = folder( type, true );

        if ( path == fullFilename.left( path.length() ) )
            return fullFilename.right( fullFilename.length() - path.length() - 1 );
    }

    return fullFilename;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool fileAccess::syncFolder( folderType::type type ) const
{
    Q_UNUSED( type );

    // we are not a remote system, nothing to do
    return true;
}
