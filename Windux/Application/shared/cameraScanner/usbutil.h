#ifndef USBUTIL_H
#define USBUTIL_H

///////////////////////////////////////////////////////////////////////////////////////////////////

/// USB Utilities.
class usbUtil
{
    typedef usbUtil _Myt;

public:

    // ========================================================================
    // Static Properties
    // ========================================================================

    /// Check for device attached to system.
    /**
     * @param[in] vid  usb vendor id
     * @param[in] pid  usb product id
     * @return  @c true if attached, @c false otherwise
     */
    static bool isAttachedToSystem( unsigned short vid, unsigned short pid );

private:

    // not implemented
    usbUtil();

    // not implemented
    _Myt& operator = ( const _Myt& rhs );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // USBUTIL_H
