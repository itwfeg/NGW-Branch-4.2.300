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
#include <QMutex>

class QJsonArray;
class QJsonObject;
class QSqlDatabase;
class QSqlRecord;
class QSqlTableModel;

///////////////////////////////////////////////////////////////////////////////////////////////////

#define DBCONFIG_STOREID "storeId"
#define DBCONFIG_STORENAME "storeName"
#define DBCONFIG_STOREGRAPHICNAME "storeGraphicName"
#define DBCONFIG_DEPTNUMBER "mostRecentActiveDeptNum"
#define DBCONFIG_PRIMARYCOUNTRYCODE "primaryCountryCode"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Scale database management.
class scaleDatabase : public QObject
{
    Q_OBJECT

    typedef scaleDatabase _Myt;
    typedef QObject _Mybase;

public:

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

    /// Check of valid.
    /**
     * @return  @c true if valid, @c false otherwise
     */
    virtual bool isValid() const;

    /// Retrieve active product count.
    /**
     * @param[in] dept  department number
     * @return  number of active products
     */
    virtual int activeProductCount( int dept = 0 ) const;

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

    // ------------------------------------------------------------------------
    // Methods
    // ------------------------------------------------------------------------

    /// Delete all product data.
    virtual void deleteAllProductData();

    /// Delete product.
    /**
     * @param[in] productNum  product number
     * @param[in] dept  department number
     * @return  @c true upon success, @c false otherwise
     */
    virtual bool deleteProduct( int productNum, int dept = 0 );

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


    /// Open the Diagnostics database.
    /**
     * @return  pointer to the database if it's found and opened
     */
    static QSqlDatabase * openDiagDb();

    /// Retrieve labels printed from the Diagnostics database.
    /**
     * @return  number of labels printed
     */
    static qlonglong getLabelsPrinted();


protected:

    QString dbname_;                                ///< Database name.

    QSqlDatabase *scaleDatabase_;                   ///< Scale database.

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

private:

    mutable QMutex m_;

    static QMutex diagm_;

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
