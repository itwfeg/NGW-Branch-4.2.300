/**
 * @file eventfactory.cpp
 *
 * @copyright Copyright (c) 2021, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#include "databaseeventtype.h"
#include "devicetype.h"
#include "eventfactory.h"
#include "fileresourceevent.h"
#include "jsondocumentevent.h"

#include <QByteArray>

///////////////////////////////////////////////////////////////////////////////////////////////////
Event *EventFactory::create( const QByteArray& a )
{
    uint v;

    // no data (or not enough data)
    if ( a.size() < 20 )
        return nullptr;

    // extract device and event
    const char *data( a.constData() );

    memcpy( &v, &data[0], sizeof(uint) );
    const DeviceType device( (DeviceType) v );

    memcpy( &v, &data[sizeof(uint)], sizeof(uint) );
    const uint event( v );

    // validate
    if (( DeviceType::Invalid == device ) || ( !event ))
        return nullptr;

    // handle device
    Event *e( nullptr );

    switch ( device )
    {
    case DeviceType::GraphicResource:
    case DeviceType::FontResource:

        e = new FileResourceEvent( a );
        break;

    case DeviceType::DepartmentDatabase:

    case DeviceType::ProductDatabase:
    case DeviceType::OperatorDatabase:
    case DeviceType::OperatorNotesDatabase:
    case DeviceType::CategoryDatabase:

    case DeviceType::ProductTextDatabase:
    case DeviceType::NutritionDatabase:

        e = new JsonDocumentEvent( a );
        break;

    default:

        e = new Event( a );
        break;

    }

    return e;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void EventFactory::destroy( const Event *doomed )
{
    if ( doomed )
        delete doomed;
}
