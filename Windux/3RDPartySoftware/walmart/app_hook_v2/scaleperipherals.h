/**
 * @file scaleperipherals.h
 * Scale peripheral version information.
 *
 * @copyright Copyright (c) 2021 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef SCALEPERIPHERALS_H
#define SCALEPERIPHERALS_H

#include <QMap>
#include <QMutex>
#include <QObject>
#include <QJsonArray>
#include <QJsonObject>

class QTimer;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Scale peripheral version information.
class scalePeripherals : public QObject
{
    Q_OBJECT

    typedef scalePeripherals _Myt;
    typedef QObject _Mybase;

public:

    // ------------------------------------------------------------------------
    // Methods
    // ------------------------------------------------------------------------

    /// Update product usage.
    /**
     * @param[in] dev  device
     * @param[in] devex  extended device information
     * @param[in] kind  device kind
     * @param[in] swVer  software version
     * @param[in] hwVer  hardware version
     */
    void updatePeripheralList( int dev, int devex, int kind, const QString& swVer, const QString& hwVer );

    /// Convert to @c QJsonArray.
    /**
     * @return  json array
     */
    QJsonArray toJsonArray() const;

    // ------------------------------------------------------------------------
    // Static Methods
    // ------------------------------------------------------------------------

    /// Retrieve global instance.
    /**
     * @return  pointer to instance
     */
    static _Myt *instance();

private slots:

    /// Request peripheral versions.
    void requestPeripheralVersions();

private:

    enum {REQUEST_TIME_INITIAL = 20 * 1000};    // 20s
    enum {REQUEST_TIME = 15 * 60 * 1000};       // 15m

    enum {DELAY_TIME = 2 * 1000};               // 2s

    enum PeripheralType
    {
        WEIGHER = 2,
        LABEL_APPLIER = 5,
        PRINTER = 8,
        WRAPPER = 10,
        NUMERIC = 142,
        MARQUEE = 143,
        GCD = 161,
    };

    typedef QMap<int, QString> deviceStringMap;
    typedef QMap<int, QJsonObject> deviceMap;

    static QMutex instanceMutex_;
    static _Myt *instance_;

    mutable QMutex m_;

    QTimer *requestTimer_;

    deviceStringMap deviceDescriptions_;
    deviceStringMap deviceKinds_;

    deviceMap devices_;

    /// Constructor.
    scalePeripherals( QObject *parent = nullptr );

    /// Destructor.
    ~scalePeripherals();

    /// Retrieve device description.
    QString deviceDescription( PeripheralType dev, int devex ) const;

    /// Retrieve device kind.
    QString deviceKind( PeripheralType dev, int kind ) const;

    // not implemented
    scalePeripherals( const _Myt& ) = delete;

    // not implemented
    _Myt & operator = ( const _Myt& ) = delete;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // SCALEPERIPHERALS_H
