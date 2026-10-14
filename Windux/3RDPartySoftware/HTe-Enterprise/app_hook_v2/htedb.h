/**
 * @file htedb.h
 * HTe interface database management.
 *
 * @copyright Copyright (c) 2022 Hobart Corporation
 * @author Larry Pevoar
 */

#ifndef HTEDB_H
#define HTEDB_H

#include <QObject>
#include <QMutex>
#include <QMap>

class QJsonArray;
class QJsonObject;
class QSqlDatabase;
class QSqlRecord;
class QSqlTableModel;

///////////////////////////////////////////////////////////////////////////////////////////////////

#if defined(Q_OS_LINUX)
static const QString HTEDBDIR ( "db/hte/" );
#elif defined(Q_OS_WIN32)
static const QString HTEDBDIR ( "db\\hte\\" );
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Scale database management.
class hteDatabase : public QObject
{
    Q_OBJECT

    typedef hteDatabase _Myt;
    typedef QObject _Mybase;

public:

    static const QString HTEDBNAMEBASE;

    static const QString FILENAME;
    static const QString FILEID;
    static const QString FILESIZE;
    static const QString FILEVERSION;
    static const QString CHECKSUM;
    static const QString LISTEDINCURRENTHEARTBEAT;
    static const QString MOSTRECENTDOWNLOADDATETIME;
    static const QString STATUS;
    static const QString DOWNLOADS_TABLE;

    static const QString CONFIGNAME;
    static const QString CONFIGVALUE;
    static const QString CONFIGS_TABLE;

    static const QString CFG_UUID;
    static const QString CFG_HEARTBEATURL;
    static const QString CFG_HEARTBEATBACKUPURL;
    static const QString CFG_HEARTBEATENDPOINT;

    // The next one should go away when we get the correct download url from the HTe server
    static const QString CFG_HEARTBEATDOWNLOADURL;

    // ------------------------------------------------------------------------
    // CTOR / DTOR
    // ------------------------------------------------------------------------

    /// Constructor.
    /**
     * @param[in] parent  parent object
     */
    hteDatabase( QObject *parent = nullptr );

    /// Destructor.
    virtual ~hteDatabase();

    // ------------------------------------------------------------------------
    // Properties
    // ------------------------------------------------------------------------

    /// Check of valid.
    /**
     * @return  @c true if valid, @c false otherwise
     */
    virtual bool isValid() const;

    // ------------------------------------------------------------------------
    // Methods
    // ------------------------------------------------------------------------


    /// Get a JSON array of files in the Downloads table.
    /**
     * @param[in] downloadFile  download File data
     * @return  @c JSON array of files in the Downloads table
     */
    virtual const QJsonArray getDownloadFileList();

    /// Set all files in the Downloads table as "not received"
    /**
     * @return  @c nothing
     */
    virtual void markAllDownloadsReceivedFlag( bool received );

    /// Delete any files in the Downloads table that were not received in the most-current heartbeat response from the server.
    /**
     * @return  @c nothing
     */
    virtual void deleteAllDownloadsNotReceived();

	/// Determines if we should download a file:
    /// true if filename not in the Downloads table or if its checksum doesn't match
    /**
     * @param[in] fileName  file name
     * @param[in] checksum  file's checksum
     * @return  @c true upon success, @c false otherwise
     */
    virtual bool shouldDownloadFile( const QJsonObject &fileObj );
	
    /// Insert/Update a file entry in the Downloads table.
    /**
     * @param[in] fileObj  download File data received from HTe server, in the heartbeat response
     * @param[in] fileWasDownloaded  if true, sets the ListedInCurrentHeartbeat to 1 (true) and sets the MostRecentDownloadDateTime to the current date/time
     * @return  @c true upon success, @c false otherwise
     */
    virtual bool insertUpdateDownloadFile( const QJsonObject& fileObj, bool fileWasDownloaded = false );


    /// Retrieve config value from database.
    /**
     * @param[in] name  config record name
     * @return  config record value
     */
    virtual QString readConfig( const QString& name ) const;

    /// Save config value to database; creates record if it doesn't exist.
    /**
     * @param[in] name   config record name
     * @param[in] value  config record value
     * @return  true if saved, false if not
     */
    virtual bool saveConfig( const QString& name, const QString& value ) const;


protected:

    QSqlDatabase *hteDatabase_;                     ///< HTe database.

    QString htedbname_;

    static const QMap<QString, QString> defaultConfigsMap;

    // ------------------------------------------------------------------------
    // Methods
    // ------------------------------------------------------------------------

    /// Close database.
    virtual void closeDatabase();

    /// Open database.
    /**
     * @return  @c true upon success, @c false otherwise
     */
    virtual bool openDatabase();

    /// Open database connection for thread.
    /**
     * @return  database connection
     */
    virtual QSqlDatabase openDatabaseConnection() const;

    /// Convert a previous database schema's data to a newer schema.
    /// There should really be only one previous version on the SD card that would need to be converted.
    /**
     * @return  bool  successful or not
     */
    virtual bool convertDbSchemaVersion( const QString previousHTeDbSchema, const QString hteDbName );


private:

    mutable QMutex m_;
    mutable QMutex mIU_;

    // not implemented
    hteDatabase() = delete;

    // not implemented
    hteDatabase( const _Myt& ) = delete;

    // not implemented
    _Myt & operator = ( const _Myt& ) = delete;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // HTEDB_H
