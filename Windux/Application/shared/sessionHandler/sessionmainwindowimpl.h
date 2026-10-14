/**
 * @file sessionmainwindowimpl.h
 * Main window widget for scale session.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef SESSIONMAINWINDOWIMPL_H
#define SESSIONMAINWINDOWIMPL_H

#include "modes/operatemode.h"
#include "modes/supervisormode.h"
#include "modes/servicemode.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Specialization for operate mode.
 */
template <>
inline OperateMode *SessionMainWindow::retrieveMode<OperateMode>()
{
    return operateMode_;
}

/**
 * Specialization for supervisor mode.
 */
template <>
inline SupervisorMode *SessionMainWindow::retrieveMode<SupervisorMode>()
{
    return supervisorMode_;
}

/**
 * Specialization for service mode.
 */
template <>
inline ServiceMode *SessionMainWindow::retrieveMode<ServiceMode>()
{
    return serviceMode_;
}

template <class Mode>
bool SessionMainWindow::switchToMode()
{
    return setMode( retrieveMode<Mode>() );
}

template <class Mode, class Screen>
bool SessionMainWindow::switchToModeScreen()
{
    // switch to requested mode
    if ( !switchToMode<Mode>() )
        return false;

    // switch to requested screen
    else if ( !switchToScreen<Screen>() )
        return false;

    return true;

}

template <class Screen>
bool SessionMainWindow::switchToScreen()
{
    ModeWidget *currentMode( mode() );
    ScreenWidget *screen( currentMode->screen() );

    // get rid of current screen when not correct type
    if (( !screen ) || ( !dynamic_cast<Screen*>( screen ) ))
    {
        // check for cached screen
        screen = currentMode->isCached<Screen>();

        // load cached screen
        if ( screen )
            currentMode->setScreen( screen );

        // set the new screen
        else
            currentMode->setScreen( new Screen( this, currentMode ) );
    }

    return true;
}

template <class Dialog>
bool SessionMainWindow::isDialogOpen() const
{
    // lock the mutex
    QMutexLocker lock( &const_cast<SessionMainWindow*>( this )->dialogListMutex_ );

    // check for dialog
    if ( !dialogList_.empty() )
        if ( qobject_cast<Dialog*>( dialogList_[0] ) )
            return true;

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // SESSIONMAINWINDOWIMPL_H
