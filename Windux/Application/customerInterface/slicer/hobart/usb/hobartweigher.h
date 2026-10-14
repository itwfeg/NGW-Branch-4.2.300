/**
 * @file hobartweigher.h
 * Hobart Weigher Device.
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef HOBARTWEIGHER_H
#define HOBARTWEIGHER_H

#include <cstddef>

#include <QMetaType>
#include <QString>
#include <QDebug>
#include <QtDebug>

#include "weigherconfiguration.h"

class hobartUsbDev;

class usbDevEndpoint;

///////////////////////////////////////////////////////////////////////////////////////////////////

// Hobart Weigher Device.
class hobartWeigher
{
    typedef hobartWeigher _Myt;

public:

    enum weigherMessage
    {
        wgWakeup = 0,
        wgWriteCfg,
        wgReqConfig,
        wgReset,
        wgStatus,
        wgEnable,
        wgDisable,
        wgMode,
        wgRezero,
        wgControl,
        wgColdReset = 10,
        wgCAT3ReqStatistics,
        wgCAT3AuditStatistics,
        wgCAT3DateTime,
        wgCAT3ReqPage,
        wgCAT3ReadRecords,
        wgCAT3ReqNextRecords,
        wgCAT3WriteRecord,
        wgCAT3RecordStatus,
        wgCAT3ReqEraseLog,
        wgCAT3LogEraseComplete,
        wgError,
        wgResetFactoryDefaults,
        wgResetFactoryDefaultsComplete
    };

    /// Weigher states.
    enum weigherState
    {
        zero,                                       ///< Weigher at zero.
        unstable,                                   ///< Weigher unstable.
        stable,                                     ///< Weigher at stable weight.
        underWeight,                                ///< Weigher under zero.
        overWeight,                                 ///< Weigher over weight limit.
        outOfMaint,                                 ///< Weigher out of maintenance zone.
        error                                       ///< Weigher error.
    };

    // ========================================================================
    // DTOR
    // ========================================================================

    /// Destructor.
    virtual ~hobartWeigher();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Set weigher mode.
    /**
     * @param[in] mode  mode
     */
    virtual void setWeigherMode( unsigned int mode );

    virtual void setReqConfigMode(unsigned int mode);

    virtual void setReqStatus(unsigned int mode);

    virtual void setWriteConfig(int state);

    virtual void sendColdReset();

    virtual void sendFactoryReset();

    virtual void reqWeigherConfig();

    //Write configuration from weigherconfiguration instance to weigher
    //virtual void slt_WriteConfig(WeigherConfiguration config);



    // ========================================================================
    // Methods
    // ========================================================================

    /// Enable weigher.
    virtual void weigherEnable();

    /// Wakeup weigher.
    virtual void weigherWakeup();

    /// Zero weigher.
    virtual void weigherZero();

Q_SIGNALS:

    /// Signal for weigher state changed.
    /**
     * @param[in] s  state
     * @param[in] weight  weight
     */
    virtual void weigherStateChanged( hobartWeigher::weigherState s, float weight ) = 0;
    /// Signal for weigher key pressed
    virtual void sig_WeigherKeyPressed() = 0;
    virtual void weigherConfigChanged(WeigherConfiguration & config) = 0;
    virtual void sig_WeigherStatus(char* buffer, int buffLen) = 0;

protected:

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    hobartWeigher();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve device logical address.
    /**
     * @return  logical address
     */
    virtual unsigned short logicalAddrWeigher() const {return LOGICAL_ADDR_WEIGHER;}

    /// Retrieve weigher interrupt in.
    /**
     * @return  weigher interrupt in
     */
    virtual usbDevEndpoint *weigherInterruptIn() const = 0;

    /// Retrieve weigher interrupt out.
    /**
     * @return  weigher interrupt out
     */
    virtual usbDevEndpoint *weigherInterruptOut() const = 0;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Send data to device.
    /**
     * @param[in] e  endpoint
     * @param[in] logicalAddr  device logical address
     * @param[in] buffer  data buffer
     * @param[in] bufferLen  data buffer length
     * @param[in] timeout  timeout (in millseconds) that this function should wait before giving up due to no response being received. For an unlimited timeout, use value 0
     * @return  number of bytes transferred
     */
    virtual size_t sendData( usbDevEndpoint *e, unsigned short logicalAddr, const unsigned char *buffer, size_t bufferLen, unsigned int timeout ) = 0;

    /// Receive message from device.
    /**
     * @param[in] buffer  data buffer
     * @param[in] bufferLen  data buffer length
     */
    virtual void recvWeigherMessage( const unsigned char *buffer, size_t bufferLen );

    /// Send message to device.
    /**
     * @param[in] buffer  data buffer
     * @param[in] bufferLen  data buffer length
     * @param[in] timeout  timeout (in millseconds) that this function should wait before giving up due to no response being received. For an unlimited timeout, use value 0
     * @return  number of bytes transferred
     */
    virtual size_t sendWeigherMessage( const unsigned char *buffer, size_t bufferLen, unsigned int timeout = XFER_TIMEOUT );

private:

    WeigherConfiguration weigherConfig;

    enum {LOGICAL_ADDR_WEIGHER = 0x0006};

    enum {XFER_TIMEOUT = 500};

    enum writeConfigState {
        wgConfigState1 = 1,
        wgConfigState2 = 2,
        wgConfigState3 = 3,

    };

    /// Send basic message to weigher.
    bool sendWeigherMessageBasic( weigherMessage msgType );

    // not implemented
    hobartWeigher( const _Myt& other );

    // not implemented
    _Myt & operator = ( const _Myt& rhs );


    QByteArray ba;
    QByteArray baWriteCfg;
    QString str;
    unsigned char* nBuffer;
    WeigherConfiguration * curerntConfig;
    bool isAvoir;
};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // HOBARTWEIGHER_H
