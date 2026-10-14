/**
 * @file supervisormodepanel.h
 * Panel for supervisor mode.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef SUPERVISORMODEPANEL_H
#define SUPERVISORMODEPANEL_H

#include "modepanel.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Panel for supervisor mode.
class SupervisorModePanel : public ModePanel
{
    Q_OBJECT

    typedef SupervisorModePanel _Myt;
    typedef ModePanel _Mybase;

public:

    /// Panel display flags.
    enum PanelFlag
    {
        CompactPanelFlag        = 0x000000,         ///< Show menu in compact form.
        NormalPanelFlag         = 0x010000          ///< Show menu in normal form.
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
    explicit SupervisorModePanel( int panelEx, SessionMainWindow *mainWindow, SessionDialog *parent = 0 );

    /// Destructor.
    virtual ~SupervisorModePanel();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve size hint.
    /**
     * @return  size hint
     */
    virtual QSize sizeHint() const;

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

    /// Slot for expanding supervisor menu.
    void onExpandMenuClicked();

    /// Slot for Backup Restore.
    void onBackupRestoreClicked();

    /// Slot for Departments.
    void onDepartmentsClicked();

    /// Slot for Normal Scale Operations.
    void onSuperNormalScaleOperationsClicked();

    /// Slot for marquee configuration.
    void onMarqueeConfigButtonClicked();

    /// Slot for Label Graphics.
    void onlabelGraphicsButtonClicked();

    /// Slot for label types.
    void onLabelTypesButtonClicked();
    
    /// Slot for Scale Statistics.
    void onScaleStatisticsButtonClicked();
    
    /// Slot for Supervisor->Operators->Configure Operators.
   void onSuperOperatorIdsClicked();
    
    /// Slot for Supervisor-Transactions-Operator Changes.
    void onSuperOperatorChangesClicked();

    /// Slot for operator note clicked.
    void onSuperOperatorNoteClicked();

    /// Slot for Supervisor-Transactions-Hourly Totals
    void onSuperHourlyTotalsClicked();

    /// Slot for Product Totals.
    void onSuperProductTotalsClicked();

    /// Slot for Viewing Supervisor Changes
    void onSuperSupervisorChangesClicked();

    /// Slot for Supervisor-Transactions-Operator Totals
    void onSuperOperatorTotalsClicked();

    /// Slot for Supervisor-Transactions-Void Transactions
    void onVoidTransactionsClicked();

    /// Slot for Supervisor-Transactions-Transaction Types
    void onSuperTotalTypesSetupClicked();

    /// Slot for Labels - Fields to Print.
    void onLabelsFieldsToPrintClicked();

    /// Slot for Operators - Operators Can Modify.
    void onOperatorsCanModifyClicked();

    /// Slot for Shelf Life Expiration.
    void onShelfLifeExpirationClicked();

    /// Slot for Store Information.
    void onStoreInformationClicked();

    /// Slot for Delete All files.
    void onDeleteAllFilesClicked();

    /// Slot for Label Text Names.
    void onLabelTextNamesClicked();

    /// Slot for setting Security label prices.
    void onSecurityLabelsClicked();

    /// Slot for Print Config
    void onPrintConfigClicked();

    /// Slot for Change Product Unit Price
    void onSuperChangeProductUnitPriceClicked();

    /// Slot for Change Reporting Classes
    void onSuperReportingClassesClicked();

    /// Slot for Change Product Class
    void onSuperChangeProductClassClicked();

    /// Slot for Change Product ->Default Products
    void onSuperProductDefaultsClicked();

    /// Slot for Change Product -> Tare
    void onSuperChangeProductTareClicked();

    /// Slot for Change Product -> Tare
    void onSuperChangeProductShelfLifeClicked();

    /// Slot for Comm Interface
    void onCommInterfaceClicked();

    /// Slot for Setup->Communications->Scale Synchronization (The artist previously known as Automatic Updates)
    void onScaleSynchronizationClicked();

    /// Slot for Print Setup
    void onPrintSetupClicked();

    /// Slot for Super -> Cool -> Assign to Categories
    void onSuperCoolMainClicked();

    /// Slot for Change Product -> All Fields
    void onSuperChangeProductAllFieldsClicked();

    /// Slot for Change Product -> Price All Fields
    void onSuperChangeProductPriceFieldsClicked();

    /// Slot for Email Setup of alert messages
    void onSuperEmailSetupClicked();

    /// Slot for Upgrading scale's firmware via SFTP
    void onSuperUpgradeFirmwareViaSFTPClicked();

    /// Slot for Rezeroing the weigher when selected
    void onRezeroWeigherClicked();

    /// Slot for allowing access to the settings wizard in Supervisor mode.
    void onSetupUtilityButtonClicked();

    /// Slot for setting graphics of Specials
    void onSuperSpecialsClicked();
    
    /// Slot for setting Categories
    void onSuperCategoriesClicked();

    /// Slot for reading scanner codes
    void onSuperScannedCodesClicked();

    /// Slot for setting configuration for firmware upgrade via USB
    void onSuperUpgradeFirmwareViaUSBClicked();

    /// Slot for Cool->Cool Categories
    void onSuperCoolCreateCategoriesClicked();

    /// Slot for Cool->Cool Pretext
    void onSuperCoolCreatePretextClicked();

    /// Slot for Cool->Cool Categories
    void onSuperCoolCreateCountriesClicked();

    /// Slot for Cool->Cool Production Steps
    void onSuperCoolCreateProdStepsClicked();

    /// Slot for Operate Mode Layout -> Move Layout
    void onOperateModeLayoutMoveLayout();

    /// Slot for Operate Mode Layout -> Change Fields
    void onOperateModeLayoutChangeFields();

    /// Slot for Printer Setup
    void onPrinterSetupClicked();

    /// Slot for Customer Display
    void onCustomerDisplayClicked();

    /// Slot for flashkey editor.
    void onFlashkeyEditorClicked();

    /// Slot for frequent shopper.
    void onFrequentShopperClicked();

    /// Slot for overriding applier rotation
    void onSuperApplierRotationOverrideClicked();

    /**
     * Slot for health agent.
     */
    void onHealthAgentClicked();

    /// Slot for printer dot status
    void onPrinterDotStatusClicked();

private:

    pushButtonList cool_;
    pushButtonList labels_;
    pushButtonList miscellaneous_;
    pushButtonList operators_;
    pushButtonList otherRecords_;
    pushButtonList actions_;
    pushButtonList productChanges_;
    pushButtonList scaleOperations_;
    pushButtonList transactions_;
    pushButtonList operateModeLayout_;

    bool compactMenu_;
    QPushButton *expandButton_;

    static const int buttonsPerRow_;

    /// Initialize.
    void initialize();

    /// Initialize menu buttons.
    void initializeButtons();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // SUPERVISORMODEPANEL_H
