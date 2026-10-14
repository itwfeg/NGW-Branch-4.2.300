/**
 * @file servicemode.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "servicemode.h"

#include "../panels/servicemodeloginpanel.h"
#include "../panels/servicemodepanel.h"
#include "../panels/wrapperservicemodediagnosticspanel.h"
#include "../panels/wrapperservicelabelapplierpanel.h"
#include "../panels/wrapperservicemodepanel.h"
#include "../panels/wrapperservicemodetestspanel.h"

#include "../screens/screenwidget.h"

#include <palette.h>

#include <Session.h>
#include <ScaleConfiguration.h>
#include <Messages.h>

// this variable can be used by various modules to check if we are in demo mode.
extern bool isDemoMode;

const bool ServiceMode::usbWorking_ = true;
const bool TURN_ON_SERVICE_MENU = false;
const bool TURN_ON_WRAPPER_SERVICE_MENU = false;

///////////////////////////////////////////////////////////////////////////////////////////////////
ServiceMode::ServiceMode( ScreenWidget *screen, SessionMainWindow *parent ) :
    _Mybase( screen, Service, parent )
{
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ServiceMode::ServiceMode( SessionMainWindow *parent ) :
    _Mybase( Service, parent )
{
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ServiceMode::~ServiceMode()
{
    // delete the screen
    if ( screen_ )
    {
        delete screen_;
        screen_ = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
PanelWidget *ServiceMode::createMenuPanel( int panelEx, SessionDialog *parent )
{
    if( TURN_ON_SERVICE_MENU && !TURN_ON_WRAPPER_SERVICE_MENU )
        return new ServiceModePanel( mainWindow_, parent );

    if ( !isDemoMode && !TURN_ON_WRAPPER_SERVICE_MENU )
        if ( !loggedIn() )
            return new ServiceModeLoginPanel( this, mainWindow_, parent );

    if ( session_->isPrepack() || TURN_ON_WRAPPER_SERVICE_MENU )
    {
        if ( WrapperServiceModeLAPanel::PANEL_ID < panelEx )
            return new WrapperServiceModeLAPanel( panelEx, mainWindow_, parent );
        else if ( WrapperServiceModeDiagnosticsPanel::PANEL_ID < panelEx )
            return new WrapperServiceModeDiagnosticsPanel( panelEx, mainWindow_, parent );
        else if ( WrapperServiceModeTestsPanel::PANEL_ID < panelEx )
            return new WrapperServiceModeTestsPanel( panelEx, mainWindow_, parent );

        return new WrapperServiceModePanel( mainWindow_, parent );
    }

    return new ServiceModePanel( mainWindow_, parent );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool ServiceMode::isKeyPresent() const
{

    if( TURN_ON_SERVICE_MENU || TURN_ON_WRAPPER_SERVICE_MENU )
        return true;

    MadpKeyActivation keyActivation( usbKeyActivation() );

    if (( keyActivation.isServiceKey() ) || ( keyActivation.isQATestKey() ))
        return true;

    return _Mybase::isServiceTokenPresent();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ServiceMode::initialize()
{
    // set object name
    if ( objectName().isEmpty() )
        setObjectName( "ServiceMode" );

    // set palette
    modifyPaletteBackground setBackground( ( QApplication::palette() ).background() );
    setBackground( this );

    setAutoFillBackground( true );

	wrTimingsData_.precedence = -1;
	wrTimingsData_.viewCurrentTimings = true;
	wrTimingsData_.trayName = "";
	wrTimingsData_.pluException = "";
	wrTimingsData_.heightBoxSelected = "";
	wrTimingsData_.wrapperModelNum = 0;
}
