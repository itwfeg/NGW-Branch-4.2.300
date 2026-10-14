/**
 * @file main.cpp
 * @brief Application entry point.
 *
 * @copyright Copyright (c) 2021, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#include "event.h"
#include "eventfactory.h"
#include "testobject.h"

#include <cassert>

#include <QCoreApplication>
#include <QDateTime>
#include <QDBusConnection>
#include <QDBusError>
#include <QDebug>

static const QString DBUS_SERVICE( "com.hobartcorp.Test" );

static const int SLEEP_TIME( 500 );

///////////////////////////////////////////////////////////////////////////////////////////////////
void sleepFor( uint ms )
{
    const QDateTime until( QDateTime::currentDateTime().addMSecs( ms ) );

    do
    {
        QCoreApplication::processEvents( QEventLoop::AllEvents, 100 );

    } while ( QDateTime::currentDateTime() < until );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void testMessageCenter()
{
    TestObject obj1( 1 );
    TestObject obj2( 2 );

    // ---- //

    obj1.runTest( 2 );
    obj2.runTest( 1 );

    sleepFor( SLEEP_TIME );
    assert( 0 == obj1.numEvents() );
    assert( 0 == obj2.numEvents() );

    // ---- //

    obj1.unreg();
    obj2.unreg();
    obj1.runTest( 2 );
    obj2.runTest( 1 );

    sleepFor( SLEEP_TIME );
    assert( 0 == obj1.numEvents() );
    assert( 0 == obj2.numEvents() );

    // ---- //

    obj1.reg();
    obj2.reg();
    obj1.runTest( 2 );
    obj2.runTest( 1 );

    sleepFor( SLEEP_TIME );
    assert( 2 == obj1.numEvents() );
    assert( 2 == obj2.numEvents() );

    // ---- //

    obj1.clear();
    obj2.clear();

    sleepFor( SLEEP_TIME );
    assert( 0 == obj1.numEvents() );
    assert( 0 == obj2.numEvents() );

    // ---- //

    obj1.enableSelf();
    obj2.enableSelf();
    obj1.runTest( 2 );
    obj2.runTest( 1 );

    sleepFor( SLEEP_TIME );
    assert( 4 == obj1.numEvents() );
    assert( 4 == obj2.numEvents() );

    // ---- //

    obj1.clear();
    obj2.clear();

    sleepFor( SLEEP_TIME );
    assert( 0 == obj1.numEvents() );
    assert( 0 == obj2.numEvents() );

    // ---- //

    obj1.disableSelf();
    obj2.disableSelf();
    obj1.runTest( 2 );
    obj2.runTest( 1 );

    sleepFor( SLEEP_TIME );
    assert( 2 == obj1.numEvents() );
    assert( 2 == obj2.numEvents() );

    // ---- //

    obj1.clear();
    obj2.clear();

    sleepFor( SLEEP_TIME );
    assert( 0 == obj1.numEvents() );
    assert( 0 == obj2.numEvents() );

    // ---- //

    obj1.unreg();
    obj2.unreg();
    obj1.runTest( 2 );
    obj2.runTest( 1 );

    sleepFor( SLEEP_TIME );
    assert( 0 == obj1.numEvents() );
    assert( 0 == obj2.numEvents() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void testEvents()
{
    static const QVector<uint> DATA_LEN = {4, 0, 0, 8};

    static const QString service( "com.hobart.ServiceName" );
    static const QString objectPath( "/Some/Object/Path" );

    Event e( DATA_LEN );

    assert( !e.isValid() );

    e.setDevice( (DeviceType) 100 );
    e.setEvent( 200 );
    e.setSenderService( service );
    e.setSenderObjectPath( objectPath );

    assert( e.isValid() );
    assert( (DeviceType) 100 == e.device() );
    assert( 200 == e.event() );
    assert( service == e.senderService() );
    assert( objectPath == e.senderObjectPath() );

    e.setData<uint>( 0, 42 );
    e.setData<QString>( 1, "42" );
    e.setData<double>( 3, 4.2 );

    assert( 42 == e.data<uint>( 0 ) );
    assert( "42" == e.data<QString>( 1 ) );
    assert( e.data<QString>( 2 ).isEmpty() );
    assert( 4.2 == e.data<double>( 3 ) );

    const QByteArray a( e.serialize() );
    const uint len( a.length() );

    assert( 97 == len );

    Event *e1 = EventFactory::create( a );

    assert( e == (*e1) );

    EventFactory::destroy( e1 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int main( int argc, char *argv[] )
{
    QCoreApplication a( argc, argv );

    // register instance with DBUS
    QDBusConnection connection( QDBusConnection::sessionBus() );

    if ( !connection.registerService( DBUS_SERVICE ) )
    {
        const QDBusError err( connection.lastError() );
        qWarning() << "failed to register service" << DBUS_SERVICE << err.type() << err.name() << err.message();
    }

    // test
    testEvents();
    testMessageCenter();

    qWarning() << "All tests passed!";

    return a.exec();
}
