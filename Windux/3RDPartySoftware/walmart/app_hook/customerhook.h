#ifndef CUSTOMERHOOK_H
#define CUSTOMERHOOK_H

#include <QDateTime>
#include <QFuture>
#include <QMap>
#include <QMutex>
#include <QObject>
#include <QPair>
#include <QVariant>

class healthStatusThread;
class networkCheckingThread;
class walmartapi;

class QJsonDocument;
class QSqlDatabase;
class QSqlTableModel;
class QSqlRecord;
class QString;
class QThread;

/// Customer Hook Singleton
class customerHook : public QObject
{
    Q_OBJECT

    typedef customerHook _Myt;
    typedef QObject _Mybase;

public:

    /// Label name and version pair.
    typedef QPair<QString, QString> labelNameVersionPair;

    /// Mapping of label versions [labelno, labelnameversion]
    typedef QMap<int, labelNameVersionPair> labelVersionMap;

    // ------------------------------------------------------------------------
    // Properties
    // ------------------------------------------------------------------------

    /// Retrieve country.
    /**
     * @return  country
     */
    QString country() const;

    /// Set country.
    /**
     * @param[in] value  country
     */
    void setCountry( const QString &value );

    /// Retrieve storenum_.
    /**
     * @return  storenum
     */
    QString storeNum() const;

    /// Set store number.
    /**
     * @param[in] value  storenum
     */
    void setStoreNum( const QString &value );

    /// Retrieve model.
    /**
     * @return  model
     */
    QString model() const;

    /// Set model.
    /**
     * @param[in] value  model
     */
    void setModel( const QString &value );

    /// Retrieve serial number.
    /**
     * @return  serialnumber
     */
    QString serialNum() const;

    /// Set serialnumber.
    /**
     * @param[in] value  serialnumber
     */
    void setSerialNum( const QString &value );

    /// Retrieve IP address.
    /**
     * @return  ip address
     */
    QString ipAddress() const;

    /// Set IP address.
    /**
     * @param[in] value  ip address
     */
    void setIpAddress( const QString& value );

    /// Retrieve MAC Address.
    /**
     * @return  mac address
     */
    QString macAddress() const;

    /// Set MAC address.
    /**
     * @param[in] value  mac address
     */
    void setMacAddress( const QString& value );

    /// Retrieve active PLU count.
    int activePluCount();

    /// Retrieve label version information.
    /**
     * @param[in] dept  department number
     * return  mapping of labels
     */
    labelVersionMap labelVersionInfo( int dept = 0 );

    /// Retrieve Walmart Canada Banner.
    /**
     * @return  banner
     */
    QString walmartCanadaBanner() const {return walmartCanadaBanner_;}

    /// Retrieve Walmart Canada Maker.
    /**
     * @return  maker
     */
    QString walmartCanadaMaker() const {return walmartCanadaMaker_;}

    // ------------------------------------------------------------------------
    // Methods
    // ------------------------------------------------------------------------

    /// Initialize hook.
    /**
     * @return  @c true upon success, @c false otherwise
     */
    bool init();

    /// Config saved hook.
    void configSaved();

    /// Initialize database name attribute.
    /**
     * @param[in] dbname  database name
     */
    void beforeOpenDatabase( const QString& dbname );

    /// Open the database.
    /**
     */
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
     * @param[in] success  @c true if print was successfull, @c false otherwise
     * @param[in] numLabelsToPrint  number of labels to print
     * @param[in] numLabelsPrinted  number of labels printed
     */
    void afterPrint( bool success, int numLabelsToPrint, int numLabelsPrinted );

    /// Perform custom hook action.
    /**
     * @param[in] commandString  action to perform
     * @param[out] result  action result
     * @param[out] resultExtended  action result extended
     */
    void customAction( const QString& commandString, int& result, QString& resultExtended );

    /// Perform health check.
    /**
     * @param[in] info  health check information
     * @param[out] response  server response
     * @return  @c true upon success, @c false otherwise
     */
    bool healthCheck( const QJsonDocument& info, QJsonDocument& response );

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

    // ------------------------------------------------------------------------
    // Properties
    // ------------------------------------------------------------------------

    /// Retrieve api for thread.
    /**
     * @return  @c walmartapi pointer
     */
    walmartapi *api();

    // ------------------------------------------------------------------------
    // Methods
    // ------------------------------------------------------------------------

    void closeDatabase();

    void readAFieldMapping(QMap<QString, QString> &theMap, const QString nameBase);

private:

    // I started out using these rules but then I started thinking that this may be
    // more difficult for the user, so I broke them out into 4 different configurations
    // But in the end I pulled them back in because using them was cleaner.
    enum FindPluOnServerRules
    {
        ServerLookupDisabled,               // SOF-3785 Disable PLU lookup on Server all of the time. Only look locally
        ServerLookupIfNoLocalCopy,          // SOF-3786 Perform PLU lookup on Server only when missing from local Database
        ServerLookupIfNotInDBOrExpired,     // SOF-3787 Perform PLU lookup on Server when missing from local Database or cache expired
        ServerLookupAlways,                 // SOF-3788 Perfrom PLU lookup on Server all the time
        ServerLookupAlwaysThenUseCache,     // SOF-4634 Perfrom PLU lookup on Server all the time, use local cache if server not available
    };

    struct pluPrint
    {
        QDateTime stamp;
        QString plu;
        QString deptNum;
        bool isRemotePrint;
        int counter;
    };

    typedef QMap<QThread*, walmartapi*> apiMap;

    typedef QList<pluPrint> pluPrintingList;

    static const char *version_;

    static QMutex instanceMutex_;
    static _Myt *instance_;

    mutable QMutex m_;

    apiMap api_;

    QSqlDatabase *scaleDatabase_;
    QString dbname_;

    QString country_;                               ///< Country
    QString storenum_;                              ///< Store Number

    QString model_;                                 ///< Scale model
    QString serialnumber_;                          ///< Scale Serial Number

    QString ipAddress_;                             ///< Scale ip address.
    QString macAddress_;                            ///< Scale MAC Address

    QString cacheexpire_;                           ///< Cache Expiration Time
    QString cacheexpireunits_;                      ///< Cache Expiration Time Units
    int serverLookupRule_;                          ///< Rule indicating what rule we follow to search the server for a PLU

    bool removeitemsfromcache_;                     ///< Whether or not to remove the items from cache
    QString minRelookupTime_;                       ///< The number of milliseconds before we go ask the Walmart server for the same plu again
    QString retriableErrors_;                       ///< string of SQLite error codes, separated by a colon, that can be retried if submitAll fails
    bool verboseDebug_;                             ///< additional debugging flag
    QString fullPluLoadCommand_;                    ///< Name of custom action to do full plu load
    QString fullPluLoadWithDeleteCommand_;          ///< Name of custom action to delete all PLU data and then do full plu load
    QString singlePluLoadCommand_;                  ///< Name of custom action to do single plu load
    QString singlePluLoadWithDeleteCommand_;        ///< Name of custom action to delete a PLU and then do single plu load

    int connectionCheckInterval_;                   ///< Connection Check Interval (milliseconds)
    networkCheckingThread *net_;

    typedef QMap<QString, QString> prDefsFieldsMap;
    prDefsFieldsMap prDefsFields_;

    typedef QMap<QString, QString> pluFieldsMap;
    pluFieldsMap pluFields_;

    typedef QMap<QString, QString> nutriFieldsMap;
    nutriFieldsMap nutriFields_;

    typedef QMap<QString, QString> expandedTextFieldsMap;
    expandedTextFieldsMap expandedTextFields_;

    QPair<int, int> offlinePrintDialog1;
    QPair<int, int> offlinePrintDialog2;

    QPair<int, int> offlineVerifyDialog1;
    int offlineVerifyDialog1_Freq;

    QPair<int, int> offlineVerifyDialog2;
    int offlineVerifyDialog2_Freq;

    int messageFadeTime;

    bool ignoreAllSslErrors_;
    bool ignoreSslHostNameMismatch_;
    bool ignoreSslSelfSignedCert_;

    bool lockout_;

    int healthCheckInterval_;
    QString healthCheckFormat_;
    healthStatusThread *health_;

    QString walmartCanadaMaker_;
    QString walmartCanadaBanner_;

    QFuture<void> databaseLoadBackgroundProcess_;

    pluPrintingList printInfo_;

    // Constructor.
    customerHook( QObject *parent = nullptr );

    // Destructor.
    ~customerHook();

    // not implemented
    _Myt & operator = ( const _Myt& ) = delete;

    bool updateDbWithPlu(const QString& deptNum, QJsonObject pluJsonObj);
    bool deletePluFromDb(const QString& plu, const QString& deptNum);
    bool saveRecordToDatabase( const QString recordName, bool dbRecExists, QSqlTableModel *dbRM, QSqlRecord &dbRec );
    bool saveToDatabase( const QString recordName, QSqlTableModel *dbRM );
    bool canErrorBeRetried( QString err );
    void createProductFromProductDefault( QSqlRecord &dbProductRec, QSqlRecord &dbProductDefaultsRec );
    void updatePluRecord( const QJsonObject &pluJsonObj, QSqlRecord &dbProductRec, QSqlRecord &dbTextRec, QSqlRecord &dbAllergensRec, QSqlRecord &dbExpText2Rec, QSqlRecord &dbNutrRec );
    bool updateField( QSqlRecord &dbRec, const QString& dbFieldName, const QString& newValue, bool forceSet = false, bool useLatin1 = false );
    void updateNutrField( QSqlRecord &dbNutrRec, QString dbFieldName, QString newValue );
    bool isCacheTimeExpired(qint64 pluRecExpiryTime);
    int determineItemType( const QJsonObject &pluJsonObj );
    bool isPriceField(const QString key);
    bool isWeightField(const QString key);
    bool isServerPriceField(const QString& key);
    bool isServerWeightField(const QString& key);
    bool isNutrifactColField(const QString& key);
    void printJsonData( const QJsonObject &pluJsonObj);
    void updatePriceWeightFields( QJsonObject &pluJsonObj);

    int updateDbWithMultiplePlus( const QString& deptNum, const QJsonArray& pluJsonObjs );
    void getAllProducts( QString deptNum, bool deleteProducts ); // this method is threaded, pass params by copy!

    void deleteSupportRecords( QSqlRecord &dbProductRec );
    void deleteSupportRecord( QSqlRecord &dbProductRec, const QString productTextType, const QString productFieldName, const QString deptNum );
    void deleteProductTextRecord( const QString ProductTextType, const QString supportNumber, const QString deptNum );
    void deleteNutrRecord( const QString supportNumber, const QString deptNum );

    qlonglong determineNextPluID();
    long determineNextProdTextNum(long startingNum, long deptNum);
    long determineNextNFNum(long startingNum, long deptNum);

    void showProductRec( QSqlRecord &dbProductRec );

    void deleteAllPluData();
    void deleteFromTable(QString tableName);
    void PresentationMethod_verifyReservedRecord(int PMType, QString PMName1);
    void TransactionType_verifyReservedRecord(int TTNum, QString TTName1, int TTAffPr, int TTBkDt, int TTAutoUPC, int TTManUPC, QString TTUPCDigit, int TTUseBgClr, int TTBgColor);
    void Class_verifyReservedRecord(int DeptNum, int ClNum, QString ClDesc1, int Cla_ClNum = -1);
    void Category_verifyReservedRecord(int DeptNum, int CaNum, int Cat_CaNum, QString CaName1, QString CaName2);
    void Special_verifyReservedRecord(int DeptNum, int SPNum, QString SPName1, QString SPImag1, QString SPImag2, int SPDefL);
    void Dept_verifyReservedRecord(int DeptNum, QString DeptName1, QString DeptName2);


    /// Read in all of the static headers and assign them to a MAP for later usage
    void readConfigurationHeaders(walmartapi *face);
    void readConfiguration();

    QString queryConfigs( const QString& configName);

    bool querySerialNumber();

    /// Retrieve cache expiration time.
    /**
     * @return  cacheexpire
     */
    QString cacheExpiration() const {return cacheexpire_;}

    /// Set cacheexpire
    /**
     * @param[in] value  cacheexpire
     */
    void setCacheExpiration( const QString &value ) {cacheexpire_ = value;}

    /// Retrieve cache expiration units.
    /**
     * @return cacheexpireunits
     */
    QString cacheExpirationUnits() const {return cacheexpireunits_;}

    /// Set cacheexpireunits
    /**
     * @param[in] value  cacheexpireunits (h = hours, m = minutes, s = seconds)
     */
    void setCacheExpirationUnits( const QString &value ) {cacheexpireunits_ = value;}

    /**
     * @return removeitemsfromcache
     */
    bool RemoveItemsFromCache() const {return removeitemsfromcache_;}

    /// Set removeitemsfromcache
    /**
     * @param[in] value  cacheexpire
     */
    void setRemoveItemsFromCache( const QString &value ) {removeitemsfromcache_ = QVariant(value).toBool();}

    /// Retrieve connection check interval.
    int connectionCheckInterval() const {return connectionCheckInterval_;}

    /// Set connection check interval.
    void setConnectionCheckInterval( int value ) {connectionCheckInterval_ = value;}

    /// Retrieve health check interval.
    int healthCheckInterval() const {return healthCheckInterval_;}

    /// Set health check interval.
    void setHealthCheckInterval( int value ) {healthCheckInterval_ = value;}

    /// Retrieve health check format.
    QString healthCheckFormat() const {return healthCheckFormat_;}

    /// Set health check format.
    void setHealthCheckFormat( const QString& value ) {healthCheckFormat_ = value;}

    /// Parse scale number from ip address.
    static QString scaleNumberFromIpAddress( const QString& addr );

    /**
     * @param[in] value  dbRecExists exists in local scale DB
     * returns true if the PLU record exists in the local scale DB and the serverLookupRule says skip server lookup
     */
    bool testServerLookupRules(qint64 pluRecExpiryTime, bool dbRecordExists);

    /**
     * returns true if the time in the cache_expiration field in the PLU returns true
     */
    bool isCacheTimeExpired(QSqlRecord dbProductRec);

};

// ------------------------------------------------------------------------
// Notes
// ------------------------------------------------------------------------
//
// List of Headers
// TimeoutLength             via static - read from file = 2000ms   (Section 11: same as Connection timeout milliseconds
// TimeoutRetryInterval      via static - read from file = 3        (Section 11: same as Retries before cache usage?)
// SCALE_TYPE                via static - read from file = HOBART
// Correlation_ID            via random - auto generated
// SCALE_ID                  via static - read from flash = 45-1151-653
// SCALE_NUMBER              via OS     - last octet ip  = 129
// IP_ADDRESS                via OS     - OS param   = 10.13.3.7
// WM_CONSUMER.ID            via static - read from file = 45344
// WM_SVC.NAME               via static - read from file = scalelookup-service
// WM_SVC.VERSION            via static - read from file = 604
// WM_SVC.ENV                via static - read from file = Jasper
// WebServiceAddress         via static - read from file = iphost
// CacheExpirationHours      via static - read from file = 24
// HttpsActive               via static - read from file = 0
// MessageFadeTime           via static - read from file = 5
// RemoveItemsFromCache      via static - read from file = false


///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // CUSTOMERHOOK_H
