/**
 * @file hobartcanweigher.h
 * Hobart CAN Weigher Device.
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef HOBARTCANWEIGHER_H
#define HOBARTCANWEIGHER_H

#include "candev.h"

#include "../hobartweigher.h"

#include <cstddef>

class hobartCanDev;

///////////////////////////////////////////////////////////////////////////////////////////////////

// Hobart CAN Weigher Device.
class hobartCanWeigher : public hobartWeigher
{
    typedef hobartCanWeigher _Myt;
    typedef hobartWeigher _Mybase;

public:

    /// CAN Frame Id Range (min, numFrames)
    typedef canDev::rangeType rangeType;

    // ========================================================================
    // DTOR
    // ========================================================================

    /// Destructor.
    virtual ~hobartCanWeigher();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Set weigher mode.
    /**
     * @param[in] mode  mode
     */
    virtual void setWeigherMode( unsigned int mode );

    /// Set/Write Value Max Assembly Number Setting to the board .
    /**
     * @param[in] assemblyNum  the board assembly number
     */
    virtual void setValueMaxAssembly(QString assemblyNum );       // These are temp types - some likely wrong

    /// Set/Write Value Max Hardware Version Setting to the board .
    /**
     * @param[in] hardwareNum  the board hardware version number
     */
    virtual void setValueMaxHwVersion(QString hardwareNum );

    /// Set/Write Value Max Serial Number Setting to the board .
    /**
     * @param[in] serialNum  the board serial number
     */
    virtual void setValueMaxSerial( QString serialNum);

    /// Set/Write Value Max Date of Manufacturing value to the board .
    /**
     * @param[in] dateNum  the date of manufacturing
     */
    virtual void setValueMaxDateOfMfg(uint32_t dateNum );
    //virtual void setValueMaxDateOfMfg(QString dateNum );

    /// No params right now, dont know if I need 5 uints or 25 of them or a big string
    virtual void setValueMaxFitParameters(int32_t a1_g, int32_t a2_m, int32_t a3_zerovalue, int32_t a4_spanvalue, int32_t a5_Z0, int32_t a6_D, int32_t a7_x2, int32_t a8_x1, int32_t a9_x0, int32_t a10_y2, int32_t a11_y1, int32_t a12_y0, int32_t a13_xrot0, int32_t a14_xrot1, int32_t a15_xrot2, int32_t a16_yrot0, int32_t a17_yrot1, int32_t a18_yrot2, int32_t a19_zrot0, int32_t a20_zrot1, int32_t a21_zrot2, int32_t a22_xgain, int32_t a23_ygain, int32_t a24_gcoeff, int32_t a25_checksum);

    /// Retrieve device range.
    /**
     * @return  device range
     */
    virtual rangeType weigherDeviceRange() const;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Enable weigher.
    virtual void weigherEnable();

    /// Wakeup weigher.
    virtual void weigherWakeup();

    /// Zero weigher.
    virtual void weigherZero();

    /// Request Weigher Status Counts
    virtual void weigherStatusReq();

    /// Request Weigher Status Counts
    virtual void accelerometerStatusReq();

    /// Request Value Max Values
    virtual void weigherValueMaxReq();

    /// Request the entire accelerometer FIT data structure
    virtual void requestFitCalibration();

    /// Request the accelerometer manufacturing date
    virtual void requestDate();

    /// Request the accelerometer assembly number.
    virtual void requestAssemblyNumber();

    /// Request the accelerometer hardware version.
    virtual void requestHardwareVersion();

    /// Request the accelerometer serial number.
    virtual void requestSerialNumber();

protected:

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    hobartCanWeigher();

    // ========================================================================
    // Methods
    // ========================================================================

    /// Send data to device.
    /**
     * @param[in] id  can frame id
     * @param[in] buffer  data buffer
     * @param[in] bufferLen  data buffer length
     * @return  @c true upon success, @c false otherwise
     */
    virtual bool sendData( unsigned int id, const unsigned char *buffer, size_t bufferLen ) = 0;

    /// Receive message from device.
    /**
     * @param[in] id  can frame id
     * @param[in] buffer  data buffer
     * @param[in] bufferLen  data buffer length
     */
    virtual void recvWeigherMessage( unsigned int id, const unsigned char *buffer, size_t bufferLen );

    /// Send message to device.
    /**
     * @param[in] id  can frame id
     * @param[in] buffer  data buffer
     * @param[in] bufferLen  data buffer length
     * @return  @c true upon success, @c false otherwise
     */
    virtual bool sendWeigherMessage( unsigned int id, const unsigned char *buffer, size_t bufferLen );

    /// Register messages.
    /**
     * @param[in] dev  device
     */
    virtual void registerWeigherMessages( hobartCanDev *dev );

private:

    enum {CAN_ID = 0x400};
    enum {CAN_RANGE = 0x100};

    enum {VALMAX_ID = 0x420};
    enum {VALMAX_RANGE = 0x17};

    enum weigherMessage
    {
        wgWakeup = 0x400,
        wgReqWakeup = 0x401,
        wgStatus = 0x402,

        WgValueMaxRequest = 0x420,
        WgValueMaxAssem = 0x421,
        WgValueMaxWriteAssem = 0x423,
        WgValueMaxHwVersion = 0x425,
        WgValueMaxWriteHwVersion = 0x426,
        WgValueMaxSerial = 0x427,
        WgValueMaxWriteSerial = 0x429,
        WgValueMaxDate = 0x42b,                                         // 1067
        WgValueMaxWriteDate = 0x42c,                                    // 1068
        WgValueMaxFitParameters = 0x430,                                // 1072

        wgMode = 0x451,
        wgReqStatus = 0x452,                                            // NEW request accelerometer
        wgEnable = 0x454,
        wgRezero = 0x456,
        WgValueMaxWriteFitParameters = 0x461                            // 1121
    };

    enum ValueMaxReq
    {
        VMREQ_ASSEMBLY,             // 0x00
        VMREQ_HWVERSION,            // 0x01
        VMREQ_SERIALNUM,            // 0x02
        VMREQ_DATE,                 // 0x03
        VMREQ_FITPARAMS,            // 0x04             // was VMREQ_XGAINFACTOR
        VMREQ_YGAINFACTOR,          // 0x05
        VMREQ_ZGAINFACTOR,          // 0x06
        VMREQ_CALIBRATION,          // 0x07 - use this one for reading the thingy.
        VMREQ_CURRENTFACTORS,       // 0x08
    };

    enum FrameID
    {
        Frame1  = 1,
        Frame2,
        Frame3,
        Frame4,
        Frame5,
        Frame6,
        Frame7,
        Frame8,
        Frame9,
        Frame10,
        Frame11,
        Frame12,
        Frame13,
        Frame14,
        Frame15
    };

    // not implemented
    hobartCanWeigher( const _Myt& other );

    int status_ctr_;
    int fitbuf_ctr_;

    uint32_t rawDate_;

    // status message
    uint16_t flags_;
    uint32_t rawAtoDCounts_;
    int32_t nonZeroCalibCounts_;
    int32_t zeroCalibCounts_;
    int32_t acclXRawCounts_;
    int32_t acclYRawCounts_;
    int32_t acclZRawCounts_;
    int32_t acclXTiltDegrees_;
    int32_t acclYTiltDegrees_;
    int32_t calRigAcclXTiltDegrees_;
    int32_t calRigAcclYTiltDegrees_;

    // FIT Buffer
    int32_t a1_g_;
    int32_t a2_m_;
    int32_t a3_zerovalue_;
    int32_t a4_spanvalue_;
    int32_t a5_Z0_;
    int32_t a6_D_;
    int32_t a7_x2_;
    int32_t a8_x1_;
    int32_t a9_x0_;
    int32_t a10_y2_;
    int32_t a11_y1_;
    int32_t a12_y0_;
    int32_t a13_xrot0_;
    int32_t a14_xrot1_;
    int32_t a15_xrot2_;
    int32_t a16_yrot0_;
    int32_t a17_yrot1_;
    int32_t a18_yrot2_;
    int32_t a19_zrot0_;
    int32_t a20_zrot1_;
    int32_t a21_zrot2_;
    int32_t a22_xgain_;
    int32_t a23_ygain_;
    int32_t a24_gcoeff_;
    int32_t a25_checksum_;

    // not implemented
    _Myt & operator = ( const _Myt& rhs );

    /// Send basic message to weigher.
    bool sendWeigherMessageBasic( weigherMessage msgType );

    /// Send Value Max Request
    bool sendValueMaxMessageRequest( ValueMaxReq vmType );

    /// Verify the values written to the board against what I stored locally in the UI
    bool verifyWrittenValues();

    /// Displays all errors that can occur while writing and verifying the data written to the accelerometer
    void popErrorDialog(QString err_msg);

    /// convert any incoming reversed hex value to unsigned int and convert to int equivalent
    uint32_t hexToUnsignedInt(const unsigned char *bufferOG);

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // HOBARTCANWEIGHER_H
