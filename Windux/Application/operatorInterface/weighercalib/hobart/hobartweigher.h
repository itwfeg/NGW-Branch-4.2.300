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

#include <stdint.h>
#include <QVector>

#include <QMetaType>

// Weigher status bits.
#define SMALL_MOTION                            0x0001
#define LARGE_MOTION                            0x0002
#define NEGATIVE_GROSS_WEIGHT                   0x0004
#define WITHIN_QUARTER_GRADUATION_OF_ZERO       0x0008
#define OVER_GROSS_WEIGHT_LIMIT                 0x0010
#define ZERO_OUT_OF_MAINTENANCE_ZONE            0x0020
#define SCALE_WARMUP                            0x0040
#define NEW_ZERO                                0x0080
#define WEIGHER_CALIBRATION_FAILED              0x0100
#define WEIGHER_EEP_READ_FAILED                 0x0200
#define WEIGHER_DISABLED                        0x0400
#define WEIGHER_KEY_PRESSED                     0x0800
#define WEIGHER_ACCL_COMM_ON                    0x4000

///////////////////////////////////////////////////////////////////////////////////////////////////

// Hobart Weigher Device.
class hobartWeigher
{

    typedef hobartWeigher _Myt;

public:

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
    virtual void setWeigherMode( unsigned int mode ) = 0;       // This isnt used anywhere.  Should it?

    /// Set/Write Value Max Assembly Number Setting to the board .
    /**
     * @param[in] assemblyNum  the board assembly number
     */
    virtual void setValueMaxAssembly( QString assemblyNum ) = 0;       // These are temp types - some likely wrong

    /// Set/Write Value Max Hardware Version Setting to the board .
    /**
     * @param[in] hardwareNum  the board hardware version number
     */
    virtual void setValueMaxHwVersion( QString hardwareNum ) = 0;

    /// Set/Write Value Max Serial Number Setting to the board .
    /**
     * @param[in] serialNum  the board serial number
     */
    virtual void setValueMaxSerial( QString serialNum) = 0;

    /// No params right now, dont know if I need 5 uints or 25 of them or a big string
    virtual void setValueMaxFitParameters(int32_t a1_g, int32_t a2_m, int32_t a3_zerovalue, int32_t a4_spanvalue, int32_t a5_Z0, int32_t a6_D, int32_t a7_x2, int32_t a8_x1, int32_t a9_x0, int32_t a10_y2, int32_t a11_y1, int32_t a12_y0, int32_t a13_xrot0, int32_t a14_xrot1, int32_t a15_xrot2, int32_t a16_yrot0, int32_t a17_yrot1, int32_t a18_yrot2, int32_t a19_zrot0, int32_t a20_zrot1, int32_t a21_zrot2, int32_t a22_xgain, int32_t a23_ygain, int32_t a24_gcoeff, int32_t a25_checksum) = 0;

    /// Set/Write Value Max Date of Manufacturing value to the board .
    /**
     * @param[in] dateNum  the date of manufacturing
     */
    virtual void setValueMaxDateOfMfg( uint32_t dateNum ) = 0;
    //virtual void setValueMaxDateOfMfg( QString dateNum ) = 0;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Enable weigher.
    virtual void weigherEnable() = 0;

    /// Wakeup weigher.
    virtual void weigherWakeup() = 0;

    /// Zero weigher.
    virtual void weigherZero() = 0;

    /// Request Weigher Status Counts
    virtual void weigherStatusReq() = 0;

    /// Request Weigher Status Counts
    virtual void accelerometerStatusReq() = 0;

    /// Request Value Max Values
    virtual void weigherValueMaxReq() = 0;

    /// Request the entire accelerometer FIT data structure
    virtual void requestFitCalibration() = 0;

    /// Request Date
    virtual void requestDate() = 0;

    /// Request the accelerometer assembly number.
    virtual void requestAssemblyNumber() = 0;

    /// Request the accelerometer hardware version.
    virtual void requestHardwareVersion() = 0;

    /// Request the accelerometer serial number.
    virtual void requestSerialNumber() = 0;


Q_SIGNALS:

    /// Signal for weigher state changed.
    /**
     * @param[in] s  state
     * @param[in] weight  weight
     */
    virtual void weigherStateChanged( hobartWeigher::weigherState s, float weight ) = 0;

    /// Signal to the UI the X,Y, and Z Gain Factor changed
    /**
     * @param[in] uint32_t mfgDate
     */
    virtual void updateValueMaxDate( uint32_t mfgDate ) = 0;

    /// Signal to the UI the X,Y, and Z Gain Factor changed
    /**
     * @param[in] QString assemblyNumber
     */
    virtual void updateValueMaxAssemblyNumber( QString assemNum ) = 0;

    /// Signal to the UI the X,Y, and Z Gain Factor changed
    /**
     * @param[in] QString hardwareVersion
     */
    virtual void updateValueMaxHWVersion( QString hwVersion ) = 0;

    /// Signal to the UI the serial number changed
    /**
     * @param[in] QString serialNumber
     */
    virtual void updateValueMaxSerialNumber( QString serialNum ) = 0;

    /// Signal for weigher state changed.
    /**
     * @param[in] s  state
     * @param[in] weight  weight
     * ADD NEW ONES
     */
    virtual void updateValueMaxWeigherStatus(uint16_t flags, uint32_t rawAtoDCounts, int32_t nonZeroCalibCounts, int32_t zeroCalibCounts, int32_t acclXRawCounts, int32_t acclYRawCounts, int32_t acclZRawCounts, int16_t acclXTiltDegrees, int16_t acclYTiltDegrees ) = 0;

    /// Signal that the fit parameters changed.
    /**
     * @param[in]   fitBuffer The buffer that contains all of the information for the FIT Parameters
     */
    virtual void updateValueMaxFitParameters(int32_t a1_g, int32_t a2_m, int32_t a3_zerovalue, int32_t a4_spanvalue, int32_t a5_Z0, int32_t a6_D, int32_t a7_x2, int32_t a8_x1, int32_t a9_x0, int32_t a10_y2, int32_t a11_y1, int32_t a12_y0, int32_t a13_xrot0, int32_t a14_xrot1, int32_t a15_xrot2, int32_t a16_yrot0, int32_t a17_yrot1, int32_t a18_yrot2, int32_t a19_zrot0, int32_t a20_zrot1, int32_t a21_zrot2, int32_t a22_xgain, int32_t a23_ygain, int32_t a24_gcoeff, int32_t a25_checksum) = 0;

protected:

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    hobartWeigher();

    // ========================================================================
    // Methods
    // ========================================================================

    /// Handle Value Max Accelerometer Manufacturing Date
    /**
     * @param[in]   mfgDate The date the board was calibrated
     */
    void handleValueMaxDate(uint32_t mfgDate);

    /// Handle Value Max Accelerometer Assembly Number
    /**
     * @param[in]   AssemNum The Board Assembly Number of the board was calibrated
     */
    void handleValueMaxAssemblyNumber(QString assemNum);

    /// Handle Value Max Accelerometer Hardware Version
    /**
     * @param[in]   hwversion The Hardware Version of the board was calibrated
     */
    void handleValueMaxHWVersion(QString hwVersion);

    /// Handle Value Max Accelerometer Serial Number
    /**
     * @param[in]   serialNum The Serial Number of the board was calibrated
     */
    void handleValueMaxSerialNumber(QString serialNum);

    /// Handle Value Max Accelerometer Serial Number
    /**
     * @param[in]   fitBuffer The buffer that contains all of the information for the FIT Parameters
     */
    void handleValueMaxFitParameters(int32_t a1_g, int32_t a2_m, int32_t a3_zerovalue, int32_t a4_spanvalue, int32_t a5_Z0, int32_t a6_D, int32_t a7_x2, int32_t a8_x1, int32_t a9_x0, int32_t a10_y2, int32_t a11_y1, int32_t a12_y0, int32_t a13_xrot0, int32_t a14_xrot1, int32_t a15_xrot2, int32_t a16_yrot0, int32_t a17_yrot1, int32_t a18_yrot2, int32_t a19_zrot0, int32_t a20_zrot1, int32_t a21_zrot2, int32_t a22_xgain, int32_t a23_ygain, int32_t a24_gcoeff, int32_t a25_checksum);

    /// Handle weigher status.
    /**
     * @param[in] flags  flags
     * @param[in] counts  counts
     * @param[in] rawCounts  raw counts
     */
    void handleWeigherStatus( uint16_t flags, uint32_t rawAtoDCounts, int32_t nonZeroCalibCounts, int32_t zeroCalibCounts, int32_t acclXRawCounts, int32_t acclYRawCounts, int32_t acclZRawCounts, int16_t acclXTiltDegrees, int16_t acclYTiltDegrees );

private:

    // not implemented
    hobartWeigher( const _Myt& other );

    // not implemented
    _Myt & operator = ( const _Myt& rhs );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // HOBARTWEIGHER_H
