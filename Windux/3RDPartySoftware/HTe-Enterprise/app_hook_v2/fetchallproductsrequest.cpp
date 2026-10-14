/**
 * @file fetchallproductsrequest.cpp
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#include "common.h"
#include "fetchallproductsrequest.h"
#include "scaleblockedplus.h"
#include "scaleconfig.h"
#include "stringsjson.h"

#include <QJsonArray>
#include <QJsonDocument>

///////////////////////////////////////////////////////////////////////////////////////////////////
fetchAllProductsRequest::fetchAllProductsRequest( QObject *parent ) :
    _Mybase( parent ),
    productCount_( -1 )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
fetchAllProductsRequest::~fetchAllProductsRequest()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QJsonDocument fetchAllProductsRequest::toJsonDoc() const
{
    QJsonObject obj( request_ );
    obj[JSON_PLU] = plu();

    return QJsonDocument( obj );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QJsonObject fetchAllProductsRequest::plu() const
{
    static const QJsonValue nullVal( QJsonValue::Null );

    const scaleBlockedProducts blockedProducts;

    QJsonObject obj;

    if ( lastFetch_.isValid() )
        obj[JSON_LAST_FETCH_PLU_STAMP] = scaleConfig::dateTimeToString( lastFetch_ );
    else
        obj[JSON_LAST_FETCH_PLU_STAMP] = nullVal;

    if ( 0 <= productCount_ )
        obj[JSON_PLU_RECORD_COUNT] = productCount_;

    obj[JSON_BLOCKED_PLUS] = blockedProducts.toJsonArray();

    return obj;
}
