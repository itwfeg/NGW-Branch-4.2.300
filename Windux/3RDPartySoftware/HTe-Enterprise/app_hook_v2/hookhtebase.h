/**
 * @file hookhtebase.h
 * HTe Base.
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef HOOKHTEBASE_H
#define HOOKHTEBASE_H

#include <QMap>
#include <QMutex>
#include <QObject>
#include <QPair>
#include <QUrl>

class hteapi;
class scaleDatabase;
class hteDatabase;

class QTimer;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// HTe Base.
class hteHookBase : public QObject
{
    Q_OBJECT

    typedef hteHookBase _Myt;
    typedef QObject _Mybase;

public:

    scaleDatabase * db() { return db_; }

    hteDatabase * htedb() { return htedb_; }


protected:

    /// Label name and version pair.
    typedef QPair<QString, QString> labelNameVersionPair;

    /// Mapping of label versions [labelno, labelnameversion]
    typedef QMap<int, labelNameVersionPair> labelVersionMap;


    mutable QMutex m_;                              ///< Mutex for access control.

    hteapi *api_;                               ///< HTe API.

    QString dbname_;                                ///< Database name.
    scaleDatabase *db_;                             ///< Database.
    hteDatabase *htedb_;                            ///< HTe Database, tracks list of downloaded files

    QString initUrl_;                               ///< Url for init event.
    QString initScaleVendor_;                       ///< Scale vendor for init event.
    QString initScaleModel_;                        ///< Scale model for init event.

    int requestTimeout_;                            ///< Request timeout time (seconds).
    int requestNumRetries_;                         ///< Request number of retries.

    // ------------------------------------------------------------------------
    // CTOR / DTOR
    // ------------------------------------------------------------------------

    /// Constructor.
    /**
     * @param[in] parent  parent object
     */
    hteHookBase( QObject *parent = nullptr );

    /// Destructor.
    virtual ~hteHookBase();

    // ------------------------------------------------------------------------
    // Properties
    // ------------------------------------------------------------------------

    /// Check if hook initialized and ready.
    /**
     * @return  @c true if ready, @c false otherwise
     */
    virtual bool isReady() const = 0;

    /// Retrieve label version information.
    /**
     * @return  label information
     */
    virtual labelVersionMap labelVersionInfo() const = 0;

    /// Retrieve last communication date and time.
    /**
     * @return  date and time
     */
    virtual QDateTime lastComm() const = 0;

    /// Retrieve last item change data and time.
    /**
     * @return  date and time
     */
    virtual QDateTime lastItemChange() const = 0;

    /// Retrieve product count.
    /**
     * @return  count of products
     */
    virtual int productCount() const = 0;

    /// Set processing products.
    /**
     * This property is invoked before and after products are handled.
     * @param[in] value  @c true before processing, @c false after processing
     */
    virtual void setProcessingProducts( bool value ) = 0;


    // ------------------------------------------------------------------------
    // Methods
    // ------------------------------------------------------------------------

    /// Delete all products.
    /**
     * @return  @c true upon success, @c false otherwise
     */
    virtual bool deleteAllProducts() = 0;

    /// Delete product.
    /**
     * @param[in] productNum  product number
     * @return  @c true upon success, @c false otherwise
     */
    virtual bool deleteProduct( int productNum ) = 0;

    /// Insert/Update COOL Categories into database.
    /**
     * @param[in] category  cool category
     * @return  @c true upon success, @c false otherwise
     */
    virtual bool insertUpdateCoolCategory( const QJsonObject& category ) = 0;

    /// Insert/Update COOL Countries into database.
    /**
     * @param[in] country  cool country
     * @return  @c true upon success, @c false otherwise
     */
    virtual bool insertUpdateCoolCountry( const QJsonObject& country ) = 0;

    /// Insert/Update COOL Pre-Texts into database.
    /**
     * @param[in] preText  cool pre-text
     * @return  @c true upon success, @c false otherwise
     */
    virtual bool insertUpdateCoolPreText( const QJsonObject& preText ) = 0;

    /// Insert/Update COOL Production Steps into database.
    /**
     * @param[in] prodStep  cool production step
     * @return  @c true upon success, @c false otherwise
     */
    virtual bool insertUpdateCoolProdStep( const QJsonObject& prodStep ) = 0;

    /// Insert/Update COOL Text into database.
    /**
     * @param[in] text  cool text
     * @return  @c true upon success, @c false otherwise
     */
    virtual bool insertUpdateCoolText( const QJsonObject& text ) = 0;

    /// Insert/Update product into database.
    /**
     * @param[in] product  product data
     * @return  @c true upon success, @c false otherwise
     */
    virtual bool insertUpdateProduct( const QJsonObject& product ) = 0;

    /// Perform health check.
    /**
     * @param[in] triggerType  trigger type
     * @param[in] force  @c true to force health check (even if disabled)
     */
    virtual void performHealthCheck( const QString& triggerType, bool force = false );

    /// Update last online time.
    virtual void updateLastOnlineTime() = 0;

    // ------------------------------------------------------------------------
    // Static Properties
    // ------------------------------------------------------------------------

    /// Retrieve last product fetch stamp.
    /**
     * @return  stamp
     */
    static QDateTime lastProductFetch();

    /// Set last product fetch stamp.
    /**
     * @param[in] value  stamp
     * @param[in] responseStamp  response message stamp
     */
    static void setLastProductFetch( const QJsonValue& value, const QJsonValue& responseStamp );

    // ------------------------------------------------------------------------
    // Static Methods
    // ------------------------------------------------------------------------

    /// Retreieve how many pending requests exist.
    /**
     * @return  number of pending requests
     */
    static unsigned int pendingRequests();

private slots:

    /// Slot for initialization.
    void onInitialize();

    /// Slot for document processing.
    void onProcessDocument( const QJsonDocument& request, int status, const QJsonDocument& response );

    /// Slot for file processing.
    void onProcessFile( const QJsonDocument& request, int status, const QString& tempFileLocation, const QString& uuid, const QString& filename );

    /// Slot for state machine processing.
    void onProcessStateMachine();

private:

    enum EventState
    {
        STATE_NONE,
        STATE_PENDING,
        STATE_COMPLETE,
        STATE_FAILED,
        STATE_PROCESSED
    };

    enum BackoutResult
    {
        BACKOUT_NOT_ATTEMPTED,
        BACKOUT_SUCCESS,
        BACKOUT_FAILED
    };

    typedef QPair<QString, QString> fileCopyInfo;

    typedef QPair<int, QString> triggerTypeInfo;
    typedef QMap<QString, triggerTypeInfo> triggerTypeInfoMap;

    typedef QMap<QString, QString> enabledEventConfigMap;

    typedef QPair<QString, QString> urlQueryParams;

    enum {INITIALIZE_INTERVAL = 30 * 1000};         // 30s
    enum {STATE_MACHINE_INTERVAL = 2 * 1000};       // 2s

    QTimer *startup_;
    QTimer *stateMachine_;

    triggerTypeInfoMap triggerTypes_;

    enabledEventConfigMap enabledEvents_;

    /// Load web headers.
    void loadWebHeaders();

    /// Handle event state.
    void handleEventState( const QString& uuid );

    /// Handle request state.
    void handleRequestState( const QString& uuid );

    /// Process event object.
    void processEvent( const QJsonObject& obj );

    /// Submit individual requests for each HTe download
    /// EventState setupHTeDownload( const QString& uuid, const QJsonObject& obj, bool updateState = false );

    /// Process HTe downloads
    EventState processHTeDownload( const QString& uuid, const QJsonObject& obj, bool updateState = false );

    /// Process metadata object.
    EventState processMetadata( const QString& uuid, const QJsonObject& obj, bool updateState = false );

    /// Queue request for state machine.
    static bool queueRequest( const QUrl& url, const QJsonDocument& request, const QString& uuidParent = QString(), const QUrl& backupUrl = QUrl() );

    /// Retrieve event state.
    static EventState requestState( const QString& uuid );

    /// Set event state.
    static bool setRequestState( const QString& uuid, EventState state, const QString& errorText = QString(), BackoutResult br = BACKOUT_NOT_ATTEMPTED );

    /// Delete any stale and completed events.
    static void cleanupRequests( bool startup = false );

    // not implemented
    hteHookBase( const _Myt& ) = delete;

    // not implemented
    _Myt & operator = ( const _Myt& ) = delete;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // HOOKHTEBASE_H
