/**
 * @file supervisormode.h
 * Supervisor mode.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef SUPERVISORMODE_H
#define SUPERVISORMODE_H

#include "modewidget.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

class SupervisorMode : public ModeWidget
{
    Q_OBJECT

    Q_PROPERTY( int menuPanelState READ menuPanelState WRITE setMenuPanelState RESET resetMenuPanelState )

    typedef SupervisorMode _Myt;
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
    explicit SupervisorMode( ScreenWidget *screen, SessionMainWindow *parent = 0 );

    /**
     * Constructor.
     *
     * @param[in] parent  parent @c SessionMainWindow widget
     */
    explicit SupervisorMode( SessionMainWindow *parent = 0 );

    /**
     * Destructor.
     */
    virtual ~SupervisorMode();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Retrieve menu panel state.
     *
     * @returns  menu panel state.
     */
    virtual int menuPanelState() const {return menuPanelState_;}

    /**
     * Retrieve operator access level.
     *
     * @returns  access level
     */
    virtual int operatorAccessLevel() const;

    /**
      * Retrieve bool is we came from Product Marquee Editor only
      * @returns boolean of whether we came from just using the product marquee editor
      */
    virtual bool cameFromProductMarqueeEditor();

    /**
      * Retrieve bool is we came from Product Nutrition Editor only
      * @returns boolean of whether we came from just using the product nutrifacts editor
      */
    virtual bool cameFromProductNutritionEditor();

    /**
      * Retrieve bool is we came from Label Placement View only
      * @returns boolean of whether we came from just using the label placement view screen
      */
    virtual bool cameFromLabelPlacementView();

    /**
      * Retrieve bool is we came from Label Placement View only
      * @returns boolean of whether we came from just using the label placement view screen
      */
    bool cameFromProductDefaults();

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

    /**
     * Set the state of whether we came from the product marquee editor or not.
     *
     * @param[in] marqueeVisit boolean
     * @returns nothing
     */
    virtual void setCameFromProductMarqueeEditor(bool marqueeVisit);

    /**
     * Set the state of whether we came from the product nutrition editor or not.
     *
     * @param[in] nutritionVisit boolean
     * @returns nothing
     */
    virtual void setCameFromProductNutritionEditor(bool nutritionVisit);

    /**
     * Set the state of whether we came from the label placement editor or not.
     *
     * @param[in] nutritionVisit boolean
     * @returns nothing
     */
    virtual void setCameFromLabelPlacementView(bool labelPlacementVisit);

    /**
     * Set the state of whether we came from the label placement editor or not.
     *
     * @param[in] nutritionVisit boolean
     * @returns nothing
     */
     void setCameFromProductDefaults(bool productDefaultsVisit);

    ///////////////////////////////////////////////////////////////////////////
    // Variables

    MadpOperatorDisplayFields operatorDisplayFields_;
	MadpAccumItemsConfig accumItemsConfig_;
	DomMarqueeConfig marqueeConfig_;
	DomTransactionDates transactionDates_;
	DomTotalTypeData totalTypeData_;
	DomDefaultCurrentPLU defaultCurrentPLU_;
    DomCoolCategory coolCategory_;
    DomCoolPretext coolPretext_;
    DomCoolProductionStep coolProductionStep_;
    DomCoolCountry coolCountry_;
	DomNormalScaleOperation normalScaleOperation_;
    DomAutomaticDataTypes automaticDataTypes_;
    DomAutomaticUpdate automaticUpdate_;
	DomSingleOperator singleOperator_;
    DomOperatorInfomation operatorInformation_;
    DomSingleScannedCode singleScannedCode_;
    DomScannerCodeList scannerCodeList_;
    DomNewScannedCode newScannedCode_;
    DomPluSupport pluSupport_;
    DomPrWriteAllProductTextPrintParams promptness_;
    DomPrWriteAllExternalPrinter extPrintSettings_;


public Q_SLOTS:

    /**
     * Restt menu panel state.
     */
    virtual void resetMenuPanelState();

    /**
     * Set menu panel state.
     *
     * @param[in] value  menu panel state.
     */
    virtual void setMenuPanelState( int value );    

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

    int menuPanelState_;
    bool cameFromProductMarqueeEditor_;
    bool cameFromProductNutritionEditor_;
    bool cameFromLabelPlacementView_;
    bool cameFromProductDefaults_;

    /**
     * Initialize.
     */
    void initialize();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // SUPERVISORMODE_H
