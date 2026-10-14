/**
 * @file scaledb.h
 * Scale database management.
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef SCALEDB_H
#define SCALEDB_H

#include <QObject>

class QJsonArray;
class QJsonObject;
class QSqlDatabase;
class QSqlRecord;
class QSqlTableModel;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Scale database management.
class scaleDatabase : public QObject
{
    Q_OBJECT

    typedef scaleDatabase _Myt;
    typedef QObject _Mybase;

public:

    /// Product lookup result codes.
    enum ProductLookupResult
    {
        ProductNotOnFile,                           ///< Product not in database.
        ProductValid,                               ///< Product exists with valid expiry.
        ProductExpired,                             ///< Product exists but is expired (cache time too old).
    };

    /// Label name and version pair.
    typedef QPair<QString, QString> labelNameVersionPair;

    /// Mapping of label versions [labelno, labelnameversion]
    typedef QMap<int, labelNameVersionPair> labelVersionMap;

    // ------------------------------------------------------------------------
    // CTOR / DTOR
    // ------------------------------------------------------------------------

    /// Constructor.
    /**
     * @param[in] name  database name
     * @param[in] parent  parent object
     */
    scaleDatabase( const QString& name, QObject *parent = nullptr );

    /// Destructor.
    virtual ~scaleDatabase();

    // ------------------------------------------------------------------------
    // Properties
    // ------------------------------------------------------------------------

    /// Retrieve active product count.
    /**
     * @param[in] dept  department number
     * @return  number of active products
     */
    virtual int activeProductCount( int dept = 0 ) const;

    /// Check of valid.
    /**
     * @return  @c true if valid, @c false otherwise
     */
    virtual bool isValid() const;

    /// Retrieve label version information.
    /**
     * @param[in] dept  department number
     * @return  label information
     */
    virtual labelVersionMap labelVersionInfo( int dept = 0 ) const;

    /// Retrieve last communication date and time.
    /**
     * @return  date and time
     */
    virtual QDateTime lastComm() const;

    /// Retrieve last item change data and time.
    /**
     * @return  date and time
     */
    virtual QDateTime lastItemChange() const;

    /// Check for product existance.
    /**
     * @param[in] productNum  product number
     * @param[in] dept  department number
     * @return  lookup result
     */
    virtual ProductLookupResult productExists( int productNum, int dept = 0 ) const;

    /// Set last communication date and time.
    /**
     * @param[in] value  date and time
     */
    virtual void setLastComm( const QDateTime &value );

    /// Set last item change data and time.
    /**
     * @param[in] value  date and time
     */
    virtual void setLastItemChange( const QDateTime &value );

    // ------------------------------------------------------------------------
    // Methods
    // ------------------------------------------------------------------------

    /// Begin database transaction.
    virtual bool beginTransaction();

    /// Delete all product data.
    virtual void deleteAllProductData();

    /// Delete product.
    /**
     * @param[in] productNum  product number
     * @param[in] dept  department number
     * @return  @c true upon success, @c false otherwise
     */
    virtual bool deleteProduct( int productNum, int dept = 0 );

    /// End database transaction.
    virtual bool endTransaction();

    /// Insert/Update COOL Categories into database.
    /**
     * @param[in] category  cool category
     * @param[in] dept  department number
     * @return  @c true upon success, @c false otherwise
     */
    virtual bool insertUpdateCoolCategory( const QJsonObject& category, int dept = 0 );

    /// Insert/Update COOL Countries into database.
    /**
     * @param[in] country  cool country
     * @param[in] dept  department number
     * @return  @c true upon success, @c false otherwise
     */
    virtual bool insertUpdateCoolCountry( const QJsonObject& country, int dept = 0 );

    /// Insert/Update COOL Pre-Text into database.
    /**
     * @param[in] preText  cool pre-text
     * @param[in] dept  department number
     * @return  @c true upon success, @c false otherwise
     */
    virtual bool insertUpdateCoolPreText( const QJsonObject& preText, int dept = 0 );

    /// Insert/Update COOL Production Step into database.
    /**
     * @param[in] prodStep  cool production step
     * @param[in] dept  department number
     * @return  @c true upon success, @c false otherwise
     */
    virtual bool insertUpdateCoolProdStep( const QJsonObject& prodStep, int dept = 0 );

    /// Insert/Update COOL Text into database.
    /**
     * @param[in] text  cool text
     * @param[in] dept  department number
     * @return  @c true upon success, @c false otherwise
     */
    virtual bool insertUpdateCoolText( const QJsonObject& text, int dept = 0 );

    /// Insert/Update product into database.
    /**
     * @param[in] product  product data
     * @param[in] dept  department number
     * @return  @c true upon success, @c false otherwise
     */
    virtual bool insertUpdateProduct( const QJsonObject& product, int dept = 0 );

    /// Retrieve config value from database.
    /**
     * @param[in] configName  config name
     * @return  config value
     */
    virtual QString queryConfigs( const QString& configName ) const;

protected:

    QString dbname_;                                ///< Database name.

    QSqlDatabase *scaleDatabase_;                   ///< Scale database.

    int cacheExpirationTime_;                       ///< Legacy V1 Hook cache expiration time.
    QString cacheExpirationUnits_;                  ///< Legacy V1 Hook cache expiration units (i.e. 'h' or 'm' or 's')

    // ------------------------------------------------------------------------
    // Methods
    // ------------------------------------------------------------------------

    /// Close database.
    virtual void closeDatabase();

    /// Open database.
    /**
     * @return  @c true upon success, @c false otherwise
     */
    virtual bool openDatabase();

    /// Open database connection for thread.
    /**
     * @return  database connection
     */
    virtual QSqlDatabase openDatabaseConnection() const;

    /// Update config value from database.
    /**
     * @param[in] configName  config name
     * @param[in] configValue  config value
     * @return  @c true upon success, @c false otherwise
     */
    virtual bool updateConfigs( const QString& configName, const QString& configValue );

private:

    void readConfiguration();

    void PresentationMethod_verifyReservedRecord( int PMType, const QString& PMName1 );

    void TransactionType_verifyReservedRecord( int TTNum, const QString& TTName1, int TTAffPr, int TTBkDt, int TTAutoUPC, int TTManUPC, const QString& TTUPCDigit, int TTUseBgClr, int TTBgColor );

    void Class_verifyReservedRecord( int DeptNum, int ClNum, const QString& ClDesc1, int Cla_ClNum = -1 );

    void Category_verifyReservedRecord( int DeptNum, int CaNum, int Cat_CaNum, const QString& CaName1, const QString& CaName2 );

    void Special_verifyReservedRecord( int DeptNum, int SPNum, const QString& SPName1, const QString& SPImag1, const QString& SPImag2, int SPDefL );

    void Dept_verifyReservedRecord( int DeptNum, const QString& DeptName1, const QString& DeptName2 );

    // not implemented
    scaleDatabase() = delete;

    // not implemented
    scaleDatabase( const _Myt& ) = delete;

    // not implemented
    _Myt & operator = ( const _Myt& ) = delete;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // SCALEDB_H
