/**
 * @file ooapi.cpp
 *
 * @copyright Copyright (c) 2021, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */


#include "common.h"
#include "ooapi.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSettings>
#include <QUrl>
#include <QUrlQuery>

#define SET_ENDPOINT( e, s ) \
    endpoints_[e] = s; \
    endpointNames_[e] = #e ;

///////////////////////////////////////////////////////////////////////////////////////////////////
OnlineOrderingAPI::OnlineOrderingAPI( QObject *parent ) :
    _Mybase( parent )
{
    loadEndpoints();

    connect( this, &_Myt::processDocument, this, &_Myt::onProcessDocument );
    connect( this, &_Myt::processDocumentJson, this, &_Myt::onProcessDocumentJson );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
OnlineOrderingAPI::~OnlineOrderingAPI()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineOrderingAPI::deleteAll( Endpoint e )
{
    qDebug() << "===== " + endpointNames_[e] + " =====";

    const QUuid uuid( QUuid::createUuid() );

    pendingRequests_[uuid] = e;
    remove( uuid, endpointUrl( e ), REQUEST_TIMEOUT, REQUEST_RETRIES );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineOrderingAPI::deleteOne( Endpoint e, const QString& key0, const QString& key1, const QString& key2 )
{
    qDebug() << "===== " + endpointNames_[e] + " =====";

    const QUuid uuid( QUuid::createUuid() );

    pendingRequests_[uuid] = e;
    remove( uuid, endpointUrl( e, key0, key1, key2 ), REQUEST_TIMEOUT, REQUEST_RETRIES );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineOrderingAPI::readAll( Endpoint e )
{
    qDebug() << "===== " + endpointNames_[e] + " =====";

    const QUuid uuid( QUuid::createUuid() );

    pendingRequests_[uuid] = e;
    send( uuid, endpointUrl( e ), REQUEST_TIMEOUT, REQUEST_RETRIES );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineOrderingAPI::readOne( Endpoint e, const QString& key0, const QString& key1, const QString& key2 )
{
    qDebug() << "===== " + endpointNames_[e] + " =====";

    const QUuid uuid( QUuid::createUuid() );

    // save off filename for later use
    if ( GET_CATEGORY_IMAGE == e )
        imageRequests_[uuid] = key0;
    else if ( GET_PRODUCT_IMAGE == e )
        imageRequests_[uuid] = key0;

    pendingRequests_[uuid] = e;
    send( uuid, endpointUrl( e, key0, key1, key2 ), REQUEST_TIMEOUT, REQUEST_RETRIES );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineOrderingAPI::writeOne( Endpoint e, const QJsonObject& obj, bool isRequest )
{
    qDebug() << "===== " + endpointNames_[e] + " =====";

    const QUuid uuid( QUuid::createUuid() );

    pendingRequests_[uuid] = e;

    if ( isRequest )
        send( uuid, endpointUrl( e ), QJsonDocument( obj ), REQUEST_TIMEOUT, REQUEST_RETRIES );
    else
        upload( uuid, endpointUrl( e ), QJsonDocument( obj ), REQUEST_TIMEOUT, REQUEST_RETRIES );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineOrderingAPI::onProcessDocument( const QUuid& uuid, const QByteArray& request, const QString& requestType, int status, const QByteArray& response, const QString& responseType )
{
    Q_UNUSED( request )
    Q_UNUSED( requestType )

    if ( !pendingRequests_.contains( uuid ) )
        return;

    // retrieve endpoint
    const Endpoint type( pendingRequests_[uuid] );

    pendingRequests_.remove( uuid );

    // retrieve filename
    QString filename;

    if ( imageRequests_.contains( uuid ) )
    {
        filename = imageRequests_[uuid];
        imageRequests_.remove( uuid );
    }

    // validate status
    if ( 200 != status )
    {
        LOG_WARN << "bad response " << qPrintable( uuid.toString() ) << " " << status;
        return;
    }

    qDebug() << "process response" << status << responseType << response.size() << "...";

    switch ( type )
    {
    case GET_CATEGORY_IMAGE:
        emit categoryImageReceived( filename, response );
        break;
    case GET_PRODUCT_IMAGE:
        emit productImageReceived( filename, response );
        break;
    default:
        LOG_WARN << "unhandled endpoint type " << type;
        break;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineOrderingAPI::onProcessDocumentJson( const QUuid& uuid, const QByteArray& request, const QString& requestType, int status, const QJsonDocument& response )
{
    Q_UNUSED( request )
    Q_UNUSED( requestType )

    if ( !pendingRequests_.contains( uuid ) )
        return;

    // retrieve endpoint
    const Endpoint type( pendingRequests_[uuid] );

    pendingRequests_.remove( uuid );

    // validate status
    if ( 200 != status )
    {
        LOG_WARN << "bad response " << qPrintable( uuid.toString() ) << " " << status;
        return;
    }

    qDebug() << "process response" << status << "JSON document ...";

    switch ( type )
    {
    case GET_CATEGORY:
    case GET_ALL_CATEGORIES:
        parseRecord( PUT_CATEGORY, response );
        break;
    case GET_ALL_CATEGORY_IMAGES:
        parseImageList( GET_CATEGORY_IMAGE, response );
        break;
    case GET_DEPARTMENT:
    case GET_ALL_DEPARTMENTS:
        parseRecord( PUT_DEPARTMENT, response );
        break;
    case GET_OPERATOR:
    case GET_ALL_OPERATORS:
        parseRecord( PUT_OPERATOR, response );
        break;
    case GET_PRODUCT:
    case GET_ALL_PRODUCTS:
        parseRecord( PUT_PRODUCT, response );
        break;
    case GET_ALL_PRODUCT_IMAGES:
        parseImageList( GET_PRODUCT_IMAGE, response );
        break;
    default:
        LOG_WARN << "unhandled endpoint type " << type;
        break;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineOrderingAPI::loadEndpoints()
{
    SET_ENDPOINT( DELETE_CATEGORY, "/api/v1/categories/{key0}/{key1}" )
    SET_ENDPOINT( DELETE_ALL_CATEGORIES, "/api/v1/categories/all" )
    SET_ENDPOINT( GET_CATEGORY, "/api/v1/categories/{key0}/{key1}" )
    SET_ENDPOINT( GET_ALL_CATEGORIES, "/api/v1/categories/all" )
    SET_ENDPOINT( PUT_CATEGORY, "/api/v1/categories" )

    SET_ENDPOINT( DELETE_CATEGORY_IMAGE, "/api/v1/images/categories/{key0}" )
    SET_ENDPOINT( DELETE_ALL_CATEGORY_IMAGES, "/api/v1/images/categories/all" )
    SET_ENDPOINT( GET_CATEGORY_IMAGE, "/api/v1/images/categories/{key0}" )
    SET_ENDPOINT( GET_ALL_CATEGORY_IMAGES, "/api/v1/images/categories/all" )
    SET_ENDPOINT( PUT_CATEGORY_IMAGE, "/api/v1/images/categories" )

    SET_ENDPOINT( DELETE_DEPARTMENT, "/api/v1/departments/{key0}" )
    SET_ENDPOINT( DELETE_ALL_DEPARTMENTS, "/api/v1/departments/all" )
    SET_ENDPOINT( GET_DEPARTMENT, "/api/v1/departments/{key0}" )
    SET_ENDPOINT( GET_ALL_DEPARTMENTS, "/api/v1/departments/all" )
    SET_ENDPOINT( PUT_DEPARTMENT, "/api/v1/departments" )

    SET_ENDPOINT( DELETE_OPERATOR, "/api/v1/operators/{key0}" )
    SET_ENDPOINT( DELETE_ALL_OPERATORS, "/api/v1/operators/all" )
    SET_ENDPOINT( GET_OPERATOR, "/api/v1/operators/{key0}" )
    SET_ENDPOINT( GET_ALL_OPERATORS, "/api/v1/operators/all" )
    SET_ENDPOINT( PUT_OPERATOR, "/api/v1/operators" )

    SET_ENDPOINT( DELETE_PRODUCT, "/api/v1/products/{key0}/{key1}" )
    SET_ENDPOINT( DELETE_ALL_PRODUCTS, "/api/v1/products/all" )
    SET_ENDPOINT( GET_PRODUCT, "/api/v1/products/{key0}/{key1}" )
    SET_ENDPOINT( GET_ALL_PRODUCTS, "/api/v1/products/all" )
    SET_ENDPOINT( PUT_PRODUCT, "/api/v1/products" )

    SET_ENDPOINT( DELETE_PRODUCT_IMAGE, "/api/v1/images/products/{key0}" )
    SET_ENDPOINT( DELETE_ALL_PRODUCT_IMAGES, "/api/v1/images/products/all" )
    SET_ENDPOINT( GET_PRODUCT_IMAGE, "/api/v1/images/products/{key0}" )
    SET_ENDPOINT( GET_ALL_PRODUCT_IMAGES, "/api/v1/images/products/all" )
    SET_ENDPOINT( PUT_PRODUCT_IMAGE, "/api/v1/images/products" )
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineOrderingAPI::parseRecord( Endpoint e, const QJsonDocument& doc )
{
    QJsonArray a;

    if ( doc.isObject() )
        a.append( doc.object() );
    else if ( doc.isArray() )
        a = doc.array();
    else
    {
        qWarning() << "bad response";
        return;
    }

    qDebug() << "process" << a.size() << "records...";

    // iterate operators
    for ( QJsonArray::const_iterator i( a.constBegin() ); i != a.constEnd(); ++i )
        if ( i->isObject() )
        {
            if ( PUT_CATEGORY == e )
                emit categoryReceived( i->toObject() );
            else if ( PUT_DEPARTMENT == e )
                emit departmentReceived( i->toObject() );
            else if ( PUT_OPERATOR == e )
                emit operatorReceived( i->toObject() );
            else if ( PUT_PRODUCT == e )
                emit productReceived( i->toObject() );
        }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OnlineOrderingAPI::parseImageList( Endpoint e, const QJsonDocument& doc )
{
    if ( !doc.isArray() )
    {
        qWarning() << "bad response";
        return;
    }

    const QJsonArray a( doc.array() );

    qDebug() << "process" << a.size() << "images...";

    // iterate images
    for ( QJsonArray::const_iterator i( a.constBegin() ); i != a.constEnd(); ++i )
        if ( i->isString() )
            readOne( e, i->toString() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QUrl OnlineOrderingAPI::endpointUrl( Endpoint e, const QString& key0, const QString& key1, const QString& key2 ) const
{
    if (( address_.isEmpty() ) || ( !endpoints_.contains( e ) ))
        return QString();

    QString s( address_ + endpoints_[e] );
    s.replace( "{key0}", QUrl::toPercentEncoding( key0 ) );
    s.replace( "{key1}", QUrl::toPercentEncoding( key1 ) );
    s.replace( "{key2}", QUrl::toPercentEncoding( key2 ) );

    return s;
}
