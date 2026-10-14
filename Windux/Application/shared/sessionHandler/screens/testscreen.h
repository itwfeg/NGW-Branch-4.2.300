/**
 * @file testscreen.h
 * Screen for developer testing.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef TESTSCREEN_H
#define TESTSCREEN_H

#include "screenwidget.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

class TestScreen : public ScreenWidget
{
    Q_OBJECT

    typedef TestScreen _Myt;
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
    explicit TestScreen( SessionMainWindow *mainWindow, ModeWidget *parent = 0 );

    /**
     * Destructor.
     */
    virtual ~TestScreen();

private Q_SLOTS:

    /**
     * Slot to set the capacity bar to the top.
     */
    void onSetCapacityBarAreaTop();

    /**
     * Slot to set the capacity bar to the bottom.
     */
    void onSetCapacityBarAreaBottom();

    /**
     * Slot to hide the weight bar in operate mode.
     */
    void onHideOperateWeightBar();

    /**
     * Slot to show the weight bar in operate mode.
     */
    void onShowOperateWeightBar();

    /**
     * Slot to play operater video.
     */
    void onPlayOperatorVideo1();

    /**
     * Slot to play operater video.
     */
    void onPlayOperatorVideo2();

private:

    /**
     * Initialize.
     */
    void initialize();

    /**
     * Initialize for demo.
     */
    void initializeForDemo();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // TESTSCREEN_H
