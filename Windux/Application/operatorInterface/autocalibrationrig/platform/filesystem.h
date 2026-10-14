/**
 * @file filesystem.h
 * This file contains methods for accessing the filesystem.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup platformCsu
 * @{
 */

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "foldertype.h"

#include <QMap>
#include <QMutex>
#include <QString>

class fileAccess;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Filesystem access class.
/**
 * This is a static wrapper class that manages instancing for @c fileAccess objects.
 */
class filesystem
{
    typedef filesystem _Myt;

public:

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve folder location.
    /**
     * @param[in] type  file type
     * @param[in] system  @c true to get system path instead of user path
     * @return  path to folder of requested type
     */
    static QString folder( folderType::type type, bool system = false );

    /// Set folder location.
    /**
     * Check if filesystem is connected to remote system.
     *
     * @returns  @c true if using remote system, @c false otherwise
     */
    static bool isConnected();

    /**
     * Set folder location.
     *
     * This method will override the default folder location.
     *
     * @param[in] value  folder name
     * @param[in] type  folder type
     * @param[in] system  @c true to get system path instead of user path
     */
    static void setFolder( const QString& value, folderType::type type, bool system = false );

    // ========================================================================
    // Methods
    // ========================================================================

    /// Get absolute file path to a file.
    /**
     * Check user and system folder for file @p filename. Returns absolute file path to the file.
     *
     * For example:
     *   filename is "help/index.html" and folder type is Html
     *   return value is "/usr/local/hobart/html/help/index.html"
     *
     * For example:
     *   filename is "icons/randyCoding.jpg" and folder type is Images
     *   return value is "/usr/local/hobart/images/icons/randyCoding.jpg"
     *
     * For example:
     *   filename is "kittenSleeping.jpg" and folder type is Images
     *   return value is "/usr/local/hobart/images/kittenSleeping.jpg"
     *
     * @param[in] filename  filename to check for
     * @param[in] type  file type
     * @return  absolute file path
     */
    static QString absoluteFilePath( const QString& filename, folderType::type type );

    /// Create a filesystem object that is mapped to a local or remote system.
    /**
     * If a @c filesystem object already exists for cookie @p cookie, it as set as the active
     * filesystem object.
     *
     * Otherwise a new filesystem object will be created and mapped to the host @p host.
     *
     * @param[in] cookie  filesystem cookie
     * @param[in] host  remote host to connect to
     * @param[in] user  username
     * @param[in] pwd  password
     */
    static void connectToHost( void *cookie, const QString& host = QString(), const QString& user = QString(), const QString& pwd = QString() );

    /// Strip absolute file path and return just filename.
    /**
     * When @p fullFilename is an absolute file name into folder @p type, this method will return
     * only the filename.
     *
     * For example:
     *   full filename is "/usr/local/hobart/html/help/index.html" and folder type is Html
     *   return value is "help/index.html"
     *
     * For example:
     *   full filename is "/usr/local/hobart/images/icons/randyCoding.jpg" and folder type is Images
     *   return value is "icons/randyCoding.jpg"
     *
     * For example:
     *   full filename is "/usr/local/hobart/images/kittenSleeping.jpg" and folder type is Images
     *   return value is "kittenSleeping.jpg"
     *
     * @param[in] fullFilename  absolute filename
     * @param[in] type  file type
     * @return  file name only
     */
    static QString fileName( const QString& fullFilename, folderType::type type );

    /// Sync files from remote system to local.
    /**
     * Retrieve instance of file access object.
     *
     * @param[in] cookie  filesystem cookie
     * @returns  pointer to object, or NULL if it does not exist
     */
    static fileAccess *instance( void *cookie );

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
    static bool syncFolder( folderType::type type );

protected:

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve instance.
    /**
     * @return  pointer to instance
     */
    static fileAccess *instance();

private:

    static QMutex mutex_;
    static fileAccess *instance_;

    typedef QMap<void*, fileAccess*> instanceMap;
    static instanceMap instances_;

    // ========================================================================

    // not implemented
    filesystem();

    // not implemented
    filesystem( const _Myt& rhs );

    // not implemented
    _Myt& operator = ( const _Myt& rhs );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // FILESYSTEM_H
