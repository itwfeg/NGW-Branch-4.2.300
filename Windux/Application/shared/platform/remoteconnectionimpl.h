/**
 * @file remoteconnectionimpl.h
 * Class implementation for remote machine connections
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup platformCsu
 * @{
 */

#ifndef REMOTECONNECTIONIMPL_H
#define REMOTECONNECTIONIMPL_H

#include "remoteconnection.h"

#if defined( Q_OS_WIN )
#include <windows.h>
#endif

#include <CkHttp.h>
#include <CkSFtp.h>
#include <CkSFtpDir.h>
#include <CkSFtpFile.h>
#include <CkString.h>

// /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// ================================================================================================
//
// Chilkat CkHttp Implementation
//
// ================================================================================================
// \/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
inline bool remoteConnection<CkHttp>::connectAsUser( const QString &host, const QString &user, const QString &pwd )
{
    Q_UNUSED( user )
    Q_UNUSED( pwd )

    QString url( "http://" + host + "/home.asp" );
    CkString data;

    // send HTTP GET looking for known legacy hlx page
    if ( c_.QuickGetStr( qPrintable( url ), data ) )
    {
/*
        // ok response
        if ( 200 == c_.get_LastStatus() )
            return true;
*/
        // Note:
        // Disabling HLX http access for now... the new chilkat libs are missing this export so
        // we are unable to link properly.  If you need HLX comms please see Randy.
        return false;
    }

    // failure
    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
inline bool remoteConnection<CkHttp>::connect( const QString &host )
{
    return connectAsUser( host, QString(), QString() );
}

template <>
inline bool remoteConnection<CkHttp>::downloadFile( const QString& remote, const QString& local )
{
    // check parameters
    if (( remote.length() ) && ( local.length() ))
    {
        // download!
        return c_.Download( qPrintable( remote ), qPrintable( local ) );
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
inline bool remoteConnection<CkHttp>::rsync( const QString& remote, const QString& local )
{
    Q_UNUSED( remote );
    Q_UNUSED( local );

    // cannot rsync with http
    return false;
}

// /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
// ================================================================================================
//
// Chilkat CkSFtp Implementation
//
// ================================================================================================
// \/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
inline bool remoteConnection<CkSFtp>::connectAsUser( const QString &host, const QString &user, const QString &pwd )
{
    // connection timeout
    c_.put_ConnectTimeoutMs( 2000 );

    // idle timeout
    c_.put_IdleTimeoutMs( 0 );

    // connect to the SSH server...
    if ( c_.Connect( qPrintable( host ), 22 ) )
    {
        // authenticate and initialize
        if ( c_.AuthenticatePw( qPrintable( user ), qPrintable( pwd ) ) )
            if ( c_.InitializeSftp() )
            {
                // success!
                return true;
            }
    }

    // failure
    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
inline bool remoteConnection<CkSFtp>::connect( const QString &host )
{
    // Note:
    // Logging in as root is no longer supported (unless you are a super awesome developer with
    // much wisdom for situations like these)! Use the -user and -pwd command line switches to
    // specify a username/password. Not sure what to use?!? Try "-user hobart -pwd admin8758"...

#ifdef QT_DEBUG
    static const QString username( "cm9vdA==" ); // root (in base64)
    static const QString password( "aG8xYmFydDI=" ); // ho1bart2 (in base64)
#else
    static const QString username( "aG9iYXJ0" ); // hobart (in base64)
    static const QString password( "YWRtaW44NzU4" ); // admin8758 (in base64)
#endif

    return connectAsUser( host, QByteArray::fromBase64( username.toLatin1() ), QByteArray::fromBase64( password.toLatin1() ) );
}

template <>
inline bool remoteConnection<CkSFtp>::downloadFile( const QString& remoteFile, const QString& local )
{
    // check parameters
    if (( remoteFile.length() ) && ( local.length() ))
    {
        const char *handle;

        // remove the drive letter, if present
        QString remote( remoteFile );

        if (( 2 < remote.length() ) && ( 0 == remote.left( 2 ).compare( "c:", Qt::CaseInsensitive ) ))
            remote = remote.mid( 2 );

        // open a file on the server
        if ( (handle = c_.openFile( qPrintable( remote ), "readOnly", "openExisting" )) )
        {
            // download the file
            bool success = c_.DownloadFile( handle, qPrintable( local ) );

            // close handle
            c_.CloseHandle( handle );

            return success;
        }
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
template <>
inline bool remoteConnection<CkSFtp>::rsync( const QString& remotePath, const QString& local )
{
    // check parameters
    if (( remotePath.length() ) && ( local.length() ))
    {
        const char *handle;

        // remove the drive letter, if present
        QString remote( remotePath );

        if (( 2 < remote.length() ) && ( 0 == remote.left( 2 ).compare( "c:", Qt::CaseInsensitive ) ))
            remote = remote.mid( 2 );

        // open a directory on the server
        if ( (handle = c_.openDir( qPrintable( remote ) )) )
        {
            bool success = false;

            CkSFtpDir *dirListing = NULL;
            CkSFtpFile *file = NULL;

            // get the directory listing
            if ( (dirListing = c_.ReadDir( handle )) )
            {
                int count = dirListing->get_NumFilesAndDirs();

                // assume success
                success = true;

                // iterate over the files
                for ( int i = 0; i < count; ++i )
                    if ( (file = dirListing->GetFileObject( i )) )
                    {
                        CkString name;
                        CkString type;

                        // retrieve name and type
                        file->get_Filename( name );
                        file->get_FileType( type );

                        // download only regular files
                        if ( "regular" == QString( type.getString() ) )
                            success &= downloadFile( remote + "/" + name.getString(), local + "/" + name.getString() );

                        delete file;
                    }

                delete dirListing;
            }

            // close handle
            c_.CloseHandle( handle );

            return success;
        }
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // REMOTECONNECTIONIMPL_H
