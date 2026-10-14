/**
 * @file kpc.h
 * Hobart KPC CAN Device.
 *
 * @copyright Copyright (c) 2017 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef KPC_H
#define KPC_H

#include "hobartcandev.h"
#include "hobartcanprinter.h"
#include "hobartcanweigher.h"

class canSocket;

///////////////////////////////////////////////////////////////////////////////////////////////////

// Hobart KPC CAN Device.
class kpc : public hobartCanDev, public hobartCanPrinter, public hobartCanWeigher
{
    Q_OBJECT

    typedef kpc _Myt;
    typedef hobartCanDev _Mybase;

Q_SIGNALS:

    /// Signal for weigher state changed.
    /**
     * @param[in] s  state
     * @param[in] weight  weight
     */
    void weigherStateChanged( hobartWeigher::weigherState s, float weight );

    /// Signal that the Manufacturing date changed.
    /**
     * @param[in] mfgDate  Manufacturing Date
     */
    void updateValueMaxDate(uint32_t mfgDate);

    /// Signal that the assembly number changed.
    /**
     * @param[in] assemNum  Board Assembly Number
     */
    void updateValueMaxAssemblyNumber(QString assemNum);

    /// Signal that the hardware version changed.
    /**
     * @param[in] hwversion     Hardware Version
     */
    void updateValueMaxHWVersion(QString hwVersion);

    /// Signal that the serial number changed.
    /**
     * @param[in] serialNum     Serial Number
     */
    void updateValueMaxSerialNumber(QString serialNum);

    /// Signal for weigher state changed.
    /**
     * @param[in] s  state
     * @param[in] weight  weight
     */
    void updateValueMaxWeigherStatus(uint16_t flags, uint32_t rawAtoDCounts, int32_t nonZeroCalibCounts, int32_t zeroCalibCounts, int32_t acclXRawCounts, int32_t acclYRawCounts, int32_t acclZRawCounts, int16_t acclXTiltDegrees, int16_t acclYTiltDegrees );


    /// Signal that the fit parameters changed.
    /**
     * @param[in]   fitBuffer The buffer that contains all of the information for the FIT Parameters
     */
    void updateValueMaxFitParameters(int32_t a1_g, int32_t a2_m, int32_t a3_zerovalue, int32_t a4_spanvalue, int32_t a5_Z0, int32_t a6_D, int32_t a7_x2, int32_t a8_x1, int32_t a9_x0, int32_t a10_y2, int32_t a11_y1, int32_t a12_y0, int32_t a13_xrot0, int32_t a14_xrot1, int32_t a15_xrot2, int32_t a16_yrot0, int32_t a17_yrot1, int32_t a18_yrot2, int32_t a19_zrot0, int32_t a20_zrot1, int32_t a21_zrot2, int32_t a22_xgain, int32_t a23_ygain, int32_t a24_gcoeff, int32_t a25_checksum);

public:

    // ========================================================================
    // CTOR/DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] s  can socket
     */
    kpc( canSocket *s );

    /// Destructor.
    virtual ~kpc();

protected:

    // ========================================================================
    // Methods
    // ========================================================================

    /// Receive data from device.
    /**
     * @param[in] id  can frame id
     * @param[in] buffer  data buffer
     * @param[in] bufferLen  data buffer length
     */
    virtual void recvData( unsigned int id, const unsigned char *buffer, size_t bufferLen );

    /// Send data to device.
    /**
     * @param[in] id  can frame id
     * @param[in] buffer  data buffer
     * @param[in] bufferLen  data buffer length
     * @return  @c true upon success, @c false otherwise
     */
    virtual bool sendData( unsigned int id, const unsigned char *buffer, size_t bufferLen );

private:

    // not implemented
    kpc();

    // not implemented
    kpc( const _Myt& other );

    // not implemented
    _Myt & operator = ( const _Myt& rhs );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // KPC_H
