/**
 * @file sessionmanager.cpp
 *
 * @copyright Copyright (c) 2021, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#include "ooapi.h"
#include "oodbadapter.h"

#include <databaseeventtype.h>
#include <databasejson.h>
#include <fileresourceevent.h>
#include <jsondocumentevent.h>
#include <resourceeventtype.h>
#include <sessioneventtype.h>

#include <QDebug>
#include <QJsonObject>

static const QString DBUS_SERVICE( "com.hobartcorp.BOODA" );
static const QString DBUS_OBJECT_PATH( "/Hobart/BOODA/API" );

// uncomment to debug content data
#define DEBUG_JSON

OnlineOrderingDatabaseAdapter *OnlineOrderingDatabaseAdapter::instance_( nullptr );
QMutex OnlineOrderingDatabaseAdapter::instanceMutex_;

///////////////////////////////////////////////////////////////////////////////////////////////////
OnlineOrderingDatabaseAdapter::OnlineOrderingDatabaseAdapter() :
    _Mybase( DBUS_SERVICE, DBUS_OBJECT_PATH ),
    api_( nullptr )
{
    // create api
    api_ = new OnlineOrderingAPI( this );
    api_->setBlocking( true );

    connect( api_, &OnlineOrderingAPI::categoryReceived, this, &_Myt::onCategoryReceived );
    connect( api_, &OnlineOrderingAPI::categoryImageReceived, this, &_Myt::onCategoryImageReceived );
    connect( api_, &OnlineOrderingAPI::departmentReceived, this, &_Myt::onDepartmentReceived );
    connect( api_, &OnlineOrderingAPI::operatorReceived, this, &_Myt::onOperatorReceived );
    connect( api_, &OnlineOrderingAPI::productReceived, this, &_Myt::onProductReceived );
    connect( api_, &OnlineOrderingAPI::productImageReceived, this, &_Myt::onProductImageReceived );

    // register service aliases
    SessionManRegisterServiceAlias a0;
    a0.setData( SESSION_MAN_REG_SERVICE_ALIAS_NAME, RESOURCE_MANAGER_DBUS_SERVICE );

    post( &a0 );

    SessionManRegisterServiceAlias a1;
    a1.setData( SESSION_MAN_REG_SERVICE_ALIAS_NAME, SCALE_DATABASE_DBUS_SERVICE );

    post( &a1 );

    // register for resources
    registerEvent( DeviceType::GraphicResource );

    // register for database events
    registerEvent( DeviceType::DepartmentDatabase );

    registerEvent( DeviceType::ProductDatabase );
    registerEvent( DeviceType::OperatorDatabase );
    registerEvent( DeviceType::CategoryDatabase );

    // create mappings
    createDatabaseMappings();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
OnlineOrderingDatabaseAdapter::~OnlineOrderingDatabaseAdapter()
{
    // unregister service aliases
    SessionManUnregisterServiceAlias a0;
    a0.setData( SESSION_MAN_UNREG_SERVICE_ALIAS_NAME, RESOURCE_MANAGER_DBUS_SERVICE );

    post( &a0 );

    SessionManUnregisterServiceAlias a1;
    a1.setData( SESSION_MAN_UNREG_SERVICE_ALIAS_NAME, SCALE_DATABASE_DBUS_SERVICE );

    post( &a1 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineOrderingDatabaseAdapter::setServerAddress( const QString& value )
{
    api_->setServerAddress( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
OnlineOrderingDatabaseAdapter *OnlineOrderingDatabaseAdapter::instance()
{
    if ( !instance_ )
    {
        QMutexLocker guard( &instanceMutex_ );

        if ( !instance_ )
            instance_ = new _Myt;
    }

    return instance_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineOrderingDatabaseAdapter::onProcessEvent( const Event *pending, bool& destroy )
{
    // we take ownership of all events that come in here
    destroy = false;

    // add event to queue
    m_.lock();
    events_.append( pending );
    m_.unlock();

    if ( !eventsProcessing_.tryLock() )
    {
        qDebug() << "event queued";
        return;
    }

    // process queue
    for ( ;; )
    {
        // grab next event
        m_.lock();

        if ( events_.isEmpty() )
            break;

        const Event *e( events_.first() );

        events_.removeFirst();
        m_.unlock();

        // process!
        qDebug() << "process event" << (uint) e->device() << e->event();

        switch ( e->device() )
        {
        case DeviceType::GraphicResource:
            processResourceEvent( dynamic_cast<const FileResourceEvent*>( e ) );
            break;

        case DeviceType::DepartmentDatabase:

        case DeviceType::ProductDatabase:
        case DeviceType::OperatorDatabase:
        case DeviceType::CategoryDatabase:
            processScaleDatabaseEvent( dynamic_cast<const JsonDocumentEvent*>( e ) );
            break;

        default:
            break;
        }

        // destroy event
        destroyEvent( e );
    }

    eventsProcessing_.unlock();
    m_.unlock();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineOrderingDatabaseAdapter::onCategoryReceived( const QJsonObject& obj )
{
#ifdef DEBUG_JSON
    qDebug() << "RECV API\n" << obj;
#endif

    // forward to sender
    const JsonDocumentEvent e( DeviceType::CategoryDatabase, (uint) DatabaseEventType::Write, QJsonDocument( transform( categoryRecFromDatabase_, obj ) ) );

    if (( respService_.isEmpty() ) || ( respObjectPath_.isEmpty() ))
        post( &e );
    else
        send( respService_, respObjectPath_, &e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineOrderingDatabaseAdapter::onCategoryImageReceived( const QString& name, const QByteArray& a )
{
    qDebug() << "RECV IMAGE" << name << a.size();

    // forward to sender
    const FileResourceEvent e( DeviceType::GraphicResource, (uint) ResourceEventType::Write, (uint) ResourceType::Graphic, (uint) GraphicSubtype::Category, name, a );

    if (( respService_.isEmpty() ) || ( respObjectPath_.isEmpty() ))
        post( &e );
    else
        send( respService_, respObjectPath_, &e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineOrderingDatabaseAdapter::onDepartmentReceived( const QJsonObject& obj )
{
#ifdef DEBUG_JSON
    qDebug() << "RECV API\n" << obj;
#endif

    // forward to sender
    const JsonDocumentEvent e( DeviceType::DepartmentDatabase, (uint) DatabaseEventType::Write, QJsonDocument( transform( departmentRecFromDatabase_, obj ) ) );

    if (( respService_.isEmpty() ) || ( respObjectPath_.isEmpty() ))
        post( &e );
    else
        send( respService_, respObjectPath_, &e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineOrderingDatabaseAdapter::onOperatorReceived( const QJsonObject& obj )
{
#ifdef DEBUG_JSON
    qDebug() << "RECV API\n" << obj;
#endif

    // forward to sender
    const JsonDocumentEvent e( DeviceType::OperatorDatabase, (uint) DatabaseEventType::Write, QJsonDocument( transform( operatorRecFromDatabase_, obj ) ) );

    if (( respService_.isEmpty() ) || ( respObjectPath_.isEmpty() ))
        post( &e );
    else
        send( respService_, respObjectPath_, &e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineOrderingDatabaseAdapter::onProductReceived( const QJsonObject& obj )
{
#ifdef DEBUG_JSON
    qDebug() << "RECV API\n" << obj;
#endif

    // forward to sender
    const JsonDocumentEvent e( DeviceType::ProductDatabase, (uint) DatabaseEventType::Write, QJsonDocument( transform( productRecFromDatabase_, obj ) ) );

    if (( respService_.isEmpty() ) || ( respObjectPath_.isEmpty() ))
        post( &e );
    else
        send( respService_, respObjectPath_, &e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineOrderingDatabaseAdapter::onProductImageReceived( const QString& name, const QByteArray& a )
{
    qDebug() << "RECV IMAGE" << name << a.size();

    // forward to sender
    const FileResourceEvent e( DeviceType::GraphicResource, (uint) ResourceEventType::Write, (uint) ResourceType::Graphic, (uint) GraphicSubtype::Product, name, a );

    if (( respService_.isEmpty() ) || ( respObjectPath_.isEmpty() ))
        post( &e );
    else
        send( respService_, respObjectPath_, &e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineOrderingDatabaseAdapter::processResourceEvent( const FileResourceEvent *e )
{
    if ( !e )
        return;

    qDebug() << "process resource event" << (uint) e->device() << e->event() << e->type() << e->subtype();

    // only process graphics
    if ( ResourceType::Graphic != (ResourceType) e->type() )
        return;

    // give responses to sender
    respService_ = e->senderService();
    respObjectPath_ = e->senderObjectPath();

    // ============================================================================
    // Category Image
    // ============================================================================
    if ( GraphicSubtype::Category == (GraphicSubtype) e->subtype() )
    {
        const QString key0( e->filename() );

        if ( (uint) ResourceEventType::Read == e->event() )
        {
            if ( key0.isEmpty() )
                qWarning() << "missing filename";
            else
                api_->getCategoryImage( key0 );
        }
        else if ( (uint) ResourceEventType::ReadAll == e->event() )
            api_->getAllCategoryImages();
        else if ( (uint) ResourceEventType::Write == e->event() )
        {
            QJsonObject obj;
            obj["filename"] = e->filename();
            obj["image"] = QString( e->data().toBase64() );

            api_->updateCategoryImage( obj );
        }
        else if ( (uint) ResourceEventType::Delete == e->event() )
        {
            if ( key0.isEmpty() )
                qWarning() << "missing filename";
            else
                api_->deleteCategoryImage( key0 );
        }
        else if ( (uint) ResourceEventType::DeleteAll == e->event() )
            api_->deleteAllCategoryImages();
    }

    // ============================================================================
    // Product Image
    // ============================================================================
    else if ( GraphicSubtype::Product == (GraphicSubtype) e->subtype() )
    {
        const QString key0( e->filename() );

        if ( (uint) ResourceEventType::Read == e->event() )
        {
            if ( key0.isEmpty() )
                qWarning() << "missing filename";
            else
                api_->getProductImage( key0 );
        }
        else if ( (uint) ResourceEventType::ReadAll == e->event() )
            api_->getAllProductImages();
        else if ( (uint) ResourceEventType::Write == e->event() )
        {
            QJsonObject obj;
            obj["filename"] = e->filename();
            obj["image"] = QString( e->data().toBase64() );

            api_->updateProductImage( obj );
        }
        else if ( (uint) ResourceEventType::Delete == e->event() )
        {
            if ( key0.isEmpty() )
                qWarning() << "missing filename";
            else
                api_->deleteProductImage( key0 );
        }
        else if ( (uint) ResourceEventType::DeleteAll == e->event() )
            api_->deleteAllProductImages();
    }

    else
    {
        qWarning() << "unhandled graphic type" << e->subtype();
    }

    // clear response info
    respService_.clear();
    respObjectPath_.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineOrderingDatabaseAdapter::processScaleDatabaseEvent( const JsonDocumentEvent *e )
{
    if ( !e )
        return;

    qDebug() << "process scale database event" << (uint) e->device() << e->event();

    const QJsonDocument doc( e->document() );

    // give responses to sender
    respService_ = e->senderService();
    respObjectPath_ = e->senderObjectPath();

    // parse attached JSON object
    QJsonObject obj;

    if ( doc.isObject() )
    {
        obj = doc.object();

#ifdef DEBUG_JSON
        qDebug() << "RECV SESSION\n" << doc.object();
#endif
    }

    // ============================================================================
    // Department
    // ============================================================================
    if ( DeviceType::DepartmentDatabase == e->device() )
    {
        QString key;

        if ( obj.contains( JSON_DEPARTMENT_DEPTNUM_FIELD ) )
            key = obj.value( JSON_DEPARTMENT_DEPTNUM_FIELD ).toString();

        if ( (uint) DatabaseEventType::Read == e->event() )
        {
            if ( key.isEmpty() )
                qWarning() << "missing dept num";
            else
                api_->getDepartment( key );
        }
        else if ( (uint) DatabaseEventType::ReadAll == e->event() )
            api_->getAllDepartments();
        else if ( (uint) DatabaseEventType::Write == e->event() )
            api_->updateDepartment( transform( departmentRecToDatabase_, obj ) );
        else if ( (uint) DatabaseEventType::Delete == e->event() )
        {
            if ( key.isEmpty() )
                qWarning() << "missing dept num";
            else
                api_->deleteDepartment( key );
        }
        else if ( (uint) DatabaseEventType::DeleteAll == e->event() )
            api_->deleteAllDepartments();
    }

    // ============================================================================
    // Product
    // ============================================================================
    else if ( DeviceType::ProductDatabase == e->device() )
    {
        QString key0;
        QString key1;

        if ( obj.contains( JSON_PRODUCT_DEPTNUM_FIELD ) )
            key0 = obj.value( JSON_PRODUCT_DEPTNUM_FIELD ).toString();

        if ( obj.contains( JSON_PRODUCT_PLU_FIELD ) )
            key1 = obj.value( JSON_PRODUCT_PLU_FIELD ).toString();

        if ( (uint) DatabaseEventType::Read == e->event() )
        {
            if ( key0.isEmpty() )
                qWarning() << "missing dept num";
            else if ( key1.isEmpty() )
                qWarning() << "missing plu";
            else
                api_->getProduct( key0, key1 );
        }
        else if ( (uint) DatabaseEventType::ReadAll == e->event() )
            api_->getAllProducts();
        else if ( (uint) DatabaseEventType::Write == e->event() )
            api_->updateProduct( transform( productRecToDatabase_, obj ) );
        else if ( (uint) DatabaseEventType::Delete == e->event() )
        {
            if ( key0.isEmpty() )
                qWarning() << "missing dept num";
            else if ( key1.isEmpty() )
                qWarning() << "missing plu";
            else
                api_->deleteProduct( key0, key1 );
        }
        else if ( (uint) DatabaseEventType::DeleteAll == e->event() )
            api_->deleteAllProducts();
    }

    // ============================================================================
    // Operator
    // ============================================================================
    else if ( DeviceType::OperatorDatabase == e->device() )
    {
        QString key;

        if ( obj.contains( JSON_OPERATOR_NUMBER_FIELD ) )
            key = obj.value( JSON_OPERATOR_NUMBER_FIELD ).toString();

        if ( (uint) DatabaseEventType::Read == e->event() )
        {
            if ( key.isEmpty() )
                qWarning() << "missing operator id";
            else
                api_->getOperator( key );
        }
        else if ( (uint) DatabaseEventType::ReadAll == e->event() )
            api_->getAllOperators();
        else if ( (uint) DatabaseEventType::Write == e->event() )
            api_->updateOperator( transform( operatorRecToDatabase_, obj ) );
        else if ( (uint) DatabaseEventType::Delete == e->event() )
        {
            if ( key.isEmpty() )
                qWarning() << "missing operator id";
            else
                api_->deleteOperator( key );
        }
        else if ( (uint) DatabaseEventType::DeleteAll == e->event() )
            api_->deleteAllOperators();
    }

    // ============================================================================
    // Category
    // ============================================================================
    else if ( DeviceType::CategoryDatabase == e->device() )
    {
        QString key0;
        QString key1;

        if ( obj.contains( JSON_CATEGORY_DEPTNUM_FIELD ) )
            key0 = obj.value( JSON_CATEGORY_DEPTNUM_FIELD ).toString();

        if ( obj.contains( JSON_CATEGORY_NUMBER_FIELD ) )
            key1 = obj.value( JSON_CATEGORY_NUMBER_FIELD ).toString();

        if ( (uint) DatabaseEventType::Read == e->event() )
        {
            if ( key0.isEmpty() )
                qWarning() << "missing dept num";
            else if ( key1.isEmpty() )
                qWarning() << "missing category num";
            else
                api_->getCategory( key0, key1 );
        }
        else if ( (uint) DatabaseEventType::ReadAll == e->event() )
            api_->getAllCategories();
        else if ( (uint) DatabaseEventType::Write == e->event() )
            api_->updateCategory( transform( categoryRecToDatabase_, obj ) );
        else if ( (uint) DatabaseEventType::Delete == e->event() )
        {
            if ( key0.isEmpty() )
                qWarning() << "missing dept num";
            else if ( key1.isEmpty() )
                qWarning() << "missing category num";
            else
                api_->deleteCategory( key0, key1 );
        }
        else if ( (uint) DatabaseEventType::DeleteAll == e->event() )
            api_->deleteAllCategories();
    }

    // clear response info
    respService_.clear();
    respObjectPath_.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineOrderingDatabaseAdapter::createDatabaseMappings()
{
    // ----------
    // Department
    // ----------

    departmentRecToDatabase_[JSON_DEPARTMENT_DEPTNUM_FIELD] = "DeptNum";
    departmentRecToDatabase_[JSON_DEPARTMENT_NAME1_FIELD] = "DeptName1";
    departmentRecToDatabase_[JSON_DEPARTMENT_NAME2_FIELD] = "DeptName2";

    createAntiMapping( departmentRecToDatabase_, departmentRecFromDatabase_ );

    // -------
    // Product
    // -------

    productRecToDatabase_[JSON_PRODUCT_PLU_FIELD] = "PrNum";
    productRecToDatabase_[JSON_PRODUCT_TYPE_FIELD] = "PrType";
    productRecToDatabase_[JSON_PRODUCT_DESCRIPTION_FIELD] = "PrDesc";
    productRecToDatabase_[JSON_PRODUCT_DEPTNUM_FIELD] = "DeptNum";

    productRecToDatabase_[JSON_PRODUCT_REPORTINGCLASS_FIELD] = "ClNum";
    productRecToDatabase_[JSON_PRODUCT_CATEGORY_FIELD] = "CaNum";
    productRecToDatabase_[JSON_PRODUCT_SPECIALNUMBER_FIELD] = "SPNum";

    productRecToDatabase_[JSON_PRODUCT_PRICEFACTOR_FIELD] = "PrPrMod";
    productRecToDatabase_[JSON_PRODUCT_PORTIONS_FIELD] = "PrPrtns";

    productRecToDatabase_[JSON_PRODUCT_BARCODEPREFIX_FIELD] = "PrBarPref";
    productRecToDatabase_[JSON_PRODUCT_RAWBARCODENUMBER_FIELD] = "PrRawBarC";
    productRecToDatabase_[JSON_PRODUCT_BARCODEPRODUCTNUMBER_FIELD] = "PrBarProN";
    productRecToDatabase_[JSON_PRODUCT_BARCODEVENDORNUMBER_FIELD] = "PrBarVenN";
    productRecToDatabase_[JSON_PRODUCT_BARCODEEANCODE_FIELD] = "PrEANCd";

    productRecToDatabase_[JSON_PRODUCT_BYCOUNT_FIELD] = "PrByCnt";
    productRecToDatabase_[JSON_PRODUCT_FORCEBYCOUNT_FIELD] = "PrIsFrcBC";
    productRecToDatabase_[JSON_PRODUCT_EXCEPTIONPRICE_FIELD] = "PrExcPr";
    productRecToDatabase_[JSON_PRODUCT_PRICE_FIELD] = "PrPrice";
    productRecToDatabase_[JSON_PRODUCT_FORCEPRICE_FIELD] = "PrIsFrcPr";

    productRecToDatabase_[JSON_PRODUCT_DISCOUNTMETHOD_FIELD] = "PrDscMe";
    productRecToDatabase_[JSON_PRODUCT_DISCOUNTPRICE1_FIELD] = "PrDscPr1";
    productRecToDatabase_[JSON_PRODUCT_DISCOUNTPRICE2_FIELD] = "PrDscPr2";
    productRecToDatabase_[JSON_PRODUCT_DISCOUNTPRICE3_FIELD] = "PrDscPr3";
    productRecToDatabase_[JSON_PRODUCT_BONUSPOINTS_FIELD] = "PrBnsPts";

    productRecToDatabase_[JSON_PRODUCT_TARE_FIELD] = "PrTare";
    productRecToDatabase_[JSON_PRODUCT_TAREB_FIELD] = "PrTareB";
    productRecToDatabase_[JSON_PRODUCT_FORCETARE_FIELD] = "PrIsFrcTr";
    productRecToDatabase_[JSON_PRODUCT_PROPORTIONALTARE_FIELD] = "PrPrpTare";
    productRecToDatabase_[JSON_PRODUCT_NETWEIGHT_FIELD] = "PrNetWt";

    productRecToDatabase_[JSON_PRODUCT_SHELFLIFEDAYS_FIELD] = "PrShlfLfD";
    productRecToDatabase_[JSON_PRODUCT_SHELFLIFEHOURS_FIELD] = "PrShlfLfH";
    productRecToDatabase_[JSON_PRODUCT_DONTPRINTSHELFLIFE_FIELD] = "PrIsPrnSL";
    productRecToDatabase_[JSON_PRODUCT_PRODUCTLIFEDAYS_FIELD] = "PrProdLfD";
    productRecToDatabase_[JSON_PRODUCT_PRODUCTLIFEHOURS_FIELD] = "PrProdLfH";
    productRecToDatabase_[JSON_PRODUCT_DONTPRINTPRODUCTLIFE_FIELD] = "PrIsPrnPL";
    productRecToDatabase_[JSON_PRODUCT_DONTPRINTPACKEDON_FIELD] = "PrIsPrnPO";

    productRecToDatabase_[JSON_PRODUCT_GRAPHIC1_FIELD] = "PrGrNm1";
    productRecToDatabase_[JSON_PRODUCT_GRAPHIC2_FIELD] = "PrGrNm2";
    productRecToDatabase_[JSON_PRODUCT_GRAPHIC3_FIELD] = "PrGrNm3";
    productRecToDatabase_[JSON_PRODUCT_GRAPHIC4_FIELD] = "PrGrNm4";
    productRecToDatabase_[JSON_PRODUCT_IMAGE_FIELD] = "PrImage";

    productRecToDatabase_[JSON_PRODUCT_NAME1_FIELD] = "PrName1";
    productRecToDatabase_[JSON_PRODUCT_NAME2_FIELD] = "PrName2";

    productRecToDatabase_[JSON_PRODUCT_SPECIAL_NUM_FIELD] = "SpNum";
    productRecToDatabase_[JSON_PRODUCT_SPECIALSTARTDATE_FIELD] = "PrSpcSD";

    productRecToDatabase_[JSON_PRODUCT_MEALSIDEITEMSGROUPNUM_FIELD] = "PrMealSideItemsGroupNum";
    productRecToDatabase_[JSON_PRODUCT_MEALSIDEITEMSAREFREE_FIELD] = "PrMealSideItemsAreFree";
    productRecToDatabase_[JSON_PRODUCT_MEALNUMSIDESINCLUDED_FIELD] = "PrMealNumSidesIncluded";

    productRecToDatabase_[JSON_PRODUCT_CUTTESTMEATSPECSHEETGROUPNUM_FIELD] = "PrCutTestMeatSpecSheetGroupNum";
    productRecToDatabase_[JSON_PRODUCT_PRODUCTIONPLANNINGTYPE_FIELD] = "PrPrPlT";

    productRecToDatabase_[JSON_PRODUCT_LABEL1_FIELD] = "LbNum1";
    productRecToDatabase_[JSON_PRODUCT_LABEL2_FIELD] = "LbNum2";
    productRecToDatabase_[JSON_PRODUCT_LABEL3_FIELD] = "LbNum3";

    productRecToDatabase_[JSON_PRODUCT_APPLYSECURITYLABEL_FIELD] = "PrIsUsSL";
    productRecToDatabase_[JSON_PRODUCT_INTAKEMODE_FIELD] = "PrInMode";
    productRecToDatabase_[JSON_PRODUCT_LABEL1ROTATION_FIELD] = "PrLblRot1";
    productRecToDatabase_[JSON_PRODUCT_LABEL1PLACEMENT_FIELD] = "PrLblPlc1";
    productRecToDatabase_[JSON_PRODUCT_LABEL2ROTATION_FIELD] = "PrLblRot2";
    productRecToDatabase_[JSON_PRODUCT_LABEL2PLACEMENT_FIELD] = "PrLblPlc2";
    productRecToDatabase_[JSON_PRODUCT_LABEL3ROTATION_FIELD] = "PrLblRot3";
    productRecToDatabase_[JSON_PRODUCT_LABEL3PLACEMENT_FIELD] = "PrLblPlc3";

    // COOL
    productRecToDatabase_[JSON_PRODUCT_DEFAULTCOOLTEXT_FIELD] = "DLXNum";
    productRecToDatabase_[JSON_PRODUCT_CURRENTCOOLTEXT_FIELD] = "LXNum";
    productRecToDatabase_[JSON_PRODUCT_VERIFYCOOL_FIELD] = "PrVCOLTxt";
    productRecToDatabase_[JSON_PRODUCT_COOLREQUIRED_FIELD] = "PrCOLReq";
    productRecToDatabase_[JSON_PRODUCT_COOLTRACKINGTEXT_FIELD] = "PrCOLTN";

    productRecToDatabase_[JSON_PRODUCT_COOLCATEGORY_FIELD] = "LCNum";
    productRecToDatabase_[JSON_PRODUCT_COOLPRETEXT_FIELD] = "LPNum";
    productRecToDatabase_[JSON_PRODUCT_COOLLISTKEY_FIELD] = "LLID";

    productRecToDatabase_[JSON_PRODUCT_EXP_TEXT1_FIELD] = "TxNum";
    productRecToDatabase_[JSON_PRODUCT_EXP_TEXT2_FIELD] = "ProdTxt7Num";
    productRecToDatabase_[JSON_PRODUCT_SPECIALMESSAGE_FIELD] = "SMNum";
    productRecToDatabase_[JSON_PRODUCT_NOTE_FIELD] = "NtNum";
    productRecToDatabase_[JSON_PRODUCT_MARQUEE_FIELD] = "MaNum";
    productRecToDatabase_[JSON_PRODUCT_COOKING_INST_FIELD] = "ProdTxt5Num";
    productRecToDatabase_[JSON_PRODUCT_RECIPE_FIELD] = "ProdTxt6Num";
    productRecToDatabase_[JSON_PRODUCT_SUGGEST_SELL_FIELD] = "ProdTxt8Num";
    productRecToDatabase_[JSON_PRODUCT_ALLERGENS_FIELD] = "ProdTxt9Num";
    productRecToDatabase_[JSON_PRODUCT_NUTRITION_FIELD] = "NFNum";

    // Pick-5
    productRecToDatabase_[JSON_PRODUCT_WEIGHTMIN_FIELD] = "PrWeightMin";
    productRecToDatabase_[JSON_PRODUCT_WEIGHTMAX_FIELD] = "PrWeightMax";
    productRecToDatabase_[JSON_PRODUCT_TOTALPRICEMIN_FIELD] = "PrTotalPriceMin";
    productRecToDatabase_[JSON_PRODUCT_TOTALPRICEMAX_FIELD] = "PrTotalPriceMax";

    // Automatic Mode Labels
    productRecToDatabase_[JSON_PRODUCT_MANUALLABELTYPENUM1_FIELD] = "PrManualLabelTypeNum1";
    productRecToDatabase_[JSON_PRODUCT_MANUALLABELTYPENUM2_FIELD] = "PrManualLabelTypeNum2";
    productRecToDatabase_[JSON_PRODUCT_MANUALLABELTYPENUM3_FIELD] = "PrManualLabelTypeNum3";
    productRecToDatabase_[JSON_PRODUCT_PREPACKLABELTYPENUM1_FIELD] = "PrPrepackLabelTypeNum1";
    productRecToDatabase_[JSON_PRODUCT_PREPACKLABELTYPENUM2_FIELD] = "PrPrepackLabelTypeNum2";
    productRecToDatabase_[JSON_PRODUCT_PREPACKLABELTYPENUM3_FIELD] = "PrPrepackLabelTypeNum3";
    productRecToDatabase_[JSON_PRODUCT_PRODENTRYLABELTYPENUM1_FIELD] = "PrProdEntryLabelTypeNum1";
    productRecToDatabase_[JSON_PRODUCT_PRODENTRYLABELTYPENUM2_FIELD] = "PrProdEntryLabelTypeNum2";
    productRecToDatabase_[JSON_PRODUCT_PRODENTRYLABELTYPENUM3_FIELD] = "PrProdEntryLabelTypeNum3";
    productRecToDatabase_[JSON_PRODUCT_SELFSERVLABELTYPENUM1_FIELD] = "PrSelfServLabelTypeNum1";
    productRecToDatabase_[JSON_PRODUCT_SELFSERVLABELTYPENUM2_FIELD] = "PrSelfServLabelTypeNum2";
    productRecToDatabase_[JSON_PRODUCT_SELFSERVLABELTYPENUM3_FIELD] = "PrSelfServLabelTypeNum3";
    productRecToDatabase_[JSON_PRODUCT_PICK5LABELTYPENUM1_FIELD] = "PrPick5LabelTypeNum1";
    productRecToDatabase_[JSON_PRODUCT_PICK5LABELTYPENUM2_FIELD] = "PrPick5LabelTypeNum2";
    productRecToDatabase_[JSON_PRODUCT_PICK5LABELTYPENUM3_FIELD] = "PrPick5LabelTypeNum3";

    createAntiMapping( productRecToDatabase_, productRecFromDatabase_ );

    // --------
    // Operator
    // --------

    operatorRecToDatabase_[JSON_OPERATOR_NUMBER_FIELD] = "OpNum";
    operatorRecToDatabase_[JSON_OPERATOR_RFID_FIELD] = "OpRFID";
    operatorRecToDatabase_[JSON_OPERATOR_NAME_FIELD] = "OpName";
    operatorRecToDatabase_[JSON_OPERATOR_LANGUAGE_FIELD] = "OpPLang";
    operatorRecToDatabase_[JSON_OPERATOR_CLASSIFICATION_FIELD] = "OpAccLev";

    operatorRecToDatabase_[JSON_OPERATOR_NOTE_FIELD] = ""; // FIXME: "ONotes";
    operatorRecToDatabase_[JSON_OPERATOR_SAWNOTE_FIELD] = "OpSawNote";

    createAntiMapping( operatorRecToDatabase_, operatorRecFromDatabase_ );

    // --------
    // Category
    // --------

    categoryRecToDatabase_[JSON_CATEGORY_NUMBER_FIELD] = "CaNum";
    categoryRecToDatabase_[JSON_CATEGORY_PARENTCATEGORY_FIELD] = "Cat_CaNum";
    categoryRecToDatabase_[JSON_CATEGORY_DEPTNUM_FIELD] = "DeptNum";
    categoryRecToDatabase_[JSON_CATEGORY_NAME1_FIELD] = "CaName1";
    categoryRecToDatabase_[JSON_CATEGORY_NAME2_FIELD] = "CaName2";
    categoryRecToDatabase_[JSON_CATEGORY_BACKGROUNDCOLOR_FIELD] = "CaBgClr";
    categoryRecToDatabase_[JSON_CATEGORY_TEXTCOLOR_FIELD] = "CaTxClr";
    categoryRecToDatabase_[JSON_CATEGORY_IMAGE_FIELD] = "CaImage";
    categoryRecToDatabase_[JSON_CATEGORY_PLAYLIST_FIELD] = "CaPList";

    createAntiMapping( categoryRecToDatabase_, categoryRecFromDatabase_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineOrderingDatabaseAdapter::createAntiMapping( const StringMapping& map, StringMapping& antiMap )
{
    antiMap.clear();

    // iterate map and create anti-map
    for ( StringMapping::const_iterator i( map.constBegin() ); i != map.constEnd(); ++i )
    {
        if ( i.value().isEmpty() )
            qWarning() << "no mapped value for" << i.key();
        else
            antiMap[i.value()] = i.key();
    }

    if ( map.size() != antiMap.size() )
        qWarning() << "anti map size does not match map size";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QJsonObject OnlineOrderingDatabaseAdapter::transform( const StringMapping& map, const QJsonObject& obj )
{
    QJsonObject result;

    // iterate object fields
    for ( QJsonObject::const_iterator i( obj.constBegin() ); i != obj.constEnd(); ++i )
    {
        const StringMapping::const_iterator key( map.find( i.key() ) );

        if ( map.constEnd() == key )
            qWarning() << "could not map key" << i.key();
        else if ( key.value().length() )
            result[key.value()] = i.value();
        else
            qWarning() << "unmapped key" << i.key();
    }

#ifdef DEBUG_JSON
    qDebug() << "XFORM\n" << result;
#endif

    return result;
}
