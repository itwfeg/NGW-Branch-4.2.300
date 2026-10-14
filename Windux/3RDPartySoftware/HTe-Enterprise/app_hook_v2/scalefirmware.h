/**
 * @file scalefirmware.h
 * Scale firmware parser object.
 *
 * @copyright Copyright (c) 2020 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef SCALEFIRMWARE_H
#define SCALEFIRMWARE_H

#include <QMap>
#include <QObject>
#include <QString>
#include <QStringList>

class QXmlStreamReader;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Scale settings.
class scaleFirmware : public QObject
{
    Q_OBJECT

    typedef scaleFirmware _Myt;
    typedef QObject _Mybase;

public:

    // ------------------------------------------------------------------------
    // CTOR / DTOR
    // ------------------------------------------------------------------------

    /// Constructor.
    /**
     * @param[in] parent  parent object
     */
    scaleFirmware( QObject *parent = nullptr );

    /// Destructor.
    virtual ~scaleFirmware();

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

    static QString buildNumber() { return buildNumber_; }

    /// Set application build number.
    /**
     * @param[in] value  build number
     */
    static void setBuildNumber( const QString& value ) {buildNumber_ = value;}

    QString versions(QString id) const;

    /// Read version information.
    /**
     * @return  @c true upon success, @c false otherwise.
     */
    virtual bool readVersionInfo();

    QStringList features() const { return features_; }
    QStringList licenses() const { return licenses_; }


protected:

    /// Map of scale settings.
    typedef QMap<QString, QString> stringMap;

    stringMap versions_;                            ///< Version information.

    QStringList features_;                          ///< Installed features.
    QStringList licenses_;                          ///< Installed licenses.

    // ------------------------------------------------------------------------
    // Methods
    // ------------------------------------------------------------------------

private:

    static QString buildNumber_;

    /// Retrieve OS version.
    static QString opersysVer();

    /// Retrieve SM Backend version.
    static QString smbackendVer();

    /// Retrieve UBOOT version.
    static QString ubootVer();

    /// Retrieve System Controller hardrware revision.
    static QString hardwareRev();

    /// Retrieve installed features.
    static QStringList installedFeatures();

    /// Retrieve installed licenses.
    static QStringList installedLicenses();

    /// Parse license xml.
    static QString parseLicense( QXmlStreamReader& xml );

    /// Parse license company xml.
    static QString parseLicenseCompany( QXmlStreamReader& xml );

    // not implemented
    scaleFirmware( const _Myt& ) = delete;

    // not implemented
    _Myt & operator = ( const _Myt& ) = delete;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // SCALEFIRMWARE_H
