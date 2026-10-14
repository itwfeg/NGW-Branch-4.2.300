/**
 * @file apihte.h
 * HTe API.
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef APIHTE_H
#define APIHTE_H

#include "abstractapi.h"

#include <QUrl>
#include <QVector>

///////////////////////////////////////////////////////////////////////////////////////////////////
class hteapi : public abstractapi
{
    Q_OBJECT

    typedef hteapi _Myt;
    typedef abstractapi _Mybase;

signals:

    /// Signal for document processing.
    /**
     * @param[in] request  request
     * @param[in] status  request status
     * @param[in] response  response
     */
    void processDocument( const QJsonDocument& request, int status, const QJsonDocument& response );

    /// Signal for file processing.
    /**
     * @param[in] request  request
     * @param[in] status  request status
     * @param[in] tempFileLocation  temporary file location
     * @param[in] status  request uuid
     * @param[in] filename  response
     */
    void processFile( const QJsonDocument& request, int status, const QString& tempFileLocation, const QString& uuid, const QString& filename );

public:

    /// Request return status.
    enum RequestStatus
    {
        SUCCESS,                                    ///< 200      - successfully received server data
        NODATA,                                     ///< 400 - no data available so......
        CACHEDATA                                   ///< 204, 404, 417 - use available cache data, timeouts
    };

    // ------------------------------------------------------------------------
    // CTOR / DTOR
    // ------------------------------------------------------------------------

    /// Constructor.
    /**
     * @param[in,out] parent  parent object
     */
    hteapi( QObject *parent = nullptr );

    /// Destructor.
    virtual ~hteapi();

    // ------------------------------------------------------------------------
    // Properties
    // ------------------------------------------------------------------------

    /// Retrieve number of saved requests.
    /**
     * @return  how many saved requests to store
     */
    virtual unsigned int savedRequests() const {return savedRequests_;}

    /// Set number of saved requests.
    /**
     * @param[in] value  how many saved requests to store
     */
    virtual void setSavedRequests( unsigned int value ) {savedRequests_ = value;}

    // ------------------------------------------------------------------------
    // Methods
    // ------------------------------------------------------------------------

    /// Import last usage data.
    /**
     * @param[out] stamp  last usage timestamp
     * @param[out] api  last usage endpoint information
     * @return  @c true upon success, @c false otherwise
     */
    virtual bool importLastUsage( QDateTime& stamp, QString& api );

public slots:

    // ------------------------------------------------------------------------
    // Methods
    // ------------------------------------------------------------------------

    /// Download file.
    /**
     * @param[in] url  url
     * @param[in] uuid  uuid
     * @param[in] filename  filename
     * @param[in] request  request; when valid download as POST otherwise GET
     */
    virtual void downloadFile( const QUrl& url, const QString& uuid, const QString& filename, const QJsonDocument& request = QJsonDocument(), unsigned int timeout = 0, unsigned int maxAttempts = 1 );

    /// Send request.
    /**
     * @param[in] url  url to download
     * @param[in] request  request
     * @param[in] timeout  >0 to for request timeout (ms)
     * @param[in] maxAttempts  number of attempts
     */
    virtual void send( const QUrl& url, const QJsonDocument& request, unsigned int timeout, unsigned int maxAttempts, const QUrl& backupUrl = QUrl() );

protected:

    enum QueryResultParam
    {
        TIMESTAMP = 0,
        PLU,
        UUID,
        RETURNCODE,
        RETURNBYTES,
        RETRIES,
        TIMESPENT,
        COMMENTS
    };

    typedef QVector<QString> queryResult;

    unsigned int savedRequests_;

    // ------------------------------------------------------------------------
    // Methods
    // ------------------------------------------------------------------------

    /// Export last usage data with current timestamp.
    /**
     * @param[in] api  last usage endpoint information
     */
    virtual void exportLastUsage( const QString& api ) const;

    /// Write query results to file.
    virtual void logQueryToFile( const queryResult& query, const QString& filename ) const;

    // ------------------------------------------------------------------------
    // Static Methods
    // ------------------------------------------------------------------------

    /// Find out the return code results.
    static QString statusCodeLookup( int code );

    /// Determine what error codes we need to retry contact.
    static bool exitRetries( int code );

private slots:

    /// Slot for reply download progress.
    void onReplyDownloadProgress( QNetworkReply *reply, qint64 bytesReceived, qint64 bytesTotal, unsigned int elapsed );

    /// Slot for reply received.
    void onReplyReceived( QNetworkReply *reply, bool valid, int statusCode, const QJsonDocument& request, const QJsonDocument& response, unsigned int elapsed );

private:

    struct requestControl
    {
        QDateTime start;
        QDateTime stop;

        unsigned int timeout;

        unsigned int maxAttempts;
        unsigned int attempts;

        QUrl url;
        QUrl backupUrl;
        QString uuid;
        QJsonDocument request;

        QString file;

        RequestStatus status;
        QJsonDocument response;

        QString location;

        bool usingBackupUrl;
    };

    typedef QMap<QNetworkReply*, requestControl> requestMap;

    mutable QMutex m_;

    requestMap pending_;

    /// Process request control.
    QNetworkReply *processRequestControl( const requestControl& rc );

    /// Create request control block.
    requestControl createDocumentRequestControl( const QUrl& url, const QString& uuid, const QJsonDocument& request, unsigned int timeout, unsigned int maxAttempts, const QUrl& backupUrl = QUrl() );

    /// Create request control block.
    requestControl createFileRequestControl( const QUrl& url, const QString& uuid, const QString& filename, const QJsonDocument& request = QJsonDocument(), unsigned int timeout = 0, unsigned int maxAttempts = 1 );

    /// Read request control block.
    requestControl readRequestControl( QNetworkReply *reply ) const;

    /// Write request control block.
    void writeRequestControl( QNetworkReply *reply, const requestControl& rc );

    /// Destroy request control block.
    void destroyRequestControl( QNetworkReply *reply );

    // not implemented
    hteapi( const _Myt& ) = delete;

    // not implemented
    _Myt &operator = ( const _Myt& ) = delete;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // APIHTE_H
