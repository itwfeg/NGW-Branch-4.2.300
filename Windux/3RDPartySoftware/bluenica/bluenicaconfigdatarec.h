/**
 * @file bluenicaconfigdatarec.h
 * Bluenica sensor configuration data record.
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef BLUENICACONFIGDATAREC
#define BLUENICACONFIGDATAREC

#include "bluenicadatarec.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Bluenica sensor configuration data record.
class bluenicaConfigDataRec : public bluenicaDataRec
{
    typedef bluenicaConfigDataRec _Myt;
    typedef bluenicaDataRec _Mybase;

public:

    /// Device features.
    enum feature
    {
        ALWAYS_VISIBLE              = 0x01,
        ACCELEROMETER               = 0x02,
        ACCEL_WAKEUP                = 0x04,
        PRIORITY_DEVICE             = 0x08,
        ADV_LIVE_TEMP               = 0x10,
        WATCHDOG_DISABLED           = 0x20
    };

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] other  data to parse
     */
    bluenicaConfigDataRec( const leAdvertInfo &other );

    /// Constructor.
    /**
     * @param[in] other  object to copy
     */
    bluenicaConfigDataRec( const _Myt &other );

    /// Destructor.
    virtual ~bluenicaConfigDataRec();

    // ========================================================================
    // Operators
    // ========================================================================

    /// Assignment operator.
    /**
     * @param[in] rhs  object to copy
     * @return  reference to this
     */
    _Myt& operator = ( const _Myt &rhs );

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve record id.
    /**
     * @return  record id
     */
    virtual unsigned int recordId() const {return recordId_;}

    /// Retrieve temperature.
    /**
     * @return  degrees celsius
     */
    virtual float temperature() const {return temp_;}

    // ========================================================================
    // Methods
    // ========================================================================

    /// Print object data.
    virtual void print() const;

    // ========================================================================
    // Static Methods
    // ========================================================================

    /// Check data type.
    /**
     * @param[in] value  advert info
     * @return  @c true if type is of _Myt, @c false otherwise
     */
    static bool isTypeOf( const leAdvertInfo &value );

protected:

    unsigned int recordId_;                         ///< Data record ID.

    float temp_;                                    ///< Temperature value (in Celsius).

    feature features_;                              ///< Device features (bit flags).

    unsigned short advVisibilityTimeout_;           ///< Device configuration.

    unsigned short tempCheckItv_;                   ///< Device configuration.
    unsigned short tempPeriodicWriteItv_;           ///< Device configuration.
    float tempWriteOnDelta_;                        ///< Device configuration.

    unsigned int versionMajor_;                     ///< Major code version component.
    unsigned int versionMinor_;                     ///< Minor code version component.

    // ========================================================================
    // Methods
    // ========================================================================

    /// Copy object.
    /**
     * @param[in] other  object to copy
     */
    virtual void copy( const _Myt &other );

    /// Reset object.
    virtual void reset();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // BLUENICACONFIGDATAREC
