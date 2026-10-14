/**
 * @file overlay.h
 * Interface class for overlays.
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef OVERLAY_H
#define OVERLAY_H

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Interface class for overlays.
class overlay
{
    typedef overlay _Myt;

public:

    /// Overlay states.
    enum stateType
    {
        Invalid,                            ///< Invalid or uninitialized.
        Ready,                              ///< Initialized and ready to activate.
        Active,                             ///< Active.
        Complete                            ///< Complete; ready to be finalized.
    };

    // ========================================================================
    // DTOR
    // ========================================================================

    /// Destructor.
    virtual ~overlay() {}

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve overlay state.
    /**
     * @return  state
     */
    virtual stateType overlayState() const {return state_;}

    // ========================================================================
    // Methods
    // ========================================================================

    /// Initialize overlay.
    /**
     * @return  @c true upon success, @c false otherwise
     */
    virtual bool initializeOverlay() {return false;}

    /// Finalize overlay.
    virtual void finalizeOverlay() {}

    /// Show overlay.
    virtual void showOverlay() {}

    /// Hide overlay.
    virtual void hideOverlay() {}

protected:

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    overlay() : state_( Invalid ) {}

    // ========================================================================
    // Properties
    // ========================================================================

    /// Set overlay state.
    /**
     * @return  state
     */
    virtual void setOverlayState( stateType value ) {state_ = value;}

private:

    stateType state_;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // OVERLAY_H
