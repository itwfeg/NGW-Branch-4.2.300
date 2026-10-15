/**
 * @file supervisormode.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "supervisormode.h"

#include "../panels/loginpanel.h"
#include "../panels/supervisormodepanel.h"

#include "../screens/screenwidget.h"

#include <palette.h>

#include <Session.h>
#include <ScaleConfiguration.h>
#include <Messages.h>

// this variable can be used by various modules to check if we are in demo mode.
extern bool isDemoMode;

///////////////////////////////////////////////////////////////////////////////////////////////////
SupervisorMode::SupervisorMode( ScreenWidget *screen, SessionMainWindow *parent ) :
    _Mybase( screen, Supervisor, parent ),
    menuPanelState_( SupervisorModePanel::CompactPanelFlag )
{
    cameFromProductMarqueeEditor_ = false;
    cameFromProductNutritionEditor_ = false;
    cameFromLabelPlacementView_ = false;
    cameFromProductDefaults_ = false;

    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
SupervisorMode::SupervisorMode( SessionMainWindow *parent ) :
    _Mybase( Supervisor, parent ),
    menuPanelState_( SupervisorModePanel::CompactPanelFlag )
{
    cameFromProductMarqueeEditor_ = false;
    cameFromProductNutritionEditor_ = false;
    cameFromLabelPlacementView_ = false;
    cameFromProductDefaults_ = false;

    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
SupervisorMode::~SupervisorMode()
{
    // delete the screen
    if ( screen_ )
    {
        delete screen_;
        screen_ = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int SupervisorMode::operatorAccessLevel() const
{
    LOG_CLASS_INFO( "SupervisorMode", "operatorAccessLevel" );

    const MadpScaleConfig *scaleConfig = &session_->scaleConfig->scaleConfig;

    // when in demo mode or supervisor not required to login then just return max permissions
    if (( isDemoMode ) || ( !scaleConfig->superForcedLogin() ))
    {
        LOG_DEBUG( "access level set to max", NULL );
        return 9;
    }

    return _Mybase::operatorAccessLevel();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
PanelWidget *SupervisorMode::createMenuPanel( int panelEx, SessionDialog *parent )
{
    const MadpScaleConfig *scaleConfig = &session_->scaleConfig->scaleConfig;

    if ( !isDemoMode )
        if (( scaleConfig->superForcedLogin() ) && ( !loggedIn() ))
            return new LoginPanel( this, mainWindow_, parent ); // UNCOMMENT SUPER LOGIN

    return new SupervisorModePanel( panelEx, mainWindow_, parent );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SupervisorMode::resetMenuPanelState()
{
    menuPanelState_ = SupervisorModePanel::CompactPanelFlag;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SupervisorMode::setMenuPanelState( int value )
{
    menuPanelState_ = (SupervisorModePanel::PanelFlag) value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SupervisorMode::setCameFromProductMarqueeEditor(bool marqueeVisit)
{
    cameFromProductMarqueeEditor_ = marqueeVisit;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SupervisorMode::setCameFromProductNutritionEditor(bool nutritionVisit)
{
    cameFromProductNutritionEditor_ = nutritionVisit;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SupervisorMode::setCameFromLabelPlacementView(bool labelPlacementVisit)
{
    cameFromLabelPlacementView_ = labelPlacementVisit;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SupervisorMode::setCameFromProductDefaults(bool productDefaultsVisit)
{
    cameFromProductDefaults_ = productDefaultsVisit;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool SupervisorMode::cameFromProductMarqueeEditor()
{
    return cameFromProductMarqueeEditor_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool SupervisorMode::cameFromProductNutritionEditor()
{
    return cameFromProductNutritionEditor_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool SupervisorMode::cameFromLabelPlacementView()
{
    return cameFromLabelPlacementView_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool SupervisorMode::cameFromProductDefaults()
{
    return cameFromProductDefaults_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool SupervisorMode::isKeyPresent() const
{
    MadpKeyActivation keyActivation( usbKeyActivation() );

    if (( keyActivation.isSupervisorKey() ) ||
        ( keyActivation.isSupervisorF10() ) ||
        ( keyActivation.isQATestKey() ))
        return true;

    return _Mybase::isSupervisorTokenPresent();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SupervisorMode::initialize()
{
    // set object name
    if ( objectName().isEmpty() )
        setObjectName( "SupervisorMode" );

    // set palette
    modifyPaletteBackground setBackground( ( QApplication::palette() ).background() );
    setBackground( this );

    setAutoFillBackground( true );
    setCameFromProductMarqueeEditor(false);
    setCameFromProductNutritionEditor(false);
    setCameFromLabelPlacementView(false);
    setCameFromProductDefaults(false);
}
