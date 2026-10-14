/**
 * @file remotefileaccess.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "logmanager.h"
#include "remoteconnection.h"
#include "remotefileaccess.h"

#ifndef DISABLE_CHILKAT
#include "remoteconnectionimpl.h"
#endif

#include <QCoreApplication>

///////////////////////////////////////////////////////////////////////////////////////////////////
remoteFileAccess::remoteFileAccess( const QString& host, const QString& user, const QString& pwd ) :
    cache_( createCacheFolder() ),
    host_( host ),
    user_( user ),
    pwd_( pwd ),
    remote_( NULL )
{
    // connect to remote system
    connect();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
remoteFileAccess::~remoteFileAccess()
{
    if ( remote_ )
    {
        delete remote_;
        remote_ = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString remoteFileAccess::folder( folderType::type type, bool system ) const
{
    // use cached folder instead of configuration
    if (( isConnected() ) && ( folderType::isCachedFolderType( type )))
    {
        QDir d( cache_ );
        folderType::appendFolderName( d, type, system );

        return d.absolutePath();
    }

    return _Mybase::folder( type, system );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool remoteFileAccess::isConnected() const
{
    return ( remote_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString remoteFileAccess::absoluteFilePath( const QString& filename, folderType::type type ) const
{
    // nothing to do
    if ( isQualifiedFilename( filename ) )
        return filename;

    // attempt to copy file from remote system
    if (( isConnected() ) && ( folderType::isCachedFolderType( type )))
    {
        // try user folder, if fails try system folder...
        if ( !downloadFile( folder( type ), filename, type ) )
            downloadFile( folder( type, true ), filename, type );
    }

    // give result to caller
    return _Mybase::absoluteFilePath( filename, type );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool remoteFileAccess::syncFolder( folderType::type type ) const
{
    // attempt to copy files from remote system
    if (( isConnected() ) && ( folderType::isCachedFolderType( type )))
    {
        bool result = false;

        // sync user folder...
        result |= rsync( folder( type ), type );

        // sync system folder...
        result |= rsync( folder( type, true ), type );

        return result;
    }

    return _Mybase::syncFolder( type );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void remoteFileAccess::clearCache()
{
    // remove all folders within the hobart temporary area
    removeFolder( QDir::tempPath() + "/hobart" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QDir remoteFileAccess::createCacheFolder()
{
    QDir folder( QDir::temp() );

    // create cache folder
    const QString cacheFolder( "hobart" );
    folder.mkdir( cacheFolder );
    folder.cd( cacheFolder );

    // create session folder
    QString sessionFolder;

    for ( ;; )
    {
        static const QString alphabet( "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789" );

        // session folder is 10 random letters/numbers
        while ( sessionFolder.length() < 10 )
            sessionFolder += alphabet[qrand() % alphabet.length()];

        if ( !folder.exists( sessionFolder ) )
            break;

        sessionFolder.clear();
    }

    folder.mkdir( sessionFolder );
    folder.cd( sessionFolder );

    // done!
    return folder;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void remoteFileAccess::connect()
{
#ifndef DISABLE_CHILKAT
    bool useConnectAsUser(( user_.length() ) && ( pwd_.length() ));

    //
    // check for SFTP connection
    //

    remote_ = new remoteConnection<CkSFtp>();

    if ((( useConnectAsUser ) && ( remote_->connectAsUser( host_, user_, pwd_ ) )) ||
        (( !useConnectAsUser ) && ( remote_->connect( host_ ) )))
    {
        QString remote;
        QString local;

        // test for linux machine
        remoteConfig_.setPlatform( platformType::EmbeddedLinux );

        remote = remoteConfig_.configFileName();
        local = cache_.absoluteFilePath( "config_linux.dat" );

        // pull configuration file local
        if ( remote_->downloadFile( remote, local ) )
        {
            // load the config
            remoteConfig_.loadFromFile( local );

            // success!
            remoteType_ = IsSFTP;
            return;
        }

        // test for windows machine
        remoteConfig_.setPlatform( platformType::Windows );

        remote = remoteConfig_.configFileName();
        local = cache_.absoluteFilePath( "config.dat" );

        // pull configuration file local
        if ( remote_->downloadFile( remote, local ) )
        {
            // load the config
            remoteConfig_.loadFromFile( local );

            // success!
            remoteType_ = IsSFTP;
            return;
        }
    }

    delete remote_;
    remote_ = NULL;

    //
    // check for HTTP connection
    //

    remote_ = new remoteConnection<CkHttp>();

    if ((( useConnectAsUser ) && ( remote_->connectAsUser( host_, user_, pwd_ ) )) ||
        (( !useConnectAsUser ) && ( remote_->connect( host_ ) )))
    {
        // success!
        remoteType_ = IsHTTP;
        return;
    }

    delete remote_;
    remote_ = NULL;
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool remoteFileAccess::downloadFile( const QDir& dir, const QString& filename, folderType::type type ) const
{
    LOG_CLASS_INFO( "remoteFileAccess", "downloadFile" );

    // sanity check
    if ( !remote_ )
        return false;

    QString localFilename( dir.absoluteFilePath( filename ) );

    // file already exists
    if ( QFile::exists( localFilename ) )
        return true;

    QString path( dir.path() );

    // check for embedded folders within filename
    int fs = localFilename.lastIndexOf( "/" );
    int bs = localFilename.lastIndexOf( "\\" );

    // if embeded folders exist, use that path instead
    if ( bs < fs )
        path = localFilename.left( fs );
    else if ( fs < bs )
        path = localFilename.left( bs );

    path = QDir::cleanPath( path );

    // make sure local path exists
    dir.mkpath( path );

    // check if system folder
    bool isSystemFile = path.contains( "system", Qt::CaseInsensitive );

    // download file
    _Myt *emitter( const_cast<_Myt*>( this ) );

    LOG_DEBUG( "downloading file '%s'...", qPrintable( localFilename ) );
    Q_EMIT emitter->downloadFileInProgress( true, localFilename );

    bool result( remote_->downloadFile( remoteFilename( filename, type, isSystemFile ), localFilename ) );

    LOG_DEBUG( "download of file complete %d", result );
    Q_EMIT emitter->downloadFileInProgress( false, QString() );

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool remoteFileAccess::rsync( const QDir& dir, folderType::type type ) const
{
    LOG_CLASS_INFO( "remoteFileAccess", "rsync" );

    // sanity check
    if ( !remote_ )
        return false;

    QString localPath( dir.path() );

    // make sure local path exists
    dir.mkpath( localPath );

    // check if system folder
    bool isSystemFile = localPath.contains( "system", Qt::CaseInsensitive );

    // sync
    _Myt *emitter( const_cast<_Myt*>( this ) );

    LOG_DEBUG( "syncing path '%s'...", qPrintable( localPath ) );
    Q_EMIT emitter->syncInProgress( true );

    bool result( remote_->rsync( remotePath( type, isSystemFile ), localPath ) );

    LOG_DEBUG( "sync of path complete %d", result );
    Q_EMIT emitter->syncInProgress( false );

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString remoteFileAccess::remoteFilename( const QString& filename, folderType::type type, bool isSystem ) const
{
    QString path( remotePath( type, isSystem ) );

    // tack on the filename
    if ( path.length() )
        path.append( "/" + filename );

    return path;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString remoteFileAccess::remotePath( folderType::type type, bool isSystem ) const
{
    QString path;

    //
    // make HTTP filename
    //
    if ( IsHTTP == remoteType_ )
    {
        // Note:
        // Legacy HLX systems only really support certain types of images, you will not find
        // audio, video, etc. on the remote system.

        // what path is it located in
        switch ( type )
        {
        case folderType::CategoriesImages:
            path = "categories";
            break;
        case folderType::FlashkeyImages:
            path = "softkeys";
            break;
        case folderType::SelfServeImages:
            path = "selfserv";
            break;
        case folderType::SpecialsImages:
            path = "specials";
            break;
        case folderType::ProductsImages:
            path = "products";
            break;
        default:
            // legacy HLX system does not support the file type
            break;
        }

        // when no path file type not supported
        if ( path.length() )
        {
            // system files are located in special folder
            if ( isSystem )
                path = "system/" + path;

            // path including hostname
            path = "http://" + host_ + "/images/" + path;
        }
    }

    else if ( IsSFTP == remoteType_ )
    {
        // load path from remote config
        path = remoteConfig_.folder( type, isSystem );
    }

    return path;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool remoteFileAccess::removeFolder( const QString& dirName )
{
    bool result = false;
    QDir dir( dirName );

    if ( dir.exists( dirName ) )
    {
        foreach ( QFileInfo info, dir.entryInfoList( QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst ) )
        {
            if ( info.isDir() )
                result = removeFolder( info.absoluteFilePath() );
            else
                result = QFile::remove( info.absoluteFilePath() );

            if ( !result )
                return result;
        }

        result = dir.rmdir( dirName );
    }

    return result;
}
