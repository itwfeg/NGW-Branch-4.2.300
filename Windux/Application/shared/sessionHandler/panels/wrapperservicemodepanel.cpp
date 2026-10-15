/**
 * @file wrapperservicemodepanel.cpp
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "wrapperservicemodediagnosticspanel.h"
#include "wrapperservicemodepanel.h"
#include "wrapperservicemodetestspanel.h"
#include "wrapperservicelabelapplierpanel.h"

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
#include "../screens/servicePrinterHeadDotStatus.h"
#include "../screens/serviceWeigherPowerupTestResults.h"
#include "../screens/serviceWeigherConfig.h"
#include "../screens/serviceWeigherCalibrate.h"
#include "../screens/serviceWeigherInfo.h"
#include "../screens/serviceinstallationlog.h"
#include "../screens/serviceHealthAgent.h"
#include "../screens/serviceCustomerDisplayOption.h"
#include "../screens/serviceLabelApplierConfig.h"
//#include "../screens/serviceLabelApplierPPCal.h"
//#include "../screens/serviceLabelApplierPPInfo.h"
#include "../screens/serviceNewWrapperVersionInfo.h"
#include "../screens/serviceNewWrapperSystemInfo.h"
#include "../screens/serviceLabelApplierSensors.h"
#include "../screens/serviceNewWrapperInterlocks.h"
#include "../screens/serviceNewWrapperConfig.h"
#include "../screens/serviceLabelApplierInfo.h"
#include "../screens/serviceLabelApplierTests.h"
#include "../screens/printerLocations.h"
#include "../screens/serviceNewWrapperSetTimings.h"
#include "../screens/serviceNewWrapperEditBox.h"
#include "../screens/serviceNewWrapperErrorHistory.h"
#include "../screens/serviceNewWrapperPkgHistory.h"
#include "../screens/serviceNewWrapperCalibration.h"
#include "../screens/serviceSecurityLabels.h"

#include "../screens/systemDiagnostics.h"   // HT-141
#include "../screens/serviceRemoveLabels.h" // HT-142

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
#include "../screens/removefirmwarefiles.h"

#include <backgroundwidgettypes.h>

#include <ScaleConfiguration.h>
#include <UiUtilities.h>

#include <QPushButton>
#include <QSignalMapper>
#include <QVBoxLayout>

/// Renderer traits type.
typedef hobart::supervisorModePanelBackgroundWidget rendererTraitsType;

const int WrapperServiceModePanel::buttonsPerRowLeft_ = 4;
const int WrapperServiceModePanel::buttonsPerRowRight_ = 4;

const int WrapperServiceModePanel::buttonsPerRowLeft_WrapOnly_ = 3;
const int WrapperServiceModePanel::buttonsPerRowRight_WrapOnly_ = 3;

const bool TURN_ON_SERVICE_MENU = false;
const bool TURN_ON_WRAPPER_MENU = false;
const bool TURN_ON_LABEL_APPLIER_MENU = false;

///////////////////////////////////////////////////////////////////////////////////////////////////
WrapperServiceModePanel::WrapperServiceModePanel( SessionMainWindow *mainWindow, SessionDialog *parent ) :
    _Mybase( mainWindow, parent )
{
    LOG_CLASS_INFO( "ServiceModePanel", "WrapperServiceModePanel" );

    setFixedSize( sizeHint() );
    setObjectName( "WrapperServiceModePanel" );

    // retrieve access level
    if ( mainWindow )
        accessLevel_ = mainWindow->retrieveMode<ServiceMode>()->operatorAccessLevel();

    LOG_DEBUG( "wrapperservicemodepanel constructor access level is %d", accessLevel_ );

    // initialize interface
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
WrapperServiceModePanel::~WrapperServiceModePanel()
{
    LOG_CLASS_INFO( "ServiceModePanel", "WrapperServiceModePanel" );

    LOG_DEBUG( "WSMP Destructor", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QSize WrapperServiceModePanel::sizeHint() const
{
    LOG_CLASS_INFO( "ServiceModePanel", "sizeHint" );
    LOG_DEBUG( "WSMP sizeHint() ", NULL );

    if( session_->uiUtilities->isWrapOnly() )
    {
        return QSize(
            ( ( ( mainWindow_->width() * rendererTraitsType::defaultWidth_ ) / SessionMainWindow::scalingDividerX_ ) * .75 ) ,
            (mainWindow_->height() * rendererTraitsType::defaultHeight_) / SessionMainWindow::scalingDividerY_ );
    }
    else
    {
        return QSize(
            (mainWindow_->width() * rendererTraitsType::defaultWidth_) / SessionMainWindow::scalingDividerX_,
            (mainWindow_->height() * rendererTraitsType::defaultHeight_) / SessionMainWindow::scalingDividerY_ );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
hobart::backgroundWidgetBase *WrapperServiceModePanel::rendererTraits() const
{
    LOG_CLASS_INFO( "ServiceModePanel", "renderTraits" );
    LOG_DEBUG( "WSMP renderTraits() ", NULL );

    return new rendererTraitsType();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QPushButton *WrapperServiceModePanel::createNamedButton( pushButtonList& buttons, const QString& buttonText, const QString& buttonObjectName, int requiredAccessLevel, bool selected )
{
    LOG_CLASS_INFO( "ServiceModePanel", "createNamedButton" );
    LOG_DEBUG( "WSMP createNamedButton() ", NULL );

    QPushButton *pushButton( createButton( buttons, buttonText, requiredAccessLevel, selected ) );

    // give object name and map signal
    if ( pushButton )
    {
        pushButton->setObjectName( buttonObjectName );

        connect( pushButton, SIGNAL( clicked() ), signalMapper_, SLOT( map() ) );
        signalMapper_->setMapping( pushButton, pushButton );
    }

    return pushButton;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WrapperServiceModePanel::onPushButtonPressed( QWidget *widget )
{
    LOG_CLASS_INFO( "ServiceModePanel", "onPushButtonPressed" );
    LOG_DEBUG( "WSMP onPushButtonPressed() ", NULL );

    if ( !widget )
        return;

    QString screen( widget->objectName() );

    if ( screen.isEmpty() )
        return;

    // peripherals
    if ( "peripheralList" == screen )
        mainWindow_->switchToModeScreen<ServiceMode, servicePeripheralList>();

    // misc
    else if ( "pricingMode" == screen )
        mainWindow_->switchToModeScreen<ServiceMode, servicePricingMode>();
    else if ( "installationLog" == screen )
        mainWindow_->switchToModeScreen<ServiceMode, ServiceInstallationLog>();
    else if ( "currencySettings" == screen )
        mainWindow_->switchToModeScreen<ServiceMode, serviceCurrencySetup>();
    else if ( "supervisorOSConfig" == screen )
        mainWindow_->switchToModeScreen<ServiceMode, serviceSupervisorOsAccess>();
    else if ( "setupUtility" == screen )
        mainWindow_->switchToModeScreen<ServiceMode, setupUtility>();
    else if ( "removeFirmwareFiles" == screen )
        mainWindow_->switchToModeScreen<ServiceMode, removeFirmwareFiles>();
    else if( "manufacturingTest" == screen )	//	HA-62
    {
        if ( mainWindow_->switchToModeScreen<ServiceMode, manufacturingTest>() )
        {
            ServiceMode *serviceMode( mainWindow_->retrieveMode<ServiceMode>() );
            manufacturingTest *screen( dynamic_cast<manufacturingTest*>( serviceMode ? serviceMode->screen() : NULL ) );

            if ( screen )
            {
                // Set to Manufacturing Test for a Prepack scale (No pre test or final test).
                screen->setManufacturingTestMode( manufacturingTest::ManufTestPrepack );
            }
        }
    }
    else if( "serviceDiagnostics" == screen)
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
    }
    //else if ( "manufacturingTest" == screen )
    //    mainWindow_->switchToModeScreen<ServiceMode, manufacturingTest>();
    else if ( "healthAgent" == screen )
        mainWindow_->switchToModeScreen<ServiceMode, serviceHealthAgent>();
    else if ( "customerDisplayOption" == screen )
        mainWindow_->switchToModeScreen<ServiceMode, serviceCustomerDisplayOption>();
    else if ( "systemDiagnostics" == screen )   // HT-141
        mainWindow_->switchToModeScreen<ServiceMode, systemDiagnostics>();
    else if ( "removeLabels" == screen )        // HT-142
        mainWindow_->switchToModeScreen<ServiceMode, serviceRemoveLabels>();

    // supervisor
    else if ( "serviceNormalOperations" == screen )
        mainWindow_->switchToModeScreen<ServiceMode, SuperNormalScaleOperation>();
    else if ( "serviceFieldsToPrint" == screen )
        mainWindow_->switchToModeScreen<ServiceMode, labelFieldsToPrint>();
    else if ( "serviceStoreInformation" == screen )
        mainWindow_->switchToModeScreen<ServiceMode, storeInformation>();
    else if ( "serviceOperatorPermissions" == screen )
        mainWindow_->switchToModeScreen<ServiceMode, operatorsCanModify>();
    else if ( "serviceProductDefaults" == screen )
        mainWindow_->switchToModeScreen<ServiceMode, SuperProductDefaults>();
    else if ( "serviceConfigureOperators" == screen )
        mainWindow_->switchToModeScreen<ServiceMode, SuperOperatorIds>();
    else if ( "frequentShopper" == screen )
        mainWindow_->switchToModeScreen<ServiceMode, serviceFrequentShopper>();
    else if ( "backupRestore" == screen )
        mainWindow_->switchToModeScreen<SupervisorMode, BackupRestore>();

    // printer
    else if ( "printerPowerupTestResults" == screen )
        mainWindow_->switchToModeScreen<ServiceMode, servicePrinterPowerupTests>();
    else if ( "printerConfigure" == screen )
        mainWindow_->switchToModeScreen<ServiceMode, servicePrinterConfig>();
    else if ( "printerMonitorSensors" == screen )
        mainWindow_->switchToModeScreen<ServiceMode, servicePrinterSensors>();
	 else if ( "printerHeadDotStatus" == screen )
        mainWindow_->switchToModeScreen<ServiceMode, servicePrinterHeadDotStatus>();
    else if ( "printerLocations" == screen )
        mainWindow_->switchToModeScreen<ServiceMode, printerLocations>();
    else if ( "serviceSecurityLabels" == screen )
        mainWindow_->switchToModeScreen<ServiceMode, serviceSecurityLabels>();


    // weigher
    else if ( "weigherPowerupTestResults" == screen )
        mainWindow_->switchToModeScreen<ServiceMode, serviceWeigherPowerupTestResults>();
    else if ( "weigherInformation" == screen )
        mainWindow_->switchToModeScreen<ServiceMode, serviceWeigherInfo>();
    else if ( "weigherConfigure" == screen )
        mainWindow_->switchToModeScreen<ServiceMode, serviceWeigherConfig>();
    else if ( "weigherCalibrate" == screen )
        mainWindow_->switchToModeScreen<ServiceMode, serviceWeigherCalibrate>();
    else if ( "weigherRezero" == screen )
        session_->uiUtilities->rezeroWeigher();

    // label applier HA-146
    //else if ( "labelApplierPowerupTestResults" == screen )
    //    ; // TODO
    else if ( "labelApplierTests" == screen )
        mainWindow_->switchToModeScreen<ServiceMode, serviceLabelApplierTests>();
    else if ( "labelApplierConfigure" == screen )
        mainWindow_->switchToModeScreen<ServiceMode, serviceLabelApplierConfig>();
    else if ( "labelApplierInfo" == screen )
        mainWindow_->switchToModeScreen<ServiceMode, serviceLabelApplierInfo>();
    else if ( "labelApplierMonitorSensors" == screen )
        mainWindow_->switchToModeScreen<ServiceMode, serviceLabelApplierSensors>();
    //else if( "labelApplierPPCal" == screen )
    //    mainWindow_->switchToModeScreen<ServiceMode, serviceLabelApplierPPCal>();
    //else if( "labelApplierPPInfo" == screen )
    //    mainWindow_->switchToModeScreen<ServiceMode, serviceLabelApplierPPInfo>();

    // wrapper
    else if ( "wrapperConfig" == screen )
    {
        LOG_DEBUG( "WSMP onPushButtonPressed() switchToConfig START ", NULL );
        mainWindow_->switchToModeScreen<ServiceMode, serviceNewWrapperConfig>();
        LOG_DEBUG( "WSMP onPushButtonPressed() switchToConfig DONE ", NULL );
    }
    else if ( "wrapperPackageHistory" == screen )
        mainWindow_->switchToModeScreen<ServiceMode, serviceNewWrapperPkgHistory>();
    else if ( "wrapperTimings" == screen )
    {
        LOG_DEBUG( "WSMP onPushButtonPressed() switchToTimings START ", NULL );

        ServiceMode *serviceMode( mainWindow_->retrieveMode<ServiceMode>() );
        if (serviceMode)
        {
            serviceNewWrapperSetTimings *timings( dynamic_cast<serviceNewWrapperSetTimings*>( serviceMode->screen() ) );
            if (!timings)
                mainWindow_->switchToModeScreen<ServiceMode, serviceNewWrapperSetTimings>();
        }

        LOG_DEBUG( "WSMP onPushButtonPressed() switchToTimings Done ", NULL );
    }
    else if ( "wrapperErrorHistory" == screen )
        mainWindow_->switchToModeScreen<ServiceMode, serviceNewWrapperErrorHistory>();
    else if ( "wrapperSystemInfo" == screen )
        mainWindow_->switchToModeScreen<ServiceMode, serviceNewWrapperSystemInfo>();
    else if ( "wrapperTests" == screen )
        mainWindow_->showMenuPanel( SessionMainWindow::ServiceMenuPanelMode, WrapperServiceModeTestsPanel::WrapperTests );
    else if ( "wrapperDiagnostics" == screen )
        mainWindow_->showMenuPanel( SessionMainWindow::ServiceMenuPanelMode, WrapperServiceModeDiagnosticsPanel::WrapperDiagnostics );
    else if ( "wrapperInterlocks" == screen )
        mainWindow_->switchToModeScreen<ServiceMode, serviceNewWrapperInterlocks>();
    else if ( "wrapperVersionInfo" == screen )
        mainWindow_->switchToModeScreen<ServiceMode, serviceNewWrapperVersionInfo>();
    else if ( "wrapperCalibrate" == screen )
        mainWindow_->switchToModeScreen<ServiceMode, serviceNewWrapperCalibration>();
    else if ( "labelApplierConfig" == screen )
        mainWindow_->showMenuPanel( SessionMainWindow::ServiceMenuPanelMode, WrapperServiceModeLAPanel::LabelApplierConfig );

    LOG_DEBUG( "WSMP onPushButtonPressed() BEFORE accept() ", NULL );

    accept();

    LOG_DEBUG( "WSMP onPushButtonPressed() AFTER accept() ", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WrapperServiceModePanel::initialize()
{
    LOG_CLASS_INFO( "ServiceModePanel", "WrapperServiceModePanel" );
    LOG_DEBUG( "WSMP init() begin ", NULL );

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
    LOG_DEBUG( "WSMP init() Call intializeButtons() ", NULL );
    initializeButtons();
    LOG_DEBUG( "WSMP returned from initializeButtons()", NULL );

    // PJR-145: Wrap-only
    int tmpButtonsPerRowLeft = ( session_->uiUtilities->isWrapOnly() ? buttonsPerRowLeft_WrapOnly_ : buttonsPerRowLeft_ );
    int tmpButtonsPerRowRight = ( session_->uiUtilities->isWrapOnly() ? buttonsPerRowRight_WrapOnly_ : buttonsPerRowRight_ );


    // column 0
    column = new QVBoxLayout();
    LOG_DEBUG( "WSMP init() Call layout perifs ", NULL );

    column->addLayout( createButtonLayout( peripherals_, tmpButtonsPerRowLeft, tr( "Peripherals" ) ) );
    LOG_DEBUG( "WSMP init() Call layout misc ", NULL );

    column->addLayout( createButtonLayout( miscellaneous_, tmpButtonsPerRowLeft, tr( "Miscellaneous" ) ) );
    LOG_DEBUG( "WSMP init() Call layout supersettings ", NULL );

    column->addLayout( createButtonLayout( supervisorSettings_, tmpButtonsPerRowLeft, tr( "Supervisor Settings" ) ) );

    column->addStretch();
    screenLayout->addLayout( column );
    screenLayout->addStretch();

    // column 1
    column = new QVBoxLayout();

    if( perifList )
    {

	    if( !( session_->uiUtilities->isWrapOnly() ) )
	    {
	        if ( perifList->isPrinterAttached() || TURN_ON_SERVICE_MENU )
	            column->addLayout( createButtonLayout( printer_, tmpButtonsPerRowRight, tr( "Printer" ) ) );

	        if ( perifList->isWeigherAttached() || TURN_ON_SERVICE_MENU )
	            column->addLayout( createButtonLayout( weigher_, tmpButtonsPerRowRight, tr( "Weigher" ) ) );

	        if ((( perifList->isLabelApplierAttached() ) && ( !perifList->isWrapperAttached())) || TURN_ON_LABEL_APPLIER_MENU )
	            column->addLayout( createButtonLayout( labelApplier_, tmpButtonsPerRowRight, tr( "Label Applier" ) ) );
	    }

	    if ( perifList->isWrapperAttached() || TURN_ON_WRAPPER_MENU )
	        column->addLayout( createButtonLayout( wrapper_, tmpButtonsPerRowRight, tr( "Wrapper" ) ) );

	    LOG_DEBUG( "WSMP init() if PerifList End ", NULL );

    }

    column->addStretch();
    screenLayout->addLayout( column );
    screenLayout->addStretch();

    LOG_DEBUG( "WSMP init() Added Stretches ", NULL );

    ///////////////////////////////////////////////////////////////////////////
    // mode buttons

    // add menu buttons
    LOG_DEBUG( "WSMP init() Call createMenuButtonLayout() START ", NULL );
    column = createMenuButtonLayout( ServiceButtonSelected );
    LOG_DEBUG( "WSMP init() Returned createMenuButtonLayout() END ", NULL );

    if ( RightDialogLocation == parentDialog_->location() )
        screenLayout->addLayout( column );
    else
        screenLayout->insertLayout( 0, column );

    LOG_DEBUG( "WSMP init() end ", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WrapperServiceModePanel::initializeButtons()
{
    LOG_CLASS_INFO( "ServiceModePanel", "initializeButtons" );
    LOG_DEBUG( "WSMP initializeButtons() Start", NULL );

    QPushButton *pushButton;

    ServiceMode *serviceMode( mainWindow_->retrieveMode<ServiceMode>() );
    MadpKeyActivation serviceModeKeyActivation( serviceMode->usbKeyActivation() );

    const MadpKeyActivation *keyActivation = &serviceModeKeyActivation;
    const MadpPeripheralList *perifList = &session_->scaleConfig->peripheralList;

    // create signal mapper for buttons
    signalMapper_ = new QSignalMapper( this );

    connect( signalMapper_, SIGNAL( mapped(QWidget*) ), SLOT( onPushButtonPressed(QWidget*) ) );

    LOG_DEBUG( "WSMP initializeButtons() Done initializing", NULL );

    //
    // PERIPHERALS
    //

    createNamedButton( peripherals_, tr( "Peripheral\nList" ), "peripheralList" );

    LOG_DEBUG( "WSMP initializeButtons() Perifs Done", NULL );

    //
    // MISC
    //

    if( !( session_->uiUtilities->isWrapOnly() ) )
        createNamedButton( miscellaneous_, tr( "Pricing\nMode" ), "pricingMode" );

    createNamedButton( miscellaneous_, tr( "Installation\nLog" ), "installationLog" );

    if( !( session_->uiUtilities->isWrapOnly() ) )
        createNamedButton( miscellaneous_, tr( "Currency\nSettings" ), "currencySettings" );

    createNamedButton( miscellaneous_, tr( "Supervisor\nAccess" ), "supervisorOSConfig" );
    createNamedButton( miscellaneous_, tr( "Remove\nFirmware" ), "removeFirmwareFiles" );

    if( TURN_ON_SERVICE_MENU )
        createNamedButton( miscellaneous_, tr( "Setup\nUtility" ), "setupUtility" );

    if ( mainWindow_->isPrimarySessionType() && !TURN_ON_SERVICE_MENU )
        createNamedButton( miscellaneous_, tr( "Setup\nUtility" ), "setupUtility" );

    if ( keyActivation->isManfTestKey() || TURN_ON_SERVICE_MENU )
    {
        createNamedButton( miscellaneous_, tr( "Manufacturing\nTest" ), "manufacturingTest" );
    }

    // FT-1222
    if ( mainWindow_->sessionType() == PrimarySessionType || TURN_ON_SERVICE_MENU )
    {
        createNamedButton( miscellaneous_, tr( "Service\nDiagnostics" ), "serviceDiagnostics" );
    }

    createNamedButton( miscellaneous_, tr( "Health\nAgent" ), "healthAgent" );

    // HT-141
    if ( mainWindow_->isPrimarySessionType() || TURN_ON_SERVICE_MENU )
    {
        createNamedButton( miscellaneous_, tr( "Advanced\nDiagnostics" ), "systemDiagnostics" );
    }

    // HT-142
    if( !( session_->uiUtilities->isWrapOnly() ) )
        createNamedButton( miscellaneous_, tr( "Remove\nLabels" ), "removeLabels" );

    LOG_DEBUG( "WSMP initializeButtons() Misc Done", NULL );


//	HA-72:  GCD/Customer display does not exist on a Prepack scale
//    // FT-979
//    // The Customer Display button is only visible when the GCD is NOT present
//    if ( 0 == perifList->gcd() )
//        createNamedButton( miscellaneous_, tr( "Customer\nDisplay" ), "customerDisplayOption" );

    //
    // SUPERVISOR SETTINGS
    //

    createNamedButton( supervisorSettings_, tr( "Normal\nOperations" ), "serviceNormalOperations" );

    if( !( session_->uiUtilities->isWrapOnly() ) )
    {
        createNamedButton( supervisorSettings_, tr( "Fields To\nPrint" ), "serviceFieldsToPrint" );
        createNamedButton( supervisorSettings_, tr( "Store\nInformation" ), "serviceStoreInformation" );
        createNamedButton( supervisorSettings_, tr( "Operator\nPermissions" ), "serviceOperatorPermissions" );
        createNamedButton( supervisorSettings_, tr( "Product\nDefaults" ), "serviceProductDefaults" );
    }

    // MAR-48
    if( !( session_->uiUtilities->isWrapOnly() ) )
        createNamedButton( supervisorSettings_, tr( "Configure\nOperators" ), "serviceConfigureOperators", 9 );

    if( !( session_->uiUtilities->isWrapOnly() ) )
    {
        // FT-585 Move Frequent Shopper to Supervisor panel
        // FT-597 wants is also back in Service panel.
        // Per Service - move under the Supervisor Settings section on the Service Panel
        createNamedButton( supervisorSettings_, tr( "Frequent\nShopper" ), "frequentShopper" );
    }

    // FT-800
    if ( mainWindow_->isPrimarySessionType() )
        createNamedButton( supervisorSettings_, tr( "Backup /\nRestore" ), "backupRestore", 3 );

    LOG_DEBUG( "WSMP initializeButtons() Supervisor Done", NULL );

    if( !( session_->uiUtilities->isWrapOnly() ) )
    {
        //
        // PERIPHERALS->PRINTER
        //

        if ( perifList->isPrinterAttached() || TURN_ON_SERVICE_MENU )
        {
            createNamedButton( printer_, tr( "Powerup\nTest Results" ), "printerPowerupTestResults" );
            createNamedButton( printer_, tr( "Configure" ), "printerConfigure" );
            createNamedButton( printer_, tr( "Monitor\nSensors" ), "printerMonitorSensors" );            

            if( session_->uiUtilities->isFeatureLicensed(SCALE_CODE_CHECKER) )
            {
                createNamedButton( printer_, tr( "Printer\nDot Status" ), "printerHeadDotStatus" );
            }

            createNamedButton( printer_, tr( "Set Printer\nLocation" ), "printerLocations" );
            createNamedButton( printer_, tr( "Security\nLabel Applier" ), "serviceSecurityLabels" );	//	HA-117
        }

    	LOG_DEBUG( "WSMP initializeButtons() Printer Done", NULL );

        //
        // PERIPHERALS->WEIGHER
        //
        if ( perifList->isWeigherAttached() || TURN_ON_SERVICE_MENU )
        {
            createNamedButton( weigher_, tr( "Powerup\nTest Results" ), "weigherPowerupTestResults" );
            createNamedButton( weigher_, tr( "Information" ), "weigherInformation" );
            createNamedButton( weigher_, tr( "Configure" ), "weigherConfigure" );
            createNamedButton( weigher_, tr( "Calibrate" ), "weigherCalibrate" );

            if ( (pushButton = createNamedButton( weigher_, tr( "Rezero\nWeigher" ), "weigherRezero" )) )
                pushButton->setEnabled( mainWindow_->isPrimarySessionType() ? true : false ); // FT-465
        }

		LOG_DEBUG( "WSMP initializeButtons() Weigher Done", NULL );
    }

    LOG_DEBUG( "WSMP initializeButtons() LabelApplier Done", NULL );

    //
    // PERIPHERALS->WRAPPER
    //
    if ( perifList->isWrapperAttached() || TURN_ON_WRAPPER_MENU )
    {
        createNamedButton( wrapper_, tr( "Configure" ), "wrapperConfig" );
        createNamedButton( wrapper_, tr( "Package\nHistory" ), "wrapperPackageHistory" );
        createNamedButton( wrapper_, tr( "Timings" ), "wrapperTimings" );
        createNamedButton( wrapper_, tr( "Error History" ), "wrapperErrorHistory" );

        bool haveBLDCBoards = true;
        DomEvolWrapperSystemInformation *evolWrapperSystemInformation = &session_->scaleConfig->evolWrapperSystemInformation;
		if (evolWrapperSystemInformation)
		{
#ifdef USE_QDEBUG
			qDebug() << "WrapperServiceModePanel::initializeButtons():       boardPartNumber = [" + evolWrapperSystemInformation->boardPartNumber() + "]";
			qDebug() << "WrapperServiceModePanel::initializeButtons():              boardRev = [" + evolWrapperSystemInformation->boardRev() + "]";
			qDebug() << "WrapperServiceModePanel::initializeButtons():  boardManufactureDate = [" + evolWrapperSystemInformation->boardManufactureDate() + "]";
#endif
            if ((evolWrapperSystemInformation->boardPartNumber() == "000000000") &&
                    (evolWrapperSystemInformation->boardRev() == "00") &&
                    (evolWrapperSystemInformation->boardManufactureDate() == "000000"))
                haveBLDCBoards = true;
            else
				haveBLDCBoards = false;
        }
        if (!haveBLDCBoards)
	        createNamedButton( wrapper_, tr( "System Info" ), "wrapperSystemInfo" );

        createNamedButton( wrapper_, tr( "Tests" ), "wrapperTests" );
        createNamedButton( wrapper_, tr( "Diagnostics" ), "wrapperDiagnostics" );
        createNamedButton( wrapper_, tr( "Interlocks" ), "wrapperInterlocks" );
        createNamedButton( wrapper_, tr( "Version Info" ), "wrapperVersionInfo" );
        createNamedButton( wrapper_, tr( "Calibrate" ), "wrapperCalibrate" );

        // SOF-5460 Service Mode UI Screen - no longer need two sliding label appliers with TOF Sensors
        //if( session_->scaleConfig->isWrapperTwoPrinter() || TURN_ON_LABEL_APPLIER_MENU )
        //    createNamedButton( wrapper_, tr( "Label\nApplier" ), "labelApplierConfig" );
    }

    LOG_DEBUG( "WSMP initializeButtons() Wrapper Done", NULL );

    LOG_DEBUG( "WSMP initializeButtons() END", NULL );
}
