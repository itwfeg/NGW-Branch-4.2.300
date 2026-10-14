/**
 * @file webviewernetwork.h
 * Network access manager for web viewer.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef WEBVIEWERNETWORK_H
#define WEBVIEWERNETWORK_H

#include <QNetworkAccessManager>
#include <QNetworkReply>
class QTimer;
#include <QNetworkReply>
class QTimer;
#include <QNetworkReply>
class QTimer;

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Request timeout object.
class requestTimeout : public QObject
{
    Q_OBJECT

    typedef requestTimeout _Myt;
    typedef QObject _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] reply  network reply
     * @param[in] parent  parent object
     */
    requestTimeout( QNetworkReply *reply, QObject *parent = 0 );

    /// Destructor.
    virtual ~requestTimeout();

private Q_SLOTS:

    /// Slot for reply destroyed.
    void onReplyDestroyed();

    /// Slot for reply download progress update.
    void onReplyDownloadProgress( qint64 bytesReceived, qint64 bytesTotal );

    /// Slot for reply finished.
    void onReplyFinished();

    /// Slot for reply upload progress update.
    void onReplyUploadProgress( qint64 bytesSent, qint64 bytesTotal );

    /// Slot for timeout.
    void onTimeout();

private:

    enum {REPLY_TIMEOUT = 30000};

    QNetworkReply *reply_;
    QTimer *timer_;

    qint64 bytesReceived_;
    qint64 bytesSent_;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Network access manager for web viewer.
/**
 * @warning
 * This implementation only handles the basic tool button with icon and text. If you want the
 * advanced features (i.e. dropdown menu) this class is not what you are looking for.
 */
class webViewerNetworkAccessManager : public QNetworkAccessManager
{
    Q_OBJECT

    typedef webViewerNetworkAccessManager _Myt;
    typedef QNetworkAccessManager _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent object
     */
    explicit webViewerNetworkAccessManager( QObject *parent = 0 );

    /// Destructor.
    virtual ~webViewerNetworkAccessManager();

protected:

    // ========================================================================
    // Methods
    // ========================================================================

    /// Handle a network request.
    /**
     * Returns a new @c QNetworkReply object to handle the operation @p op and request @p req. The
     * device @p outgoingData is always 0 for Get and Head requests, but is the value passed to
     * post() and put() in those operations (the QByteArray variants will pass a QBuffer object).
     *
     * The default implementation calls QNetworkCookieJar::cookiesForUrl() on the cookie jar set
     * with setCookieJar() to obtain the cookies to be sent to the remote server.
     *
     * The returned object must be in an open state.
     *
     * @param[in] op  operation
     * @param[in] request  network request
     * @param[in] outgoingData  outgoing data
     * @return  pointer to network reply
     */
    virtual QNetworkReply *createRequest( Operation op, const QNetworkRequest& request, QIODevice *outgoingData );

private Q_SLOTS:

    /// Slot to handle authentication required.
    void onAuthenticationRequired( QNetworkReply *reply, QAuthenticator *auth );

    /// Slot to handle proxy authentication required.
    void onProxyAuthenticationRequired( const QNetworkProxy &proxy, QAuthenticator *auth );

    /// Slot to reply finished.
    void onReplyFinished();

    /// Slot to handle SSL errors.
    void onSslErrors( QNetworkReply *reply, const QList<QSslError>& errors );

};

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

/** @} */

#endif // WEBVIEWERNETWORK_H
