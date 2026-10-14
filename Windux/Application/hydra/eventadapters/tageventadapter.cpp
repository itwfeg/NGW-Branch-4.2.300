/**
 * @file tageventadapter.cpp
 *
 * @copyright Copyright (c) 2021, ITW Hobart Food Equipment Group, U.S.A.
 * @author Randy Blankley
 */

#include "tageventadapter.h"
#include "transformprodtext.h"
#include "transformresource.h"

#include <databaseeventtype.h>
#include <databasejson.h>
#include <evoltags.h>
#include <fileresourceevent.h>
#include <jsondocumentevent.h>

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>

static const char RS = 0x1e;
static const char US = 0x1f;

///////////////////////////////////////////////////////////////////////////////////////////////////

struct TagFieldMap
{
    const char* tag;
    const char* json;
};

static const TagFieldMap DEPARTMENT_FIELDS[] = {
    {"d#",      JSON_DEPARTMENT_DEPTNUM_FIELD},
    {"dm",      JSON_DEPARTMENT_NAME1_FIELD},
    {"dM",      JSON_DEPARTMENT_NAME2_FIELD},
    {NULL,      NULL},
};

static const TagFieldMap PRODUCT_FIELDS[] = {
    {"p#",      JSON_PRODUCT_PLU_FIELD},
    {"PT",      JSON_PRODUCT_TYPE_FIELD},
    {"dt",      JSON_PRODUCT_DESCRIPTION_FIELD},
    {"d#",      JSON_PRODUCT_DEPTNUM_FIELD},

    {"rc",      JSON_PRODUCT_REPORTINGCLASS_FIELD},
    {"C#",      JSON_PRODUCT_CATEGORY_FIELD},
    {"S#",      JSON_PRODUCT_SPECIALNUMBER_FIELD},

    {"pm",      JSON_PRODUCT_PRICEFACTOR_FIELD},
    {"p_",      JSON_PRODUCT_PORTIONS_FIELD},
    {"pf",      NULL},                                              // product flags
    {"tf",      NULL},                                              // totals flags

    {"u#",      JSON_PRODUCT_BARCODEPREFIX_FIELD},
    {"up",      JSON_PRODUCT_RAWBARCODENUMBER_FIELD},               // upc number
    {"pn",      JSON_PRODUCT_BARCODEPRODUCTNUMBER_FIELD},
    {"vn",      JSON_PRODUCT_BARCODEVENDORNUMBER_FIELD},
    {"Ec",      JSON_PRODUCT_BARCODEEANCODE_FIELD},

    {"bc",      JSON_PRODUCT_BYCOUNT_FIELD},
    {"fb",      JSON_PRODUCT_FORCEBYCOUNT_FIELD},
    {"xp",      JSON_PRODUCT_EXCEPTIONPRICE_FIELD},
    {"xt",      NULL},                                              // exception price tare
    {"u$",      JSON_PRODUCT_PRICE_FIELD},
    {"fp",      JSON_PRODUCT_FORCEPRICE_FIELD},

    {"Dt",      JSON_PRODUCT_DISCOUNTMETHOD_FIELD},
    {"dp",      JSON_PRODUCT_DISCOUNTPRICE1_FIELD},
    {"D2",      JSON_PRODUCT_DISCOUNTPRICE2_FIELD},
    {"D3",      JSON_PRODUCT_DISCOUNTPRICE3_FIELD},
    {"ps",      JSON_PRODUCT_BONUSPOINTS_FIELD},

    {"ta",      JSON_PRODUCT_TARE_FIELD},
    {"T2",      JSON_PRODUCT_TAREB_FIELD},
    {"fT",      JSON_PRODUCT_FORCETARE_FIELD},
    {"pt",      JSON_PRODUCT_PROPORTIONALTARE_FIELD},
    {"nw",      JSON_PRODUCT_NETWEIGHT_FIELD},

    {"sl",      JSON_PRODUCT_SHELFLIFEDAYS_FIELD},
    {"SL",      JSON_PRODUCT_SHELFLIFEHOURS_FIELD},
    {"PS",      JSON_PRODUCT_DONTPRINTSHELFLIFE_FIELD},
    {"pl",      JSON_PRODUCT_PRODUCTLIFEDAYS_FIELD},
    {"PL",      JSON_PRODUCT_PRODUCTLIFEHOURS_FIELD},
    {"Pi",      JSON_PRODUCT_DONTPRINTPRODUCTLIFE_FIELD},
    {"PP",      JSON_PRODUCT_DONTPRINTPACKEDON_FIELD},

    {"g#",      NULL},                                              // graphic number
    {"G1",      JSON_PRODUCT_GRAPHIC1_FIELD},
    {"G2",      JSON_PRODUCT_GRAPHIC2_FIELD},
    {"G3",      JSON_PRODUCT_GRAPHIC3_FIELD},
    {"G4",      JSON_PRODUCT_GRAPHIC4_FIELD},
    {"i1",      JSON_PRODUCT_IMAGE_FIELD},                          // graphic name

    {"n1",      JSON_PRODUCT_NAME1_FIELD},
    {"n2",      JSON_PRODUCT_NAME2_FIELD},

    {"S#",      JSON_PRODUCT_SPECIAL_NUM_FIELD},
    {"sS",      JSON_PRODUCT_SPECIALSTARTDATE_FIELD},

    {"Bg",      JSON_PRODUCT_MEALSIDEITEMSGROUPNUM_FIELD},
    {"Bf",      JSON_PRODUCT_MEALSIDEITEMSAREFREE_FIELD},
    {"Bn",      JSON_PRODUCT_MEALNUMSIDESINCLUDED_FIELD},

    {"cM",      JSON_PRODUCT_CUTTESTMEATSPECSHEETGROUPNUM_FIELD},
    {"Pp",      JSON_PRODUCT_PRODUCTIONPLANNINGTYPE_FIELD},

    {"l1",      JSON_PRODUCT_LABEL1_FIELD},
    {"l2",      JSON_PRODUCT_LABEL2_FIELD},
    {"l3",      JSON_PRODUCT_LABEL3_FIELD},

    {"us",      JSON_PRODUCT_APPLYSECURITYLABEL_FIELD},
    {"im",      JSON_PRODUCT_INTAKEMODE_FIELD},
    {"p1",      JSON_PRODUCT_LABEL1ROTATION_FIELD},
    {"L1",      JSON_PRODUCT_LABEL1PLACEMENT_FIELD},
    {"p2",      JSON_PRODUCT_LABEL2ROTATION_FIELD},
    {"L2",      JSON_PRODUCT_LABEL2PLACEMENT_FIELD},
    {"p3",      JSON_PRODUCT_LABEL3ROTATION_FIELD},
    {"L3",      JSON_PRODUCT_LABEL3PLACEMENT_FIELD},

    // COOL
    {"c7",      JSON_PRODUCT_DEFAULTCOOLTEXT_FIELD},
    {"c8",      JSON_PRODUCT_CURRENTCOOLTEXT_FIELD},
    {"c9",      NULL},                                              // cool list first link
    {"cf",      JSON_PRODUCT_VERIFYCOOL_FIELD},
    {"co",      JSON_PRODUCT_COOLREQUIRED_FIELD},
    {"cc",      JSON_PRODUCT_COOLTRACKINGTEXT_FIELD},

    {"cb",      JSON_PRODUCT_COOLCATEGORY_FIELD},
    {"ci",      JSON_PRODUCT_COOLPRETEXT_FIELD},
    {"cs",      JSON_PRODUCT_COOLLISTKEY_FIELD},

    // Product Text
    {"r#",      JSON_PRODUCT_EXP_TEXT1_FIELD},                      // expanded text 1
    {"y7",      JSON_PRODUCT_EXP_TEXT2_FIELD},                      // expanded text 2
    {"s#",      JSON_PRODUCT_SPECIALMESSAGE_FIELD},
    {"n#",      JSON_PRODUCT_NOTE_FIELD},
    {"m#",      JSON_PRODUCT_MARQUEE_FIELD},
    {"y5",      JSON_PRODUCT_COOKING_INST_FIELD},                   // cooking
    {"y6",      JSON_PRODUCT_RECIPE_FIELD},                         // recipe
    {"y8",      JSON_PRODUCT_SUGGEST_SELL_FIELD},                   // suggestive sell
    {"y9",      JSON_PRODUCT_ALLERGENS_FIELD},                      // allergens
    {"N#",      JSON_PRODUCT_NUTRITION_FIELD},

    // Pick-5
    {"mw",      JSON_PRODUCT_WEIGHTMIN_FIELD},
    {"Mw",      JSON_PRODUCT_WEIGHTMAX_FIELD},
    {"mp",      JSON_PRODUCT_TOTALPRICEMIN_FIELD},
    {"Mp",      JSON_PRODUCT_TOTALPRICEMAX_FIELD},

    // Automatic Mode Labels
    {"l4",      JSON_PRODUCT_MANUALLABELTYPENUM1_FIELD},
    {"l5",      JSON_PRODUCT_MANUALLABELTYPENUM2_FIELD},
    {"l6",      JSON_PRODUCT_MANUALLABELTYPENUM3_FIELD},
    {"l7",      JSON_PRODUCT_PREPACKLABELTYPENUM1_FIELD},
    {"l8",      JSON_PRODUCT_PREPACKLABELTYPENUM2_FIELD},
    {"l9",      JSON_PRODUCT_PREPACKLABELTYPENUM3_FIELD},
    {"L4",      JSON_PRODUCT_PRODENTRYLABELTYPENUM1_FIELD},
    {"L5",      JSON_PRODUCT_PRODENTRYLABELTYPENUM2_FIELD},
    {"L6",      JSON_PRODUCT_PRODENTRYLABELTYPENUM3_FIELD},
    {"L7",      JSON_PRODUCT_SELFSERVLABELTYPENUM1_FIELD},
    {"L8",      JSON_PRODUCT_SELFSERVLABELTYPENUM2_FIELD},
    {"L9",      JSON_PRODUCT_SELFSERVLABELTYPENUM3_FIELD},
    {"La",      JSON_PRODUCT_PICK5LABELTYPENUM1_FIELD},
    {"Lb",      JSON_PRODUCT_PICK5LABELTYPENUM2_FIELD},
    {"Lc",      JSON_PRODUCT_PICK5LABELTYPENUM3_FIELD},

    {NULL,      NULL},
};

static const TagFieldMap OPERATOR_FIELDS[] = {
    {"oi",      JSON_OPERATOR_NUMBER_FIELD},
    {"or",      JSON_OPERATOR_RFID_FIELD},
    {"on",      JSON_OPERATOR_NAME_FIELD},
    {"ol",      JSON_OPERATOR_LANGUAGE_FIELD},
    {"oc",      JSON_OPERATOR_CLASSIFICATION_FIELD},                // access level

    {"oN",      JSON_OPERATOR_NOTE_FIELD},
    {"os",      JSON_OPERATOR_SAWNOTE_FIELD},

    {NULL,      NULL},
};

static const TagFieldMap OPERATOR_NOTES_FIELDS[] = {
    {"oN",      JSON_OPERATOR_NOTES_NOTE_FIELD},
    {NULL,      NULL},
};

static const TagFieldMap CATEGORY_FIELDS[] = {
    {"C#",      JSON_CATEGORY_NUMBER_FIELD},
    {"rC",      JSON_CATEGORY_PARENTCATEGORY_FIELD},
    {"d#",      JSON_CATEGORY_DEPTNUM_FIELD},
    {"do",      NULL},                                              // display order

    {"n1",      JSON_CATEGORY_NAME1_FIELD},
    {"n2",      JSON_CATEGORY_NAME2_FIELD},

    {"bC",      JSON_CATEGORY_BACKGROUNDCOLOR_FIELD},
    {"tC",      JSON_CATEGORY_TEXTCOLOR_FIELD},
    {"i1",      JSON_CATEGORY_IMAGE_FIELD},
    {"Gf",      JSON_CATEGORY_PLAYLIST_FIELD},

    {NULL,      NULL},
};

static const TagFieldMap EXP_TEXT1_FIELDS[] = {
    {"r#",      JSON_PRODUCT_TEXT_NUMBER_FIELD},
    {"d#",      JSON_PRODUCT_TEXT_DEPTNUM_FIELD},
    {"rt",      JSON_PRODUCT_TEXT_TEXT_FIELD},
    {NULL,      NULL},
};

static const TagFieldMap EXP_TEXT2_FIELDS[] = {
    {"y7",      JSON_PRODUCT_TEXT_NUMBER_FIELD},
    {"d#",      JSON_PRODUCT_TEXT_DEPTNUM_FIELD},
    {"Y7",      JSON_PRODUCT_TEXT_TEXT_FIELD},
    {NULL,      NULL},
};

static const TagFieldMap SPECIAL_MSGS_FIELDS[] = {
    {"s#",      JSON_PRODUCT_TEXT_NUMBER_FIELD},
    {"d#",      JSON_PRODUCT_TEXT_DEPTNUM_FIELD},
    {"sm",      JSON_PRODUCT_TEXT_TEXT_FIELD},
    {NULL,      NULL},
};

static const TagFieldMap PRODUCT_NOTES_FIELDS[] = {
    {"n#",      JSON_PRODUCT_TEXT_NUMBER_FIELD},
    {"d#",      JSON_PRODUCT_TEXT_DEPTNUM_FIELD},
    {"an",      JSON_PRODUCT_TEXT_TEXT_FIELD},
    {NULL,      NULL},
};

static const TagFieldMap MARQUEES_FIELDS[] = {
    {"m#",      JSON_PRODUCT_TEXT_NUMBER_FIELD},
    {"d#",      JSON_PRODUCT_TEXT_DEPTNUM_FIELD},
    {"mm",      JSON_PRODUCT_TEXT_TEXT_FIELD},
    {NULL,      NULL},
};

static const TagFieldMap COOKING_INST_FIELDS[] = {
    {"y5",      JSON_PRODUCT_TEXT_NUMBER_FIELD},
    {"d#",      JSON_PRODUCT_TEXT_DEPTNUM_FIELD},
    {"Y5",      JSON_PRODUCT_TEXT_TEXT_FIELD},
    {NULL,      NULL},
};

static const TagFieldMap RECIPE_FIELDS[] = {
    {"y6",      JSON_PRODUCT_TEXT_NUMBER_FIELD},
    {"d#",      JSON_PRODUCT_TEXT_DEPTNUM_FIELD},
    {"Y6",      JSON_PRODUCT_TEXT_TEXT_FIELD},
    {NULL,      NULL},
};

static const TagFieldMap SUGGEST_SELL_FIELDS[] = {
    {"y8",      JSON_PRODUCT_TEXT_NUMBER_FIELD},
    {"d#",      JSON_PRODUCT_TEXT_DEPTNUM_FIELD},
    {"Y8",      JSON_PRODUCT_TEXT_TEXT_FIELD},
    {NULL,      NULL},
};

static const TagFieldMap ALLERGENS_FIELDS[] = {
    {"y9",      JSON_PRODUCT_TEXT_NUMBER_FIELD},
    {"d#",      JSON_PRODUCT_TEXT_DEPTNUM_FIELD},
    {"Y9",      JSON_PRODUCT_TEXT_TEXT_FIELD},
    {NULL,      NULL},
};

static const TagFieldMap NUTRIFACTS_FIELDS[] = {
    {"N#",      JSON_NUTRIFACTS_NUMBER_FIELD},
    {"d#",      JSON_NUTRIFACTS_DEPTNUM_FIELD},
    {"N1",      JSON_NUTRIFACTS_COL1_DATA_FIELD},
    {"N2",      JSON_NUTRIFACTS_COL1_DATA_FIELD},
    {"N3",      JSON_NUTRIFACTS_COL1_DATA_FIELD},
    {"N4",      JSON_NUTRIFACTS_COL1_DATA_FIELD},
    {NULL,      NULL},
};

static const TagFieldMap GRAPHIC_RESOURCE_FIELDS[] = {
    {"FN",      JSON_RESOURCE_FILE_NAME},
    {"rs",      JSON_RESOURCE_DATA},
    {NULL,      NULL},
};

static const TagFieldMap FONT_RESOURCE_FIELDS[] = {
    {"FN",      JSON_RESOURCE_FILE_NAME},
    {"rs",      JSON_RESOURCE_DATA},
    {NULL,      NULL},
};

///////////////////////////////////////////////////////////////////////////////////////////////////

struct TagEventMap
{
    DeviceType device;
    const TagFieldMap *fields;
    const char *read;
    const char *write;
    const char *del;
    const char *delAll;
};

static const TagEventMap TAG_EVENT_MAP[] = {
    {DeviceType::DepartmentDatabase,            DEPARTMENT_FIELDS,          EVOL_TAG_READ_DEPT_NAME,            EVOL_TAG_WRITE_DEPT_NAME,           NULL,                               EVOL_TAG_DEL_ALL_DEPT_NAMES},

    {DeviceType::ProductDatabase,               PRODUCT_FIELDS,             EVOL_TAG_READ_PRODUCT,              EVOL_TAG_WRITE_PRODUCT,             EVOL_TAG_DEL_PRODUCT,               EVOL_TAG_DEL_ALL_PRODUCT},
    {DeviceType::OperatorDatabase,              OPERATOR_FIELDS,            EVOL_TAG_READ_OPERATOR,             EVOL_TAG_WRITE_OPERATOR,            EVOL_TAG_DEL_OPERATOR,              EVOL_TAG_DEL_ALL_OPERATOR},
    {DeviceType::OperatorNotesDatabase,         OPERATOR_NOTES_FIELDS,      EVOL_TAG_READ_OPERATOR_NOTES,       EVOL_TAG_WRITE_OPERATOR_NOTES,      NULL,                               EVOL_TAG_DEL_ALL_OPERATOR_NOTES},
    {DeviceType::CategoryDatabase,              CATEGORY_FIELDS,            EVOL_TAG_READ_CATEGORY,             EVOL_TAG_WRITE_CATEGORY,            EVOL_TAG_DEL_CATEGORY,              EVOL_TAG_DEL_ALL_CATEGORY},

    {DeviceType::ProductTextDatabase,           EXP_TEXT1_FIELDS,           EVOL_TAG_READ_EXP_TEXT1,            EVOL_TAG_WRITE_EXP_TEXT1,           EVOL_TAG_DEL_EXP_TEXT1,             EVOL_TAG_DEL_ALL_EXP_TEXT1},
    {DeviceType::ProductTextDatabase,           EXP_TEXT2_FIELDS,           EVOL_TAG_READ_EXP_TEXT2,            EVOL_TAG_WRITE_EXP_TEXT2,           EVOL_TAG_DEL_EXP_TEXT2,             EVOL_TAG_DEL_ALL_EXP_TEXT2},
    {DeviceType::ProductTextDatabase,           SPECIAL_MSGS_FIELDS,        EVOL_TAG_READ_SPECIAL_MSGS,         EVOL_TAG_WRITE_SPECIAL_MSGS,        NULL,                               NULL},
    {DeviceType::ProductTextDatabase,           PRODUCT_NOTES_FIELDS,       EVOL_TAG_READ_PRODUCT_NOTES,        EVOL_TAG_WRITE_PRODUCT_NOTES,       EVOL_TAG_DEL_PRODUCT_NOTE,          EVOL_TAG_DEL_ALL_PRODUCT_NOTES},
    {DeviceType::ProductTextDatabase,           MARQUEES_FIELDS,            EVOL_TAG_READ_MARQUEES,             EVOL_TAG_WRITE_MARQUEES,            EVOL_TAG_DEL_MARQUEE,               EVOL_TAG_DEL_ALL_MARQUEES},
    {DeviceType::ProductTextDatabase,           COOKING_INST_FIELDS,        EVOL_TAG_READ_COOKING_INST,         EVOL_TAG_WRITE_COOKING_INST,        EVOL_TAG_DEL_COOKING_INST,          EVOL_TAG_DEL_ALL_COOKING_INST},
    {DeviceType::ProductTextDatabase,           RECIPE_FIELDS,              EVOL_TAG_READ_RECIPE,               EVOL_TAG_WRITE_RECIPE,              EVOL_TAG_DEL_RECIPE,                EVOL_TAG_DEL_ALL_RECIPE},
    {DeviceType::ProductTextDatabase,           SUGGEST_SELL_FIELDS,        EVOL_TAG_READ_SUGGEST_SELL,         EVOL_TAG_WRITE_SUGGEST_SELL,        EVOL_TAG_DEL_SUGGEST_SELL,          EVOL_TAG_DEL_ALL_SUGGEST_SELL},
    {DeviceType::ProductTextDatabase,           ALLERGENS_FIELDS,           EVOL_TAG_READ_ALLERGENS,            EVOL_TAG_WRITE_ALLERGENS,           EVOL_TAG_DEL_ALLERGENS,             EVOL_TAG_DEL_ALL_ALLERGENS},

    {DeviceType::NutritionDatabase,             NUTRIFACTS_FIELDS,          EVOL_TAG_READ_NU_DATA,              EVOL_TAG_WRITE_NU_DATA,             EVOL_TAG_DEL_NU_DATA,               EVOL_TAG_DEL_ALL_NU_DATA},

    {DeviceType::GraphicResource,               GRAPHIC_RESOURCE_FIELDS,    EVOL_TAG_READ_ROM_GRAPHICS,         EVOL_TAG_WRITE_ROM_GRAPHICS,        NULL,                               NULL},
    {DeviceType::GraphicResource,               GRAPHIC_RESOURCE_FIELDS,    EVOL_TAG_READ_PRODUCT_GRAPHICS,     EVOL_TAG_WRITE_PRODUCT_GRAPHICS,    EVOL_TAG_DEL_PRODUCT_GRAPHIC,       EVOL_TAG_DEL_ALL_PRODUCT_GRAPHICS},
    {DeviceType::GraphicResource,               GRAPHIC_RESOURCE_FIELDS,    EVOL_TAG_READ_CATEGORY_GRAPHICS,    EVOL_TAG_WRITE_CATEGORY_GRAPHICS,   EVOL_TAG_DEL_CATEGORY_GRAPHIC,      EVOL_TAG_DEL_ALL_CATEGORY_GRAPHICS},
    {DeviceType::GraphicResource,               GRAPHIC_RESOURCE_FIELDS,    EVOL_TAG_READ_SPECIALS_GRAPHICS,    EVOL_TAG_WRITE_SPECIALS_GRAPHICS,   EVOL_TAG_DEL_SPECIALS_GRAPHIC,      EVOL_TAG_DEL_ALL_SPECIALS_GRAPHICS},
    {DeviceType::GraphicResource,               GRAPHIC_RESOURCE_FIELDS,    EVOL_TAG_READ_STREAMING_GRAPHICS,   EVOL_TAG_WRITE_STREAMING_GRAPHICS,  EVOL_TAG_DEL_STREAMING_GRAPHIC,     EVOL_TAG_DEL_ALL_STREAMING_GRAPHICS},
    {DeviceType::GraphicResource,               GRAPHIC_RESOURCE_FIELDS,    EVOL_TAG_READ_USER_GRAPHICS,        EVOL_TAG_WRITE_USER_GRAPHICS,       EVOL_TAG_DEL_USER_GRAPHIC,          EVOL_TAG_DEL_ALL_USER_GRAPHICS},
    {DeviceType::GraphicResource,               GRAPHIC_RESOURCE_FIELDS,    EVOL_TAG_READ_SOFTKEY_GRAPHICS,     EVOL_TAG_WRITE_SOFTKEY_GRAPHICS,    EVOL_TAG_DEL_SOFTKEY_GRAPHIC,       EVOL_TAG_DEL_ALL_SOFTKEY_GRAPHICS},
    {DeviceType::GraphicResource,               GRAPHIC_RESOURCE_FIELDS,    EVOL_TAG_READ_PLANOGRAM_GRAPHICS,   EVOL_TAG_WRITE_PLANOGRAM_GRAPHICS,  EVOL_TAG_DEL_PLANOGRAM_GRAPHICS,    EVOL_TAG_DEL_ALL_PLANOGRAM_GRAPHICS},

    {DeviceType::FontResource,                  FONT_RESOURCE_FIELDS,       EVOL_TAG_READ_USER_FONTS,           EVOL_TAG_WRITE_USER_FONTS,          EVOL_TAG_DEL_USER_FONT,             EVOL_TAG_DEL_ALL_USER_FONTS},

    {DeviceType::Invalid,                       NULL,                       NULL,                               NULL,                               NULL,                               NULL},
};

///////////////////////////////////////////////////////////////////////////////////////////////////
TagEventAdapter::TagEventAdapter() :
    ignoreUnknownRecTags_( true ),
    ignoreUnknownFieldTags_( false )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
TagEventAdapter::~TagEventAdapter()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool TagEventAdapter::ignoreUnknownFieldTags() const
{
    return ignoreUnknownFieldTags_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool TagEventAdapter::ignoreUnknownRecTags() const
{
    return ignoreUnknownRecTags_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void TagEventAdapter::setIgnoreUnknownFieldTags( bool value )
{
    ignoreUnknownFieldTags_ = value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void TagEventAdapter::setIgnoreUnknownRecTags( bool value )
{
    ignoreUnknownRecTags_ = value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void TagEventAdapter::destroyEvents( const QList<Event*>& doomed )
{
    foreach ( Event *e, doomed )
    {
        if ( e )
            delete e;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QList<Event*> TagEventAdapter::translateComm( QByteArray& buffer, bool *valid )
{
    QList<Event*> events;

    // assume success
    if ( valid )
        (*valid) = true;

main_loop:

    // look for a message within the buffer
    for ( int i = 0; i < buffer.size(); ++i )
        if ( RS == buffer.at( i ) )
        {
            Event *e( nullptr );

            // convert message
            if ( (e = convert( buffer.left( i ) )) )
                events.append( e );
            else
            {
                const QByteArray b( buffer.left( 4 ) );

                if ( ignoreUnknownRecTags() )
                    qWarning() << "dropping unknown/bad tag" << b;
                else
                {
                    if ( valid )
                        (*valid) = false;

                    qCritical() << "unknown/bad tag" << b;
                    goto done;
                }
            }

            // try to find another message
            buffer = buffer.mid( i+1 );

            goto main_loop;
        }

done:

    return events;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QByteArray TagEventAdapter::translateEvent( const Event *e, bool *valid )
{
    if ( !e )
    {
        if ( valid )
            (*valid) = false;

        return QByteArray();
    }

    QByteArray a;

    // convert event
    const JsonDocumentEvent *docEvent( dynamic_cast<const JsonDocumentEvent*>( e ) );

    if ( docEvent )
    {
        QString tag;

        Event *temp( TransformProductText::convertFromEvent( docEvent, tag ) );

        if ( !temp )
            a = convert( docEvent );
        else
        {
            if ( 2 == tag.length() )
                a = convert( dynamic_cast<const JsonDocumentEvent*>( temp ), tag.toLatin1().constData() );

            delete temp;
        }
    }

    // convert event
    const FileResourceEvent *fileResEvent( dynamic_cast<const FileResourceEvent*>( e ) );

    if ( fileResEvent )
    {
        QString tag;

        Event *temp( TransformResource::convertFromEvent( fileResEvent, tag ) );

        if ( temp )
        {
            if ( 2 == tag.length() )
                a = convert( dynamic_cast<const JsonDocumentEvent*>( temp ), tag.toLatin1().constData() );

            delete temp;
        }
    }

    // TODO
    // other event types

    // check for success
    if ( !a.size() )
    {
        if ( ignoreUnknownRecTags() )
            qWarning() << "dropping unknown/bad device" << (uint) e->device();
        else
        {
            if ( valid )
                (*valid) = false;

            qCritical() << "unknown/bad device" << (uint) e->device();
        }
    }

    return a;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
Event *TagEventAdapter::convert( const QByteArray& buffer )
{
    if ( buffer.size() < 4 )
    {
        qWarning() << "invalid buffer size";
        return nullptr;
    }

    // split buffer into fields
    const QList<QByteArray> fields( buffer.split( US ) );

    TagDataPairs data;

    // split each field into tag and data
    foreach ( const QByteArray& f, fields )
    {
        if ( f.size() < 2 )
        {
            qWarning() << "invalid field length" << f;
            return nullptr;
        }

        data.append( TagDataPair( f.left( 2 ), f.mid( 2 ) ) );
    }

    Event *e( nullptr );

    // record type
    //   process the RT record if they send it -or-
    //   if there is no RT then use previous record type (i.e. sending multiple product writes in a row)
    if (( "RT" == data[0].first ) || ( prevRecTag_.length() ))
    {
        // use previous record tag
        if ( "RT" != data[0].first )
        {
            qDebug() << "using previous record type" << prevRecTag_;
            data.prepend( TagDataPair( "RT", prevRecTag_.toLatin1() ) );
        }

        // clear out previous record tag
        prevRecTag_.clear();

        // convert record
        if ( !(e = convertRecord( data )) )
            qWarning() << "failed to convert record" << data[0].second;
        else
        {
            // transform
            JsonDocumentEvent *docEvent( TransformProductText::convertToEvent( e, data[0].second ) );

            if ( docEvent )
            {
                delete e;
                e = docEvent;
            }

            // transform
            FileResourceEvent *fileResEvent( TransformResource::convertToEvent( e, data[0].second ) );

            if ( fileResEvent )
            {
                delete e;
                e = fileResEvent;
            }
        }
    }

    // unknown
    else
    {
        // clear out previous record tag
        prevRecTag_.clear();

        qWarning() << "unknown tag type" << fields[0];
    }

    return e;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
Event *TagEventAdapter::convertRecord( const TagDataPairs& data )
{
    const QString recordType( data[0].second );

    qDebug() << "processing RT" << recordType;

    // locate this tag in the event map
    for ( uint i( 0 ); ; ++i )
    {
        const TagEventMap *mapping( &TAG_EVENT_MAP[i] );

        // reached end of mapping and did not find match
        if ( !(uint)( mapping->device ) )
            break;

        // read all (no data means read all)
        if (( recordType == mapping->read ) && ( 1 == data.size() ))
            return new JsonDocumentEvent( mapping->device, (uint) DatabaseEventType::ReadAll );

        // delete all
        else if ( recordType == mapping->delAll )
            return new JsonDocumentEvent( mapping->device, (uint) DatabaseEventType::DeleteAll );

        uint event( 0 );

        // read
        if ( recordType == mapping->read )
            event = (uint) DatabaseEventType::Read;

        // write
        else if ( recordType == mapping->write )
            event = (uint) DatabaseEventType::Write;

        // delete
        else if ( recordType == mapping->del )
            event = (uint) DatabaseEventType::Delete;

        if ( !event )
            continue;

        QJsonDocument doc;

        if (( !convertRecordFields( data, mapping->fields, doc ) ) || ( doc.isNull() ))
        {
            qWarning() << "failed to convert one (or more) fields";
            return nullptr;
        }

        // save off this record type
        prevRecTag_ = recordType;

        return new JsonDocumentEvent( mapping->device, event, doc );
    }

    qWarning() << "unmapped RT" << recordType;
    return nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool TagEventAdapter::convertRecordFields( const TagDataPairs& data, const TagFieldMap *fields, QJsonDocument& doc )
{
    if ( !fields )
    {
        qCritical() << "no mapping for RT" << data[0].second;
        return false;
    }

    QJsonObject obj;

    // map all tag fields to json fields
    // skip index 0 (RT)
    for ( int i( 1 ); i < data.size(); ++i )
    {
        bool found( false );

        // find tag in fields list
        for ( int f( 0 ); ; ++f )
        {
            // reached end of list
            if ( !fields[f].tag )
                break;

            // found!
            else if ( fields[f].tag == data[i].first )
            {
                const char *mapping( fields[f].json );

                // when mapping exists add it to the JSON object
                if ( mapping )
                    obj[mapping] = QString::fromUtf8( data[i].second );

                found = true;
                break;
            }
        }

        // did not find tag... either drop this field or abort
        if ( !found )
        {
            if ( ignoreUnknownFieldTags() )
                qWarning() << "dropping unknown/bad RT" << data[0].second << "field" << data[i].first;
            else
            {
                qCritical() << "unknown/bad RT" << data[0].second << "field" << data[i].first;
                return false;
            }
        }
    }

    // give result to caller
    doc.setObject( obj );
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QByteArray TagEventAdapter::convert( const JsonDocumentEvent *e, const char *tag )
{
    if ( !e )
        return QByteArray();

    // locate this device in the event map
    for ( uint i( 0 ); ; ++i )
    {
        const TagEventMap *mapping( &TAG_EVENT_MAP[i] );

        // reached end of mapping and did not find match
        if ( !(uint)( mapping->device ) )
            break;

        // not a match
        else if ( e->device() != mapping->device )
            continue;

        // ---- //

        QByteArray a;
        a.append( "RT" );

        switch ( (DatabaseEventType) e->event() )
        {
        case DatabaseEventType::Read:
            a.append( tag ? tag : mapping->read );

            if ( !convertRecordFields( e->document(), mapping->fields, a ) )
                return QByteArray();

            break;

        case DatabaseEventType::ReadAll:
            a.append( tag ? tag : mapping->read );
            break;

        case DatabaseEventType::Write:
            a.append( tag ? tag : mapping->write );

            if ( !convertRecordFields( e->document(), mapping->fields, a ) )
                return QByteArray();

            break;

        case DatabaseEventType::Delete:
            a.append( tag ? tag : mapping->del );

            if ( !convertRecordFields( e->document(), mapping->fields, a ) )
                return QByteArray();

            break;

        case DatabaseEventType::DeleteAll:
            a.append( tag ? tag : mapping->delAll );
            break;

        default:
            qWarning() << "unknown/bad event" << e->event() << "for device" << (uint) e->device();
            return QByteArray();
        }

        // append record separator
        a.append( RS );

        return a;
    }

    return QByteArray();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool TagEventAdapter::convertRecordFields( const QJsonDocument& doc, const TagFieldMap *fields, QByteArray& a )
{
    // validate
    if ( !doc.isObject() )
    {
        qCritical() << "not an object";
        return false;
    }
    else if ( !fields )
    {
        qCritical() << "no mapping for device";
        return false;
    }

    const QJsonObject obj( doc.object() );

    // map all json fields to tag
    for ( QJsonObject::const_iterator i( obj.constBegin() ); i != obj.constEnd(); ++i )
    {
        // skip empty fields
        if ( i->isNull() )
            continue;

        bool found( false );

        // find tag in fields list
        for ( int f( 0 ); ; ++f )
        {
            // reached end of list
            if ( !fields[f].tag )
                break;

            // found!
            else if ( fields[f].json == i.key() )
            {
                found = true;

                // convert data to string value
                if ( !i->isNull() )
                {
                    QString data;

                    if ( i->isString() )
                        data = i->toString();
                    else if ( i->isDouble() )
                    {
                        const double vald( i->toDouble() );
                        const int vali( i->toInt() );

                        if ( 0.0 == (vald - vali) )
                            data = QString::number( vali );
                        else
                            data = QString::number( vald );
                    }
                    else if ( i->isBool() )
                        data = (i->toBool() ? "1" : "0");
                    else
                    {
                        qWarning() << "unsupported data type" << i->type();
                    }

                    a.append( US );
                    a.append( fields[f].tag );
                    a.append( data.toUtf8() );
                }

                break;
            }
        }

        // did not find tag... either drop this field or abort
        if ( !found )
        {
            if ( ignoreUnknownFieldTags() )
                qWarning() << "dropping unknown/bad field" << i.key();
            else
            {
                qCritical() << "unknown/bad field" << i.key();
                return false;
            }
        }
    }

    // success!
    return true;
}
