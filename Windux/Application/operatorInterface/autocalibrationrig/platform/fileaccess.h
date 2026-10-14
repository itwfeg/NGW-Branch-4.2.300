/**
 * @file fileaccess.h
 * Class that allows access to files in specific locations.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup platformCsu
 * @{
 */

#ifndef FILEACCESS_H
#define FILEACCESS_H

#include "folderconfig.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Class that allows access to files in specific locations.
class fileAccess : public folderConfig
{
    Q_OBJECT

    typedef fileAccess _Myt;
    typedef folderConfig _Mybase;

Q_SIGNALS:

    /**
     * Signal when download of file is in progress.
     *
     * @param[in] active  @c true when downloading a file, @c false when not (i.e. download complete)
     * @param[in] filename  name of file
     */
    void downloadFileInProgress( bool active, const QString& filename );

    /**
     * Signal when sync is in progress.
     *
     * @param[in] active  @c true when syncing, @c false when not (i.e. sync complete)
     */
    void syncInProgress( bool active );

public:

    // ============================================================================================
    // CTOR / DTOR
    // ============================================================================================

    /// Constructor.
    fileAccess();

    /// Destructor.
    virtual ~fileAccess();

    // ============================================================================================
    // Properties

    /**
     * Check if filesystem is connected to remote system.
     *
     * @returns  @c true if using remote system, @c false otherwise
     */
    virtual bool isConnected() const {return true;}

    ///////////////////////////////////////////////////////////////////////////
    // Methods
    // ============================================================================================

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
     * When @p fullFilename is an absolute file name into folder @p type, this method will return
     * only the filename.
     *
     * @param[in] fullFilename  absolute filename
     * @param[in] type  file type
     * @return  file name only
     */
    virtual QString fileName( const QString& fullFilename, folderType::type type ) const;

    /// Sync files from remote system to local.
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

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // FILEACCESS_H
