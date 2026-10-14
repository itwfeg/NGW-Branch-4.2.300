/**
 * @file scalesettings.cpp
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#include "common.h"
#include "scaleconfig.h"
#include "scalesettings.h"
#include "stringsjson.h"
#include "scaledb.h"
#include "hookhte.h"

#include <QFile>
#include <QHostInfo>
#include <QJsonArray>
#include <QJsonObject>
//#include <QNetworkAddressEntry>
//#include <QNetworkInterface>
#include <QProcess>
#include <QTimeZone>


QString scaleSettings::scaleMode_;
QString scaleSettings::scaleItemEntryType_;

QString scaleSettings::printerLabelStock_;

bool scaleSettings::weigherExists_( false );
QString scaleSettings::weigherPrecision_;
QString scaleSettings::weigherCalibTimestamp_;
QString scaleSettings::weigherConfigTimestamp_;

bool scaleSettings::customerDisplayExists_( false );

///////////////////////////////////////////////////////////////////////////////////////////////////
scaleSettings::scaleSettings( QObject *parent ) :
    _Mybase( parent )
{
    // read settings
    readScaleSettings();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
scaleSettings::~scaleSettings()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool scaleSettings::isValid() const
{
    return settings_.size();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QJsonObject scaleSettings::toJsonObject() const
{
    static const QJsonValue nullVal( QJsonValue::Null );

    const QTimeZone tz( QTimeZone::systemTimeZone() );

    // ---- //

    QJsonObject obj;

    const scaleDatabase *db ( hteHook::instance()->db() );
    bool dbValid( db && db->isValid() );

    obj[JSON_STORE_NUMBER] = ( dbValid ) ? db->queryConfigs( DBCONFIG_STOREID ).toInt() : nullVal;
    obj[JSON_STORE_NAME] = ( dbValid ) ? db->queryConfigs( DBCONFIG_STORENAME ) : nullVal;
    obj[JSON_STORE_GRAPHIC] = ( dbValid ) ? checkStringValueNull( db->queryConfigs( DBCONFIG_STOREGRAPHICNAME ) ) : nullVal;

    obj[JSON_DEPT_NUMBER] = ( dbValid ) ? QVariant( db->queryConfigs( DBCONFIG_DEPTNUMBER ) ).toInt() : nullVal;
    obj[JSON_COUNTRY_CODE] = ( dbValid ) ? db->queryConfigs( DBCONFIG_PRIMARYCOUNTRYCODE ) : nullVal;

    obj[JSON_SCALE_MODE] = ( scaleMode_.length() ) ? scaleMode_ : nullVal;

    obj[JSON_SCALE_ITEM_ENTRY_TYPE] = ( scaleItemEntryType_.length() ) ? scaleItemEntryType_ : nullVal;

    obj[JSON_LABEL_STOCK] = ( printerLabelStock_.length() ) ? printerLabelStock_ : nullVal;

    obj[JSON_HAS_CUSTOMER_DISPLAY] = customerDisplayExists_;

    obj[JSON_HAS_LOAD_CELL] = weigherExists_;

    if ( weigherExists_ )
    {
        if ( weigherPrecision_.length() )
            obj[JSON_WEIGHER_PRECISION] = weigherPrecision_;

        if ( weigherCalibTimestamp_.length() )
        {
            // convert to ISO data
            QDate d( QDate::fromString( weigherCalibTimestamp_, "MMM dd, yy" ) );

            if ( d.isValid() )
            {
                if ( d.year() < 1990 )
                    d = d.addYears( 100 );

                obj[JSON_WEIGHER_CALIB_STAMP] = d.toString( Qt::ISODate );
            }
            else
            {
                LOG_WARN << "failed to convert weigher calibration stamp " << qPrintable( weigherCalibTimestamp_ );
                obj[JSON_WEIGHER_CALIB_STAMP] = weigherCalibTimestamp_;
            }
        }

        if ( weigherConfigTimestamp_.length() )
        {
            // convert to ISO data
            QDate d( QDate::fromString( weigherConfigTimestamp_, "MMM dd, yy" ) );

            if ( d.isValid() )
            {
                if ( d.year() < 1990 )
                    d = d.addYears( 100 );

                obj[JSON_WEIGHER_CONF_STAMP] = d.toString( Qt::ISODate );
            }
            else
            {
                LOG_WARN << "failed to convert weigher configuration stamp " << qPrintable( weigherConfigTimestamp_ );
                obj[JSON_WEIGHER_CONF_STAMP] = weigherConfigTimestamp_;
            }
        }
    }

    obj[JSON_TIME_ZONE] = tz.displayName( QTimeZone::GenericTime );
    obj[JSON_TIME_KEEPER] = settings_["timeKeeper"].trimmed().toUpper();


    // TODO
    // temporary, because server is not handling this correctly yet
    obj[JSON_WEIGHER_CALIB_STAMP] = nullVal;
    obj[JSON_WEIGHER_CONF_STAMP] = nullVal;


    return obj;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool scaleSettings::readScaleSettings()
{
    return scaleConfig::instance()->parseConfigFile( SCALESETTINGS_FILE, "=", settings_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QJsonValue scaleSettings::checkStringValueNull( const QString& value, bool zeroIsNull )
{
    static const QJsonValue nullVal( QJsonValue::Null );

    if (( value.isNull() ) || ( value.isEmpty() ))
        return nullVal;

    // empty ip address
    if ( "..." == value )
        return nullVal;

    // zero value
    if (( zeroIsNull ) && ( "0" == value ))
        return nullVal;

    return value;
}
