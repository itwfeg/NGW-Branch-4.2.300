/**
 * @file scanneragent.h
 * Scanner agent class.
 *
 * @copyright Copyright (c) 2018 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef SCANNERAGENT
#define SCANNERAGENT

#include <QObject>
#include <QString>

class QBluetoothDeviceInfo;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Scanner agent for bluetooth.
class scannerAgent : public QObject
{
    Q_OBJECT

    typedef scannerAgent _Myt;
    typedef QObject _Mybase;

signals:

    /// Signal for when device discovered.
    /**
     * @param[in] info  device info
     */
    void deviceDiscovered( const QBluetoothDeviceInfo &info );

    /// Signal for scanning started.
    void scanningStarted();

    /// Signal for scanning stopped.
    void scanningStopped();

public:

    /// Scan types.
    enum scanType
    {
        SCAN_TYPE_DEFAULT,                          ///< Default BLE scan.
        SCAN_TYPE_ACTIVE,                           ///< Active BLE scan.
        SCAN_TYPE_PASSIVE                           ///< Passive BLE scan.
    };

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Destructor.
    virtual ~scannerAgent();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve error string.
    /**
     * @return  error string
     */
    virtual QString errorString() const;

    /// Retrieve if scanning is active.
    /**
     * @return  @c true if scanning active, @c false otherwise
     */
    virtual bool isScanningActive() const = 0;

public slots:

    // ========================================================================
    // Methods
    // ========================================================================

    /// Start scanning.
    /**
     * @param[in] type  scan type
     */
    virtual void startScanning( scanType type = SCAN_TYPE_DEFAULT ) = 0;

    /// Stop scanning.
    virtual void stopScanning() = 0;

protected:

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent object
     */
    scannerAgent( QObject *parent = 0 );

    // ========================================================================
    // Properties
    // ========================================================================

    /// Set error string.
    /**
     * @param[in] value  error string
     */
    virtual void setErrorString( const QString &value );

    // ========================================================================
    // Methods
    // ========================================================================

    /// Reset error string.
    virtual void resetErrorString();

private:

    bool error_;

    QString errorMessage_;

    // not implemented
    scannerAgent( const _Myt & );

    // not implemented
    _Myt & operator = ( const _Myt & );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // SCANNERAGENT

