/**
 * @file scaleconfig.cpp
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#include "common.h"
#include "scaleinfo.h"
#include "scaleconfig.h"
#include "scalefirmware.h"
#include "stringsconfig.h"
#include "stringsjson.h"
#include "scaledb.h"
#include "hookhte.h"

#include <QByteArray>
#include <QCoreApplication>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonValue>
#include <QProcess>
#include <QString>
#include <QTextStream>



///////////////////////////////////////////////////////////////////////////////////////////////////
scaleInfo::scaleInfo( QObject *parent ) :
    _Mybase( parent )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
scaleInfo::~scaleInfo()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QJsonObject scaleInfo::toJsonObject() const
{
    static const QJsonValue nullVal( QJsonValue::Null );

    // populate object with config values
    QJsonObject obj;

    scaleDatabase *db = hteHook::instance()->db();
    bool dbValid( db && db->isValid() );

    obj[JSON_SERIAL_NUMBER] = (double) scaleConfig::instance()->serialNum();
    obj[JSON_SCALE_MODEL] = scaleConfig::instance()->fullModel();
    obj[JSON_STORE_NUMBER] = ( dbValid ) ? QVariant( db->queryConfigs( DBCONFIG_STOREID ) ).toInt() : nullVal;
    obj[JSON_DEPT_NUMBER] = ( dbValid ) ? QVariant( db->queryConfigs( DBCONFIG_DEPTNUMBER ) ).toInt() : nullVal;
    obj[JSON_COUNTRY_CODE] = ( dbValid ) ? db->queryConfigs( DBCONFIG_PRIMARYCOUNTRYCODE ) : nullVal;
    obj[JSON_PROFILE_ID] = nullVal;
    obj[JSON_BUILD_NUMBER] = ( scaleFirmware::buildNumber().length() ) ? scaleFirmware::buildNumber() : nullVal;

    scaleFirmware fw;
    fw.readVersionInfo();
    obj[JSON_APPLICATION] = fw.versions("Release ID");
    obj[JSON_OPERATING_SYSTEM] = fw.versions("OS Version");
    obj[JSON_SYSTEM_CONTROLLER] = fw.versions("HW Rev");
    obj[JSON_LOADER] = fw.versions("Bootloader Version");
    obj[JSON_SM_BACKEND] = fw.versions("SM Backend Version");

    // get features
    QJsonArray features;
    QStringList features_ = fw.features();
    if ( features_.size() > 0 )
    {
        foreach ( const QString& feature, features_ )
            features.append( feature );
        obj[JSON_FEATURES] = features;
    }
    else
        obj[JSON_FEATURES] = nullVal;

    // get licensing
    QJsonArray licenses;
    QStringList licenses_ = fw.licenses();
    if ( licenses_.size() > 0 )
    {
        foreach ( const QString& license, licenses_ )
            licenses.append( license );
        obj[JSON_LICENSES] = licenses;
    }
    else
        obj[JSON_LICENSES] = nullVal;

    return obj;
}

