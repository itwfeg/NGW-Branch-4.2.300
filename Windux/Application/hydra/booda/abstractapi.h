/**
 * @file abstractapi.h
 * Abstract API Interface.
 *
 * @author Randy Blankley
 */

#ifndef ABSTRACTAPI_H
#define ABSTRACTAPI_H

#include <QByteArray>
#include <QDateTime>
#include <QMap>
#include <QMutex>
#include <QObject>

class QNetworkAccessManager;
class QNetworkReply;
class QString;
class QTimer;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Abstract API Interface.
class AbstractWebInterface : public QObject
{
    Q_OBJECT

    using _Myt = AbstractWebInterface;
    using _Mybase = QObject;

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
     * @param[in,out] reply  reply
     * @param[in] valid  @c true if reply was valid, @c false otherwise
     * @param[in] statusCode  http status code
     * @param[in] content  response
     * @param[in] contentType  response type
     * @param[in] elapsed  time elapsed (ms)
     */
    void replyReceived( QNetworkReply *reply, bool valid, int statusCode, const QByteArray& content, const QString& contentType, unsigned int elapsed );

    /// Signal for requests pending changed.
    /**
     * @param[in] pending  number of requests pending
     */
    void requestsPendingChanged( int pending );

public:

    /// Web headers map.
    using HeadersMap = QMap<QByteArray, QByteArray>;

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve web headers.
    /**
     * @return  web headers
     */
    virtual HeadersMap headers() const {return headers_;}

    /// Retrieve network access manager.
    /**
     * @return  pointer to network access manager
     */
    virtual QNetworkAccessManager *networkAccessManager() {return networkAccess_;}

    /// Set web headers.
    /**
     * @param[in] value  web headers
     */
    virtual void setHeaders( const HeadersMap& value ) {headers_ = value;}

    /// Set network access manager.
    /**
     * @param[in] value  pointer to network access manager
     */
    virtual void setNetworkAccessManager( QNetworkAccessManager *value );

protected:

    QNetworkAccessManager *networkAccess_;          ///< Network access.

    HeadersMap headers_;                            ///< Headers.

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in,out] parent  parent object
     */
    AbstractWebInterface( QObject *parent = nullptr );

    /// Destructor.
    ~AbstractWebInterface();

    // ========================================================================
    // Methods
    // ========================================================================

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

    /// Post data.
    /**
     * @param[in] url  url
     * @param[in] content  request
     * @param[in] contentType  request type
     * @param[in] blocking  @c true to block on reply, @c false otherwise
     * @param[in] timeout  >0 to for request timeout (ms)
     * @return  pointer to reply
     */
    virtual QNetworkReply *post( const QUrl& url, const QByteArray &content, const QString& contentType, bool blocking = false, unsigned int timeout = 0 ) {
        return handleRequest( POST, url, blocking, timeout, content, contentType );
    }

    /// Put data.
    /**
     * @param[in] url  url
     * @param[in] content  request
     * @param[in] contentType  request type
     * @param[in] blocking  @c true to block on reply, @c false otherwise
     * @param[in] timeout  >0 to for request timeout (ms)
     * @return  pointer to reply
     */
    virtual QNetworkReply *put( const QUrl& url, const QByteArray &content, const QString& contentType, bool blocking = false, unsigned int timeout = 0 ) {
        return handleRequest( PUT, url, blocking, timeout, content, contentType );
    }

private slots:

    /// Slot for network reply download progress.
    void onDownloadProgress( qint64 bytesReceived, qint64 bytesTotal );

    /// Slot for network reply finished.
    void onFinished();

private:

    enum {DEFAULT_TIMEOUT = 5 * 60 * 1000}; // 5m

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

        QTimer *timeout;
    };

    using RequestMap = QMap<QNetworkReply*, RequestControl>;

    mutable QMutex m_;

    RequestMap pending_;

    /// Create request control block.
    void createRequestControl( QNetworkReply *reply, unsigned int timeout );

    /// Read request control block.
    RequestControl readRequestControl( QNetworkReply *reply ) const;

    /// Destroy request control block.
    RequestControl destroyRequestControl( QNetworkReply *reply );

    /// Handle network request.
    QNetworkReply *handleRequest( Method m, const QUrl &url, bool blocking, unsigned int timeout, const QByteArray& content = QByteArray(), const QString& contentType = QString() );

    /// Parse network reply.
    void parseNetworkReply( QNetworkReply *reply );

    /// Save content for debugging purposes.
    static void saveContent( const QByteArray& a, const QString& filename );

    // not implemented
    AbstractWebInterface( const _Myt& ) = delete;

    // not implemented
    AbstractWebInterface( const _Myt&& ) = delete;

    // not implemented
    _Myt& operator = ( const _Myt& ) = delete;

    // not implemented
    _Myt& operator = ( const _Myt&& ) = delete;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // ABSTRACTAPI_H
