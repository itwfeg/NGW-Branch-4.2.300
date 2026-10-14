/**
 * @file productrec.h
 * Product record support.
 *
 * @copyright Copyright (c) 2021 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef PRODUCTREC_H
#define PRODUCTREC_H

#include <QMap>
#include <QMutex>
#include <QObject>
#include <QJsonObject>
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QString>

class QSqlTableModel;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Product record support.
class productRecord : public QObject
{
    typedef productRecord _Myt;
    typedef QObject _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] obj  product data
     * @param[in] dept  product department
     * @param[in] parent  parent object
     * @param[in] db  database
     */
    productRecord( const QJsonObject& obj, int dept = 0, QObject *parent = nullptr, QSqlDatabase db = QSqlDatabase() );

    /// Deconstructor.
    virtual ~productRecord();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Check of valid.
    /**
     * @return  @c true if valid, @c false otherwise
     */
    virtual bool isValid() const {return valid_;}

    // ========================================================================
    // Methods
    // ========================================================================

    /// Insert/Update record.
    /**
     * @return  @c true if successful, @c false otherwise
     */
    virtual bool insertUpdate();

protected:

    QSqlDatabase db_;                               ///< Database.

    bool valid_;                                    ///< Valid data exists to insert/update product.

    int dept_;                                      ///< Product department.

    int64_t productNum_;                            ///< Product number.
    int productType_;                               ///< Product type.

    QJsonObject obj_;                               ///< Product object.

    QSqlRecord prdef_;                              ///< Product default.

    // ========================================================================
    // Methods
    // ========================================================================

    /// Create product record using product defaults.
    /**
     * @param[in,out] prod  product to create
     */
    virtual void createProductFromProductDefault( QSqlRecord& prod ) const;

    /// Determine next cool list number.
    /**
     * @return  next number
     */
    virtual int64_t determineNextCoolListNum() const;

    /// Determine next nutrition data number.
    /**
     * @param[in] startingNum  starting number
     * @return  next number
     */
    virtual int64_t determineNextNutrifactNum( int64_t startingNum ) const;

    /// Determine next product id.
    /**
     * @return  next id
     */
    virtual int64_t determineNextPluID();

    /// Determine next product text number.
    /**
     * @param[in] expTextType  expanded text type
     * @param[in] startingNum  starting number
     * @return  next number
     */
    virtual int64_t determineNextProdTextNum( int expTextType, int64_t startingNum ) const;

    /// Populate COOL information.
    /**
     * @param[in,out] prod  product to populate
     * @return  @c true upon success, @c false otherwise
     */
    virtual bool populateCool( QSqlRecord& prod ) const;

    /// Populate nutrition data.
    /**
     * @param[in,out] prod  product to populate
     * @param[in] prodExists  @c true if product existed, @c false if created
     * @return  @c true upon success, @c false otherwise
     */
    virtual bool populateNutritionData( QSqlRecord& prod, bool prodExists ) const;

    /// Populate product info.
    /**
     * @param[in,out] prod  product to populate
     */
    virtual void populateProductData( QSqlRecord& prod ) const;

    /// Populate texts.
    /**
     * @param[in,out] prod  product to populate
     * @param[in] prodExists  @c true if product existed, @c false if created
     * @return  @c true upon success, @c false otherwise
     */
    virtual bool populateTexts( QSqlRecord& prod, bool prodExists ) const;

    /// Show product record.
    /**
     * @param[in] prod  product record to show
     */
    virtual void showProductRec( const QSqlRecord& prod ) const;

    /// Update blocked products file.
    /**
     * @return  @c true upon success, @c false otherwise
     */
    virtual bool updateBlockedProducts() const;

private:

    /// Map of string to string.
    typedef QMap<QString, QString> stringMap;

    /// Map of string to int.
    typedef QMap<QString, int> intMap;

    static QMutex fieldMappingMutex_;               ///< Mutex to control field mapping
    static stringMap fieldMapping_;                 ///< Mapping of product fields

    static stringMap prodDefMapping_;               ///< Mapping of product defaults
    static stringMap expTextMapping_;               ///< Mapping of expanded text fields
    static stringMap coolMapping_;                  ///< Mapping of cool fields

    static intMap productTypeMap_;                  ///< Product types.
    static intMap expTextTypeMap_;                  ///< Expanded text types.

    static intMap conversionMultiplierMap_;         ///< Fields to convert from double.

    static intMap labelPlacements_;                 ///< Label placements.
    static intMap labelRotations_;                  ///< Label rotations.

    /// Create category linked record.
    bool createListLinkedRecords( const QString& table, int64_t coolListNum, const QJsonArray& a ) const;

    /// Load data mapping from file.
    static void loadMapping( const QString& filename, const QString& keyBase, QMap<QString, QString>& mapping );

    /// Check if tag is label placement.
    static bool isLabelPlacement( const QString& tag );

    /// Check if tag is label rotation.
    static bool isLabelRotation( const QString& tag );

    // not implemented
    productRecord() = delete;

    // not implemented
    productRecord( const _Myt& ) = delete;

    // not implemented
    _Myt& operator = ( const _Myt& ) = delete;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // PRODUCTREC_H
