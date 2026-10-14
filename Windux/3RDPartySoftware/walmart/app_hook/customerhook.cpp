#include "apiwalmart.h"
#include "customerhook.h"
#include "errordialogmgr.h"
#include "healthstatusthread.h"
#include "labelparser.h"
#include "networkaccess.h"
#include "networkcheckingthread.h"

#include <QByteArray>
#include <QFile>
#include <QHostAddress>
#include <QHostInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMap>
#include <QNetworkInterface>
#include <QSettings>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlField>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlRecord>
#include <QSqlTableModel>
#include <QString>
#include <QStringList>
#include <QThread>
#include <QVariant>

#include <QtConcurrent>

#include <clio.h>

const char *customerHook::version_ = "0.0.1";

QMutex customerHook::instanceMutex_;
customerHook *customerHook::instance_( nullptr );

///////////////////////////////////////////////////////////////////////////////////////////////////
customerHook::customerHook( QObject *parent ) :
    _Mybase( parent ),
    m_( QMutex::Recursive ),
    scaleDatabase_( nullptr ),
    dbname_( ""),
    country_( "" ),
    storenum_( "" ),
    serialnumber_( "" ),
    cacheexpire_( "" ),
    connectionCheckInterval_( 60 * 1000 ),
    net_( nullptr ),
    ignoreAllSslErrors_( false ),
    ignoreSslHostNameMismatch_( false ),
    ignoreSslSelfSignedCert_( false ),
    lockout_( false ),
    healthCheckInterval_( 60 * 60 * 1000 ),
    healthCheckFormat_( "US" ),
    health_( nullptr )
{
    LOG_INFO << "customerHook Constructor...";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
customerHook::~customerHook()
{
    LOG_DEBUG << "customerHook Destructor";
    closeDatabase();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString customerHook::country() const
{
    QMutexLocker guard( &m_ );
    return country_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void customerHook::setCountry( const QString &value )
{
    QMutexLocker guard( &m_ );
    country_ = value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString customerHook::storeNum() const
{
    QMutexLocker guard( &m_ );
    return storenum_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void customerHook::setStoreNum( const QString &value )
{
    QMutexLocker guard( &m_ );
    storenum_ = value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString customerHook::model() const
{
    QMutexLocker guard( &m_ );
    return model_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void customerHook::setModel( const QString &value )
{
    QMutexLocker guard( &m_ );
    model_ = value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString customerHook::serialNum() const
{
    QMutexLocker guard( &m_ );
    return serialnumber_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void customerHook::setSerialNum( const QString &value )
{
    QMutexLocker guard( &m_ );
    serialnumber_ = value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString customerHook::ipAddress() const
{
    QMutexLocker guard( &m_ );
    return ipAddress_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void customerHook::setIpAddress( const QString& value )
{
    QMutexLocker guard( &m_ );

    // nothing to do
    if ( ipAddress_ == value )
        return;

    // refresh interfaces
    foreach ( walmartapi *api, api_.values() )
    {
        api->setHttpHeadersMap( "IP_ADDRESS", value );
        api->setHttpHeadersMap( "SCALE_NUMBER", scaleNumberFromIpAddress( value ) );
    }

    LOG_DEBUG << "IP = " << value.toStdString();
    ipAddress_ = value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString customerHook::macAddress() const
{
    QMutexLocker guard( &m_ );
    return macAddress_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void customerHook::setMacAddress( const QString& value )
{
    QMutexLocker guard( &m_ );
    macAddress_ = value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int customerHook::activePluCount()
{
    QMutexLocker guard( &m_ );

    int result( 0 );

    if ( !scaleDatabase_ )
        return result;

    // save off current date time
    const qint64 now( QDateTime::currentDateTime().toMSecsSinceEpoch() );

    LOG_TRACE << "current date time is " << now;

    // compute cache time
    qint64 cacheTime( cacheExpiration().toInt() );

    if ( 0 < cacheTime )
    {
        cacheTime *= 1000;                          // convert to ms

        if ( "h" == cacheExpirationUnits() )
            cacheTime *= 3600;                      // convert to hr
        else if ( "m" == cacheExpirationUnits() )
            cacheTime *= 60;                        // convert to min
    }

    LOG_TRACE << "cache time is " << cacheTime;

    QSqlTableModel tableModel( this, (*scaleDatabase_) );
    tableModel.setEditStrategy( QSqlTableModel::OnManualSubmit );
    tableModel.setTable( "prods" );

    LOG_TRACE << "retrieve active plu count...";

    // select
    if ( !tableModel.select() )
        LOG_DEBUG << "no product data";
    else
    {
        const int rows( tableModel.rowCount() );

        // if cache never expires then all products are valid
        if ( cacheTime < 0 )
        {
            LOG_DEBUG << "cache is set to never expire";
            result = rows;
        }

        // iterate each product, evaluate expiry
        else
        {
            LOG_DEBUG << "iterate " << rows << " products";

            for ( int row( 0 ); row < rows; ++row )
            {
                QSqlRecord record( tableModel.record( row ) );

                if ( record.isEmpty() )
                {
                    LOG_WARN << "empty record found";
                    continue;
                }

                // parse record
                QSqlField num( record.field( "prnum" ) );
                QSqlField created( record.field( "prcdnm1" ) );

                if (( !num.isValid() ) || ( !created.isValid() ))
                {
                    LOG_WARN << "invalid product data";
                    continue;
                }

                const QVariant numValue( num.value() );
                const QVariant createdValue( created.value() );

                if ( numValue.isNull() )
                {
                    LOG_WARN << "invalid product data";
                    continue;
                }

                // null created datetime is fine, means its value
                if ( createdValue.isNull() )
                    ++result;

                // check when expired
                else
                {
                    qint64 expiry( createdValue.toLongLong() );
                    expiry += cacheTime;

                    LOG_TRACE << "prod " << numValue.toLongLong() << " expiry is " << expiry;

                    // product is valid
                    if ( now <= expiry )
                        ++result;

                    // product is expired
                    else
                    {
                        const QDateTime expiryDateTime( QDateTime::fromMSecsSinceEpoch( expiry ) );
                        const QString expiryDateTimeStr( expiryDateTime.toString() );

                        LOG_DEBUG << "expired product " << numValue.toLongLong() << " " << expiryDateTimeStr.toStdString();
                    }
                }
            }
        }
    }

    LOG_TRACE << "retrieve active plu count... complete";
    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
customerHook::labelVersionMap customerHook::labelVersionInfo( int dept )
{
    QMutexLocker guard( &m_ );

    labelVersionMap result;

    if ( !scaleDatabase_ )
        return result;

    LOG_TRACE << "checking label versions...";

    QSqlTableModel tableModel( this, (*scaleDatabase_) );
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

            LOG_DEBUG << "parse label " << numValue.toStdString();
            LOG_TRACE << binValue.toStdString();

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

    LOG_TRACE << "checking label versions... complete";
    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
customerHook *customerHook::instance()
{
    if ( !instance_ )
    {
        QMutexLocker guard( &instanceMutex_ );

        if ( !instance_ )
            instance_ = new _Myt();
    }

    return instance_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void customerHook::destroy()
{
    if ( instance_ )
    {
        QMutexLocker guard( &instanceMutex_ );

        if ( instance_ )
        {
            delete instance_;
            instance_ = nullptr;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool customerHook::init()
{
    QMutexLocker guard( &m_ );

    LOG_INFO << "==================================================";
    LOG_INFO << "walmart customer hook";
    LOG_INFO << "version: " << version_;
    LOG_INFO << "build date: " << __DATE__ << " " << __TIME__;
    LOG_INFO << "==================================================";
    
    // read config
    readConfiguration();

    LOG_DEBUG << "conn check interval " << connectionCheckInterval();
    LOG_DEBUG << "health check interval " << healthCheckInterval();

    // read serial number info
    querySerialNumber();

    // start network checking thread
    if ( connectionCheckInterval() )
    {
        net_ = new networkCheckingThread( this );
        net_->setCheckingInterval( connectionCheckInterval() );

        net_->setOfflineTimerMax( offlinePrintDialog1.first );                              // should this be cache timer?
        net_->setDialogTimes( 0, offlineVerifyDialog1, offlineVerifyDialog1_Freq );
        net_->setDialogTimes( 1, offlineVerifyDialog2, offlineVerifyDialog2_Freq );

        net_->start( QThread::IdlePriority );
    }

    // start health check thread
    if ( healthCheckInterval() )
    {
        health_ = new healthStatusThread( this );
        health_->setCheckingInterval( healthCheckInterval() );
        health_->setFormat( healthCheckFormat() );

        health_->start( QThread::IdlePriority );
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void customerHook::configSaved()
{
    QMutexLocker guard( &m_ );

    LOG_TRACE << "config saved";

    setCountry( queryConfigs( "primaryCountryCode" ) );
    setStoreNum( queryConfigs( "storeId" ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void customerHook::closeDatabase()
{
    LOG_TRACE << "entry";

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
void customerHook::beforeOpenDatabase( const QString& dbname )
{
    QMutexLocker guard( &m_ );

    //LOG_DEBUG << "Begin - dbname_=" << dbname_.toStdString() << "  dbname=" << dbname.toStdString();

    dbname_ = dbname;

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void customerHook::openDatabase()
{
    QMutexLocker guard( &m_ );

    const QString SQLITE3_DRIVER( "QSQLITE" );

    LOG_DEBUG << "open - dbname_=" << dbname_.toStdString();

    if ( dbname_.isEmpty() )
        return;

    if ( scaleDatabase_ )
        LOG_DEBUG << "database already open";
    else if ( !QSqlDatabase::isDriverAvailable( SQLITE3_DRIVER ) )
        LOG_ERROR << "database driver " << SQLITE3_DRIVER.toStdString() << " is not available!";
    else
    {
        scaleDatabase_ = new QSqlDatabase( QSqlDatabase::addDatabase( SQLITE3_DRIVER ) );

        LOG_INFO << "opening database " << dbname_.toStdString() << " ...";
        scaleDatabase_->setDatabaseName( dbname_ );

        if ( scaleDatabase_->open() )
        {
            LOG_INFO << "open database success";
/*
            {
                QString walModeSQL("PRAGMA journal_mode=WAL");
                QSqlQuery setupQuery(walModeSQL, *scaleDatabase_);

                QSqlError err( setupQuery.lastError() );
                LOG_DEBUG << "Result of query '" << walModeSQL.toStdString()
                             << "'   error type: " << err.type()
                             << "   native error code: " << err.nativeErrorCode().toStdString()
                             << "   error text: " << err.text().toStdString()
                             << "   database text: " << err.databaseText().toStdString()
                             << "   driver text: " << err.driverText().toStdString();
            }
*/
            setCountry( queryConfigs( "primaryCountryCode" ) );
            setStoreNum( queryConfigs( "storeId" ) );
        }
        else
        {
            const QSqlError e( scaleDatabase_->lastError() );

            LOG_ERROR << "failed to open database " << e.type() << " " << e.text().toStdString();

            delete scaleDatabase_;
            scaleDatabase_ = nullptr;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool customerHook::beforePluRead(const QString& plu, const QString& deptNum)
{
    QMutexLocker guard( &m_ );

    LOG_DEBUG << "Begin request for - pluNum = " << plu.toStdString() << "  deptNum = " << deptNum.toStdString();

    //
    // offline lockout dialog
    //

    const int offlineTimerMax( offlinePrintDialog1.first );

    const QDateTime lastSuccess( net_->lastOnlineTime() );
    const QDateTime now( QDateTime::currentDateTime() );

    if ( net_->isOnline() )
        LOG_TRACE << "online";
    else if (( 0 < offlineTimerMax ) && ( lastSuccess.isValid() ) && ( lastSuccess <= now ))
    {
        // compute offline countdown timer
        const int offlineAmount( lastSuccess.msecsTo( now ) );

        if ( 0 < offlineAmount )
        {
            const int countdownTimer( offlineTimerMax - offlineAmount );
            const int countdownTimerHours( std::ceil( (double)(countdownTimer) / (double)(60 * 60 * 1000) ) );

            LOG_DEBUG << "countdown timer at " << countdownTimer << " (" << countdownTimerHours << " hr)";

            // AF-5.c
            // countdown at 0 hours
            if ( countdownTimer <= 0 )
            {
                if ( lockout_ )
                    LOG_DEBUG << "lockout";
                else
                {
                    LOG_WARN << "!!!! LOCKOUT !!!!";
                    LOG_WARN << "CONTACT FIELD SERVICES TO RAISE AN INCIDENT";
                    LOG_WARN << "scale offline since " << lastSuccess.toString().toStdString();
                    LOG_WARN << "scale offline for " << offlineAmount;
                    LOG_WARN << "initial countdown timer was " << offlineTimerMax;
                    LOG_WARN << "countdown timer at " << countdownTimer << " (" << countdownTimerHours << " hr)";
                    LOG_WARN << "REBOOT IN ORDER TO USE SCALE AGAIN";
                    LOG_WARN << "!!!! LOCKOUT !!!!";

                    errorDialogManager::instance()->showNotAvailable( lastSuccess );
                    lockout_ = true;
                }

                return false;
            }
        }
    }

    // **************************************************************
    // Initially attempt to read the plu from the scale database
    // **************************************************************

    bool pluRecExistsInDb = false;
    qint64 pluRecExpiryTime = 0;
    {
        QString initialPluSQL("select DeptNum, PrNum, PrCdNm1 from prods where DeptNum=" + deptNum + " and PrNum=" + plu);
        QSqlQuery initialPluQuery(initialPluSQL, *scaleDatabase_);

        QSqlError err( initialPluQuery.lastError() );
        LOG_DEBUG << "Result of query '" << initialPluSQL.toStdString()
                     << "'   error type: " << err.type()
                     << "   native error code: " << err.nativeErrorCode().toStdString()
                     << "   error text: " << err.text().toStdString()
                     << "   database text: " << err.databaseText().toStdString()
                     << "   driver text: " << err.driverText().toStdString();

        if (initialPluQuery.first())
        {
            // column numbers are based on the ordering in the select statement above
            if ( initialPluQuery.value(0).toString() == deptNum && initialPluQuery.value(1).toString() == plu )
            {
                // Yes, the plu we're looking for already exists in the database
                pluRecExistsInDb = true;

                qint64 currTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
                pluRecExpiryTime = initialPluQuery.value(2).toString().toLongLong();

                // Phase 2 testing for server lookup rules
                if( testServerLookupRules( pluRecExpiryTime, pluRecExistsInDb ) )
                    return true;

                // If we just read this PLU from the server within the last "configurable amount of time" (say, 5 seconds)
                // and we got the PLU from the Walmart server, then don't go get it from the server again;
                // it's going to be there, and the data is most likely the same as what we just read.
                // So return to chobart_d and tell it that it's ok to lookup the plu.
                if ( currTime - pluRecExpiryTime <= minRelookupTime_.toLongLong() )
                {
                    LOG_DEBUG << "Just received this PLU within the last " << minRelookupTime_.toStdString() << "ms; no need to get it again";
                    return true;
                }
            }
        }
    }


    // **************************************************************
    // See if the PLU is on the Walmart Server
    // **************************************************************

    QJsonDocument doc;

    LOG_TRACE << "invoke api...";
    walmartapi::RequestStatus result = api()->getOneItem( plu, country(), storeNum(), deptNum, doc );

    LOG_TRACE << "invoke api complete " << result;

    QDateTime stamp;
    QString endpoint;

    if ( walmartapi::importLastUsage( stamp, endpoint ) )
    {
        const int delta( stamp.msecsTo( QDateTime::currentDateTime() ) );
        const int maxTime( minRelookupTime_.toInt() );

        LOG_TRACE << "api last usage delta " << delta << " with max time " << maxTime;

        // api was able to connect, we are probably online
        if (( 0 < delta ) && ( delta < maxTime ))
        {
            LOG_DEBUG << "updating network checking thread with most recent api stamp";
            net_->setLastOnlineTime( stamp );
        }
    }

    LOG_TRACE << "process result...";

    if ( result == walmartapi::SUCCESS )
    {
        // **************************************************************
        // The PLU is on the Walmart Server
        // **************************************************************

        LOG_DEBUG << "api()->getOneItem returned SUCCESS - we received the PLU data from the server";


        const QByteArray a( doc.toBinaryData() );
        //LOG_DEBUG << "customerHook::beforePluRead  " << HEX_DUMP( a.constData(), a.length() );               // prints out everything so for now I commented out

        //LOG_DEBUG << "QJsonDocument is object = " << doc.isObject();
        //LOG_DEBUG << "QJsonDocument is array = " << doc.isArray();

        QJsonObject pluJsonObj;
        if ( doc.isArray() )
        {
            // Walmart is returning the data as a JSON array of JSON PLU objects, currently an array of a single PLU object
            QJsonArray jsonArray = doc.array();
            //LOG_DEBUG << "jsonArray size = " << jsonArray.size();

            // The array will contain a single PLU object, so just get offset 0.
            QJsonValueRef val = jsonArray[0];

            // Refer to the PLU object as a QJsonObject
            pluJsonObj = val.toObject();
        }
        else if ( doc.isObject() )
        {
            // Handle it in the case that Walmart just sends us a single PLU object in the JSON data, not an array of PLU objects
            // Refer to the PLU object as a QJsonObject
            pluJsonObj = doc.object();
        }
        else
        {
            LOG_WARN << "PLU JSON Object received is not an array or an object";
            return false;
        }

        // Look for the plu number field in the JSON data and make sure the plu number matches the one the operator is looking for
        QJsonValue keyData = pluJsonObj["pluNbr"];
        //LOG_DEBUG << "PLU JSON Object received, pluNbr = " << keyData.toDouble() << "  PLU " << plu.toStdString();
        if ( QString::number(keyData.toDouble()) != plu )
        {
            LOG_WARN << "PLU JSON Object received is not for the desired PLU " << plu.toStdString();
            return false;
        }

        updatePriceWeightFields(pluJsonObj);

        // fixup UPC field as per Walmart rules
        if (( pluJsonObj.contains( "upc" ) ) && ( pluJsonObj["upc"].isString() ) &&
            ( pluJsonObj.contains( "upcType" ) ) && ( pluJsonObj["upcType"].isString() ))
        {
            const QString upcData = pluJsonObj["upc"].toString();
            const QString upcTypeData = pluJsonObj["upcType"].toString();

            LOG_DEBUG << "upc info " << upcTypeData.toStdString() << " " << upcData.toStdString();

            if (( "02" == upcTypeData ) && ( 11 == upcData.length() ) && ( '2' == upcData[0] ))
            {
                LOG_TRACE << "fixup UPC 2";
                pluJsonObj["upc"] = upcData.mid( 1 );
            }
        }

        if( verboseDebug_ )
            printJsonData( pluJsonObj );

        bool recordAdded = updateDbWithPlu(deptNum, pluJsonObj);
        LOG_DEBUG << "Saved Prods to db for PLU, chobart_d can load PLU " << plu.toStdString();
        return recordAdded;
    }
    else if ( result == walmartapi::CACHEDATA )
    {
        // ****************************************************************************
        // Could not communicate with the Walmart Server for some reason;
        // use the PLU in the scale's database, if it exists and it's not expired.
        // ****************************************************************************

        LOG_DEBUG << "api()->getOneItem returned CACHEDATA ";

        // If the PLU is not in the database
        if ( !pluRecExistsInDb )
        {
            //LOG_DEBUG << "Using CACHEDATA, but the PLU doesn't exist in the database, PLU " << plu.toStdString();
            return false;
        }

        // If the PLU in the database is not 'expired', respond back to chobart_d to go ahead and lookup and use the PLU in the scale's database (the cache).
        if ( !isCacheTimeExpired(pluRecExpiryTime) )
        {
            LOG_DEBUG << "Using CACHEDATA, the PLU exists, and is not expired, for PLU " << plu.toStdString();
            return true;
        }

        // If deleting-from-cache is not enabled, then don't delete an expired PLU.
        // But also, since it's expired, don't let the operator use it.
        if ( !RemoveItemsFromCache() )
        {
            //LOG_DEBUG << "Using CACHEDATA, the PLU exists, but is expired, but not deleting from cache, for PLU " << plu.toStdString();
            return false;
        }

        deletePluFromDb(plu, deptNum);

        return false;
    }
    else  // walmartapi::NODATA
    {
        // **************************************************************
        // The PLU is NOT on the Walmart Server
        // **************************************************************

        LOG_DEBUG << "api()->getOneItem returned NODATA ";

        if ( pluRecExistsInDb )
            deletePluFromDb(plu, deptNum);

        return false;
    }

    return false;
}


bool customerHook::updateDbWithPlu(const QString& deptNum, QJsonObject pluJsonObj)
{
    //LOG_DEBUG << "pluJsonObj Size = " << pluJsonObj.size();
    if ( pluJsonObj.isEmpty() )
    {
        LOG_WARN << "PLU JSON Object received is empty";
        return false;
    }

    QStringList keyList = pluJsonObj.keys();
    if ( keyList.isEmpty() )
    {
        LOG_WARN << "PLU JSON Object received has no keyList";
        return false;
    }

    // Look for the plu number field in the JSON data and make sure the plu number matches the one the operator is looking for
    QJsonValue keyData = pluJsonObj["pluNbr"];
    QString plu = QString::number(keyData.toDouble());
    LOG_DEBUG << "PLU JSON Object received, pluNbr = " << plu.toStdString();

    QSqlTableModel dbProductsRM;
    dbProductsRM.setTable("Prods");
    dbProductsRM.setEditStrategy(QSqlTableModel::OnManualSubmit);
    QSqlRecord dbProductRec;

    QString filter("PrNum=" + plu + " and DeptNum=" + deptNum);
    //LOG_DEBUG << "filter = " << filter.toStdString();
    dbProductsRM.setFilter(filter);

    bool dbRecExists = dbProductsRM.select();
    int rowCount = dbProductsRM.rowCount();
    //LOG_DEBUG << "Prods select, select() returned " << dbRecExists << "  rowCount = " << rowCount;

    if (dbRecExists && rowCount > 0)
    {
        dbProductRec = dbProductsRM.record(0);
        if (dbProductRec.isEmpty())
        {
            LOG_WARN << "dbProductRec read from the db, but is empty for PLU " << plu.toStdString();
            dbRecExists = false;
        }
        //else
            //LOG_DEBUG << "Successfully read Prods db record for PLU " << plu.toStdString();
    }
    else
    {
        dbRecExists = false;
        //LOG_DEBUG << "Product not found in db for PLU " << plu.toStdString();
    }

    // ---- //

    bool dbTextRecExists = false;
    QSqlTableModel dbTextsRM;
    dbTextsRM.setTable("ProductTexts");
    dbTextsRM.setEditStrategy(QSqlTableModel::OnManualSubmit);
    QSqlRecord dbTextRec;

    if (dbRecExists)
    {
        // If the product record is in the database, see if it has a TxNum (Expanded Text Number).
        // If it does, look up the ProductText record for it.

        QSqlField field(dbProductRec.field("TxNum"));
        QVariant fieldVar(field.value());
        if (fieldVar.toInt() != 0)
        {
            const QString prodTxNum = fieldVar.toString();

            QString textFilter("ProdTextType=1 and ProdTextNum=" + prodTxNum + " and DeptNum=" + deptNum);
            //LOG_DEBUG << "text filter = " << textFilter.toStdString();
            dbTextsRM.setFilter(textFilter);

            dbTextRecExists = dbTextsRM.select();
            const int rowCount = dbTextsRM.rowCount();
            //LOG_DEBUG << "ProductTexts select, select() returned " << dbTextRecExists << "  rowCount = " << rowCount;

            if (dbTextRecExists && rowCount > 0)
            {
                dbTextRec = dbTextsRM.record(0);
                if (dbTextRec.isEmpty())
                {
                    LOG_WARN << "dbTextRec read from the db, but is empty for TextNum " << prodTxNum.toStdString();
                    dbTextRecExists = false;
                }
                //else
                    //LOG_DEBUG << "Successfully read ProductTexts db record for TextNum " << prodTxNum.toStdString();
            }
            else
            {
                dbTextRecExists = false;
                //LOG_DEBUG << "ProductTexts not found in db for TextNum " << prodTxNum.toStdString();
            }
        }
    }

    // Get an empty record as the basis for a new ProductTexts record
    if (!dbTextRecExists)
        dbTextRec = dbTextsRM.record();

    // ---- //

    bool dbAllergensRecExists = false;
    QSqlTableModel dbAllergensRM;
    dbAllergensRM.setTable("ProductTexts");
    dbAllergensRM.setEditStrategy(QSqlTableModel::OnManualSubmit);
    QSqlRecord dbAllergensRec;

    if (dbRecExists)
    {
        // If the product record is in the database, see if it has a TxNum (Expanded Text Number).
        // If it does, look up the ProductText record for it.

        QSqlField field(dbProductRec.field("ProdTxt9Num"));
        QVariant fieldVar(field.value());
        if (fieldVar.toInt() != 0)
        {
            const QString prodTxNum = fieldVar.toString();

            QString textFilter("ProdTextType=9 and ProdTextNum=" + prodTxNum + " and DeptNum=" + deptNum);
            //LOG_DEBUG << "text filter = " << textFilter.toStdString();
            dbAllergensRM.setFilter(textFilter);

            dbAllergensRecExists = dbAllergensRM.select();
            const int rowCount = dbAllergensRM.rowCount();
            //LOG_DEBUG << "ProductTexts select, select() returned " << dbTextRecExists << "  rowCount = " << rowCount;

            if (dbAllergensRecExists && rowCount > 0)
            {
                dbAllergensRec = dbAllergensRM.record(0);
                if (dbAllergensRec.isEmpty())
                {
                    LOG_WARN << "dbAllergensRec read from the db, but is empty for TextNum " << prodTxNum.toStdString();
                    dbAllergensRecExists = false;
                }
                //else
                    //LOG_DEBUG << "Successfully read ProductTexts db record for TextNum " << prodTxNum.toStdString();
            }
            else
            {
                dbAllergensRecExists = false;
                //LOG_DEBUG << "ProductTexts not found in db for TextNum " << prodTxNum.toStdString();
            }
        }
    }

    // Get an empty record as the basis for a new ProductTexts record
    if (!dbAllergensRecExists)
        dbAllergensRec = dbAllergensRM.record();

    // ---- //

    bool dbExpText2RecExists = false;
    QSqlTableModel dbExpText2RM;
    dbExpText2RM.setTable("ProductTexts");
    dbExpText2RM.setEditStrategy(QSqlTableModel::OnManualSubmit);
    QSqlRecord dbExpText2Rec;

    if (dbRecExists)
    {
        // If the product record is in the database, see if it has a TxNum (Expanded Text Number).
        // If it does, look up the ProductText record for it.

        QSqlField field(dbProductRec.field("ProdTxt7Num"));
        QVariant fieldVar(field.value());
        if (fieldVar.toInt() != 0)
        {
            const QString prodTxNum = fieldVar.toString();

            QString textFilter("ProdTextType=7 and ProdTextNum=" + prodTxNum + " and DeptNum=" + deptNum);
            //LOG_DEBUG << "text filter = " << textFilter.toStdString();
            dbExpText2RM.setFilter(textFilter);

            dbExpText2RecExists = dbExpText2RM.select();
            const int rowCount = dbExpText2RM.rowCount();
            //LOG_DEBUG << "ProductTexts select, select() returned " << dbTextRecExists << "  rowCount = " << rowCount;

            if (dbExpText2RecExists && rowCount > 0)
            {
                dbExpText2Rec = dbExpText2RM.record(0);
                if (dbExpText2Rec.isEmpty())
                {
                    LOG_WARN << "dbExpText2Rec read from the db, but is empty for TextNum " << prodTxNum.toStdString();
                    dbExpText2RecExists = false;
                }
                //else
                    //LOG_DEBUG << "Successfully read ProductTexts db record for TextNum " << prodTxNum.toStdString();
            }
            else
            {
                dbExpText2RecExists = false;
                //LOG_DEBUG << "ProductTexts not found in db for TextNum " << prodTxNum.toStdString();
            }
        }
    }

    // Get an empty record as the basis for a new ProductTexts record
    if (!dbExpText2RecExists)
        dbExpText2Rec = dbExpText2RM.record();

    // ---- //

    bool dbNutrRecExists = false;
    QSqlTableModel dbNutrFsRM;
    dbNutrFsRM.setTable("NutrFs");
    dbNutrFsRM.setEditStrategy(QSqlTableModel::OnManualSubmit);
    QSqlRecord dbNutrRec;

    if (dbRecExists)
    {
        // If the product record is in the database, see if it has a NFNum (NutriFact Number).
        // If it does, look up the NutrFs record for it.

        QSqlField field(dbProductRec.field("NFNum"));
        QVariant fieldVar(field.value());
        if (fieldVar.toInt() != 0)
        {
            const QString prodNFNum = fieldVar.toString();

            QString nutrFilter("NFNum=" + prodNFNum + " and DeptNum=" + deptNum);
            //LOG_DEBUG << "text filter = " << nutrFilter.toStdString();
            dbNutrFsRM.setFilter(nutrFilter);

            dbNutrRecExists = dbNutrFsRM.select();
            const int rowCount = dbNutrFsRM.rowCount();
            //LOG_DEBUG << "NutrFs select, select() returned " << dbNutrRecExists << "  rowCount = " << rowCount;

            if (dbNutrRecExists && rowCount > 0)
            {
                dbNutrRec = dbNutrFsRM.record(0);
                if (dbNutrRec.isEmpty())
                {
                    LOG_WARN << "dbNutrRec read from the db, but is empty for NFNum " << prodNFNum.toStdString();
                    dbNutrRecExists = false;
                }
                //else
                //    LOG_DEBUG << "Successfully read NutrFs db record for NFNum " << prodNFNum.toStdString();
            }
            else
            {
                dbNutrRecExists = false;
                //LOG_DEBUG << "NutrFs not found in db for NFNum " << prodNFNum.toStdString();
            }
        }
    }

    // Get an empty record as the basis for a new ProductTexts record
    if (!dbNutrRecExists)
        dbNutrRec = dbNutrFsRM.record();
    //LOG_DEBUG << "initial dbNutrRec is " << ((dbNutrRec.isEmpty()) ? "EMPTY" : "NOT EMPTY");


    // ---- //


    bool returnVal = true;

    //  If we have an existing record in the database, update it.
    if (dbRecExists)
        LOG_DEBUG << "We have an existing record in the database, update it";
    else
    {
        // No existing PLU record in the database; Create it using the corresponding Product Default record in the database.

        //LOG_DEBUG << "We DO NOT have an existing record in the database, create it from Product Default";

        QSqlRecord dbProductDefaultsRec;
        QSqlTableModel dbProductDefaultsRM;
        dbProductDefaultsRM.setTable("PrDefs");
        dbProductDefaultsRM.setEditStrategy(QSqlTableModel::OnManualSubmit);

        int itemType = determineItemType( pluJsonObj );
        if ( itemType == 0)
            returnVal = false;
        else
        {
            QString filter("PDType=" + QString::number(itemType) + " and DeptNum=" + deptNum);
            //LOG_DEBUG << "Looking up PrDefs to create new Plu, filter = " << filter.toStdString();
            dbProductDefaultsRM.setFilter(filter);

            bool dbRecExists = dbProductDefaultsRM.select();
            rowCount = dbProductDefaultsRM.rowCount();
            //LOG_DEBUG << "PrDefs select, select() returned " << dbRecExists << "  rowCount = " << rowCount;

            if (dbRecExists && rowCount > 0)
            {
                //LOG_DEBUG << "PrDefs db record found";

                dbProductDefaultsRec = dbProductDefaultsRM.record(0);
                if ( dbProductDefaultsRec.isEmpty() )
                {
                    LOG_WARN << "PrDefs db record is empty, " << filter.toStdString();
                    returnVal = false;
                }
            }
            else
            {
                LOG_WARN << "PrDefs db record not found, " << filter.toStdString();
                returnVal = false;
            }

            if (returnVal)
            {
                // Get an empty record as the basis for the new Prods record
                dbProductRec = dbProductsRM.record();

                //LOG_DEBUG << "empty product record";
                //showProductRec(dbProductRec);

                // Need to create a new PLUID for the new Prods record
                qlonglong nextPluID( determineNextPluID() );
                if ( nextPluID < 0 )
                {
                    LOG_WARN << "Could not determine a new PrID for a new product record, for PLU " << plu.toStdString();
                    returnVal = false;
                }
                else
                {
                    // Need to explicitly set these fields
                    updateField(dbProductRec, "PrID", QString::number(nextPluID), true);
                    updateField(dbProductRec, "PrNum", plu, true);
                    updateField(dbProductRec, "DeptNum", deptNum, true);

                    createProductFromProductDefault(dbProductRec, dbProductDefaultsRec);

                    //LOG_DEBUG << "After setting product fields from product default fields";
                    //showProductRec(dbProductRec);
                }
            }
        }
    }


    if (returnVal)
    {
        updatePluRecord( pluJsonObj, dbProductRec, dbTextRec, dbAllergensRec, dbExpText2Rec, dbNutrRec );


        //LOG_DEBUG << "After updating product fields from JSON data";
        //showProductRec(dbProductRec);

        // Clear indicator that this PLU record will become expired, from when the scale was previously detected to be offline.
        // To do this, we record the current time when the plu is read from the Walmart server.

        updateField(dbProductRec, "PrCdNm1", QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch()));


        //LOG_DEBUG << "before saving - dbTextRec is " << ((dbTextRec.isEmpty()) ? "EMPTY" : "NOT EMPTY");
        //LOG_DEBUG << "before saving - dbNutrRec is " << ((dbNutrRec.isEmpty()) ? "EMPTY" : "NOT EMPTY");

        // Save the ProductTexts record to the database.
        if (dbTextRec.field("ProdTextCheck").value().toInt() == 42)
        {
            // We need to save the Expanded Text Record; it's been changed

            // chobart_d always just sets the 'check' field to 1, since the beginning of HT's existence.
            // I was setting it to 42 to indicate the hook changed it.
            dbTextRec.setValue("ProdTextCheck", QVariant((int)1));

            if ( !dbTextRecExists )
            {
                // The ProductTexts record DID NOT ALREADY exist when this method started, so I need to set some fields to create a new record.

                // Need to explicitly set these fields
                dbTextRec.setValue("DeptNum", QVariant(deptNum));
                dbTextRec.setValue("ProdTextType", QVariant((int)1));

                // Determine the ProdTextNum for the new record.  The algorithm is that we try the plu number as the ProdTextNum value to use
                // to create the new ProductTexts record.  If it's already taken, then just increment by 1 until you find one available,
                // i.e. there is no ProductTexts record with that ProdTextNum.
               long newProdTextNum = determineNextProdTextNum(plu.toLong(), deptNum.toLong());
               dbTextRec.setValue("ProdTextNum", QVariant(QString::number(newProdTextNum)));

               // Set the TxNum into the Product record
               dbProductRec.setValue("TxNum", QVariant(QString::number(newProdTextNum)));
            }

            if ( !saveRecordToDatabase( "ProductTexts", dbTextRecExists, &dbTextsRM, dbTextRec ) )
                returnVal = false;
        }

        // Save the Allergens record to the database.
        if (dbAllergensRec.field("ProdTextCheck").value().toInt() == 42)
        {
            // We need to save the Expanded Text Record; it's been changed

            // chobart_d always just sets the 'check' field to 1, since the beginning of HT's existence.
            // I was setting it to 42 to indicate the hook changed it.
            dbAllergensRec.setValue("ProdTextCheck", QVariant((int)1));

            if ( !dbAllergensRecExists )
            {
                // The ProductTexts record DID NOT ALREADY exist when this method started, so I need to set some fields to create a new record.

                // Need to explicitly set these fields
                dbAllergensRec.setValue("DeptNum", QVariant(deptNum));
                dbAllergensRec.setValue("ProdTextType", QVariant((int)9));

                // Determine the ProdTextNum for the new record.  The algorithm is that we try the plu number as the ProdTextNum value to use
                // to create the new ProductTexts record.  If it's already taken, then just increment by 1 until you find one available,
                // i.e. there is no ProductTexts record with that ProdTextNum.
               long newProdTextNum = determineNextProdTextNum(plu.toLong(), deptNum.toLong());
               dbAllergensRec.setValue("ProdTextNum", QVariant(QString::number(newProdTextNum)));

               // Set the TxNum into the Product record
               dbProductRec.setValue("ProdTxt9Num", QVariant(QString::number(newProdTextNum)));
            }

            if ( !saveRecordToDatabase( "ProductTexts", dbAllergensRecExists, &dbAllergensRM, dbAllergensRec ) )
                returnVal = false;
        }

        // Save the Exp Text 2 record to the database.
        if (dbExpText2Rec.field("ProdTextCheck").value().toInt() == 42)
        {
            // We need to save the Expanded Text Record; it's been changed

            // chobart_d always just sets the 'check' field to 1, since the beginning of HT's existence.
            // I was setting it to 42 to indicate the hook changed it.
            dbExpText2Rec.setValue("ProdTextCheck", QVariant((int)1));

            if ( !dbExpText2RecExists )
            {
                // The ProductTexts record DID NOT ALREADY exist when this method started, so I need to set some fields to create a new record.

                // Need to explicitly set these fields
                dbExpText2Rec.setValue("DeptNum", QVariant(deptNum));
                dbExpText2Rec.setValue("ProdTextType", QVariant((int)7));

                // Determine the ProdTextNum for the new record.  The algorithm is that we try the plu number as the ProdTextNum value to use
                // to create the new ProductTexts record.  If it's already taken, then just increment by 1 until you find one available,
                // i.e. there is no ProductTexts record with that ProdTextNum.
               long newProdTextNum = determineNextProdTextNum(plu.toLong(), deptNum.toLong());
               dbExpText2Rec.setValue("ProdTextNum", QVariant(QString::number(newProdTextNum)));

               // Set the TxNum into the Product record
               dbProductRec.setValue("ProdTxt7Num", QVariant(QString::number(newProdTextNum)));
            }

            if ( !saveRecordToDatabase( "ProductTexts", dbExpText2RecExists, &dbExpText2RM, dbExpText2Rec ) )
                returnVal = false;
        }

        // Save the NutrFs record to the database.
        if (dbNutrRec.field("NFCheck").value().toInt() == 42)
        {
            // We need to save the Nutrifacts Record; it's been changed

            // chobart_d always just sets the 'check' field to 1, since the beginning of HT's existence.
            // I was setting it to 42 to indicate the hook changed it.
            dbNutrRec.setValue("NFCheck", QVariant((int)1));

            if ( !dbNutrRecExists )
            {
                // The NutrFs record DID NOT ALREADY exist when this method started, so I need to set some fields to create a new record.

                // Need to explicitly set these fields
                dbNutrRec.setValue("DeptNum", QVariant(deptNum));

                // Determine the NFNum for the new record.  The algorithm is that we try the plu number as the NFNum value to use
                // to create the new NutrFs record.  If it's already taken, then just increment by 1 until you find one available,
                // i.e. there is no NutrFs record with that NFNum.
               long newNFNum = determineNextNFNum(plu.toLong(), deptNum.toLong());
               dbNutrRec.setValue("NFNum", QVariant(QString::number(newNFNum)));

               // Set the NfNum into the Product record
               dbProductRec.setValue("NFNum", QVariant(QString::number(newNFNum)));
            }

            if ( !saveRecordToDatabase( "NutrFs", dbNutrRecExists, &dbNutrFsRM, dbNutrRec ) )
                returnVal = false;
        }


        // Save the Product record to the database.
        if ( !saveRecordToDatabase( "Prods", dbRecExists, &dbProductsRM, dbProductRec ) )
            returnVal = false;
    }

    LOG_DEBUG << "Saved Prods to db for PLU " << plu.toStdString();
    return returnVal;
}

int customerHook::updateDbWithMultiplePlus( const QString& deptNum, const QJsonArray& jsonArray )
{
    const int numItemsReceived( jsonArray.size() );

    LOG_DEBUG << "items received " << numItemsReceived;

    int numItemsProcessed = 0;

    // iterate each item
    foreach ( const QJsonValue& v, jsonArray )
    {
        QMutexLocker guard( &m_ );

        //LOG_TRACE << "customAction - " << action.toStdString() << " - PLU Data: " << HEX_DUMP( jsonArray[i].toObject(), jsonArray[i].toObject().length() );               // prints out everything so for now I commented out

        if ( updateDbWithPlu( deptNum, v.toObject() ) )
            ++numItemsProcessed;
    }

    LOG_DEBUG << "items processed " << numItemsProcessed;
    return numItemsProcessed;
}

void customerHook::getAllProducts( QString deptNum, bool deleteProducts )
{
    // tell operator we are loading database
    errorDialogManager::instance()->showLoadingDatabase( messageFadeTime );

    if ( deleteProducts )
    {
        QMutexLocker guard( &m_ );

        LOG_DEBUG << "delete products...";
        deleteAllPluData();
    }

    // **************************************************************
    // See if the PLU is on the Walmart Server
    // **************************************************************

    QJsonDocument doc;

    LOG_TRACE << "invoke api...";
    walmartapi::RequestStatus apiResult = api()->getAllItems( country(), storeNum(), deptNum, doc );

    LOG_TRACE << "invoke api complete " << apiResult;

    QDateTime stamp;
    QString endpoint;

    if ( walmartapi::importLastUsage( stamp, endpoint ) )
    {
        const int delta( stamp.msecsTo( QDateTime::currentDateTime() ) );
        const int maxTime( minRelookupTime_.toInt() );

        LOG_TRACE << "api last usage delta " << delta << " with max time " << maxTime;

        // api was able to connect, we are probably online
        if (( 0 < delta ) && ( delta < maxTime ))
        {
            LOG_DEBUG << "updating network checking thread with most recent api stamp";
            net_->setLastOnlineTime( stamp );
        }
    }

    // process result
    if ( apiResult != walmartapi::SUCCESS )
        LOG_WARN << "bad result " << apiResult;
    else
    {
        LOG_DEBUG << "api()->getAllItems returned SUCCESS - we received PLU data from the server";

        const QByteArray a( doc.toBinaryData() );
        LOG_DEBUG << "PLU Data length: " << a.length();               // prints out everything so for now I commented out

        if ( doc.isArray() )
        {
            // Walmart is returning the data as a JSON array of JSON PLU objects, currently an array of a single PLU object
            const QJsonArray jsonArray( doc.array() );
            LOG_DEBUG << "jsonArray size = " << jsonArray.size();

            QString jsonData( doc.toJson() );
            LOG_DEBUG << "\nFULL LOAD Json Data: " << "\n" << jsonData.toStdString() << "\n";

            updateDbWithMultiplePlus( deptNum, jsonArray );
        }
        else
        {
            LOG_WARN << "getAllItems - did not receive a JSON array";
        }
    }
}

bool customerHook::deletePluFromDb(const QString& plu, const QString& deptNum)
{
    bool result = false;
    QSqlTableModel dbProductsRM;
    dbProductsRM.setTable("Prods");
    dbProductsRM.setEditStrategy(QSqlTableModel::OnManualSubmit);

    QString filter("PrNum=" + plu + " and DeptNum=" + deptNum);
    //LOG_DEBUG << "filter = " << filter.toStdString();
    dbProductsRM.setFilter(filter);

    if (dbProductsRM.select() && dbProductsRM.rowCount() > 0)
    {
        //LOG_DEBUG << "PLU to delete exists in the database for PLU " << plu.toStdString();
        QSqlRecord dbProductRec = dbProductsRM.record(0);

        // Also delete all records associated with this plu (support records that aren't linked to other plu's, ...)
        deleteSupportRecords(dbProductRec);

        if ( !dbProductsRM.removeRows(0, 1) )
            LOG_WARN << "Could not remove the expired PLU from the database, for PLU " << plu.toStdString();
        else if ( !saveToDatabase( "Prods", &dbProductsRM ) )
            LOG_WARN << "Could not delete the expired PLU record from the database, for PLU " << plu.toStdString();
        else
        {
            LOG_DEBUG << "Removed PLU from the database for PLU " << plu.toStdString();
            result = true;
        }
    }
    else
    {
        LOG_WARN << "Product was to be deleted from the db, but wasn't found in db for PLU " << plu.toStdString();
        result = true;
    }

    return result;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
bool customerHook::saveRecordToDatabase( const QString recordName, bool dbRecExists, QSqlTableModel *dbRM, QSqlRecord &dbRec )
{
    if (dbRecExists)
    {
        // The record already existed when we started, so just update it.

        //LOG_DEBUG << "Before setting product record to RM";
        //showProductRec(dbProductRec);
        //rowCount = dbProductsRM->rowCount();
        //LOG_DEBUG << "before setRecord into dbProductsRM   rowCount = " << rowCount;

        if ( !dbRM->setRecord(0, dbRec) )
        {
            LOG_WARN << "Could not set the db record for " << recordName.toStdString();
            return false;
        }
    }
    else
    {
        // The record DID NOT ALREADY exist when we started, so I need to create a new one.

        if ( !dbRM->insertRecord(0, dbRec) )
        {
            LOG_WARN << "Could not insert the db record for " << recordName.toStdString();
            return false;
        }
     }

     if ( !saveToDatabase( recordName, dbRM ) )
         return false;

     return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool customerHook::saveToDatabase( const QString recordName, QSqlTableModel *dbRM )
{
    bool submitSuccess = false;
    int retries = 10;
    while (retries > 0)
    {
        if ( dbRM->submitAll() )
        {
            if (retries < 10)
                LOG_DEBUG << "Submitted " << recordName.toStdString() << " to the database";
            submitSuccess = true;
            retries = 0;
        }
        else
        {
            QSqlError err( ((QSqlQueryModel *) (dbRM))->lastError() );
            if ( canErrorBeRetried(err.nativeErrorCode() ) )
            {
                LOG_WARN << "Error submitting " << recordName.toStdString() << " to the database, will retry  retries=" << retries;

                if ( !dbname_.isEmpty() )
                {
                    //LOG_DEBUG << "During submit of " << recordName.toStdString() << " - before closing database";
                    closeDatabase();
                    //LOG_DEBUG << "During submit of " << recordName.toStdString() << " - before reopening database";
                    openDatabase();
                    //LOG_DEBUG << "During submit of " << recordName.toStdString() << " - before resubmitting to database";
                }

                retries--;
            }
            else
            {
                LOG_WARN << "Error submitting " << recordName.toStdString() << " to the database: "
                         << "   error type: " << err.type()
                         << "   native error code: " << err.nativeErrorCode().toStdString()
                         << "   error text: " << err.text().toStdString()
                         << "   database text: " << err.databaseText().toStdString()
                         << "   driver text: " << err.driverText().toStdString();
                 retries = 0;
            }
        }
    }
    return submitSuccess;
}

bool customerHook::canErrorBeRetried( QString err )
{
    err.append(':');
    bool result = ( retriableErrors_.indexOf(err) >= 0 ) ? true : false;
    LOG_DEBUG << "canErrorBeRetried - err=" << err.toStdString() << "  retriableErors=" << retriableErrors_.toStdString() << "  result=" << result;
    return result;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
bool customerHook::isCacheTimeExpired(qint64 pluRecExpiryTime)
{
    // cacheExpiration of -1 (or any negative) means ignore the expiration time; there is no expiration of plu's in the scale database
    // Thus, only check for expiring plu's if not -1
    // NOTE: 0 results in plu's always expiring since time between lookups is greater than 0
    int cacheTime( cacheExpiration().toInt() );             // var to use as cache time
    if ( cacheTime < 0 )
    {
        LOG_DEBUG << "cache never expires, so use PLU if you can ";
        return false;
    }

    bool isExpired = false;
    qint64 cacheMS = cacheTime * 1000;   // to get to milliseconds
    if ( cacheExpirationUnits() == "h" )
        cacheMS *= 3600;
    else if ( cacheExpirationUnits() == "m" )
        cacheMS *= 60;

    QDateTime current = QDateTime::currentDateTime();
    qint64 currentMSecsSinceEpoch = current.toMSecsSinceEpoch();

    // pluRecTime could be 0 if either the value is 0, or it couldn't be converted to a number; thus, it hasn't been set, and so consider it expired
    if ( pluRecExpiryTime == 0 || ((currentMSecsSinceEpoch - pluRecExpiryTime) > cacheMS) )
        isExpired = true;

    LOG_DEBUG << "current time = " << currentMSecsSinceEpoch << "  dbRecTime = " << pluRecExpiryTime << "  difference = " << currentMSecsSinceEpoch - pluRecExpiryTime << "  time to become expired = " << cacheMS;

    return isExpired;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
int customerHook::determineItemType( const QJsonObject &pluJsonObj )
{
    // From "answers 062820.docx",
    // FixedWeightIdicator and packageCode together will be used to identify if an item is by Fixed Weight, Random Weight or By Count.
    // Fixed Weight – fixedWeightInd - TRUE  , PackageCode = 01
    // Random Weight – fixedWeightInd - FALSE  , PackageCode = 00
    // By Count - fixedWeightInd -  FALSE , PackageCode = 01
    // From chobart_d Product.h, ProductType enum, RW=1, FW=2, BC=3
    // Returns 0 if can't be determined

    int packageCode = 0;
    bool fixedWeightInd = false;
    QJsonObject::const_iterator iter = pluJsonObj.find("fixedWeightInd");
    if (iter != pluJsonObj.end())
        fixedWeightInd = pluJsonObj["fixedWeightInd"].toBool();   // Walmart indicated this would be boolean in the JSON data
    else
    {
        LOG_WARN << "Did not receive fixedWeightInd in JSON data from server, so couldn't determine Item Type";
        return 0;
    }
    iter = pluJsonObj.find("packageCode");
    if (iter != pluJsonObj.end())
        packageCode = pluJsonObj["packageCode"].toString().toInt();   // Walmart indicated this would be numeric in the JSON data
    else
    {
        LOG_WARN << "Did not receive packageCode in JSON data from server, so couldn't determine Item Type";
        return 0;
    }

    int itemType = 0;
    if ( fixedWeightInd && packageCode == 1 )
        itemType = 2;   // FW
    else if ( !fixedWeightInd )
    {
        if (packageCode == 1)
            itemType = 3;   // BC
        else if (packageCode == 0)
            itemType = 1;   // RW
    }

    if (itemType == 0)
        LOG_WARN << "Could not determine item type based on fixedWeightInd = " << fixedWeightInd << " and packageCode = " << packageCode;
    //else
        //LOG_DEBUG << "Determine item type based on fixedWeightInd = " << fixedWeightInd << " and packageCode = " << packageCode << "  resulting itemType = " << itemType <<
        //             ((itemType == 1) ? " RW" : ((itemType == 2) ? " FW" : " BC"));

    return itemType;
}


// Returns true if the product record was updated, false if not
void customerHook::updatePluRecord( const QJsonObject &pluJsonObj, QSqlRecord &dbProductRec, QSqlRecord &dbTextRec, QSqlRecord &dbAllergensRec, QSqlRecord &dbExpText2Rec, QSqlRecord &dbNutrRec )
{
    //LOG_DEBUG << "begin";
    // See Revision 25760 and before, for examples of json document accessing and field access.

    QStringList keyList = pluJsonObj.keys();

    bool isPluField = false;
    bool isNutriField = false;
    bool clearedNutrData = false;
    bool isExpandedTextField = false;
    bool isProdText7Field = false;
    bool isProdText9Field = false;
    bool useLatin1 = false;

    QString hobartFieldName;
    QString newValue;

    // Separately determine if the item type needs to be updated, since it's based on two Walmart fields
    int itemType = determineItemType( pluJsonObj );
    if (itemType > 0 )
        updateField(dbProductRec, "PrType", QString::number(itemType));

    foreach( const QString &key, keyList )
    {
        QJsonValue keyData = pluJsonObj[key];

        isPluField = false;
        isNutriField = false;
        isExpandedTextField = false;
        isProdText7Field = false;
        isProdText9Field = false;
        useLatin1 = false;

        hobartFieldName.clear();
        newValue.clear();

        pluFieldsMap::iterator iter = pluFields_.find(key);
        if (iter != pluFields_.end())
        {
            hobartFieldName = iter.value();
            if ( !hobartFieldName.isEmpty() )
                isPluField = true;
        }
        else
        {
            iter = nutriFields_.find(key);
            if (iter != nutriFields_.end())
            {
                hobartFieldName = iter.value();
                if ( !hobartFieldName.isEmpty() )
                    isNutriField = true;
            }
            else
            {
                iter = expandedTextFields_.find(key);
                if (iter != expandedTextFields_.end())
                {
                    hobartFieldName = iter.value();
                    if ( "TxNum" == hobartFieldName )
                        isExpandedTextField = true;
                    else if ( "ProdTxt7Num" == hobartFieldName )
                        isProdText7Field = true;
                    else if ( "ProdTxt9Num" == hobartFieldName )
                        isProdText9Field = true;
                }
            }
        }

        switch ( keyData.type() )
        {
            case  QJsonValue::Null:
                newValue = QString();
                //LOG_DEBUG << "key " << key.toStdString() << "  value is Null";
                break;
            case  QJsonValue::String:
                newValue = keyData.toString();

                // need to convert from utf-8 encoding to latin1
                if (( isExpandedTextField ) ||
                    ( isProdText7Field ) ||
                    ( isProdText9Field ) ||
                    (( isPluField ) && ( "PrDesc" == hobartFieldName )))
                {
                    useLatin1 = true;
                }

                //LOG_DEBUG << "key " << key.toStdString() << "  string value is " << newValue.toStdString();
                break;
            case  QJsonValue::Double:
            {
                double temp = keyData.toDouble();
                newValue = QString::number(temp);
                if (newValue.indexOf('.') >= 0)
                {
                    if ( isPriceField(hobartFieldName) )
                    {
                        newValue = QString::number(temp * 100);  // TODO - get the multiplier from Configuration
                    }
                    else if ( isWeightField(hobartFieldName) )
                    {
                        newValue = QString::number(temp * 1000);  // TODO - get the multiplier from Configuration
                    }
                }
                //LOG_DEBUG << "key " << key.toStdString() << "  double value is " << newValue.toStdString();
                break;
            }
            case  QJsonValue::Bool:
                newValue = QString::number(keyData.toBool());
                //LOG_DEBUG << "key " << key.toStdString() << "  bool value is " << newValue.toStdString();
                break;
            default:
                newValue = keyData.toString();
                //LOG_DEBUG << "key " << key.toStdString() << "  default value is " << newValue.toStdString();
                break;
        }

        if (isPluField)
        {
            updateField( dbProductRec, hobartFieldName, newValue, false, useLatin1 );
        }
        else if (isNutriField)
        {
            if (!clearedNutrData)
            {
                QString prevValue(dbNutrRec.field("NFCol1").value().toString());
                //LOG_DEBUG << "NutrFs.NFCol1 being cleared - prev value = " << prevValue.toStdString();

                QString prevValue2(dbNutrRec.field("NFCol2").value().toString());
                //LOG_DEBUG << "NutrFs.NFCol2 being cleared - prev value = " << prevValue2.toStdString();

                dbNutrRec.setValue("NFCol1", QVariant(""));
                dbNutrRec.setValue("NFCol2", QVariant(""));

                dbNutrRec.setValue("NFCheck", QVariant((int)42));   // Set this to indicate the nutrifacts record has changed

                clearedNutrData = true;
            }
            updateNutrField(dbNutrRec, hobartFieldName, newValue);
        }
        else if (isExpandedTextField)
        {
            if ( updateField( dbTextRec, "ProdTextDesc", newValue, false, useLatin1 ) )
                dbTextRec.setValue("ProdTextCheck", QVariant((int)42));   // If expanded text was updated, set this to indicate the expanded text record has changed
        }
        else if (isProdText7Field)
        {
            if ( updateField( dbExpText2Rec, "ProdTextDesc", newValue, false, useLatin1 ) )
                dbExpText2Rec.setValue("ProdTextCheck", QVariant((int)42));   // If expanded text was updated, set this to indicate the expanded text record has changed
        }
        else if (isProdText9Field)
        {
            if ( updateField( dbAllergensRec, "ProdTextDesc", newValue, false, useLatin1 ) )
                dbAllergensRec.setValue("ProdTextCheck", QVariant((int)42));   // If expanded text was updated, set this to indicate the expanded text record has changed
        }
    }

    // Handle Desc2 field from Walmart, appending a \n and then DESC2 onto PRDesc
    // This has to come after processing the scaleItemDesc1 (Hobart PrDesc) field, in the loop above.
    if ( pluJsonObj.contains( "scaleItemDesc2" ) )
    {
        const QString desc2( pluJsonObj["scaleItemDesc2"].toString() );

        if ( desc2.length() )
        {
            // Code snippet basically from updateField() method
            const QSqlField field( dbProductRec.field( "PrDesc" ) );
            QVariant fieldVar( field.value() );

            // need to convert from utf-8 encoding to latin1
            const QString newValue = QString::fromLatin1( fieldVar.toByteArray() ) + '\n' + desc2;
            fieldVar = newValue.toLatin1();

            dbProductRec.setValue( "PrDesc", fieldVar );
        }
        //LOG_DEBUG << "Resulting PrDesc appended with scaleItemDesc2: " << newValue.toStdString();
    }

    //LOG_DEBUG << "end";
}


///////////////////////////////////////////////////////////////////////////////////////////////////
bool customerHook::updateField( QSqlRecord &dbRec, const QString& dbFieldName, const QString& newValue, bool forceSet, bool useLatin1 )
{
    if ( dbRec.indexOf( dbFieldName ) < 0 )
    {
        LOG_WARN << dbFieldName.toStdString() << " NOT FOUND in db record";
        return false;
    }

    const QSqlField field( dbRec.field( dbFieldName ) );
    QVariant fieldVar( field.value() );

    const QString prevValue( fieldVar.toString() );
    bool setIt( true );

    // check if different
    if ( prevValue.isNull() )
        setIt = !newValue.isNull();
    else
        setIt = (prevValue != newValue);

    // nothing to do
    if (( !forceSet ) && ( !setIt ))
        return false;

    // do latin1 conversion
    if ( useLatin1 )
        fieldVar = newValue.toLatin1();
    else
        fieldVar = newValue;

    // set!
    dbRec.setValue( dbFieldName, fieldVar );

    //LOG_DEBUG << "setting field " << dbFieldName.toStdString() << " - prev value = " << (prevValue.isNull() ? "NULL" : prevValue.toStdString()) << "  new value = " << (newValue.isNull() ? "NULL" : newValue.toStdString());
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void customerHook::updateNutrField( QSqlRecord &dbNutrRec, QString dbFieldName, QString newValue )
{
    //LOG_DEBUG << "updateNutrField - tag = " << dbFieldName.toStdString() << "  value = " << newValue.toStdString();
    QChar Sub('\x1A');

    if (dbNutrRec.indexOf("NFCol1") < 0)
        LOG_WARN << "NutrFs.NFCol1 NOT FOUND in db record";
    else
    {
        QString colName( "NFCol1" );
        if( isNutrifactColField( dbFieldName ) )
            colName = "NFCol2";

        QString prevValue(dbNutrRec.field(colName).value().toString());
        if ( !prevValue.isEmpty() )
            prevValue += Sub;

        prevValue += dbFieldName + newValue;
        dbNutrRec.setValue(colName, QVariant(prevValue));
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////
bool customerHook::isPriceField(const QString key)
{
    return (key == "PrPrice" || key == "PrExcPr" || key == "PrTotalPriceMin" || key == "PrTotalPriceMax") ? true : false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool customerHook::isWeightField(const QString key)
{
    return (key == "PrTare" || key == "PrTareB" || key == "PrWeightMin" || key == "PrWeightMax") ? true : false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool customerHook::isServerPriceField(const QString &key)
{
    return ( key == "sellPrice" ) ? true : false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool customerHook::isServerWeightField(const QString &key)
{
    return ( key == "tareWeight" || key == "tareWeightB" ) ? true : false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool customerHook::isNutrifactColField(const QString &key)
{
    Q_UNUSED( key )

    return false;

    // not sure why these are going to different column??
    //return ( key == "DiFP" || key == "CaFP" ) ? true : false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void customerHook::deleteSupportRecords( QSqlRecord &dbProductRec )
{
    // Reference Product::deleteRecord()

    // Only the following Product support records are deleted when the product is deleted:
    // ExpandedText,
    // SpecialMessage,
    // ProductNote,
    // MarqueeText,
    // ProductText5,  // CookingInstructionsDev
    // ProductText6,  // RecipeDev
    // ProductText7,  // ExpandedText2Dev
    // ProductText8,  // SuggestiveSellingDev
    // ProductText9,  // AllergensDev

    // The following Product support records are NOT deleted when the product is deleted:
    //      Classes, Categories, Specials, anything COOL related, Label Types,
    //      Totals (they would still want to retrieve them since labels were actually printed)

    QString deptNum(dbProductRec.field("DeptNum").value().toString());

    // Note: The second parameter, the productTextType, came from Product.h - ProdSupportTextType enum
    deleteSupportRecord( dbProductRec, "1", "TxNum", deptNum );
    deleteSupportRecord( dbProductRec, "2", "SMNum", deptNum );
    deleteSupportRecord( dbProductRec, "3", "NtNum", deptNum );
    deleteSupportRecord( dbProductRec, "4", "MaNum", deptNum );
    deleteSupportRecord( dbProductRec, "5", "ProdTxt5Num", deptNum );
    deleteSupportRecord( dbProductRec, "6", "ProdTxt6Num", deptNum );
    deleteSupportRecord( dbProductRec, "7", "ProdTxt7Num", deptNum );
    deleteSupportRecord( dbProductRec, "8", "ProdTxt8Num", deptNum );
    deleteSupportRecord( dbProductRec, "9", "ProdTxt9Num", deptNum );
    deleteSupportRecord( dbProductRec, "20", "NFNum", deptNum );
}

void customerHook::deleteSupportRecord( QSqlRecord &dbProductRec, const QString productTextType, const QString productFieldName, const QString deptNum )
{
    QVariant fieldVar(dbProductRec.field(productFieldName).value());
    if (fieldVar.toInt() != 0)
    {
        QString supportNum = fieldVar.toString();

        QSqlTableModel dbRM;
        dbRM.setTable("Prods");
        dbRM.setEditStrategy(QSqlTableModel::OnManualSubmit);

        QString filter(productFieldName + "=" + supportNum + " and DeptNum=" + deptNum);
        //LOG_DEBUG << "deleteSupportRecord Prods filter = " << filter.toStdString();
        dbRM.setFilter(filter);

        // See how many PLU's have this Product Support record number, for the specified Product Support type/field name.
        // If there's more than one, we can't delete it or the second PLU would no longer have it's Product Support record.
        // If there's only one, then we can delete it, since we are deleting the PLU that is linked to it.

        bool dbRecExists = dbRM.select();
        int rowCount = dbRM.rowCount();
        //LOG_DEBUG << "deleteSupportRecord Prods select, select() returned " << dbRecExists << "  rowCount = " << rowCount;

        if ( dbRecExists && rowCount == 1 )
        {
            if (productTextType == "20")
                deleteNutrRecord( supportNum, deptNum );
            else
                deleteProductTextRecord( productTextType, supportNum, deptNum );
        }
    }
}

void customerHook::deleteProductTextRecord( const QString productTextType, const QString supportNumber, const QString deptNum )
{
    // Delete the ProductTexts record, since the plu that is being deleted is the only one using it

    bool dbRecExists = false;
    QSqlTableModel dbRM;
    dbRM.setTable("ProductTexts");
    dbRM.setEditStrategy(QSqlTableModel::OnManualSubmit);

    QString filter("ProdTextType=" + productTextType + " and ProdTextNum=" + supportNumber + " and DeptNum=" + deptNum);
    //LOG_DEBUG << "deleteProductTextRecord filter = " << filter.toStdString();
    dbRM.setFilter(filter);

    dbRecExists = dbRM.select();
    int rowCount = dbRM.rowCount();
    //LOG_DEBUG << "ProductTexts select, select() returned " << dbRecExists << "  rowCount = " << rowCount;

    if (dbRecExists && rowCount > 0)
    {
        if ( !dbRM.removeRows(0, 1) )
            LOG_WARN << "Couldn't removeRow of ProductTexts record from the database for textType = " << productTextType.toStdString() << "  textNum = " << supportNumber.toStdString();
        else
        {
            if ( !saveToDatabase( "ProductTexts",  &dbRM ) )
                LOG_WARN << "Couldn't delete ProductTexts record from the database for textType = " << productTextType.toStdString() << "  textNum = " << supportNumber.toStdString();
        }
    }
    else
    {
        ; //LOG_DEBUG << "ProductTexts not found in db for for textType = " << productTextType.toStdString() << "  textNum = " << supportNumber.toStdString();
    }
}


void customerHook::deleteNutrRecord( const QString supportNumber, const QString deptNum )
{
    // Delete the NutrFs record, since the plu that is being deleted is the only one using it

    bool dbRecExists = false;
    QSqlTableModel dbRM;
    dbRM.setTable("NutrFs");
    dbRM.setEditStrategy(QSqlTableModel::OnManualSubmit);

    QString filter("NFNum=" + supportNumber + " and DeptNum=" + deptNum);
    //LOG_DEBUG << "deleteNutrRecord filter = " << filter.toStdString();
    dbRM.setFilter(filter);

    dbRecExists = dbRM.select();
    int rowCount = dbRM.rowCount();
    //LOG_DEBUG << "NutrFs select, select() returned " << dbRecExists << "  rowCount = " << rowCount;

    if (dbRecExists && rowCount > 0)
    {
        if ( !dbRM.removeRows(0, 1) )
            LOG_WARN << "Couldn't removeRow of NutrFs record from the database for NFNum = " << supportNumber.toStdString();
        else
        {
            if ( !saveToDatabase( "NutrFs",  &dbRM ) )
                LOG_WARN << "Couldn't delete NutrFs record from the database for NFNum = " << supportNumber.toStdString();
        }
    }
    else
    {
        ; //LOG_DEBUG << "NutrFs not found in db for for textNum = " << supportNumber.toStdString();
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////
qlonglong customerHook::determineNextPluID()
{
    // Logic comes from chobart_d:
    // SqliteRecordManager::create()
    // which calls sqliterecordmanager::performAutoIncrements()   ** most of the logic is here **
    // which calls SqliteDatabase::getAutoIncrementValue(const string& field)
    // which calls SqliteAutoIncrementsRM::operator()(const string& key)

    qlonglong nextPluId = -1;

    QSqlRecord dbAutoIncRec;
    QSqlTableModel dbAutoIncRM;
    dbAutoIncRM.setTable("AutoInc");
    dbAutoIncRM.setEditStrategy(QSqlTableModel::OnManualSubmit);

    bool dbRecExists = dbAutoIncRM.select();
    int rowCount = dbAutoIncRM.rowCount();
    //LOG_DEBUG << "AutoInc select, select() returned " << dbRecExists << "  rowCount = " << rowCount;

    if (dbRecExists && rowCount > 0)
    {
        dbAutoIncRec = dbAutoIncRM.record(0);
        if (dbAutoIncRec.isEmpty())
        {
            LOG_WARN << "AutoInc record read from db, but is empty";
            dbRecExists = false;
        }
    }
    else
    {
        LOG_WARN << "Could not read AutoInc db record";
        dbRecExists = false;
    }

    if ( dbRecExists )
    {
        QSqlField field(dbAutoIncRec.field("AIPrID"));
        QVariant fieldVar(field.value());
        nextPluId = fieldVar.toLongLong() + 1;

        QSqlTableModel dbProductsRM;
        dbProductsRM.setTable("Prods");
        dbProductsRM.setEditStrategy(QSqlTableModel::OnManualSubmit);
        QString filter;
        dbRecExists = false;
        while (true)
        {
            filter = "PrID=" + QString::number(nextPluId);
            //LOG_DEBUG << "filter = " << filter.toStdString();
            dbProductsRM.setFilter(filter);

            dbRecExists = dbProductsRM.select();
            rowCount = dbProductsRM.rowCount();
            //LOG_DEBUG << "Prods select, select() returned " << dbRecExists << "  rowCount = " << rowCount;

            if (dbRecExists && rowCount > 0)
            {
                //LOG_DEBUG << "Attempt to create product with PluId " << nextPluId << " but it already exists, trying next one";
                nextPluId++;
            }
            else
                break;
        }

        fieldVar = QVariant(nextPluId);
        field.setValue(fieldVar);
        dbAutoIncRec.setValue("AIPrID", fieldVar);
        //LOG_DEBUG << "AutoInc AIPrID updated to " << nextPluId;

        dbAutoIncRM.setRecord(0, dbAutoIncRec);
        if ( !saveToDatabase( "AutoInc", &dbAutoIncRM ) )
            LOG_WARN << "Could not update AutoInc db record for AIPrID = " << nextPluId;
    }

    return nextPluId;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
long customerHook::determineNextProdTextNum(long startingNum, long deptNum)
{
    // Determine the ProdTextNum for the new record.  The algorithm is that we try the plu number as the ProdTextNum value to use
    // to create the new ProductTexts record.  If it's already taken, then just increment by 1 until you find one available,
    // i.e. there is no ProductTexts record with that ProdTextNum.

    //LOG_DEBUG << "determineNextProdTextNum - startingNum = " << startingNum << "  deptNum = " << deptNum;

    bool dbRecExists = false;
    QSqlTableModel dbRM;
    dbRM.setTable("ProductTexts");
    dbRM.setEditStrategy(QSqlTableModel::OnManualSubmit);

    while (true)
    {
        QString filter("ProdTextType=1 and ProdTextNum=" + QString::number(startingNum) + " and DeptNum=" + QString::number(deptNum));
        //LOG_DEBUG << "filter = " << filter.toStdString();
        dbRM.setFilter(filter);

        dbRecExists = dbRM.select();
        int rowCount = dbRM.rowCount();
        //LOG_DEBUG << "ProductTexts select, select() returned " << dbRecExists << "  rowCount = " << rowCount;

        if (dbRecExists && rowCount > 0)
        {
            //LOG_DEBUG << "Attempt to create ProductTexts with ProdTextNum " << startingNum << " but it already exists, trying next one";
            startingNum++;
        }
        else
            break;
    }

    return startingNum;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
long customerHook::determineNextNFNum(long startingNum, long deptNum)
{
    // Determine the ProdTextNum for the new record.  The algorithm is that we try the plu number as the ProdTextNum value to use
    // to create the new ProductTexts record.  If it's already taken, then just increment by 1 until you find one available,
    // i.e. there is no ProductTexts record with that ProdTextNum.

    //LOG_DEBUG << "determineNextNFNum - startingNum = " << startingNum << "  deptNum = " << deptNum;

    bool dbRecExists = false;
    QSqlTableModel dbRM;
    dbRM.setTable("NutrFs");
    dbRM.setEditStrategy(QSqlTableModel::OnManualSubmit);

    while (true)
    {
        QString filter("NFNum=" + QString::number(startingNum) + " and DeptNum=" + QString::number(deptNum));
        //LOG_DEBUG << "filter = " << filter.toStdString();
        dbRM.setFilter(filter);

        dbRecExists = dbRM.select();
        int rowCount = dbRM.rowCount();
        //LOG_DEBUG << "NutrFs select, select() returned " << dbRecExists << "  rowCount = " << rowCount;

        if (dbRecExists && rowCount > 0)
        {
            //LOG_DEBUG << "Attempt to create NutrFs with NFNum " << startingNum << " but it already exists, trying next one";
            startingNum++;
        }
        else
            break;
    }

    return startingNum;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
void customerHook::createProductFromProductDefault( QSqlRecord &dbProductRec, QSqlRecord &dbProductDefaultsRec )
{
    //LOG_DEBUG << "Creating Prods record from PrDefs";

    QString prodsFieldName;
    QString prDefsFieldName;


    for (int i = 0; i < dbProductDefaultsRec.count(); i++)
    {
        prodsFieldName = "";

        QSqlField field(dbProductDefaultsRec.field(i));
        QString prDefsFieldName( dbProductDefaultsRec.fieldName(i) );

        prDefsFieldsMap::iterator iter = prDefsFields_.find(prDefsFieldName);
        if (iter != prDefsFields_.end())
            prodsFieldName = iter.value();
        //else
            //LOG_DEBUG << "PRDefs field " << prDefsFieldName.toStdString() << " does not have a corresponding Prods table field";

        if ( !prodsFieldName.isEmpty() )
        {
            QVariant fieldVar(field.value());
            QString prDefsValue(fieldVar.toString());
            //LOG_DEBUG << "Updating prods field " << prodsFieldName.toStdString() << " from PRDefs field " << prDefsFieldName.toStdString() << ", value = " << prDefsValue.toStdString();
            updateField(dbProductRec, prodsFieldName, prDefsValue);
        }
    }

}


///////////////////////////////////////////////////////////////////////////////////////////////////
void customerHook::showProductRec( QSqlRecord &dbProductRec )
{
    LOG_DEBUG << "showProductRec  begin";
    LOG_DEBUG << "productRec is empty = " << ((dbProductRec.isEmpty()) ? "true" : "false");
    for (int i = 0; i < dbProductRec.count(); i++)
    {
        QSqlField field(dbProductRec.field(i));
        QString prodsFieldName = dbProductRec.fieldName(i);

        QVariant fieldVar(field.value());
        QString prDefsValue(fieldVar.toString());
        LOG_DEBUG << " field = " << prodsFieldName.toStdString() << "  type = " << fieldVar.typeName() << "  value = " << prDefsValue.toStdString();
    }
    LOG_DEBUG << "showProductRec  end";
}



///////////////////////////////////////////////////////////////////////////////////////////////////
bool customerHook::beforePluTextRead( const QString& number, const QString& deptNum, int type )
{
    QMutexLocker guard( &m_ );

    Q_UNUSED( number );
    Q_UNUSED( deptNum );
    Q_UNUSED( type );

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool customerHook::beforePrint( const QString& plu, const QString& deptNum, bool isRemotePrint )
{
    QMutexLocker guard( &m_ );

    LOG_DEBUG << "beforePrint " << plu.toStdString() << " " << deptNum.toStdString();

    //
    // offline verify dialogs
    //

    const int offlineTimerMax( offlinePrintDialog1.first );

    const QDateTime lastSuccess( net_->lastOnlineTime() );
    const QDateTime now( QDateTime::currentDateTime() );

    if ( net_->isOnline() )
        LOG_TRACE << "online";
    else if (( 0 < offlineTimerMax ) && ( lastSuccess.isValid() ) && ( lastSuccess <= now ))
    {
        // compute offline countdown timer
        const int offlineAmount( lastSuccess.msecsTo( now ) );

        if ( 0 < offlineAmount )
        {
            const int countdownTimer( offlineTimerMax - offlineAmount );
            const int countdownTimerHours( std::ceil( (double)(countdownTimer) / (double)(60 * 60 * 1000) ) );

            LOG_DEBUG << "countdown timer at " << countdownTimer << " (" << countdownTimerHours << " hr)";

            bool forceCheckOnline( false );

            // AF-5.c
            // countdown at 0 hours
            if ( countdownTimer <= 0 )
            {
                if ( lockout_ )
                    LOG_DEBUG << "lockout";
                else
                {
                    LOG_WARN << "!!!! LOCKOUT !!!!";
                    LOG_WARN << "CONTACT FIELD SERVICES TO RAISE AN INCIDENT";
                    LOG_WARN << "scale offline since " << lastSuccess.toString().toStdString();
                    LOG_WARN << "scale offline for " << offlineAmount;
                    LOG_WARN << "initial countdown timer was " << offlineTimerMax;
                    LOG_WARN << "countdown timer at " << countdownTimer << " (" << countdownTimerHours << " hr)";
                    LOG_WARN << "REBOOT IN ORDER TO USE SCALE AGAIN";
                    LOG_WARN << "!!!! LOCKOUT !!!!";

                    errorDialogManager::instance()->showNotAvailable( lastSuccess );
                    lockout_ = true;
                }

                LOG_DEBUG << "cancel printing locked out";
                return false;
            }

            // AF-5.a
            // coundown in first range (between 144 and 120 hours)
            else if (( offlinePrintDialog1.second < offlinePrintDialog1.first ) && ( 0 < offlinePrintDialog1.first ) &&
                     ( offlinePrintDialog1.second < countdownTimer ) && ( countdownTimer <= offlinePrintDialog1.first ))
            {
                LOG_INFO << "showing comm error dialog (144-120)";
                forceCheckOnline = errorDialogManager::instance()->showCommError( lastSuccess, countdownTimerHours );

                // operator chose cancel
                if ( !forceCheckOnline )
                {
                    LOG_DEBUG << "cancel printing (144-120)";
                    return false;
                }
            }

            // AF-5.b
            // coundown in second range (between 24 and 0 hours)
            else if (( offlinePrintDialog2.second < offlinePrintDialog2.first ) && ( 0 < offlinePrintDialog2.first ) &&
                     ( offlinePrintDialog2.second < countdownTimer ) && ( countdownTimer <= offlinePrintDialog2.first ))
            {
                LOG_INFO << "showing comm error dialog (24-0)";
                forceCheckOnline = errorDialogManager::instance()->showCommError( lastSuccess, countdownTimerHours );

                // operator chose cancel
                if ( !forceCheckOnline )
                {
                    LOG_DEBUG << "cancel printing (24-0)";
                    return false;
                }
            }

            // Basic Flow 12
            if (( forceCheckOnline ) && ( !net_->forceCheckOnlineStatus() ))
            {
                if ( messageFadeTime <= 0 )
                    LOG_WARN << "bad message fade time " << messageFadeTime;
                else
                {
                    LOG_INFO << "showing error still detected";
                    errorDialogManager::instance()->showErrorStillDetected( messageFadeTime );
                }
            }
        }

        LOG_TRACE << "printing...";
    }

    // keep track of items printed
    pluPrint info;
    info.stamp = QDateTime::currentDateTime();
    info.plu = plu;
    info.deptNum = deptNum;
    info.isRemotePrint = isRemotePrint;
    info.counter = 0;

    printInfo_.append( info );

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void customerHook::afterPrint( bool success, int numLabelsToPrint, int numLabelsPrinted )
{
    Q_UNUSED( numLabelsToPrint )

    QMutexLocker guard( &m_ );

    // sanity check
    if ( printInfo_.isEmpty() )
        return;

    if ( !success )
        LOG_DEBUG << "afterPrint FAILURE";
    else
    {
        // update print information
        pluPrint info( printInfo_.last() );
        info.counter += numLabelsPrinted;

        printInfo_.last() = info;

        LOG_DEBUG << "afterPrint " << info.plu.toStdString() << " " << info.deptNum.toStdString() << " " << info.isRemotePrint << " " << info.counter;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void customerHook::deleteAllPluData()
{
    // *******************************************************************************************************
    // The following are the tables that are considered "PLU" data by chobart_d (from BackupRestoreManager)
    // Delete all records from these tables before processing data from the Walmart server.
    // *******************************************************************************************************

    deleteFromTable("PrStats");       //    productManager->handleDeleteAllProductStatistic();

    deleteFromTable("PMethds");       //    presentationMethodManager->handleDeleteAll();
    deleteFromTable("SGraphs");       //    streamingGraphicManager->handleDeleteAll();

    deleteFromTable("COLCats");       //    coolManager->handleDeleteAllCOOLCategories();
    deleteFromTable("COLCtrs");       //    coolManager->handleDeleteAllCOOLCountries();
    deleteFromTable("COLStps");       //    coolManager->handleDeleteAllCOOLProductionSteps();
    deleteFromTable("COLPTxs");       //    coolManager->handleDeleteAllCOOLPretexts();
    deleteFromTable("COLTxts");       //    coolManager->handleDeleteAllCOOLTexts();
    deleteFromTable("COLLsts");       //    coolManager->handleDeleteAllCOOLShortlists();
    deleteFromTable("COLLTxs");       //    coolManager->handleDeleteAllCOOLListTexts();

    deleteFromTable("ProductTexts");  //    expandedTextManager->handleDeleteAll();
                                //    specialMessageManager->handleDeleteAll();
                                //    productNoteManager->handleDeleteAll();
                                //    productText5Manager->handleDeleteAll();
                                //    productText6Manager->handleDeleteAll();
                                //    productText7Manager->handleDeleteAll();
                                //    productText8Manager->handleDeleteAll();
                                //    productText9Manager->handleDeleteAll();
                                //    marqueeTextManager->handleDeleteAll();

    deleteFromTable("Classes");       //    classManager->handleDeleteAll();
    deleteFromTable("NutrFs");        //    nutrifactsManager->handleDeleteAll();
    deleteFromTable("Categrs");       //    categoryManager->handleDeleteAll();
    deleteFromTable("Spcials");       //    specialsManager->handleDeleteAll();

    deleteFromTable("TrTypes");       //    transactionTypeManager->handleDeleteAll();

    deleteFromTable("Prods");         //    productManager->handleDeleteAll();
    deleteFromTable("Depts");         //    deptManager->handleDeleteAll();

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
void customerHook::deleteFromTable(QString tableName)
{
    qint64 beginTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
    QString query("delete from ");
    QSqlQuery deleteSql(query + tableName, *scaleDatabase_);
    qint64 endTime = QDateTime::currentDateTime().toMSecsSinceEpoch();

    QSqlError err( deleteSql.lastError() );
    LOG_DEBUG << "Result of deleting " << tableName.toStdString()
                 << "   error type: " << err.type()
                 << "   native error code: " << err.nativeErrorCode().toStdString()
                 << "   error text: " << err.text().toStdString()
                 << "   database text: " << err.databaseText().toStdString()
                 << "   driver text: " << err.driverText().toStdString();

    LOG_DEBUG << "Elapsed time to delete " << tableName.toStdString() << ":  " << endTime - beginTime << " ms";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void customerHook::PresentationMethod_verifyReservedRecord(int PMType, QString PMName1)
{
    QSqlTableModel dbRM;
    dbRM.setTable("PMethds");
    dbRM.setEditStrategy(QSqlTableModel::OnManualSubmit);
    QSqlRecord dbRec;

    QString filter( "PMType=" + QString::number(PMType) );
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

        saveRecordToDatabase( "PMethds", false, &dbRM, dbRec );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void customerHook::TransactionType_verifyReservedRecord(int TTNum, QString TTName1, int TTAffPr, int TTBkDt, int TTAutoUPC, int TTManUPC, QString TTUPCDigit, int TTUseBgClr, int TTBgColor)
{
    QSqlTableModel dbRM;
    dbRM.setTable("TrTypes");
    dbRM.setEditStrategy(QSqlTableModel::OnManualSubmit);
    QSqlRecord dbRec;

    QString filter( "TTNum=" + QString::number(TTNum) );
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

        saveRecordToDatabase( "TrTypes", false, &dbRM, dbRec );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void customerHook::Class_verifyReservedRecord(int DeptNum, int ClNum, QString ClDesc1, int Cla_ClNum)
{
    QSqlTableModel dbRM;
    dbRM.setTable("Classes");
    dbRM.setEditStrategy(QSqlTableModel::OnManualSubmit);
    QSqlRecord dbRec;

    QString filter("ClNum=" + QString::number(ClNum) + " and DeptNum=" + QString::number(DeptNum));
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

        saveRecordToDatabase( "Classes", false, &dbRM, dbRec );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void customerHook::Category_verifyReservedRecord(int DeptNum, int CaNum, int Cat_CaNum, QString CaName1, QString CaName2)
{
    QSqlTableModel dbRM;
    dbRM.setTable("Categrs");
    dbRM.setEditStrategy(QSqlTableModel::OnManualSubmit);
    QSqlRecord dbRec;

    QString filter("CaNum=" + QString::number(CaNum) + " and DeptNum=" + QString::number(DeptNum));
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

        saveRecordToDatabase( "Categrs", false, &dbRM, dbRec );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void customerHook::Special_verifyReservedRecord(int DeptNum, int SPNum, QString SPName1, QString SPImag1, QString SPImag2, int SPDefL)
{
    QSqlTableModel dbRM;
    dbRM.setTable("Spcials");
    dbRM.setEditStrategy(QSqlTableModel::OnManualSubmit);
    QSqlRecord dbRec;

    QString filter("SPNum=" + QString::number(SPNum) + " and DeptNum=" + QString::number(DeptNum));
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

        saveRecordToDatabase( "Spcials", false, &dbRM, dbRec );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void customerHook::Dept_verifyReservedRecord(int DeptNum, QString DeptName1, QString DeptName2)
{
    QSqlTableModel dbRM;
    dbRM.setTable("Depts");
    dbRM.setEditStrategy(QSqlTableModel::OnManualSubmit);
    QSqlRecord dbRec;

    QString filter("DeptNum=" + QString::number(DeptNum));
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

        saveRecordToDatabase( "Depts", false, &dbRM, dbRec );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void customerHook::customAction( const QString& commandString, int &result, QString& resultExtended )
{
    QMutexLocker guard( &m_ );

    if ( !scaleDatabase_ )
    {
        LOG_WARN << "database is not open!";
        result = -2;
        return;
    }

    LOG_DEBUG << "perform custom action " << commandString.toStdString();
    result = -1;

    QString action( commandString );
    QString pluNum;
    QString deptNum( "0" );

    // parse out command string parameters
    const QStringList actionParams( commandString.split("|") );

    if ( 3 <= actionParams.size() )
        deptNum = actionParams[2];
    if ( deptNum.isEmpty() )
        deptNum = "0";

    if ( 2 <= actionParams.size() )
        pluNum = actionParams[1];

    if ( 1 <= actionParams.size() )
        action = actionParams[0];

    LOG_DEBUG << "action=" << action.toStdString() << "  pluNum=" << pluNum.toStdString() << "  deptNum=" << deptNum.toStdString();


    // full plu database load w/ delete (restore)  ||  full plu database load (merge)
    if (( fullPluLoadCommand_ == action ) || ( fullPluLoadWithDeleteCommand_ == action ))
    {
        LOG_INFO << action.toStdString();

        // we are already processing plus!
        if ( databaseLoadBackgroundProcess_.isRunning() )
        {
            LOG_WARN << "database load is already in progress!";

            // tell operator we are loading database
            errorDialogManager::instance()->showLoadingDatabaseInProgressError( messageFadeTime );

            resultExtended = action + " already processing load!";
            LOG_INFO << resultExtended.toStdString();
            result = 1;
        }
        else
        {
            const bool deleteProducts( fullPluLoadWithDeleteCommand_ == action );

            // background this process
            databaseLoadBackgroundProcess_ = QtConcurrent::run( this, &_Myt::getAllProducts, deptNum, deleteProducts );

            resultExtended = action + " task backgrounded";
            LOG_INFO << resultExtended.toStdString();
            result = 0;
        }
    }

    // single plu record load w/ delete (restore)  ||  single plu record load (merge)
    else if ( singlePluLoadCommand_ == action || singlePluLoadWithDeleteCommand_ == action )
    {
        LOG_INFO << action.toStdString() << " plu " << pluNum.toStdString();

        if ( pluNum.isEmpty() )
        {
            resultExtended = "No plu number entered";
            return;
        }

        if ( singlePluLoadWithDeleteCommand_ == action )
            deletePluFromDb( pluNum, deptNum );

        // **************************************************************
        // See if the PLU is on the Walmart Server
        // **************************************************************

        QJsonDocument doc;

        LOG_TRACE << "invoke api...";
        walmartapi::RequestStatus apiResult = api()->getOneItem( pluNum, country(), storeNum(), deptNum, doc );

        LOG_TRACE << "invoke api complete " << apiResult;

        QDateTime stamp;
        QString endpoint;

        if ( walmartapi::importLastUsage( stamp, endpoint ) )
        {
            const int delta( stamp.msecsTo( QDateTime::currentDateTime() ) );
            const int maxTime( minRelookupTime_.toInt() );

            LOG_TRACE << "api last usage delta " << delta << " with max time " << maxTime;

            // api was able to connect, we are probably online
            if (( 0 < delta ) && ( delta < maxTime ))
            {
                LOG_DEBUG << "updating network checking thread with most recent api stamp";
                net_->setLastOnlineTime( stamp );
            }
        }

        LOG_TRACE << "process apiResult ...";

        // process result
        if ( apiResult != walmartapi::SUCCESS )
            LOG_WARN << "bad result " << apiResult;
        else
        {
            int numItemsReceived = 0;

            LOG_DEBUG << "customAction - " << action.toStdString() << " - api()->getOneItem returned SUCCESS - we received PLU data from the server";

            // parse out object to process
            QJsonObject obj;

            if ( doc.isArray() )
            {
                const QJsonArray jsonArray( doc.array() );
                LOG_DEBUG << "jsonArray size = " << jsonArray.size();

                numItemsReceived = jsonArray.size();   // should only be 1

                if ( numItemsReceived )
                    obj = jsonArray[0].toObject();
            }
            else if ( doc.isObject() )
            {
                obj = doc.object();
                LOG_DEBUG << "json object found";

                numItemsReceived = 1;
            }

            // we found object to process
            if ( numItemsReceived )
            {
                int numItemsProcessed = 0;

                QString jsonData( doc.toJson() );
                LOG_DEBUG << "\nCustom Json Data: " << "\n" << jsonData.toStdString() << "\n";

                //LOG_DEBUG << "customAction - " << action.toStdString() << " - PLU Data: " << HEX_DUMP( jsonArray[i].toObject(), jsonArray[i].toObject().length() );               // prints out everything so for now I commented out
                if ( updateDbWithPlu( deptNum, obj ) )
                    numItemsProcessed++;

                resultExtended = action + " items received: " + QString::number(numItemsReceived) + "  processed: " + QString::number(numItemsProcessed);
                LOG_DEBUG << resultExtended.toStdString();
                result = 0;
            }
            else
            {
                resultExtended = action + " - getOneItem - did not receive a JSON array or object to process";
                LOG_WARN << resultExtended.toStdString();
            }
        }
    }

    LOG_TRACE << "done";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void customerHook::printJsonData(const QJsonObject &pluJsonObj)
{
    QJsonValue keyData = pluJsonObj["pluNbr"];

    if( QString::number(keyData.toDouble()) > 0 )
    {
        QString printJson( " JSON Object Server Data\n");
        foreach( const QString& key, pluJsonObj.keys() )
        {
            QJsonValue jvalue = pluJsonObj.value(key);
            printJson.append( "\"" + key + "\":");

            switch ( jvalue.type() )
            {
                case  QJsonValue::Null:
                    printJson.append("");
                    break;
                case  QJsonValue::String:
                    printJson.append( "\"" + jvalue.toString() + "\"" );
                    break;
                case  QJsonValue::Double:
                    printJson.append( QString::number( jvalue.toDouble() ) );
                    break;
                case  QJsonValue::Bool:
                    printJson.append( QString::number(jvalue.toBool() ) );
                    break;
                default:
                    printJson.append( jvalue.toString() );
                    break;
            }

            printJson.append( "\n" );
        }
        LOG_DEBUG << printJson.toStdString();
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////
void customerHook::updatePriceWeightFields(QJsonObject &pluJsonObj)
{
    static const QString sellPrice( "sellPrice" );
    static const QString tareWeight( "tareWeight" );
    static const QString tareWeightB( "tareWeightB" );

    QMap<QString,int> multipliers;
    multipliers[sellPrice] = 100;
    multipliers[tareWeight] = 1000;
    multipliers[tareWeightB] = 1000;

    // Fix the sell price/weight - Invalid Prices include these examples:
    // 1: incoming JSON = 4.0,  means 4.00, saved to our db as 004 (4 cents not as 4 dollars as intended)
    // 2: incoming JSON = 68.0, means 68.00, saved to our db as 068 (68 cents, not as 68 dollars as intended)
    // 3: verify prices like 3.30, 5.50, 150, 0.68, 0.001, 2.5, 3, 1, 10, 100, 0.1, 0.10, 0.01, etc

    // Steps
    // convert to a whole number to a QString which drops the trailing .0
    // multiply the string times 100 or 1000 to get the correct precision
    // stuff it back into the JSON as string not a double which would lose the precision again

    foreach ( const QString& key, multipliers.keys() )
        if ( pluJsonObj.contains( key ) )
        {
            const QJsonValue keyData( pluJsonObj[key] );

            if ( keyData.isDouble() )
            {
                const double val( keyData.toDouble() * multipliers[key] );
                pluJsonObj[key] = QString::number( int( std::round( val ) ) );
            }
        }

}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool customerHook::healthCheck( const QJsonDocument& info, QJsonDocument& response )
{
    QMutexLocker guard( &m_ );

    LOG_TRACE << "invoke api...";
    const walmartapi::RequestStatus status( api()->healthCheck( country(), storeNum(), QString(), info, response ) );

    LOG_TRACE << "invoke api complete " << status;

    if ( walmartapi::SUCCESS != status )
        return false;

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool customerHook::testServerLookupRules( qint64 pluRecExpiryTime, bool dbRecordExists )
{
    bool skipServerLookup( false );
    bool phaseTwoServerLookup( false );     // just want to make sure something added

    if( ( FindPluOnServerRules::ServerLookupDisabled <= serverLookupRule_ ) && ( serverLookupRule_ <= FindPluOnServerRules::ServerLookupAlwaysThenUseCache ) )
    {
        // The current state after phase 1 means we look up the data in the DB, and
        // and then we may look it up on the server based on what rules we are following
        LOG_DEBUG << " " << "We have a server lookup rule so find out if we lookup on the server or not.";
        phaseTwoServerLookup = true;
    }

    // if false we are just going to follow phase 1 rules. Look up in Local DB, Look up on Server, follow CACHE RULES
    if( phaseTwoServerLookup )
    {
        if( FindPluOnServerRules::ServerLookupDisabled == serverLookupRule_ )
        {
            // USE CASE SOF-3785 Disable PLU lookup from item query hook function completely
            LOG_DEBUG << " " << "ServerLookupDisabled = TRUE do nothing and look up locally on scale every time.";
            skipServerLookup = true;
        }
        else if ( FindPluOnServerRules::ServerLookupIfNoLocalCopy == serverLookupRule_ )
        {
            // USE CASE: 3786 - Perform PLU lookup (on server) only when missing from Database
            // If true, and the PLU exists in the local scale DB then skip
            // reading it from the Server and immediately return and look up local

            if( dbRecordExists )
            {
                // give us the chance to exit early
                LOG_DEBUG << " " << "ServerLookupIfNoLocalCopy = TRUE and PLU found in scale DB so no lookup on server. Found so exit";
                skipServerLookup = true;
            }
            else
            {
                // we were supposed to exit if we found the PLU on local server, didnt find it, so do a server lookup and try and find it.
                LOG_DEBUG << " " << "ServerLookupIfNoLocalCopy = TRUE but we didnt find the PLU locally, so go find it on the server";
            }
        }
        else if( FindPluOnServerRules::ServerLookupIfNotInDBOrExpired == serverLookupRule_ )
        {
            bool isExpired( false );
            if( dbRecordExists )
                isExpired = isCacheTimeExpired( pluRecExpiryTime );

            LOG_DEBUG << " " << "ServerLookupIfNotInDBOrExpired() dbRecordExists: " << dbRecordExists;
            LOG_DEBUG << " " << "ServerLookupIfNotInDBOrExpired() isExpired: " << isExpired;

            if ( !dbRecordExists  || isExpired )
            {
                // USE CASE 3787 Perform PLU lookup ONLY when missing from Database and/or cache expired
                // PLU does not exist on server OR the cache as expired - return FALSE (actually nothing)
                // and look up the PLU on the server. Would exit if PLU exists and the cache timer not expired.
                LOG_DEBUG << " " << "ServerLookupIfNotInDBOrExpired = TRUE but we didnt find the PLU locally or the cache was expired so lookup on server";
            }
            else
            {
                if( dbRecordExists && !isExpired )
                {
                    // Double check and make sure
                    LOG_DEBUG << " " << "ServerFindPluNotInDBCacheExp() = TRUE and PLU was found in scale DB and the timer has not expired. Found it so exit";
                    skipServerLookup = true;
                }
            }
        }
        else if (( FindPluOnServerRules::ServerLookupAlways == serverLookupRule_ ) ||
                 ( FindPluOnServerRules::ServerLookupAlwaysThenUseCache == serverLookupRule_ ))
        {
            // USE CASE 3788 Perform PLU lookup (on the server) all the time
            LOG_DEBUG << " " << "ServerFindPlu() = TRUE so look up the PLU on the server every time.";
        }
        else
        {
            LOG_DEBUG << " " << "ERROR - No special rules to follow for phase 2";
        }
    }

    return skipServerLookup;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
walmartapi *customerHook::api()
{
    QThread *currentThread( QThread::currentThread() );

    walmartapi *iface;

    if ( api_.contains( currentThread ) )
        iface = api_[currentThread];
    else
    {
        iface = api_[currentThread] = new walmartapi();

        // read in configuration web headers
        readConfigurationHeaders( iface );

        iface->setHttpHeadersMap( "SCALE_ID" , serialNum() );
        iface->setHttpHeadersMap( "SCALE_NUMBER", scaleNumberFromIpAddress( ipAddress() ) );

        iface->setHttpHeadersMap( "IP_ADDRESS", ipAddress() );

        // setup ssl options
        networkAccess *network( qobject_cast<networkAccess*>( iface->networkAccessManager() ) );

        if ( network )
        {
            if ( ignoreAllSslErrors_ )
            {
                LOG_INFO << "ignore all ssl errors";
                network->setIgnoreAllSslErrors( true );
            }

            QList<QSslError> errors;

            if ( ignoreSslSelfSignedCert_ )
            {
                LOG_INFO << "ignore ssl self signed certs";
                errors.append( QSslError::SelfSignedCertificate );
            }

            if ( ignoreSslHostNameMismatch_ )
            {
                LOG_INFO << "ignore ssl host name mismatch";
                errors.append( QSslError::HostNameMismatch );
            }

            if ( errors.size() )
                network->setIgnoreSslErrors( errors );
        }

        if ( FindPluOnServerRules::ServerLookupAlwaysThenUseCache == serverLookupRule_ )
        {
            LOG_INFO << "using CACHE when offline to API";
            iface->setUseCacheWhenOffline( true );
        }
    }

    return iface;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void customerHook::readAFieldMapping(QMap<QString, QString> &theMap, const QString nameBase)
{
    //LOG_DEBUG << "put the db record column mappings into the map for: " << nameBase.toStdString();

    theMap.clear();

    QSettings internal_configs( "/usr/share/walmart-apphook/hobart_internal.config", QSettings::IniFormat );

    QString currentField;
    bool moreFields = true;
    int offset = 0;
    while (moreFields)
    {
        currentField = nameBase + QStringLiteral("%1").arg(offset++, 3, 10, QLatin1Char('0'));
        QString value = internal_configs.value(currentField).toString();
        //LOG_DEBUG << "try to find  " << currentField.toStdString() << "  value is " << value.toStdString();
        if ( value.isEmpty() )
        {
            //LOG_DEBUG << "end of keys ... couldn't find " << currentField.toStdString();
            moreFields = false;
        }
        else
        {
            int i = value.indexOf(':');
            if (i >= 0)
            {
                QString walmartFieldName(value.left(i));
                QString hobartFieldName(value.section(QChar(':'), 1));
                //LOG_DEBUG << "walmartFieldName = " << walmartFieldName.toStdString() << "  hobartFieldName = " << hobartFieldName.toStdString();

                theMap[walmartFieldName] = hobartFieldName;
            }
        }
    }

    //LOG_DEBUG << "readAFieldMapping, fields loaded = " << theMap.size();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void customerHook::readConfigurationHeaders( walmartapi *face )
{
    if ( !face )
        return;

    QSettings web_configs( "/usr/share/walmart-apphook/web_headers.config", QSettings::IniFormat );

    face->setHttpHeadersMap( "SCALE_TYPE" , web_configs.value("Scale_Type").toString() );                       // set header
    face->setHttpHeadersMap( "WM_CONSUMER.ID" , web_configs.value("WM_CONSUMER.ID").toString() );               // set header
    face->setHttpHeadersMap( "WM_SVC.NAME" , web_configs.value("WM_SVC.NAME").toString() );                     // set header
    face->setHttpHeadersMap( "WM_SVC.VERSION" , web_configs.value("WM_SVC.VERSION").toString() );               // set header
    face->setHttpHeadersMap( "WM_SVC.ENV" , web_configs.value("WM_SVC.ENV").toString() );                       // set header

    face->setHttpHeadersMap( "TimeoutLength" , web_configs.value("TimeoutLength").toString() );                     // set configuration
    face->setHttpHeadersMap( "AllItemsTimeoutLength" , web_configs.value("AllItemsTimeoutLength").toString() );     // set configuration
    face->setHttpHeadersMap( "TimeoutRetryInterval" , web_configs.value("TimeoutRetryInterval").toString() );       // set configuration

    face->setHttpHeadersMap( "NumberOfRequestsSaved" , web_configs.value("NumberOfRequestsSaved").toString() );     // set configuration

    face->setHttpHeadersMap( "BannerCode" , web_configs.value("BannerCode").toString() );                       // set configuration
    face->setHttpHeadersMap( "Department" , web_configs.value("Department").toString() );                       // set configuration
    face->setHttpHeadersMap( "NoQueryParams" , web_configs.value("NoQueryParams").toString() );                 // set configuration

    // look for custom headers
    for ( int i = 1; i <= 9; i++ )
    {
        const QString headerKey( "Header" + QString::number( i ) );
        const QString headerVal( "HeaderVal" + QString::number( i ) );

        const QString key( web_configs.value(headerKey).toString() );
        const QString val( web_configs.value(headerVal).toString() );

        if (( key.length() ) && ( val.length() ))
            face->setHttpHeadersMap( key, val, true );              // set configuration
        else
        {
            break;
        }
    }

    const QString sslActiveStr = web_configs.value( "HttpsActive" ).toString();
    const QString addrhostStr = web_configs.value( "WebServiceAddress" ).toString();
    const QString getOneItemStr = web_configs.value( "GetOneItem" ).toString();
    const QString getAllItemsStr = web_configs.value( "GetAllItems" ).toString();
    const QString healthCheckStr = web_configs.value( "HealthCheck" ).toString();

    // set some class vars so other methods can use them
    face->setSSL( sslActiveStr == "1" ? true : false );
    face->setHost( addrhostStr );
    face->setEndpointPath( "GetOneItem", getOneItemStr );
    face->setEndpointPath( "GetAllItems", getAllItemsStr );
    face->setEndpointPath( "HealthCheck", healthCheckStr );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void customerHook::readConfiguration()
{
    readAFieldMapping(pluFields_, "plufield");
    readAFieldMapping(nutriFields_, "nutrifield");
    readAFieldMapping(expandedTextFields_, "exptxtfield");
    readAFieldMapping(prDefsFields_, "prdefsfield");

    QSettings internal_configs( "/usr/share/walmart-apphook/hobart_internal.config", QSettings::IniFormat );

    minRelookupTime_ = internal_configs.value("MinRelookupTime").toString();
    retriableErrors_ = internal_configs.value("RetriableErrors").toString();
    verboseDebug_ = internal_configs.value("VerboseDebug").toBool();

    if ( minRelookupTime_.isEmpty() )
        minRelookupTime_ = "10000";

    if ( retriableErrors_.isEmpty() )
        retriableErrors_ = "5:";

    fullPluLoadCommand_ = internal_configs.value("FullPluLoadCommand").toString();
    fullPluLoadWithDeleteCommand_ = internal_configs.value("FullPluLoadWithDeleteCommand").toString();
    singlePluLoadCommand_ = internal_configs.value("SinglePluLoadCommand").toString();
    singlePluLoadWithDeleteCommand_ = internal_configs.value("SinglePluLoadWithDeleteCommand").toString();

    if ( fullPluLoadCommand_.isEmpty() )
        fullPluLoadCommand_ = "MergeDatabaseLoad";

    if ( fullPluLoadWithDeleteCommand_.isEmpty() )
        fullPluLoadWithDeleteCommand_ = "FullDatabaseLoad";

    if ( singlePluLoadCommand_.isEmpty() )
        singlePluLoadCommand_ = "MergeProductLoad";

    if ( singlePluLoadWithDeleteCommand_.isEmpty() )
        singlePluLoadWithDeleteCommand_ = "FullProductLoad";

    LOG_INFO << "action " << fullPluLoadCommand_.toStdString();
    LOG_INFO << "action " << fullPluLoadWithDeleteCommand_.toStdString();
    LOG_INFO << "action " << singlePluLoadCommand_.toStdString();
    LOG_INFO << "action " << singlePluLoadWithDeleteCommand_.toStdString();

    QSettings web_configs( "/usr/share/walmart-apphook/web_headers.config", QSettings::IniFormat );

    setCacheExpiration( web_configs.value("CacheExpirationTime").toString() );                                                                             // set this so it can be used other places
    setCacheExpirationUnits( web_configs.value("CacheExpirationUnits").toString() );                                                                       // set this so it can be used other places
    setRemoveItemsFromCache( web_configs.value("RemoveItemsFromCache").toString() );                                                                       // set this so it can be used other places

    setConnectionCheckInterval( web_configs.value( "ConnectionCheckInterval" ).toInt() * 1000 );
    setHealthCheckInterval( web_configs.value( "HealthCheckInterval" ).toInt() * 60000 );

    // save off info for health checking
    walmartCanadaMaker_ = web_configs.value("Scale_Type").toString();
    walmartCanadaBanner_ = web_configs.value("BannerCode").toString();

    const QString format( web_configs.value( "HealthCheckFormat" ).toString() );

    if ( format.length() )
        setHealthCheckFormat( format );

    LOG_INFO << "cache expiration timer:" << "\n" <<
        "    CacheExpirationTime  " << cacheExpiration().toStdString() << "\n" <<
        "    CacheExpirationUnits " << cacheExpirationUnits().toStdString();

    const QString rule( web_configs.value( "ServerLookupRule" ).toString() );

    if ( rule.isEmpty() )
        serverLookupRule_ = 0;
    else
        serverLookupRule_ = rule.toInt();

    LOG_INFO << "server lookup rule " << serverLookupRule_;

    //
    // Dialog Setup
    //

    const QString off_print_dialog_1_start( web_configs.value("OfflinePrintDialog.1.Start").toString() );
    const QString off_print_dialog_1_end( web_configs.value("OfflinePrintDialog.1.End").toString() );

    const QString off_print_dialog_2_start( web_configs.value("OfflinePrintDialog.2.Start").toString() );
    const QString off_print_dialog_2_end( web_configs.value("OfflinePrintDialog.2.End").toString() );

    const QString off_verify_dialog_1_start( web_configs.value("OfflineVerifyDialog.1.Start").toString() );
    const QString off_verify_dialog_1_end( web_configs.value("OfflineVerifyDialog.1.End").toString() );
    const QString off_verify_dialog_1_freq( web_configs.value("OfflineVerifyDialog.1.Frequency").toString() );

    const QString off_verify_dialog_2_start( web_configs.value("OfflineVerifyDialog.2.Start").toString() );
    const QString off_verify_dialog_2_end( web_configs.value("OfflineVerifyDialog.2.End").toString() );
    const QString off_verify_dialog_2_freq( web_configs.value("OfflineVerifyDialog.2.Frequency").toString() );

    const QString msg_fade_time( web_configs.value("MessageFadeTime").toString() );

    LOG_DEBUG << "dialog times (from config):" << "\n" <<
        "    OfflinePrintDialog.1.Start      " << off_print_dialog_1_start.toStdString() << "\n" <<
        "    OfflinePrintDialog.1.End        " << off_print_dialog_1_end.toStdString() << "\n" <<
        "    OfflinePrintDialog.2.Start      " << off_print_dialog_2_start.toStdString() << "\n" <<
        "    OfflinePrintDialog.2.End        " << off_print_dialog_2_end.toStdString() << "\n" <<
        "    OfflineVerifyDialog.1.Start     " << off_verify_dialog_1_start.toStdString() << "\n" <<
        "    OfflineVerifyDialog.1.End       " << off_verify_dialog_1_end.toStdString() << "\n" <<
        "    OfflineVerifyDialog.1.Frequency " << off_verify_dialog_1_freq.toStdString() << "\n" <<
        "    OfflineVerifyDialog.2.Start     " << off_verify_dialog_2_start.toStdString() << "\n" <<
        "    OfflineVerifyDialog.2.End       " << off_verify_dialog_2_end.toStdString() << "\n" <<
        "    OfflineVerifyDialog.2.Frequency " << off_verify_dialog_2_freq.toStdString() << "\n" <<
        "    MessageFadeTime                 " << msg_fade_time.toStdString();

    static const int HR_TO_MSEC = 60 * 60 * 1000;
    static const int MIN_TO_MSEC = 60 * 1000;

    offlinePrintDialog1.first = (off_print_dialog_1_start.isEmpty() ? 144 : off_print_dialog_1_start.toInt()) * HR_TO_MSEC;
    offlinePrintDialog1.second = (off_print_dialog_1_end.isEmpty() ? 120 : off_print_dialog_1_end.toInt()) * HR_TO_MSEC;

    offlinePrintDialog2.first = (off_print_dialog_2_start.isEmpty() ? 24 : off_print_dialog_2_start.toInt()) * HR_TO_MSEC;
    offlinePrintDialog2.second = (off_print_dialog_2_end.isEmpty() ? 0 : off_print_dialog_2_end.toInt()) * HR_TO_MSEC;

    offlineVerifyDialog1.first = (off_verify_dialog_1_start.isEmpty() ? 120 : off_verify_dialog_1_start.toInt()) * HR_TO_MSEC;
    offlineVerifyDialog1.second = (off_verify_dialog_1_end.isEmpty() ? 72 : off_verify_dialog_1_end.toInt()) * HR_TO_MSEC;
    offlineVerifyDialog1_Freq = (off_verify_dialog_1_freq.isEmpty() ? 60 : off_verify_dialog_1_freq.toInt()) * MIN_TO_MSEC;

    offlineVerifyDialog2.first = (off_verify_dialog_2_start.isEmpty() ? 72 : off_verify_dialog_2_start.toInt()) * HR_TO_MSEC;
    offlineVerifyDialog2.second = (off_verify_dialog_2_end.isEmpty() ? 24 : off_verify_dialog_2_end.toInt()) * HR_TO_MSEC;
    offlineVerifyDialog2_Freq = (off_verify_dialog_2_freq.isEmpty() ? 30 : off_verify_dialog_2_freq.toInt()) * MIN_TO_MSEC;

    messageFadeTime = (msg_fade_time.isEmpty() ? 5000 : msg_fade_time.toInt());

    LOG_INFO << "dialog times:" << "\n" <<
        "    offlinePrintDialog1 start  " << offlinePrintDialog1.first << " (" << offlinePrintDialog1.first / HR_TO_MSEC << " hr)" << "\n" <<
        "    offlinePrintDialog1 end    " << offlinePrintDialog1.second << " (" << offlinePrintDialog1.second / HR_TO_MSEC << " hr)" << "\n" <<
        "    offlinePrintDialog2 start  " << offlinePrintDialog2.first << " (" << offlinePrintDialog2.first / HR_TO_MSEC << " hr)" << "\n" <<
        "    offlinePrintDialog2 end    " << offlinePrintDialog2.second << " (" << offlinePrintDialog2.second / HR_TO_MSEC << " hr)" << "\n" <<
        "    offlineVerifyDialog1 start " << offlineVerifyDialog1.first << " (" << offlineVerifyDialog1.first / HR_TO_MSEC << " hr)" << "\n" <<
        "    offlineVerifyDialog1 end   " << offlineVerifyDialog1.second << " (" << offlineVerifyDialog1.second / HR_TO_MSEC << " hr)" << "\n" <<
        "    offlineVerifyDialog1 freq  " << offlineVerifyDialog1_Freq << " (" << offlineVerifyDialog1_Freq / MIN_TO_MSEC << " min)" << "\n" <<
        "    offlineVerifyDialog2 start " << offlineVerifyDialog2.first << " (" << offlineVerifyDialog2.first / HR_TO_MSEC << " hr)" << "\n" <<
        "    offlineVerifyDialog2 end   " << offlineVerifyDialog2.second << " (" << offlineVerifyDialog2.second / HR_TO_MSEC << " hr)" << "\n" <<
        "    offlineVerifyDialog2 freq  " << offlineVerifyDialog2_Freq << " (" << offlineVerifyDialog2_Freq / MIN_TO_MSEC << " min)" << "\n" <<
        "    messageFadeTime            " << messageFadeTime;

    //
    // Ignore SLL Errors
    //

    ignoreAllSslErrors_ = ("1" == web_configs.value("IgnoreAllSslErrors").toString());
    ignoreSslHostNameMismatch_ = ("1" == web_configs.value("IgnoreSslHostNameMismatch").toString());
    ignoreSslSelfSignedCert_ = ("1" == web_configs.value("IgnoreSslSelfSignedCert").toString());

    LOG_INFO << "ignore ssl errors:" << "\n" <<
        "    IgnoreAllSslErrors        " << ignoreAllSslErrors_ << "\n" <<
        "    IgnoreSslHostNameMismatch " << ignoreSslHostNameMismatch_ << "\n" <<
        "    IgnoreSslSelfSignedCert   " << ignoreSslSelfSignedCert_ << "\n";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString customerHook::queryConfigs( const QString& configName)
{
    QSqlTableModel dbConfigsRM;
    dbConfigsRM.setTable("Configs");
    dbConfigsRM.setEditStrategy(QSqlTableModel::OnManualSubmit);

    QString filter("CoName='" + configName + "'");
    //LOG_DEBUG << "queryConfigs - filter = " << filter.toStdString();
    dbConfigsRM.setFilter(filter);

    bool dbRecExists = dbConfigsRM.select();
    int rowCount = dbConfigsRM.rowCount();
    //LOG_DEBUG << "Configs select, select() returned " << dbRecExists << "  rowCount = " << rowCount;

    QString configValue("");

    QSqlRecord dbConfigRec;
    if (dbRecExists && rowCount > 0)
    {
        dbConfigRec = dbConfigsRM.record(0);
        if (dbConfigRec.isEmpty())
            dbRecExists = false;
        configValue = dbConfigRec.value(1).toString();
        //LOG_DEBUG << "Configs read " << filter.toStdString() << "  value = " << configValue.toStdString();
    }
    else
        LOG_WARN << "Could not get Configs from db " << filter.toStdString();

    return configValue;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool customerHook::querySerialNumber()
{
    bool setserial = false;

    if( serialNum().length() > 0)
    {
        LOG_DEBUG << "we have a serial number so exit";
        return setserial;
    }

    QByteArray buffer;
    QString filename = "/dev/mtd3";

    //load the file
    if ( QFile::exists( filename ) )
    {
        QFile f( filename );
        if ( f.open( QFile::ReadOnly ) )
        {
            LOG_DEBUG << "Opened Flash file";

            buffer = f.readAll();
            f.close();
        }

        if( buffer.length() )
        {
            static const QString modelfrontstr( "<SStyle>" );
            static const QString modelendstr( "</SStyle>" );
            static const QString serialfrontstr( "<Serial>" );                       // <Serial>1234567</Serial>
            static const QString serialendstr( "</Serial>" );                        // 130     138    145    154

            QString str_data( buffer );

            int start_serial = str_data.indexOf(serialfrontstr);
            int end_serial   = str_data.indexOf(serialendstr);
            int serial_begin = start_serial + serialfrontstr.length();
            int serial_len   = end_serial - serial_begin;

            QString tmpSerialNum( buffer.mid(serial_begin  , serial_len) );
            LOG_DEBUG << "buffer tmpSerialNum " << tmpSerialNum.toStdString();

            int start_model = str_data.indexOf(modelfrontstr);
            int end_model   = str_data.indexOf(modelendstr);
            int model_begin = start_model + modelfrontstr.length();
            int model_len   = end_model - model_begin;

            QString tmpModel( buffer.mid(model_begin  , model_len) );
            LOG_DEBUG << "buffer tmpModel " << tmpModel.toStdString();

            setserial = true;

            setModel( tmpModel );
            setSerialNum( tmpSerialNum );
        }
        else
            LOG_DEBUG << "nothing in the buffer";
    }
    else
        LOG_DEBUG << "File did not exist";

    return setserial;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString customerHook::scaleNumberFromIpAddress( const QString& addr )
{
    QString scaleNum;

    // parse out scale number (last octet)
    QStringList octets( addr.split(".") );

    if ( 4 <= octets.size() )
        scaleNum = octets[3];

    return scaleNum;
}
