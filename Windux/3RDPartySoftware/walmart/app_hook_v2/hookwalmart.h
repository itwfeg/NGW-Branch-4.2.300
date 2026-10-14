/**
 * @file hookwalmart.h
 * Walmart Hook Interface.
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef HOOKWALMART_H
#define HOOKWALMART_H

#include "hookwalmartbase.h"

class networkCheckingThread;
class scaleDatabase;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Walmart Hook Interface.
class walmartHook : public walmartHookBase
{
    Q_OBJECT

    typedef walmartHook _Myt;
    typedef walmartHookBase _Mybase;

public:

    // ------------------------------------------------------------------------
    // Methods
    // ------------------------------------------------------------------------

    /// Initialize hook.
    /**
     * @return  @c true upon success, @c false otherwise
     */
    bool initialize();

    /// Finalize hook.
    void finalize();

    /// Config saved hook.
    void configSaved();

    /// Initialize database name attribute.
    /**
     * @param[in] dbname  database name
     */
    void beforeOpenDatabase( const QString& dbname );

    /// Open the database.
    void openDatabase();

    /// Read PLU record hook.
    /**
     * @param[in] plu  plu number
     * @param[in] deptNum  department number
     * @return  @c true to have chobart_d use the plu record in the scale's database, @c false otherwise
     */
    bool beforePluRead( const QString& plu, const QString& deptNum );

    /// Read PLU Text record hook.
    /**
     * @param[in] number  text number
     * @param[in] deptNum  department number
     * @param[in] type  text type
     * @return  @c true to continue, @c false otherwise
     */
    bool beforePluTextRead( const QString& number, const QString& deptNum, int type );

    /// Print button hook.
    /**
     * @param[in] plu  plu number
     * @param[in] deptNum  department number
     * @param[in] isRemotePrint  @c true if remote printing, @c false otherwise
     * @return  @c true to continue, @c false otherwise
     */
    bool beforePrint( const QString& plu,  const QString& deptNum, bool isRemotePrint = false );

    /// After print hook.
    /**
     * @param[in] success  @c true if print was successful, @c false otherwise
     * @param[in] numLabelsToPrint  number of labels to print
     * @param[in] numLabelsPrinted  number of labels printed
     * @param[in] templateLabel  @c true if template (cake) label, @c false otherwise
     */
    void afterPrint( bool success, int numLabelsToPrint, int numLabelsPrinted, bool templateLabel = false );

    /// Perform custom hook action.
    /**
     * @param[in] commandString  action to perform
     * @param[out] result  action result
     * @param[out] resultExtended  action result extended
     */
    void customAction( const QString& commandString, int& result, QString& resultExtended );

    /// Update periphal device version info.
    /**
     * @param[in] device  device
     * @param[in] deviceEx  extended device
     * @param[in] kind  device kind
     * @param[in] swVer  software version
     * @param[in] hwVer  hardware version
     */
    void updatePeripheralVersion( int device, int deviceEx, int kind, const QString& swVer, const QString& hwVer );

    /// Update scale state.
    /**
     * @param[in] type  scale state
     * @param[in] value  value
     */
    void updateScaleState( int type, const QString& value );

    // ------------------------------------------------------------------------
    // Static Methods
    // ------------------------------------------------------------------------

    /// Retrieve global instance.
    /**
     * @return  pointer to instance
     */
    static _Myt *instance();

    /// Destroy global instance.
    static void destroy();

protected:

    /// Server lookup rules for v1 hook.
    enum ServerLookupRule
    {
        ServerLookupDisabled = 0,                   ///< Disable PLU lookup on Server all of the time, only look locally.
        ServerLookupIfNoLocalCopy,                  ///< Perform PLU lookup on Server only when missing from local Database.
        ServerLookupIfNoLocalCopyOrExpired,         ///< Perform PLU lookup on Server when missing from local Database or cache expired.
        ServerLookupAlways,                         ///< Perform PLU lookup on Server all the time.
        ServerLookupAlwaysThenUseCache,             ///< Perform PLU lookup on Server all the time, use local cache if server not available.
    };

    QString dbname_;                                ///< Database name.
    scaleDatabase *db_;                             ///< Database.

    QString singlePluLoadCommand_;                  ///< Command to load single product.
    QString singlePluLoadWithDeleteCommand_;        ///< Command to load single product after delete.
    QString healthCheckCommand_;                    ///< Command to perform health check.

    QString printingPlu_;                           ///< PLU for printing.
    QString printingDeptNum_;                       ///< Dept Number for printing.
    bool printingIsRemotePrint_;                    ///< Printing PLU is remote print.

    bool lockout_;                                  ///< Scale in lockout mode.

    int shutdownMessageDialogTime_;                 ///< How long to wait for shutdown message before showing dialog.
    int shutdownMessageMaxWaitTime_;                ///< How long to wait for shutdown message to complete.

    ServerLookupRule serverLookupRule_;             ///< Legacy V1 Hook single plu lookup rule.
    bool removeItemsFromCache_;                     ///< Legacy V1 Hook remove items from cache flag.

    int minRelookupTime_;                           ///< Legacy V1 Hook minimum re-lookup time.

    // ------------------------------------------------------------------------
    // Properties
    // ------------------------------------------------------------------------

    /// Check if hook initialized and ready.
    /**
     * @return  @c true if ready, @c false otherwise
     */
    bool isReady() const;

    /// Retrieve label version information.
    /**
     * @return  label information
     */
    labelVersionMap labelVersionInfo() const;

    /// Retrieve last communication date and time.
    /**
     * @return  date and time
     */
    QDateTime lastComm() const;

    /// Retrieve last item change data and time.
    /**
     * @return  date and time
     */
    QDateTime lastItemChange() const;

    /// Retrieve product count.
    /**
     * @return  count of products
     */
    int productCount() const;

    /// Set processing products.
    /**
     * This property is invoked before and after products are handled.
     * @param[in] value  @c true before processing, @c false after processing
     */
    void setProcessingProducts( bool value );

    /// Retrieve store information.
    /**
     * @param[out] id  store id
     * @param[out] name  store name
     * @param[out] graphic  store graphic
     * @return  @c true upon success, @c false otherwise
     */
    bool storeInformation( QString& id, QString& name, QString& graphic ) const;

    // ------------------------------------------------------------------------
    // Methods
    // ------------------------------------------------------------------------

    /// Close the database.
    void closeDatabase();

    /// Delete all products.
    /**
     * @return  @c true upon success, @c false otherwise
     */
    bool deleteAllProducts();

    /// Delete product.
    /**
     * @param[in] productNum  product number
     * @param[in] dept  department number
     * @return  @c true upon success, @c false otherwise
     */
    bool deleteProduct( int productNum, int dept = 0 );

    /// Fetch single product from server.
    /**
     * @param[in] productNum  product number
     * @param[in] dept  department number
     * @return  @c true upon success, @c false otherwise
     */
    bool fetchSingleProduct( int productNum, int dept = 0 );

    /// Insert/Update COOL Categories into database.
    /**
     * @param[in] category  cool category
     * @return  @c true upon success, @c false otherwise
     */
    bool insertUpdateCoolCategory( const QJsonObject& category );

    /// Insert/Update COOL Countries into database.
    /**
     * @param[in] country  cool country
     * @return  @c true upon success, @c false otherwise
     */
    bool insertUpdateCoolCountry( const QJsonObject& country );

    /// Insert/Update COOL Pre-Texts into database.
    /**
     * @param[in] preText  cool pre-text
     * @return  @c true upon success, @c false otherwise
     */
    bool insertUpdateCoolPreText( const QJsonObject& preText );

    /// Insert/Update COOL Production Steps into database.
    /**
     * @param[in] prodStep  cool production step
     * @return  @c true upon success, @c false otherwise
     */
    bool insertUpdateCoolProdStep( const QJsonObject& prodStep );

    /// Insert/Update COOL Text into database.
    /**
     * @param[in] text  cool text
     * @return  @c true upon success, @c false otherwise
     */
    bool insertUpdateCoolText( const QJsonObject& text );

    /// Insert/Update product into database.
    /**
     * @param[in] product  product data
     * @return  @c true upon success, @c false otherwise
     */
    bool insertUpdateProduct( const QJsonObject& product );

    /// Read configuration.
    void readConfiguration();

    /// Update communication time.
    void updateCommTime();

    /// Update last online time.
    void updateLastOnlineTime();

private slots:

    /// Slot for online status changed.
    void onOnlineStatusChanged( bool online, int offlineAmount );

private:

    enum {DEFAULT_SHUTDOWN_MSG_MAX_WAIT = 30000}; // 30s

    /// Scale device information.
    enum ScaleDevice
    {
        WEIGHER_DEVICE = 2,
        CUSTOMER_DISPLAY_DEVICE = 161
    };

    /// Scale state information.
    enum ScaleState
    {
        SCALE_BUILD_NUMBER = 0,
        SCALE_MODE,
        SCALE_MODE_VALUE,
        SCALE_MODE_VALUE_DESCRIPTION,
        SCALE_ITEM_ENTRY_TYPE,
        SCALE_LABEL_STOCK_1,
        SCALE_LABEL_STOCK_2,
        SCALE_LABEL_STOCK_3,
        SCALE_WEIGHER_PRECISION,
        SCALE_WEIGHER_CALIB_STAMP,
        SCALE_WEIGHER_CONF_STAMP
    };

    typedef QPair<int, int> dialogStartStopTime;

    static const char *version_;

    static QMutex instanceMutex_;
    static _Myt *instance_;

    networkCheckingThread *net_;

    typedef QPair<int, int> productNumber;
    typedef QMap<productNumber, qint64> lookupTimesMap;

    lookupTimesMap lookupTimes;

    /// Constructor.
    walmartHook( QObject *parent = nullptr );

    /// Destructor.
    ~walmartHook();

    /// Lookup offline dialog times.
    static bool lookupOfflineDialogTimes( int& max, dialogStartStopTime& d0, dialogStartStopTime& d1, int& fade );

    // not implemented
    walmartHook( const _Myt& ) = delete;

    // not implemented
    _Myt & operator = ( const _Myt& ) = delete;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // HOOKWALMART_H
