/**
 * @file operatemodepanel.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "operatemodepanel.h"

#include "cooltextpanel.h"
#include "deptpanel.h"
#include "expirationhistorypanel.h"
#include "totaltypespanel.h"
#include "voidtransactionpanel.h"
#include "sizelabelspanel.h"
#include "sizelabelspanel.h"

#include "../sessionmainwindow.h"
#include "../sessionmainwindowimpl.h"
#include "../sessiondialog.h"

#include "../dialogs/aboutdialog.h"
#include "../dialogs/messageboxdialog.h"
#include "../dialogs/nowservingdialog.h"
#include "../dialogs/planogramdialog.h"
#include "../dialogs/printersettingsdialog.h"
#include "../dialogs/printercleanerdialog.h"
#include "../dialogs/weightsandmeasuresdialog.h"
#include "../dialogs/wrappercalibrationdialog.h"
#include "../dialogs/firmwareversiondialog.h"

#include "../modes/operatemode.h"

#include "../screens/flashkeyeditor.h"
#include "../screens/labelplacement.h"
#include "../screens/plumainscreen.h"
#include "../screens/pluinfoscreen.h"
#include "../screens/webbrowserscreen.h"
#include "../screens/wrappersettings.h"
#include "../screens/updateNow.h"
#include "../screens/sendToDept.h"
#include "../screens/servicePrinterHeadDotStatus.h"
#include <backgroundwidgettypes.h>

#include <ScaleConfiguration.h>
#include <UiUtilities.h>

#include <QDir>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>

/// Renderer traits type.
typedef hobart::operateModePanelBackgroundWidget rendererTraitsType;

const int OperateModePanel::buttonsPerRow_ = 1;
const bool TURN_ON_OPERATE_MENU = false;
const bool TURN_ON_WRAPPER_MENU = false;
const bool TURN_ON_LABEL_APPLIER_MENU = false;
const bool TURN_ON_ALL_MENUS = false;

///////////////////////////////////////////////////////////////////////////////////////////////////
OperateModePanel::OperateModePanel( int panelEx, SessionMainWindow *mainWindow, SessionDialog *parent ) :
    _Mybase( mainWindow, parent ),
    extendedPanel_( (ExtendedPanel) panelEx ),
    pickFive_( NULL )
{
    LOG_CLASS_INFO( "OperateModePanel", "OperateModePanel" );

    setFixedSize( sizeHint() );
    setObjectName( "OperateModePanel" );

    // retrieve access level
    if ( mainWindow )
        accessLevel_ = mainWindow->retrieveMode<OperateMode>()->operatorAccessLevel();

    LOG_INFO( "access level is %d", accessLevel_ );

    // initialize interface
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
OperateModePanel::~OperateModePanel()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QSize OperateModePanel::sizeHint() const
{
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
PanelWidget *OperateModePanel::createExtendedPanel()
{
    if ( CoolTextExtendedPanel == extendedPanel_ )
        return new CoolTextPanel( mainWindow_, parentDialog_ );
    else if ( ExpirationHistoryExtendedPanel == extendedPanel_ )
        return new ExpirationHistoryPanel( mainWindow_, parentDialog_ );
    else if ( TotalTypesExtendedPanel == extendedPanel_ )
        return new TotalTypesPanel( mainWindow_, parentDialog_ );
    else if ( VoidTransactionExtendedPanel == extendedPanel_ )
        return new VoidTransactionPanel( mainWindow_, parentDialog_ );
    else if ( SizeLabelsExtendedPanel == extendedPanel_ )
        return new SizeLabelsPanel( mainWindow_, parentDialog_ );
    else if ( ChooseDepartmentExtendedPanel == extendedPanel_ )
        return new DepartmentPanel( mainWindow_, parentDialog_ );

    return _Mybase::createExtendedPanel();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
hobart::backgroundWidgetBase *OperateModePanel::rendererTraits() const
{
    return new rendererTraitsType();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateModePanel::onCreateCoolClicked()
{
    mainWindow_->showMenuPanel( SessionMainWindow::OperateMenuPanelMode, CoolTextExtendedPanel );
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateModePanel::onExpirationHistoryClicked()
{
    mainWindow_->showMenuPanel( SessionMainWindow::OperateMenuPanelMode, ExpirationHistoryExtendedPanel );
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateModePanel::onRezeroWeigherClicked()
{
    session_->uiUtilities->rezeroWeigher();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateModePanel::onSelectScaleDeptClicked()
{
    mainWindow_->showMenuPanel( SessionMainWindow::OperateMenuPanelMode, ChooseDepartmentExtendedPanel );
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateModePanel::onSelectPick5Clicked()
{
    // PICK 5 Enabled thru the menu option

    accept();

    // check if we have a  product AND if it is active for pick5
    DomCurrentPLU *product( mainWindow_->retrieveMode<OperateMode>()->currentProduct() );

    if( session_->scaleConfig->getIsPick5Enabled() )
    {
        // Then we need to disable it
        if ( product )
        {
            // if we have an active product we need to disable the orange screen
            if( session_->scaleConfig->getIsPick5Active() )
                session_->scaleConfig->setIsPick5Active( false );
        }

        // turn off background (maybe) and reset menu text option
        session_->scaleConfig->setIsPick5Enabled( false );
        pickFive_->setText( "Enable\nPick 5" );
    }
    else
    {
        // or enable it if it is a min max pick5 item

        // at this point I am testing
        // 1. are we licensed?
        // 2. do we have non blank fields > 0 in weight or price.
        // If so, turn orange. Dialog pops when printing happens.
        const QString minWtStr( product->minWeight() );
        const QString maxWtStr( product->maxWeight() );
        const QString minPrStr( product->minPrice() );
        const QString maxPrStr( product->maxPrice() );

        float minWt = minWtStr.toFloat();
        float maxWt = maxWtStr.toFloat();
        float minPr = minPrStr.toFloat();
        float maxPr = maxPrStr.toFloat();

        if( (minWt >= 0 && maxWt > 0) || (minPr >= 0 && maxPr > 0) )
            session_->scaleConfig->setIsPick5Active( true );

        session_->scaleConfig->setIsPick5Enabled( true );
        pickFive_->setText( "Disable\nPick 5" );
    }

    session_->send( SoftkeyDev, ExecuteSoftkey, "62,62," + QString::number( 999003 ) );

    // we need to check if we have a PLU on the screen, and clear background if we do set if not
    mainWindow_->togglePickFive( session_->scaleConfig->getIsPick5Active() );

    if ( product )
    {
        // SOF-2750: Retrieve new totals and update screen
        session_->send( ProductDev, ProductReadCurrentTotalValues );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateModePanel::onTotalTypesClicked()
{
    mainWindow_->showMenuPanel( SessionMainWindow::OperateMenuPanelMode, TotalTypesExtendedPanel );
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateModePanel::onVoidTransactionClicked()
{
    mainWindow_->showMenuPanel( SessionMainWindow::OperateMenuPanelMode, VoidTransactionExtendedPanel );
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateModePanel::onWeightsAndMeasuresClicked()
{
    weightsandmeasuresdialog *weightsAndMeasuresDialog;
    weightsAndMeasuresDialog = new weightsandmeasuresdialog( mainWindow_ );

    weightsAndMeasuresDialog->open();

    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateModePanel::onFlashkeyEditorClicked()
{
    // only switch when features to edit
    if ( editFeatureList_.count() )
    {
        mainWindow_->switchToModeScreen<OperateMode, FlashkeyEditor>();

        OperateMode *mode = mainWindow_->retrieveMode<OperateMode>();
        FlashkeyEditor *screen = mode ? dynamic_cast<FlashkeyEditor*>( mode->screen() ) : NULL;

        // set feature list
        if ( screen )
            screen->enableOperateFeatures( editFeatureList_ );
    }

    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateModePanel::onOperatorNoteClicked()
{
    // request operator note
    session_->send( OperatorDev, OpReadNote );
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateModePanel::onPrinterSettingsClicked()
{
    PrinterSettingsDialog *dialog = new PrinterSettingsDialog( mainWindow_ );

    // show dialog!
    dialog->open();

    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateModePanel::onPrinterCleanerClicked()
{
    PrinterCleanerDialog *dialog = new PrinterCleanerDialog( mainWindow_ );

    // show dialog!
    dialog->open();

    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateModePanel::onSizeLabelsClicked()
{
    mainWindow_->showMenuPanel( SessionMainWindow::OperateMenuPanelMode, SizeLabelsExtendedPanel );
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateModePanel::onAboutScaleClicked()
{
    AboutDialog *dialog = new AboutDialog( mainWindow_ );

    // show dialog!
    dialog->open();

    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateModePanel::onPlanogramClicked()
{
    PlanogramDialog *dialog = new PlanogramDialog( mainWindow_ );

    // show dialog!
    dialog->open();

    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateModePanel::onQueueManagementClicked()
{
    NowServingDialog *dialog = new NowServingDialog( mainWindow_ );

    // show dialog!
    dialog->open();

    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateModePanel::onServeNextCustomerClicked()
{
    MadpScaleConfig *scaleConfig( &session_->scaleConfig->scaleConfig );

    int command( NowServingQueueManagement::ServingNextCustomer );

    // using pre-printed paper roll, not electronic system
    if ( 2 == scaleConfig->nowServingSysType() )
        command = NowServingQueueManagement::IncrementCustomerNumber;

    session_->send(
        SessionDev,
        SnNowServingQueueManagement,
        QString::number( command ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateModePanel::onEjectMediaClicked()
{
    mainWindow_->ejectMedia();
    accept();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateModePanel::onPluInfoClicked()
{
    mainWindow_->switchToModeScreen<OperateMode, PluInfoScreen>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateModePanel::onWrapperSettingsClicked()
{
    mainWindow_->switchToModeScreen<OperateMode, WrapperSettingsScreen>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateModePanel::onFirmwareVersionsClicked()
{
    FirmwareVersionDialog *firmwareVersionDialog = new FirmwareVersionDialog( mainWindow_ );

    firmwareVersionDialog->open();

    accept();

}

void OperateModePanel::onUpdateNowClicked()
{
    // HT-361
    MadpScaleConfig *scaleConfig( &session_->scaleConfig->scaleConfig );
    QString backupFrequency = scaleConfig->autoBackupDBFrequency();
    if( !( mainWindow_->checkForAutomaticBackup( backupFrequency ) ) )
        return;

    mainWindow_->switchToModeScreen<OperateMode, updateNow>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateModePanel::onSendToDeptClicked()
{
    // HT-361
    MadpScaleConfig *scaleConfig( &session_->scaleConfig->scaleConfig );
    QString backupFrequency = scaleConfig->autoBackupDBFrequency();
    if( !( mainWindow_->checkForAutomaticBackup( backupFrequency ) ) )
        return;

    // TBD
    mainWindow_->switchToModeScreen<OperateMode, sendToDept>();
    accept();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateModePanel::onLabelPlacementClicked()
{
    mainWindow_->switchToModeScreen<OperateMode, LabelPlacementScreen>();
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateModePanel::initialize()
{
    OperateMode *mode = mainWindow_->retrieveMode<OperateMode>();
    PluMainScreen *pluMainScreen;
    WebBrowserScreen *webBrowserScreen;

    // retrieve editable features
    if ( mode )
    {
        if ( (webBrowserScreen = dynamic_cast<WebBrowserScreen*>( mode->screen() )) )
            editFeatureList_ = webBrowserScreen->editableFeatures();
        else if ( (pluMainScreen = dynamic_cast<PluMainScreen*>( mode->screen() )) )
            editFeatureList_ = pluMainScreen->editableFeatures();
    }

    // screen layout
    QHBoxLayout *screenLayout = new QHBoxLayout( this );

    screenLayout->setContentsMargins( QMargins( 20, 20, 20, 20 ) );
    screenLayout->addStretch();

    // create button columns
    QVBoxLayout *column;

    ///////////////////////////////////////////////////////////////////////////
    // operate mode buttons

    // create our buttons
    initializeButtons();

    // left column
    column = new QVBoxLayout();
    column->addLayout( createButtonLayout( left_, buttonsPerRow_ ) );
    screenLayout->addLayout( column );
    screenLayout->addStretch();

    // right column
    column = new QVBoxLayout();
    column->addLayout( createButtonLayout( right_, buttonsPerRow_ ) );
    screenLayout->addLayout( column );
    screenLayout->addStretch();

    ///////////////////////////////////////////////////////////////////////////
    // mode buttons

    // add menu buttons
    column = createMenuButtonLayout( OperateButtonSelected );

    if ( RightDialogLocation == parentDialog_->location() )
        screenLayout->addLayout( column );
    else
        screenLayout->insertLayout( 0, column );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateModePanel::initializeButtons()
{
    UiUtilities *util( mainWindow_->session()->uiUtilities );

    QPushButton *pushButton;
    MadpScaleConfig *scaleConfig = &session_->scaleConfig->scaleConfig;
    MadpPeripheralList *perifList = &session_->scaleConfig->peripheralList;

    // SOF-3695: Only display Dot Wear selections for HTm scale model
    const QString scaleModel( scaleConfig->scaleName() );

    // DomAutomaticUpdate *UpdateNow = &session_->sc
    bool isProductActive( false );

    // check for active product
    if ( mainWindow_->isOperateMode() )
    {
        DomCurrentPLU *product( mainWindow_->retrieveMode<OperateMode>()->currentProduct() );

        // valid product with item number
        if (( product ) && ( product->itemNumber().trimmed().length() ))
            isProductActive = true;
    }

    //
    // Left Column
    //

    if( !( session_->uiUtilities->isWrapOnly() )  || TURN_ON_ALL_MENUS )
    {
        if(scaleConfig->allowOperatorUpdateNow() == "1" || TURN_ON_ALL_MENUS )
         {
             pushButton = createButton( left_, tr( "Update\nNow"), 0 );
             pushButton->setObjectName( "updateNow" );
             connect( pushButton, SIGNAL( clicked() ), SLOT( onUpdateNowClicked() ) );
         }
    }

    if( ( perifList->firstWeigher() && !( session_->uiUtilities->isWrapOnly() ) )  || TURN_ON_ALL_MENUS )
    {
        pushButton = createButton( left_, tr( "Rezero\nWeigher" ), 0  );
        pushButton->setObjectName( "rezeroWeigher" );
        connect( pushButton, SIGNAL( clicked() ), SLOT( onRezeroWeigherClicked() ) );
        pushButton->setEnabled( mainWindow_->isPrimarySessionType() ? true : false ); // FT-465
    }

    if ( ( session_->isPrepack() && !( session_->uiUtilities->isWrapOnly() ) ) || TURN_ON_WRAPPER_MENU )
    {
        if ( perifList->isWrapperAttached() || TURN_ON_WRAPPER_MENU )
        {
            pushButton = createButton( left_, tr( "Wrapper\nSettings" ), 0 );
            pushButton->setObjectName( "wrapperSettings" );
            connect( pushButton, SIGNAL( clicked() ), SLOT( onWrapperSettingsClicked() ) );
        }
    }

    if( !( session_->uiUtilities->isWrapOnly() ) || TURN_ON_ALL_MENUS )
    {
        pushButton = createButton( left_, tr( "Void\nTransaction" ), 0, VoidTransactionExtendedPanel == extendedPanel_ );
        pushButton->setObjectName( "voidTransaction" );
        connect( pushButton, SIGNAL( clicked() ), SLOT( onVoidTransactionClicked() ) );
    }

    if( !( session_->uiUtilities->isWrapOnly() ) || TURN_ON_ALL_MENUS )
    {
        pushButton = createButton( left_, tr( "Total\nTypes" ), 0, TotalTypesExtendedPanel == extendedPanel_ );
        pushButton->setObjectName( "totalTypes" );
        connect( pushButton, SIGNAL( clicked() ), SLOT( onTotalTypesClicked() ) );
    }

    if ( !session_->isPrepack()  || TURN_ON_ALL_MENUS )
    {
        pushButton = createButton( left_, tr( "Expiration\nHistory" ), 0, ExpirationHistoryExtendedPanel == extendedPanel_ );
        pushButton->setObjectName( "expirationHistory" );
        connect( pushButton, SIGNAL( clicked() ), SLOT( onExpirationHistoryClicked() ) );
    }

    if ( !session_->isPrepack()  || TURN_ON_ALL_MENUS )
    {
        if ( ( ( util->isFeatureLicensed( SCALE_NOW_SERVING, false ) ) &&
            ( 0 < scaleConfig->nowServingSysType() ) && ( scaleConfig->nowServingSysType() <= 2 ))
             || TURN_ON_ALL_MENUS )
        {
            if ( mainWindow_->isPrimarySessionType() )
            {
                pushButton = createButton( left_, tr( "Serve Next\nCustomer" ), 0 );
                pushButton->setObjectName( "serveNextCustomer" );
                connect( pushButton, SIGNAL( clicked() ), SLOT( onServeNextCustomerClicked() ) );
            }

            pushButton = createButton( left_, tr( "Queue\nManagement" ), 0 );
            pushButton->setObjectName( "queueManagement" );
            connect( pushButton, SIGNAL( clicked() ), SLOT( onQueueManagementClicked() ) );
        }
    }

    if( !( session_->uiUtilities->isWrapOnly() ) || TURN_ON_ALL_MENUS )
    {

        pushButton = createButton( left_, tr( "Change\nDepartment" ), 0, ChooseDepartmentExtendedPanel == extendedPanel_ );
        pushButton->setObjectName( "chooseDepartment" );
        connect( pushButton, SIGNAL( clicked() ), SLOT( onSelectScaleDeptClicked() ) );

        if( ( perifList->firstWeigher() && !( session_->uiUtilities->isWrapOnly() ) ) || TURN_ON_ALL_MENUS )
        {
            pushButton = createButton( left_, tr( "Weights &&\nMeasures" ), 0 );
            pushButton->setObjectName( "weightsAndMeasures" );
            connect( pushButton, SIGNAL( clicked() ), SLOT( onWeightsAndMeasuresClicked() ) );
        }
    }

    if ( (isProductActive && !( session_->uiUtilities->isWrapOnly() )) || TURN_ON_LABEL_APPLIER_MENU)
    {
        if ( session_->isPrepack() || TURN_ON_LABEL_APPLIER_MENU)
        {
            pushButton = createButton( left_, tr( "Product\nInformation" ), 0 );
            pushButton->setObjectName( "productInformation" );
            connect( pushButton, SIGNAL( clicked() ), SLOT( onPluInfoClicked() ) );

            // need a product for label placement
            if ( perifList->isLabelApplierAttached() || TURN_ON_LABEL_APPLIER_MENU)
            {
                pushButton = createButton( left_, tr( "Label\nPlacement" ), 0 );
                pushButton->setObjectName( "labelPlacement" );
                connect( pushButton, SIGNAL( clicked() ), SLOT( onLabelPlacementClicked() ) );
            }
        }
    }

    if ( session_->isPrepack() || TURN_ON_WRAPPER_MENU )
    {
        if ( perifList->isWrapperAttached() || TURN_ON_WRAPPER_MENU )
        {
            pushButton = createButton( left_, tr( "Firmware\nVersions" ), 0 );
            pushButton->setObjectName( "firmwareVersions" );
            connect( pushButton, SIGNAL( clicked() ), SLOT( onFirmwareVersionsClicked() ) );
        }
    }

    // SOF-3695: Only display Dot Wear selections for HTm scale model
    if( scaleModel.size() >= 3 && scaleModel.toUpper().startsWith( "HTM") )
    {
        if( scaleConfig->allowOperatorDotWearRaw() == "1" ? true : false || TURN_ON_ALL_MENUS)
        {
            pushButton = createButton( left_, tr("Printer\nDot Status"), 0 );
            pushButton->setObjectName("printerDotStatus");
            connect( pushButton, SIGNAL( clicked() ), SLOT( onPrinterDotStatusClicked() ) );
        }
    }

    //
    // Right Column
    //

    //  HA-185: If Operator login is required, we need to show either a Login or Logout button on this panel.

    if( !( session_->uiUtilities->isWrapOnly() ) || TURN_ON_ALL_MENUS)
    {
        if( (scaleConfig->allowOperatorSendToDept() == "1" && scaleConfig->AutomaticUpdateScaleType() == "1") || TURN_ON_ALL_MENUS )
        {
            pushButton = createButton( right_, tr( "Send to\nDept."), 0 );
            pushButton->setObjectName( "sendToDept" );
            connect( pushButton, SIGNAL( clicked() ), SLOT( onSendToDeptClicked() ) );
        }
    }

    if ( (session_->isPrepack() && scaleConfig->forcedLogin()) || TURN_ON_ALL_MENUS )
    {
        pushButton = createButton( right_, tr( "Log in/out\nOperator"), 0 );
        pushButton->setObjectName( "logInOutOperator" );
        connect( pushButton, SIGNAL( clicked() ), SLOT( onLogInOutOperatorClicked()));
    }

    if ( mainWindow_->isPrimarySessionType() )
    {
#if defined( Q_OS_LINUX )
        QDir d( "/media/thumb" );

        if ( d.exists() )
        {
            pushButton = createButton( right_, tr( "Eject\nMedia" ), 0 );
            pushButton->setObjectName( "ejectMedia" );
            connect( pushButton, SIGNAL( clicked() ), SLOT( onEjectMediaClicked() ) );
        }

#else
        // TODO

#endif
    }

    // need a product for create cool button to show
    if ( ( isProductActive && !( session_->uiUtilities->isWrapOnly() )) || TURN_ON_ALL_MENUS )
    {
        pushButton = createButton( right_, tr( "Create\nCOOL" ), 0, CoolTextExtendedPanel == extendedPanel_ );
        pushButton->setObjectName( "createCool" );
        connect( pushButton, SIGNAL( clicked() ), SLOT( onCreateCoolClicked() ) );
    }

    if ( (scaleConfig->showPrinterSettings() && !( session_->uiUtilities->isWrapOnly() )) || TURN_ON_ALL_MENUS )
    {
        pushButton = createButton( right_, tr( "Printer\nSettings" ), 0 );
        pushButton->setObjectName( "printerSettings" );
        connect( pushButton, SIGNAL( clicked() ), SLOT( onPrinterSettingsClicked() ) );
    }

    // NGW-143
    if ( session_->isServiceCounter() )
    {
        pushButton = createButton( right_, tr( "Size\nLabels" ), 0 );
        pushButton->setObjectName( "sizeLabels" );
        connect( pushButton, SIGNAL( clicked() ), SLOT( onSizeLabelsClicked() ) );
    }

    if( !( session_->uiUtilities->isWrapOnly() ) )
    {
        pushButton = createButton( right_, tr( "Operator\nNote" ), 0 );
        pushButton->setObjectName( "operatorNote" );
        connect( pushButton, SIGNAL( clicked() ), SLOT( onOperatorNoteClicked() ) );
    }

    // Why is this on here when we have a capacity bar button option?
    // Got rid of it 10-04-2018 when I added PICK 5
    // pushButton = createButton( right_, tr( "Planogram" ), 0 );
    // pushButton->setObjectName( "planogram" );
    // connect( pushButton, SIGNAL( clicked() ), SLOT( onPlanogramClicked() ) );

    if( util->isFeatureLicensed( SCALE_PICK5, false ) )
    {
        if( session_->scaleConfig->getIsPick5Enabled() )
            pickFive_ = createButton( right_, tr( "Disable\nPick 5" ), 0 );
        else
            pickFive_ = createButton( right_, tr( "Enable\nPick 5" ), 0 );

        pickFive_->setObjectName( "turnOnPickFive" );
        connect( pickFive_, SIGNAL( clicked() ), SLOT( onSelectPick5Clicked() ) );
    }

    // FT-1322 disable operator flashkey editor
    if( !( session_->uiUtilities->isWrapOnly() ) || TURN_ON_ALL_MENUS )
    {
        if ( (( !scaleConfig->disableOperatorFlashkeyEditor() ) && ( editFeatureList_.count() )) || TURN_ON_ALL_MENUS )
        {
            pushButton = createButton( right_, tr( "Flashkey\nEditor" ), 0 );
            pushButton->setObjectName( "flashkeyEditor" );
            connect( pushButton, SIGNAL( clicked() ), SLOT( onFlashkeyEditorClicked() ) );
        }
    }

    // SOF-1234
    if ( session_->isPrepack() || TURN_ON_WRAPPER_MENU )
    {
        // SOF-4436: Do not show for AWS1 and AWS2 wrapper models
        DomEvolWrapperSystemInformation *evolWrapperSystemInformation( &session_->scaleConfig->evolWrapperSystemInformation );

        if ( evolWrapperSystemInformation->awsModelNumber() >= DomEvolWrapperSystemInformation::reducedTimingModels )
        {
            pushButton = createButton( right_, tr( "Calibrate\nWrapper" ), 0 );
            pushButton->setObjectName( "calibrateWrapper" );
            connect( pushButton, SIGNAL( clicked() ), SLOT(onCalibrateWrapperClicked()) );
        }
    }

    // SOF-5370 only display clean printhead option with FSS scales
    /* "Hide" clean printer button as per Jim
    if ( session_->scaleConfig->scaleConfig.scaleName().startsWith( "FS" ) )
    {
        pushButton = createButton( right_, tr( "Clean\nPrinter" ), 0 );
        pushButton->setObjectName( "printerCleaner" );
        connect( pushButton, SIGNAL( clicked() ), SLOT( onPrinterCleanerClicked() ) );
    }
    */

    pushButton = createButton( right_, tr( "About\nScale" ), 0 );
    pushButton->setObjectName( "aboutScale" );
    connect( pushButton, SIGNAL( clicked() ), SLOT( onAboutScaleClicked() ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateModePanel::onLogInOutOperatorClicked()
{
    if ( session_->scaleConfig->getIsOperatorLoggedIn() )
        session_->send( OperatorDev, OpLogout );
    else
        session_->send( OperatorDev, OpPromptForOperatorIDOrRFID );
    accept();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OperateModePanel::onCalibrateWrapperClicked()
{
    WrapperCalibrationDialog *d( new WrapperCalibrationDialog( mainWindow_ ) );
    d->open();

    accept();

}

void OperateModePanel::onPrinterDotStatusClicked()
{
    //mainWindow_->switchToModeScreen<SupervisorMode, servicePrinterHeadDotStatus>();
    //accept();

    mainWindow_->switchToModeScreen<OperateMode, servicePrinterHeadDotStatus>();
    accept();
}
