/**
 * @file testobject.h
 * @brief Class that tests message center functionality.
 *
 * @copyright Copyright (c) 2021, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#ifndef TESTOBJECT_H
#define TESTOBJECT_H

#include <messagecenterclient.h>

#include <QMap>
#include <QPair>

class Event;

///////////////////////////////////////////////////////////////////////////////////////////////////
class TestObject : public MessageCenterClient
{
    Q_OBJECT

    using _Myt = TestObject;
    using _Mybase = MessageCenterClient;

public:

    TestObject( uint device );

    void reg();
    void unreg();

    void enableSelf();
    void disableSelf();

    void runTest( uint sendDevice ) const;

    uint numEvents() const;
    void clear();

protected:

    using EventType = QPair<uint, uint>;

    DeviceType device_;

    QMap<EventType, bool> events_;

    virtual void onProcessEvent( const Event *e, bool& destroy ) override;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // TESTOBJECT_H
