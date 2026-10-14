/**
 * @file serializedapi.h
 * API with serialized requests and responses.
 *
 * @author Randy Blankley
 */

#ifndef SERIALIZEDAPI_H
#define SERIALIZEDAPI_H

#include "abstractapi.h"

#include <QUrl>
#include <QUuid>

///////////////////////////////////////////////////////////////////////////////////////////////////

/// API with serialized requests and responses.
class SerializedWebInterface : public AbstractWebInterface
{
    Q_OBJECT

    using _Myt = SerializedWebInterface;
    using _Mybase = AbstractWebInterface;

signals:

    /// Signal for document processing.
    /**
     * @param[in] uuid  request id
     * @param[in] request  request
     * @param[in] requestType  request type
     * @param[in] status  request status
     * @param[in] response  response
     * @param[in] responseType  response type
     */
    void processDocument( const QUuid& uuid, const QByteArray& request, const QString& requestType, int status, const QByteArray& response, const QString& responseType );

    /// Signal for file processing.
    /**
     * @param[in] uuid  request id
     * @param[in] request  request
     * @param[in] requestType  request type
     * @param[in] status  request status
     * @param[in] filename  response
     */
    void processFile( const QUuid& uuid, const QByteArray& request, const QString& requestType, int status, const QString& filename );

public:

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve if API should block.
    /**
     * @return  @c true if blocking enabled, @c false otherwise
     */
    virtual bool isBlocking() const {return blocking_;}

public slots:

    // ========================================================================
    // Properties
    // ========================================================================

    /// Set if API should block.
    /**
     * @param[in] value  @c true to block on responses, @c false otherwise
     */
    virtual void setBlocking( bool value ) {blocking_ = value;}

    // ========================================================================
    // Methods
    // ========================================================================

    /// Download file.
    /**
     * @param[in] uuid  uuid
     * @param[in] url  url to download
     */
    virtual void downloadFile( const QUuid& uuid, const QUrl& url );

    /// Download file.
    /**
     * @param[in] uuid  uuid
     * @param[in] url  url to download
     * @param[in] request  request
     * @param[in] requestType  request type
     */
    virtual void downloadFile( const QUuid& uuid, const QUrl& url, const QByteArray& request, const QString& requestType );

    /// Remove (delete) resource.
    /**
     * @param[in] uuid  uuid
     * @param[in] url  url to remove
     * @param[in] timeout  >0 to for request timeout (ms)
     * @param[in] maxAttempts  number of attempts
     */
    virtual void remove( const QUuid& uuid, const QUrl& url, unsigned int timeout, unsigned int maxAttempts );

    /// Send request.
    /**
     * @param[in] uuid  uuid
     * @param[in] url  url to download
     * @param[in] timeout  >0 to for request timeout (ms)
     * @param[in] maxAttempts  number of attempts
     */
    virtual void send( const QUuid& uuid, const QUrl& url, unsigned int timeout, unsigned int maxAttempts );

    /// Send request.
    /**
     * @param[in] uuid  uuid
     * @param[in] url  url to download
     * @param[in] request  request
     * @param[in] requestType  request type
     * @param[in] timeout  >0 to for request timeout (ms)
     * @param[in] maxAttempts  number of attempts
     */
    virtual void send( const QUuid& uuid, const QUrl& url, const QByteArray& request, const QString& requestType, unsigned int timeout, unsigned int maxAttempts );

    /// Upload resource.
    /**
     * @param[in] uuid  uuid
     * @param[in] url  url to download
     * @param[in] request  request
     * @param[in] requestType  request type
     * @param[in] timeout  >0 to for request timeout (ms)
     * @param[in] maxAttempts  number of attempts
     */
    virtual void upload( const QUuid& uuid, const QUrl& url, const QByteArray& request, const QString& requestType, unsigned int timeout, unsigned int maxAttempts );

protected:

    bool blocking_;                                 ///< Blocking enabled.

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in,out] parent  parent object
     */
    SerializedWebInterface( QObject *parent = nullptr );

    /// Destructor.
    ~SerializedWebInterface();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Check if should retry request.
    /**
     * Default implementation returns @c true in all cases.
     * @param[in] uuid  uuid
     * @param[in] request  request
     * @param[in] requestType  request type
     * @param[in] status  request status
     * @return  @c true to retry, @c false to not retry
     */
    virtual bool retryRequest( const QUuid& uuid, const QByteArray& request, const QString& requestType, int status ) const;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Handle process document.
    /**
     * Default implementation emits a signal.
     * @param[in] uuid  request id
     * @param[in] request  request
     * @param[in] requestType  request type
     * @param[in] status  request status
     * @param[in] response  response
     * @param[in] responseType  response type
     */
    virtual void handleProcessDocument( const QUuid& uuid, const QByteArray& request, const QString& requestType, int status, const QByteArray& response, const QString& responseType );

    /// Wait for response.
    /**
     * @param[in] uuid  uuid to wait for
     */
    virtual void waitForResponse( const QUuid& uuid ) const;

private slots:

    /// Slot for reply download progress.
    void onReplyDownloadProgress( QNetworkReply *reply, qint64 bytesReceived, qint64 bytesTotal, unsigned int elapsed );

    /// Slot for reply received.
    void onReplyReceived( QNetworkReply *reply, bool valid, int statusCode, const QByteArray& content, const QString& contentType, unsigned int elapsed );

private:

    enum Method
    {
        DELETE_RESOURCE,
        GET,
        POST,
        PUT
    };

    struct RequestControl
    {
        QDateTime start;
        QDateTime stop;

        unsigned int timeout;

        unsigned int maxAttempts;
        unsigned int attempts;

        QUuid uuid;
        QUrl url;

        QByteArray request;
        QString requestType;

        Method method;
        bool file;

        int status;

        QString location;
    };

    using RequestMap = QMap<QNetworkReply*, RequestControl>;

    mutable QMutex m_;

    RequestMap pending_;

    /// Process request control.
    QNetworkReply *processRequestControl( const RequestControl& rc );

    /// Create request control block.
    RequestControl createDocumentRequestControl( const QUuid& uuid, const QUrl& url, Method m, const QByteArray& request, const QString& requestType, unsigned int timeout, unsigned int maxAttempts );

    /// Create request control block.
    RequestControl createFileRequestControl( const QUuid& uuid, const QUrl& url, Method m, const QByteArray& request, const QString& requestType );

    /// Read request control block.
    RequestControl readRequestControl( QNetworkReply *reply ) const;

    /// Write request control block.
    void writeRequestControl( QNetworkReply *reply, const RequestControl& rc );

    /// Destroy request control block.
    void destroyRequestControl( QNetworkReply *reply );

    /// Check request control for uuid exists or not.
    bool requestControlExists( const QUuid& uuid ) const;

    // not implemented
    SerializedWebInterface( const _Myt& ) = delete;

    // not implemented
    SerializedWebInterface( const _Myt&& ) = delete;

    // not implemented
    _Myt& operator = ( const _Myt& ) = delete;

    // not implemented
    _Myt& operator = ( const _Myt&& ) = delete;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // SERIALIZEDAPI_H
