/**
 * @file abstractrequest.cpp
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#include "abstractrequest.h"
#include "scaleconfig.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
abstractRequest::abstractRequest( QObject *parent ) :
    _Mybase( parent ),
    request_( scaleConfig::instance()->createRequestInfo() )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
abstractRequest::abstractRequest( const QString& requestUuid, QObject *parent ) :
    _Mybase( parent ),
    request_( scaleConfig::instance()->createRequestInfo( requestUuid ) )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
abstractRequest::~abstractRequest()
{
}
