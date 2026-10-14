/**
 * @file operatemodepanel.h
 * Panel for operate mode.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef OPERATEMODEPANEL_H
#define OPERATEMODEPANEL_H

#include "modepanel.h"

#include <QPushButton>

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Panel for operate mode.
class OperateModePanel : public ModePanel
{
    Q_OBJECT

    typedef OperateModePanel _Myt;
    typedef ModePanel _Mybase;

public:

    /// Which extended operate menu panel should be shown.
    enum ExtendedPanel
    {
        CoolTextExtendedPanel = 1,                      ///< Open panel with cool text shown
        ExpirationHistoryExtendedPanel,                 ///< Open panel with expiration history shown
        TotalTypesExtendedPanel,                        ///< Open panel with total types shown
        VoidTransactionExtendedPanel,                   ///< Open panel with void transaction shown
        SizeLabelsExtendedPanel,                        ///< Open panel with size labels shown
        ChooseDepartmentExtendedPanel                   ///< Open panel with size labels shown
    };

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] panelEx  extended panel to create
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c SessionDialog object
     */
    explicit OperateModePanel( int panelEx, SessionMainWindow *mainWindow, SessionDialog *parent = 0 );

    /// Destructor.
    virtual ~OperateModePanel();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve size hint.
    /**
     * @return  size hint
     */
    virtual QSize sizeHint() const;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Create an extended menu panel object.
    /**
     * @return  pointer to panel
     */
    virtual PanelWidget *createExtendedPanel();

protected:

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve renderer traits.
    /**
     * @return  traits object
     */
    virtual hobart::backgroundWidgetBase *rendererTraits() const;

private Q_SLOTS:

    /// Slot for about scale.
    void onAboutScaleClicked();

    /// Slot for create cool.
    void onCreateCoolClicked();

    /// Slot for expiration history.
    void onExpirationHistoryClicked();

    /// Slot for flashkey editor.
    void onFlashkeyEditorClicked();

    /// Slot for label placement.
    void onLabelPlacementClicked();

    /// Slot for operator note.
    void onOperatorNoteClicked();

    /// Slot for planogram.
    void onPlanogramClicked();

    /// Slot for plu info.
    void onPluInfoClicked();

    /// Slot for printer settings.
    void onPrinterSettingsClicked();

    /// Slot for printer label cleaning.
    void onPrinterCleanerClicked();

    /// Slot for selecting scale department.
    void onSelectScaleDeptClicked();

    /// Slot for turning on and turning off pick 5
    void onSelectPick5Clicked();

    /// Slot for size labels.
    void onSizeLabelsClicked();

    /// Slot for Rezero Weigher.
    void onRezeroWeigherClicked();

    /// Slot for total types.
    void onTotalTypesClicked();

    /// Slot for void transaction.
    void onVoidTransactionClicked();

    /// Slot for Weights & Measures.
    void onWeightsAndMeasuresClicked();

    /// Slot for wrapper settings.
    void onWrapperSettingsClicked();

    /// Slot for software versions.
    void onFirmwareVersionsClicked();

    /// Slot to log in/out the Operator.
    void onLogInOutOperatorClicked();

    /// Slot for queue management.
    void onQueueManagementClicked();

    /// Slot for serve next customer.
    void onServeNextCustomerClicked();

    /// Slot for ejecting USB media.
    void onEjectMediaClicked();

    /**
     * @brief onUpdateNowClicked
     * Slot for Retrieving Updates from the configured primary source of updates
     */
    void onUpdateNowClicked();

    /**
      * @brief onSendToDeptClicked
      * Slot for updating other scales in Dept from this Primary scale
    */
    void onSendToDeptClicked();

    /// Slot for calibrate wrapper.
    void onCalibrateWrapperClicked();

    void onPrinterDotStatusClicked();

private:

    ExtendedPanel extendedPanel_;

    QPushButton *pickFive_;
    pushButtonList left_;
    pushButtonList right_;

    QStringList editFeatureList_;

    static const int buttonsPerRow_;

    /// Initialize.
    void initialize();

    /// Initialize menu buttons.
    void initializeButtons();



};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // OPERATEMODEPANEL_H
