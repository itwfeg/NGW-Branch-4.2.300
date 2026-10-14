/**
 * @file emptyscreen.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "emptyscreen.h"

#include <palette.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
EmptyScreen::EmptyScreen( SessionMainWindow *mainWindow, ModeWidget *parent ) :
    _Mybase( mainWindow, parent )
{
    // set palette
    modifyPaletteBackground setBackground( Qt::transparent );
    setBackground( this );

    setAttribute( Qt::WA_TransparentForMouseEvents );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
EmptyScreen::~EmptyScreen()
{
}
