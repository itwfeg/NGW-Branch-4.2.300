/**
 * @file scalesettings.h
 * Scale settings parser object.
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef SCALESETTINGS_H
#define SCALESETTINGS_H

#include <QMap>
#include <QObject>
#include <QString>

class QXmlStreamReader;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Scale settings.
class scaleSettings : public QObject
{
    Q_OBJECT

    typedef scaleSettings _Myt;
    typedef QObject _Mybase;

public:

    // ------------------------------------------------------------------------
    // CTOR / DTOR
    // ------------------------------------------------------------------------

    /// Constructor.
    /**
     * @param[in] parent  parent object
     */
    scaleSettings( QObject *parent = nullptr );

    /// Destructor.
    virtual ~scaleSettings();

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

    /// Set customer display exists.
    /**
     * @param[in] value  @c true if exists, @c false otherwise
     */
    static void setCustomerDisplayExists( bool value ) {customerDisplayExists_ = value;}

    /// Set printer label stock.
    /**
     * @param[in] value  stock size
     */
    static void setPrinterLabelStock( const QString& value ) {printerLabelStock_ = value;}

    /// Set scale item entry type.
    /**
     * @param[in] value  item entry type
     */
    static void setScaleItemEntryType( const QString& value ) {scaleItemEntryType_ = value;}

    /// Set scale mode.
    /**
     * @param[in] value  mode
     */
    static void setScaleMode( const QString& value ) {scaleMode_ = value;}

    /// Set store information.
    /**
     * @param[in] id  store id
     * @param[in] name  store name
     * @param[in] graphic  store graphic
     */
    static void setStoreInformation( const QString& id, const QString& name, const QString& graphic );

    /// Set weigher calibration timestamp.
    /**
     * @param[in] value  timestamp
     */
    static void setWeigherCalibTimestamp( const QString& value ) {weigherCalibTimestamp_ = value.trimmed();}

    /// Set weigher configuration timestamp.
    /**
     * @param[in] value  timestamp
     */
    static void setWeigherConfigTimestamp( const QString& value ) {weigherConfigTimestamp_ = value.trimmed();}

    /// Set weigher exists.
    /**
     * @param[in] value  @c true if exists, @c false otherwise
     */
    static void setWeigherExists( bool value ) {weigherExists_ = value;}

    /// Set weigher precision.
    /**
     * @param[in] value  precision
     */
    static void setWeigherPrecision( const QString& value ) {weigherPrecision_ = value;}

protected:

    /// Map of scale settings.
    typedef QMap<QString, QString> stringMap;

    stringMap settings_;                            ///< Scale settings.
    stringMap wpaSupplicant_;                       ///< WPA Supplicant config.

    static QString storeId_;                        ///< Store id.
    static QString storeName_;                      ///< Store name.
    static QString storeGraphic_;                   ///< Store graphic.

    static QString scaleMode_;                      ///< Current mode.
    static QString scaleItemEntryType_;             ///< Current item entry type.

    static QString printerLabelStock_;              ///< Printer label stock.

    static bool weigherExists_;                     ///< Weigher exists or not.
    static QString weigherPrecision_;               ///< Weigher precision.
    static QString weigherCalibTimestamp_;          ///< Weigher calibration timestamp.
    static QString weigherConfigTimestamp_;         ///< Weigher configuration timestamp.

    static bool customerDisplayExists_;             ///< Customer display exists or not.

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
    scaleSettings( const _Myt& ) = delete;

    // not implemented
    _Myt & operator = ( const _Myt& ) = delete;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // SCALESETTINGS_H
