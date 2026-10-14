/**
 * @file remotefileaccess.h
 * Class that allows access to remote files.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup platformCsu
 * @{
 */

#ifndef REMOTEFILEACCESS_H
#define REMOTEFILEACCESS_H

#include "fileaccess.h"

#include <QDir>

class abstractRemoteConnection;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Class that allows access to remote files.
class remoteFileAccess : public fileAccess
{
    Q_OBJECT

    typedef remoteFileAccess _Myt;
    typedef fileAccess _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] host  remote host to connect to
     * @param[in] user  username
     * @param[in] pwd  password
     */
    remoteFileAccess( const QString& host, const QString& user = QString(), const QString& pwd = QString() );

    /// Destructor.
    virtual ~remoteFileAccess();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve folder location.
    /**
     * @param[in] type  file type
     * @param[in] system  @c true to get system path instead of user path
     * @return  path to folder of requested type
     */
    virtual QString folder( folderType::type type, bool system = false ) const;

    /// Check if filesystem is connected to remote system.
    /**
     * @return  @c true if using remote system, @c false otherwise
     */
    virtual bool isConnected() const;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Get absolute file path to a file.
    /**
     * Check user and system folder for file @p filename. Returns absolute file path to the file.
     *
     * @param[in] filename  filename to check for
     * @param[in] type  file type
     * @return  absolute file path
     */
    virtual QString absoluteFilePath( const QString& filename, folderType::type type ) const;

    /// Strip absolute file path and return just filename.
    /**
     * This method will pull all files from a remote folder local. This method does nothing when
     * connected locally, it only works for remote file access.
     *
     * @warning
     * This method will not recursively pull files, it only pulls the immediate files from the
     * remote folder.
     *
     * @param[in] type  file type
     * @return  @c true if folder was synced, @c false otherwise
     */
    virtual bool syncFolder( folderType::type type ) const;

    // ========================================================================
    // Static Methods
    // ========================================================================

    /// Clear all files from the cache.
    /**
     * This method will delete all cache folders from the temporary folder.
     */
    static void clearCache();

protected:

    // ========================================================================
    // Static Methods
    // ========================================================================

    /// Create cache folder for temporary files.
    /**
     * @return  directory of newly created cache location
     */
    static QDir createCacheFolder();

private:

    enum ConnectionType
    {
        IsHTTP,
        IsSFTP
    };

    QDir cache_;
    QString host_;

    QString user_;
    QString pwd_;

    abstractRemoteConnection *remote_;
    ConnectionType remoteType_;

    folderConfig remoteConfig_;

    // ========================================================================

    /// Connect to remote system.
    void connect();

    /// Cache file from remote system to local.
    bool downloadFile( const QDir& dir, const QString& filename, folderType::type type ) const;

    /// Cache all files in folder from remote system to local.
    bool rsync( const QDir& dir, folderType::type type ) const;

    /// Make a remote filename.
    QString remoteFilename( const QString& filename, folderType::type type, bool isSystem ) const;

    /// Make a remote path.
    QString remotePath( folderType::type type, bool isSystem ) const;

    /// Remove a folder and all files/subfolders within.
    static bool removeFolder( const QString& path );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // REMOTEFILEACCESS_H
