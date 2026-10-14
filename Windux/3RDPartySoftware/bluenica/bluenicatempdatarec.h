/**
 * @file bluenicatempdatarec.h
 * Bluenica temperature sensor data record.
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef BLUENICATEMPDATAREC
#define BLUENICATEMPDATAREC

#include "bluenicadatarec.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Bluenica temperature sensor data record.
class bluenicaTempDataRec : public bluenicaDataRec
{
    typedef bluenicaTempDataRec _Myt;
    typedef bluenicaDataRec _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] other  data to parse
     */
    bluenicaTempDataRec( const leAdvertInfo &other );

    /// Constructor.
    /**
     * @param[in] other  object to copy
     */
    bluenicaTempDataRec( const _Myt &other );

    /// Destructor.
    virtual ~bluenicaTempDataRec();

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

    /// Retrieve age.
    /**
     * @return  record age (in seconds)
     */
    virtual unsigned int age() const {return age_;}

    /// Retrieve battery level.
    /**
     * @return  percent
     */
    virtual unsigned int batteryLevel() const {return batteryLevel_;}

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

    unsigned int batteryLevel_;                     ///< Battery level (%).

    unsigned int clock_;                            ///< Device clock value at the time the record has been created, in seconds.
    unsigned int age_;                              ///< Data age (time since this record has been created), updated every 10 seconds.

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

#endif // BLUENICATEMPDATAREC
