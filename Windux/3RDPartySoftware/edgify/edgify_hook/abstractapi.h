/**
 * @file abstractapi.h
 * Abstract API Interface.
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef ABSTRACTAPI_H
#define ABSTRACTAPI_H

#include <QByteArray>
#include <QDateTime>
#include <QJsonDocument>
#include <QMap>
#include <QMutex>
#include <QObject>

class QNetworkAccessManager;
class QNetworkReply;
class QString;
class QTimer;

///////////////////////////////////////////////////////////////////////////////////////////////////
class abstractapi : public QObject
{
    Q_OBJECT

    typedef abstractapi _Myt;
    typedef QObject _Mybase;

signals:

    /// Signal for reply download progress.
    /**
     * @param[in,out] reply  reply
     * @param[in] bytesReceived  bytes received
     * @param[in] bytesTotal  bytes total
     * @param[in] elapsed  time elapsed (ms)
     */
    void replyDownloadProgress( QNetworkReply *reply, qint64 bytesReceived, qint64 bytesTotal, unsigned int elapsed );

    /// Signal for reply received.
    /**
     * @param[in] reply  reply
     * @param[in] replySize  reply size
     * @param[in] valid  @c true if reply was valid, @c false otherwise
     * @param[in] statusCode  http status code
     * @param[in] request  request document
     * @param[in] response  response document
     * @param[in] elapsed  time elapsed (ms)
     */
    void replyReceived( QNetworkReply *reply, int replySize, bool valid, int statusCode, const QJsonDocument& request, const QJsonDocument& response, unsigned int elapsed );

    /// Signal for requests pending changed.
    /**
     * @param[in] pending  number of requests pending
     */
    void requestsPendingChanged( int pending );

public:

    /// Web headers map.
    typedef QMap<QByteArray, QByteArray> headersMap;

    // ------------------------------------------------------------------------
    // Properties
    // ------------------------------------------------------------------------

    /// Retrieve web headers.
    /**
     * @return  web headers
     */
    virtual headersMap headers() const {return headers_;}

    /// Retrieve network access manager.
    /**
     * @return  pointer to network access manager
     */
    virtual QNetworkAccessManager *networkAccessManager() {return networkAccess_;}

    /// Set web headers.
    /**
     * @param[in] value  web headers
     */
    virtual void setHeaders( const headersMap& value ) {headers_ = value;}

protected:

    QNetworkAccessManager *networkAccess_;          ///< Network access.

    headersMap headers_;                            ///< Headers.

    // ------------------------------------------------------------------------
    // CTOR / DTOR
    // ------------------------------------------------------------------------

    /// Constructor.
    /**
     * @param[in,out] parent  parent object
     */
    abstractapi( QObject *parent = nullptr );

    /// Destructor.
    virtual ~abstractapi();

    // ------------------------------------------------------------------------
    // Methods
    // ------------------------------------------------------------------------

    /// Delete resource.
    /**
     * @param[in] url  url
     * @param[in] blocking  @c true to block on reply, @c false otherwise
     * @param[in] timeout  >0 to for request timeout (ms)
     * @return  pointer to reply
     */
    virtual QNetworkReply *deleteResource( const QUrl& url, bool blocking = false, unsigned int timeout = 0 ) {
        return handleRequest( DELETE_RESOURCE, url, blocking, timeout );
    }

    /// Get resource.
    /**
     * @param[in] url  url
     * @param[in] blocking  @c true to block on reply, @c false otherwise
     * @param[in] timeout  >0 to for request timeout (ms)
     * @return  pointer to reply
     */
    virtual QNetworkReply *get( const QUrl& url, bool blocking = false, unsigned int timeout = 0 ) {
        return handleRequest( GET, url, blocking, timeout );
    }

    /// Post JSON document.
    /**
     * @param[in] url  url
     * @param[in] doc  JSON document
     * @param[in] blocking  @c true to block on reply, @c false otherwise
     * @param[in] timeout  >0 to for request timeout (ms)
     * @return  pointer to reply
     */
    virtual QNetworkReply *post( const QUrl& url, const QJsonDocument &doc, bool blocking = false, unsigned int timeout = 0 ) {
        return handleRequest( POST, url, blocking, timeout, doc );
    }

    /// Put JSON document.
    /**
     * @param[in] url  url
     * @param[in] doc  JSON document
     * @param[in] blocking  @c true to block on reply, @c false otherwise
     * @param[in] timeout  >0 to for request timeout (ms)
     * @return  pointer to reply
     */
    virtual QNetworkReply *put( const QUrl& url, const QJsonDocument &doc, bool blocking = false, unsigned int timeout = 0 ) {
        return handleRequest( PUT, url, blocking, timeout, doc );
    }

private slots:

    /// Slot for network reply download progress.
    void onDownloadProgress( qint64 bytesReceived, qint64 bytesTotal );

    /// Slot for network reply finished.
    void onFinished();

private:

    enum {DEFAULT_TIMEOUT = 5 * 60 * 1000}; // 5m

    enum method
    {
        DELETE_RESOURCE,
        GET,
        POST,
        PUT
    };

    struct requestControl
    {
        QDateTime start;
        QDateTime stop;

        QJsonDocument doc;

        QTimer *timeout;
    };

    typedef QMap<QNetworkReply*, requestControl> requestMap;

    mutable QMutex m_;

    requestMap pending_;

    /// Create request control block.
    void createRequestControl( QNetworkReply *reply, const QJsonDocument& doc, unsigned int timeout );

    /// Read request control block.
    requestControl readRequestControl( QNetworkReply *reply ) const;

    /// Destroy request control block.
    requestControl destroyRequestControl( QNetworkReply *reply );

    /// Handle network request.
    QNetworkReply *handleRequest( method m, const QUrl &url, bool blocking, unsigned int timeout, const QJsonDocument &doc = QJsonDocument() );

    /// Parse network reply.
    void parseNetworkReply( QNetworkReply *reply, bool &valid );

    /// Save document for debugging purposes.
    static void saveDocument( const QJsonDocument& doc, const QString& filename );

    // not implemented
    abstractapi( const _Myt& ) = delete;

    // not implemented
    _Myt &operator = ( const _Myt& ) = delete;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // ABSTRACTAPI_H
