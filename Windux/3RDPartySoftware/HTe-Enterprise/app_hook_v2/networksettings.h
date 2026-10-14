/**
 * @file scalesettings.h
 * Scale settings parser object.
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef NETWORKSETTINGS_H
#define NETWORKSETTINGS_H

#include <QMap>
#include <QObject>
#include <QString>

class QXmlStreamReader;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Scale settings.
class networkSettings : public QObject
{
    Q_OBJECT

    typedef networkSettings _Myt;
    typedef QObject _Mybase;

public:

    // ------------------------------------------------------------------------
    // CTOR / DTOR
    // ------------------------------------------------------------------------

    /// Constructor.
    /**
     * @param[in] parent  parent object
     */
    networkSettings( QObject *parent = nullptr );

    /// Destructor.
    virtual ~networkSettings();

    // ------------------------------------------------------------------------
    // Properties
    // ------------------------------------------------------------------------

    /// Check of valid.
    /**
     * @return  @c true if valid, @c false otherwise
     */
    virtual bool isValid() const;

    // ------------------------------------------------------------------------
    // Methods
    // ------------------------------------------------------------------------

    /// Convert to @c QJsonObject.
    /**
     * @return  json object
     */
    virtual QJsonObject toJsonObject() const;

    // ------------------------------------------------------------------------
    // Static Properties
    // ------------------------------------------------------------------------

    /// Retrieve adapter address.
    /**
     * Get address of active adapter.
     * @param[out] ipAddress4  ip 4 address
     * @param[out] ipAddress6  ip 6 address
     * @return  @c true if address found, @c false otherwise.
     */
    static bool ipAddress( QString& ipAddress4, QString& ipAddress6 );


protected:

    /// Map of scale settings.
    typedef QMap<QString, QString> stringMap;

    stringMap settings_;                            ///< Network settings.
    stringMap wpaSupplicant_;                       ///< WPA Supplicant config.


    // ------------------------------------------------------------------------
    // Methods
    // ------------------------------------------------------------------------

    /// Read scale settings from file.
    /**
     * @return  @c true upon success, @c false otherwise.
     */
    virtual bool readScaleSettings();

    /// Read config from wpa supplicant (wpa keys)
    /**
     * @return  @c true upon success, @c false otherwise.
     */
    virtual bool readSupplicant();

private:

    stringMap connectionTypes_;

    stringMap securityMethods_;

    stringMap openEncryptionTypes_;
    stringMap wpaEncryptionTypes_;
    stringMap eapEncryptionTypes_;

    /// Retrieve adapter information.
    static bool queryAdapterInfo( int type, QString& macAddress, QString& ipAddress4, QString& ipAddress6 );

    /// Retrieve wired connection status.
    static QString queryWiredConnectionStatus();

    /// Retrieve wireless connection status.
    static QString queryWirelessConnectionStatus();

    /// Check for null string value.
    static QJsonValue checkStringValueNull( const QString& value, bool zeroIsNull = false );

    // not implemented
    networkSettings( const _Myt& ) = delete;

    // not implemented
    _Myt & operator = ( const _Myt& ) = delete;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // NETWORKSETTINGS_H
