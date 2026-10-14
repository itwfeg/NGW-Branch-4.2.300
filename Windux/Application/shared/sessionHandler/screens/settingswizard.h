/**
 * @file settingswizard.h
 * Screen for scale settings.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Todd Davis
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef SETTINGSWIZARD_H
#define SETTINGSWIZARD_H

#include "screenwidget.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

class SettingsWizard : public ScreenWidget
{
    Q_OBJECT

    typedef SettingsWizard _Myt;
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
    explicit SettingsWizard( SessionMainWindow *mainWindow, ModeWidget *parent = 0 );

    /**
     * Destructor.
     */
    virtual ~SettingsWizard();

private:

    QWidget *widget_;
};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // SETTINGSWIZARD_H
