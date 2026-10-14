/**
 * @file apiwalmart.h
 * Walmart API.
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef APIWALMART_H
#define APIWALMART_H

#include <QJsonDocument>
#include <QMutex>
#include <QNetworkRequest>
#include <QObject>
#include <QString>

class QDateTime;
class QNetworkAccessManager;
class QNetworkReply;
class QUrl;
class QTimer;

///////////////////////////////////////////////////////////////////////////////////////////////////
class walmartapi : public QObject
{
    Q_OBJECT

    typedef walmartapi _Myt;
    typedef QObject _Mybase;

signals:

    /// Signal for requests pending changed.
    void requestsPendingChanged( int pending );

public:

    // ========================================================================
    // Properties
    // ========================================================================

    /// Constructor.
    /**
     * @param[in,out] parent  parent object
     */
    walmartapi( QObject *parent = 0 );

    /// Destructor.
    ~walmartapi();

    // ========================================================================
    // Properties
    // ========================================================================

    /// request return status
    enum RequestStatus
    {
        SUCCESS,            // 200      - successfully received server data
        NODATA,             // 400 - no data available so......
        CACHEDATA           // 204, 404, 417 - use available cache data, timeouts
    };

    /// Retrieve host.
    /**
     * @return  host
     */
    QString host() const {return host_;}

    /// Set host.
    /**
     * @param[in] value  host
     */
    void setHost( const QString &value ) {host_ = value;}

    /// Retrieve SSL enabled.
    /**
     * @return  @c true if ssl enabled, @c false otherwise
     */
    bool ssl() const {return ssl_;}

    /// Set SSL enabled.
    /**
     * @param[in] value  ssl enabled
     */
    void setSSL( bool value ) {ssl_ = value;}

    /// Retrieve value of name value pair.
    /**
     * @return value at key
     */
    QByteArray httpHeadersMap( const QString& key ) const;

    /// Set name+value pairs of headers.
    /**
     * @param[in, in] key value QMap key, QMap value
     */
    void setHttpHeadersMap( const QString& key, const QString& value, bool custom = false );

    /// Retrieve endpoint.
    /**
     * @return  endpoint path
     */
    QString endpointPath( const QString& key ) const;

    /// Set endpoint.
    /**
     * @param[in] key  endpoint name
     * @param[in] value  endpoint path
     */
    void setEndpointPath( const QString& key, const QString& value );

    /// Return use cache response when offline.
    /**
     * @return  @c true to use cache when offline, @c false otherwise
     */
    bool useCacheWhenOffline() const {return useCacheWhenOffline_;}

    /// Set use cache response when offline.
    /**
     * @param[in] value  @c true to use cache when offline, @c false otherwise
     */
    void setUseCacheWhenOffline( bool value ) {useCacheWhenOffline_ = value;}

    /// Retrieve network access manager.
    /**
     * @return  pointer to network access manager
     */
    QNetworkAccessManager *networkAccessManager() {return networkAccess_;}

    // ========================================================================
    // Static Methods
    // ========================================================================

    /// Import last usage data.
    /**
     * @param[out] stamp  last usage timestamp
     * @param[out] api  las usage endpoint information
     * @return  @c true upon success, @c false otherwise
     */
    static bool importLastUsage( QDateTime& stamp, QString& api );

public slots:

    // ========================================================================
    // Methods
    // ========================================================================

    /// Retrieve one item.
    /**
     * @param[in] plu  product
     * @param[in] country  country code
     * @param[in] store  store number
     * @param[in] dept  dept number
     * @param[out] response  plu
     * @return  @c return code enum type
     */
    RequestStatus getOneItem( const QString& plu, const QString& country, const QString& store, const QString& dept, QJsonDocument &response );

    /// Retrieve all items.
    /**
     * @param[in] country  country code
     * @param[in] store  store number
     * @param[in] dept  dept number
     * @param[out] response  plu
     * @return  @c return code enum type
     */
    RequestStatus getAllItems( const QString& country, const QString& store, const QString& dept, QJsonDocument &response );

    /// Perform health check request.
    /**
     * @param[in] country  country code
     * @param[in] store  store number
     * @param[in] dept  dept number
     * @param[in] info  health check info
     * @param[out] response  health check response
     * @return  @c return code enum type
     */
    RequestStatus healthCheck( const QString& country, const QString& store, const QString& dept, const QJsonDocument &info, QJsonDocument &response );

protected:

    /// Endpoints.
    enum endpoint
    {
        API_GET_ONE_ITEM,                           ///< Get one item.
        API_GET_ALL_ITEMS,                          ///< Get all items.
        API_HEALTH_CHECK,                           ///< Health check status reporting.
    };

    QNetworkAccessManager *networkAccess_;          ///< Network access.

    bool ssl_;                                      ///< Use SSL for host.
    QString host_;                                  ///< Host.

    QStringList headers_;                           ///< Headers.
    QStringList customHeaders_;                     ///< Custom Headers.

    bool useCacheWhenOffline_;                      ///< Use cache when offline.

    // ========================================================================
    // Methods
    // ========================================================================

    /// Delete resource.
    /**
     * @param[in] e  endpoint
     * @param[in] blocking  @c true to block on reply, @c false otherwise
     * @return  pointer to reply
     */
    QNetworkReply *deleteResource( endpoint e, bool blocking = false ) {
        return handleRequest( DELETE, createUrl( e ), blocking );
    }

    /// Delete resource at path.
    /**
     * @param[in] e  endpoint
     * @param[in] path  path
     * @param[in] blocking  @c true to block on reply, @c false otherwise
     * @return  pointer to reply
     */
    QNetworkReply *deleteResourceX( endpoint e, const QString& path, bool blocking = false ) {
        return handleRequest( DELETE, createUrlPath( e, path ), blocking );
    }

    /// Get resource.
    /**
     * @param[in] e  endpoint
     * @param[in] query  query
     * @param[in] blocking  @c true to block on reply, @c false otherwise
     * @return  pointer to reply
     */
    QNetworkReply *get( endpoint e, const QString &query = QString(), bool blocking = false ) {
        return handleRequest( GET, createUrl( e, query ), blocking );
    }

    /// Get resource at path.
    /**
     * @param[in] e  endpoint
     * @param[in] path  path
     * @param[in] query  query
     * @param[in] blocking  @c true to block on reply, @c false otherwise
     * @return  pointer to reply
     */
    QNetworkReply *getX( endpoint e, const QString& path, const QString &query = QString(), bool blocking = false ) {
        return handleRequest( GET, createUrlPath( e, path, query ), blocking );
    }

    /// Post JSON document.
    /**
     * @param[in] e  endpoint
     * @param[in] doc  JSON document
     * @param[in] blocking  @c true to block on reply, @c false otherwise
     * @return  pointer to reply
     */
    QNetworkReply *post( endpoint e, const QJsonDocument &doc, bool blocking = false ) {
        return handleRequest( POST, createUrl( e ), blocking, doc );
    }

    /// Post JSON document at path.
    /**
     * @param[in] e  endpoint
     * @param[in] path  path
     * @param[in] doc  JSON document
     * @param[in] blocking  @c true to block on reply, @c false otherwise
     * @return  pointer to reply
     */
    QNetworkReply *postX( endpoint e, const QString& path, const QJsonDocument &doc, bool blocking = false ) {
        return handleRequest( POST, createUrlPath( e, path ), blocking, doc );
    }

    /// Put JSON document.
    /**
     * @param[in] e  endpoint
     * @param[in] doc  JSON document
     * @param[in] blocking  @c true to block on reply, @c false otherwise
     * @return  pointer to reply
     */
    QNetworkReply *put( endpoint e, const QJsonDocument &doc, bool blocking = false ) {
        return handleRequest( PUT, createUrl( e ), blocking, doc );
    }

    /// Put JSON document at path.
    /**
     * @param[in] e  endpoint
     * @param[in] path  path
     * @param[in] doc  JSON document
     * @param[in] blocking  @c true to block on reply, @c false otherwise
     * @return  pointer to reply
     */
    QNetworkReply *putX( endpoint e, const QString& path, const QJsonDocument &doc, bool blocking = false ) {
        return handleRequest( PUT, createUrlPath( e, path ), blocking, doc );
    }

private slots:

    /// Slot for network reply finished.
    void onFinished();

    /// Slot for timeout.
    void onTimeoutHit();

private:

    static QMutex lastUsage_;

    static int uuidCtr_;

    enum RequestResults
    {
        TIMESTAMP,
        PLU,
        UUID,
        RETURNCODE,
        RETURNBYTES,
        RETRIES,
        TIMESPENT,
        COMMENTS
    };

    enum method
    {
        DELETE,
        GET,
        POST,
        PUT
    };

    QMap<endpoint, QString> paths_;
    QMap<QString, endpoint> pathKeys_;

    typedef QMap<QString, QByteArray> headerMap;
    headerMap webheaders_;

    typedef QMap<RequestResults, QString> requestsMap;
    requestsMap reqResults_;

    QMap<QString, QString> urlParams_;

    int pending_;
    int howManyRequests_;
    int requestTimeout_;
    int allItemsRequestTimeout_;
    int savedRequests_;

    QString bannerCode_;
    int deptOverride_;
    bool noQueryParams_;

    QTimer *timer_;

    /// Create URL.
    QUrl createUrl( endpoint e, const QString &query = QString() ) const;

    /// Create URL with path.
    QUrl createUrlPath( endpoint e, const QString &path, const QString &query = QString() ) const;

    /// Check for URL match.
    bool isUrlMatch( const QUrl &url, endpoint e ) const;

    /// Handle network request.
    QNetworkReply *handleRequest( method m, const QUrl &url, bool blocking, const QJsonDocument &doc = QJsonDocument() );

    /// Parse network reply.
    QJsonDocument parseNetworkReply(QNetworkReply *reply, RequestStatus &retStatus , int &replyLen, int &retCode);

    /// Save document for debugging purposes.
    void saveDocument( const QJsonDocument &doc, const QString &filename );

    /// Generate the unique id for the transaction and set it to the map
    QString generateUniqueId( int storeNo, uint64_t epoch_secs );

    /// write query results to file
    void logQueryToFile(const QString &filename);

    /// determine what error codes we need to retry contact
    bool exitRetries(int code);

    /// find out the return code results
    QString statusCodeLookup(int code);

    /// Export last usage data with current timestamp.
    static void exportLastUsage( const QString& api );

    // not implemented
    walmartapi( const _Myt &other );

    // not implemented
    _Myt &operator = ( const _Myt &rhs );
};

// ------------------------------------------------------------------------
// Notes
// ------------------------------------------------------------------------
//
// List of Headers
// TimeoutLength             via static - read from file = 2000ms   (Section 11: same as Connection timeout milliseconds
// TimeoutRetryInterval      via static - read from file = 3        (Section 11: same as Retries before cache usage?)
// SCALE_TYPE                via static - read from file = HOBART
// Correlation_ID            via random - auto generated by Hobart, Walmart official one: 0272cdc8-1b18-4ec1-932f-3e0904acf7ef
// SCALE_ID                  via static - read from flash = 45-1151-653
// SCALE_NUMBER              via OS     - last octet ip  = 129
// IP_ADDRESS                via OS     - OS param   = 10.13.3.7
// WM_CONSUMER.ID            via static - read from file = 45344
// WM_SVC.NAME               via static - read from file = scalelookup-service
// WM_SVC.VERSION            via static - read from file = 604
// WM_SVC.ENV                via static - read from file = 714
// WebServiceAddress         via static - read from file = iphost
// CacheExpirationHours      via static - read from file = 24
// HttpsActive               via static - read from file = 0
// MessageFadeTime           via static - read from file = 5
// RemoveItemsFromCache      via static - read from file = false


///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // APIWALMART_H
