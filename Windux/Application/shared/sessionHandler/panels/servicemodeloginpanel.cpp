/**
 * @file servicemodeloginpanel.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "servicemodeloginpanel.h"

#include "../sessionmainwindow.h"

#include "../modes/servicemode.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
ServiceModeLoginPanel::ServiceModeLoginPanel( ServiceMode *serviceMode, SessionMainWindow *mainWindow, SessionDialog *parent ) :
    _Mybase( serviceMode, mainWindow, parent )
{
    static const bool totpAllowed( true );
	
    setObjectName( "ServiceModeLoginPanel" );

    // figure out what is allowed
    if ( totpAllowed )
        allowLoginOrServiceKey();
    else if (( !serviceMode->usbWorking() ) || ( mainWindow_->isSecondarySessionType() ))
        allowLoginOrServiceKey();
    else
        allowServiceKeyOnly();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ServiceModeLoginPanel::~ServiceModeLoginPanel()
{
}
