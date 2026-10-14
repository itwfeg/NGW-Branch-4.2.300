/**
 * @file hookwalmartbase.h
 * Walmart Base.
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef HOOKWALMARTBASE_H
#define HOOKWALMARTBASE_H

#include <QMap>
#include <QMutex>
#include <QObject>
#include <QPair>

class walmartapi;

class QTimer;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Walmart Base.
class walmartHookBase : public QObject
{
    Q_OBJECT

    typedef walmartHookBase _Myt;
    typedef QObject _Mybase;

public:


protected:

    /// Single product lookup results.
    enum SingleProductLookupResult
    {
        LookupSuccess,                              ///< Success.
        LookupSuccessNotOnFile,                     ///< Success, but item is not on file.
        LookupFailure,                              ///< Failure.
    };

    /// Label name and version pair.
    typedef QPair<QString, QString> labelNameVersionPair;

    /// Mapping of label versions [labelno, labelnameversion]
    typedef QMap<int, labelNameVersionPair> labelVersionMap;


    mutable QMutex m_;                              ///< Mutex for access control.

    walmartapi *api_;                               ///< Walmart API.

    QString initUrl_;                               ///< Url for init event.
    QString initScaleVendor_;                       ///< Scale vendor for init event.
    QString initScaleModel_;                        ///< Scale model for init event.

    QString businessUnitType_;                      ///< Business unit type for PLU event.

    int requestTimeout_;                            ///< Request timeout time (seconds).
    int requestNumRetries_;                         ///< Request number of retries.

    QString singleItemLookupUrl_;                   ///< Url for single item lookup.

    QString bannerCode_;                            ///< Banner code.
    QString deptOverride_;                          ///< Department number override.

    int singleItemLookupTimeout_;                   ///< Timeout time for single item lookup (milliseconds).
    int singleItemLookupNumRetries_;                ///< Number of retries for single item lookup.

    // ------------------------------------------------------------------------
    // CTOR / DTOR
    // ------------------------------------------------------------------------

    /// Constructor.
    /**
     * @param[in] parent  parent object
     */
    walmartHookBase( QObject *parent = nullptr );

    /// Destructor.
    virtual ~walmartHookBase();

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

    /// Retrieve store information.
    /**
     * @param[out] id  store id
     * @param[out] name  store name
     * @param[out] graphic  store graphic
     * @return  @c true upon success, @c false otherwise
     */
    virtual bool storeInformation( QString& id, QString& name, QString& graphic ) const = 0;

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
     * @param[in] dept  department number
     * @return  @c true upon success, @c false otherwise
     */
    virtual bool deleteProduct( int productNum, int dept = 0 ) = 0;

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

    /// Perform single product lookup.
    /**
     * @param[in] productNum  product number
     * @param[in] dept  department number
     * @param[out] obj  product information
     * @return  lookup result
     */
    virtual SingleProductLookupResult performSingleProductLookup( int productNum, int dept, QJsonObject& obj );

    /// Update communication time.
    virtual void updateCommTime() = 0;

    /// Update last online time.
    virtual void updateLastOnlineTime() = 0;

    // ------------------------------------------------------------------------
    // Static Properties
    // ------------------------------------------------------------------------

    /// Retrieve last product fetch stamp.
    /**
     * @return  stamp
     */
    static QString lastProductFetch();

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

    /// Handle event state.
    void handleEventState( const QString& uuid );

    /// Handle request state.
    void handleRequestState( const QString& uuid );

    /// Process event object.
    void processEvent( const QJsonObject& obj );

    /// Process metadata object.
    EventState processMetadata( const QString& uuid, const QJsonObject& obj, bool updateState = false );

    /// Load web headers.
    static void loadWebHeaders( const QString& f, walmartapi *api );

    /// Queue request for state machine.
    static bool queueRequest( const QUrl& url, const QJsonDocument& request, const QString& uuidParent = QString() );

    /// Retrieve event state.
    static EventState requestState( const QString& type, const QString& uuid );

    /// Set event state.
    static bool setRequestState( const QString& type, const QString& uuid, EventState state, const QString& errorText = QString(), BackoutResult br = BACKOUT_NOT_ATTEMPTED );

    /// Delete any stale and completed events.
    static void cleanupRequests( bool startup = false );

    /// Scale number from ip address.
    static QString scaleNumberFromIpAddress( const QString& addr );

    /// Load field mappings from file.
    static void loadMapping( const QString& filename, const QString& keyBase, QMap<QString, QString>& mapping );

    /// Determine item type from data.
    static int determineItemType( const QJsonObject& pluJsonObj );

    // not implemented
    walmartHookBase( const _Myt& ) = delete;

    // not implemented
    _Myt & operator = ( const _Myt& ) = delete;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // HOOKWALMARTBASE_H
