/**
 * @file emptyscreen.h
 * Screen with nothing in it.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef EMPTYSCREEN_H
#define EMPTYSCREEN_H

#include "screenwidget.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

class EmptyScreen : public ScreenWidget
{
    typedef EmptyScreen _Myt;
    typedef ScreenWidget _Mybase;

public:

    ///////////////////////////////////////////////////////////////////////////
    // CTOR / DTOR

    /**
     * Constructor.
     *
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
     */
    explicit EmptyScreen( SessionMainWindow *mainWindow, ModeWidget *parent = 0 );

    /**
     * Destructor.
     */
    virtual ~EmptyScreen();

};


///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // EMPTYSCREEN_H
