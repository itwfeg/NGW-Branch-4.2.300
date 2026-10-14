/**
 * @file transformresourcecpp
 *
 * @copyright Copyright (c) 2021, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#include "transformresource.h"

#include <databaseeventtype.h>
#include <databasejson.h>
#include <evoltags.h>
#include <fileresourceevent.h>
#include <jsondocumentevent.h>
#include <resourceeventtype.h>

#include <QJsonDocument>
#include <QJsonObject>

///////////////////////////////////////////////////////////////////////////////////////////////////

struct TransformResourceMap
{
    DatabaseEventType db;
    ResourceEventType res;
};

static const TransformResourceMap TRANSFORM_RESOURCE_MAP[] = {
    {DatabaseEventType::Read,           ResourceEventType::Read},
    {DatabaseEventType::ReadAll,        ResourceEventType::ReadAll},
    {DatabaseEventType::Write,          ResourceEventType::Write},
    {DatabaseEventType::Delete,         ResourceEventType::Delete},
    {DatabaseEventType::DeleteAll,      ResourceEventType::DeleteAll},

    {DatabaseEventType::Invalid,        ResourceEventType::Invalid}
};

///////////////////////////////////////////////////////////////////////////////////////////////////

struct TransformResourceTypeMap
{
    const char *tag;
    uint event;
    uint type;
    uint subtype;
};

static const TransformResourceTypeMap TRANSFORM_RESOURCE_TYPE_MAP[] = {
    {EVOL_TAG_READ_ROM_GRAPHICS,            (uint) ResourceEventType::Read,         (uint) ResourceType::Graphic,       (uint) GraphicSubtype::ROM},
    {EVOL_TAG_WRITE_ROM_GRAPHICS,           (uint) ResourceEventType::Write,        (uint) ResourceType::Graphic,       (uint) GraphicSubtype::ROM},

    {EVOL_TAG_READ_PRODUCT_GRAPHICS,        (uint) ResourceEventType::Read,         (uint) ResourceType::Graphic,       (uint) GraphicSubtype::Product},
    {EVOL_TAG_WRITE_PRODUCT_GRAPHICS,       (uint) ResourceEventType::Write,        (uint) ResourceType::Graphic,       (uint) GraphicSubtype::Product},
    {EVOL_TAG_DEL_PRODUCT_GRAPHIC,          (uint) ResourceEventType::Delete,       (uint) ResourceType::Graphic,       (uint) GraphicSubtype::Product},
    {EVOL_TAG_DEL_ALL_PRODUCT_GRAPHICS,     (uint) ResourceEventType::DeleteAll,    (uint) ResourceType::Graphic,       (uint) GraphicSubtype::Product},

    {EVOL_TAG_READ_CATEGORY_GRAPHICS,       (uint) ResourceEventType::Read,         (uint) ResourceType::Graphic,       (uint) GraphicSubtype::Category},
    {EVOL_TAG_WRITE_CATEGORY_GRAPHICS,      (uint) ResourceEventType::Write,        (uint) ResourceType::Graphic,       (uint) GraphicSubtype::Category},
    {EVOL_TAG_DEL_CATEGORY_GRAPHIC,         (uint) ResourceEventType::Delete,       (uint) ResourceType::Graphic,       (uint) GraphicSubtype::Category},
    {EVOL_TAG_DEL_ALL_CATEGORY_GRAPHICS,    (uint) ResourceEventType::DeleteAll,    (uint) ResourceType::Graphic,       (uint) GraphicSubtype::Category},
    {EVOL_TAG_READ_CATEGORY_GRAPHICS,       (uint) ResourceEventType::ReadAll,      (uint) ResourceType::Graphic,       (uint) GraphicSubtype::Category},

    {EVOL_TAG_READ_SPECIALS_GRAPHICS,       (uint) ResourceEventType::Read,         (uint) ResourceType::Graphic,       (uint) GraphicSubtype::Specials},
    {EVOL_TAG_WRITE_SPECIALS_GRAPHICS,      (uint) ResourceEventType::Write,        (uint) ResourceType::Graphic,       (uint) GraphicSubtype::Specials},
    {EVOL_TAG_DEL_SPECIALS_GRAPHIC,         (uint) ResourceEventType::Delete,       (uint) ResourceType::Graphic,       (uint) GraphicSubtype::Specials},
    {EVOL_TAG_DEL_ALL_SPECIALS_GRAPHICS,    (uint) ResourceEventType::DeleteAll,    (uint) ResourceType::Graphic,       (uint) GraphicSubtype::Specials},
    {EVOL_TAG_READ_SPECIALS_GRAPHICS,       (uint) ResourceEventType::ReadAll,      (uint) ResourceType::Graphic,       (uint) GraphicSubtype::Specials},

    {EVOL_TAG_READ_STREAMING_GRAPHICS,      (uint) ResourceEventType::Read,         (uint) ResourceType::Graphic,       (uint) GraphicSubtype::Streaming},
    {EVOL_TAG_WRITE_STREAMING_GRAPHICS,     (uint) ResourceEventType::Write,        (uint) ResourceType::Graphic,       (uint) GraphicSubtype::Streaming},
    {EVOL_TAG_DEL_STREAMING_GRAPHIC,        (uint) ResourceEventType::Delete,       (uint) ResourceType::Graphic,       (uint) GraphicSubtype::Streaming},
    {EVOL_TAG_DEL_ALL_STREAMING_GRAPHICS,   (uint) ResourceEventType::DeleteAll,    (uint) ResourceType::Graphic,       (uint) GraphicSubtype::Streaming},
    {EVOL_TAG_READ_STREAMING_GRAPHICS,      (uint) ResourceEventType::ReadAll,      (uint) ResourceType::Graphic,       (uint) GraphicSubtype::Streaming},

    {EVOL_TAG_READ_USER_GRAPHICS,           (uint) ResourceEventType::Read,         (uint) ResourceType::Graphic,       (uint) GraphicSubtype::User},
    {EVOL_TAG_WRITE_USER_GRAPHICS,          (uint) ResourceEventType::Write,        (uint) ResourceType::Graphic,       (uint) GraphicSubtype::User},
    {EVOL_TAG_DEL_USER_GRAPHIC,             (uint) ResourceEventType::Delete,       (uint) ResourceType::Graphic,       (uint) GraphicSubtype::User},
    {EVOL_TAG_DEL_ALL_USER_GRAPHICS,        (uint) ResourceEventType::DeleteAll,    (uint) ResourceType::Graphic,       (uint) GraphicSubtype::User},
    {EVOL_TAG_READ_USER_GRAPHICS,           (uint) ResourceEventType::ReadAll,      (uint) ResourceType::Graphic,       (uint) GraphicSubtype::User},

    {EVOL_TAG_READ_USER_FONTS,              (uint) ResourceEventType::Read,         (uint) ResourceType::Font,          (uint) FontSubtype::User},
    {EVOL_TAG_WRITE_USER_FONTS,             (uint) ResourceEventType::Write,        (uint) ResourceType::Font,          (uint) FontSubtype::User},
    {EVOL_TAG_DEL_USER_FONT,                (uint) ResourceEventType::Delete,       (uint) ResourceType::Font,          (uint) FontSubtype::User},
    {EVOL_TAG_DEL_ALL_USER_FONTS,           (uint) ResourceEventType::DeleteAll,    (uint) ResourceType::Font,          (uint) FontSubtype::User},
    {EVOL_TAG_READ_USER_FONTS,              (uint) ResourceEventType::ReadAll,      (uint) ResourceType::Font,          (uint) FontSubtype::User},

    {EVOL_TAG_READ_SOFTKEY_GRAPHICS,        (uint) ResourceEventType::Read,         (uint) ResourceType::Graphic,       (uint) GraphicSubtype::Softkey},
    {EVOL_TAG_WRITE_SOFTKEY_GRAPHICS,       (uint) ResourceEventType::Write,        (uint) ResourceType::Graphic,       (uint) GraphicSubtype::Softkey},
    {EVOL_TAG_DEL_SOFTKEY_GRAPHIC,          (uint) ResourceEventType::Delete,       (uint) ResourceType::Graphic,       (uint) GraphicSubtype::Softkey},
    {EVOL_TAG_DEL_ALL_SOFTKEY_GRAPHICS,     (uint) ResourceEventType::DeleteAll,    (uint) ResourceType::Graphic,       (uint) GraphicSubtype::Softkey},
    {EVOL_TAG_READ_SOFTKEY_GRAPHICS,        (uint) ResourceEventType::ReadAll,      (uint) ResourceType::Graphic,       (uint) GraphicSubtype::Softkey},

    {EVOL_TAG_READ_PLANOGRAM_GRAPHICS,      (uint) ResourceEventType::Read,         (uint) ResourceType::Graphic,       (uint) GraphicSubtype::Planogram},
    {EVOL_TAG_WRITE_PLANOGRAM_GRAPHICS,     (uint) ResourceEventType::Write,        (uint) ResourceType::Graphic,       (uint) GraphicSubtype::Planogram},
    {EVOL_TAG_DEL_PLANOGRAM_GRAPHICS,       (uint) ResourceEventType::Delete,       (uint) ResourceType::Graphic,       (uint) GraphicSubtype::Planogram},
    {EVOL_TAG_DEL_ALL_PLANOGRAM_GRAPHICS,   (uint) ResourceEventType::DeleteAll,    (uint) ResourceType::Graphic,       (uint) GraphicSubtype::Planogram},
    {EVOL_TAG_READ_PLANOGRAM_GRAPHICS,      (uint) ResourceEventType::ReadAll,      (uint) ResourceType::Graphic,       (uint) GraphicSubtype::Planogram},

    {NULL,                                  0,                                      0,                                  0},
};

///////////////////////////////////////////////////////////////////////////////////////////////////
Event *TransformResource::convertFromEvent( const FileResourceEvent *old, QString& tag )
{
    if ( !old )
        return nullptr;
    else if (( DeviceType::GraphicResource != old->device() ) && ( DeviceType::FontResource != old->device() ))
        return nullptr;

    // locate file type/subtype from tag
    for ( uint i( 0 ); ; ++i )
    {
        const TransformResourceTypeMap *xform( &TRANSFORM_RESOURCE_TYPE_MAP[i] );

        if ( !xform->tag )
            break;
        else if ( xform->event != old->event() )
            continue;
        else if (( xform->type != old->type() ) || ( xform->subtype != old->subtype() ))
            continue;

        // give tag to caller
        tag = xform->tag;

        // find event
        uint event( 0 );

        for ( uint i( 0 ); ; ++i )
        {
            if ( DatabaseEventType::Invalid == TRANSFORM_RESOURCE_MAP[i].db )
                break;
            else if ( TRANSFORM_RESOURCE_MAP[i].res == (ResourceEventType) old->event() )
            {
                event = (uint) TRANSFORM_RESOURCE_MAP[i].db;
                break;
            }
        }

        // convert event
        JsonDocumentEvent *result( nullptr );

        if (( DatabaseEventType::ReadAll == (DatabaseEventType) event ) ||
            ( DatabaseEventType::DeleteAll == (DatabaseEventType) event ))
        {
            result = new JsonDocumentEvent( old->device(), event );
        }
        else if ( event )
        {
            QJsonObject obj;

            // attach filename if present
            if ( old->filename().length() )
                obj[JSON_RESOURCE_FILE_NAME] = old->filename();

            // attach data if present
            if ( old->data().length() )
            {
                const QByteArray a( old->data().toHex() );

                obj[JSON_RESOURCE_DATA] = a.constData();
            }

            result = new JsonDocumentEvent( old->device(), event, QJsonDocument( obj ) );
        }

        return result;
    }

    return nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
FileResourceEvent *TransformResource::convertToEvent( const Event *old, const QString& tag )
{
    if ( !old )
        return nullptr;
    else if (( DeviceType::GraphicResource != old->device() ) && ( DeviceType::FontResource != old->device() ))
        return nullptr;

    const JsonDocumentEvent *e( dynamic_cast<const JsonDocumentEvent*>( old ) );

    if ( !e )
        return nullptr;

    // find event
    uint event( 0 );

    for ( uint i( 0 ); ; ++i )
    {
        if ( DatabaseEventType::Invalid == TRANSFORM_RESOURCE_MAP[i].db )
            break;
        else if ( TRANSFORM_RESOURCE_MAP[i].db == (DatabaseEventType) e->event() )
        {
            event = (uint) TRANSFORM_RESOURCE_MAP[i].res;
            break;
        }
    }

    // locate file type/subtype from tag
    for ( uint i( 0 ); ; ++i )
    {
        const TransformResourceTypeMap *xform( &TRANSFORM_RESOURCE_TYPE_MAP[i] );

        if ( !xform->tag )
            break;
        else if ( tag != xform->tag )
            continue;

        FileResourceEvent *result( nullptr );

        // convert event
        if (( ResourceEventType::ReadAll == (ResourceEventType) event ) ||
            ( ResourceEventType::DeleteAll == (ResourceEventType) event ))
        {
            // create event
            result = new FileResourceEvent( old->device(), event, xform->type, xform->subtype );
        }
        else if ( event )
        {
            // retrieve json object and make sure it has filename
            const QJsonDocument doc( e->document() );

            QJsonObject obj;

            if ( doc.isObject() )
                obj = doc.object();

            if ( obj.contains( JSON_RESOURCE_FILE_NAME ) )
            {
                // create event
                result = new FileResourceEvent( old->device(), event, xform->type, xform->subtype );

                // set file name if exists
                if ( obj.contains( JSON_RESOURCE_FILE_NAME ) )
                    result->setFilename( obj[JSON_RESOURCE_FILE_NAME].toString() );

                // attach file data if exists
                if ( obj.contains( JSON_RESOURCE_DATA ) )
                    result->setData( QByteArray::fromHex( obj[JSON_RESOURCE_DATA].toString().toLatin1() ) );
            }
        }

        return result;
    }

    return nullptr;
}
