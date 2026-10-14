/**
 * @file productrec.h
 *
 * @copyright Copyright (c) 2021 Hobart Corporation
 * @author Randy Blankley
 */

#include "common.h"
#include "dbutil.h"
#include "nutrifactsparser.h"
#include "productrec.h"
#include "scaleblockedplus.h"
#include "stringsjson.h"

#include <QDateTime>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QSettings>
#include <QSqlField>
#include <QSqlTableModel>

#if defined(Q_OS_LINUX)
static const QString INTERNAL_CONFIG( "/usr/share/walmart-apphook/hobart_internal.config" );
#elif defined(Q_OS_WIN32)
static const QString INTERNAL_CONFIG( "hobart_internal.config" );
#endif

QMutex productRecord::fieldMappingMutex_( QMutex::Recursive );
productRecord::stringMap productRecord::fieldMapping_;

productRecord::stringMap productRecord::prodDefMapping_;
productRecord::stringMap productRecord::expTextMapping_;
productRecord::stringMap productRecord::coolMapping_;

productRecord::intMap productRecord::productTypeMap_;
productRecord::intMap productRecord::expTextTypeMap_;

productRecord::intMap productRecord::conversionMultiplierMap_;

productRecord::intMap productRecord::labelPlacements_;
productRecord::intMap productRecord::labelRotations_;

///////////////////////////////////////////////////////////////////////////////////////////////////
productRecord::productRecord( const QJsonObject& obj, int dept, QObject *parent, QSqlDatabase db ) :
    _Mybase( parent ),
    db_( db ),
    valid_( false ),
    dept_( dept ),
    productNum_( 0 ),
    productType_( 0 ),
    obj_( obj )
{
    QMutexLocker guard( &fieldMappingMutex_ );

    // load mappings
    if ( !fieldMapping_.size() )
    {
        loadMapping( INTERNAL_CONFIG, "plufield", fieldMapping_ );
        loadMapping( INTERNAL_CONFIG, "prdefsfield", prodDefMapping_ );
        loadMapping( INTERNAL_CONFIG, "exptxtfield", expTextMapping_ );
        loadMapping( INTERNAL_CONFIG, "coolfield", coolMapping_ );

        productTypeMap_["RANDOM_WEIGHT"] = 1;
        productTypeMap_["FIXED_WEIGHT"] = 2;
        productTypeMap_["BY_COUNT"] = 3;
        productTypeMap_["FLUID_OUNCES"] = 4;

        expTextTypeMap_["TxNum"] = 1;
        expTextTypeMap_["SMNum"] = 2;
        expTextTypeMap_["NtNum"] = 3;
        expTextTypeMap_["MaNum"] = 4;
        expTextTypeMap_["ProdTxt5Num"] = 5;
        expTextTypeMap_["ProdTxt6Num"] = 6;
        expTextTypeMap_["ProdTxt7Num"] = 7;
        expTextTypeMap_["ProdTxt8Num"] = 8;
        expTextTypeMap_["ProdTxt9Num"] = 9;

        // price fields
        conversionMultiplierMap_["PrPrice"] = 100;
        conversionMultiplierMap_["PrExcPr"] = 100;
        conversionMultiplierMap_["PrTotalPriceMin"] = 100;
        conversionMultiplierMap_["PrTotalPriceMax"] = 100;

        // weight fields
        conversionMultiplierMap_["PrTare"] = 1000;
        conversionMultiplierMap_["PrTareB"] = 1000;
        conversionMultiplierMap_["PrWeightMin"] = 1000;
        conversionMultiplierMap_["PrWeightMax"] = 1000;

        // label placements
        labelPlacements_["RIGHT_SIDE"] = 0;
        labelPlacements_["LEFT_SIDE"] = 1;

        // label rotations
        labelRotations_["ZERO"] = 0;
        labelRotations_["FORTY_FIVE"] = 45;
        labelRotations_["NINETY"] = 90;
        labelRotations_["ONE_THIRTY_FIVE"] = 135;
        labelRotations_["ONE_EIGHTY"] = 180;
        labelRotations_["TWO_TWENTY_FIVE"] = 225;
        labelRotations_["TWO_SEVENTY"] = 270;
        labelRotations_["THREE_FIFTEEN"] = 315;
    }

    // parse product information
    if (( obj.contains( JSON_PRODUCT_NUM ) ) && ( obj[JSON_PRODUCT_NUM].isDouble() ))
        productNum_ = obj[JSON_PRODUCT_NUM].toInt();

    if ( obj.contains( JSON_PRODUCT_TYPE ) )
    {
        if ( obj[JSON_PRODUCT_TYPE].isDouble() )
            productType_ = obj[JSON_PRODUCT_TYPE].toInt();
        else if ( obj[JSON_PRODUCT_TYPE].isString() )
        {
            const QString key( obj[JSON_PRODUCT_TYPE].toString() );

            if ( productTypeMap_.contains( key ) )
                productType_ = productTypeMap_[key];
        }
    }

    LOG_TRACE << "dept " << dept_ << " type " << productType_ << " num " << productNum_;

    // check validity
    // fetch product default record
    if (( 0 < productType_ ) && ( 0 < productNum_ ))
    {
        const QString filter( "PDType=" + QString::number( productType_ ) + " and DeptNum=" + QString::number( dept_ ) );

        // setup table models
        QSqlTableModel prdefsModel( nullptr, db_ );
        prdefsModel.setTable( "PrDefs" );
        prdefsModel.setEditStrategy( QSqlTableModel::OnManualSubmit );

        // check for existance of product default
        valid_ = dbutil::selectRecord( prdefsModel, filter, prdef_ );

        LOG_DEBUG << "prdef valid " << valid_;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
productRecord::~productRecord()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool productRecord::insertUpdate()
{
    QSqlTableModel prodsModel( nullptr, db_ );
    prodsModel.setTable( "Prods" );
    prodsModel.setEditStrategy( QSqlTableModel::OnManualSubmit );

    const QString filter( "PrNum=" + QString::number( productNum_ ) + " and DeptNum=" + QString::number( dept_ ) );

    // check for product record
    QSqlRecord prod;

    bool prodExists( false );

    if ( dbutil::selectRecord( prodsModel, filter, prod ) )
    {
        LOG_DEBUG << "product " << productNum_ << " exists ";
        prodExists = true;
    }
    else
    {
        LOG_DEBUG << "product " << productNum_ << " does not exist ";
        prod = prodsModel.record();

        // generate new id
        const int64_t id( determineNextPluID() );

        if ( id < 0 )
        {
            LOG_WARN << "error getting next plu id";
            return false;
        }

        LOG_TRACE << "product id " << id;

        // set required fields
        dbutil::updateField( prod, "PrID", QString::number( id ), true );
        dbutil::updateField( prod, "PrNum", QString::number( productNum_ ), true );
        dbutil::updateField( prod, "DeptNum", QString::number( dept_ ), true );

        createProductFromProductDefault( prod );

#if defined( QT_DEBUG)
        LOG_DEBUG << "After setting product fields from product default fields";
        showProductRec( prod );
#endif
    }

    // populate record
    if (( populateNutritionData( prod, prodExists ) ) &&
        ( populateTexts( prod, prodExists ) ) &&
        ( populateCool( prod ) ))
    {
        const QDateTime now( QDateTime::currentDateTime() );

        populateProductData( prod );

        prod.setValue( "PrCdNm1", QString::number( now.toMSecsSinceEpoch() ) );

#if defined( QT_DEBUG)
        LOG_DEBUG << "After setting product fields from data received";
        showProductRec( prod );
#endif

        if ( !updateBlockedProducts() )
            LOG_WARN << "failed to update blocked plus";

        // save!
        if ( dbutil::saveRecordToDatabase( prodsModel, "Product", prod, prodExists ) )
        {
            LOG_DEBUG << "saved product " << productNum_;
            return true;
        }
    }

    LOG_WARN << "failed to save product " << productNum_;
    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void productRecord::createProductFromProductDefault( QSqlRecord& prod ) const
{
    LOG_TRACE << "Creating Prods record from PrDefs";

    // iterate all fields
    for ( int i( prdef_.count() ); i--; )
    {
        const QSqlField field( prdef_.field( i ) );
        const QString fieldName( prdef_.fieldName( i ) );

        // lookup field mapping
        const stringMap::iterator iter( prodDefMapping_.find( fieldName ) );

        if ( prodDefMapping_.end() != iter )
        {
            const QString mappedFieldName( iter.value() );

            if ( mappedFieldName.isEmpty() )
                continue;

            // populate product with value
            const QVariant value( field.value() );
            const QString valueStr( value.toString() );

#if defined( QT_DEBUG )
            LOG_TRACE << "Updating prods field " << qPrintable( mappedFieldName ) <<
                " from PRDefs field " << qPrintable( fieldName ) << ", value = " << qPrintable( valueStr );
#endif

            dbutil::updateField( prod, mappedFieldName, valueStr );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int64_t productRecord::determineNextCoolListNum() const
{
    int64_t nextListNum( -1 );

    QSqlTableModel dbAutoIncRM( nullptr, db_ );
    dbAutoIncRM.setTable( "AutoInc" );
    dbAutoIncRM.setEditStrategy( QSqlTableModel::OnManualSubmit );

    QSqlRecord dbAutoIncRec;

    // get next cool list number from auto inc table
    if ( dbutil::selectRecord( dbAutoIncRM, QString(), dbAutoIncRec ) )
    {
        QSqlField field( dbAutoIncRec.field( "AILLID" ) );

        const QVariant fieldVar( field.value() );
        nextListNum = fieldVar.toLongLong() + 1;

        // ---- //

        // make sure the record number does not exist
        QSqlTableModel dbCoolListRM( nullptr, db_ );
        dbCoolListRM.setTable( "COLLsts" );
        dbCoolListRM.setEditStrategy( QSqlTableModel::OnManualSubmit );

        QSqlRecord dbCoolListRec;

        for ( ;; )
        {
            const QString filter( "LLID=" + QString::number( nextListNum ) );

            if ( !dbutil::selectRecord( dbCoolListRM, filter, dbCoolListRec ) )
                break;
            else
            {
                LOG_DEBUG << "Attempt to create cool list with LLID " << nextListNum << " but it already exists, trying next one";
                ++nextListNum;
            }
        }

        // ---- //

        // update auto inc table with new number
        field.setValue( QVariant( (qlonglong) nextListNum ) );
        dbAutoIncRec.setValue( "AILLID", fieldVar );

        LOG_TRACE << "AutoInc AILLID updated to " << nextListNum;

        dbAutoIncRM.setRecord( 0, dbAutoIncRec );

        if ( !dbutil::saveToDatabase( dbAutoIncRM, "AutoInc" ) )
            LOG_WARN << "Could not update AutoInc db record for AILLID = " << nextListNum;
    }

    return nextListNum;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int64_t productRecord::determineNextNutrifactNum( int64_t startingNum ) const
{
    // Determine the ProdTextNum for the new record.  The algorithm is that we try the plu number as the ProdTextNum value to use
    // to create the new ProductTexts record.  If it's already taken, then just increment by 1 until you find one available,
    // i.e. there is no ProductTexts record with that ProdTextNum.

    //LOG_DEBUG << "determineNextNFNum - startingNum = " << startingNum << "  deptNum = " << deptNum;

    QSqlTableModel dbRM( nullptr, db_ );
    dbRM.setTable( "NutrFs" );
    dbRM.setEditStrategy( QSqlTableModel::OnManualSubmit );

    QSqlRecord rec;

    for ( ;; )
    {
        const QString filter(
            "NFNum=" + QString::number( startingNum ) +
                " and DeptNum=" + QString::number( dept_ ) );

        if ( !dbutil::selectRecord( dbRM, filter, rec ) )
            break;
        else
        {
#if defined( QT_TRACE )
            LOG_DEBUG << "Attempt to create NutrFs with NFNum " << startingNum << " but it already exists, trying next one";
#endif

            startingNum++;
        }
    }

    return startingNum;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int64_t productRecord::determineNextPluID()
{
    int64_t nextPluId( -1 );

    QSqlTableModel dbAutoIncRM( nullptr, db_ );
    dbAutoIncRM.setTable( "AutoInc" );
    dbAutoIncRM.setEditStrategy( QSqlTableModel::OnManualSubmit );

    QSqlRecord dbAutoIncRec;

    // get next plu record number from auto inc table
    if ( dbutil::selectRecord( dbAutoIncRM, QString(), dbAutoIncRec ) )
    {
        QSqlField field( dbAutoIncRec.field( "AIPrID" ) );

        const QVariant fieldVar( field.value() );
        nextPluId = fieldVar.toLongLong() + 1;

        // ---- //

        // make sure the record number does not exist
        QSqlTableModel dbProductsRM( nullptr, db_ );
        dbProductsRM.setTable( "Prods" );
        dbProductsRM.setEditStrategy( QSqlTableModel::OnManualSubmit );

        QSqlRecord dbProductsRec;

        for ( ;; )
        {
            const QString filter( "PrID=" + QString::number( nextPluId ) );

            if ( !dbutil::selectRecord( dbProductsRM, filter, dbProductsRec ) )
                break;
            else
            {
                LOG_DEBUG << "Attempt to create product with PluId " << nextPluId << " but it already exists, trying next one";
                ++nextPluId;
            }
        }

        // ---- //

        // update auto inc table with new number
        dbAutoIncRec.setValue( "AIPrID", QVariant( (qlonglong) nextPluId ) );

        LOG_TRACE << "AutoInc AIPrID updated to " << nextPluId;

        dbAutoIncRM.setRecord( 0, dbAutoIncRec );

        if ( !dbutil::saveToDatabase( dbAutoIncRM, "AutoInc" ) )
            LOG_WARN << "Could not update AutoInc db record for AIPrID = " << nextPluId;
    }

    return nextPluId;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int64_t productRecord::determineNextProdTextNum( int expTextType, int64_t startingNum ) const
{
    // Determine the ProdTextNum for the new record.  The algorithm is that we try the plu number as the ProdTextNum value to use
    // to create the new ProductTexts record.  If it's already taken, then just increment by 1 until you find one available,
    // i.e. there is no ProductTexts record with that ProdTextNum.

    //LOG_DEBUG << "determineNextProdTextNum - startingNum = " << startingNum << "  deptNum = " << deptNum;

    QSqlTableModel dbRM( nullptr, db_ );
    dbRM.setTable( "ProductTexts" );
    dbRM.setEditStrategy( QSqlTableModel::OnManualSubmit );

    QSqlRecord rec;

    for ( ;; )
    {
        const QString filter(
            "ProdTextType=" + QString::number( expTextType ) +
                " and ProdTextNum=" + QString::number( startingNum ) +
                " and DeptNum=" + QString::number( dept_ ) );

        if ( !dbutil::selectRecord( dbRM, filter, rec ) )
            break;
        else
        {
#if defined( QT_DEBUG )
            LOG_TRACE << "Attempt to create ProductTexts with ProdTextType " << expTextType << " and ProdTextNum " << startingNum << " but it already exists, trying next one";
#endif

            startingNum++;
        }
    }

    return startingNum;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool productRecord::populateCool( QSqlRecord& prod ) const
{
    if (( obj_.contains( JSON_COOL ) ) && ( obj_[JSON_COOL].isObject() ))
    {
        const QJsonObject& parsed( obj_[JSON_COOL].toObject() );

        foreach ( const QString& key, parsed.keys() )
        {
            // lookup cool mapping
            const stringMap::iterator iter( coolMapping_.find( key ) );

            if ( coolMapping_.end() != iter )
            {
                const QString mappedFieldName( iter.value() );

                if ( mappedFieldName.isEmpty() )
                    continue;

                // special handling for text list
                if ( JSON_COOL_TEXT_LIST != key )
                {
                    bool useLatin1( false );

                    const QString value( dbutil::convertToString( parsed[key], useLatin1 ) );

#if defined( QT_DEBUG )
                    LOG_TRACE << "Updating prods field " << qPrintable( mappedFieldName ) <<
                        " from JsonObject field " << qPrintable( key ) << ", value = " << qPrintable( value );
#endif
                    dbutil::updateField( prod, mappedFieldName, value, false, useLatin1 );
                }

                else if ( parsed[JSON_COOL_TEXT_LIST].isArray() )
                {
                    const int64_t coolListNum( determineNextCoolListNum() );

                    // create a cool list
                    QSqlTableModel dbCoolListRM( nullptr, db_ );
                    dbCoolListRM.setTable( "COLLsts" );
                    dbCoolListRM.setEditStrategy( QSqlTableModel::OnManualSubmit );

                    QSqlRecord dbCoolListRec;

                    const QString coolListFilter(
                        "LLID=" + QString::number( coolListNum ) +
                            " and DeptNum=" + QString::number( dept_ ) );

                    if ( dbutil::selectRecord( dbCoolListRM, coolListFilter, dbCoolListRec ) )
                        LOG_WARN << "cool list exists " << coolListNum;
                    else
                    {
                        dbCoolListRec = dbCoolListRM.record();
                        dbutil::updateField( dbCoolListRec, "LLID", QString::number( coolListNum ), true );
                        dbutil::updateField( dbCoolListRec, "DeptNum", QString::number( dept_ ), true );
                        dbutil::updateField( dbCoolListRec, "LLNum", QString::number( productNum_ ), true );
                        dbutil::updateField( dbCoolListRec, "LLCHeck", "1", true );

                        // save!
                        if ( !dbutil::saveRecordToDatabase( dbCoolListRM, "COLLsts", dbCoolListRec ) )
                        {
                            LOG_WARN << "failed to create cool list " << coolListNum;
                            return false;
                        }

                        // create cool linked text records
                        else if ( !createListLinkedRecords( "COLLTxs", coolListNum, parsed[JSON_COOL_TEXT_LIST].toArray() ) )
                        {
                            LOG_WARN << "failed to create cool list linked text records " << coolListNum;
                            return false;
                        }

                        else
                        {
#if defined( QT_DEBUG )
                            LOG_TRACE << "Updating prods field " << qPrintable( mappedFieldName ) <<
                                " from JsonObject field " << qPrintable( key ) << ", value = " << coolListNum;
#endif
                            dbutil::updateField( prod, mappedFieldName, QString::number( coolListNum ), true );
                        }
                    }

                } // cool list
            } // if value is mapped to scale table
        } // for each value
    } // product contains cool

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool productRecord::populateNutritionData( QSqlRecord& prod, bool prodExists ) const
{
    const QString key( JSON_NUTRITION_FACTS );
    const QString mappedFieldName( expTextMapping_[key] );

    bool result( true );

    if ( !obj_.contains( JSON_NUTRITION_FACTS ) )
        LOG_DEBUG << "product does not have nutrition fact data";
    else if ( mappedFieldName.isEmpty() )
        LOG_DEBUG << "unmapped text field " << qPrintable( key );
    else if ( obj_[JSON_NUTRITION_FACTS].isNull() )
    {
#if defined( QT_DEBUG )
        LOG_TRACE << "Updating prods field " << qPrintable( mappedFieldName ) <<
            " from JsonObject field " << qPrintable( key ) << ", value = NULL";
#endif

        prod.setNull( mappedFieldName );
    }
    else if ( !obj_[JSON_NUTRITION_FACTS].isObject() )
        LOG_WARN << "nutrition facts not an object";
    else
    {
        QSqlTableModel dbNutrFsRM( nullptr, db_ );
        dbNutrFsRM.setTable( "NutrFs" );
        dbNutrFsRM.setEditStrategy( QSqlTableModel::OnManualSubmit );

        bool dbNutrRecExists( false );
        QSqlRecord dbNutrRec;

        // If the product record is in the database, see if it has a NFNum (NutriFact Number).
        // If it does, look up the NutrFs record for it.
        if ( prodExists )
        {
            const QSqlField field( prod.field( "NFNum" ) );
            const QVariant fieldVar( field.value() );

            if ( fieldVar.toInt() )
            {
                const QString prodNFNum( fieldVar.toString() );

                const QString nutrFilter(
                    "NFNum=" + prodNFNum +
                        " and DeptNum=" + QString::number( dept_ ) );

                if ( dbutil::selectRecord( dbNutrFsRM, nutrFilter, dbNutrRec ) )
                {
                    if ( dbNutrRec.isEmpty() )
                        LOG_WARN << "dbNutrRec read from the db, but is empty for NFNum " << qPrintable( prodNFNum );
                    else
                    {
                        dbNutrRecExists = true;
                    }
                }
            }

        } // product record exists

        // Get an empty record as the basis for a new ProductTexts record
        if ( !dbNutrRecExists )
        {
            dbNutrRec = dbNutrFsRM.record();

            // The NutrFs record DID NOT ALREADY exist when this method started, so I need to set some fields to create a new record.

            // Need to explicitly set these fields
            dbNutrRec.setValue( "DeptNum", QVariant( dept_ ) );

            // Determine the NFNum for the new record.  The algorithm is that we try the plu number as the NFNum value to use
            // to create the new NutrFs record.  If it's already taken, then just increment by 1 until you find one available,
            // i.e. there is no NutrFs record with that NFNum.
            const int64_t newNutrifactNum( determineNextNutrifactNum( productNum_ ) );

            dbNutrRec.setValue( "NFNum", QVariant( QString::number( newNutrifactNum ) ) );

            // update product record
            prod.setValue( mappedFieldName, QVariant( QString::number( newNutrifactNum ) ) );
        }

        // UPDATE
        const nutritionFactsParser nutrifacts( obj_[JSON_NUTRITION_FACTS].toObject() );

        const QString nutrifactsCol1( nutrifacts.nutrifacts() );

        dbutil::updateField( dbNutrRec, "NFCol1", nutrifactsCol1, false, true );
        dbNutrRec.setValue( "NFCol2", QVariant("") );
        dbNutrRec.setValue( "NFCol3", QVariant("") );
        dbNutrRec.setValue( "NFCol4", QVariant("") );

        // Set this to indicate the nutrifacts record has changed
        dbNutrRec.setValue("NFCheck", QVariant( (int) 1 ) );

#if defined( QT_DEBUG )
        LOG_TRACE << "Updating prods field " << qPrintable( mappedFieldName ) <<
            " from JsonObject field " << qPrintable( key ) << ", col1 value = " << qPrintable( nutrifactsCol1 );
#endif

        // SAVE!!!
        if ( !dbutil::saveRecordToDatabase( dbNutrFsRM, key, dbNutrRec, dbNutrRecExists ) )
        {
            LOG_WARN << "failed to save nutrition data record " << qPrintable( key );
            result = false;
        }

    } // nutrition facts json is an object

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void productRecord::populateProductData( QSqlRecord& prod ) const
{
    QList<QJsonObject> objsToParse;
    objsToParse.append( obj_ );

    if (( obj_.contains( JSON_PRICING ) ) && ( obj_[JSON_PRICING].isObject() ))
        objsToParse.append( obj_[JSON_PRICING].toObject() );

    if (( obj_.contains( JSON_PRODUCT_LIFE ) ) && ( obj_[JSON_PRODUCT_LIFE].isObject() ))
        objsToParse.append( obj_[JSON_PRODUCT_LIFE].toObject() );

    if (( obj_.contains( JSON_LABELING ) ) && ( obj_[JSON_LABELING].isObject() ))
        objsToParse.append( obj_[JSON_LABELING].toObject() );

    foreach ( const QJsonObject& parsed, objsToParse )
        foreach ( const QString& key, parsed.keys() )
        {
            // lookup field mapping
            const stringMap::iterator iter( fieldMapping_.find( key ) );

            if ( fieldMapping_.end() != iter )
            {
                const QString mappedFieldName( iter.value() );

                if ( mappedFieldName.isEmpty() )
                    continue;

                bool useLatin1( false );
                int mult( 0 );

                if ( conversionMultiplierMap_.contains( mappedFieldName ) )
                {
                    mult = conversionMultiplierMap_[mappedFieldName];
                    LOG_DEBUG << "converting field to integer " << qPrintable( mappedFieldName ) << " mult " << mult;
                }

                QString value( dbutil::convertToString( parsed[key], useLatin1, mult ) );

                // they have <BR> in their specification... lets replace with carriage returns
                static const QString lineBreak( "<BR>" );

                if (( "PrDesc" == mappedFieldName ) && ( value.contains( lineBreak ) ))
                {
                    LOG_DEBUG << "found line breaks in product description";
                    value.replace( lineBreak, "\n" );
                }
                else if ( isLabelPlacement( mappedFieldName ) )
                {
                    if ( labelPlacements_.contains( value ) )
                        value = QString::number( labelPlacements_[value] );
                    else
                    {
                        LOG_DEBUG << "missing label placement enum value " << qPrintable( value );
                    }
                }
                else if ( isLabelRotation( mappedFieldName ) )
                {
                    if ( labelRotations_.contains( value ) )
                        value = QString::number( labelRotations_[value] );
                    else
                    {
                        LOG_DEBUG << "missing label rotation enum value " << qPrintable( value );
                    }
                }

#if defined( QT_DEBUG )
                LOG_TRACE << "Updating prods field " << qPrintable( mappedFieldName ) <<
                    " from JsonObject field " << qPrintable( key ) << ", value = " << qPrintable( value );
#endif
                dbutil::updateField( prod, mappedFieldName, value, false, useLatin1 );
            }
        }

}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool productRecord::populateTexts( QSqlRecord& prod, bool prodExists ) const
{
    bool result( true );

    if ( !obj_.contains( JSON_TEXTS ) )
        return true;
    else if ( !obj_[JSON_TEXTS].isObject() )
    {
        LOG_WARN << "texts not an object";
        return true;
    }

    // iterate all keys
    const QJsonObject parsed( obj_[JSON_TEXTS].toObject() );

    foreach ( const QString& key, parsed.keys() )
    {
        // lookup field mapping
        const stringMap::iterator iter( expTextMapping_.find( key ) );

        if ( expTextMapping_.end() == iter )
        {
            LOG_WARN << "unhandled text field " << qPrintable( key );
            continue;
        }

        const QString mappedFieldName( iter.value() );

        if ( mappedFieldName.isEmpty() )
        {
            LOG_DEBUG << "unmapped text field " << qPrintable( key );
            continue;
        }
        else if ( !expTextTypeMap_.contains( mappedFieldName ) )
        {
            LOG_WARN << "missing text type for " << qPrintable( mappedFieldName );
            continue;
        }

        // ---- //

        const int prodTxType( expTextTypeMap_[mappedFieldName] );

        bool useLatin1( false );

        const QString value( dbutil::convertToString( parsed[key], useLatin1 ) );

        // use null value when empty
        if ( value.isEmpty() )
        {
#if defined( QT_DEBUG )
            LOG_TRACE << "Updating prods field " << qPrintable( mappedFieldName ) <<
                " from JsonObject field " << qPrintable( key ) << ", value = NULL";
#endif
            prod.setNull( mappedFieldName );
            continue;
        }

        QSqlTableModel dbTextsRM( nullptr, db_ );
        dbTextsRM.setTable( "ProductTexts" );
        dbTextsRM.setEditStrategy( QSqlTableModel::OnManualSubmit );

        bool dbTextRecExists( false );
        QSqlRecord dbTextRec;

        // If the product record is in the database, see if it has a TxNum (Expanded Text Number).
        // If it does, look up the ProductText record for it.
        if ( prodExists )
        {
            const QSqlField field( prod.field( mappedFieldName ) );
            const QVariant fieldVar( field.value() );

            if ( fieldVar.toInt() )
            {
                const QString prodTxNum( fieldVar.toString() );

                const QString textFilter(
                    "ProdTextType=" + QString::number( prodTxType ) +
                        " and ProdTextNum=" + prodTxNum +
                        " and DeptNum=" + QString::number( dept_ ) );

                if ( dbutil::selectRecord( dbTextsRM, textFilter, dbTextRec ) )
                {
                    if ( dbTextRec.isEmpty() )
                        LOG_WARN << "dbTextRec read from the db, but is empty for TextType " << prodTxType << " TextNum " << qPrintable( prodTxNum );
                    else
                    {
                        dbTextRecExists = true;
                    }
                }
            }

        } // product record exists

        // Get an empty record as the basis for a new ProductTexts record
        if ( !dbTextRecExists )
        {
            dbTextRec = dbTextsRM.record();

            // The ProductTexts record DID NOT ALREADY exist when this method started, so I need to set some fields to create a new record.

            // Need to explicitly set these fields
            dbTextRec.setValue( "DeptNum", QVariant( dept_ ) );
            dbTextRec.setValue( "ProdTextType", QVariant( prodTxType ) );

            // Determine the ProdTextNum for the new record.  The algorithm is that we try the plu number as the ProdTextNum value to use
            // to create the new ProductTexts record.  If it's already taken, then just increment by 1 until you find one available,
            // i.e. there is no ProductTexts record with that ProdTextNum.
            const int64_t newProdTextNum( determineNextProdTextNum( prodTxType, productNum_ ) );

            dbTextRec.setValue( "ProdTextNum", QVariant( QString::number( newProdTextNum ) ) );

            // update product record
            prod.setValue( mappedFieldName, QVariant( QString::number( newProdTextNum ) ) );
        }

        // UPDATE
        dbutil::updateField( dbTextRec, "ProdTextDesc", value, false, useLatin1 );

        // chobart_d always just sets the 'check' field to 1, since the beginning of HT's existence.
        // I was setting it to 42 to indicate the hook changed it.
        dbTextRec.setValue( "ProdTextCheck", QVariant( (int) 1 ) );

#if defined( QT_DEBUG )
        LOG_TRACE << "Updating prods field " << qPrintable( mappedFieldName ) <<
            " from JsonObject field " << qPrintable( key ) << ", value = " << qPrintable( value );
#endif

        // SAVE!!!
        if ( !dbutil::saveRecordToDatabase( dbTextsRM, key, dbTextRec, dbTextRecExists ) )
        {
            LOG_WARN << "failed to save expanded text record " << qPrintable( key );
            result = false;

            break;
        }

    } // for each json field

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void productRecord::showProductRec( const QSqlRecord& dbProductRec ) const
{
    LOG_DEBUG << "showProductRec  begin";
    LOG_DEBUG << "productRec is empty = " << ((dbProductRec.isEmpty()) ? "true" : "false");

    for ( int i( 0 ); i < dbProductRec.count(); ++i )
    {
        const QSqlField field( dbProductRec.field( i ) );
        const QVariant fieldVar( field.value() );

        LOG_DEBUG << " field = " << qPrintable( dbProductRec.fieldName( i ) ) <<
            "  type = " << fieldVar.typeName() <<
            "  value = " << qPrintable( fieldVar.toString() );
    }

    LOG_DEBUG << "showProductRec  end";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool productRecord::updateBlockedProducts() const
{
    if ( !obj_.contains( JSON_IS_REMOTE_PRINT_ONLY ) )
        return true;

    const QJsonValue v( obj_[JSON_IS_REMOTE_PRINT_ONLY] );

    if ( !v.isBool() )
        return true;

    scaleBlockedProducts blocked;

    if ( v.toBool() )
        blocked.addBlockedProduct( productNum_ );
    else
        blocked.removeBlockedProduct( productNum_ );

    return blocked.saveFile();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool productRecord::createListLinkedRecords( const QString& table, int64_t coolListNum, const QJsonArray& a ) const
{
    QSqlTableModel tableModel( nullptr, db_ );
    tableModel.setTable( table );
    tableModel.setEditStrategy( QSqlTableModel::OnManualSubmit );

    const QString deleteFilter( "LLID=" + QString::number( coolListNum ) + " and DeptNum=" + QString::number( dept_ ) );

    // remove old records
    dbutil::deleteRecords( tableModel, deleteFilter );

    // add new records
    foreach ( const QJsonValue& v, a )
        if ( v.isObject() )
        {
            const QJsonObject obj( v.toObject() );

            if (( !obj.contains( JSON_COOL_TEXT_SEQ_NO ) ) || ( !obj[JSON_COOL_TEXT_SEQ_NO].isDouble() ))
            {
                LOG_WARN << "cool text seq num missing or invalid";
                continue;
            }
            else if (( !obj.contains( JSON_COOL_TEXT_NUM ) ) || ( !obj[JSON_COOL_TEXT_NUM].isDouble() ))
            {
                LOG_WARN << "cool text id missing or invalid";
                continue;
            }

            const int order( obj[JSON_COOL_TEXT_SEQ_NO].toInt() );
            const int linkedNum( obj[JSON_COOL_TEXT_NUM].toInt() );

            const QString filter(
                "LLID=" + QString::number( coolListNum ) +
                    " and LXNum=" + QString::number( linkedNum ) +
                    " and DeptNum=" + QString::number( dept_ ) );

            // check for record
            QSqlRecord rec;

            if ( dbutil::selectRecord( tableModel, filter, rec ) )
                LOG_TRACE << "record exists " << coolListNum << " " << linkedNum;
            else
            {
                LOG_DEBUG << "record does not exist " << coolListNum << " " << linkedNum;
                rec = tableModel.record();

                // set required fields
                dbutil::updateField( rec, "LLID", QString::number( coolListNum ), true );
                dbutil::updateField( rec, "LXNum", QString::number( linkedNum ), true );
                dbutil::updateField( rec, "DeptNum", QString::number( dept_ ), true );
                dbutil::updateField( rec, "XLOrder", QString::number( order ), true );
                dbutil::updateField( rec, "XLCheck", "1", true );

                // save!
                if ( dbutil::saveRecordToDatabase( tableModel, tableModel.tableName(), rec, false ) )
                    LOG_TRACE << "created record " << coolListNum << " " << linkedNum;
                else
                {
                    LOG_WARN << "failed to created record " << coolListNum << " " << linkedNum;
                    return false;
                }
            }
        }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void productRecord::loadMapping( const QString& filename, const QString& keyBase, QMap<QString, QString>& mapping )
{
    QSettings configs( filename, QSettings::IniFormat );

    // clear existing mapping
    mapping.clear();

    // load in mappings
    for ( int i( 0 ); i < 1000; ++i )
    {
        const QString key( keyBase + QString( "%0" ).arg( i, 3, 10, QChar( '0' ) ) );
        const QString value( configs.value( key ).toString() );

        if ( value.isEmpty() )
            break;

        // split string into key:value pair
        else if ( 2 <= value.length() )
        {
            const int split( value.indexOf( ':' ) );

            if ( 0 < split )
                mapping[value.left( split )] = value.mid( split+1 );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool productRecord::isLabelPlacement( const QString& tag )
{
    return (( "PrLblPlc1" == tag ) || ( "PrLblPlc2" == tag ) || ( "PrLblPlc3" == tag ));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool productRecord::isLabelRotation( const QString& tag )
{
    return (( "PrLblRot1" == tag ) || ( "PrLblRot2" == tag ) || ( "PrLblRot3" == tag ));
}
