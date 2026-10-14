/**
 * @file apiedgify.h
 * Edgify API.
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef APIEDGIFY_H
#define APIEDGIFY_H

#include "abstractapi.h"

#include <QUrl>
#include <QVector>

///////////////////////////////////////////////////////////////////////////////////////////////////
class edgifyapi : public abstractapi
{
    Q_OBJECT

    typedef edgifyapi _Myt;
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
     * @param[in] filename  response
     */
    void processFile( const QJsonDocument& request, int status, const QString& tempFileLocation, const QString& filename );

public:

    // ------------------------------------------------------------------------
    // CTOR / DTOR
    // ------------------------------------------------------------------------

    /// Constructor.
    /**
     * @param[in,out] parent  parent object
     */
    edgifyapi( QObject *parent = nullptr );

    /// Destructor.
    virtual ~edgifyapi();

    // ------------------------------------------------------------------------
    // Properties
    // ------------------------------------------------------------------------

    /// Retrieve if API should block.
    /**
     * @return  @c true if blocking enabled, @c false otherwise
     */
    virtual bool isBlocking() const {return blocking_;}

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

public slots:

    // ------------------------------------------------------------------------
    // Methods
    // ------------------------------------------------------------------------

    /// Send request.
    /**
     * @param[in] url  url to download
     * @param[in] request  request
     * @param[in] timeout  >0 to for request timeout (ms)
     * @param[in] maxAttempts  number of attempts
     */
    virtual void send( const QUrl& url, const QJsonDocument& request, unsigned int timeout, unsigned int maxAttempts );

    /// Set if API should block.
    /**
     * @param[in] value  @c true to block on responses, @c false otherwise
     */
    virtual void setBlocking( bool value ) {blocking_ = value;}

protected:

    enum QueryResultParam
    {
        TIMESTAMP = 0,
        RETURNCODE,
        RETURNBYTES,
        RETRIES,
        TIMESPENT,
        STATUS
    };

    typedef QVector<QString> queryResult;

    unsigned int savedRequests_;

    bool blocking_;

    // ------------------------------------------------------------------------
    // Methods
    // ------------------------------------------------------------------------

    /// Write query results to file.
    virtual void logQueryToFile( const queryResult& query, const QString& filename ) const;

    /// Wait for all pending requests to complete.
    virtual void waitForPending() const;

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
    void onReplyReceived( QNetworkReply *reply, int replySize, bool valid, int statusCode, const QJsonDocument& request, const QJsonDocument& response, unsigned int elapsed );

private:

    struct requestControl
    {
        QDateTime start;
        QDateTime stop;

        unsigned int timeout;

        unsigned int maxAttempts;
        unsigned int attempts;

        QUrl url;
        QJsonDocument request;

        QString file;

        int status;
        QJsonDocument response;

        QString location;
    };

    typedef QMap<QNetworkReply*, requestControl> requestMap;

    mutable QMutex m_;

    requestMap pending_;

    /// Process request control.
    QNetworkReply *processRequestControl( const requestControl& rc );

    /// Create request control block.
    requestControl createDocumentRequestControl( const QUrl& url, const QJsonDocument& request, unsigned int timeout, unsigned int maxAttempts );

    /// Create request control block.
    requestControl createFileRequestControl( const QUrl& url, const QString& filename, const QJsonDocument& request = QJsonDocument() );

    /// Read request control block.
    requestControl readRequestControl( QNetworkReply *reply, bool& exists ) const;

    /// Write request control block.
    void writeRequestControl( QNetworkReply *reply, const requestControl& rc );

    /// Destroy request control block.
    void destroyRequestControl( QNetworkReply *reply );

    /// Number of pending requests.
    int numPendingRequests() const;

    // not implemented
    edgifyapi( const _Myt& ) = delete;

    // not implemented
    _Myt &operator = ( const _Myt& ) = delete;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // APIEDGIFY_H
