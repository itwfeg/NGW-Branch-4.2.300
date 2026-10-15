/**
 * @file servicemodepanel.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "servicemodepanel.h"

#include "../sessionmainwindow.h"
#include "../sessionmainwindowimpl.h"
#include "../sessiondialog.h"

#include "../modes/servicemode.h"

#include "../screens/emptyscreen.h"
#include "../screens/setupUtility.h"
#include "../screens/testscreen.h"
#include "../screens/manufacturingtest.h"
#include "../screens/serviceSupervisorOsAccess.h"
#include "../screens/servicePricingMode.h"
#include "../screens/servicePeripheralList.h"
#include "../screens/serviceCurrencySetup.h"
#include "../screens/servicePrinterPowerupTests.h"
#include "../screens/servicePrinterConfig.h"
#include "../screens/servicePrinterSensors.h"
#include "../screens/servicePrinterSensors.h"
#include "../screens/servicePrinterHeadDotStatus.h"
#include "../screens/serviceWeigherPowerupTestResults.h"
#include "../screens/serviceWeigherConfig.h"
#include "../screens/serviceWeigherCalibrate.h"
#include "../screens/serviceWeigherAuditLog.h"
#include "../screens/serviceWeigherInfo.h"
#include "../screens/serviceinstallationlog.h"
#include "../screens/serviceHealthAgent.h"
#include "../screens/serviceCustomerDisplayOption.h"
#include "../screens/serviceRemoveLabels.h"
#include "../screens/systemDiagnostics.h"
#include "../screens/serviceWeigherReset.h"
#include "../screens/servicePrinterReset.h"

//-------------------------------------------------------
// Supervisor Settings - duplicated from Supervisor panel
//-------------------------------------------------------
#include "../screens/superNormalScaleOperation.h"
#include "../screens/labelFieldsToPrint.h"
#include "../screens/storeInformation.h"
#include "../screens/operatorsCanModify.h"
#include "../screens/superProductDefaults.h"
#include "../screens/superOperatorIds.h"
#include "../screens/serviceFrequentShopper.h"
#include "../screens/BackupRestore.h"
#include "../screens/printerLocations.h"

#include <backgroundwidgettypes.h>

#include <ScaleConfiguration.h>
#include <UiUtilities.h>

#include <QVBoxLayout>
#include <QPushButton>

const bool TURN_ON_SERVICE_MENU = true;
const bool TURN_ON_WRAPPER_MENU = true;
const bool TURN_ON_LABEL_APPLIER_MENU = true;

/// Renderer traits type.
typedef hobart::serviceModePanelBackgroundWidget rendererTraitsType;

const int ServiceModePanel::buttonsPerRow_ = 3;

///////////////////////////////////////////////////////////////////////////////////////////////////
ServiceModePanel::ServiceModePanel( SessionMainWindow *mainWindow, SessionDialog *parent ) :
    _Mybase( mainWindow, parent )
{
    LOG_CLASS_INFO( "ServiceModePanel", "ServiceModePanel" );

    setFixedSize( sizeHint() );
    setObjectName( "ServiceModePanel" );

    // retrieve access level
    if ( mainWindow )
        accessLevel_ = mainWindow->retrieveMode<ServiceMode>()->operatorAccessLevel();

    LOG_INFO( "access level is %d", accessLevel_ );

    // initialize interface
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ServiceModePanel::~ServiceModePanel()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QSize ServiceModePanel::sizeHint() const
{
    return QSize(
        (mainWindow_->width() * rendererTraitsType::defaultWidth_) / SessionMainWindow::scalingDividerX_,
        (mainWindow_->height() * rendererTraitsType::defaultHeight_) / SessionMainWindow::scalingDividerY_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
hobart::backgroundWidgetBase *ServiceModePanel::rendererTraits() const
{
    return new rendererTraitsType();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ServiceModePanel::onEmptyPageButtonClicked()
{
    mainWindow_->switchToModeScreen<ServiceMode, EmptyScreen>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ServiceModePanel::onSetupUtilityButtonClicked()
{
    mainWindow_->switchToModeScreen<ServiceMode, setupUtility>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ServiceModePanel::onSystemDiagnosticsButtonClicked()
{
    mainWindow_->switchToModeScreen<ServiceMode, systemDiagnostics>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ServiceModePanel::onManufacturingTestPreTestButtonClicked()
{
    // FT-1222
    if ( mainWindow_->switchToModeScreen<ServiceMode, manufacturingTest>() )
    {
        ServiceMode *serviceMode( mainWindow_->retrieveMode<ServiceMode>() );
        manufacturingTest *screen( dynamic_cast<manufacturingTest*>( serviceMode ? serviceMode->screen() : NULL ) );

        if ( screen )
        {
            accept();

            // Set to Manufacturing Test:  Pre-Test Mode
            // Panel is accepted first so it is dismissed before the printer/weigher reset dialogs appear
            screen->setManufacturingTestMode( manufacturingTest::ManufTestPreTest );
            return;
        }
    }

    accept();
}



///////////////////////////////////////////////////////////////////////////////////////////////////
void ServiceModePanel::onManufacturingTestFinalButtonClicked()
{
    // FT-1222
    if ( mainWindow_->switchToModeScreen<ServiceMode, manufacturingTest>() )
    {
        ServiceMode *serviceMode( mainWindow_->retrieveMode<ServiceMode>() );
        manufacturingTest *screen( dynamic_cast<manufacturingTest*>( serviceMode ? serviceMode->screen() : NULL ) );

        if ( screen )
        {
            // Set to Manufacturing Test: Final Test Mode
            screen->setManufacturingTestMode( manufacturingTest::ManufTestFinalTest );
        }
    }

    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// FT-1222
void ServiceModePanel::onServiceDiagnosticsButtonClicked()
{
    if ( mainWindow_->switchToModeScreen<ServiceMode, manufacturingTest>() )
    {
        ServiceMode *serviceMode( mainWindow_->retrieveMode<ServiceMode>() );
        manufacturingTest *screen( dynamic_cast<manufacturingTest*>( serviceMode ? serviceMode->screen() : NULL ) );

        if ( screen )
        {
            // Set to Service Diagnostics mode
            // Display the Service Diagnostic tab
            screen->setManufacturingTestMode( manufacturingTest::ServiceDiagnosticMode );
        }
    }

    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ServiceModePanel::onTestPageButtonClicked()
{
    mainWindow_->switchToModeScreen<ServiceMode, TestScreen>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ServiceModePanel::onFrequentShopperClicked()
{
    mainWindow_->switchToModeScreen<ServiceMode, serviceFrequentShopper>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ServiceModePanel::onSupervisorOsAccessClicked()
{
    mainWindow_->switchToModeScreen<ServiceMode, serviceSupervisorOsAccess>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ServiceModePanel::onCurrencySettingsClicked()
{
    mainWindow_->switchToModeScreen<ServiceMode, serviceCurrencySetup>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ServiceModePanel::onPricingModeClicked()
{
    mainWindow_->switchToModeScreen<ServiceMode, servicePricingMode>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ServiceModePanel::onPeripheralListClicked()
{
    mainWindow_->switchToModeScreen<ServiceMode, servicePeripheralList>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ServiceModePanel::onPrinterPowerupTestResultsClicked()
{
    mainWindow_->switchToModeScreen<ServiceMode, servicePrinterPowerupTests>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ServiceModePanel::onPrinterConfigurationClicked()
{
    mainWindow_->switchToModeScreen<ServiceMode, servicePrinterConfig>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ServiceModePanel::onPrinterMonitorSensorsClicked()
{
    mainWindow_->switchToModeScreen<ServiceMode, servicePrinterSensors>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ServiceModePanel::onPrinterHeadDotStatusClicked()
{
    mainWindow_->switchToModeScreen<ServiceMode, servicePrinterHeadDotStatus>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ServiceModePanel::onWeigherPowerupTestResultsClicked()
{
    mainWindow_->switchToModeScreen<ServiceMode, serviceWeigherPowerupTestResults>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ServiceModePanel::onWeigherConfigurationClicked()
{
    mainWindow_->switchToModeScreen<ServiceMode, serviceWeigherConfig>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ServiceModePanel::onWeigherCalibrationClicked()
{
    mainWindow_->switchToModeScreen<ServiceMode, serviceWeigherCalibrate>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ServiceModePanel::onWeigherAuditLogClicked()
{
    mainWindow_->switchToModeScreen<ServiceMode, serviceWeigherAuditLog>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ServiceModePanel::onWeigherInformationClicked()
{
    mainWindow_->switchToModeScreen<ServiceMode, serviceWeigherInfo>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ServiceModePanel::onRezeroWeigherClicked()
{
    session_->uiUtilities->rezeroWeigher();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// SOF-1359
void ServiceModePanel::onResetWeigherClicked()
{
    // Reset Weigher to Factory Defaults
    mainWindow_->switchToModeScreen<ServiceMode, serviceWeigherReset>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// SOF-1358
void ServiceModePanel::onResetPrinterClicked()
{
    // Reset Printer to Factory Defaults
    mainWindow_->switchToModeScreen<ServiceMode, servicePrinterReset>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ServiceModePanel::onServiceInstallationLogClicked()
{
    mainWindow_->switchToModeScreen<ServiceMode, ServiceInstallationLog>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ServiceModePanel::onHealthAgentClicked()
{
    mainWindow_->switchToModeScreen<ServiceMode, serviceHealthAgent>();
    accept();
}

// FT-979
///////////////////////////////////////////////////////////////////////////////////////////////////
void ServiceModePanel::onCustomerDisplayOptionClicked()
{
    mainWindow_->switchToModeScreen<ServiceMode, serviceCustomerDisplayOption>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ServiceModePanel::onRemoveLabelsClicked()
{
    mainWindow_->switchToModeScreen<ServiceMode, serviceRemoveLabels>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ServiceModePanel::onSuperNormalScaleOperationsClicked()
{
    mainWindow_->switchToModeScreen<ServiceMode, SuperNormalScaleOperation>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ServiceModePanel::onLabelsFieldsToPrintClicked()
{
    mainWindow_->switchToModeScreen<ServiceMode, labelFieldsToPrint>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ServiceModePanel::onStoreInformationClicked()
{
    mainWindow_->switchToModeScreen<ServiceMode, storeInformation>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ServiceModePanel::onOperatorsCanModifyClicked()
{
    mainWindow_->switchToModeScreen<ServiceMode, operatorsCanModify>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ServiceModePanel::onSuperProductDefaultsClicked()
{
    mainWindow_->switchToModeScreen<ServiceMode, SuperProductDefaults>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ServiceModePanel::onSuperOperatorIdsClicked()
{
    mainWindow_->switchToModeScreen<ServiceMode, SuperOperatorIds>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ServiceModePanel::onBackupRestoreClicked()
{
    mainWindow_->switchToModeScreen<ServiceMode, BackupRestore>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ServiceModePanel::onPrinterLocationsClicked()
{

    mainWindow_->switchToModeScreen<SupervisorMode, printerLocations>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ServiceModePanel::initialize()
{
    MadpPeripheralList *perifList = &session_->scaleConfig->peripheralList;

    // screen layout
    QHBoxLayout *screenLayout = new QHBoxLayout( this );

    screenLayout->setContentsMargins( QMargins( 20, 20, 20, 20 ) );
    screenLayout->addStretch();

    // create button columns
    QVBoxLayout *column;

    ///////////////////////////////////////////////////////////////////////////
    // service mode buttons

    // create our buttons
    initializeButtons();

    // column 0
    column = new QVBoxLayout();
    column->addLayout( createButtonLayout( peripherals_, buttonsPerRow_, tr( "Peripherals" ) ) );

    if( perifList->firstPrinter() || TURN_ON_SERVICE_MENU )
        column->addLayout( createButtonLayout( printer_, buttonsPerRow_, tr( "Printer" ) ) );

    if( perifList->firstWeigher() || TURN_ON_SERVICE_MENU )
        column->addLayout( createButtonLayout( weigher_, buttonsPerRow_, tr( "Weigher" ) ) );

    column->addStretch();
    screenLayout->addLayout( column );
    screenLayout->addStretch();

    // column 1
    column = new QVBoxLayout();

    column->addLayout( createButtonLayout( miscellaneous_, buttonsPerRow_, tr( "Miscellaneous" ) ) );
    column->addLayout( createButtonLayout( supervisorSettings_, buttonsPerRow_, tr( "\n\nSupervisor Settings" ) ) );

    column->addStretch();
    screenLayout->addLayout( column );
    screenLayout->addStretch();

    ///////////////////////////////////////////////////////////////////////////
    // mode buttons

    // add menu buttons
    column = createMenuButtonLayout( ServiceButtonSelected );

    if ( RightDialogLocation == parentDialog_->location() )
        screenLayout->addLayout( column );
    else
        screenLayout->insertLayout( 0, column );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ServiceModePanel::initializeButtons()
{
    QPushButton *pushButton;

    ServiceMode *serviceMode( mainWindow_->retrieveMode<ServiceMode>() );
    MadpKeyActivation serviceModeKeyActivation( serviceMode->usbKeyActivation() );

    const MadpKeyActivation *keyActivation = &serviceModeKeyActivation;
    const MadpPeripheralList *perifList = &session_->scaleConfig->peripheralList;
    const DomWeigherConfigRead *weigherFields = &session_->scaleConfig->weigherConfigRead;
    MadpScaleConfig *config = &session_->scaleConfig->scaleConfig;
    QString licenses  = config->licensedFeatures();
    QString modelName = session_->scaleConfig->getScaleModelName();

    int wayer_model = weigherFields->weigherModel();

    //
    // PERIPHERALS
    //

    pushButton = createButton( peripherals_, tr( "Peripheral\nList" ), 0 );
    pushButton->setObjectName( "peripheralList" );
    connect( pushButton, SIGNAL( clicked() ), this, SLOT( onPeripheralListClicked() ) );

    //
    // PERIPHERALS->PRINTER
    //

    if( perifList->firstPrinter() || TURN_ON_SERVICE_MENU )
    {
        pushButton = createButton( printer_, tr( "Powerup\nTest Results" ), 0 );
        pushButton->setObjectName( "printerPowerupTestResults" );
        connect( pushButton, SIGNAL( clicked() ), this, SLOT( onPrinterPowerupTestResultsClicked() ) );

        pushButton = createButton( printer_, tr( "Configure" ), 0 );
        pushButton->setObjectName( "printerConfigure" );
        connect( pushButton, SIGNAL( clicked() ), this, SLOT( onPrinterConfigurationClicked() ) );

        pushButton = createButton( printer_, tr( "Monitor\nSensors" ), 0 );
        pushButton->setObjectName( "printerMonitorSensors" );
        connect( pushButton, SIGNAL( clicked() ), this, SLOT( onPrinterMonitorSensorsClicked() ) );

        if( session_->uiUtilities->isFeatureLicensed(SCALE_CODE_CHECKER) )
        {
            pushButton = createButton( printer_, tr( "Printer\nDot Status" ), 0 );
            pushButton->setObjectName( "printerHeadDotStatus" );
            connect( pushButton, SIGNAL( clicked() ), this, SLOT( onPrinterHeadDotStatusClicked() ) );
        }


        // SOF-2008: Only display for System Test key
        if ( keyActivation->isQATestKey() || TURN_ON_SERVICE_MENU )
        {
            if( session_->isServiceCounter() )
            {
                pushButton = createButton( printer_, tr( "Reset\nPrinter" ), 0 );
                pushButton->setObjectName( "printerReset" );
                connect( pushButton, SIGNAL( clicked() ), this, SLOT( onResetPrinterClicked() ) );
                pushButton->setEnabled( mainWindow_->isPrimarySessionType() ? true : false ); // FT-465
            }
        }
    }

    //
    // PERIPHERALS->WEIGHER
    //

    if( perifList->firstWeigher() || TURN_ON_SERVICE_MENU )
    {
        pushButton = createButton( weigher_, tr( "Powerup\nTest Results" ), 0 );
        pushButton->setObjectName( "weigherPowerupTestResults" );
        connect( pushButton, SIGNAL( clicked() ), this, SLOT( onWeigherPowerupTestResultsClicked() ) );

        pushButton = createButton( weigher_, tr( "Configure" ), 0 );
        pushButton->setObjectName( "weigherConfigure" );
        connect( pushButton, SIGNAL( clicked() ), this, SLOT( onWeigherConfigurationClicked() ) );

        pushButton = createButton( weigher_, tr( "Information" ), 0 );
        pushButton->setObjectName( "weigherInformation" );
        connect( pushButton, SIGNAL( clicked() ), this, SLOT( onWeigherInformationClicked() ) );

        pushButton = createButton( weigher_, tr( "Calibrate" ), 0 );
        pushButton->setObjectName( "weigherCalibrate" );
        connect( pushButton, SIGNAL( clicked() ), this, SLOT( onWeigherCalibrationClicked() ) );

        // we will have to add other models in the future but for now just HT Weigher.
        if( ( TURN_ON_SERVICE_MENU ||
              SessionMainWindow::HLX1 == wayer_model ||
              SessionMainWindow::SASCALE1 == wayer_model ||     // HT-374
              SessionMainWindow::SASCALE2 == wayer_model ||
              SessionMainWindow::HTM == wayer_model) &&   // SOF-3140
            ( session_->uiUtilities->isFeatureLicensed( SCALE_CAT3_AUDITTRAIL ) ) )
        {
            pushButton = createButton( weigher_, tr( "Audit\nLog" ), 0 );
            pushButton->setObjectName( "weigherAuditLog" );
            connect( pushButton, SIGNAL( clicked() ), this, SLOT( onWeigherAuditLogClicked() ) );
        }

        pushButton = createButton( weigher_, tr( "Rezero\nWeigher" ), 0 );
        pushButton->setObjectName( "weigherRezero" );
        connect( pushButton, SIGNAL( clicked() ), this, SLOT( onRezeroWeigherClicked() ) );
        pushButton->setEnabled( mainWindow_->isPrimarySessionType() ? true : false ); // FT-465

        // SOF-2008: Only display for System Test key
        if ( keyActivation->isQATestKey() || TURN_ON_SERVICE_MENU )
        {
            if( session_->isServiceCounter() )
            {
                pushButton = createButton( weigher_, tr( "Reset\nWeigher" ), 0 );
                pushButton->setObjectName( "weigherReset" );
                connect( pushButton, SIGNAL( clicked() ), this, SLOT( onResetWeigherClicked() ) );
                pushButton->setEnabled( mainWindow_->isPrimarySessionType() ? true : false ); // FT-465
            }
        }
    }

    //
    // MISC
    //

    pushButton = createButton( miscellaneous_, tr( "Pricing\nMode" ), 0 );
    pushButton->setObjectName( "pricingMode" );
    connect( pushButton, SIGNAL( clicked() ), this, SLOT( onPricingModeClicked() ) );

    pushButton = createButton( miscellaneous_, tr( "Installation\nLog" ), 0 );
    pushButton->setObjectName( "installationLog" );
    connect( pushButton, SIGNAL( clicked() ), this, SLOT( onServiceInstallationLogClicked() ) );

    pushButton = createButton( miscellaneous_, tr( "Currency\nSettings" ), 0 );
    pushButton->setObjectName( "currencySettings" );
    connect( pushButton, SIGNAL( clicked() ), this, SLOT( onCurrencySettingsClicked() ) );

    pushButton = createButton( miscellaneous_, tr( "Supervisor\nAccess" ), 0 );
    pushButton->setObjectName( "supervisorOSConfig" );
    connect( pushButton, SIGNAL( clicked() ), this, SLOT( onSupervisorOsAccessClicked() ) );

#ifdef QT_DEBUG
    pushButton = createButton( miscellaneous_, tr( "Empty\nPage" ), 0 );
    pushButton->setObjectName( "emptyPage" );
    connect( pushButton, SIGNAL( clicked() ), this, SLOT( onEmptyPageButtonClicked() ) );
#endif

#ifdef QT_DEBUG
    pushButton = createButton( miscellaneous_, tr( "Test\nScreen" ), 0 );
    pushButton->setObjectName( "testScreen" );
    connect( pushButton, SIGNAL( clicked() ), this, SLOT( onTestPageButtonClicked() ) );
#endif

    if(mainWindow_->sessionType() == PrimarySessionType || TURN_ON_SERVICE_MENU )
    {
        pushButton = createButton( miscellaneous_, tr( "Setup\nUtility" ), 0 );
        pushButton->setObjectName( "setupUtility" );
        connect( pushButton, SIGNAL( clicked() ), this, SLOT( onSetupUtilityButtonClicked() ) );
    }

    if ( keyActivation->isManfTestKey() || TURN_ON_SERVICE_MENU )
    {
        pushButton = createButton( miscellaneous_, tr( "Manufacturing\nPre-Test" ), 0 );
        pushButton->setObjectName( "manufacturingTestPreTest" );
        connect( pushButton, SIGNAL( clicked() ), this, SLOT( onManufacturingTestPreTestButtonClicked() ) );
    }

    // FT-1222
    if ( keyActivation->isManfTestKey() || TURN_ON_SERVICE_MENU )
    {
        pushButton = createButton( miscellaneous_, tr( "Manufacturing\nFinal Test" ), 0 );
        pushButton->setObjectName( "manufacturingTestFinalTest" );
        connect( pushButton, SIGNAL( clicked() ), this, SLOT( onManufacturingTestFinalButtonClicked() ) );
    }

    // FT-1222
    if ( mainWindow_->sessionType() == PrimarySessionType || TURN_ON_SERVICE_MENU )
    {
        pushButton = createButton( miscellaneous_, tr( "Service\nDiagnostics" ), 0 );
        pushButton->setObjectName( "serviceDiagnostics" );
        connect( pushButton, SIGNAL( clicked() ), this, SLOT( onServiceDiagnosticsButtonClicked() ) );
    }

    pushButton = createButton( miscellaneous_, tr( "Health\nAgent" ), 0 );
    pushButton->setObjectName( "healthAgent" );
    connect( pushButton, SIGNAL( clicked() ), this, SLOT( onHealthAgentClicked() ) );

    // FT-979
    if( perifList->gcd() == 0 || TURN_ON_SERVICE_MENU )
    {
        // The Customer Display button is only visible when the GCD is NOT present
        pushButton = createButton( miscellaneous_, tr( "Customer\nDisplay" ), 0 );
        pushButton->setObjectName( "customerDisplayOption" );
        connect( pushButton, SIGNAL( clicked() ), this, SLOT( onCustomerDisplayOptionClicked() ) );
    }

    if(mainWindow_->sessionType() == PrimarySessionType)
    {
        pushButton = createButton( miscellaneous_, tr( "Advanced\nDiagnostics" ), 0 );
        pushButton->setObjectName( "systemDiagnostics" );
        connect( pushButton, SIGNAL( clicked() ), this, SLOT( onSystemDiagnosticsButtonClicked() ) );
    }

    // Remove this feature for the 2nd printer release but leave it available
    pushButton = createButton( miscellaneous_, tr( "Remove\nLabels" ), 0 );
    pushButton->setObjectName( "removeLabels" );
    connect( pushButton, SIGNAL( clicked() ), this, SLOT( onRemoveLabelsClicked() ) );

    //
    // SUPERVISOR SETTINGS
    //

    pushButton = createButton( supervisorSettings_, tr( "Normal\nOperations" ), 0 );
    pushButton->setObjectName( "serviceNormalOperations" );
    connect( pushButton, SIGNAL( clicked() ), this, SLOT( onSuperNormalScaleOperationsClicked() ) );

    pushButton = createButton( supervisorSettings_, tr( "Fields To\nPrint" ), 0 );
    pushButton->setObjectName( "serviceFieldsToPrint" );
    connect( pushButton, SIGNAL( clicked() ), this, SLOT( onLabelsFieldsToPrintClicked() ) );

    pushButton = createButton( supervisorSettings_, tr( "Store\nInformation" ), 0 );
    pushButton->setObjectName( "serviceStoreInformation" );
    connect( pushButton, SIGNAL( clicked() ), this, SLOT( onStoreInformationClicked() ) );

    pushButton = createButton( supervisorSettings_, tr( "Operator\nPermissions" ), 0 );
    pushButton->setObjectName( "serviceOperatorPermissions" );
    connect( pushButton, SIGNAL( clicked() ), this, SLOT( onOperatorsCanModifyClicked() ) );

    pushButton = createButton( supervisorSettings_, tr( "Product\nDefaults" ), 0 );
    pushButton->setObjectName( "serviceProductDefaults" );
    connect( pushButton, SIGNAL( clicked() ), this, SLOT( onSuperProductDefaultsClicked() ) );

    pushButton = createButton( supervisorSettings_, tr( "Configure\nOperators" ), 9 );
    pushButton->setObjectName( "serviceConfigureOperators" );
    connect( pushButton, SIGNAL( clicked() ), this, SLOT( onSuperOperatorIdsClicked() ) );

    // FT-585 Move Frequent Shopper to Supervisor panel
    // FT-597 wants is also back in Service panel.
    // Per Service - move under the Supervisor Settings section on the Service Panel
    pushButton = createButton( supervisorSettings_, tr( "Frequent\nShopper" ), 0 );
    pushButton->setObjectName( "frequentShopper" );
    connect( pushButton, SIGNAL( clicked() ), this, SLOT( onFrequentShopperClicked() ) );

    // FT-800
    if(mainWindow_->sessionType() == PrimarySessionType)
    {
        pushButton = createButton( supervisorSettings_, tr( "Backup /\nRestore" ), 3 );
        pushButton->setObjectName( "backupRestore" );
        connect( pushButton, SIGNAL( clicked() ), this, SLOT( onBackupRestoreClicked() ) );
        //pushButton->setEnabled( mainWindow_->isPrimarySessionType() ? true : false ); // similar issue as FT-465. Dont allow this remotely
    }

    if(mainWindow_->sessionType() == PrimarySessionType)
    {
        // FT-1741
        if( licenses.contains( "SCALE_PRINTER_EPP" ) && modelName.length() > 0 && modelName.left( 3 ) != "HTs" )
        {
            pushButton = createButton( supervisorSettings_, tr( "Printer\nSetup" ), 9 );
            pushButton->setObjectName( "printerLocations" );
            connect( pushButton, SIGNAL( clicked() ), this, SLOT( onPrinterLocationsClicked() ) );
        }
    }
}
