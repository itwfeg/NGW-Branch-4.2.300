/**
 * @file servicemodeloginpanel.h
 * Panel for logging into service mode.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef SERVICEMODELOGINPANEL_H
#define SERVICEMODELOGINPANEL_H

#include "loginpanel.h"

class ServiceMode;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Panel for logging into service mode.
class ServiceModeLoginPanel : public LoginPanel
{
    Q_OBJECT

    typedef ServiceModeLoginPanel _Myt;
    typedef LoginPanel _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] serviceMode  pointer to the service mode object
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c SessionDialog object
     */
    explicit ServiceModeLoginPanel( ServiceMode *serviceMode, SessionMainWindow *mainWindow, SessionDialog *parent = 0 );

    /// Destructor.
    virtual ~ServiceModeLoginPanel();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // SERVICEMODELOGINPANEL_H
