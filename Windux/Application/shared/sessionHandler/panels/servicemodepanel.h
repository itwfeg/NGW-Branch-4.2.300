/**
 * @file servicemodepanel.h
 * Panel for service mode.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef SERVICEMODEPANEL_H
#define SERVICEMODEPANEL_H

#include "modepanel.h"

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Panel for service mode.
class ServiceModePanel : public ModePanel
{
    Q_OBJECT

    typedef ServiceModePanel _Myt;
    typedef ModePanel _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c SessionDialog object
     */
    explicit ServiceModePanel( SessionMainWindow *mainWindow, SessionDialog *parent = 0 );

    /// Destructor.
    virtual ~ServiceModePanel();

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

    /// Slot for empty page.
    void onEmptyPageButtonClicked();

    /// Slot for Setup Utility.
    void onSetupUtilityButtonClicked();

    /// Slot for System Diagnostics.
    void onSystemDiagnosticsButtonClicked();

    /// Slot for Manufacturing Test - Pre-Test.
    void onManufacturingTestPreTestButtonClicked();

    /// Slot for Manufacturing Test - Final Test.
    void onManufacturingTestFinalButtonClicked(); // FT-1222

    /// Slot for Service Diagnostics.
    void onServiceDiagnosticsButtonClicked(); // FT-1222

    /// Slot for test page.
    void onTestPageButtonClicked();

    /// Slot for Supervisor OS Access setting.
    void onSupervisorOsAccessClicked();

    /// Slot for Currency Settings setting.
    void onCurrencySettingsClicked();

    /// Slot for Pricing Mode.
    void onPricingModeClicked();

    /// Slot for Peripheral List.
    void onPeripheralListClicked();

    /// Slot for Printer Powerup Test Results.
    void onPrinterPowerupTestResultsClicked();

    /// Slot for Printer Configuration.
    void onPrinterConfigurationClicked();

    /// Slot for Printer Monitor Sensors.
    void onPrinterMonitorSensorsClicked();

    /// Slot to see the Printer Head Dot Status
    void onPrinterHeadDotStatusClicked();

    /// Slot for Weigher Powerup Test Results.
    void onWeigherPowerupTestResultsClicked();

    /// Slot for Weigher Calibration.
    void onWeigherCalibrationClicked();

    /**
     * Slot for Audit Log.
     */
    void onWeigherAuditLogClicked();

    /**
     * Slot for Weigher Configuration.
     */
    void onWeigherConfigurationClicked();

    /// Slot for Weigher Information.
    void onWeigherInformationClicked();

    /// Slot for Installation Log.
    void onServiceInstallationLogClicked();

    /// Slot for Rezero Weigher.
    void onRezeroWeigherClicked();

    /// SOF-1359: Slot for Reset Weigher
    void onResetWeigherClicked();

    /// SOF-1358: Slot for Reset Printer
    void onResetPrinterClicked();

    /// Slot for Health Agent.
    void onHealthAgentClicked();

    /// Slot for Customer Display Option.
    void onCustomerDisplayOptionClicked(); // FT-979

    /// Slot for remove labels.
    void onRemoveLabelsClicked();

    /// Slot for Normal Scale Operations.
    void onSuperNormalScaleOperationsClicked();

    /// Slot for Labels - Fields to Print.
    void onLabelsFieldsToPrintClicked();

    /// Slot for Store Information.
    void onStoreInformationClicked();

    /// Slot for Operators - Operators Can Modify.
    void onOperatorsCanModifyClicked();

    /// Slot for Change Product ->Default Products
    void onSuperProductDefaultsClicked();

    /// Slot for Supervisor->Operators->Configure Operators.
    void onSuperOperatorIdsClicked();

    /// Slot for Frequent Shopper settings.
    void onFrequentShopperClicked();

    /// Slot for Backup/Restore.
    void onBackupRestoreClicked();

    /// Slot for Printer Locations.
    void onPrinterLocationsClicked();

private:

    pushButtonList peripherals_;
    pushButtonList printer_;
    pushButtonList weigher_;
    pushButtonList miscellaneous_;
    pushButtonList supervisorSettings_;

    static const int buttonsPerRow_;

    /// Initialize.
    void initialize();

    /// Initialize menu buttons.
    void initializeButtons();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // SERVICEMODEPANEL_H
