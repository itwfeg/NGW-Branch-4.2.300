/**
 * @file custgstmp.h
 * Customer media player for gstreamer.
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef CUSTGSTMP_H
#define CUSTGSTMP_H

#include "gstmp.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Customer media player for gstreamer.
class custgstmp : public gstmp
{
    typedef custgstmp _Myt;
    typedef gstmp _Mybase;

public:

    /// DBUS service name.
    static const QString DBUS_SERVICE_NAME;

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent object
     */
    custgstmp( QObject *parent = NULL );

    /// Destructor.
    virtual ~custgstmp();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve DBUS service name.
    /**
     * @return  path name
     */
    virtual QString dbusServiceName() const {return DBUS_SERVICE_NAME;}

    /// Retrieve framebuffer device.
    /**
     * @return  framebuffer
     */
    virtual QString fb() const;

    /// Retrieve v4l2 device.
    /**
     * @return  v4l2 device
     */
    virtual QString v4l2dev() const;

private:

    // not implemented
    _Myt& operator = ( const _Myt &value );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // CUSTGSTMP_H
