/**
 * @file hooktiliterbase.cpp
 *
 * @copyright Copyright (c) 2021 Hobart Corporation
 * @author Randy Blankley
 */

#include "apitiliter.h"
#include "common.h"
#include "stringsjson.h"
#include "hooktiliterbase.h"

#include <QCoreApplication>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QObject>
#include <QSettings>
#include <QStringList>
#include <QUrl>

#if defined(Q_OS_LINUX)
static const QString WEB_HEADERS_FILE( "/usr/share/tiliter-apphook/web_headers.config" );
#elif defined(Q_OS_WIN32)
static const QString WEB_HEADERS_FILE( "web_headers.config" );
#endif

static const QString LOW_SCORE( "Low score" );
static const QString BACKGROUND( "Background" );
static const QString HAND( "Non-Produce - Hand" );

static const QString PRODUCT( "product" );
static const QString SPECIAL( "special" );

///////////////////////////////////////////////////////////////////////////////////////////////////
tiliterHookBase::tiliterHookBase( QObject *parent ) :
    _Mybase( parent ),
    m_( QMutex::Recursive ),
    api_( nullptr ),
    requestTimeout_( 0 ),
    requestNumRetries_( 0 )
{
    LOG_DEBUG << "ctor";

    // move to application thread
    moveToThread( QCoreApplication::instance()->thread() );

    // setup api
    api_ = new tiliterapi( this );

    connect( api_, &tiliterapi::processDocument, this, &_Myt::onProcessDocument );
    connect( api_, &tiliterapi::processFile, this, &_Myt::onProcessFile );

    connect( this, &tiliterHookBase::apiSend, api_, &tiliterapi::send, Qt::QueuedConnection );

    // setup api headers
    loadWebHeaders();

}

///////////////////////////////////////////////////////////////////////////////////////////////////
tiliterHookBase::~tiliterHookBase()
{
    LOG_DEBUG << "dtor";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void tiliterHookBase::onProcessDocument( const QJsonDocument& request, int status, const QJsonDocument& response )
{
    Q_UNUSED( request );

    QMutexLocker guard( &m_ );

    if (( 200 != status ) || ( !response.isObject() ))
    {
        LOG_WARN << "bad response";
        return;
    }

    const QJsonObject obj( response.object() );

    const QString pred_guid( obj[JSON_GUID].toString() );
    const double pred_time( obj[JSON_PREDICTION_TIME].toDouble() );

    // show item predictions
    const QJsonArray preds( obj[JSON_PREDICTIONS].toArray() );

    if ( preds.isEmpty() )
    {
        LOG_WARN << "empty predictions";
        return;
    }

    LOG_DEBUG << "===== results " << qPrintable( pred_guid ) << " (" << pred_time << " seconds) =====";

    QStringList products;

    foreach ( const QJsonValue& v, preds )
        if ( v.isObject() )
        {
            const QJsonObject item( v.toObject() );

            // check for empty or special prediction
            if ( 1 == preds.size() )
            {
                if (( LOW_SCORE == item[JSON_FULL_NAME].toString() ) && ( 100 == item[JSON_SCORE].toInt() ))
                {
                    LOG_INFO << "** LOW SCORE **";
                    sendPredictResults( LowScore );
                    return;
                }
                else if ( SPECIAL == item[JSON_PRODUCT_TYPE].toString() )
                {
                    if ( BACKGROUND == item[JSON_FULL_NAME].toString() )
                    {
                        LOG_INFO << "** BACKGROUND **";
                        sendPredictResults( Background );
                        return;
                    }
                    else if ( HAND == item[JSON_FULL_NAME].toString() )
                    {
                        LOG_INFO << "** HAND **";
                        sendPredictResults( Hand );
                        return;
                    }
                }
            }

            if ( PRODUCT != item[JSON_TYPE].toString() )
            {
                LOG_DEBUG << "skipping non-product";
                continue;
            }

            static const QString itemFormat( "%1 %2 %3 %4 %5 %6" );

            LOG_DEBUG << qPrintable( itemFormat
                .arg( item[JSON_RANK].toInt(), 3 )
                .arg( item[JSON_SCORE].toDouble(), 6 )
                .arg( item[JSON_CATEGORY].toString(), 16 )
                .arg( item[JSON_CUSTOM_ID].toString(), 8 )
                .arg( item[JSON_CUSTOM_NAME].toString(), 32 )
                .arg( item[JSON_PRODUCT_TYPE].toString(), 20 ) );

            products.append( item[JSON_CUSTOM_ID].toString() );
        }

    // display bag predictions
    const QJsonArray bag_preds( obj[JSON_BAG_PREDICTIONS].toArray() );

    if ( bag_preds.size() )
    {
        LOG_DEBUG << "----- BAGS ----";

        foreach ( const QJsonValue& v, bag_preds )
            if ( v.isObject() )
            {
                const QJsonObject bag( v.toObject() );

                static const QString itemFormat( "%1 %2" );

                LOG_DEBUG << qPrintable( itemFormat
                    .arg( bag[JSON_SCORE].toInt(), 3 )
                    .arg( bag[JSON_BAG_NAME].toString(), 16 ) );
            }
    }

    // display organic information
    if (( obj.contains( JSON_ORGANIC_INFO ) ) && ( obj[JSON_ORGANIC_INFO].isObject() ))
    {
        const QJsonObject organicInfo( obj[JSON_ORGANIC_INFO].toObject() );

        LOG_DEBUG << "----- ORGANIC INFO ----";

        static const QString itemFormat( "%1 %2" );

        LOG_DEBUG << qPrintable( itemFormat
            .arg( organicInfo[JSON_ACCURACY].toDouble(), 6 )
            .arg( organicInfo[JSON_THREASHOLD].toInt(), 3 ) );
    }

    sendPredictResults( Items, products );

    LOG_TRACE << "done";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void tiliterHookBase::onProcessFile( const QJsonDocument& request, int status, const QString& filenameCached, const QString& filename )
{
    Q_UNUSED( request )
    Q_UNUSED( filenameCached )

    LOG_DEBUG << "process file '" << qPrintable( filename ) << "' " << status;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void tiliterHookBase::predict()
{
    const QUrl url( formatEndpointUrl( resultUrl_ ) );

    LOG_DEBUG << "requesting tiliter prediction...";
    emit apiSend( url, QJsonDocument(), requestTimeout_, requestNumRetries_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void tiliterHookBase::loadWebHeaders()
{
    QSettings s( WEB_HEADERS_FILE, QSettings::IniFormat );

    tiliterapi::headersMap headers;

    // look for web headers
    for ( int i = 1; i <= 16; i++ )
    {
        const QString headerKey( "Header" + QString::number( i ) );
        const QString headerVal( "HeaderVal" + QString::number( i ) );

        const QString key( s.value( headerKey ).toString() );
        const QString val( s.value( headerVal ).toString() );

        if (( key.length() ) && ( val.length() ))
        {
            LOG_TRACE << "loaded header " << i << ": " << qPrintable( key ) << " " << qPrintable( val );
            headers[key.toLatin1()] = val.toLatin1();
        }
        else
        {
            break;
        }
    }

    // set web headers
    if ( headers.empty() )
        LOG_TRACE << "no web headers to set";
    else if ( api_ )
    {
        LOG_INFO << "setting web headers...";
        api_->setHeaders( headers );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QUrl tiliterHookBase::formatEndpointUrl( const QString& url ) const
{
    // use host address from DB unless empty or override
    QString addr( hostAddress() );

    if (( addr.isEmpty() ) || ( hostAddress_.length() ))
        addr = hostAddress_;

    // use host port from DB unless empty or override
    int port( hostPort() );

    if (( !port ) || ( 0 < hostPort_ ))
        port = hostPort_;

    // format
    QString endpoint( url );
    endpoint.replace( "{device}", addr );
    endpoint.replace( "{port}", QString::number( port ) );

    return QUrl( endpoint );
}
