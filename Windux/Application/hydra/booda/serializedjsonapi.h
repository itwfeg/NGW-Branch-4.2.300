/**
 * @file serializedjsonapi.h
 * Serialized JSON API Interface.
 *
 * @author Randy Blankley
 */

#ifndef SERIALIZEDJSONAPI_H
#define SERIALIZEDJSONAPI_H

#include "serializedapi.h"

class QJsonDocument;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Serialized Json API Interface.
class SerializedJsonWebInterface : public SerializedWebInterface
{
    Q_OBJECT

    using _Myt = SerializedJsonWebInterface;
    using _Mybase = SerializedWebInterface;

signals:

    /// Signal for document processing.
    /**
     * @param[in] uuid  request id
     * @param[in] request  request
     * @param[in] requestType  request type
     * @param[in] status  request status
     * @param[in] response  response
     */
    void processDocumentJson( const QUuid& uuid, const QByteArray& request, const QString& requestType, int status, const QJsonDocument& response );

public slots:

    // ========================================================================
    // Methods
    // ========================================================================

    /// Download file.
    /**
     * @param[in] uuid  uuid
     * @param[in] url  url to download
     */
    virtual void downloadFile( const QUuid& uuid, const QUrl& url ) override {
        _Mybase::downloadFile( uuid, url );
    }

    /// Download file.
    /**
     * @param[in] uuid  uuid
     * @param[in] url  url to download
     * @param[in] request  request
     */
    virtual void downloadFile( const QUuid& uuid, const QUrl& url, const QJsonDocument& request );

    /// Download file.
    /**
     * @param[in] uuid  uuid
     * @param[in] url  url to download
     * @param[in] request  request
     * @param[in] requestType  request type
     */
    virtual void downloadFile( const QUuid& uuid, const QUrl& url, const QByteArray& request, const QString& requestType ) override {
        _Mybase::downloadFile( uuid, url, request, requestType );
    }

    /// Send request.
    /**
     * @param[in] uuid  uuid
     * @param[in] url  url to download
     * @param[in] timeout  >0 to for request timeout (ms)
     * @param[in] maxAttempts  number of attempts
     */
    virtual void send( const QUuid& uuid, const QUrl& url, unsigned int timeout, unsigned int maxAttempts ) override {
        _Mybase::send( uuid, url, timeout, maxAttempts );
    }

    /// Send request.
    /**
     * @param[in] uuid  uuid
     * @param[in] url  url to download
     * @param[in] request  request
     * @param[in] timeout  >0 to for request timeout (ms)
     * @param[in] maxAttempts  number of attempts
     */
    virtual void send( const QUuid& uuid, const QUrl& url, const QJsonDocument& request, unsigned int timeout, unsigned int maxAttempts );

    /// Send request.
    /**
     * @param[in] uuid  uuid
     * @param[in] url  url to download
     * @param[in] request  request
     * @param[in] requestType  request type
     * @param[in] timeout  >0 to for request timeout (ms)
     * @param[in] maxAttempts  number of attempts
     */
    virtual void send( const QUuid& uuid, const QUrl& url, const QByteArray& request, const QString& requestType, unsigned int timeout, unsigned int maxAttempts ) override {
        _Mybase::send( uuid, url, request, requestType, timeout, maxAttempts );
    }

    /// Upload resource.
    /**
     * @param[in] uuid  uuid
     * @param[in] url  url to download
     * @param[in] request  request
     * @param[in] timeout  >0 to for request timeout (ms)
     * @param[in] maxAttempts  number of attempts
     */
    virtual void upload( const QUuid& uuid, const QUrl& url, const QJsonDocument& request, unsigned int timeout, unsigned int maxAttempts );

    /// Upload resource.
    /**
     * @param[in] uuid  uuid
     * @param[in] url  url to download
     * @param[in] request  request
     * @param[in] requestType  request type
     * @param[in] timeout  >0 to for request timeout (ms)
     * @param[in] maxAttempts  number of attempts
     */
    virtual void upload( const QUuid& uuid, const QUrl& url, const QByteArray& request, const QString& requestType, unsigned int timeout, unsigned int maxAttempts ) override {
        _Mybase::upload( uuid, url, request, requestType, timeout, maxAttempts );
    }

protected:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in,out] parent  parent object
     */
    SerializedJsonWebInterface( QObject *parent = nullptr );

    /// Destructor.
    ~SerializedJsonWebInterface();

    // ========================================================================
    // Methods
    // ========================================================================

    /// Handle process document.
    /**
     * @param[in] uuid  request id
     * @param[in] request  request
     * @param[in] requestType  request type
     * @param[in] status  request status
     * @param[in] response  response
     * @param[in] responseType  response type
     */
    virtual void handleProcessDocument( const QUuid& uuid, const QByteArray& request, const QString& requestType, int status, const QByteArray& response, const QString& responseType ) override;

private:

    /// Save document for debugging purposes.
    static void saveDocument( const QJsonDocument& content, const QString& filename );

    // not implemented
    SerializedJsonWebInterface( const _Myt& ) = delete;

    // not implemented
    SerializedJsonWebInterface( const _Myt&& ) = delete;

    // not implemented
    _Myt& operator = ( const _Myt& ) = delete;

    // not implemented
    _Myt& operator = ( const _Myt&& ) = delete;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // SERIALIZEDJSONAPI_H
