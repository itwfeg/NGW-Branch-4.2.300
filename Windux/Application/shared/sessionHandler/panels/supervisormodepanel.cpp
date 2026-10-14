/**
 * @file supervisormodepanel.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#if 1
//	DEBUG_QT_LOCAL will allow us to see all of the menu picks if we are not the primary session.
#define DEBUG_QT_LOCAL
#endif

static const bool isDebugBuild = false;


#include <logmanager.h>

#include "supervisormodepanel.h"

#include "../sessionmainwindow.h"
#include "../sessionmainwindowimpl.h"
#include "../sessiondialog.h"

#include "../modes/operatemode.h"
#include "../modes/supervisormode.h"

#include "../screens/setupUtility.h"
#include "../screens/BackupRestore.h"
#include "../screens/marqueeconfigscreen.h"
#include "../screens/labelGraphics.h"
#include "../screens/labeltypesscreen.h"
#include "../screens/scaleStatistics.h"
#include "../screens/superOperatorIds.h"
#include "../screens/superOperatorChanges.h"
#include "../screens/superHourlyTotals.h"
#include "../screens/superProductTotals.h"
#include "../screens/superOperatorTotals.h"
#include "../screens/superSupervisorChanges.h"
#include "../screens/superTotalTypesSetup.h"
#include "../screens/supervoidtransactions.h"
#include "../screens/superProductDefaults.h"
#include "../screens/labelFieldsToPrint.h"
#include "../screens/operatorsCanModify.h"
#include "../screens/shelfLifeExpiration.h"
#include "../screens/storeInformation.h"
#include "../screens/deleteAllFiles.h"
#include "../screens/labelTextNames.h"
#include "../screens/superPrintConfig.h"
#include "../screens/superChangeProductClass.h"
#include "../screens/superChangeProductUnitPrice.h"
#include "../screens/superCommInterface.h"
#include "../screens/superScaleSynchronization.h"
#include "../screens/superPrintSetup.h"
#include "../screens/superReportingClasses.h"
#include "../screens/superChangeProductAllFields.h"
#include "../screens/superChangeProductPriceFields.h"
#include "../screens/superCoolMain.h"
#include "../screens/superUpgradeFirmwareViaSFTP.h"
#include "../screens/superSpecials.h"
#include "../screens/superCategories.h"
#include "../screens/superScannedCodes.h"
#include "../screens/superUpgradeFirmwareViaUSB.h"
#include "../screens/superCoolCreateCategories.h"
#include "../screens/superCoolCreatePretext.h"
#include "../screens/superCoolCreateProdSteps.h"
#include "../screens/superCoolCreateCountries.h"
#include "../screens/superChangeProductTare.h"
#include "../screens/superChangeProductShelfLife.h"
#include "../screens/superEmailSetup.h"
#include "../screens/plumainscreen.h"
#include "../screens/webbrowserscreen.h"
#include "../screens/superNormalScaleOperation.h"
#include "../screens/superPrinterSetup.h"
#include "../screens/superCustomerDisplay.h"
#include "../screens/flashkeyeditor.h"
#include "../screens/serviceFrequentShopper.h"
#include "../screens/superApplierRotationOverride.h"
// HTA-149 #include "../screens/superScaleManagement.h"
#include "../screens/printerLocations.h"
#include "../screens/superSecurityLabels.h"
#include "../screens/serviceHealthAgent.h"
#include "../screens/superDepartments.h"
#include "../screens/servicePrinterHeadDotStatus.h"

// RSD: Added to test
#include "../dialogs/texteditor.h"
// RSD: End of additions

#include <backgroundwidgettypes.h>
#include <font.h>
#include <palette.h>
#include <pushbutton.h>

#include <ScaleConfiguration.h>
#include <UiUtilities.h>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>

/// Renderer traits type.
typedef hobart::supervisorModePanelBackgroundWidget rendererTraitsType;

/// Compact renderer traits type.
typedef hobart::supervisorModeCompactPanelBackgroundWidget compactRendererTraitsType;

const int SupervisorModePanel::buttonsPerRow_ = 2;

///////////////////////////////////////////////////////////////////////////////////////////////////
SupervisorModePanel::SupervisorModePanel( int panelEx, SessionMainWindow *mainWindow, SessionDialog *parent ) :
    _Mybase( mainWindow, parent ),
    compactMenu_( ((NormalPanelFlag & panelEx) ? false : true) )
{
    LOG_CLASS_INFO( "SupervisorModePanel", "SupervisorModePanel" );

    setFixedSize( sizeHint() );
    setObjectName( "SupervisorModePanel" );

    // retrieve access level
    if ( mainWindow )
        accessLevel_ = mainWindow->retrieveMode<SupervisorMode>()->operatorAccessLevel();

    LOG_INFO( "access level is %d", accessLevel_ );

    // initialize interface
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
SupervisorModePanel::~SupervisorModePanel()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QSize SupervisorModePanel::sizeHint() const
{
    QSize s;

    // compute size
    if ( compactMenu_ )
    {
        if( session_->uiUtilities->isWrapOnly() )
        {
            s.setWidth( ( ( ( mainWindow_->width() * compactRendererTraitsType::defaultWidth_ ) / SessionMainWindow::scalingDividerX_ ) * .75 ) );
            s.setHeight( (mainWindow_->height() * compactRendererTraitsType::defaultHeight_) / SessionMainWindow::scalingDividerY_ );
        }
        else
        {
            s.setWidth( (mainWindow_->width() * compactRendererTraitsType::defaultWidth_) / SessionMainWindow::scalingDividerX_ );
            s.setHeight( (mainWindow_->height() * compactRendererTraitsType::defaultHeight_) / SessionMainWindow::scalingDividerY_ );
        }
    }
    else
    {
        if( session_->uiUtilities->isWrapOnly() )
        {
            s.setWidth( ( ( ( mainWindow_->width() * rendererTraitsType::defaultWidth_ ) / SessionMainWindow::scalingDividerX_ ) * .75 ) );
            s.setHeight( (mainWindow_->height() * rendererTraitsType::defaultHeight_) / SessionMainWindow::scalingDividerY_ );
        }
        else
        {
            s.setWidth( (mainWindow_->width() * rendererTraitsType::defaultWidth_) / SessionMainWindow::scalingDividerX_ );
            s.setHeight( (mainWindow_->height() * rendererTraitsType::defaultHeight_) / SessionMainWindow::scalingDividerY_ );
        }

        // adjust width by dialog offset to take up entire screen
        const int offset( parentDialog_->dialogOffset() );

        s.rwidth() += ((mainWindow_->width() * offset) / SessionMainWindow::scalingDividerX_) - offset;
    }

    // give result to caller
    return s;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
hobart::backgroundWidgetBase *SupervisorModePanel::rendererTraits() const
{
    if ( compactMenu_ )
        return new compactRendererTraitsType();

    return new rendererTraitsType();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SupervisorModePanel::onExpandMenuClicked()
{
    SupervisorMode *supervisorMode( mainWindow_->retrieveMode<SupervisorMode>() );
    supervisorMode->setMenuPanelState( NormalPanelFlag );

    mainWindow_->showMenuPanel( SessionMainWindow::SupervisorMenuPanelMode );
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SupervisorModePanel::onBackupRestoreClicked()
{
    mainWindow_->switchToModeScreen<SupervisorMode, BackupRestore>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SupervisorModePanel::onDepartmentsClicked()
{
    mainWindow_->switchToModeScreen<SupervisorMode, SuperDepartmentsMaint>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SupervisorModePanel::onSuperNormalScaleOperationsClicked()
{
    mainWindow_->switchToModeScreen<SupervisorMode, SuperNormalScaleOperation>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SupervisorModePanel::onlabelGraphicsButtonClicked()
{
    mainWindow_->switchToModeScreen<SupervisorMode, labelGraphics>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SupervisorModePanel::onLabelTypesButtonClicked()
{
    mainWindow_->switchToModeScreen<SupervisorMode, LabelTypesScreen>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SupervisorModePanel::onMarqueeConfigButtonClicked()
{
    mainWindow_->switchToModeScreen<SupervisorMode, MarqueeConfigScreen>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SupervisorModePanel::onScaleStatisticsButtonClicked()
{
    mainWindow_->switchToModeScreen<SupervisorMode, scaleStatistics>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SupervisorModePanel::onSuperOperatorIdsClicked()
{
	mainWindow_->switchToModeScreen<SupervisorMode, SuperOperatorIds>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SupervisorModePanel::onSuperOperatorChangesClicked()
{
	mainWindow_->switchToModeScreen<SupervisorMode, SuperOperatorChanges>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SupervisorModePanel::onSuperOperatorNoteClicked()
{
    // request operator note for editing
    session_->send( OperatorDev, OpReadNoteForEditing );
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SupervisorModePanel::onSuperHourlyTotalsClicked()
{
	mainWindow_->switchToModeScreen<SupervisorMode, SuperHourlyTotals>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SupervisorModePanel::onSuperProductTotalsClicked()
{
	mainWindow_->switchToModeScreen<SupervisorMode, SuperProductTotals>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SupervisorModePanel::onSuperSupervisorChangesClicked()
{
    mainWindow_->switchToModeScreen<SupervisorMode, SuperSupervisorChanges>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SupervisorModePanel::onSuperOperatorTotalsClicked()
{
    mainWindow_->switchToModeScreen<SupervisorMode, SuperOperatorTotals>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SupervisorModePanel::onSuperTotalTypesSetupClicked()
{
    mainWindow_->switchToModeScreen<SupervisorMode, SuperTotalTypesSetup>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SupervisorModePanel::onVoidTransactionsClicked()
{
	mainWindow_->switchToModeScreen<SupervisorMode, SuperVoidTransactions>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SupervisorModePanel::onSuperProductDefaultsClicked()
{
    mainWindow_->switchToModeScreen<SupervisorMode, SuperProductDefaults>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SupervisorModePanel::onLabelsFieldsToPrintClicked()
{
    mainWindow_->switchToModeScreen<SupervisorMode, labelFieldsToPrint>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SupervisorModePanel::onOperatorsCanModifyClicked()
{
    mainWindow_->switchToModeScreen<SupervisorMode, operatorsCanModify>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SupervisorModePanel::onShelfLifeExpirationClicked()
{
    mainWindow_->switchToModeScreen<SupervisorMode, shelfLifeExpiration>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SupervisorModePanel::onStoreInformationClicked()
{
    mainWindow_->switchToModeScreen<SupervisorMode, storeInformation>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SupervisorModePanel::onDeleteAllFilesClicked()
{
    mainWindow_->switchToModeScreen<SupervisorMode, deleteAllFiles>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SupervisorModePanel::onLabelTextNamesClicked()
{
    mainWindow_->switchToModeScreen<SupervisorMode, labelTextNames>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SupervisorModePanel::onSecurityLabelsClicked()
{
    mainWindow_->switchToModeScreen<SupervisorMode, superSecurityLabels>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SupervisorModePanel::onPrintConfigClicked()
{
    mainWindow_->switchToModeScreen<SupervisorMode, superPrintConfig>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SupervisorModePanel::onCommInterfaceClicked()
{
    mainWindow_->switchToModeScreen<SupervisorMode, superCommInterface>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SupervisorModePanel::onScaleSynchronizationClicked()
{
    mainWindow_->switchToModeScreen<SupervisorMode, SuperScaleSynchronization>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SupervisorModePanel::onPrintSetupClicked()
{
    mainWindow_->switchToModeScreen<SupervisorMode, superPrintSetup>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SupervisorModePanel::onSuperChangeProductUnitPriceClicked()
{
    MadpInternalConfig *internalConfigData = &session_->scaleConfig->internalConfig;
    internalConfigData->changeProductActive = true;
    mainWindow_->switchToModeScreen<SupervisorMode, superChangeProductUnitPrice>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SupervisorModePanel::onSuperChangeProductClassClicked()
{
    MadpInternalConfig *internalConfigData = &session_->scaleConfig->internalConfig;
    internalConfigData->changeProductActive = true;    mainWindow_->switchToModeScreen<SupervisorMode, superChangeProductClass>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SupervisorModePanel::onSuperReportingClassesClicked()
{

    mainWindow_->switchToModeScreen<SupervisorMode, superReportingClasses>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SupervisorModePanel::onSuperChangeProductTareClicked()
{
    MadpInternalConfig *internalConfigData = &session_->scaleConfig->internalConfig;
    internalConfigData->changeProductActive = true;
    mainWindow_->switchToModeScreen<SupervisorMode, superChangeProductTare>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SupervisorModePanel::onSuperChangeProductShelfLifeClicked()
{
    MadpInternalConfig *internalConfigData = &session_->scaleConfig->internalConfig;
    internalConfigData->changeProductActive = true;
    mainWindow_->switchToModeScreen<SupervisorMode, superChangeProductShelfLife>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SupervisorModePanel::onSuperChangeProductAllFieldsClicked()
{
    MadpInternalConfig *internalConfigData = &session_->scaleConfig->internalConfig;
    internalConfigData->changeProductActive = true;
    mainWindow_->switchToModeScreen<SupervisorMode, superChangeProductAllFields>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SupervisorModePanel::onSuperChangeProductPriceFieldsClicked()
{
    MadpInternalConfig *internalConfigData = &session_->scaleConfig->internalConfig;
    internalConfigData->changeProductActive = true;
    mainWindow_->switchToModeScreen<SupervisorMode, superChangeProductPriceFields>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SupervisorModePanel::onSuperEmailSetupClicked()
{
    mainWindow_->switchToModeScreen<SupervisorMode, superEmailSetup>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SupervisorModePanel::onSuperCoolMainClicked()
{
    mainWindow_->switchToModeScreen<SupervisorMode, SuperCoolMain>();		// Cool Assign to Categories
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SupervisorModePanel::onSuperUpgradeFirmwareViaSFTPClicked()
{
    mainWindow_->switchToModeScreen<SupervisorMode, superUpgradeFirmwareViaSFTP>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SupervisorModePanel::onRezeroWeigherClicked()
{
    session_->uiUtilities->rezeroWeigher();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SupervisorModePanel::onSetupUtilityButtonClicked()
{
    mainWindow_->switchToModeScreen<SupervisorMode, setupUtility>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SupervisorModePanel::onSuperSpecialsClicked()
{
    mainWindow_->switchToModeScreen<SupervisorMode, superSpecials>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SupervisorModePanel::onSuperCategoriesClicked()
{
    mainWindow_->switchToModeScreen<SupervisorMode, superCategories>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SupervisorModePanel::onSuperScannedCodesClicked()
{
    mainWindow_->switchToModeScreen<SupervisorMode, SuperScannedCodes>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SupervisorModePanel::onSuperUpgradeFirmwareViaUSBClicked()
{
    mainWindow_->switchToModeScreen<SupervisorMode, superUpgradeFirmwareViaUSB>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SupervisorModePanel::onSuperCoolCreateCategoriesClicked()
{
	mainWindow_->switchToModeScreen<SupervisorMode, SuperCoolCreateCategories>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SupervisorModePanel::onSuperCoolCreatePretextClicked()
{
	mainWindow_->switchToModeScreen<SupervisorMode, SuperCoolCreatePretext>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SupervisorModePanel::onSuperCoolCreateProdStepsClicked()
{
	mainWindow_->switchToModeScreen<SupervisorMode, SuperCoolCreateProdSteps>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SupervisorModePanel::onSuperCoolCreateCountriesClicked()
{
	mainWindow_->switchToModeScreen<SupervisorMode, SuperCoolCreateCountries>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SupervisorModePanel::onPrinterSetupClicked()
{
    MadpScaleConfig *config = &session_->scaleConfig->scaleConfig;
    QString licenses  = config->licensedFeatures();
    QString modelName = session_->scaleConfig->getScaleModelName();

    if(!session_->isPrepack())
    {
        // FT-1741
        if( licenses.contains( "SCALE_PRINTER_EPP" ) && modelName.length() > 0 && modelName.left( 3 ) != "HTs" )
            mainWindow_->switchToModeScreen<SupervisorMode, printerLocations>();
        else
            mainWindow_->switchToModeScreen<SupervisorMode, superPrinterSetup>();
    }
    else
    {
        mainWindow_->switchToModeScreen<SupervisorMode, printerLocations>();
    }

    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SupervisorModePanel::onCustomerDisplayClicked()
{
    mainWindow_->switchToModeScreen<SupervisorMode, superCustomerDisplay>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SupervisorModePanel::onOperateModeLayoutMoveLayout()
{
    if ( mainWindow_->switchToModeScreen<OperateMode, PluMainScreen>() )
    {
        OperateMode *operateMode = mainWindow_->retrieveMode<OperateMode>();
        operateMode->setWidgetDragLock( false );
    }

    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SupervisorModePanel::onOperateModeLayoutChangeFields()
{
    OperateMode *operateMode = mainWindow_->retrieveMode<OperateMode>();
    bool okay = false;

    // if already on plu main or web browser screen there is no need to switch
    if ( mainWindow_->isOperateMode() )
        if (( dynamic_cast<PluMainScreen*>( operateMode->screen() ) ) || ( dynamic_cast<WebBrowserScreen*>( operateMode->screen() ) ))
            okay = true;

    // switch screens
    if (( okay ) || ( mainWindow_->navigateHome() ))
        operateMode->setWidgetSettingsLock( false );

    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SupervisorModePanel::onFlashkeyEditorClicked()
{
    mainWindow_->switchToModeScreen<SupervisorMode, FlashkeyEditor>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SupervisorModePanel::onFrequentShopperClicked()
{
    mainWindow_->switchToModeScreen<SupervisorMode, serviceFrequentShopper>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SupervisorModePanel::onSuperApplierRotationOverrideClicked()
{
    mainWindow_->switchToModeScreen<SupervisorMode, superApplierRotationOverride>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/* HTA-149
 * void SupervisorModePanel::onScaleManagementButtonClicked()
{
    mainWindow_->switchToModeScreen<SupervisorMode, superScaleManagement>();
    accept();
}
*/

///////////////////////////////////////////////////////////////////////////////////////////////////
void SupervisorModePanel::onHealthAgentClicked()
{
    mainWindow_->switchToModeScreen<SupervisorMode, serviceHealthAgent>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SupervisorModePanel::onPrinterDotStatusClicked(){
    mainWindow_->switchToModeScreen<SupervisorMode, servicePrinterHeadDotStatus>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SupervisorModePanel::initialize()
{
    // screen layout
    QHBoxLayout *screenLayout = new QHBoxLayout( this );

    screenLayout->setContentsMargins( QMargins( 20, 20, 20, 20 ) );
    screenLayout->addStretch();

    // create button columns
    QVBoxLayout *column;

    ///////////////////////////////////////////////////////////////////////////
    // supervisor mode buttons

    // create our buttons
    initializeButtons();

    // compact menu
    if ( compactMenu_ )
    {
        const QSize expandButtonSize( (window()->width() * 180) / 1024, (window()->height() * 45) / 600 );

        const hobart::backgroundWidgetRenderer renderExpandButton(
            new hobart::panelButtonBackgroundWidget( 4 ),
            new hobart::transparentBackgroundWidgetRendererAppearance() );

        const QPixmap expandButtonPixmap( renderExpandButton.drawControl( expandButtonSize, hobart::ReadOnly ) );

        modifyFontBold setFontBold( true );
        modifyPaletteForeground setForeground( modifyPaletteForeground::createPalette( hobart::colorPalette::blue ) );

        // create expand button
        expandButton_ = new hobart::pushButton(
            (RightDialogLocation == parentDialog_->location() ? tr( "< ALL FUNCTIONS" ) : tr( "ALL FUNCTIONS >" )),
            this,
            expandButtonPixmap );

        expandButton_->setObjectName( "expandButton" );
        setFontBold( expandButton_ );
        setForeground( expandButton_ );

        connect( expandButton_, SIGNAL( clicked() ), SLOT( onExpandMenuClicked() ) );

        // column 0
        if( !( session_->uiUtilities->isWrapOnly() ) )
        {
            column = new QVBoxLayout();
            column->addLayout( createButtonLayout( productChanges_, buttonsPerRow_, tr( "Product Changes" ) ) );
            column->addLayout( createButtonLayout( otherRecords_, buttonsPerRow_, tr( "Support Records" ) ) );
            column->addLayout( createButtonLayout( transactions_, buttonsPerRow_, tr( "Transactions" ) ) );
            if ( actions_.count() )
                column->addLayout( createButtonLayout( actions_, buttonsPerRow_, tr( "Actions" ) ) );
            column->addStretch();

            if ( RightDialogLocation == parentDialog_->location() )
                column->addWidget( expandButton_ );

            screenLayout->addLayout( column );
            screenLayout->addStretch();
        }

        // column 1
        column = new QVBoxLayout();
        column->addLayout( createButtonLayout( scaleOperations_, buttonsPerRow_, tr( "Scale Operations" ) ) );

        if( !( session_->uiUtilities->isWrapOnly() ) )
            column->addLayout( createButtonLayout( labels_, buttonsPerRow_, tr( "Labels" ) ) );

        // MAR-48
        if( !( session_->uiUtilities->isWrapOnly() ) )
            column->addLayout( createButtonLayout( operators_, buttonsPerRow_, tr( "Operators" ) ) );

        column->addStretch();

        if ( RightDialogLocation != parentDialog_->location() )
            column->addWidget( expandButton_ );

        screenLayout->addLayout( column );
        screenLayout->addStretch();
    }

    // normal menu
    else
    {
	    // column 0
        if( !( session_->uiUtilities->isWrapOnly() ) )
        {
            column = new QVBoxLayout();
            column->addLayout( createButtonLayout( productChanges_, buttonsPerRow_, tr( "Product Changes" ) ) );
            column->addLayout( createButtonLayout( otherRecords_, buttonsPerRow_, tr( "Support Records" ) ) );
            if ( actions_.count() )
                column->addLayout( createButtonLayout( actions_, buttonsPerRow_, tr( "Actions" ) ) );
            column->addStretch();
            screenLayout->addLayout( column );
            screenLayout->addStretch();
        }

	    // column 1
	    column = new QVBoxLayout();
	    column->addLayout( createButtonLayout( scaleOperations_, buttonsPerRow_, tr( "Scale Operations" ) ) );
	    column->addStretch();
	    screenLayout->addLayout( column );
	    screenLayout->addStretch();
	
	    // column 2
	    column = new QVBoxLayout();

        if( !( session_->uiUtilities->isWrapOnly() ) )
        {
            column->addLayout( createButtonLayout( transactions_, buttonsPerRow_, tr( "Transactions" ) ) );
            column->addLayout( createButtonLayout( labels_, buttonsPerRow_, tr( "Labels" ) ) );
        }

        column->addLayout( createButtonLayout( miscellaneous_, buttonsPerRow_, tr( "Miscellaneous" ) ) );
        column->addStretch();
	    screenLayout->addLayout( column );
	    screenLayout->addStretch();
	
        // column 3
        column = new QVBoxLayout();

        if( !( session_->uiUtilities->isWrapOnly() ) )
            column->addLayout( createButtonLayout( cool_, buttonsPerRow_, tr( "COOL" ) ) );

        // MAR-48
        if( !( session_->uiUtilities->isWrapOnly() ) )
            column->addLayout( createButtonLayout( operators_, buttonsPerRow_, tr( "Operators" ) ) );

        if( !( session_->uiUtilities->isWrapOnly() ) )
            column->addLayout( createButtonLayout( operateModeLayout_, buttonsPerRow_, tr( "Operate Mode Layout" ) ) );

        column->addStretch();
        screenLayout->addLayout( column );
        screenLayout->addStretch();
    }

    ///////////////////////////////////////////////////////////////////////////
    // mode buttons

    // add menu buttons
    column = createMenuButtonLayout( SupervisorButtonSelected );

    if ( RightDialogLocation == parentDialog_->location() )
        screenLayout->addLayout( column );
    else
        screenLayout->insertLayout( 0, column );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SupervisorModePanel::initializeButtons()
{
    QPushButton *pushButton;
    MadpScaleConfig *settings = &session_->scaleConfig->scaleConfig;
    MadpInternalConfig *internalConfigData = &session_->scaleConfig->internalConfig;
    MadpOperatorDisplayFields *displayOptions = &session_->scaleConfig->operatorDisplayFields;

    internalConfigData->changeProductActive = false;                // for roaming around in different Change Product Pages and retaining the PLU

    // SOF-3695: Only display Dot Wear selections for HTm scale model
    const QString scaleModel( settings->scaleName() );

    if( !( session_->uiUtilities->isWrapOnly() ) )
    {
        //
        // COOL
        //

        if ( !compactMenu_ )
        {
            pushButton = createButton( cool_, tr( "Categories" ), 5 );
            pushButton->setObjectName( "coolCategories" );
            connect( pushButton, SIGNAL( clicked() ), this, SLOT( onSuperCoolCreateCategoriesClicked() ) );
        }

        if ( !compactMenu_ )
        {
            pushButton = createButton( cool_, tr( "Countries" ), 5 );
            pushButton->setObjectName( "countries" );
            connect( pushButton, SIGNAL( clicked() ), this, SLOT( onSuperCoolCreateCountriesClicked() ) );
        }

        if ( !compactMenu_ )
        {
            pushButton = createButton( cool_, tr( "Production\nSteps" ), 5 );
            pushButton->setObjectName( "productionSteps" );
            connect( pushButton, SIGNAL( clicked() ), this, SLOT( onSuperCoolCreateProdStepsClicked() ) );
        }

        if ( !compactMenu_ )
        {
            pushButton = createButton( cool_, tr( "Pre-Text" ), 5 );
            pushButton->setObjectName( "preText" );
            connect( pushButton, SIGNAL( clicked() ), this, SLOT( onSuperCoolCreatePretextClicked() ) );
        }

        if ( !compactMenu_ )
        {
            pushButton = createButton( cool_, tr( "Assign To\nCategories" ), 5 );
            pushButton->setObjectName( "assignToCategories" );
            connect( pushButton, SIGNAL( clicked() ), this, SLOT( onSuperCoolMainClicked() ) );	 //  COOL - assign to categories
        }
    }

    //
    // Labels
    //

    if( !( session_->uiUtilities->isWrapOnly() ) )
    {
        pushButton = createButton( labels_, tr( "Label\nTypes" ), 5 );
        pushButton->setObjectName( "labelTypes" );
        connect( pushButton, SIGNAL( clicked() ), this, SLOT( onLabelTypesButtonClicked() ) );

        pushButton = createButton( labels_, tr( "Label\nGraphics" ), 5 );
        pushButton->setObjectName( "labelGraphics" );
        connect( pushButton, SIGNAL( clicked() ), this, SLOT( onlabelGraphicsButtonClicked() ) );

        if ( !compactMenu_ )
        {
            pushButton = createButton( labels_, tr( "Fields To\nPrint" ), 9 );
            pushButton->setObjectName( "fieldsToPrint" );
            connect( pushButton, SIGNAL( clicked() ), this, SLOT( onLabelsFieldsToPrintClicked() ) );
        }

        if ( !compactMenu_ )
        {
            pushButton = createButton( labels_, tr( "Text\nNames" ), 9 );
            pushButton->setObjectName( "textNames" );
            connect( pushButton, SIGNAL( clicked() ), this, SLOT( onLabelTextNamesClicked() ) );
        }

        if ( !compactMenu_ && ( session_->isPrepack() || isDebugBuild ) )	//	HA-117 // HT-18
        {
            pushButton = createButton( labels_, tr( "Security\nLabels" ), 9 );
            pushButton->setObjectName( "securityLabels" );
            connect( pushButton, SIGNAL( clicked() ), this, SLOT( onSecurityLabelsClicked() ) );
        }
    }

    //
    // Miscellaneous
    //

    if( !compactMenu_ && !( session_->uiUtilities->isWrapOnly() ) )
    {
        pushButton = createButton( miscellaneous_, tr( "Frequent\nShopper" ), 9);
        pushButton->setObjectName( "frequentShopper" );
        connect( pushButton, SIGNAL( clicked() ), this, SLOT( onFrequentShopperClicked() ) );
    }

    // Note:
    // For Fastrack, the rebootUnprotectedAllowed() flag controls access to setup utility.
    if( !compactMenu_ )
    {
#ifndef DEBUG_QT_LOCAL
       if(mainWindow_->sessionType() == PrimarySessionType)
#endif
       {
            // FT-584 New Feature allow Setup utility always
            //if ( settings->rebootUnprotectedAllowed() )
            //{
                pushButton = createButton( miscellaneous_, tr( "Setup\nUtility" ), 9 );
                pushButton->setObjectName( "setupUtility" );
                connect( pushButton, SIGNAL( clicked() ), this, SLOT( onSetupUtilityButtonClicked() ) );
            //}
        }
    }

    if ( !compactMenu_ )
    {
        pushButton = createButton( miscellaneous_, tr( "Health\nAgent" ), 5 );
        pushButton->setObjectName( "healthAgent" );
        connect( pushButton, SIGNAL( clicked() ), this, SLOT( onHealthAgentClicked() ) );
    }

    // SOF-3695: Only display Dot Wear selections for HTm scale model
    if( scaleModel.size() >= 3 && scaleModel.toUpper().startsWith( "HTM") )
    {
        if ( !compactMenu_  && settings->allowSupervisorDotWearRaw() == "1" ? true : false )
        {
            pushButton = createButton( miscellaneous_, tr( "Pinter\nDot Status" ), 0 );
            pushButton->setObjectName( "DotStatus" );
            connect( pushButton, SIGNAL( clicked() ), this, SLOT( onPrinterDotStatusClicked() ) );
        }
    }

    //
    // Operators
    //

    // MAR-48
    if( !( session_->uiUtilities->isWrapOnly() ) )
    {
        pushButton = createButton( operators_, tr( "Configure\nOperators" ), 9 );
        pushButton->setObjectName( "configureOperators" );
        connect( pushButton, SIGNAL( clicked() ), this, SLOT( onSuperOperatorIdsClicked() ) );
    }

    if( !( session_->uiUtilities->isWrapOnly() ) )
    {
        if ( !compactMenu_ )
        {
            pushButton = createButton( operators_, tr( "Operator\nPermissions" ), 7 );
            pushButton->setObjectName( "operatorPermissions" );
            connect( pushButton, SIGNAL( clicked() ), this, SLOT( onOperatorsCanModifyClicked() ) );
        }

        pushButton = createButton( operators_, tr( "Operator\nChanges" ), 9 );
        pushButton->setObjectName( "operatorChanges" );
        connect( pushButton, SIGNAL( clicked() ), this, SLOT( onSuperOperatorChangesClicked() ) );

        pushButton = createButton( operators_, tr( "Supervisor\nChanges" ), 5 );
        pushButton->setObjectName( "supervisorChanges" );
        connect( pushButton, SIGNAL( clicked() ), this, SLOT( onSuperSupervisorChangesClicked() ) );
    }

    // MAR-48
    if( !( session_->uiUtilities->isWrapOnly() ) )
    {
        if ( !compactMenu_ )
        {
            pushButton = createButton( operators_, tr( "Operator\nNote" ), 2 );
            pushButton->setObjectName( "operatorNote" );
            connect( pushButton, SIGNAL( clicked() ), this, SLOT( onSuperOperatorNoteClicked() ) );
        }
    }

    //
    // Other Records
    //

    if( !( session_->uiUtilities->isWrapOnly() ) )
    {
        pushButton = createButton( otherRecords_, tr( "Flashkeys" ), 5 );
        pushButton->setObjectName( "flashkeys" );
        connect( pushButton, SIGNAL( clicked() ), this, SLOT( onFlashkeyEditorClicked() ) );

        pushButton = createButton( otherRecords_, tr( "Categories" ), 5 );
        pushButton->setObjectName( "otherCategories" );
        connect( pushButton, SIGNAL( clicked() ), this, SLOT( onSuperCategoriesClicked() ) );

        if ( !compactMenu_ )
        {
            pushButton = createButton( otherRecords_, tr( "Departments" ), 5 );
            pushButton->setObjectName( "departments" );
            connect(pushButton, SIGNAL(clicked()), this, SLOT(onDepartmentsClicked()));

            pushButton = createButton( otherRecords_, tr( "Specials" ), 5 );
            pushButton->setObjectName( "specials" );
            connect(pushButton, SIGNAL(clicked()), this, SLOT(onSuperSpecialsClicked()));

            if( displayOptions->enableSelfService() )   // we enable this if we are self service. Should we also test if we have a scanner attached???
            {
                pushButton = createButton( otherRecords_, tr( "Scanner\nCodes" ), 5 );
                pushButton->setObjectName( "scannedCodes" );
                connect( pushButton, SIGNAL( clicked() ), this, SLOT( onSuperScannedCodesClicked() ) );
            }
        }
    }

    //
    // Product Changes
    //

    if( !( session_->uiUtilities->isWrapOnly() ) )
    {
        pushButton = createButton( productChanges_, tr( "Unit\nPrice" ), 4 );
        pushButton->setObjectName( "unitPrice" );
        connect(pushButton, SIGNAL(clicked()), this, SLOT(onSuperChangeProductUnitPriceClicked()));

        pushButton = createButton( productChanges_, tr( "All\nFields" ), 5 );
        pushButton->setObjectName( "allFields" );
        connect( pushButton, SIGNAL( clicked() ), this, SLOT( onSuperChangeProductAllFieldsClicked() ) );

        if ( !compactMenu_ )
        {
            pushButton = createButton( productChanges_, tr( "Tare" ), 5 );
            pushButton->setObjectName( "tare" );
            connect(pushButton, SIGNAL(clicked()), this, SLOT(onSuperChangeProductTareClicked()));
        }

        //if ( !compactMenu_ )
        //{
        pushButton = createButton( productChanges_, tr( "Price\nFields" ), 5 );
        pushButton->setObjectName( "priceFields" );
        connect(pushButton, SIGNAL(clicked()), this, SLOT(onSuperChangeProductPriceFieldsClicked()));
        //}

        if ( !compactMenu_ )
        {
            pushButton = createButton( productChanges_, tr( "Shelf\nLife" ), 5 );
            pushButton->setObjectName( "shelfLife" );
            connect(pushButton, SIGNAL(clicked()), this, SLOT(onSuperChangeProductShelfLifeClicked()));
        }

        if ( !compactMenu_ )
        {
            pushButton = createButton( productChanges_, tr( "Class\nChanges" ), 5 );
            pushButton->setObjectName( "classChanges" );
            connect(pushButton, SIGNAL(clicked()), this, SLOT(onSuperChangeProductClassClicked()));
        }

        if ( !compactMenu_ )
        {
            pushButton = createButton( productChanges_, tr( "Reporting\nClasses" ), 5 );
            pushButton->setObjectName( "reportingClasses" );
            connect( pushButton, SIGNAL( clicked() ), this, SLOT( onSuperReportingClassesClicked() ) );
        }

        //if ( !compactMenu_ )
        //{
        pushButton = createButton( productChanges_, tr( "Product\nDefaults" ), 5 );
        pushButton->setObjectName( "productDefaults" );
        connect( pushButton, SIGNAL( clicked() ), this, SLOT( onSuperProductDefaultsClicked() ) );
        //}
    }

    //
    // Scale Operations
    //

    if(mainWindow_->sessionType() == PrimarySessionType)
    {
        pushButton = createButton( scaleOperations_, tr( "Backup /\nRestore" ), 3 );
        pushButton->setObjectName( "backupRestore" );
        connect( pushButton, SIGNAL( clicked() ), this, SLOT( onBackupRestoreClicked() ) );
        //pushButton->setEnabled( mainWindow_->isPrimarySessionType() ? true : false ); // similar issue as FT-465. Dont allow this remotely
    }
    pushButton = createButton( scaleOperations_, tr( "Normal\nOperations" ), 9 );
    pushButton->setObjectName( "normalOperations" );
    connect( pushButton, SIGNAL( clicked() ), this, SLOT( onSuperNormalScaleOperationsClicked() ) );


    pushButton = createButton( scaleOperations_, tr( "Display\nOptions" ), 9 );
    pushButton->setObjectName( "customerDisplay" );
    connect( pushButton, SIGNAL( clicked() ), this, SLOT( onCustomerDisplayClicked() ) );

    if ( !compactMenu_ && session_->isServiceCounter())		//	HA-72
    {
        pushButton = createButton( scaleOperations_, tr( "Marquee" ), 5 );
        pushButton->setObjectName( "marquee" );
        connect( pushButton, SIGNAL( clicked() ), this, SLOT( onMarqueeConfigButtonClicked() ) );
    }

    if( !( session_->uiUtilities->isWrapOnly() ) )
    {
        if ( !compactMenu_  && settings->showDeleteAllFiles() )
        {
            pushButton = createButton( scaleOperations_, tr( "Delete\nAll Files" ), 9 );
            pushButton->setObjectName( "deleteAllFiles" );
            connect( pushButton, SIGNAL( clicked() ), this, SLOT( onDeleteAllFilesClicked() ) );
        }
    }

    if ( !compactMenu_ )
    {
        pushButton = createButton( scaleOperations_, tr( "Show\nStatistics" ), 2 );
        pushButton->setObjectName( "showStatistics" );
        connect( pushButton, SIGNAL( clicked() ), this, SLOT( onScaleStatisticsButtonClicked() ) );
    }

    if( !( session_->uiUtilities->isWrapOnly() ) )
    {
        if ( !compactMenu_ && settings->showPrinterSettings() )
        {
            pushButton = createButton( scaleOperations_, tr( "Printer\nSetup" ), 2 );
            pushButton->setObjectName( "printerSetup" );
            connect( pushButton, SIGNAL( clicked() ), this, SLOT( onPrinterSetupClicked() ) );
        }
    }

    if ( !compactMenu_ && session_->isServiceCounter())		//	HA-72
    {
        pushButton = createButton( scaleOperations_, tr( "Expiration\nNote" ), 5 );
        pushButton->setObjectName( "expirationNote" );
        connect( pushButton, SIGNAL( clicked() ), this, SLOT( onShelfLifeExpirationClicked() ) );
    }

    if( !( session_->uiUtilities->isWrapOnly() ) )
    {
        if ( !compactMenu_ )
        {
            pushButton = createButton( scaleOperations_, tr( "Store\nInformation" ), 5 );
            pushButton->setObjectName( "storeInformation" );
            connect( pushButton, SIGNAL( clicked() ), this, SLOT( onStoreInformationClicked() ) );
        }

        if ( !compactMenu_ )
        {
            pushButton = createButton( scaleOperations_, tr( "Report\nSetup" ), 5 );
            pushButton->setObjectName( "reportSetup" );
            connect( pushButton, SIGNAL( clicked() ), this, SLOT( onPrintSetupClicked() ) );
        }

        if ( settings->showPrinterSettings() )
        {
            pushButton = createButton( scaleOperations_, tr( "Print\nConfiguration" ), 5 );
            pushButton->setObjectName( "printConfiguration" );
            connect( pushButton, SIGNAL( clicked() ), this, SLOT( onPrintConfigClicked() ) );
        }

        if ( !compactMenu_ )
        {
            pushButton = createButton( scaleOperations_, tr( "Email\nAlerts" ), 7 );
            pushButton->setObjectName( "emailAlerts" );
            connect( pushButton, SIGNAL( clicked() ), this, SLOT( onSuperEmailSetupClicked() ) );
        }

        if ( !compactMenu_ )
        {
            pushButton = createButton( scaleOperations_, tr( "Comm\nInterface" ), 7 );
            pushButton->setObjectName( "commInterface" );
            connect( pushButton, SIGNAL( clicked() ), this, SLOT( onCommInterfaceClicked() ) );
        }
    }

    if ( !compactMenu_ && session_->isServiceCounter())		//	HA-72
    {
        pushButton = createButton( scaleOperations_, tr( "Synchronize\nScales" ), 7 );
        pushButton->setObjectName( "synchronizeScales" );
        connect( pushButton, SIGNAL( clicked() ), this, SLOT( onScaleSynchronizationClicked() ) );
    }

    /* HTA-149
     * if ( !compactMenu_ && session_->isPrepack())
    {
        pushButton = createButton( scaleOperations_, tr( "Scale\nManagement" ), 7 );
        pushButton->setObjectName( "scaleManagement" );
        connect( pushButton, SIGNAL( clicked() ), this, SLOT( onScaleManagementButtonClicked() ) );
    }
    */
    if ( !compactMenu_ )
    {
        pushButton = createButton( scaleOperations_, tr( "Upgrade\nvia USB" ), 9 );
        pushButton->setObjectName( "upgradeViaUSB" );
        connect(pushButton, SIGNAL(clicked()), this, SLOT(onSuperUpgradeFirmwareViaUSBClicked()));
    }

    if ( !compactMenu_ )
    {
        pushButton = createButton( scaleOperations_, tr( "Update\nvia SFTP" ), 9 );
        pushButton->setObjectName( "upgradeViaSFTP" );
        connect(pushButton, SIGNAL(clicked()), this, SLOT(onSuperUpgradeFirmwareViaSFTPClicked()));
    }

    if( !( session_->uiUtilities->isWrapOnly() ) )
    {
        if ( session_->isPrepack()|| isDebugBuild )        // HT-18
        {
            pushButton = createButton( scaleOperations_, tr( "Rotation\nOverride" ), 9 );
            pushButton->setObjectName( "applierRotationOverride" );
            connect(pushButton, SIGNAL(clicked()), this, SLOT(onSuperApplierRotationOverrideClicked()));
        }
    }

    //
    // Actions
    //

    // as per James Meyer remove rezero from super functions because is already on operate panel
    //if ( perifList->firstWeigher() )
    //{
    //    pushButton = createButton( actions_, tr( "Rezero\nWeigher" ), 2 );
    //    pushButton->setObjectName( "rezeroWeigher" );
    //    connect( pushButton, SIGNAL( clicked() ), SLOT( onRezeroWeigherClicked() ) );
    //    pushButton->setEnabled( mainWindow_->isPrimarySessionType() ? true : false ); // FT-465
    //}

    //
    // Transactions
    //

    if( !( session_->uiUtilities->isWrapOnly() ) )
    {
        pushButton = createButton( transactions_, tr( "Hourly\nTotals" ), 5 );
        pushButton->setObjectName( "hourlyTotals" );
        connect( pushButton, SIGNAL( clicked() ), this, SLOT( onSuperHourlyTotalsClicked() ) );

        pushButton = createButton( transactions_, tr( "Void\nTransaction" ), 7 );
        pushButton->setObjectName( "voidTransaction" );
        connect( pushButton, SIGNAL( clicked() ), this, SLOT( onVoidTransactionsClicked() ) );

        if ( !compactMenu_ )
        {
            pushButton = createButton( transactions_, tr( "Product\nTotals" ), 5 );
            pushButton->setObjectName( "productTotals" );
            connect( pushButton, SIGNAL( clicked() ), this, SLOT( onSuperProductTotalsClicked() ) );
        }

        if ( !compactMenu_ )
        {
            pushButton = createButton( transactions_, tr( "Transaction\nTypes" ), 7 );
            pushButton->setObjectName( "transactionTypes" );
            connect( pushButton, SIGNAL( clicked() ), this, SLOT( onSuperTotalTypesSetupClicked() ) );
        }

        if( settings->forcedLogin() )       // if under config operators, the field "Operators must login to run the scale" is checked
        {
            pushButton = createButton( transactions_, tr( "Operator\nTotals" ), 5 );
            pushButton->setObjectName( "operatorTotals" );
            connect( pushButton, SIGNAL( clicked() ), this, SLOT( onSuperOperatorTotalsClicked() ) );
        }
    }

    //
    // Operate Mode Layout
    //

    if( !( session_->uiUtilities->isWrapOnly() ) )
    {
        if ( !compactMenu_ )
        {
            pushButton = createButton( operateModeLayout_, tr( "Move\nLayout" ), 9 );
            pushButton->setObjectName( "moveLayout" );
            connect( pushButton, SIGNAL( clicked() ), SLOT( onOperateModeLayoutMoveLayout() ) );
        }

        if ( !compactMenu_ )
        {
            pushButton = createButton( operateModeLayout_, tr( "Change\nFields" ), 9 );
            pushButton->setObjectName( "changeFields" );
            connect( pushButton, SIGNAL( clicked() ), SLOT( onOperateModeLayoutChangeFields() ) );
        }
    }
}
