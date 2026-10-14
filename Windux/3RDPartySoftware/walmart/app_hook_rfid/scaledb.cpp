/**
 * @file scaledb.cpp
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#include "coolrec.h"
#include "common.h"
#include "dbutil.h"
#include "labelparser.h"
#include "productrec.h"
#include "scaledb.h"
#include "stringsjson.h"

#include <QDateTime>
#include <QJsonArray>
#include <QJsonObject>
#include <QSettings>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlField>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlTableModel>

#if defined(Q_OS_LINUX)
static const QString INTERNAL_CONFIG( "/usr/share/walmart-apphook/hobart_internal.config" );
#elif defined(Q_OS_WIN32)
static const QString INTERNAL_CONFIG( "hobart_internal.config" );
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
scaleDatabase::scaleDatabase( const QString& name, QObject *parent ) :
    _Mybase( parent ),
    dbname_( name ),
    scaleDatabase_( nullptr )
{
    LOG_DEBUG << "ctor";
    openDatabase();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
scaleDatabase::~scaleDatabase()
{
    LOG_DEBUG << "dtor";
    closeDatabase();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int scaleDatabase::activeProductCount( int dept ) const
{
    int result( 0 );

    if ( !isValid() )
        LOG_WARN << "not valid";
    else
    {
        QSqlDatabase conn( openDatabaseConnection() );

        QSqlTableModel tableModel( nullptr, conn );
        tableModel.setEditStrategy( QSqlTableModel::OnManualSubmit );
        tableModel.setTable( "prods" );

        LOG_DEBUG << "filtering for dept " << dept;

        QString filter( "deptnum=" + QString::number( dept ) );
        tableModel.setFilter( filter );

        LOG_TRACE << "retrieve active plu count...";

        // select
        if ( !tableModel.select() )
            LOG_DEBUG << "no product data";
        else
        {
            while ( tableModel.canFetchMore() )
                tableModel.fetchMore();

            result = tableModel.rowCount();
        }
    }

    LOG_DEBUG << "done " << result;
    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool scaleDatabase::isValid() const
{
    if ( scaleDatabase_ )
        return true;

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
scaleDatabase::labelVersionMap scaleDatabase::labelVersionInfo( int dept ) const
{
    labelVersionMap result;

    if ( !isValid() )
        LOG_WARN << "not valid";
    else
    {
        QSqlDatabase conn( openDatabaseConnection() );

        LOG_TRACE << "checking label versions...";

        QSqlTableModel tableModel( nullptr, conn );
        tableModel.setEditStrategy( QSqlTableModel::OnManualSubmit );
        tableModel.setTable( "labels" );

        LOG_DEBUG << "filtering for dept " << dept;

        QString filter( "deptnum=" + QString::number( dept ) );
        tableModel.setFilter( filter );

        // select
        if ( !tableModel.select() )
            LOG_DEBUG << "no label data";
        else
        {
            const int rows( tableModel.rowCount() );

            LOG_DEBUG << "iterate " << rows << " labels";

            for ( int row( 0 ); row < rows; ++row )
            {
                QSqlRecord record( tableModel.record( row ) );

                if ( record.isEmpty() )
                {
                    LOG_WARN << "empty record found";
                    continue;
                }

                // parse record
                QSqlField num( record.field( "lbnum" ) );
                QSqlField bin( record.field( "lbbin" ) );

                if (( !num.isValid() ) || ( !bin.isValid() ))
                {
                    LOG_WARN << "invalid label data";
                    continue;
                }

                const QString numValue( num.value().toString() );
                const QString binValue( bin.value().toString() );

                LOG_DEBUG << "parse label " << qPrintable( numValue );
                LOG_TRACE << qPrintable( binValue );

                const labelParser info( binValue );

                if ( !info.isValid() )
                {
                    LOG_WARN << "could not parse label data";
                    continue;
                }

                // save
                result[numValue.toInt()] = labelNameVersionPair( info.name(), info.version() );
            }
        }
    }

    LOG_TRACE << "checking label versions... complete";
    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QDateTime scaleDatabase::lastComm() const
{
    QSqlDatabase conn( openDatabaseConnection() );

    const QString result( queryConfigs( "communicationHistory" ) );

    LOG_DEBUG << "db value " << qPrintable( result );

    if (( result.isNull() ) || ( result.isEmpty() ))
        return QDateTime();

    const QDateTime dt( QDateTime::fromString( result, "yyyy-MM-dd hh:mm:ss" ) );

    LOG_TRACE << "converted value " << qPrintable( dt.toString() );

    return dt;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QDateTime scaleDatabase::lastItemChange() const
{
    QSqlDatabase conn( openDatabaseConnection() );

    const QString result( queryConfigs( "communicationItemReceived" ) );

    LOG_DEBUG << "db value " << qPrintable( result );

    if (( result.isNull() ) || ( result.isEmpty() ))
        return QDateTime();

    const QDateTime dt( QDateTime::fromString( result, "yyyy-MM-dd hh:mm:ss" ) );

    LOG_TRACE << "converted value " << qPrintable( dt.toString() );

    return dt;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool scaleDatabase::beginTransaction()
{
    QSqlDatabase conn( openDatabaseConnection() );

    if ( !conn.transaction() )
    {
        const QSqlError e( conn.lastError() );
        LOG_ERROR << "failed to begin transaction " << e.type() << " " << qPrintable( e.text() );

        return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool scaleDatabase::endTransaction()
{
    QSqlDatabase conn( openDatabaseConnection() );

    if ( conn.commit() )
        return true;
    else
    {
        const QSqlError e( conn.lastError() );
        LOG_ERROR << "failed to end transaction " << e.type() << " " << qPrintable( e.text() );
    }

    if ( !conn.rollback() )
    {
        const QSqlError e( conn.lastError() );
        LOG_ERROR << "failed to rollback transaction " << e.type() << " " << qPrintable( e.text() );

        closeDatabase();
        openDatabase();
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void scaleDatabase::deleteAllProductData()
{
    if ( !isValid() )
    {
        LOG_WARN << "not valid";
        return;
    }

    QSqlDatabase conn( openDatabaseConnection() );

    // *******************************************************************************************************
    // The following are the tables that are considered "PLU" data by chobart_d (from BackupRestoreManager)
    // Delete all records from these tables before processing data from the Walmart server.
    // *******************************************************************************************************

    dbutil::deleteFromTable(conn, "PrStats");       //    productManager->handleDeleteAllProductStatistic();

    dbutil::deleteFromTable(conn, "PMethds");       //    presentationMethodManager->handleDeleteAll();
    dbutil::deleteFromTable(conn, "SGraphs");       //    streamingGraphicManager->handleDeleteAll();

    dbutil::deleteFromTable(conn, "COLCats");       //    coolManager->handleDeleteAllCOOLCategories();
    dbutil::deleteFromTable(conn, "COLCtrs");       //    coolManager->handleDeleteAllCOOLCountries();
    dbutil::deleteFromTable(conn, "COLStps");       //    coolManager->handleDeleteAllCOOLProductionSteps();
    dbutil::deleteFromTable(conn, "COLPTxs");       //    coolManager->handleDeleteAllCOOLPretexts();
    dbutil::deleteFromTable(conn, "COLTxts");       //    coolManager->handleDeleteAllCOOLTexts();
    dbutil::deleteFromTable(conn, "COLLsts");       //    coolManager->handleDeleteAllCOOLShortlists();
    dbutil::deleteFromTable(conn, "COLLTxs");       //    coolManager->handleDeleteAllCOOLListTexts();

    dbutil::deleteFromTable(conn, "ProductTexts");  //    expandedTextManager->handleDeleteAll();
                                                    //    specialMessageManager->handleDeleteAll();
                                                    //    productNoteManager->handleDeleteAll();
                                                    //    productText5Manager->handleDeleteAll();
                                                    //    productText6Manager->handleDeleteAll();
                                                    //    productText7Manager->handleDeleteAll();
                                                    //    productText8Manager->handleDeleteAll();
                                                    //    productText9Manager->handleDeleteAll();
                                                    //    marqueeTextManager->handleDeleteAll();

    dbutil::deleteFromTable(conn, "Classes");       //    classManager->handleDeleteAll();
    dbutil::deleteFromTable(conn, "NutrFs");        //    nutrifactsManager->handleDeleteAll();
    dbutil::deleteFromTable(conn, "Categrs");       //    categoryManager->handleDeleteAll();
    dbutil::deleteFromTable(conn, "Spcials");       //    specialsManager->handleDeleteAll();

    dbutil::deleteFromTable(conn, "TrTypes");       //    transactionTypeManager->handleDeleteAll();

    dbutil::deleteFromTable(conn, "Prods");         //    productManager->handleDeleteAll();
    dbutil::deleteFromTable(conn, "Depts");         //    deptManager->handleDeleteAll();

    // Graphics are files in the filesystem, we should probably delete these too (used by Walmart?)
    //    categoryGraphicManager->handleDeleteAll();
    //    productGraphicManager->handleDeleteAll();
    //    specialsGraphicManager->handleDeleteAll();


    // **************************************************************************************************
    // Recreate the default / reserved records, in the tables that have them (from SessionManager)
    // **************************************************************************************************

    // Recreate the default PresentationMethod records - PresentationMethod::verifyReservedRecords(DBAccess *dbAccess)
    PresentationMethod_verifyReservedRecord(1, "Top Sellers");
    PresentationMethod_verifyReservedRecord(2, "Specials");
    PresentationMethod_verifyReservedRecord(3, "Keyboard Search");
    PresentationMethod_verifyReservedRecord(4, "PLU Entry");
    PresentationMethod_verifyReservedRecord(5, "Category/Subcategory");
    PresentationMethod_verifyReservedRecord(6, "Start Screen");

    // Recreate the default Transaction Type records.
    TransactionType_verifyReservedRecord(1, "Production", 1, 0, 0, 0, "0", 0, 0);

    // from SessionManager::verifyDeptBasedReservedRecords(&dba);
    // Recreate the default Class records - Class::verifyReservedRecords(DBAccess *dbAccess)
    Class_verifyReservedRecord(0, 999999, "Grand Totals");
    Class_verifyReservedRecord(0, 999998, "Void Totals");
    Class_verifyReservedRecord(0, 999997, "Price Change Totals");
    Class_verifyReservedRecord(0, 999995, "Lost Plu Totals", 999999);

    // Recreate the default Category records - Category::verifyReservedRecords(DBAccess *dbAccess)
    Category_verifyReservedRecord(0, 999999, 0, "Master Category", "");

    // Recreate the default Specials records - Special::verifyReservedRecords(DBAccess *dbAccess)
    Special_verifyReservedRecord(0, 999990, "New Products", "new", "new", 30);
    Special_verifyReservedRecord(0, 999991, "Price Reductions", "sale", "sale", 30);

    // Recreate the default Department records - Dept_verifyReservedRecords(DBAccess *dbAccess)
    Dept_verifyReservedRecord(0, "Default", "Default");
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool scaleDatabase::deleteProduct( int productNum, int dept )
{
    if ( !isValid() )
    {
        LOG_WARN << "not valid";
        return false;
    }

    int attempts( 3 );

    do
    {
        QSqlTableModel prodsModel( nullptr, openDatabaseConnection() );
        prodsModel.setTable( "Prods" );
        prodsModel.setEditStrategy( QSqlTableModel::OnManualSubmit );

        const QString filter( "PrNum=" + QString::number( productNum ) + " and DeptNum=" + QString::number( dept ) );

        // check for product record
        QSqlRecord prod;

        if ( !dbutil::selectRecord( prodsModel, filter, prod ) )
        {
            LOG_DEBUG << "product " << productNum << " does not exist ";
            return true;
        }

        if (( prodsModel.removeRow( 0 ) ) && ( prodsModel.submitAll() ))
        {
            LOG_DEBUG << "deleted product num " << productNum;
            return true;
        }

    } while ( 0 < --attempts );

    LOG_WARN << "failed to delete product num " << productNum;
    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool scaleDatabase::insertUpdateCoolCategory( const QJsonObject& category, int dept )
{
    if ( !isValid() )
    {
        LOG_WARN << "not valid";
        return false;
    }

    int attempts( 3 );

    do
    {
        QSqlDatabase conn( openDatabaseConnection() );

        coolRecord rec( coolRecord::Category, category, dept, this, conn );

        if ( !rec.isValid() )
            return false;
        else if ( rec.insertUpdate() )
            return true;

        LOG_WARN << "failed to save cool record";

    } while ( 0 < --attempts );

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool scaleDatabase::insertUpdateCoolCountry( const QJsonObject& country, int dept )
{
    if ( !isValid() )
    {
        LOG_WARN << "not valid";
        return false;
    }

    int attempts( 3 );

    do
    {
        QSqlDatabase conn( openDatabaseConnection() );

        coolRecord rec( coolRecord::Country, country, dept, this, conn );

        if ( !rec.isValid() )
            return false;
        else if ( rec.insertUpdate() )
            return true;

        LOG_WARN << "failed to save cool record";

    } while ( 0 < --attempts );

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool scaleDatabase::insertUpdateCoolPreText( const QJsonObject& preText, int dept )
{
    if ( !isValid() )
    {
        LOG_WARN << "not valid";
        return false;
    }

    int attempts( 3 );

    do
    {
        QSqlDatabase conn( openDatabaseConnection() );

        coolRecord rec( coolRecord::PreText, preText, dept, this, conn );

        if ( !rec.isValid() )
            return false;
        else if ( rec.insertUpdate() )
            return true;

        LOG_WARN << "failed to save cool record";

    } while ( 0 < --attempts );

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool scaleDatabase::insertUpdateCoolProdStep( const QJsonObject& prodStep, int dept )
{
    if ( !isValid() )
    {
        LOG_WARN << "not valid";
        return false;
    }

    int attempts( 3 );

    do
    {
        QSqlDatabase conn( openDatabaseConnection() );

        coolRecord rec( coolRecord::ProductionStep, prodStep, dept, this, conn );

        if ( !rec.isValid() )
            return false;
        else if ( rec.insertUpdate() )
            return true;

        LOG_WARN << "failed to save cool record";

    } while ( 0 < --attempts );

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool scaleDatabase::insertUpdateCoolText( const QJsonObject& text, int dept )
{
    if ( !isValid() )
    {
        LOG_WARN << "not valid";
        return false;
    }

    int attempts( 3 );

    do
    {
        QSqlDatabase conn( openDatabaseConnection() );

        coolRecord rec( coolRecord::Text, text, dept, this, conn );

        if ( !rec.isValid() )
            return false;
        else if ( rec.insertUpdate() )
            return true;

        LOG_WARN << "failed to save cool record";

    } while ( 0 < --attempts );

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool scaleDatabase::insertUpdateProduct( const QJsonObject& product, int dept )
{
    if ( !isValid() )
    {
        LOG_WARN << "not valid";
        return false;
    }

    // they want the ability to generate cool text from plu record updates... this is handled here
    if (( product.contains( JSON_COOL ) ) && ( product[JSON_COOL].isObject() ))
    {
        const QJsonObject cool( product[JSON_COOL].toObject() );

        if (( cool.contains( JSON_COOL_TEXT_LIST ) ) && ( cool[JSON_COOL_TEXT_LIST].isArray() ))
        {
            const QJsonArray a( cool[JSON_COOL_TEXT_LIST].toArray() );

            // add new records
            foreach ( const QJsonValue& v, a )
                if ( v.isObject() )
                {
                    const QJsonObject obj( v.toObject() );

                    if (( obj.contains( JSON_COOL_TEXT_NUM ) ) && ( obj[JSON_COOL_TEXT_NUM].isDouble() ) &&
                        ( obj.contains( JSON_COOL_TEXT_NAME ) ) && ( obj[JSON_COOL_TEXT_NAME].isString() ))
                    {
                        QJsonObject newCoolTextRec;
                        newCoolTextRec[JSON_COOL_TEXT_ID] = obj[JSON_COOL_TEXT_NUM];
                        newCoolTextRec[JSON_COOL_TEXT_NAME] = obj[JSON_COOL_TEXT_NAME];

                        LOG_DEBUG << "generating cool text record...";

                        if ( !insertUpdateCoolText( newCoolTextRec, dept ) )
                            LOG_WARN << "failed to create cool text record";
                    }
                }

        }
    }

    // parse json object
    const int productNum( product[JSON_PRODUCT_NUM].toInt() );
    const QString productDesc( product[JSON_DESCRIPTION].toString() );

    LOG_DEBUG << "product " << productNum << " '" << qPrintable( productDesc ) << "'";

    int attempts( 3 );

    do
    {
        QSqlDatabase conn( openDatabaseConnection() );

        productRecord rec( product, dept, this, conn );

        if ( !rec.isValid() )
            return false;
        else if ( rec.insertUpdate() )
            return true;

        LOG_WARN << "failed to save product " << productNum;

    } while ( 0 < --attempts );

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString scaleDatabase::queryConfigs( const QString& configName ) const
{
    QString configValue;

    if ( !isValid() )
        LOG_WARN << "not valid";
    else
    {
        QSqlDatabase conn( openDatabaseConnection() );

        QSqlTableModel dbConfigsRM( nullptr, conn );
        dbConfigsRM.setTable( "Configs" );
        dbConfigsRM.setEditStrategy( QSqlTableModel::OnManualSubmit );

        QSqlRecord dbConfigRec;

        const QString filter( "CoName='" + configName + "'" );

        if ( dbutil::selectRecord( dbConfigsRM, filter, dbConfigRec ) )
        {
            if ( !dbConfigRec.isEmpty() )
                configValue = dbConfigRec.value( 1 ).toString();

            LOG_TRACE << "Configs read " << qPrintable( filter ) << "  value = " << qPrintable( configValue );
        }
        else
        {
            LOG_WARN << "Could not get Configs from db " << qPrintable( filter );
        }
    }

    return configValue;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void scaleDatabase::closeDatabase()
{
    if ( scaleDatabase_ )
    {
        LOG_INFO << "closing database ...";
        scaleDatabase_->close();

        delete scaleDatabase_;
        scaleDatabase_ = nullptr;
    }

    LOG_TRACE << "exit";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool scaleDatabase::openDatabase()
{
    const QString SQLITE3_DRIVER( "QSQLITE" );

    if ( scaleDatabase_ )
    {
        LOG_DEBUG << "already open";
        return true;
    }

    LOG_DEBUG << "open db " << qPrintable( dbname_ );

    if ( dbname_.isEmpty() )
        LOG_WARN << "empty db name";
    else if ( !QSqlDatabase::isDriverAvailable( SQLITE3_DRIVER ) )
        LOG_ERROR << "database driver " << qPrintable( SQLITE3_DRIVER ) << " is not available!";
    else
    {
        scaleDatabase_ = new QSqlDatabase( QSqlDatabase::addDatabase( SQLITE3_DRIVER ) );

        LOG_INFO << "opening database " << qPrintable( dbname_ ) << " ...";
        scaleDatabase_->setDatabaseName( dbname_ );

        if ( scaleDatabase_->open() )
        {
            LOG_INFO << "open database success";
            LOG_INFO << "country code " << qPrintable( queryConfigs( "primaryCountryCode" ) );
            LOG_INFO << "store " << qPrintable( queryConfigs( "storeId" ) );

            return true;
        }
        else
        {
            const QSqlError e( scaleDatabase_->lastError() );

            LOG_ERROR << "failed to open database " << e.type() << " " << qPrintable( e.text() );

            delete scaleDatabase_;
            scaleDatabase_ = nullptr;
        }
    }

    LOG_TRACE << "done";
    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QSqlDatabase scaleDatabase::openDatabaseConnection() const
{
    const QString connName( QString::number( (quintptr) QThread::currentThreadId() ) );

    QSqlDatabase db( QSqlDatabase::database( connName ) );

    if (( !db.isOpen() ) || ( !db.isValid() ))
    {
       db = QSqlDatabase::cloneDatabase( *scaleDatabase_, connName );

       if ( !db.open() )
           LOG_ERROR << "failed to open db connection";
    }

    return db;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void scaleDatabase::PresentationMethod_verifyReservedRecord( int PMType, const QString& PMName1 )
{
    QSqlDatabase conn( openDatabaseConnection() );

    QSqlTableModel dbRM( nullptr, conn );
    dbRM.setTable("PMethds");
    dbRM.setEditStrategy(QSqlTableModel::OnManualSubmit);
    QSqlRecord dbRec;

    const QString filter( "PMType=" + QString::number(PMType) );
    //LOG_DEBUG << "filter = " << filter.toStdString();
    dbRM.setFilter(filter);

    bool dbRecExists = dbRM.select();
    int rowCount = dbRM.rowCount();
    //LOG_DEBUG << "PMethds select, select() returned " << dbRecExists << "  rowCount = " << rowCount;

    if (dbRecExists && rowCount == 0)
    {
        // Get an empty record as the basis for a new Production Method record
        dbRec = dbRM.record();

        dbRec.setValue("PMType", QVariant(PMType));
        dbRec.setValue("PMName1", QVariant(PMName1));
        dbRec.setValue("PMCheck", QVariant((int)1));

        dbutil::saveRecordToDatabase( dbRM, "PMethds", dbRec );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void scaleDatabase::TransactionType_verifyReservedRecord( int TTNum, const QString& TTName1, int TTAffPr, int TTBkDt, int TTAutoUPC, int TTManUPC, const QString& TTUPCDigit, int TTUseBgClr, int TTBgColor )
{
    QSqlDatabase conn( openDatabaseConnection() );

    QSqlTableModel dbRM( nullptr, conn );
    dbRM.setTable("TrTypes");
    dbRM.setEditStrategy(QSqlTableModel::OnManualSubmit);
    QSqlRecord dbRec;

    const QString filter( "TTNum=" + QString::number(TTNum) );
    //LOG_DEBUG << "filter = " << filter.toStdString();
    dbRM.setFilter(filter);

    bool dbRecExists = dbRM.select();
    int rowCount = dbRM.rowCount();
    //LOG_DEBUG << "TrTypes select, select() returned " << dbRecExists << "  rowCount = " << rowCount;

    if (dbRecExists && rowCount == 0)
    {
        // Get an empty record as the basis for a new Transaction Type record
        dbRec = dbRM.record();

        dbRec.setValue("TTNum", QVariant(TTNum));
        dbRec.setValue("TTName1", QVariant(TTName1));
        dbRec.setValue("TTAffPr", QVariant(TTAffPr));
        dbRec.setValue("TTBkDt", QVariant(TTBkDt));
        dbRec.setValue("TTAutoUPC", QVariant(TTAutoUPC));
        dbRec.setValue("TTManUPC", QVariant(TTManUPC));
        dbRec.setValue("TTUPCDigit", QVariant(TTUPCDigit));
        dbRec.setValue("TTUseBgClr", QVariant(TTUseBgClr));
        dbRec.setValue("TTBgColor", QVariant(TTBgColor));
        dbRec.setValue("TTCheck", QVariant((int)1));

        dbutil::saveRecordToDatabase( dbRM, "TrTypes", dbRec );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void scaleDatabase::Class_verifyReservedRecord( int DeptNum, int ClNum, const QString& ClDesc1, int Cla_ClNum )
{
    QSqlDatabase conn( openDatabaseConnection() );

    QSqlTableModel dbRM( nullptr, conn );
    dbRM.setTable("Classes");
    dbRM.setEditStrategy(QSqlTableModel::OnManualSubmit);
    QSqlRecord dbRec;

    const QString filter("ClNum=" + QString::number(ClNum) + " and DeptNum=" + QString::number(DeptNum));
    //LOG_DEBUG << "filter = " << filter.toStdString();
    dbRM.setFilter(filter);

    bool dbRecExists = dbRM.select();
    int rowCount = dbRM.rowCount();
    //LOG_DEBUG << "Classes select, select() returned " << dbRecExists << "  rowCount = " << rowCount;

    if (dbRecExists && rowCount == 0)
    {
        // Get an empty record as the basis for a new Class record
        dbRec = dbRM.record();

        dbRec.setValue("DeptNum", QVariant(DeptNum));
        dbRec.setValue("ClNum", QVariant(ClNum));
        dbRec.setValue("ClDesc1", QVariant(ClDesc1 + " \nCreated on " + QDateTime::currentDateTime().toString("MMM d, yy") ));
        if (Cla_ClNum != -1)
            dbRec.setValue("Cla_ClNum", QVariant(Cla_ClNum));
        dbRec.setValue("ClCheck", QVariant((int)1));

        dbutil::saveRecordToDatabase( dbRM, "Classes", dbRec );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void scaleDatabase::Category_verifyReservedRecord( int DeptNum, int CaNum, int Cat_CaNum, const QString& CaName1, const QString& CaName2 )
{
    QSqlDatabase conn( openDatabaseConnection() );

    QSqlTableModel dbRM( nullptr, conn );
    dbRM.setTable("Categrs");
    dbRM.setEditStrategy(QSqlTableModel::OnManualSubmit);
    QSqlRecord dbRec;

    const QString filter("CaNum=" + QString::number(CaNum) + " and DeptNum=" + QString::number(DeptNum));
    //LOG_DEBUG << "filter = " << filter.toStdString();
    dbRM.setFilter(filter);

    bool dbRecExists = dbRM.select();
    int rowCount = dbRM.rowCount();
    //LOG_DEBUG << "Categrs select, select() returned " << dbRecExists << "  rowCount = " << rowCount;

    if (dbRecExists && rowCount == 0)
    {
        // Get an empty record as the basis for a new Categrs record
        dbRec = dbRM.record();

        dbRec.setValue("DeptNum", QVariant(DeptNum));
        dbRec.setValue("CaNum", QVariant(CaNum));
        dbRec.setValue("Cat_CaNum", QVariant(Cat_CaNum));
        dbRec.setValue("CaName1", QVariant(CaName1));
        dbRec.setValue("CaName2", QVariant(CaName2));
        dbRec.setValue("CaCheck", QVariant((int)1));

        dbutil::saveRecordToDatabase( dbRM, "Categrs", dbRec );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void scaleDatabase::Special_verifyReservedRecord( int DeptNum, int SPNum, const QString& SPName1, const QString& SPImag1, const QString& SPImag2, int SPDefL )
{
    QSqlDatabase conn( openDatabaseConnection() );

    QSqlTableModel dbRM( nullptr, conn );
    dbRM.setTable("Spcials");
    dbRM.setEditStrategy(QSqlTableModel::OnManualSubmit);
    QSqlRecord dbRec;

    const QString filter("SPNum=" + QString::number(SPNum) + " and DeptNum=" + QString::number(DeptNum));
    //LOG_DEBUG << "filter = " << filter.toStdString();
    dbRM.setFilter(filter);

    bool dbRecExists = dbRM.select();
    int rowCount = dbRM.rowCount();
    //LOG_DEBUG << "Spcials select, select() returned " << dbRecExists << "  rowCount = " << rowCount;

    if (dbRecExists && rowCount == 0)
    {
        // Get an empty record as the basis for a new Spcials record
        dbRec = dbRM.record();

        dbRec.setValue("DeptNum", QVariant(DeptNum));
        dbRec.setValue("SPNum", QVariant(SPNum));
        dbRec.setValue("SPName1", QVariant(SPName1));
        dbRec.setValue("SPImag1", QVariant(SPImag1));
        dbRec.setValue("SPImag2", QVariant(SPImag2));
        dbRec.setValue("SPDefL", QVariant(SPDefL));
        dbRec.setValue("SPCheck", QVariant((int)1));

        dbutil::saveRecordToDatabase( dbRM, "Spcials", dbRec );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void scaleDatabase::Dept_verifyReservedRecord( int DeptNum, const QString& DeptName1, const QString& DeptName2 )
{
    QSqlDatabase conn( openDatabaseConnection() );

    QSqlTableModel dbRM( nullptr, conn );
    dbRM.setTable("Depts");
    dbRM.setEditStrategy(QSqlTableModel::OnManualSubmit);
    QSqlRecord dbRec;

    const QString filter("DeptNum=" + QString::number(DeptNum));
    //LOG_DEBUG << "filter = " << filter.toStdString();
    dbRM.setFilter(filter);

    bool dbRecExists = dbRM.select();
    int rowCount = dbRM.rowCount();
    //LOG_DEBUG << "Depts select, select() returned " << dbRecExists << "  rowCount = " << rowCount;

    if (dbRecExists && rowCount == 0)
    {
        // Get an empty record as the basis for a new Depts record
        dbRec = dbRM.record();

        dbRec.setValue("DeptNum", QVariant(DeptNum));
        dbRec.setValue("DeptName1", QVariant(DeptName1));
        dbRec.setValue("DeptName2", QVariant(DeptName2));
        dbRec.setValue("DeptCheck", QVariant((int)1));

        dbutil::saveRecordToDatabase( dbRM, "Depts", dbRec );
    }
}

