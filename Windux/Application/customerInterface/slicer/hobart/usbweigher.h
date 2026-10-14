/**
 * @file usbweigher.h
 * Hobart Weigher Adapter class.
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef USBWEIGHER_H
#define USBWEIGHER_H

#include <QMap>
#include <QObject>
#include <QThread>

#include "../scaleinterface.h"

#include "usb/hobartweigher.h"

class usbDevManager;
class usbDev;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Hobart Weigher Adapter.
class HobartUsbWeigher : public QObject, public ScaleInterface
{
    Q_OBJECT

    typedef HobartUsbWeigher _Myt;
    typedef QObject _Mybase;

    typedef ScaleInterface _Myimpl;

Q_SIGNALS:

    /// Signal for weight updated.
    /**
     * @param[in] state  weigher state
     * @param[in] weight  weight in lbs
     */
    void sig_weightUpdated( ScaleInterface::State state, ScaleInterface::weightInPounds_t weight );
    void sig_OnWeigherKeyPressed();
    void sig_ReqConfig();
    void sig_WeigherConfigChanged(WeigherConfiguration & config);
    void sig_WriteConfig(WeigherConfiguration & config);
    void sig_WeigherStatus(char* buffer, int buffLen);
    void sig_SetWeigherMode(WeighingMode mode);
    void sig_WeigherReset();

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent object
     */
    HobartUsbWeigher( QObject *parent = 0 );

    /// Destructor.
    virtual ~HobartUsbWeigher();

public Q_SLOTS:

    // ========================================================================
    // Methods
    // ========================================================================

    /// Zero weigher.
    virtual void ZeroScale();

    void slt_ReqConfig();
    void slt_WriteConfig(WeigherConfiguration & config);
    void slt_SetWeigherMode(WeighingMode mode);
    void slt_WeigherReset();


private Q_SLOTS:

    /// Slot for device arrived.
    void onDeviceArrived( usbDev* device );

    /// Slot for device left.
    void onDeviceLeft( usbDev* device );

    /// Slot for weigher init.
    void onWeigherInit();

    /// Slot for weigher state changed.
    void onWeigherStateChanged( hobartWeigher::weigherState s, float weight );

    //slot for weigher config changed
    void onWeigherConfigChanged(WeigherConfiguration &config);

    /// Slot for weigher Key Pressed.
    void slt_OnWeigherKeyPressed();
    void slt_WeigherStatus(char* buffer, int buffLen);

private:

    QMap<hobartWeigher::weigherState,State> stateMap_;

    usbDevManager *usbMan_;

    hobartWeigher *weigher_;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // USBWEIGHER_H
