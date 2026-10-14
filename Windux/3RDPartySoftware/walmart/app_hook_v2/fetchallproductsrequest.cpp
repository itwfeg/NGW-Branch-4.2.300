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
    businessUnitType_( 1 ),
    productCount_( -1 )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
fetchAllProductsRequest::fetchAllProductsRequest( const QString& requestUuid, QObject *parent ) :
    _Mybase( requestUuid, parent ),
    businessUnitType_( 1 ),
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

    // as per Walmart Varun Mannuru, add MDM Location Model when last PLU fetch is not valid
    if (( lastFetch_.isEmpty() ) || ( lastFetch_.isNull() ))
    {
        if ( !obj.contains( JSON_SCALE ) )
            LOG_WARN << "scale missing from object";
        else
        {
            QJsonObject mdmLocationModel;
            mdmLocationModel[JSON_DIVISION_NBR] = businessUnitType_;

            QJsonObject scale( obj[JSON_SCALE].toObject() );
            scale[JSON_MDM_LOCATION_MODEL] = mdmLocationModel;

            // update scale object with MDM info
            obj[JSON_SCALE] = scale;
        }
    }

    return QJsonDocument( obj );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QJsonObject fetchAllProductsRequest::plu() const
{
    static const QJsonValue nullVal( QJsonValue::Null );

    const scaleBlockedProducts blockedProducts;

    QJsonObject obj;

    if (( !lastFetch_.isEmpty() ) && ( !lastFetch_.isNull() ))
        obj[JSON_LAST_FETCH_PLU_STAMP] = lastFetch_;
    else
        obj[JSON_LAST_FETCH_PLU_STAMP] = nullVal;

    if ( 0 <= productCount_ )
        obj[JSON_PLU_RECORD_COUNT] = productCount_;

    obj[JSON_BLOCKED_PLUS] = blockedProducts.toJsonArray();

    return obj;
}
