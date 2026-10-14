/**
 * @file transformprodtext.cpp
 *
 * @copyright Copyright (c) 2021, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#include "transformprodtext.h"

#include <databaseeventtype.h>
#include <databasejson.h>
#include <evoltags.h>
#include <jsondocumentevent.h>

#include <QJsonDocument>
#include <QJsonObject>

///////////////////////////////////////////////////////////////////////////////////////////////////

struct TransformProductTextMap
{
    const char *tag;
    uint event;
    uint type;
};

static const TransformProductTextMap TRANSFORM_PRODUCT_TEXT_MAP[] = {
    {EVOL_TAG_READ_EXP_TEXT1,           (uint) DatabaseEventType::Read,         (uint) ProductTextType::ExpandedText1},
    {EVOL_TAG_WRITE_EXP_TEXT1,          (uint) DatabaseEventType::Write,        (uint) ProductTextType::ExpandedText1},
    {EVOL_TAG_DEL_EXP_TEXT1,            (uint) DatabaseEventType::Delete,       (uint) ProductTextType::ExpandedText1},
    {EVOL_TAG_DEL_ALL_EXP_TEXT1,        (uint) DatabaseEventType::DeleteAll,    (uint) ProductTextType::ExpandedText1},

    {EVOL_TAG_READ_EXP_TEXT2,           (uint) DatabaseEventType::Read,         (uint) ProductTextType::ExpandedText2},
    {EVOL_TAG_WRITE_EXP_TEXT2,          (uint) DatabaseEventType::Write,        (uint) ProductTextType::ExpandedText2},
    {EVOL_TAG_DEL_EXP_TEXT2,            (uint) DatabaseEventType::Delete,       (uint) ProductTextType::ExpandedText2},
    {EVOL_TAG_DEL_ALL_EXP_TEXT2,        (uint) DatabaseEventType::DeleteAll,    (uint) ProductTextType::ExpandedText2},

    {EVOL_TAG_READ_SPECIAL_MSGS,        (uint) DatabaseEventType::Read,         (uint) ProductTextType::SpecialMessage},
    {EVOL_TAG_WRITE_SPECIAL_MSGS,       (uint) DatabaseEventType::Write,        (uint) ProductTextType::SpecialMessage},

    {EVOL_TAG_READ_PRODUCT_NOTES,       (uint) DatabaseEventType::Read,         (uint) ProductTextType::ProductNote},
    {EVOL_TAG_WRITE_PRODUCT_NOTES,      (uint) DatabaseEventType::Write,        (uint) ProductTextType::ProductNote},
    {EVOL_TAG_DEL_PRODUCT_NOTE,         (uint) DatabaseEventType::Delete,       (uint) ProductTextType::ProductNote},
    {EVOL_TAG_DEL_ALL_PRODUCT_NOTES,    (uint) DatabaseEventType::DeleteAll,    (uint) ProductTextType::ProductNote},

    {EVOL_TAG_READ_MARQUEES,            (uint) DatabaseEventType::Read,         (uint) ProductTextType::Marquee},
    {EVOL_TAG_WRITE_MARQUEES,           (uint) DatabaseEventType::Write,        (uint) ProductTextType::Marquee},
    {EVOL_TAG_DEL_MARQUEE,              (uint) DatabaseEventType::Delete,       (uint) ProductTextType::Marquee},
    {EVOL_TAG_DEL_ALL_MARQUEES,         (uint) DatabaseEventType::DeleteAll,    (uint) ProductTextType::Marquee},

    {EVOL_TAG_READ_COOKING_INST,        (uint) DatabaseEventType::Read,         (uint) ProductTextType::CookingInstructions},
    {EVOL_TAG_WRITE_COOKING_INST,       (uint) DatabaseEventType::Write,        (uint) ProductTextType::CookingInstructions},
    {EVOL_TAG_DEL_COOKING_INST,         (uint) DatabaseEventType::Delete,       (uint) ProductTextType::CookingInstructions},
    {EVOL_TAG_DEL_ALL_COOKING_INST,     (uint) DatabaseEventType::DeleteAll,    (uint) ProductTextType::CookingInstructions},

    {EVOL_TAG_READ_RECIPE,              (uint) DatabaseEventType::Read,         (uint) ProductTextType::Recipe},
    {EVOL_TAG_WRITE_RECIPE,             (uint) DatabaseEventType::Write,        (uint) ProductTextType::Recipe},
    {EVOL_TAG_DEL_RECIPE,               (uint) DatabaseEventType::Delete,       (uint) ProductTextType::Recipe},
    {EVOL_TAG_DEL_ALL_RECIPE,           (uint) DatabaseEventType::DeleteAll,    (uint) ProductTextType::Recipe},

    {EVOL_TAG_READ_SUGGEST_SELL,        (uint) DatabaseEventType::Read,         (uint) ProductTextType::SuggestiveSelling},
    {EVOL_TAG_WRITE_SUGGEST_SELL,       (uint) DatabaseEventType::Write,        (uint) ProductTextType::SuggestiveSelling},
    {EVOL_TAG_DEL_SUGGEST_SELL,         (uint) DatabaseEventType::Delete,       (uint) ProductTextType::SuggestiveSelling},
    {EVOL_TAG_DEL_ALL_SUGGEST_SELL,     (uint) DatabaseEventType::DeleteAll,    (uint) ProductTextType::SuggestiveSelling},

    {EVOL_TAG_READ_ALLERGENS,           (uint) DatabaseEventType::Read,         (uint) ProductTextType::Allergens},
    {EVOL_TAG_WRITE_ALLERGENS,          (uint) DatabaseEventType::Write,        (uint) ProductTextType::Allergens},
    {EVOL_TAG_DEL_ALLERGENS,            (uint) DatabaseEventType::Delete,       (uint) ProductTextType::Allergens},
    {EVOL_TAG_DEL_ALL_ALLERGENS,        (uint) DatabaseEventType::DeleteAll,    (uint) ProductTextType::Allergens},

    {NULL,                              0,                                      0},
};

///////////////////////////////////////////////////////////////////////////////////////////////////
Event *TransformProductText::convertFromEvent( const JsonDocumentEvent *old, QString& tag )
{
    if ( !old )
        return nullptr;
    else if ( DeviceType::ProductTextDatabase != old->device() )
        return nullptr;

    // retrieve document object
    const QJsonDocument doc( old->document() );

    if ( !doc.isObject() )
        return nullptr;

    QJsonObject obj( doc.object() );

    // retrieve product text type
    if ( !obj.contains( JSON_PRODUCT_TEXT_TYPE_FIELD ) )
        return nullptr;

    const uint type( obj[JSON_PRODUCT_TEXT_TYPE_FIELD].toInt() );

    if ( !type )
        return nullptr;

    // locate product text type from tag
    for ( uint i( 0 ); ; ++i )
    {
        const TransformProductTextMap *xform( &TRANSFORM_PRODUCT_TEXT_MAP[i] );

        if ( !xform->tag )
            break;
        else if ( xform->event != old->event() )
            continue;
        else if ( xform->type != type )
            continue;

        // give tag to caller
        tag = xform->tag;

        // convert event
        JsonDocumentEvent *result( nullptr );

        if (( DatabaseEventType::ReadAll == (DatabaseEventType) old->event() ) ||
            ( DatabaseEventType::DeleteAll == (DatabaseEventType) old->event() ))
        {
            result = new JsonDocumentEvent( old->device(), old->event() );
        }
        else
        {
            // stip out type
            obj.remove( JSON_PRODUCT_TEXT_TYPE_FIELD );

            result = new JsonDocumentEvent( old->device(), old->event(), QJsonDocument( obj ) );
        }

        return result;
    }

    return nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
JsonDocumentEvent *TransformProductText::convertToEvent( const Event *old, const QString& tag )
{
    if ( !old )
        return nullptr;
    else if ( DeviceType::ProductTextDatabase != old->device() )
        return nullptr;

    const JsonDocumentEvent *e( dynamic_cast<const JsonDocumentEvent*>( old ) );

    if ( !e )
        return nullptr;

    // locate product text type from tag
    for ( uint i( 0 ); ; ++i )
    {
        const TransformProductTextMap *xform( &TRANSFORM_PRODUCT_TEXT_MAP[i] );

        if ( !xform->tag )
            break;
        else if ( tag != xform->tag )
            continue;

        // retrieve document object
        QJsonObject obj;

        const QJsonDocument doc( e->document() );

        if ( doc.isObject() )
            obj = doc.object();

        // insert type
        obj[JSON_PRODUCT_TEXT_TYPE_FIELD] = (int) xform->type;

        return new JsonDocumentEvent( old->device(), old->event(), QJsonDocument( obj ) );
    }

    return nullptr;
}
