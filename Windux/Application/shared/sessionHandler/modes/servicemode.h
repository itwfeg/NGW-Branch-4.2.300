/**
 * @file servicemode.h
 * Service mode.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef SERVICEMODE_H
#define SERVICEMODE_H

#include "modewidget.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

class ServiceMode : public ModeWidget
{
    Q_OBJECT

    typedef ServiceMode _Myt;
    typedef ModeWidget _Mybase;

public:

    ///////////////////////////////////////////////////////////////////////////
    // CTOR / DTOR

    /**
     * Constructor.
     *
     * @param[in] screen  supervisor screen to display
     * @param[in] parent  parent @c SessionMainWindow widget
     */
    explicit ServiceMode( ScreenWidget *screen, SessionMainWindow *parent = 0 );

    /**
     * Constructor.
     *
     * @param[in] parent  parent @c SessionMainWindow widget
     */
    explicit ServiceMode( SessionMainWindow *parent = 0 );

    /**
     * Destructor.
     */
    virtual ~ServiceMode();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Check if USB is working or not.
     *
     * @returns  @c true if USB is working, @c false otherwise
     */
    virtual bool usbWorking() const {return usbWorking_;}

    ///////////////////////////////////////////////////////////////////////////
    // Methods

    /**
     * Create a menu panel object.
     *
     * @param[in] panelEx  extended panel type
     * @param[in] parent  panel parent dialog
     * @returns  pointer to panel
     */
    virtual PanelWidget *createMenuPanel( int panelEx, SessionDialog *parent );

    ///////////////////////////////////////////////////////////////////////////
    // Variables

	DomInstallationLogDeviceData installationLogDeviceData_;
    DomNormalScaleOperation normalScaleOperation_;
    MadpOperatorDisplayFields operatorDisplayFields_;
    MadpAccumItemsConfig accumItemsConfig_;
    DomDefaultCurrentPLU defaultCurrentPLU_;
    DomSingleOperator singleOperator_;
    DomOperatorInfomation operatorInformation_;
    DomPrWriteAllProductTextPrintParams promptness_;
    DomPrWriteAllExternalPrinter extPrintSettings_;

	WrapperTimingsData wrTimingsData_;

protected:

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Check if logged in via the usb key.
     *
     * @returns  @c true if usb key present, @c false otherwise
     */
    virtual bool isKeyPresent() const;

private:

    static const bool usbWorking_;

    /**
     * Initialize.
     */
    void initialize();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // SERVICEMODE_H
