/**
 * @file testobject.cpp
 *
 * @copyright Copyright (c) 2021, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#include "testobject.h"

#include <event.h>

static const QString DBUS_SERVICE( "com.hobartcorp.Test" );
static const QString DBUS_OBJECT_PATH( "/TestObject%1" );

///////////////////////////////////////////////////////////////////////////////////////////////////
TestObject::TestObject( uint device ) :
    _Mybase( DBUS_SERVICE, DBUS_OBJECT_PATH.arg( device ) ),
    device_( (DeviceType) device )
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void TestObject::reg()
{
    registerEvent( device_, 1 );
    registerEvent( device_, 2 );
    registerEvent( device_, 3 );
    registerEvent( device_, 4 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void TestObject::unreg()
{
    unregisterEvent( device_, 1 );
    unregisterEvent( device_, 2 );
    unregisterEvent( device_, 3 );
    unregisterEvent( device_, 4 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void TestObject::enableSelf()
{
    setEnableSelfProcessing( true );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void TestObject::disableSelf()
{
    setEnableSelfProcessing( false );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void TestObject::runTest( uint sendDevice ) const
{
    // self
    post( device_, 1 );
    send( service(), objectPath(), device_, 2 );

    // other
    post( (DeviceType) sendDevice, 3 );
    send( DBUS_SERVICE, DBUS_OBJECT_PATH.arg( sendDevice ), (DeviceType) sendDevice, 4 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
uint TestObject::numEvents() const
{
    return events_.size();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void TestObject::clear()
{
    events_.clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void TestObject::onProcessEvent( const Event *e, bool& destroy )
{
    Q_UNUSED( destroy )

    const EventType et( (uint) e->device(), e->event() );
    events_[et] = true;
}
