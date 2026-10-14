/**
 * @file abstractrequest.cpp
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#include "abstractevent.h"
#include "scaleconfig.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
abstractEvent::abstractEvent( QObject *parent ) :
    _Mybase( parent ),
    event_( scaleConfig::instance()->createEventInfo() )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
abstractEvent::~abstractEvent()
{
}
