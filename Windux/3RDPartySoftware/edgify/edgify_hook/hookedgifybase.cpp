/**
 * @file hookedgifybase.cpp
 *
 * @copyright Copyright (c) 2021 Hobart Corporation
 * @author Randy Blankley
 */

#include "apiedgify.h"
#include "common.h"
#include "stringsjson.h"
#include "hookedgifybase.h"
#include "uartedgify.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QHostInfo>
#include <QObject>
#include <QSettings>
#include <QStringList>
#include <QTemporaryDir>
#include <QUrl>

#if defined(Q_OS_LINUX)
static const QString WEB_HEADERS_FILE( "/usr/share/edgify-apphook/web_headers.config" );
#elif defined(Q_OS_WIN32)
static const QString WEB_HEADERS_FILE( "web_headers.config" );
#endif

static const QString UNKNOWN( "Unknown" );
static const QString BACKGROUND( "Background" ); // or perhaps "Platter" ?

static const QString CERTAIN( "CERTAIN" );

///////////////////////////////////////////////////////////////////////////////////////////////////
edgifyHookBase::edgifyHookBase( QObject *parent ) :
    _Mybase( parent ),
    m_( QMutex::Recursive ),
    api_( nullptr ),
    requestTimeout_( 0 ),
    requestNumRetries_( 0 ),
    uart_( nullptr ),
    uartErrors_( 0 ),
    uartBaudRate_( 115200 ),
    haveResult_( false )
{
    LOG_DEBUG << "ctor";

    // move to application thread
    moveToThread( QCoreApplication::instance()->thread() );

    // setup api
    api_ = new edgifyapi( this );

    connect( api_, &edgifyapi::processDocument, this, &_Myt::onProcessDocument );
    connect( api_, &edgifyapi::processFile, this, &_Myt::onProcessFile );

    connect( this, &edgifyHookBase::apiSend, api_, &edgifyapi::send, Qt::QueuedConnection );

    // setup api headers
    loadWebHeaders();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
edgifyHookBase::~edgifyHookBase()
{
    LOG_DEBUG << "dtor";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void edgifyHookBase::onProcessDocument( const QJsonDocument& request, int status, const QJsonDocument& response )
{
    Q_UNUSED( request );

    QMutexLocker guard( &m_ );

    if (( 200 != status ) || ( !response.isObject() ))
    {
        LOG_WARN << "bad response";
        return;
    }

    const QJsonObject obj( response.object() );
    const QJsonObject pred( obj[JSON_PREDICTION].toObject() );

    if ( !pred.contains( JSON_PREDICTIONS ) )
    {
        LOG_WARN << "response does not contain predictions";
        return;
    }

    const QString pred_uuid( pred[JSON_UUID].toString() );

    const QDateTime pred_at( QDateTime::fromString( pred[JSON_PREDICTED_AT].toString(), Qt::ISODate ) );
    const double pred_time( pred[JSON_DURATION].toDouble() );

    // show item predictions
    const QJsonArray preds( pred[JSON_PREDICTIONS].toArray() );

    if ( preds.isEmpty() )
    {
        LOG_WARN << "empty predictions";
        return;
    }

    // check certain flag
    bool certain( false );

    if (( pred.contains( JSON_CERTAIN ) ) && ( pred[JSON_CERTAIN].isBool() ))
    {
        certain = pred[JSON_CERTAIN].toBool();

        if ( certain )
            LOG_DEBUG << "CERTAIN flag is set";
    }

    LOG_DEBUG << "===== results " << qPrintable( pred_uuid ) << " (" << pred_time << " ms) =====";

    QStringList products;
    QStringList probs;

    int rank( 1 );

    foreach ( const QJsonValue& v, preds )
        if ( v.isObject() )
        {
            const QJsonObject p( v.toObject() );

            if ( !p.contains( JSON_DATA ) )
                continue;

            const QJsonArray data( p[JSON_DATA].toArray() );

            // pair of ("product", "confidence")
            if ( 2 == data.size() )
            {
                const QString product( data[0].toString() );
                const QString confidenceStr( data[1].toString() );

                const double confidence( confidenceStr.toDouble() );

                // check for empty or special prediction
                if ( 1 == preds.size() )
                {
                    if (( UNKNOWN == product ) && ( 1.0 <= confidence ))
                    {
                        LOG_INFO << "** LOW SCORE **";
                        sendPredictResults( LowScore );
                        return;
                    }
                    else if (( BACKGROUND == product ) && ( 1.0 <= confidence ))
                    {
                        LOG_INFO << "** BACKGROUND **";
                        sendPredictResults( Background );
                        return;
                    }
                }

                static const QString itemFormat( "%1 %2 %3" );

                LOG_DEBUG << qPrintable( itemFormat
                    .arg( rank++, 3 )
                    .arg( confidence, 6 )
                    .arg( product, 32 ) );

                products.append( product );

                if ( certain )
                    probs.append( CERTAIN );
                else
                    probs.append( QString::number( confidence ) );
            }
        }

    // save
    const QString filename( savePrediction( pred ) );

    // send result to service
    sendPredictResults( Items, filename, products, probs );
	
	haveResult_ = true;

    LOG_TRACE << "done";
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void edgifyHookBase::onProcessFile( const QJsonDocument& request, int status, const QString& filenameCached, const QString& filename )
{
    Q_UNUSED( request )
    Q_UNUSED( filenameCached )

    LOG_DEBUG << "process file '" << qPrintable( filename ) << "' " << status;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void edgifyHookBase::enableSerialComms()
{
    // setup UART wrapper
    if (( uartFile_.length() ) && ( 0 < uartBaudRate_ ))
    {
        uart_ = new edgifyuart( uartFile_, uartBaudRate_, this );

        connect( uart_, &edgifyuart::processDocument, this, &_Myt::onProcessDocument );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void edgifyHookBase::predict()
{
    LOG_DEBUG << "requesting edgify prediction...";

    bool pred( false );

    if ( uart_ )
    {
        if ( uart_->valid() )
        {
            LOG_TRACE << "predict UART...";

            uart_->predict();
            pred = true;
        }
        else if ( 0 == (++uartErrors_ % 3) )
        {
            LOG_ERROR << "attempting to reset UART...";
            uart_->forceReset();

            uart_->predict();
            pred = true;
        }
        else
        {
            LOG_WARN << "UART not valid";
        }
    }

    if ( !pred )
    {
        const QUrl url( formatEndpointUrl( predictionUrl_ ) );

        emit apiSend( url, QJsonDocument(), requestTimeout_, requestNumRetries_ );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void edgifyHookBase::updateTruth( const QString& label, const QString& source )
{
    if ( label.isEmpty() )
    {
        LOG_WARN << "empty label";
        return;
    }
    else if ( !haveResult_ )
    {
        LOG_WARN << "no result";
        return;
    }

    QJsonObject groundTruth;
    groundTruth[JSON_LABEL] = label;
    groundTruth[JSON_SOURCE] = source;
    groundTruth[JSON_PREDICTION] = preds_.back();

    QJsonObject obj;
    obj[JSON_SOURCE] = QHostInfo::localHostName();
    obj[JSON_GROUND_TRUTH] = groundTruth;

    LOG_DEBUG << "updating edgify ground truth " << qPrintable( label ) << " from '" << qPrintable( source ) << "'...";

    const QJsonDocument doc( obj );

    if (( uart_ ) && ( uart_->valid() ))
        uart_->setGroundTruth( doc );
    else
    {
        const QUrl url( formatEndpointUrl( setGroundTruthUrl_ ) );

        emit apiSend( url, doc, requestTimeout_, requestNumRetries_ );
    }

    haveResult_ = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void edgifyHookBase::loadWebHeaders()
{
    QSettings s( WEB_HEADERS_FILE, QSettings::IniFormat );

    edgifyapi::headersMap headers;

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
QUrl edgifyHookBase::formatEndpointUrl( const QString& url ) const
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

///////////////////////////////////////////////////////////////////////////////////////////////////
QString edgifyHookBase::savePrediction( const QJsonObject& obj )
{
    QString result;

    // remove oldest prediction
    if ( NUM_SAVED_PREDS <= preds_.size() )
    {
        const QJsonObject image( ( preds_.first() )[JSON_IMAGE].toObject() );

        const QString uuid( image[JSON_UUID].toString() );

        if (( tempDir_.isValid() ) && ( uuid.length() ))
        {
#if QT_VERSION_CHECK( 5, 9, 0 ) <= QT_VERSION
            const QString filename( tempDir_.filePath( uuid ) );
#else
            const QString filename( tempDir_.path() + QDir::separator() + uuid );
#endif

            LOG_DEBUG << "remove prediction image " << qPrintable( filename );

            QFile::remove( filename );
        }

        preds_.pop_front();
    }

    // save prediction
    const QJsonObject image( obj[JSON_IMAGE].toObject() );

    const QString uuid( image[JSON_UUID].toString() );
    const QByteArray a( QByteArray::fromBase64( image[JSON_IMAGE].toString().toLatin1() ) );

    if (( tempDir_.isValid() ) && ( uuid.length() ) && ( a.size() ))
    {
#if QT_VERSION_CHECK( 5, 9, 0 ) <= QT_VERSION
        const QString filename( tempDir_.filePath( uuid ) );
#else
        const QString filename( tempDir_.path() + QDir::separator() + uuid );
#endif

        LOG_DEBUG << "saving prediction image to " << qPrintable( filename );

        QFile f( filename );

        if ( f.open( QFile::WriteOnly ) )
        {
            f.write( a );
            f.close();

            // give filename to caller
            result = filename;
        }
    }

    preds_.push_back( obj );

    return result;
}
