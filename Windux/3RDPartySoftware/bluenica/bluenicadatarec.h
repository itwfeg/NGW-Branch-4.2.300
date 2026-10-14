/**
 * @file bluenicadatarec.h
 * Abstract Bluenica sensor data record.
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef BLUENICADATAREC
#define BLUENICADATAREC

#include <string>

class leAdvertInfo;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Abstract Bluenica sensor data record.
class bluenicaDataRec
{
    typedef bluenicaDataRec _Myt;

public:

    // ========================================================================
    // DTOR
    // ========================================================================

    /// Destructor.
    virtual ~bluenicaDataRec();

    // ========================================================================
    // Operators
    // ========================================================================

    /// Retrieve device address.
    /**
     * @return  address
     */
    virtual std::string address() const {return address_;}

    /// Retrieve sender address.
    /**
     * @return  address of sender (i.e. device or repeater)
     */
    virtual std::string senderAddress() const {return senderAddress_;}

    /// Retrieve timestamp.
    /**
     * @return  seconds since epoch
     */
    virtual unsigned int timestamp() const {return timestamp_;}

    // ========================================================================
    // Methods
    // ========================================================================

    /// Print object data.
    virtual void print() const;

    // ========================================================================
    // Static Methods
    // ========================================================================

    /// Check if Bluenica sensor.
    /**
     * @param[in] value  advert info
     * @return  @c true if Bluenica sensor, @c false otherwise
     */
    static bool isBluenicaSensor( const leAdvertInfo &value );

    /// Factory method for decoding Bluenica sensor data.
    /**
     * @warning
     * Some sensor readings will not decodable.  This method will only decode the sensors that
     * broadcast unencrypted partner sensor information.
     *
     * @param[in] value  advert info
     * @return  pointer to sensor upon success, @c NULL otherwise
     */
    static _Myt *decode( const leAdvertInfo &value );

protected:

    time_t timestamp_;                              ///< Record timestamp.

    std::string senderAddress_;                     ///< Sender address (i.e. device or repeater).
    std::string address_;                           ///< Device address.

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    bluenicaDataRec();

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

    // ========================================================================
    // Static Methods
    // ========================================================================

    /// Decrypt EIR.
    /**
     * @param[in,out] eir  eir to decrypt
     */
    static void decrypt( std::string &eir );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // BLUENICADATAREC
