/**
 * @file sessioninfo.h
 * Methods for retrieving session info without having direct pointers.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef SESSIONINFO_H
#define SESSIONINFO_H

#include "sessionmainwindow.h"

#include <QWidget>

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class sessionInfo
 * @brief Functor for retrieving session info without having direct pointers
 *
 * This functor object will enumerate parents until if finds session information. The intended use
 * is for objects that have no mainWindow_ or session_ pointer available.
 *
 * Example usage:
 * @code
 *   sessionInfo mySessionInfo( this );
 *   ModeWidget *mode = mySessionInfo()->mode();
 *   Session *session = mySessionInfo()->session();
 *   SessionMainWindow *mainWindow = mySessionInfo();
 *   mainWindow->session()->send( ... );
 *   mainWindow->isOperateMode();
 * @endcode
 */
class sessionInfo : public std::unary_function<QWidget, SessionMainWindow>
{
public:

    /**
     * Constructor.
     */
    sessionInfo() : mainWindow_( NULL ) {}

    /**
     * Constructor.
     *
     * @param[in] widget  widget that has a SessionMainWindow for an ancestor
     */
    sessionInfo( const QWidget *widget ) : mainWindow_( findSessionMainWindow( widget ) ) {}

    /// Destructor.
    virtual ~sessionInfo() {}

    /**
     * Function operator.
     *
     * @returns  pointer to session main window
     */
    SessionMainWindow *operator () () const {
        return mainWindow_;
    }

    /**
     * Function operator.
     *
     * @param[in] widget  widget that has a SessionMainWindow for an ancestor
     * @returns  pointer to session main window
     */
    SessionMainWindow *operator () ( const QWidget *widget ) const {
        return findSessionMainWindow( widget );
    }

private:

    SessionMainWindow *mainWindow_;

    /// Finds the @c SessionMainWindow object for widget @p widget.
    static SessionMainWindow *findSessionMainWindow( const QWidget *widget ) {
        const SessionMainWindow *result( NULL );
        while ( widget ) {
            if ( (result = dynamic_cast<const SessionMainWindow*>( widget )) )
                break;
            widget = dynamic_cast<const QWidget*>( widget->parent() );
        }
        return const_cast<SessionMainWindow*>( result );
    }

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // SESSIONINFO_H
