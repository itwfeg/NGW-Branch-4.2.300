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

#include <QFile>
#include <QHostInfo>
#include <QJsonArray>
#include <QJsonObject>
#include <QNetworkAddressEntry>
#include <QNetworkInterface>
#include <QProcess>
#include <QTimeZone>

QString scaleSettings::storeId_;
QString scaleSettings::storeName_;
QString scaleSettings::storeGraphic_;

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
    readSupplicant();

    // ---- //

    connectionTypes_["0"] = "WIRED";
    connectionTypes_["1"] = "WIRELESS";

    securityMethods_["0"] = "SECURITY_OPEN";                // "OPEN"
    securityMethods_["1"] = "SECURITY_WPA_PERSONAL";        // "WPA-Personal"
    securityMethods_["2"] = "SECURITY_WPA_EAP_TLS";         // "WPA EAP-TLS"
    securityMethods_["3"] = "SECURITY_WPA_PEAP";            // "WPA-Enterprise (PEAP)"
    securityMethods_["4"] = "SECURITY_WPA_LEAP";            // "WPA-Enterprise (LEAP)"
    securityMethods_["5"] = "SECURITY_WPA2_PERSONAL";       // "WPA2-Personal"
    securityMethods_["6"] = "SECURITY_WPA2_EAP_TLS";        // "WPA2 EAP-TLS"
    securityMethods_["7"] = "SECURITY_WPA2_PEAP";           // "WPA2-Enterprise (PEAP)"
    securityMethods_["8"] = "SECURITY_WPA2_LEAP";           // "WPA2-Enterprise (LEAP)"
    securityMethods_["9"] = "SECURITY_802X1_PEAP";          // "802.1X (PEAP)"
    securityMethods_["10"] = "SECURITY_802X1_LEAP";         // "802.1X (LEAP)"

    openEncryptionTypes_["0"] = "WEP";
    openEncryptionTypes_["1"] = "NONE";

    wpaEncryptionTypes_["0"] = "AES";
    wpaEncryptionTypes_["1"] = "TKIP";

    eapEncryptionTypes_["0"] = "WEP";
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

    QJsonObject store;
    store[JSON_STORE_ID] = storeId_.toInt();
    store[JSON_STORE_NAME] = storeName_;
    store[JSON_STORE_GRAPHIC] = checkStringValueNull( storeGraphic_ );

    // ---- //

    QJsonObject ntp;
    ntp[JSON_SERVER] = settings_["timeKeeper"];
    ntp[JSON_FREQUENCY] = settings_["ntpFrequency"].toInt();
    ntp[JSON_DAY_OF_WEEK] = settings_["ntpDayOfWeek"].toInt();
    ntp[JSON_TIME_OF_DAY] = settings_["ntpTimeOfDay"];
    ntp[JSON_TIME_ZONE] = settings_["ntpTimeZone"];
    ntp[JSON_ANTI_MERIDIAN] = settings_["ntpAntiMeridian"];

    // ---- //

    QString macAddress;
    QString ipAddress4;
    QString ipAddress6;

    QJsonObject network;
    network[JSON_HOST_NAME] = QHostInfo::localHostName();
    network[JSON_CONNECTION_TYPE] = connectionTypes_[settings_["connectionType"]];

    if ( queryAdapterInfo( 0, macAddress, ipAddress4, ipAddress6 ) )
    {
        QJsonObject wired;
        wired[JSON_MAC_ADDRESS] = macAddress;
        wired[JSON_IP_ADDRESS4] = checkStringValueNull( ipAddress4 );
        wired[JSON_IP_ADDRESS6] = checkStringValueNull( ipAddress6 );

        const QString speed( queryWiredConnectionStatus() );

        if ( speed.length() )
            wired[JSON_CONNECTION_SPEED] = speed;

        network[JSON_WIRED] = wired;
    }

    if ( queryAdapterInfo( 1, macAddress, ipAddress4, ipAddress6 ) )
    {
        QJsonObject wireless;
        wireless[JSON_MAC_ADDRESS] = macAddress;
        wireless[JSON_IP_ADDRESS4] = checkStringValueNull( ipAddress4 );
        wireless[JSON_IP_ADDRESS6] = checkStringValueNull( ipAddress6 );
        wireless[JSON_CHANNEL] = settings_["channel"];
        wireless[JSON_AUTO_LINK_RATE] = ("1" == settings_["automaticLinkRate"]);
        wireless[JSON_LINK_RATE] = settings_["linkRate"];

        // as per Varun 2022-05-31 do not send "auto" for channel, send NULL instead
        if ( 0 == wireless[JSON_CHANNEL].toString().compare( "auto", Qt::CaseInsensitive ) )
            wireless[JSON_CHANNEL] = nullVal;

        // as per Varun 2022-05-31 send integer value instead of string value
        else
        {
            const int channel( wireless[JSON_CHANNEL].toString().toInt() );

            wireless[JSON_CHANNEL] = channel;
        }

        // as per Varun 2022-05-31 link rate may only be one of these two values:
        // - AUTO
        // - MBS_PER_SECOND
        if ( 0 == wireless[JSON_LINK_RATE].toString().compare( "auto", Qt::CaseInsensitive ) )
            wireless[JSON_LINK_RATE] = "AUTO";
        else if ( 0 < wireless[JSON_LINK_RATE].toString().length() )
            wireless[JSON_LINK_RATE] = "MBS_PER_SECOND";
        else
            wireless[JSON_LINK_RATE] = nullVal;

        const QString speed( queryWirelessConnectionStatus() );

        if ( speed.length() )
            wireless[JSON_SIGNAL_STRENGTH] = speed;

        QJsonObject adapter;
        adapter[JSON_SSID] = settings_["connectionName"];
        adapter[JSON_SECURITY_METHOD] = securityMethods_[settings_["security"]];

        if ( adapter[JSON_SECURITY_METHOD].toString().trimmed().isEmpty() )
            adapter[JSON_SECURITY_METHOD] = nullVal;

        /*
            security
             0 = OPEN
                encryption
                0 = WEP
                1 = None

             1 = WPA-Personal
             2 = WPA EAP-TLS
             3 = WPA-Enterprise (PEAP)
             4 = WPA-Enterprise (LEAP)
             5 = WPA2-Personal
             6 = WPA2 EAP-TLS
             7 = WPA2-Enterprise (PEAP)
             8 = WPA2-Enterprise (LEAP)
                encryption
                0 = AES
                1 = TKIP

             9 = 802.1X (PEAP)
            10 = 802.1X (LEAP)
                encryption
                0 = WEP
        */
        if ( "0" == settings_["security"] )
            adapter[JSON_ENCRYPTION] = openEncryptionTypes_[settings_["encryption"]];
        else if (( "9" == settings_["security"] ) || ( "10" == settings_["security"] ))
            adapter[JSON_ENCRYPTION] = eapEncryptionTypes_[settings_["encryption"]];
        else
            adapter[JSON_ENCRYPTION] = wpaEncryptionTypes_[settings_["encryption"]];

        if ( adapter[JSON_ENCRYPTION].toString().trimmed().isEmpty() )
            adapter[JSON_ENCRYPTION] = nullVal;

        wireless[JSON_ADAPTER] = adapter;

        QJsonObject auth;

        // wep
        if ( "0" == settings_["security"] )
        {
            int idx( wpaSupplicant_["wep_tx_keyidx"].toInt() );
            ++idx;

            if (( idx < 1 ) || ( 4 < idx ))
                LOG_WARN << "wep key index out of range " << idx;

            auth[JSON_DEFAULT_KEY] = idx;
            auth[JSON_KEY1] = wpaSupplicant_["wep_key0"];
            auth[JSON_KEY2] = wpaSupplicant_["wep_key1"];
            auth[JSON_KEY3] = wpaSupplicant_["wep_key2"];
            auth[JSON_KEY4] = wpaSupplicant_["wep_key3"];
        }
        // peap/leap
        else if (( "3" == settings_["security"] ) || ( "4" == settings_["security"] ) ||
            ( "7" == settings_["security"] ) || ( "8" == settings_["security"] ) ||
            ( "9" == settings_["security"] ) || ( "10" == settings_["security"] ))
        {
            auth[JSON_USERNAME] = settings_["eapUsername"];
            auth[JSON_PASSWORD] = settings_["eapPassword"];
        }
        // eap/tls
        else if (( "2" == settings_["security"] ) || ( "6" == settings_["security"] ))
        {
            auth[JSON_SERVER_CERT] = settings_["serverCertFilename"];
            auth[JSON_CLIENT_CERT] = settings_["clientCertFilename"];
            auth[JSON_CHAIN] = settings_["chainFilename"];
            auth[JSON_CSR] = settings_["csrFilename"];
            auth[JSON_CSR_KEY] = settings_["csrKeyFilename"];
            auth[JSON_CLIENT_KEY] = settings_["clientKeyFilename"];
            auth[JSON_CLIENT_CERT_USERS] = settings_["clientCertUsersFilename"];
            auth[JSON_CLIENT_CERT_PASSWORD] = settings_["clientCertPassword"];
        }
        // wpa
        else
        {
            auth[JSON_PASSPHRASE] = settings_["wpaPassphrase"];
        }

        wireless[JSON_AUTHENTICATION] = auth;

        network[JSON_WIRELESS] = wireless;
    }

    QJsonObject ipconfig;
    ipconfig[JSON_IS_STATIC] = ("1" == settings_["isStatic"]);
    ipconfig[JSON_DHCP_SERVER] = checkStringValueNull( settings_["DHCPHostname"], true );
    ipconfig[JSON_IP_ADDRESS] = checkStringValueNull( settings_["ipAddress"] );
    ipconfig[JSON_SUBNET_MASK] = checkStringValueNull( settings_["netmask"] );
    ipconfig[JSON_GATEWAY] = checkStringValueNull( settings_["gateway"] );

    network[JSON_IP_CONFIG] = ipconfig;

    QJsonObject dns;
    dns[JSON_AUTO_DNS] = ("1" == settings_["autoDNS"]);
    dns[JSON_DNS_SERVER1] = checkStringValueNull( settings_["dns1"] );
    dns[JSON_DNS_SERVER2] = checkStringValueNull( settings_["dns2"] );
    dns[JSON_DNS_SERVER3] = checkStringValueNull( settings_["dns3"] );
    dns[JSON_DNS_SUFFIX1] = checkStringValueNull( settings_["DNSSuffix1"] );
    dns[JSON_DNS_SUFFIX2] = checkStringValueNull( settings_["DNSSuffix2"] );
    dns[JSON_DNS_SUFFIX3] = checkStringValueNull( settings_["DNSSuffix3"] );
    dns[JSON_DNS_SUFFIX4] = checkStringValueNull( settings_["DNSSuffix4"] );
    dns[JSON_DNS_SUFFIX5] = checkStringValueNull( settings_["DNSSuffix5"] );
    dns[JSON_DNS_SUFFIX6] = checkStringValueNull( settings_["DNSSuffix6"] );

    network[JSON_DNS] = dns;

    // ---- //

    QJsonObject snmp;
    snmp[JSON_IS_ENABLED] = ("1" == settings_["snmpEnabled"]);
    snmp[JSON_VARIABLES_PORT] = settings_["snmpVariablesPort"].toInt();
    snmp[JSON_TRAPS_IP] = checkStringValueNull( settings_["snmpTrapsIP"] );
    snmp[JSON_TRAPS_PORT] = settings_["snmpTrapsPort"].toInt();

    // ---- //

    QJsonObject obj;
    obj[JSON_STORE] = store;

    if ( scaleMode_.length() )
        obj[JSON_SCALE_MODE] = scaleMode_;

    if ( scaleItemEntryType_.length() )
        obj[JSON_SCALE_ITEM_ENTRY_TYPE] = scaleItemEntryType_;

    if ( printerLabelStock_.length() )
        obj[JSON_LABEL_STOCK] = printerLabelStock_;

    obj[JSON_HAS_LOAD_CELL] = weigherExists_;

    if ( weigherExists_ )
    {
        QStringList formats;
        formats.append( "MMM dd, yy" );
        formats.append( "dd MMM yy" );

        if ( weigherPrecision_.length() )
            obj[JSON_WEIGHER_PRECISION] = weigherPrecision_;

        if ( weigherCalibTimestamp_.length() )
        {
            bool valid( false );

            // convert to ISO data
            foreach ( const QString& format, formats )
            {
                QDate d( QDate::fromString( weigherCalibTimestamp_, format ) );

                if ( d.isValid() )
                {
                    if ( d.year() < 1990 )
                        d = d.addYears( 100 );

                    obj[JSON_WEIGHER_CALIB_STAMP] = d.toString( Qt::ISODate );

                    valid = true;
                    break;
                }
            }

            if ( !valid )
            {
                LOG_WARN << "failed to convert weigher calibration stamp " << qPrintable( weigherCalibTimestamp_ );
                obj[JSON_WEIGHER_CALIB_STAMP] = weigherCalibTimestamp_;
            }
        }

        if ( weigherConfigTimestamp_.length() )
        {
            bool valid( false );

            // convert to ISO data
            foreach ( const QString& format, formats )
            {
                QDate d( QDate::fromString( weigherConfigTimestamp_, format ) );

                if ( d.isValid() )
                {
                    if ( d.year() < 1990 )
                        d = d.addYears( 100 );

                    obj[JSON_WEIGHER_CONF_STAMP] = d.toString( Qt::ISODate );

                    valid = true;
                    break;
                }
            }

            if ( !valid )
            {
                LOG_WARN << "failed to convert weigher configuration stamp " << qPrintable( weigherConfigTimestamp_ );
                obj[JSON_WEIGHER_CONF_STAMP] = weigherConfigTimestamp_;
            }
        }
    }

    obj[JSON_HAS_CUSTOMER_DISPLAY] = customerDisplayExists_;

    obj[JSON_TIME_ZONE] = tz.displayName( QTimeZone::GenericTime );
    obj[JSON_TIME_KEEPER] = settings_["timeKeeper"].trimmed().toUpper();
    obj[JSON_NTP] = ntp;
    obj[JSON_NETWORK] = network;
    obj[JSON_SNMP] = snmp;

    return obj;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool scaleSettings::ipAddress( QString& ipAddress4Result, QString& ipAddress6Result )
{
    QString macAddress;
    QString ipAddress4;
    QString ipAddress6;

    QJsonValue ipAddress4Val;
    QJsonValue ipAddress6Val;

    // wired
    if ( queryAdapterInfo( 0, macAddress, ipAddress4, ipAddress6 ) )
    {
        ipAddress4Val = checkStringValueNull( ipAddress4 );
        ipAddress6Val = checkStringValueNull( ipAddress6 );

        if (( !ipAddress4Val.isNull() ) || ( !ipAddress6Val.isNull() ))
        {
            ipAddress4Result = ipAddress4Val.toString();
            ipAddress6Result = ipAddress6Val.toString();

            return true;
        }
    }

    // wireless
    if ( queryAdapterInfo( 1, macAddress, ipAddress4, ipAddress6 ) )
    {
        ipAddress4Val = checkStringValueNull( ipAddress4 );
        ipAddress6Val = checkStringValueNull( ipAddress6 );

        if (( !ipAddress4Val.isNull() ) || ( !ipAddress6Val.isNull() ))
        {
            ipAddress4Result = ipAddress4Val.toString();
            ipAddress6Result = ipAddress6Val.toString();

            return true;
        }
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void scaleSettings::setStoreInformation( const QString& id, const QString& name, const QString& graphic )
{
    storeId_ = id;
    storeName_ = name;
    storeGraphic_ = graphic;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool scaleSettings::readScaleSettings()
{
#if defined(Q_OS_LINUX)
    static const QString file( "/usr/local/hobart/db/settingswizard/scalesettings" );
#elif defined(Q_OS_WIN32)
    static const QString file( /* "C:\\Program Files\\Hobart\\Db\\settingswizard\\" */ "scalesettings" );
#endif

    return scaleConfig::instance()->parseConfigFile( file, "=", settings_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool scaleSettings::readSupplicant()
{
#if defined(Q_OS_LINUX)
    static const QString file( "/etc/wpa_supplicant.conf" );
#elif defined(Q_OS_WIN32)
    static const QString file( /* "C:\\Program Files\\Hobart\\Db\\settingswizard\\" */ "NetKeys.txt" );
#endif

    return scaleConfig::instance()->parseConfigFile( file, "=", wpaSupplicant_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool scaleSettings::queryAdapterInfo( int type, QString& macAddress, QString& ipAddress4, QString& ipAddress6 )
{
#if defined(Q_OS_LINUX)
    static const QString ethernet( "eth0" );
    static const QString wifi( "wlan0" );
#elif defined(Q_OS_WIN32)
    static const QString ethernet( "Ethernet" );
    static const QString wifi( "Wi-Fi" );
#endif

    macAddress.clear();
    ipAddress4.clear();
    ipAddress6.clear();

    // loop through adapters to get mac address
    foreach ( const QNetworkInterface& networkInterface, QNetworkInterface::allInterfaces() )
    {
        if ( !networkInterface.isValid() )
            continue;
#if QT_VERSION >= 0x050b00
        else if (( 0 == type ) && ( QNetworkInterface::Ethernet != networkInterface.type() ))
            continue;
        else if (( 1 == type ) && ( QNetworkInterface::Wifi != networkInterface.type() ))
            continue;
#else
        else if ( QNetworkInterface::IsLoopBack & networkInterface.flags() )
            continue;
#endif

        LOG_DEBUG << qPrintable( networkInterface.name())  << " " << qPrintable( networkInterface.humanReadableName() );

        const QString name( networkInterface.humanReadableName() );

        if (( 0 == type ) && ( !name.contains( ethernet ) ))
            continue;
        else if (( 1 == type ) && ( !name.contains( wifi ) ))
            continue;

        macAddress = networkInterface.hardwareAddress();

        const QList<QNetworkAddressEntry> addrs( networkInterface.addressEntries() );

        foreach ( const QNetworkAddressEntry& addr, addrs )
        {
            const QHostAddress a( addr.ip() );

            if ( QAbstractSocket::IPv4Protocol == a.protocol() )
                ipAddress4 = a.toString();
            else if ( QAbstractSocket::IPv6Protocol == a.protocol() )
                ipAddress6 = a.toString();
        }

        return true;
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString scaleSettings::queryWiredConnectionStatus()
{
    QString result;

#if defined(Q_OS_LINUX)
    static const QString program( "mii-tool" );
    static const QString adapter( "eth0" );

    QStringList args;
    args.append( adapter );

    QProcess p;
    p.start( program, args );

    if ( p.waitForFinished() )
    {
        const QByteArray prefix( adapter.toLatin1() + ": " );
        const QByteArray a( p.readAllStandardOutput() );

        if ( a.startsWith( prefix ) )
        {
            result = QString::fromLatin1( a ).mid( prefix.length() );
            result = result.trimmed();
        }
    }
#endif

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString scaleSettings::queryWirelessConnectionStatus()
{
    QString result;

#if defined(Q_OS_LINUX)
    static const QString program( "iwconfig" );
    static const QString adapter( "wlan0" );

    QStringList args;
    args.append( adapter );

    QProcess p;
    p.start( program, args );

    if ( p.waitForFinished() )
    {
        const QByteArray prefix( "Link Quality" );
        const QByteArray a( p.readAllStandardOutput() );

        QTextStream s( a );

        // parse all lines
        while ( !s.atEnd() )
        {
            const QString line( s.readLine() );

            LOG_TRACE << qPrintable( line );

            if ( !line.contains( prefix ) )
                continue;

            result = line.trimmed();
            break;
        }
    }
#endif

    return result;
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
