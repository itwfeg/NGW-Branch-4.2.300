/**
 * @file superNormalScaleOperation.cpp
 * 
 * @class SuperNormalScaleOperation
 * @brief Supervisor mode - Configuration - Normal Scale Operations
 *
 * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
 * @Author: Job E. Crew with some co-author assistance from Larry Pevoar
 * @{
 */

#include <logmanager.h>
#include <filesystem.h>

#include "superNormalScaleOperation.h"
#include "ui_superNormalScaleOperation.h"
#include "../sessionmainwindow.h"
#include "../sessionmainwindowimpl.h"
#include "../dialogs/timeEntryDialog.h"
#include "../dialogs/wrappercalibrationdialog.h"
#include "supertextfileeditor.h"
#include <softkeyswidget.h>

#include "../modes/supervisormode.h"
#include "../modes/servicemode.h"

#include "../widgets/dynamiccontentwidget.h"
#include "../widgets/labelplacementwidget.h"

#include <numericlineedit.h>

#include <ScaleConfiguration.h>
#include <UiUtilities.h>
#include <ScaleClasses.h>

#include <QDesktopWidget>
#include <QMessageBox>
#include <QIntValidator>
#include <QSize>
#include <QDir>

#include <QDateTimeEdit>
#include <QPushButton>
#include <QSignalMapper>
#include <numberpadwidget.h>
#include "../../source/HFC/CfgBaseFieldOffsets.h"
#include <QTimer>

static const int PRIMARY_LANGUAGE_CHANGED = 1;
static const int SECONDARY_LANGUAGE_CHANGED = 2;
static const int MAX_DIGITS_EAN13 = 12;
static const int EAN_ACCUM = 3;

/////////////////////////////////////////////////////////////////////////////////////
//
//               C O N S T R U C T O R S  &  D E S T R U C T O R S
//
/////////////////////////////////////////////////////////////////////////////////////

namespace hobart{
    class numericLineEdit;
}

SuperNormalScaleOperation::SuperNormalScaleOperation(SessionMainWindow *mainWindow, ModeWidget *parent)
    : _Mybase( mainWindow, parent ),
      ui(new Ui::SuperNormalScaleOperation),
      normalScaleOperation_(NULL),
      operatorDisplayFields_(NULL),
      accumItemsConfig_(NULL),
      scaleConfigData_(NULL),
      currencyData_(NULL),
      periphSettings_(NULL),
      signalMapper_( NULL ),
      isLicensed_FPP_( true ),
      isLicensed_FullscreenFlashkeys_( true ),
      isLicensed_WebBrowser_( true ),
      isLicensed_MealBuilder_( false ),
      isLicensed_Enhanced_CenterScreen_( false )
{
    ui->setupUi(this);
    //scaleScreen();

    // Retrieve config object

    //------------------------------
    // Coming from Supervisor panel:
    //------------------------------
    if ( mainWindow_->isSupervisorMode() )
        normalScaleOperation_ = &dynamic_cast<SupervisorMode*>( mode_ )->normalScaleOperation_;

    if ( mainWindow_->isSupervisorMode() )
        operatorDisplayFields_ = &dynamic_cast<SupervisorMode*>( mode_ )->operatorDisplayFields_;

    if ( mainWindow_->isSupervisorMode() )
        accumItemsConfig_ = &dynamic_cast<SupervisorMode*>( mode_ )->accumItemsConfig_;

    //---------------------------
    // Coming from Service panel:
    //---------------------------
    if ( mainWindow_->isServiceMode() )
        normalScaleOperation_ = &dynamic_cast<ServiceMode*>( mode_ )->normalScaleOperation_;

    if ( mainWindow_->isServiceMode() )
        operatorDisplayFields_ = &dynamic_cast<ServiceMode*>( mode_ )->operatorDisplayFields_;

    if ( mainWindow_->isServiceMode() )
        accumItemsConfig_ = &dynamic_cast<ServiceMode*>( mode_ )->accumItemsConfig_;


    eanConfigEditor_ = NULL;
    dataHasBeenInitialized = false;

    if ( session_->isPrepack() )
    {
        // HA-72:  Hide SelfService and all Production entry related fields for Prepack
        // Hide sub-tabs on the "Features" tab
        // Need to remove from right-to-left

        if( session_->uiUtilities->isWrapOnly() ) // PJR-145: Wrap-only
            ui->tabSubFeatures->removeTab( 7 );// Cutting Test

        ui->tabSubFeatures->removeTab( 4 );    // Self-Service
        ui->tabSubFeatures->removeTab( 3 );    // Now Serving
        ui->tabSubFeatures->removeTab( 2 );    // Accumulate
        ui->tabSubFeatures->removeTab( 1 );    // Production Entry

        if( session_->uiUtilities->isWrapOnly() ) // PJR-145: Wrap-only
            ui->tabSubFeatures->removeTab( 0 );// Flashkeys

        //  HA-141: Remove special auto lookup from Access
        ui->lblSpAutoLookupDigit->hide();
        ui->lblSpAutoLookupDigitX->hide();
        ui->cboSpAutoLookupDigit->hide();

        // PJR-145: Wrap-only
        if( session_->uiUtilities->isWrapOnly() )
        {
            ui->tabNormalScaleOperation->removeTab( 4 );  // Panels
            ui->tabNormalScaleOperation->removeTab( 0 );  // Product
        }

        // SOF-1001
        // Allow wrapper calibrate in reduced timing models
        DomEvolWrapperSystemInformation *evolWrapperSystemInformation( &session_->scaleConfig->evolWrapperSystemInformation );

        if ( evolWrapperSystemInformation->awsModelNumber() < DomEvolWrapperSystemInformation::reducedTimingModels )
            ui->calibrateWrapperButton->hide();
    }
	else
	{
        // Hide sub-tabs on the "Features" tab
        // Need to remove from right-to-left
        ui->tabSubFeatures->removeTab( 6 );    // Wrapper Settings
        ui->tabSubFeatures->removeTab( 5 );    // Sealer Belt

        // Hide right screen config on the Panels tab, only applies to prepack
        ui->lblRightPanelMode->setVisible( false );
        ui->cboRightPanelMode->setVisible( false );

        // Hide the "Tare setback" field
        ui->lblTareSetback->setVisible( false );
        ui->txtTareSetback->setVisible( false );
        ui->lblUnits_2->setVisible( false );
    }
    scaleScreen();
    expandTabs(ui->tabNormalScaleOperation);
    expandTabs(ui->tabSubFeatures);
    initialize();
    translate();

    // HTs
    // these features are disabled
    isLicensed_FPP_                   = session_->uiUtilities->isFeatureLicensed( SCALE_FPP );
    isLicensed_FullscreenFlashkeys_   = session_->uiUtilities->isFeatureLicensed( SCALE_FULLSCREEN_FLASHKEYS );
    isLicensed_WebBrowser_            = session_->uiUtilities->isFeatureLicensed( SCALE_WEB_BROWSER );
    isLicensed_MealBuilder_           = session_->uiUtilities->isFeatureLicensed( SCALE_MEAL_BUILDER );
    isLicensed_Enhanced_CenterScreen_ = session_->uiUtilities->isFeatureLicensed( SCALE_ENHANCED_CENTERSCREEN );

    ui->chkEnableProdPlan->setEnabled( isLicensed_FPP_ );
    ui->lblProdPlanFrame->setEnabled( isLicensed_FPP_ );
    ui->txtProdPlanningFrame->setEnabled( isLicensed_FPP_ );
    ui->lblProdPlanningUpdate->setEnabled( isLicensed_FPP_ );
    ui->txtProdPlanningUpdate->setEnabled( isLicensed_FPP_ );

    ui->lblAccessBrowserMode->setEnabled( isLicensed_FullscreenFlashkeys_ || isLicensed_WebBrowser_ );
    ui->cboAccessBrowserMode->setEnabled( isLicensed_FullscreenFlashkeys_ || isLicensed_WebBrowser_ );
    ui->lblStartScreenHome->setEnabled( isLicensed_FullscreenFlashkeys_ || isLicensed_WebBrowser_ || isLicensed_MealBuilder_ );
    ui->cboStartScreenHome->setEnabled( isLicensed_FullscreenFlashkeys_ || isLicensed_WebBrowser_ || isLicensed_MealBuilder_ );

    ui->lblDefaultBrowserHomePage->setEnabled( isLicensed_WebBrowser_ );
    ui->txtDefaultHomePage->setEnabled( isLicensed_WebBrowser_ );
    ui->chkRemoveAddressBar->setEnabled( isLicensed_WebBrowser_ );

    ui->lblRunScreenHomePage->setEnabled( isLicensed_WebBrowser_ );
    ui->txtRunScreenHomePage->setEnabled( isLicensed_WebBrowser_ );

    ui->lblEnhancedCenterScreenAddress->setEnabled( isLicensed_Enhanced_CenterScreen_ );
    ui->txtEnhancedCenterScreenAddress->setEnabled( isLicensed_Enhanced_CenterScreen_ );
    ui->chkEnableEnhancedCenterScreen->setEnabled( isLicensed_Enhanced_CenterScreen_ );
    ui->chkShowSearchInKeypad->setEnabled( isLicensed_Enhanced_CenterScreen_ );

    // PJR-145: Wrap-only
    if( session_->uiUtilities->isWrapOnly() )
    {
        // Scale Access tab:
        // -----------------

        ui->chkDisableOpFlashkeyEditor->hide();         // Disable operator flashkey editor
        ui->chkRemoveAddressBar->hide();                // Remove address bar from web control
        ui->chkDisableLeftPanel->hide();                // Disable left panel
        ui->chkShowPrinterSettings->hide();             // Allow access to Printer Settings
        ui->chkShowSelectAllWhenVoidingTrans->hide();   // Display 'Select All' when voiding transactions
        ui->chkShowDeleteAllFiles->hide();              // Allow access to Delete all Files
        ui->chkSearchMenuButton->hide();                // Display Search menu in Operator Mode

        // System tab:
        // -----------

        // By count calculation method
        ui->lblByCountCalculationMethod->hide();
        ui->cboByCountCalculationMethod->hide();

        // Features->Wrapper Settings tab:
        // -------------------------------

        // Label Placement
        ui->lblLabelPlacement->hide();
        ui->cboLabelPlacement->hide();

        ui->lblLabelPlacement_2->hide();
        ui->cboLabelPlacement_2->hide();

        // Film Width & Tension
        ui->chk_displayFilmWidth->hide();
        ui->chk_displayFilmTension->hide();
    }
}

SuperNormalScaleOperation::~SuperNormalScaleOperation()
{
    if(eanConfigEditor_ != NULL)
        delete eanConfigEditor_;

    if(signalMapper_ != NULL)
        delete signalMapper_;

    // Removed compress time fields 05-15-2015 during timesys 1.15 release
    //if( compressHours_ != NULL)
    //    delete compressHours_;
    //if( compressMinutes_ != NULL)
    //    delete compressMinutes_;
    if( maxTare_ != NULL)
        delete maxTare_;
    if( logoutIdleRemoteTime_ != NULL)
        delete logoutIdleRemoteTime_;
    if( numOfCharsOnFlashkey_ != NULL)
        delete numOfCharsOnFlashkey_;
    if( defaultFlashkeyFontPointsize_ != NULL)
        delete defaultFlashkeyFontPointsize_;
    if( resetStatsDays_ != NULL)
        delete resetStatsDays_;
    if( pingTarget1_ != NULL)
        delete pingTarget1_;
    if( pingTarget2_ != NULL)
        delete pingTarget2_;
    if( pingTarget3_ != NULL)
        delete pingTarget3_;
    if( pingTarget4_ != NULL)
        delete pingTarget4_;
    if( pingHours_ != NULL)
        delete pingHours_;
    if( pingMinutes_ != NULL)
        delete pingMinutes_;
    if( accumItemsScaleId_ != NULL)
        delete accumItemsScaleId_;
    if( accumItemsTransNum_ != NULL)
        delete accumItemsTransNum_;
    if( accumItemsEanCode_ != NULL)
        delete accumItemsEanCode_;
    if( accumItemsEanPrefix_ != NULL)
        delete accumItemsEanPrefix_;

    if (tareSetback_ != NULL)
        delete tareSetback_;
    if (periodicOfflineBeep_ != NULL)
        delete periodicOfflineBeep_;
    if (maxSuspendTests_ != NULL)
        delete maxSuspendTests_;
    if (maxSuspendTestsPerOper_ != NULL)
        delete maxSuspendTestsPerOper_;
    if (inactivityLogout_ != NULL)
        delete inactivityLogout_;
    if (shelfLifeHoursValidator_ != NULL)
        delete shelfLifeHoursValidator_;
    if (shelfLifeMinutesValidator_ != NULL)
        delete shelfLifeMinutesValidator_;
    if (transHoursToView_ != NULL)
        delete transHoursToView_;
    if (minSweetheartWeight_ != NULL)
        delete minSweetheartWeight_;

    delete ui;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool SuperNormalScaleOperation::helpText( QString& title, QString& description ) const
{
    Q_UNUSED( title );
    Q_UNUSED( description );

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::translate()
{
    ui->retranslateUi( this );

    ui->cboSuggSellStyle->setItemText(0, tr("None"));
    ui->cboSuggSellStyle->setItemText(1, tr("Okay"));
    ui->cboSuggSellStyle->setItemText(2, tr("Yes\\No"));

    ui->cboRemoteAccessLoginMethod->setItemText(0, tr("None"));
    ui->cboRemoteAccessLoginMethod->setItemText(1, tr("Operator ID"));
    ui->cboRemoteAccessLoginMethod->setItemText(2, tr("Username\\Password"));

    if ( !session_->isPrepack() )
        ui->cboSpAutoLookupDigit->setItemText(0, tr("None"));

    ui->cboStartScreenHome->setItemText(0, tr("Left Panel"));
    ui->cboStartScreenHome->setItemText(1, tr("Center Panel"));
    ui->cboStartScreenHome->setItemText(2, tr("Meal Builder"));

    ui->cboNumberPad->setItemText(0, tr("Number Pad") );
    ui->cboNumberPad->setItemText(1, tr("Flashkeys ") );
    ui->cboNumberPad->setItemText(2, tr("Product Image") );
    //ui->cboNumberPad->setItemText(3, tr("Best Sellers") ); // FT-669

    // disable web address if web control selected
    ui->cboAccessBrowserMode->setItemText(0, tr("Flashkeys "));
    ui->cboAccessBrowserMode->setItemText(1, tr("Best Sellers "));
    ui->cboAccessBrowserMode->setItemText(2, tr("Web Control "));
    ui->cboAccessBrowserMode->setItemText(3, tr("Fixed Flashkeys"));

    // disable web address if web control selected
    ui->cboRunScreen->setItemText(0, tr("Flashkeys "));
    ui->cboRunScreen->setItemText(1, tr("Best Sellers"));
    ui->cboRunScreen->setItemText(2, tr("Web Control"));
    ui->cboRunScreen->setItemText(3, tr("Fixed Flashkeys"));

    ui->cboRightPanelMode->setItemText(0, tr("Product Information"));
    ui->cboRightPanelMode->setItemText(1, tr("Wrapper Settings"));
    ui->cboRightPanelMode->setItemText(2, tr("Label Placement"));

    ui->cboBottomPanelMode->setItemText(0, tr("Build a Meal"));
    ui->cboBottomPanelMode->setItemText(1, tr("Cut Test Option One"));

    ui->cboLanguageCode->setItemText(0, tr("English (US)"));
    ui->cboLanguageCode->setItemText(1, tr("Spanish (Mexico)"));
    ui->cboLanguageCode->setItemText(2, tr("English (Canada)"));
    ui->cboLanguageCode->setItemText(3, tr("French (Canada)"));

    ui->cboSecLanguageCode->setItemText(0, tr("None"));
    ui->cboSecLanguageCode->setItemText(1, tr("English (US)"));
    ui->cboSecLanguageCode->setItemText(2, tr("Spanish (Mexico)"));
    ui->cboSecLanguageCode->setItemText(3, tr("English (Canada)"));
    ui->cboSecLanguageCode->setItemText(4, tr("French (Canada)"));

    ui->cboDateStyle->setItemText(0, tr("FE 28,24"));
    ui->cboDateStyle->setItemText(1, tr("Feb 28,24"));
    ui->cboDateStyle->setItemText(2, tr("24/02/28"));
    ui->cboDateStyle->setItemText(3, tr("02/28/24"));
    ui->cboDateStyle->setItemText(4, tr("28/Feb/24"));
    ui->cboDateStyle->setItemText(5, tr("28/02/24"));
    ui->cboDateStyle->setItemText(6, tr("24 FE 28"));
    ui->cboDateStyle->setItemText(7, tr("28 FEB 24"));

    ui->cboTimeFormat->setItemText(0, tr("12 Hour"));
    ui->cboTimeFormat->setItemText(1, tr("24 Hour"));

    ui->cboPopupKeypadStyle->setItemText(0, tr("Telephone"));
    ui->cboPopupKeypadStyle->setItemText(1, tr("PC Keyboard"));

    ui->cboOpLoginKeyboardStyle->setItemText(0, tr("QWERTY Keyboard"));
    ui->cboOpLoginKeyboardStyle->setItemText(1, tr("Numeric Keypad"));

    // Tab 4 - Network
    ui->cboNowServingType->setItemText(0, tr("None"));
    ui->cboNowServingType->setItemText(1, tr("Electronic"));
    ui->cboNowServingType->setItemText(2, tr("Pre-printed Dispenser"));

    // Features Tab || Flashkeys
    ui->cboDefaultSearchMethod->setItemText(0, tr("By description"));
    ui->cboDefaultSearchMethod->setItemText(1, tr("By plu number"));

    // Features Tab || Self Serve
    // where to start
    ui->cboStartWhere->setItemText(0, tr("None (default)"));
    ui->cboStartWhere->setItemText(1, tr("Green GO button"));                           // begin users on the Start Panel
    ui->cboStartWhere->setItemText(2, tr("Weight required to select item"));            // weight required to select item  ( NEW option: clickable? )
    ui->cboStartWhere->setItemText(3, tr("Use Playlists"));                             // enable playlists
    ui->cboStartWhere->setItemText(4, tr("Log into Self Service"));                     // forced login

    // item entry options
    ui->cboItemEntryWho->setItemText(0, tr("Product Flashkeys"));                       // default amoung flashkeys           (NEW option: Include keypad? )
    ui->cboItemEntryWho->setItemText(1, tr("Best Seller Flashkeys"));                   // show best sellers on center panel  (NEW option: Include keypad? )
    ui->cboItemEntryWho->setItemText(2, tr("Product Flashkeys with Keypad menu"));      // BigY request to includea Keypad button
    ui->cboItemEntryWho->setItemText(3, tr("Fixed Flashkeys"));                         // PJR-383: Fixed Flashkeys
    ui->cboItemEntryWho->setItemText(4, tr("Keypad only"));

    // how to search options
    ui->cboSearchHow->setItemText(0, tr("A-Z categories with Best Sellers"));           // search A-Z,
    ui->cboSearchHow->setItemText(1, tr("Character by character"));                     // search char by char
    ui->cboSearchHow->setItemText(2, tr("None (hide Search button)"));                  // No Search for you

    // What to print options
    ui->cboPrintWhat->setItemText( 0, tr("Print label") );                                 // regular default print option
    ui->cboPrintWhat->setItemText( 1, tr("Display label on screen") );                     // BigY & chkselfServeDisplayLabelOnScreenGO (Amazon Go - [X] Print label to screen)
    ui->cboPrintWhat->setItemText( 2, tr("Display label on screen and print") );           // BigY
    ui->cboPrintWhat->setItemText( 3, tr("Display barcode on screen and print") );         // Delhaize via Hannnaford
    if( session_->scaleConfig->getScaleModelName().contains( "SSLSC" ) )
        ui->cboPrintWhat->setItemText( 4, tr("Display QR code only") );                    // Walmart (PJR413/SOF-4772)

    ui->cboPrintingWhen->setItemText( 0, tr("Take-label video") );                         // originally the default
    ui->cboPrintingWhen->setItemText( 1, tr("Take-label image") );                         // when weight required

    // Fresh Serve Setup
    ui->cboFlashkeyType_0->setItemText( 0, tr("Softkey Group") );
    ui->cboFlashkeyType_0->setItemText( 1, tr("Best Sellers") );
    ui->cboFlashkeyType_1->setItemText( 0, tr("Softkey Group") );
    ui->cboFlashkeyType_1->setItemText( 1, tr("Best Sellers") );
    ui->cboFlashkeyType_2->setItemText( 0, tr("Softkey Group") );
    ui->cboFlashkeyType_2->setItemText( 1, tr("Best Sellers") );
    ui->cboFlashkeyType_3->setItemText( 0, tr("Softkey Group") );
    ui->cboFlashkeyType_3->setItemText( 1, tr("Best Sellers") );
    ui->cboFlashkeyType_4->setItemText( 0, tr("Softkey Group") );
    ui->cboFlashkeyType_4->setItemText( 1, tr("Best Sellers") );


    // PJR383
    // ui->cboSelfServeIgnorePrinterErrors->setItemText(0, tr("Print Label"));
    // ui->cboSelfServeIgnorePrinterErrors->setItemText(1, tr("Print Label to Screen"));
    // ui->cboSelfServeIgnorePrinterErrors->setItemText(2, tr("Print Label to Screen and Printer"));

    /* Restranslate on a language change the cutting test type combo box */
    // Features Tab || Cutting Test
    ui->cboCuttingTestType->setItemText(0, tr("None"));
    ui->cboCuttingTestType->setItemText(1, tr("Default"));
    ui->cboCuttingTestType->setItemText(2, tr("Option 1"));

    if ( session_->isPrepack() )
    {
        ui->cboIntakeMode->setItemText(0, tr("Gentle"));
        ui->cboIntakeMode->setItemText(1, tr("Normal"));

        ui->cboLabelPlacement->setItemText(0, tr("Upper Right"));
        ui->cboLabelPlacement->setItemText(1, tr("Upper Left"));
        /*
        ui->cboLabelPlacement->setItemText(3, tr("Lower Right"));
        ui->cboLabelPlacement->setItemText(4, tr("Lower Left"));
        */

        //ui->cboLabelPlacement_2->setItemText(0, tr("Upper Right"));
        //ui->cboLabelPlacement_2->setItemText(1, tr("Upper Left"));

        ui->cboFilmTension->setItemText(0, tr("Low"));
        ui->cboFilmTension->setItemText(1, tr("Medium"));
    }


    if( isMetric_ == false)
    {
        ui->cboFixedWeightUnits->setItemText(0, tr("Whole Oz") );
        ui->cboFixedWeightUnits->setItemText(1, tr("1/10 Oz") );
    }

    ui->cboAlwaysEnableVirtualKeyboard->setItemText( 0, tr("Never Enable" ));
    ui->cboAlwaysEnableVirtualKeyboard->setItemText( 1, tr("Always Enable" ));

    // these are done on INDEX, NOT DATA OR ENUM VAL

    ui->cboBarcodeTypeOver99->setItemText( 0, tr("EAN-13" ) );          // default
    ui->cboBarcodeTypeOver99->setItemText( 1, tr("EAN-128" ) );         // now known as GS1-128
    ui->cboBarcodeTypeOver99->setItemText( 2, tr("GS1 GTIN-14" ) );     // These are GTINs
    ui->cboBarcodeTypeOver99->setItemText( 3, tr("GS1 GTIN-13" ) );     // These are GTINs
    ui->cboBarcodeTypeOver99->setItemText( 4, tr("QR Code M" ) );
    ui->cboBarcodeTypeOver99->setItemText( 5, tr("QR Code L" ) );
    ui->cboBarcodeTypeOver99->setItemText( 6, tr("QR Code Q" ) );
    ui->cboBarcodeTypeOver99->setItemText( 7, tr("QR Code H" ) );
    ui->cboBarcodeTypeOver99->setItemText( 8, tr("Data Matrix" ) );
    ui->cboBarcodeTypeOver99->setItemText( 9, tr("GS1 GTIN-14 (One-Line)" ) );  // Also a GTIN!

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool SuperNormalScaleOperation::onScreenChanging()
{
    LOG_CLASS_INFO( "SuperNormalScaleOperation", "onScreenChanging" );

    LOG_DEBUG( "Top of onScreenChanging", NULL );

    currentChangeTab( -1 );                                         // save current tab, pass a -1 will save current no matter what

    saveDataOnExit();

    return _Mybase::onScreenChanging();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                          CLASS METHODS
//
//////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::initialize()
{
    // initialize vars
    // Removed compress time fields 05-15-2015 during timesys 1.15 release
    //compressHours_ = NULL;
    //compressMinutes_ = NULL;
    maxTare_ = NULL;
    logoutIdleRemoteTime_ = NULL;
    numOfCharsOnFlashkey_ = NULL;
    defaultFlashkeyFontPointsize_ = NULL;
    resetStatsDays_ = NULL;
    pingTarget1_ = NULL;
    pingTarget2_ = NULL;
    pingTarget3_ = NULL;
    pingTarget4_ = NULL;
    pingHours_ = NULL;
    pingMinutes_ = NULL;
    accumItemsScaleId_ = NULL;
    accumItemsTransNum_ = NULL;
    accumItemsEanCode_ = NULL;
    accumItemsEanPrefix_ = NULL;

    tareSetback_ = NULL;
    periodicOfflineBeep_ = NULL;
    maxSuspendTests_ = NULL;
    maxSuspendTestsPerOper_ = NULL;
    inactivityLogout_ = NULL;
    shelfLifeHoursValidator_ = NULL;
    shelfLifeMinutesValidator_ = NULL;
    transHoursToView_ = NULL;
    minSweetheartWeight_ = NULL;

    hasChanged = false;
    hasChangedDisplayOptions = false;
    tabAccumHasChanged = false;
    dateTimeHasChanged = false;
    langHasChanged = false;
    theWeigherMaxWt = 0;                                            // dont know this until weigher update
    initNetworkCbos = true;                                         // true for network checkboxes the first time in
    currentTab = 0;                                                 // the current tab always starts with the 0 tab
    isUpdating = false;
    isMetric_ =  false;
    weigherModel_ = SessionMainWindow::HLX1;                        // DEFAULT: this may change but right now its the HLX-1 weigher
    neverCallThisCodeAgain = false;                                 // only call the validators ONCE on entry
    maxTareHasBeenUpdated = false;
    dataHasBeenInitialized = false;                                 // set to true when message comes in and is processed
    wrDisplayFieldsHaveChanged = false;
    msgCounter = 0;                                                 // have to receive 4 config msgs before dataHasBeenInitialized can be set to true
    alreadyReceivedScaleOpWriteMessage = false;
    alreadyReceivedCfpPrepackMessage = false;

    isShelfLifeAmSelected = true;

    currencyData_    = &session_->scaleConfig->currencySettings;
    scaleConfigData_ = &session_->scaleConfig->scaleConfig;

    isMetric_ =  scaleConfigData_->isScaleMetric();
    // JHC - TEST TO FORCE lbs
    //isMetric_ = 0;

    teDialog_ = NULL;

	sundayOnTime_ = ui->le_timeOnSunday;
    mondayOnTime_ = ui->le_timeOnMonday;
    tuesdayOnTime_ = ui->le_timeOnTuesday;
    wednesdayOnTime_ = ui->le_timeOnWednesday;
    thursdayOnTime_ = ui->le_timeOnThursday;
    fridayOnTime_ = ui->le_timeOnFriday;
    saturdayOnTime_ = ui->le_timeOnSaturday;
    sundayOffTime_ = ui->le_timeOffSunday;
    mondayOffTime_ = ui->le_timeOffMonday;
    tuesdayOffTime_ = ui->le_timeOffTuesday;
    wednesdayOffTime_ = ui->le_timeOffWednesday;
    thursdayOffTime_ = ui->le_timeOffThursday;
    fridayOffTime_ = ui->le_timeOffFriday;
    saturdayOffTime_ = ui->le_timeOffSaturday;

    // SOF-3846: Find out if a Cutter is attached
    cutterAttached_ = false;
    cutterEnabled_ = false;
    session_->send( Printer, PrReqCutterStatus );
    ui->chkDisableCuttingBetweenLinerlessLabels->setEnabled( false );

    disableCuttingBetweenLabelsToggled = false;

    // TO ADD A WRAPPER TAB
    // WRAPPER NOTE - In order to add a Wrapper Tab, the other tabs will have to be made less wide.  The width the tabs are
    // WRAPPER NOTE - right now is 148px. They need to be resized to 126px to then add a Wrapper tab.  You must set the stylesheet
    // WRAPPER NOTE - property on the tab.  ui->tabNormalScaleOperation->setStyleSheet("QTabBar::tab { border: 2px solid #C4C4C3; border-color: rgb(0, 176, 240); border-bottom-color: rgb(0,176,240); height: 50px; min-width: 148px; background: rgb(227, 227, 227); border-top-left-radius: 8px; border-top-right-radius: 8px; }"     );
    // WRAPPER NOTE - That is for the QTabBar part of it.  You must also add in the other style sheet properties as well.  Just go
    // WRAPPER NOTE - to the stylesheet property in QT Creator and use that as a basis of the entire property you must set here.

    //setupValidators();                                            // moved to onSessionMessage()
    //setupValidators();        // moved to onSessionMessage()
    ui->txtPingMinutes->setValidator(pingMinutes_ = new QIntValidator(0, INT_MAX, this) );
    ui->txtPingMinutes->setDecimalPlaces(0);
    ui->txtPingMinutes->setInputWrapping(true);
    pingMinutes_->setTop(59);

    setupWidgets();							//	Let's set up the widgets before the slots and signals.
    setupSlotsAndSignals();                                         // initialize() was just getting too large
    setupWeigherFields(); //HT-59 max tare will not be set correctly since setupValidators was moved to sessionMessage()
    setupSelfServePlaylists();

    //FT-54 Network connectivity cannot be turned off, changes will not save
	updateNetworkCheckboxesChanged( ui->chkCheckNetConnectivity );
    update();
    ui->tabNormalScaleOperation->setTabEnabled(1,false);
    ui->tabNormalScaleOperation->setTabEnabled(2,false);
    ui->tabNormalScaleOperation->setTabEnabled(3,false);
    ui->tabNormalScaleOperation->setTabEnabled(4,false);
    ui->tabNormalScaleOperation->setTabEnabled(5,false);
    ui->tabNormalScaleOperation->setTabEnabled(6,false);

    ui->cboBarcodeTypeOver99->setCurrentIndex( 0 );           // set as 0th item until updated. goes with chkAutoUpdateUPCBarcodeOver99

    ui->txtAccumItemsServerAddress->setVisible( false );
    ui->txtAccumItemsServerAddress->setEnabled(false);
    ui->lblServerStatus->setVisible( false );
    ui->lblServerAddress->setVisible( false );
    ui->testServerStatus->setVisible( false );

    connect(ui->testServerStatus, SIGNAL (clicked()), SLOT(testServerStatusClicked()));

    // send out initial batch of messages and requests
    // The request for the weigher, a 1,36 msg, occurs on the reply of the 112,70. If no weigher, the 1,2 reply will not occur
    translate();
    session_->send(ConfigurationDev, CfgCurrencyRead);              // 112,100 -> get Currency
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::setupSlotsAndSignals()
{
    //  Setup the SIGNALS and SLOTs

    // Tab 0 - Product

    // Tab 0.0
    connect(ui->cboFixedWeightUnits,                SIGNAL(currentIndexChanged(int)),    this, SLOT(comboboxChanged(int)));
    connect(ui->cboExternalWeightPrecision,         SIGNAL(currentIndexChanged(int)),    this, SLOT(comboboxChanged(int)));
    connect(ui->txtMaxTare,                         SIGNAL(editingFinished()),		     this, SLOT(textboxChangedMaxTare()));
    if ( session_->isPrepack() )
        connect(ui->txtTareSetback,                 SIGNAL(editingFinished()),		     this, SLOT(textboxChangedTareSetback()));
    connect(ui->chkUseAutoLookup,                   SIGNAL(stateChanged(int)),           this, SLOT(checkboxChangedAutoLookup(int)));
    connect(ui->cboNumOfDigits,                     SIGNAL(currentIndexChanged(int)),    this, SLOT(comboboxChangedNumOfDigits(int)));
    if ( !session_->isPrepack() )
        connect(ui->cboSpAutoLookupDigit,           SIGNAL(currentIndexChanged(int)),    this, SLOT(comboboxChanged(int)));
    connect(ui->chkUseSecondTare,                   SIGNAL(stateChanged (int)),          this, SLOT(checkboxChanged(int)));
    connect(ui->chkFixedWeightVerification,         SIGNAL(stateChanged (int)),          this, SLOT(checkboxChanged(int)));
    connect(ui->chkEnableShelfLifeExtraDay,         SIGNAL(stateChanged(int)),           this, SLOT(checkboxChanged(int)));
    connect(ui->chkEnableShelfLifeExtraDay,         SIGNAL(stateChanged(int)),           this, SLOT(checkboxChangedEnableExtraShelfLifeDay(int)));
    connect(ui->butShelfLifeAmPm,                   SIGNAL(clicked()),                   this, SLOT(toggleShelfLifeAmPm()));

    // Tab 0.1
    connect(ui->chkAutoDisplaySuggestiveSelling,    SIGNAL(stateChanged (int)),          this, SLOT(checkboxChanged(int)));
    connect(ui->txtSuggSellFreq,                    SIGNAL(editingFinished()),    		 this, SLOT(textboxChanged()));
    connect(ui->cboSuggSellStyle,                   SIGNAL(currentIndexChanged(int)),    this, SLOT(comboboxChanged(int)));
    connect(ui->chkAutoDisplayProdNotes,            SIGNAL(stateChanged (int)),          this, SLOT(checkboxChanged(int)));
    connect(ui->chkAutoDisplayCookingNotes,         SIGNAL(stateChanged (int)),          this, SLOT(checkboxChanged(int)));
    connect(ui->chkAutoUpdateUPCBarcodeOver99,      SIGNAL(stateChanged (int)),          this, SLOT(checkboxChanged(int)));
    connect(ui->cboBarcodeTypeOver99,               SIGNAL(currentIndexChanged(int)),    this, SLOT(comboboxChanged(int)));
    connect(ui->chkClearPluAfterPrinting,           SIGNAL(stateChanged (int)),          this, SLOT(checkboxChanged(int)));
    connect(ui->chkDoNotPrintNutrifacts,            SIGNAL(stateChanged (int)),          this, SLOT(checkboxChanged(int)));

    // Tab 0.2
    connect(ui->chkTrackOperChanges,                SIGNAL(stateChanged (int)),          this, SLOT(checkboxChangedTrackOperators(int)));
    connect(ui->chkTrackSuperChanges,               SIGNAL(stateChanged (int)),          this, SLOT(checkboxChangedTrackSuperOperators(int)));
    connect(ui->chkCollectTransactions,             SIGNAL(stateChanged (int)),          this, SLOT(checkboxChangedCollectTrans(int)));
    connect(ui->cboDaysToKeepTransactions,          SIGNAL(currentIndexChanged(int)),    this, SLOT(comboboxChanged(int)));
    connect(ui->cboDaysToKeepTransactions,          SIGNAL(currentIndexChanged(int)),    this, SLOT(comboboxDaysToKeepTrans( int )));
    connect(ui->txtTransHoursToView,                SIGNAL(editingFinished()),    		 this, SLOT(textboxChanged()));
    connect(ui->chkCOOLTrackNum,                    SIGNAL(stateChanged (int)),          this, SLOT(checkboxChanged(int)));
    connect(ui->txtMinSweetheartWeight,             SIGNAL(editingFinished()),    		 this, SLOT(textboxChangedMinSweetheartWeight()));

    // Tab 1 - Scale Access
    connect(ui->chkRemoveAddressBar,                SIGNAL(stateChanged (int)),          this, SLOT(checkboxChangedRemoveAddressBar(int)));
    connect(ui->chkDisableOpFlashkeyEditor,         SIGNAL(stateChanged (int)),          this, SLOT(checkboxChangedDisableOpFlashkeyEditor(int)));
    connect(ui->chkShowPrinterSettings,             SIGNAL(stateChanged (int)),          this, SLOT(checkboxChangedUpdatePrinter(int)));
    connect(ui->chkDisableLeftPanel,                SIGNAL(stateChanged (int)),          this, SLOT(checkboxChangedDisableLeftPanel(int)));
    connect(ui->chkShowDeleteAllFiles,              SIGNAL(stateChanged (int)),          this, SLOT(checkboxChangedUpdateDelete(int)));

    connect(ui->chkRemoteAccessPermitted,           SIGNAL(stateChanged (int)),          this, SLOT(checkboxChangedRemoteAccess(int)));
    connect(ui->cboRemoteAccessLoginMethod,         SIGNAL(currentIndexChanged(int)),    this, SLOT(comboboxLoginMethodChanged(int)));
    connect(ui->txtRemoteAccessUserName,            SIGNAL(editingFinished()),    		 this, SLOT(textboxChanged()));
    connect(ui->txtRemoteAccessPassword,            SIGNAL(editingFinished()),    		 this, SLOT(textboxChanged()));
    connect(ui->txtLogoutRemoteIdleTime,            SIGNAL(editingFinished()),    		 this, SLOT(textboxChanged()));
    connect(ui->chkShowSelectAllWhenVoidingTrans,   SIGNAL(stateChanged (int)),          this, SLOT(checkboxChanged(int)));
    connect(ui->chkSearchMenuButton,                SIGNAL(stateChanged (int)),          this, SLOT(checkboxChangedUpdateSearch(int)));
    connect(ui->chkShowSearchInKeypad,              SIGNAL(stateChanged (int)),          this, SLOT(checkboxChangedUpdateSearchOnKeypad(int)));

    // Tab 2 - System
    connect(ui->cboLanguageCode,                    SIGNAL(currentIndexChanged(int)),    this, SLOT(comboboxPriLangChanged(int)));
    connect(ui->cboSecLanguageCode,                 SIGNAL(currentIndexChanged(int)),    this, SLOT(comboboxSecLangChanged(int)));
    connect(ui->cboDateStyle,                       SIGNAL(currentIndexChanged(int)),    this, SLOT(comboboxDateChanged(int)));
    connect(ui->cboTimeFormat,                      SIGNAL(currentIndexChanged(int)),    this, SLOT(comboboxTimeChanged(int)));
    connect(ui->cboPopupKeypadStyle,                SIGNAL(currentIndexChanged(int)),    this, SLOT(comboboxChanged(int)));
    connect(ui->cboOpLoginKeyboardStyle,            SIGNAL(currentIndexChanged(int)),    this, SLOT(comboboxChanged(int)));
    connect(ui->cboByCountCalculationMethod,        SIGNAL(currentIndexChanged(int)),    this, SLOT(comboboxChanged(int)));
    connect(ui->cboAlwaysEnableVirtualKeyboard,     SIGNAL(currentIndexChanged (int)),   this, SLOT(comboboxChanged(int)));
    if ( !session_->isPrepack() )
        connect(ui->chkClearPluAfterPrinting,       SIGNAL(stateChanged (int)),          this, SLOT(checkboxChanged(int)));
    else
    {
        ui->chkClearPluAfterPrinting->setHidden(true);
    }

    // Tab 3 - Network
    signalMapper_ = new QSignalMapper( this );
    connect(ui->chkCheckNetConnectivity,            SIGNAL(stateChanged (int)),          signalMapper_, SLOT(map()));
    if ( session_->isServiceCounter() )
    {
        connect(ui->chkPingPrimary,                 SIGNAL(stateChanged (int)),          signalMapper_, SLOT(map()));
    }
    else
    {
        ui->chkPingPrimary->setHidden(true);    //  HA-142: Remove/hide this for a Prepack scale
    }
    connect(ui->chkPingGateway,                     SIGNAL(stateChanged (int)),          signalMapper_, SLOT(map()));
    connect(ui->chkPingTarget,                      SIGNAL(stateChanged (int)),          signalMapper_, SLOT(map()));
    connect(ui->chkNetworkDetailsDisplay,           SIGNAL(stateChanged (int)),          signalMapper_, SLOT(map()));
    signalMapper_->setMapping( ui->chkCheckNetConnectivity, ui->chkCheckNetConnectivity );
    if ( session_->isServiceCounter() )
    {
        signalMapper_->setMapping( ui->chkPingPrimary, ui->chkPingPrimary );
    }
    signalMapper_->setMapping( ui->chkPingGateway, ui->chkPingGateway );
    signalMapper_->setMapping( ui->chkPingTarget, ui->chkPingTarget );
    signalMapper_->setMapping( ui->chkNetworkDetailsDisplay, ui->chkNetworkDetailsDisplay );
    connect(signalMapper_,                          SIGNAL(mapped(QWidget*)),            this, SLOT(updateNetworkCheckboxesChanged(QWidget*)));

    connect(ui->txtPingTarget1,                     SIGNAL(editingFinished()),    		this, SLOT(textboxChanged()));
    connect(ui->txtPingTarget2,                     SIGNAL(editingFinished()),    		this, SLOT(textboxChanged()));
    connect(ui->txtPingTarget3,                     SIGNAL(editingFinished()),    		this, SLOT(textboxChanged()));
    connect(ui->txtPingTarget4,                     SIGNAL(editingFinished()),    		this, SLOT(textboxChanged()));
    connect(ui->txtPingHours,                       SIGNAL(editingFinished()),    	    this, SLOT(textboxChanged()));
    connect(ui->txtPingMinutes,                     SIGNAL(editingFinished()),   		this, SLOT(textboxChanged()));
    connect(ui->txtPingHours,                       SIGNAL(textChanged(QString)),       this, SLOT(textPingHoursChanged(QString)));

    connect(ui->txtPeriodicOfflineBeep,             SIGNAL(editingFinished()),    	    this, SLOT(textboxChanged()));

    // Tab 4 - Panels
    connect(ui->chkEnableCountryOfOrgin,            SIGNAL(stateChanged (int)),          this, SLOT(checkboxChangedDisplayOptions(int)));
    connect(ui->chkEnableTrackingNumber,            SIGNAL(stateChanged (int)),          this, SLOT(checkboxChangedDisplayOptions(int)));
    connect(ui->chkEnableWrapTypes,                 SIGNAL(stateChanged (int)),          this, SLOT(checkboxChangedDisplayOptions(int)));
    connect(ui->chkEnableTransType,                 SIGNAL(stateChanged(int)),           this, SLOT(checkboxChangedEnableTransType(int)));
    connect(ui->chkEnableSellBy,                    SIGNAL(stateChanged (int)),          this, SLOT(checkboxChangedDisplayOptions(int)));
    connect(ui->chkEnableBestBefore,                SIGNAL(stateChanged (int)),          this, SLOT(checkboxChangedDisplayOptions(int)));
    connect(ui->chkEnablePackedOn,                  SIGNAL(stateChanged (int)),          this, SLOT(checkboxChangedDisplayOptions(int)));
    connect(ui->cboAccessBrowserMode,               SIGNAL(currentIndexChanged(int)),    this, SLOT(comboboxChangedAccessBrowser(int)));
    connect(ui->cboRunScreen,                       SIGNAL(currentIndexChanged(int)),    this, SLOT(comboboxChangedRunScreenBrowser(int)));
    connect(ui->chkEnableEnhancedCenterScreen,      SIGNAL(stateChanged(int)),           this, SLOT(checkboxChangedEnableEnhancedCenterScreen(int)));

    connect(ui->cboNumberPad,                       SIGNAL(currentIndexChanged(int)),    this, SLOT(checkboxChangedDisplayOptions(int)));
    connect(ui->cboStartScreenHome,                 SIGNAL(currentIndexChanged(int)),    this, SLOT(comboboxChangedOperateStartScreen(int)));
    connect(ui->txtEnhancedCenterScreenAddress,     SIGNAL(editingFinished()),    		 this, SLOT(textboxChangedEnhancedCenterHomePage()));
    connect(ui->txtRunScreenHomePage,               SIGNAL(editingFinished()),    		 this, SLOT(textboxChangedRunScreenHomePage()));
    connect(ui->txtDefaultHomePage,                 SIGNAL(editingFinished()),    		 this, SLOT(textboxChangedDefaultHomePage()));

    if ( session_->isPrepack() )
    {
        connect(ui->cboRightPanelMode,              SIGNAL(currentIndexChanged(int)),    this, SLOT(comboboxRightPanelMode(int)));
    }

    // Tab 5.0 - Features->Flashkeys
    connect(ui->chkAppendingPluNumberToName,        SIGNAL(stateChanged (int)),          this, SLOT(appendingFlashkeyCheckboxChanged(int)));
    connect(ui->chkNumberOfCharacters,              SIGNAL(stateChanged (int)),          this, SLOT(numOfCharsOnFlashkeyCheckboxChanged(int)));
    connect(ui->txtNumberOfCharacters,              SIGNAL(editingFinished()),    		 this, SLOT(numOfCharsOnFlashkeyTextChanged()));
    connect(ui->txtResetStatisticsDays,             SIGNAL(editingFinished()),    		 this, SLOT(textboxChangedResetStatistics()));
    connect(ui->butReset,                           SIGNAL(clicked()),                   this, SLOT(resetStatistics()));
    connect(ui->cboDefaultSearchMethod,             SIGNAL(currentIndexChanged(int)),    this, SLOT(comboboxChangedSearchMethod(int)));

    /* Set up the Cutting Test Signals and Slots */
    // Tab 5.7 - Features->Cutting Test
    connect(ui->cboCuttingTestType,                 SIGNAL(currentIndexChanged(int)),    this, SLOT(comboboxChangedCutTest(int)));      ///<Cut Test Type SIGNAL/SLOT
    connect(ui->txtMaxSuspendedTests,               SIGNAL(editingFinished()),    		 this, SLOT(textboxChanged()));                 ///<Suspended Tests SIGNAL/SLOT
    connect(ui->txtMaxSuspendedTestsPerOperator,    SIGNAL(editingFinished()),    		 this, SLOT(textboxChanged()));                 ///<Suspended Tests/Operator SIGNAL/SLOT
    connect(ui->txtInactivitylogout,                SIGNAL(editingFinished()),    		 this, SLOT(textboxChanged()));                 ///<Inactivity timeout SIGNAL/SLOT

    connect(ui->txtDefaultFlashkeyFontPointsize,    SIGNAL(editingFinished()),    		 this, SLOT(defaultFlashkeyFontPointsizeChanged()));

    if ( !session_->isPrepack() )
    {
        // Tab 5.1 - Production Entry
        connect(ui->chkEnableProdEntry,                 SIGNAL(stateChanged (int)),          this, SLOT(checkboxChangedUpdateProdEntry(int)));
        connect(ui->chkProdEntryContinuousPrint,        SIGNAL(stateChanged (int)),          this, SLOT(checkboxChangedDisplayOptions(int)));
        connect(ui->chkDisableCuttingBetweenLinerlessLabels, SIGNAL( stateChanged ( int ) ), this, SLOT(checkboxChangedDisplayOptions( int ) ) );
        connect(ui->chkDisableCuttingBetweenLinerlessLabels, SIGNAL( stateChanged ( int ) ), this, SLOT(checkboxChangedDisableCuttingBetweenLabels( int ) ) );

        connect(ui->chkEnableProdPlan,                  SIGNAL(stateChanged (int)),          this, SLOT(checkboxChangedProdPlan(int)));
        connect(ui->txtProdPlanningFrame,               SIGNAL(textEdited(QString)),         this, SLOT(textboxChangedDisplayOptions(QString)));
        connect(ui->txtProdPlanningUpdate,              SIGNAL(textEdited(QString)),    	 this, SLOT(textboxChangedDisplayOptions(QString)));

        // Tab 5.2 Accumulate Items
        connect(ui->chkEnableAccumItems,                SIGNAL(stateChanged (int)),          this, SLOT(checkboxChangedUpdateAccumulate(int)));
        connect(ui->chkRequireWeightForMealItems,       SIGNAL(stateChanged (int)),          this, SLOT(checkboxChangedAccumulate(int)));
        connect(ui->chkAccumItemsReturnToStartScreen,   SIGNAL(stateChanged (int)),          this, SLOT(checkboxChangedAccumulate(int)));
        connect(ui->chkReadTransactionsCompatibility,   SIGNAL(stateChanged (int)),          this, SLOT(checkboxChangedAccumulate(int)));
        connect(ui->chkEnableItemStripMode,             SIGNAL(stateChanged (int)),          this, SLOT(checkboxChangedAccumulateStrip(int)));
        connect(ui->chkEnableAccumItemsTotalsOnly,      SIGNAL(stateChanged (int)),          this, SLOT(checkboxChangedAccumulate(int)));
        connect(ui->chkBarcodeUseFirstItem,             SIGNAL(stateChanged (int)),          this, SLOT(checkboxChangedAccumulate(int)));
        connect(ui->butAccumItemsEANFormatString,       SIGNAL(clicked()),                   this, SLOT(launchEanEditor()));
        connect(ui->txtAccumItemsEANPrefix,             SIGNAL(editingFinished()),    		 this, SLOT(textboxChangedAccumulate()));
        connect(ui->txtAccumItemsEANCode,               SIGNAL(editingFinished()),    		 this, SLOT(textboxChangedAccumulate()));
        connect(ui->txtAccumItemsScaleId,               SIGNAL(editingFinished()),    		 this, SLOT(textboxChangedAccumulate()));
        connect(ui->txtAccumItemsTransNum,              SIGNAL(editingFinished()),    		 this, SLOT(textboxChangedAccumulate()));
        connect(ui->chkEnableAccumItemsAndFlashkeys,    SIGNAL(stateChanged (int)),          this, SLOT(checkboxChangedAccumulate(int)));
        connect(ui->chkEnableCloudFeature,              SIGNAL(stateChanged (int)),          this, SLOT(checkboxChangedEnableCloudFeature(int)));
        connect(ui->txtAccumItemsServerAddress,         SIGNAL(editingFinished()),           this, SLOT(textboxChangedAccumulate()));

        // Tab 5.3 Now Serving
        connect(ui->txtNowServing1,                     SIGNAL(editingFinished()),    		this, SLOT(textboxChanged()));
        connect(ui->txtNowServing2,                     SIGNAL(editingFinished()),    		this, SLOT(textboxChanged()));
        connect(ui->txtNowServing3,                     SIGNAL(editingFinished()),    		this, SLOT(textboxChanged()));
        connect(ui->txtNowServing4,                     SIGNAL(editingFinished()),    		this, SLOT(textboxChanged()));
        connect(ui->cboNowServingType,                  SIGNAL(currentIndexChanged(int)),   this, SLOT(comboboxChangedNowServing(int)));

        // Tab 5.4 Self Service
        connect(ui->chkEnableSelfService,                     SIGNAL(stateChanged (int)),          this, SLOT(checkboxChangedDisplayOptions(int)));
        connect( ui->chkEnableGen2UI,                         SIGNAL(stateChanged (int)),          this, SLOT(checkboxChangedSelfServGenUIs(int)));
        connect(ui->chkHelpPrompts,                           SIGNAL(stateChanged (int)),          this, SLOT(checkboxChanged(int)));
        connect(ui->chkSelfServeTareOnRandomWeight,           SIGNAL(stateChanged (int)),          this, SLOT(checkboxChangedDisplayOptions(int)));
        connect(ui->chkSelfServeByCountItemsAllowed,          SIGNAL(stateChanged (int)),          this, SLOT(checkboxChangedDisplayOptions(int)));
        connect(ui->chkSelfServeDisplayPluLabel,              SIGNAL(stateChanged (int)),          this, SLOT(checkboxChangedDisplayOptions(int)));
        connect(ui->butEditPlaylist,                          SIGNAL(clicked()),                   this, SLOT(editPlaylistFile() ) );
        connect(ui->cboSelfServePlaylist,                     SIGNAL(currentIndexChanged(int)),    this, SLOT(comboboxChangedDisplayOptions(int)));
        connect(ui->cboStartWhere,                            SIGNAL(currentIndexChanged (int)),   this, SLOT(comboboxChangedSelfServStartScreen(int)));
        connect(ui->cboItemEntryWho,                          SIGNAL(currentIndexChanged (int)),   this, SLOT(comboboxChangedSelfServItemEntryScreen(int)));
        connect(ui->cboSearchHow,                             SIGNAL(currentIndexChanged (int)),   this, SLOT(comboboxChangedSelfServSearchScreen(int)));
        connect(ui->cboPrintWhat,                             SIGNAL(currentIndexChanged (int)),   this, SLOT(comboboxChangedSelfServPrintScreen(int)));
        connect(ui->cboPrintingWhen,                          SIGNAL(currentIndexChanged (int)),   this, SLOT(comboboxChangedSelfServAfterPrintScreen(int)));
        connect(ui->chkStartScreenClickable,                  SIGNAL(stateChanged (int)),          this, SLOT(checkboxChangedDisplayOptions(int)));
        connect(ui->chkSelfServePrintOnStableWeight,          SIGNAL(stateChanged (int)),          this, SLOT(checkboxChangedDisplayOptions(int)));
        connect(ui->chkIncludeNumpad,                         SIGNAL(stateChanged (int)),          this, SLOT(checkboxChangedDisplayOptions(int)));
        connect(ui->chkSelfServeHideButtonBar,                SIGNAL(stateChanged (int)),          this, SLOT(checkboxChangedDisplayOptions(int)));

        connect(ui->cboFlashkeyType_0,                        SIGNAL(currentIndexChanged(int)),    this, SLOT(comboboxChangedFlashkeyType(int)));
        connect(ui->cboFlashkeyType_1,                        SIGNAL(currentIndexChanged (int)),   this, SLOT(comboboxChangedFlashkeyType(int)));
        connect(ui->cboFlashkeyType_2,                        SIGNAL(currentIndexChanged (int)),   this, SLOT(comboboxChangedFlashkeyType(int)));
        connect(ui->cboFlashkeyType_3,                        SIGNAL(currentIndexChanged (int)),   this, SLOT(comboboxChangedFlashkeyType(int)));
        connect(ui->cboFlashkeyType_4,                        SIGNAL(currentIndexChanged (int)),   this, SLOT(comboboxChangedFlashkeyType(int)));

        connect(ui->txtFlashkeyText_0,                    SIGNAL(textEdited(QString)),    	   this, SLOT(textboxChangedDisplayOptions(QString)));
        connect(ui->txtFlashkeyText_1,                    SIGNAL(textEdited(QString)),    	   this, SLOT(textboxChangedDisplayOptions(QString)));
        connect(ui->txtFlashkeyText_2,                    SIGNAL(textEdited(QString)),    	   this, SLOT(textboxChangedDisplayOptions(QString)));
        connect(ui->txtFlashkeyText_3,                    SIGNAL(textEdited(QString)),    	   this, SLOT(textboxChangedDisplayOptions(QString)));
        connect(ui->txtFlashkeyText_4,                    SIGNAL(textEdited(QString)),    	   this, SLOT(textboxChangedDisplayOptions(QString)));

        connect(ui->txtFlashkeySecText_0,                 SIGNAL(textEdited(QString)),    	   this, SLOT(textboxChangedDisplayOptions(QString)));
        connect(ui->txtFlashkeySecText_1,                 SIGNAL(textEdited(QString)),    	   this, SLOT(textboxChangedDisplayOptions(QString)));
        connect(ui->txtFlashkeySecText_2,                 SIGNAL(textEdited(QString)),    	   this, SLOT(textboxChangedDisplayOptions(QString)));
        connect(ui->txtFlashkeySecText_3,                 SIGNAL(textEdited(QString)),    	   this, SLOT(textboxChangedDisplayOptions(QString)));
        connect(ui->txtFlashkeySecText_4,                 SIGNAL(textEdited(QString)),    	   this, SLOT(textboxChangedDisplayOptions(QString)));

        connect(ui->txtFlashkeyNum_0,                     SIGNAL(textEdited(QString)),    	   this, SLOT(textboxChangedDisplayOptions(QString)));
        connect(ui->txtFlashkeyNum_1,                     SIGNAL(textEdited(QString)),    	   this, SLOT(textboxChangedDisplayOptions(QString)));
        connect(ui->txtFlashkeyNum_2,                     SIGNAL(textEdited(QString)),    	   this, SLOT(textboxChangedDisplayOptions(QString)));
        connect(ui->txtFlashkeyNum_3,                     SIGNAL(textEdited(QString)),    	   this, SLOT(textboxChangedDisplayOptions(QString)));
        connect(ui->txtFlashkeyNum_4,                     SIGNAL(textEdited(QString)),    	   this, SLOT(textboxChangedDisplayOptions(QString)));

        // PJR413 (SOF-4772) - FixedPLU
        connect(ui->txtSelfServeFixedPlu,                     SIGNAL(textEdited(QString)),         this, SLOT(textboxChangedDisplayOptions(QString)));

        connect(ui->butShiftLeft,                             SIGNAL(clicked()),                   this, SLOT(editLabelOffsetLeft() ) );
        connect(ui->butShiftRight,                            SIGNAL(clicked()),                   this, SLOT(editLabelOffsetRight() ) );
    }
    else
    {
        // Tab 5.5 - Wrapper Settings (Prepack)
        connect(ui->cboIntakeMode,                      SIGNAL(currentIndexChanged(int)),    this, SLOT(comboboxChanged(int)));
        connect(ui->cboLabelPlacement,                  SIGNAL(currentIndexChanged(int)),    this, SLOT(comboboxChanged(int)));
        connect(ui->cboLabelPlacement,                  SIGNAL(currentIndexChanged(int)),    this, SLOT(onPrimaryLabelChange(int)));
        connect(ui->cboLabelPlacement_2,                SIGNAL(currentIndexChanged(int)),    this, SLOT(comboboxChanged(int)));
        connect(ui->cboFilmTension,                     SIGNAL(currentIndexChanged(int)),    this, SLOT(comboboxChanged(int)));
        connect(ui->cboTensionFront2Back,               SIGNAL(currentIndexChanged(int)),    this, SLOT(comboboxChanged(int)));
        connect(ui->cboTensionLeft2Right,               SIGNAL(currentIndexChanged(int)),    this, SLOT(comboboxChanged(int)));

        connect(ui->chk_displayFilmWidth,               SIGNAL(stateChanged(int)),           this, SLOT(checkboxChanged(int)));
        connect(ui->chk_displayFilmTension,             SIGNAL(stateChanged(int)),           this, SLOT(checkboxChanged(int)));

        connect(ui->calibrateWrapperButton,             SIGNAL(clicked(bool)),                     SLOT(calibrateWrapper()));

        // Tab 5.6 - Sealer Belt (Prepack)
        connect(ui->chkBarcodeUseFirstItem,             SIGNAL(stateChanged (int)),          this, SLOT(checkboxChangedAccumulate(int)));
        sundayOnTime_->installEventFilter( this );
        mondayOnTime_->installEventFilter( this );
        tuesdayOnTime_->installEventFilter( this );
        wednesdayOnTime_->installEventFilter( this );
        thursdayOnTime_->installEventFilter( this );
        fridayOnTime_->installEventFilter( this );
        saturdayOnTime_->installEventFilter( this );
        sundayOffTime_->installEventFilter( this );
        mondayOffTime_->installEventFilter( this );
        tuesdayOffTime_->installEventFilter( this );
        wednesdayOffTime_->installEventFilter( this );
        thursdayOffTime_->installEventFilter( this );
        fridayOffTime_->installEventFilter( this );
        saturdayOffTime_->installEventFilter( this );

        offsetMap_[sundayOnTime_] = 0;
        offsetMap_[mondayOnTime_] = 1;
        offsetMap_[tuesdayOnTime_] = 2;
        offsetMap_[wednesdayOnTime_] = 3;
        offsetMap_[thursdayOnTime_] = 4;
        offsetMap_[fridayOnTime_] = 5;
        offsetMap_[saturdayOnTime_] = 6;
        offsetMap_[sundayOffTime_] = 7;
        offsetMap_[mondayOffTime_] = 8;
        offsetMap_[tuesdayOffTime_] = 9;
        offsetMap_[wednesdayOffTime_] = 10;
        offsetMap_[thursdayOffTime_] = 11;
        offsetMap_[fridayOffTime_] = 12;
        offsetMap_[saturdayOffTime_] = 13;
    }

    // Click any Tab
    connect(ui->tabNormalScaleOperation,            SIGNAL(currentChanged(int)),         this, SLOT(currentChangeTab(int)));
    connect(ui->tabSubFeatures,                     SIGNAL(currentChanged(int)),         this, SLOT(currentChangeSubTab(int)));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::setupSelfServePlaylists()
{
    LOG_CLASS_INFO( "SuperNormalScaleOperation", "setupSelfServePlaylists" );

    // for remote session we need to sync playlist folder
    if ( !mainWindow_->isPrimarySessionType() )
    {
        LOG_TRACE( "sync self serve playlist folder...", NULL );

            if ( !filesystem::syncFolder( folderType::Playlists ) )
                LOG_WARN( "could not sync self serve playlist folder!", NULL );
    }

    // populate combo with playlist files
    QDir dir( filesystem::folder( folderType::Playlists ) );

    QStringList filters;
    filters << "*.txt";

    dir.setFilter( QDir::Files );
    dir.setNameFilters( filters );

    QStringList files( dir.entryList() );

    for( QStringList::ConstIterator file = files.begin(); file != files.end(); ++file )
    {
        QString filename( *file );

        LOG_DEBUG( "found self serve playlist '%s'", qPrintable( filename ) );
        ui->cboSelfServePlaylist->addItem( filename, filename );
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::editPlaylistFile()
{
    QString filename( (ui->cboSelfServePlaylist->itemData( ( ui->cboSelfServePlaylist->currentIndex() ) ) ).toString() );

    if ( filename.length() )
    {
        filename = filesystem::absoluteFilePath( filename, folderType::Playlists );

        if ( mainWindow_->switchToModeScreen<SupervisorMode, superTextFileEditor>() )
        {
            session_->scaleConfig->returnHomeLocation = OriginalScreen::TEXT_EDITOR_SELF_SERV_SCREEN;

            superTextFileEditor *editor = dynamic_cast<superTextFileEditor*>( mode_->screen() );

            if ( editor )
                editor->loadFile( filename );
        }
    }
}


void SuperNormalScaleOperation::initializeMaxTare(QString incomingMaxTare)
{
    Q_UNUSED( incomingMaxTare );
}

void SuperNormalScaleOperation::textPingHoursChanged(QString hours)
{
    if(hours.toInt() == 0)
    {
        pingMinutes_->setBottom(1);
        if(ui->txtPingMinutes->text() == "0" || ui->txtPingMinutes->text() == "00")
        {
             ui->txtPingMinutes->setText("1");
        }
    }
    else
    {
        pingMinutes_->setBottom(0);
    }
}

void SuperNormalScaleOperation::setupWidgets()
{
    isUpdating = true;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // TAB 0 - Product
    ///////////////////////////////////////////////////////////////////////////////////////////////

    ui->cboExternalWeightPrecision->addItem("2","2");
    ui->cboExternalWeightPrecision->addItem("3","3");

    ui->txtTareSetback->setEnabled( false );                        // disabled until 1,2 comes in and we know we have a weigher
    ui->txtMaxTare->setEnabled( false );                            // disabled until 1,2 comes in and we know we have a weigher

    // FT-134 Fixed Weight Units should not be available on a Metric Scale.
    if( isMetric_ == false)
    {
        ui->cboFixedWeightUnits->setVisible( true );
        ui->lblFixedWeightUnits->setVisible( true );
        ui->cboFixedWeightUnits->addItem( tr("Whole Oz"),"0");
        ui->cboFixedWeightUnits->addItem( tr("1/10th Oz"),"1");
    }
    else
    {
        ui->cboFixedWeightUnits->setVisible( false );
        ui->lblFixedWeightUnits->setVisible( false );
    }
    QString cboDaysVal = "";
    for(int j = 1; j <= 15; j++)
    {
        cboDaysVal = QString::number(j);
        ui->cboDaysToKeepTransactions->addItem(cboDaysVal, cboDaysVal);
    }

    QString cboDigitsVal = "";
    // FT-132 The Number of Lookup digits drop down bar should not have the option to select either "1", or a, "2" 
    for(int j = 3; j <= 6; j++)
    {
        cboDigitsVal = QString::number(j);
        ui->cboNumOfDigits->addItem(cboDigitsVal, cboDigitsVal);
    }

    if ( !session_->isPrepack() )
    {
        ui->cboSpAutoLookupDigit->addItem( tr( "None" ), 0 );
        ui->cboSpAutoLookupDigit->addItem( "1", 1 );
        ui->cboSpAutoLookupDigit->addItem( "2", 2 );
        ui->cboSpAutoLookupDigit->addItem( "3", 3 );
        ui->cboSpAutoLookupDigit->addItem( "4", 4 );
        ui->cboSpAutoLookupDigit->addItem( "5", 5 );
        ui->cboSpAutoLookupDigit->addItem( "6", 6 );
        ui->cboSpAutoLookupDigit->addItem( "7", 7 );
        ui->cboSpAutoLookupDigit->addItem( "8", 8 );
        ui->cboSpAutoLookupDigit->addItem( "9", 9 );
    }

    //ui->cboBarcodeTypeOver99->addItem( "UPCA", 0 );           // too small
    //ui->cboBarcodeTypeOver99->addItem( "UPCD3", 1 );          // not supported
    //ui->cboBarcodeTypeOver99->addItem( "UPCE", 2 );           // not supoprted
    ui->cboBarcodeTypeOver99->addItem( "UPCE", 3 );             // default
    ui->cboBarcodeTypeOver99->addItem( "EAN-128", 4);           // now known as GS1-128
    ui->cboBarcodeTypeOver99->addItem( "GS1 GTIN-14", 5 );      // These are GTINs where are enums 5,6,7,8
    ui->cboBarcodeTypeOver99->addItem( "GS1 GTIN-13", 6 );      // These are GTINs where are enums 5,6,7,8
    //ui->cboBarcodeTypeOver99->addItem( "GS1 GTIN-12", 7 );    // These are GTINs but are too small
    //ui->cboBarcodeTypeOver99->addItem( "GS1 GTIN-8", 8 );     // These are GTINs but are too small
    ui->cboBarcodeTypeOver99->addItem( "QR Code M", 9);         // QR
    ui->cboBarcodeTypeOver99->addItem( "QR Code L", 10);        // QR
    ui->cboBarcodeTypeOver99->addItem( "QR Code Q", 11);        // QR
    ui->cboBarcodeTypeOver99->addItem( "QR Code H", 12);        // QR
    ui->cboBarcodeTypeOver99->addItem( "Data Matrix", 13);      // Data Matrix
    ui->cboBarcodeTypeOver99->addItem( "GS1 GTIN-14 (One-Line)", 14 );  // GTIN-14 from above (5), but forced to fit on one line

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // TAB 1 - Scale Access
    ///////////////////////////////////////////////////////////////////////////////////////////////

    ui->cboSuggSellStyle->addItem("None", "0");
    ui->cboSuggSellStyle->addItem("Okay", "1");
    ui->cboSuggSellStyle->addItem("Yes\\No", "2");

    ui->cboRightPanelMode->addItem( tr("None"), "0" );
    ui->cboRightPanelMode->addItem( tr("Wrapper Settings"), "1" );
    ui->cboRightPanelMode->addItem( tr("Label Placement"), "2" );

    ui->cboBottomPanelMode->addItem( tr("Build a Meal"), "1");
    ui->cboBottomPanelMode->addItem( tr("Cut Test Option One"), "2");

    ui->cboRemoteAccessLoginMethod->addItem("None", "0");
    ui->cboRemoteAccessLoginMethod->addItem("Operator ID", "1");
    ui->cboRemoteAccessLoginMethod->addItem("Username\\Password", "2");

    changeRemoteAccessLoginMethod(0);

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // TAB 2 - System
    ///////////////////////////////////////////////////////////////////////////////////////////////

    ui->cboLanguageCode->addItem(tr("English (US)"), "enUS");       //QString data = cboLanguageCode->itemData(0).toString();
    ui->cboLanguageCode->addItem(tr("Spanish (Mexico)"), "esMX");
    ui->cboLanguageCode->addItem(tr("English (Canada)"), "enCA");
    ui->cboLanguageCode->addItem(tr("French (Canada)"), "frCA");

    ui->cboSecLanguageCode->addItem(tr("None"), "none");
    ui->cboSecLanguageCode->addItem(tr("English (US)"), "enUS");       //QString data = cboLanguageCode->itemData(0).toString();
    ui->cboSecLanguageCode->addItem(tr("Spanish (Mexico)"), "esMX");
    ui->cboSecLanguageCode->addItem(tr("English (Canada)"), "enCA");
    ui->cboSecLanguageCode->addItem(tr("French (Canada)"), "frCA");

    ui->cboDateStyle->addItem(tr("FE 28,13"), "0");
    ui->cboDateStyle->addItem(tr("Feb 28,13"), "1");
    ui->cboDateStyle->addItem(tr("13/02/28"), "2");
    ui->cboDateStyle->addItem(tr("02/28/13"), "3");
    ui->cboDateStyle->addItem(tr("28/Feb/13"), "5");
    ui->cboDateStyle->addItem(tr("28/02/13"), "6");
    ui->cboDateStyle->addItem(tr("13 FE 28"), "7");
    ui->cboDateStyle->addItem(tr("28 FEB 13"), "8");

    ui->cboTimeFormat->addItem(tr("12 Hour"), "0");
    ui->cboTimeFormat->addItem(tr("24 Hour"), "1");

    ui->cboPopupKeypadStyle->addItem(tr("Telephone"), "1");
    ui->cboPopupKeypadStyle->addItem(tr("PC Keyboard"), "2");

    ui->cboOpLoginKeyboardStyle->addItem(tr("QWERTY Keyboard"), "1");
    ui->cboOpLoginKeyboardStyle->addItem(tr("Numeric Keypad"), "2");

    ui->cboStartScreenHome->addItem(tr("Left Panel"), "0");
    ui->cboStartScreenHome->addItem(tr("Center Panel"), "1");
    ui->cboStartScreenHome->addItem(tr("Meal Builder"), "2");

    ui->cboByCountCalculationMethod->addItem("1", "1");
    ui->cboByCountCalculationMethod->addItem("2", "2");
    ui->cboByCountCalculationMethod->addItem("3", "3");

    ui->cboAlwaysEnableVirtualKeyboard->addItem(tr("Never Enable", "0"));
    ui->cboAlwaysEnableVirtualKeyboard->addItem(tr("Always Enable", "1"));

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Tab 4 - Panels
    ///////////////////////////////////////////////////////////////////////////////////////////////

    ui->cboAccessBrowserMode->addItem( tr("Flashkeys "), QString::number( hobart::dynamicContentWidget::Flashkeys ) );
    ui->cboAccessBrowserMode->addItem( tr("Best Sellers "), QString::number( hobart::dynamicContentWidget::BestSellerList ) );
    ui->cboAccessBrowserMode->addItem( tr("Web Control "), QString::number( hobart::dynamicContentWidget::WebBrowser ) );
    ui->cboAccessBrowserMode->addItem( tr("Fixed Flashkeys"), QString::number( hobart::dynamicContentWidget::FixedFlashkeys ) );

    ui->cboRunScreen->addItem( tr("Flashkeys "), QString::number( hobart::dynamicContentWidget::Flashkeys ) );
    ui->cboRunScreen->addItem( tr("Best Sellers"), QString::number( hobart::dynamicContentWidget::BestSellerList ) );
    ui->cboRunScreen->addItem( tr("Web Browser"), QString::number( hobart::dynamicContentWidget::WebBrowser ) );
    ui->cboRunScreen->addItem( tr("Fixed Flashkeys"), QString::number( hobart::dynamicContentWidget::FixedFlashkeys ) );

    ui->cboNumberPad->addItem( tr("Number Pad"), QString::number( hobart::dynamicContentWidget::NumberPad ) );
    ui->cboNumberPad->addItem( tr("Flashkeys "), QString::number( hobart::dynamicContentWidget::Flashkeys ) );
    ui->cboNumberPad->addItem( tr("Product Image"), QString::number( hobart::dynamicContentWidget::ProductImage ) );

    // FT-1779 - removed the check for "if ( session_->isPrepack() )" before the following line
    ui->cboNumberPad->addItem( tr("Run Totals"), QString::number( hobart::dynamicContentWidget::RunTotals ) );
	 
    //ui->cboNumberPad->addItem( tr("Best Sellers"), QString::number( hobart::dynamicContentWidget::BestSellerList ) ); // FT-669

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Tab 5.0 - Flashkeys
    ///////////////////////////////////////////////////////////////////////////////////////////////

    ui->cboDefaultSearchMethod->addItem(tr("By description"), "0");
    ui->cboDefaultSearchMethod->addItem(tr("By plu number"), "1");

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Tab 5.3 - Now Serving
    ///////////////////////////////////////////////////////////////////////////////////////////////
    ui->cboNowServingType->addItem(tr("None"), "0");
    ui->cboNowServingType->addItem(tr("Electronic"), "1");
    ui->cboNowServingType->addItem(tr("Pre-printed Dispenser"), "2");

	if ( session_->isPrepack() )
	{
	    ///////////////////////////////////////////////////////////////////////////////////////////////
	    // Tab 5.5 - Wrapper Settings
	    ///////////////////////////////////////////////////////////////////////////////////////////////

		ui->cboIntakeMode->clear();
	    ui->cboIntakeMode->addItem(tr("Gentle"), "1");
	    ui->cboIntakeMode->addItem(tr("Normal"), "2");

		ui->cboLabelPlacement->clear();
        ui->cboLabelPlacement->addItem(tr("Upper Right"), "0");
        ui->cboLabelPlacement->addItem(tr("Upper Left"), "1");
        //ui->cboLabelPlacement->addItem(tr("Lower Right"), "3");
        //ui->cboLabelPlacement->addItem(tr("Lower Left"), "4");

        populateSecondaryPositions();
/*
        ui->cboLabelPlacement_2->clear();
        ui->cboLabelPlacement_2->addItem(tr("Upper Right"), "0");
        ui->cboLabelPlacement_2->addItem(tr("Upper Left"), "1");
        ui->cboLabelPlacement_2->addItem(tr("Lower Right"), "3");
        ui->cboLabelPlacement_2->addItem(tr("Lower Left"), "4");
        ui->cboLabelPlacement_2->addItem(tr("Right of Primary"), "5");
        ui->cboLabelPlacement_2->addItem(tr("Left of Primary"), "6");
        ui->cboLabelPlacement_2->addItem(tr("Above Primary"), "7");
        ui->cboLabelPlacement_2->addItem(tr("Below Primary"), "8");
*/
		ui->cboFilmTension->clear();
	    ui->cboFilmTension->addItem(tr("Low"), "0");
	    ui->cboFilmTension->addItem(tr("Medium"), "1");

		ui->cboTensionFront2Back->clear();
	    ui->cboTensionFront2Back->addItem("-5", "-5");
	    ui->cboTensionFront2Back->addItem("-4", "-4");
	    ui->cboTensionFront2Back->addItem("-3", "-3");
	    ui->cboTensionFront2Back->addItem("-2", "-2");
	    ui->cboTensionFront2Back->addItem("-1", "-1");
	    ui->cboTensionFront2Back->addItem("0", "0");
	    ui->cboTensionFront2Back->addItem("+1", "1");
	    ui->cboTensionFront2Back->addItem("+2", "2");
	    ui->cboTensionFront2Back->addItem("+3", "3");
	    ui->cboTensionFront2Back->addItem("+4", "4");
	    ui->cboTensionFront2Back->addItem("+5", "5");

		ui->cboTensionLeft2Right->clear();
	    ui->cboTensionLeft2Right->addItem("-5", "-5");
	    ui->cboTensionLeft2Right->addItem("-4", "-4");
	    ui->cboTensionLeft2Right->addItem("-3", "-3");
	    ui->cboTensionLeft2Right->addItem("-2", "-2");
	    ui->cboTensionLeft2Right->addItem("-1", "-1");
	    ui->cboTensionLeft2Right->addItem("0", "0");
	    ui->cboTensionLeft2Right->addItem("+1", "1");
	    ui->cboTensionLeft2Right->addItem("+2", "2");
	    ui->cboTensionLeft2Right->addItem("+3", "3");
	    ui->cboTensionLeft2Right->addItem("+4", "4");
	    ui->cboTensionLeft2Right->addItem("+5", "5");
	}

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Tab 5.6 - Self Serve
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Set up values for how to print a label in self service

    if( session_->uiUtilities->scaleName().contains("FS") )
        ui->chkEnableGen2UI->setVisible( true );
    else
        ui->chkEnableGen2UI->setVisible( false );

//#ifdef QT_DEBUG
//    ui->chkEnableGen2UI->setVisible( true );
//#endif

    // where to start options
    ui->cboStartWhere->addItem(tr("None (default)"), 0);                                // dont even have to start on start screen at all
    ui->cboStartWhere->addItem(tr("Green GO button"), 1);                               // begin users on the Start Panel
    ui->cboStartWhere->addItem(tr("Weight required to select item"), 2);                // weight required to select item  ( NEW option: clickable? )
    ui->cboStartWhere->addItem(tr("Use Playlists"), 3);                                 // enable playlists
    ui->cboStartWhere->addItem(tr("Log into Self Service"), 4);                         // forced login

    // who enters items options
    ui->cboItemEntryWho->addItem(tr("Product Flashkeys"), 0);
    ui->cboItemEntryWho->addItem(tr("Best Seller Flashkeys"), 1);
    ui->cboItemEntryWho->addItem(tr("Product Flashkeys with Keypad button"), 2);        // BigY request to include a Keypad button
    ui->cboItemEntryWho->addItem(tr("Fixed Flashkeys"), 3);                             // PJR-383: Fixed Flashkeys
    ui->cboItemEntryWho->addItem(tr("Keypad only"), 4);

    // how to search options
    ui->cboSearchHow->addItem(tr("A-Z categories with Best Sellers"), 0);               // search A-Z,
    ui->cboSearchHow->addItem(tr("Character by character"), 1);                         // search char by char
    ui->cboSearchHow->addItem(tr("None (hide Search button)"), 2);                      // no seach for you

    // What to print options
    ui->cboPrintWhat->addItem( tr("Print label"), 0 );                              // regular default print option
    ui->cboPrintWhat->addItem( tr("Display label on screen"), 1 );                  // BigY & chkselfServeDisplayLabelOnScreenGO (Amazon Go - [X] Print label to screen)
    ui->cboPrintWhat->addItem( tr("Display label on screen and print"), 2 );        // BigY
    ui->cboPrintWhat->addItem( tr("Display barcode on screen and print"), 3 );      // BigY and Hannaford via Delhize
    if( session_->scaleConfig->getScaleModelName().contains( "SSLSC" ) )
        ui->cboPrintWhat->addItem( tr("Display QR code only"), 4 );                 // Walmart (PJR413/SOF-4772)

    ui->cboPrintingWhen->addItem( tr("Take-label video"), 0 );                            // default
    ui->cboPrintingWhen->addItem( tr("Take-label image"), 1 );                         // weight required

    ui->chkStartScreenClickable->setVisible( false );
    ui->chkSelfServePrintOnStableWeight->setVisible( false );
    ui->chkIncludeNumpad->setVisible( false );
    ui->cboSelfServePlaylist->setEnabled( false );
    ui->butEditPlaylist->setEnabled( false );
    ui->chkSelfServeDisplayPluLabel->setVisible( false );

    // Fresh Serve Setup

    ui->cboFlashkeyType_0->addItem( tr("Softkey Group"), 0 );
    ui->cboFlashkeyType_0->addItem( tr("Best Sellers"), 1 );
    ui->cboFlashkeyType_1->addItem( tr("Softkey Group"), 0 );
    ui->cboFlashkeyType_1->addItem( tr("Best Sellers"), 1 );
    ui->cboFlashkeyType_2->addItem( tr("Softkey Group"), 0 );
    ui->cboFlashkeyType_2->addItem( tr("Best Sellers"), 1 );
    ui->cboFlashkeyType_3->addItem( tr("Softkey Group"), 0 );
    ui->cboFlashkeyType_3->addItem( tr("Best Sellers"), 1 );
    ui->cboFlashkeyType_4->addItem( tr("Softkey Group"), 0 );
    ui->cboFlashkeyType_4->addItem( tr("Best Sellers"), 1 );


    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Tab 5.7 - Cutting Test
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /* Set up the values for the cutting test type combo box */
    ui->cboCuttingTestType->addItem( tr("None"), "0");
    ui->cboCuttingTestType->addItem( tr("Default"), "1");
    ui->cboCuttingTestType->addItem( tr("Option 1"), "2");

    isUpdating = false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::setupValidators()
{
	LOG_CLASS_INFO( "SuperNormalScaleOperation", "setupValidators" );

    QIntValidator *ipAddressValidator( new QIntValidator( 0, 255, this ) );
    QIntValidator *suggSellFrequencyValidator( new QIntValidator( 1, 99, this ) );

    // NUMERIC VALIDATORS
    // Removed compress time fields 05-15-2015 during timesys 1.15 release
    //ui->txtCompressTimeHours->setValidator(compressHours_ = new QIntValidator(0, INT_MAX, this) );
    //ui->txtCompressTimeHours->setInputLeadingZeros(true);
    //ui->txtCompressTimeHours->setInputWrapping(true);
    //ui->txtCompressTimeHours->setDecimalPlaces(0);
    //compressHours_->setTop(12);
    //compressHours_->setBottom(1);

    //ui->txtCompressTimeMinutes->setValidator(compressMinutes_ = new QIntValidator(0, INT_MAX, this) );
    //ui->txtCompressTimeMinutes->setInputLeadingZeros(true);
    //ui->txtCompressTimeMinutes->setInputWrapping(true);
    //ui->txtCompressTimeMinutes->setDecimalPlaces(0);
    //compressMinutes_->setTop(59);
    //compressMinutes_->setBottom(0);

    // FT1699 - moved to setupWeigherFields() and updateWeigherFields(). Cant do here because we need the divisionSize from 1,2
    //ui->txtMaxTare->setValidator(maxTare_ = new QIntValidator(0, INT_MAX, this));
    //ui->txtMaxTare->setDecimalPlaces( 3 );
    //ui->txtMaxTare->setDecimal( currencyData_->unitSeparator() );
    //ui->txtMaxTare->setInputWrapping(true);
    //maxTare_->setTop( (2 == ui->txtMaxTare->decimalPlaces()) ? 999 : 9995 ); // FT-1496

    int precision = scaleConfigData_->tarePrecision();

    ui->txtTareSetback->setValidator(tareSetback_ = new QIntValidator(0, INT_MAX, this));
    ui->txtTareSetback->setDecimalPlaces( precision );
    ui->txtTareSetback->setDecimal( currencyData_->unitSeparator() );
    ui->txtTareSetback->setInputWrapping(true);
	LOG_TRACE( "tareSetback_->setTop(makeValidatorTop(%d)", precision+1 );
    tareSetback_->setTop(makeValidatorTop(precision+1));

    ui->txtMinSweetheartWeight->setValidator(minSweetheartWeight_ = new QIntValidator(0, INT_MAX, this) );
    ui->txtMinSweetheartWeight->setDecimalPlaces( precision );
    ui->txtMinSweetheartWeight->setDecimal( currencyData_->unitSeparator() );
    ui->txtMinSweetheartWeight->setInputWrapping(true);
    minSweetheartWeight_->setTop( (2 == precision) ? 999 : 9995 );
    minSweetheartWeight_->setBottom(0);

    ui->txtMaxTare->setValidator(maxTare_ = new QIntValidator(0, INT_MAX, this));
    ui->txtMaxTare->setDecimalPlaces( precision );
    ui->txtMaxTare->setDecimal( currencyData_->unitSeparator() );
    ui->txtMaxTare->setInputWrapping(true);

    /////////////////////////////////////
    // HTA-59 Default Settings for weigher tares
    // the default tares are:
    //

    /* Default DB Values:
        tare setback: 1
        Max Tare: 999
        platter tare enabled: 0
        change zero tare only once: 0
        change forced tare only once: 0
        change forced tare only once: 0
        external weight precision: 2
        tare entry precision: 2
        user rezero weight limit 10
        weigher wakeup sensor attached: 1


    */

    // AWS REV 2 DR
       // Errored Condition
          // Weight Precision 2
          // Tare entry precision 2
          // Max tare 9.99

       // Correct Condition
          // Tare precision 3
          // Max tare 9.995

    // AWS REV 2
       // Errored Condition
          // Weight precision 2
          // Tare precision disabled (greyed out)
          // Max tare 99.90

       // Correct Condition
          // Max tare 9.99

    /////////////////////////////////////

    // Keeps seeting this back to 9995 as top
    maxTare_->setTop( (2 == ui->txtMaxTare->decimalPlaces()) ? 999 : 9995 ); // FT-1496, HW-75


    ui->txtSuggSellFreq->setValidator( suggSellFrequencyValidator );
    ui->txtSuggSellFreq->setDecimalPlaces(0);
    ui->txtSuggSellFreq->setInputWrapping(true);

    ui->txtLogoutRemoteIdleTime->setValidator(logoutIdleRemoteTime_ = new QIntValidator(0, INT_MAX, this) );
    ui->txtLogoutRemoteIdleTime->setDecimalPlaces(0);
    ui->txtLogoutRemoteIdleTime->setInputWrapping(true);
    logoutIdleRemoteTime_->setTop(120);

    ui->txtNumberOfCharacters->setValidator(numOfCharsOnFlashkey_ = new QIntValidator(0, INT_MAX, this) );
    ui->txtNumberOfCharacters->setDecimalPlaces(0);
    ui->txtNumberOfCharacters->setInputWrapping(true);
    numOfCharsOnFlashkey_->setTop(99);

    ui->txtDefaultFlashkeyFontPointsize->setValidator(defaultFlashkeyFontPointsize_ = new QIntValidator(0, INT_MAX, this) );
    ui->txtDefaultFlashkeyFontPointsize->setDecimalPlaces(0);
    ui->txtDefaultFlashkeyFontPointsize->setInputWrapping(true);
    defaultFlashkeyFontPointsize_->setTop(99);

    ui->txtResetStatisticsDays->setValidator(resetStatsDays_ = new QIntValidator(0, INT_MAX, this) );
    ui->txtResetStatisticsDays->setDecimalPlaces(0);
    ui->txtResetStatisticsDays->setInputWrapping(true);
    resetStatsDays_->setTop(99);

    ui->txtPingTarget1->setValidator(ipAddressValidator);
    ui->txtPingTarget1->setDecimalPlaces(0);
    ui->txtPingTarget1->setInputWrapping(true);

    ui->txtPingTarget2->setValidator(ipAddressValidator);
    ui->txtPingTarget2->setDecimalPlaces(0);
    ui->txtPingTarget2->setInputWrapping(true);

    ui->txtPingTarget3->setValidator(ipAddressValidator);
    ui->txtPingTarget3->setDecimalPlaces(0);
    ui->txtPingTarget3->setInputWrapping(true);

    ui->txtPingTarget4->setValidator(ipAddressValidator);
    ui->txtPingTarget4->setDecimalPlaces(0);
    ui->txtPingTarget4->setInputWrapping(true);

    ui->txtPingHours->setValidator(pingHours_ = new QIntValidator(0, INT_MAX, this) );
    ui->txtPingHours->setDecimalPlaces(0);
    ui->txtPingHours->setInputWrapping(true);
    pingHours_->setTop(23);

    //ui->txtPingMinutes->setValidator(pingMinutes_ = new QIntValidator(0, INT_MAX, this) );
    //ui->txtPingMinutes->setDecimalPlaces(0);
    //ui->txtPingMinutes->setInputWrapping(true);
    //pingMinutes_->setTop(59);

    //
    ui->txtNowServing1->setValidator(ipAddressValidator);
    ui->txtNowServing1->setDecimalPlaces(0);
    ui->txtNowServing1->setInputWrapping(true);

    ui->txtNowServing2->setValidator(ipAddressValidator);
    ui->txtNowServing2->setDecimalPlaces(0);
    ui->txtNowServing2->setInputWrapping(true);

    ui->txtNowServing3->setValidator(ipAddressValidator);
    ui->txtNowServing3->setDecimalPlaces(0);
    ui->txtNowServing3->setInputWrapping(true);

    ui->txtNowServing4->setValidator(ipAddressValidator);
    ui->txtNowServing4->setDecimalPlaces(0);
    ui->txtNowServing4->setInputWrapping(true);

    ui->txtAccumItemsScaleId->setValidator(accumItemsScaleId_ = new QIntValidator(0, INT_MAX, this) );
    ui->txtAccumItemsScaleId->setDecimalPlaces(0);
    ui->txtAccumItemsScaleId->setInputWrapping(true);
    accumItemsScaleId_->setTop(99);

    ui->txtAccumItemsTransNum->setValidator(accumItemsTransNum_ = new QIntValidator(0, INT_MAX, this) );
    ui->txtAccumItemsTransNum->setDecimalPlaces(0);
    ui->txtAccumItemsTransNum->setInputWrapping(true);
    accumItemsTransNum_->setTop(999999);

    //ui->XXXtxtAccumItemsEANCode->setValidator(XXXaccumItemsEanCode_ = new QIntValidator(0, INT_MAX, this) );
    //ui->XXXtxtAccumItemsEANCode->setDecimalPlaces(0);
    //ui->XXXtxtAccumItemsEANCode->setInputWrapping(true);
    //ui->XXXtxtAccumItemsEANCode->setInputLeadingZeros( true );
    //XXXaccumItemsEanCode_->setTop(999999);

    QRegExp rx( QString("[0-9]*") );
    accumItemsEanCode_ = new QRegExpValidator( rx, this );
    ui->txtAccumItemsEANCode->setValidator(accumItemsEanCode_);

    ui->txtAccumItemsEANPrefix->setValidator(accumItemsEanPrefix_ = new QIntValidator(0, INT_MAX, this) );
    ui->txtAccumItemsEANPrefix->setDecimalPlaces(0);
    ui->txtAccumItemsEANPrefix->setInputWrapping(true);
    accumItemsEanPrefix_->setTop(99);

    ui->txtPeriodicOfflineBeep->setValidator(periodicOfflineBeep_ = new QIntValidator(0, INT_MAX, this) );
    ui->txtPeriodicOfflineBeep->setDecimalPlaces(0);
    ui->txtPeriodicOfflineBeep->setInputWrapping(true);
    periodicOfflineBeep_->setTop(120);

    //////
    ui->txtMaxSuspendedTests->setValidator(maxSuspendTests_ = new QIntValidator(0, INT_MAX, this) );
    ui->txtMaxSuspendedTests->setDecimalPlaces(0);
    ui->txtMaxSuspendedTests->setInputWrapping(true);
    maxSuspendTests_->setTop(99);

    ui->txtMaxSuspendedTestsPerOperator->setValidator(maxSuspendTestsPerOper_ = new QIntValidator(0, INT_MAX, this) );
    ui->txtMaxSuspendedTestsPerOperator->setDecimalPlaces(0);
    ui->txtMaxSuspendedTestsPerOperator->setInputWrapping(true);
    maxSuspendTestsPerOper_->setTop(99);

    ui->txtInactivitylogout->setValidator(inactivityLogout_ = new QIntValidator(0, INT_MAX, this) );
    ui->txtInactivitylogout->setDecimalPlaces(0);
    ui->txtInactivitylogout->setInputWrapping(true);
    inactivityLogout_->setTop(120);

    // HT-365
    ui->leShelfLifeHour->setValidator(shelfLifeHoursValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->leShelfLifeHour->setInputWrapping(true);
    ui->leShelfLifeHour->setInputLeadingZeros(true);
    shelfLifeHoursValidator_->setTop( 12 );
    shelfLifeHoursValidator_->setBottom( 0 ); // Allow 00:00 to be entered to indicate disabled

    // HT-365
    ui->leShelfLifeMinutes->setValidator(shelfLifeMinutesValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->leShelfLifeMinutes->setInputLeadingZeros(true);
    ui->leShelfLifeMinutes->setInputWrapping(true);
    shelfLifeMinutesValidator_->setTop( 59 );
    shelfLifeMinutesValidator_->setBottom( 0 );

    ui->txtFlashkeyNum_0->setValidator(flashkeyNumValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->txtFlashkeyNum_0->setDecimalPlaces(0);
    ui->txtFlashkeyNum_0->setInputWrapping(true);
    ui->txtFlashkeyNum_0->setInputTruncation(true);
    flashkeyNumValidator_->setTop(makeValidatorTop(6));

    ui->txtFlashkeyNum_1->setValidator(flashkeyNumValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->txtFlashkeyNum_1->setDecimalPlaces(0);
    ui->txtFlashkeyNum_1->setInputWrapping(true);
    ui->txtFlashkeyNum_1->setInputTruncation(true);
    flashkeyNumValidator_->setTop(makeValidatorTop(6));

    ui->txtFlashkeyNum_2->setValidator(flashkeyNumValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->txtFlashkeyNum_2->setDecimalPlaces(0);
    ui->txtFlashkeyNum_2->setInputWrapping(true);
    ui->txtFlashkeyNum_2->setInputTruncation(true);
    flashkeyNumValidator_->setTop(makeValidatorTop(6));

    ui->txtFlashkeyNum_3->setValidator(flashkeyNumValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->txtFlashkeyNum_3->setDecimalPlaces(0);
    ui->txtFlashkeyNum_3->setInputWrapping(true);
    ui->txtFlashkeyNum_3->setInputTruncation(true);
    flashkeyNumValidator_->setTop(makeValidatorTop(6));

    ui->txtFlashkeyNum_4->setValidator(flashkeyNumValidator_ = new QIntValidator(0, INT_MAX, this));
    ui->txtFlashkeyNum_4->setDecimalPlaces(0);
    ui->txtFlashkeyNum_4->setInputWrapping(true);
    ui->txtFlashkeyNum_4->setInputTruncation(true);
    flashkeyNumValidator_->setTop(makeValidatorTop(6));

    session_->send(ConfigurationDev, CfgScaleOperationRead);        // 112,70 -> get Normal Scale Operation Config
    session_->send(ConfigurationDev, CfgPrepackRead);               // 112,140 -> get Prepack Config (operator display fields)
    session_->send(ConfigurationDev, CfgAccumulateItemsRead);       // 112,240 -> get Accumulate Items Config
    session_->send(ConfigurationDev, CfgFrequentShopperRead);       // 112,40  -> get Frequent Shopper
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// SOF-43
void SuperNormalScaleOperation::setupHoursToViewRecordsValidator( int numDays )
{
    ui->txtTransHoursToView->setValidator( transHoursToView_ = new QIntValidator( 1, INT_MAX, this ) );
    ui->txtTransHoursToView->setDecimalPlaces( 0 );
    ui->txtTransHoursToView->setInputWrapping( true );

    transHoursToView_->setBottom( 1 );

    int maxHours = numDays * 24;
    transHoursToView_->setTop( maxHours );

    // Reset hours to max for the # of days
    if( ui->txtTransHoursToView->toInt() > maxHours )
        ui->txtTransHoursToView->setText( QString::number( maxHours ) );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::saveDataOnExit()
{
    LOG_CLASS_INFO( "SuperNormalScaleOperation", "saveDataOnExit" );

    LOG_DEBUG( "Top of saveDataOnExit", NULL );   

    if(!dataHasBeenInitialized)
        return;                                                     // FT-1091: if we havent processed initial message, no save occurs

    if( dateTimeHasChanged || langHasChanged )
        saveDateTimeFormat();

    /**
     * Save and Send All Configuration Data
     */

    QString msg = normalScaleOperation_->collectTransactionsRaw() + "," +
    normalScaleOperation_->daysToKeepTransactions() + "," +
    normalScaleOperation_->coolTrackingNumRaw() + "," +
    normalScaleOperation_->useAutoLookupRaw() + "," +
    normalScaleOperation_->numOfDigits() + "," +
    normalScaleOperation_->soundBeepOnStableWtRaw() + "," +
    normalScaleOperation_->autoDisplayProductNotesRaw() + "," +
    normalScaleOperation_->accessBrowserModeRaw() + "," +
    normalScaleOperation_->autoLoadOldFirmwareRaw() + "," +
    normalScaleOperation_->uniqueNumbers() + "," +
    normalScaleOperation_->popupKeypad() + "," +
    normalScaleOperation_->popupKeypadStyleRaw() + "," +
    normalScaleOperation_->opLoginKeyboardStyleRaw() + "," +
    normalScaleOperation_->enablePowerSaver() + "," +
    normalScaleOperation_->powerSaveIdleTime() + "," +
    normalScaleOperation_->logoutRemoteIdleTime() + "," +
    "\"" + normalScaleOperation_->defaultHomePage() + "\"," +
    normalScaleOperation_->defaultProductRecord() + "," +
    normalScaleOperation_->fixedWeightUnits() + ",";

    if( maxTareHasBeenUpdated  && ui->txtMaxTare->isEnabled())
        msg += "\"" + normalScaleOperation_->maxTare() + "\"" + ",";
    else
        msg += ",";

    if( ui->txtTareSetback->isEnabled())
        msg += "\"" + normalScaleOperation_->tareSetback() + "\"" + ",";
    else
        msg += ",";

    msg += normalScaleOperation_->beeperVolume() + "," +
    normalScaleOperation_->byCountCalculationMethodRaw() + "," +
    normalScaleOperation_->externalWeightPrecision() + "," +
    normalScaleOperation_->tareEntryPrecision() + "," +
    normalScaleOperation_->clearPluAfterPrintingRaw() + "," +
    normalScaleOperation_->operatorRezero() + "," +
    normalScaleOperation_->filmWidth() + "," +
    normalScaleOperation_->filmTension() + "," +
    normalScaleOperation_->tensionFrontToBack() + "," +
    normalScaleOperation_->tensionLeftToRight() + "," +
    normalScaleOperation_->intakeMode() + "," +
    normalScaleOperation_->labelPlacement() + "," +
    normalScaleOperation_->applierRotation() + "," +
    "\"" + normalScaleOperation_->timeOnSun() + "\"," +
    "\"" + normalScaleOperation_->timeOffSun() + "\"," +
    "\"" + normalScaleOperation_->timeOnMon() + "\"," +
    "\"" + normalScaleOperation_->timeOffMon() + "\"," +
    "\"" + normalScaleOperation_->timeOnTue() + "\"," +
    "\"" + normalScaleOperation_->timeOffTue() + "\"," +
    "\"" + normalScaleOperation_->timeOnWed() + "\"," +
    "\"" + normalScaleOperation_->timeOffWed() + "\"," +
    "\"" + normalScaleOperation_->timeOnThu() + "\"," +
    "\"" + normalScaleOperation_->timeOffThu() + "\"," +
    "\"" + normalScaleOperation_->timeOnFri() + "\"," +
    "\"" + normalScaleOperation_->timeOffFri() + "\"," +
    "\"" + normalScaleOperation_->timeOnSat() + "\"," +
    "\"" + normalScaleOperation_->timeOffSat() + "\"," +
    normalScaleOperation_->compressFlagRaw() + "," +
    normalScaleOperation_->compressTime() + "," +
    normalScaleOperation_->autoLookupOverrideDigit() + "," +
    normalScaleOperation_->checkNetConnectivity() + "," +
    "0"  + "," +                              // access desktop is always 0 - never allow it anymore
    normalScaleOperation_->pingPrimaryRaw() + "," +
    normalScaleOperation_->pingGatewayRaw() + "," +
    normalScaleOperation_->gcdLayoutType() + "," +
    normalScaleOperation_->gcdColorScheme() + "," +
    normalScaleOperation_->accessGcdResetRaw() + "," +
    normalScaleOperation_->showDeleteAllFilesRaw() + "," +
    normalScaleOperation_->showPrinterSettingsRaw() + "," +
    normalScaleOperation_->remoteAccessPermittedRaw() + "," +
    normalScaleOperation_->remoteAccessLoginMethodRaw() + "," +
    normalScaleOperation_->unitPriceSecurityLabel() + "," +
    normalScaleOperation_->totalPriceSecurityLabel() + "," +
    normalScaleOperation_->backdateByDays() + "," +
    normalScaleOperation_->pingTarget() + "," +
    normalScaleOperation_->pingTargetIPAddress() + "," +
    normalScaleOperation_->pingInterval() + "," +

    // TODO - Need to test if the new support works as expected
    normalScaleOperation_->autoDisplaySuggestiveSellingRaw() + "," +
    normalScaleOperation_->autoDisplayCookingNotesRaw() + "," +
    normalScaleOperation_->gcdIdlePlaybackTime() + "," +
    "\"" + normalScaleOperation_->remoteAccessUserName() + "\"," +
    "\"" + normalScaleOperation_->remoteAccessPassword() + "\"," +
    "\"" + normalScaleOperation_->systemFont() + "\"," +
    normalScaleOperation_->startScreenHome() + "," +
    normalScaleOperation_->allowAddressBarRaw() + "," +
    normalScaleOperation_->resetStatisticsDays() + "," +
    normalScaleOperation_->defaultSearchMethod() + "," +
    "\"" + normalScaleOperation_->runScreenHomePage() + "\"," +
    normalScaleOperation_->dcaStartScreenTypeRaw() + "," +
    normalScaleOperation_->dcaRunScreenTypeRaw() + "," +
    normalScaleOperation_->dcaKeypadAreaTypeRaw() + "," +
    normalScaleOperation_->trackingOperatorChangesRaw() + "," +
    normalScaleOperation_->enableDisplayNetworkDetailsRaw() + "," +
    normalScaleOperation_->disableOperatorFlashkeyEditorRaw() + "," +
    normalScaleOperation_->disableStartScreenRaw() + "," +
    normalScaleOperation_->rightScreenTypeRaw() + "," +
    normalScaleOperation_->showTotalTypeOnRunScreenRaw() + "," +
    normalScaleOperation_->nowServingQueueIPAddress() + "," +
    normalScaleOperation_->nowServingSysTypeRaw() + "," +
    normalScaleOperation_->displaySelfServeStartScreenRaw() + "," +
    normalScaleOperation_->bestSellersOnSelfServeMainScreenRaw() + "," +
    normalScaleOperation_->mealsRequireWeightRaw() + "," +
    normalScaleOperation_->suggestiveSellingStyleRaw() + "," +
    normalScaleOperation_->suggestiveSellingFreqRaw() + "," +
    normalScaleOperation_->alwaysEnableVirtualKeyboardRaw() + "," +
    normalScaleOperation_->showSelectAllWhenVoidingTransRaw() + "," +
    normalScaleOperation_->periodicOfflineBeep() + "," +
    normalScaleOperation_->fixedWeightVerificationRaw() + "," +
    normalScaleOperation_->cuttingTestTypeRaw() + "," +                 ///< save cut test type
    normalScaleOperation_->maxSuspendedTestsRaw() + "," +               ///< save cut test max suspended tests
    normalScaleOperation_->maxSuspendedTestsPerOperatorRaw() + "," +    ///< save cut test max tests/operator
    normalScaleOperation_->inactivitylogoutRaw() + "," +                ///< save cut test inactivity logout
    normalScaleOperation_->bottomScreenTypeRaw() + "," +
    normalScaleOperation_->enableSearchMenuButtonRaw() + "," +
    normalScaleOperation_->enableHelpPromptsRaw() + "," +
    normalScaleOperation_->enableUseCharacterSearchingRaw() + "," +
    normalScaleOperation_->useSecondTareRaw() + "," +
    normalScaleOperation_->trackingSupervisorChangesRaw() + "," +
    normalScaleOperation_->variableShelfLifeTime() + "," +
    normalScaleOperation_->selfServeWeightRequiredToSelectItemRaw() + "," +
    normalScaleOperation_->selfServeByCountItemsAllowedRaw() + "," +
    normalScaleOperation_->selfServeDisplayLabelOnScreenGORaw() + "," +
    normalScaleOperation_->voidTransactionsViewHoursRaw() + "," +
    normalScaleOperation_->selfServeDisplayTareOnRandomWeightRaw() + "," +
    normalScaleOperation_->autoDoNotPrintNutrifactsRaw() + "," +
    normalScaleOperation_->selfServeEnablePlaylistsRaw() + "," +
    normalScaleOperation_->selfServePlaylistFileRaw() + "," +
    normalScaleOperation_->enableSearchOnKeypadRaw() + "," +
    normalScaleOperation_->enableEnhancedCenterScreenRaw() + "," +
    normalScaleOperation_->enhancedCenterHomePage() + "," +
    normalScaleOperation_->autoUpdateUPCBarcodeOver99Raw() + "," +
    normalScaleOperation_->barcodeTypeOver99Raw() + "," +
    normalScaleOperation_->disableCuttingBetweenLabelsRaw() + "," +
    normalScaleOperation_->conserveLinerlessStockRaw() + "," +
    normalScaleOperation_->selfServeIgnorePrinterErrorsRaw() + "," +
    normalScaleOperation_->selfServeCombineSearchKeypadFlashkeysRaw() + "," +
    normalScaleOperation_->selfServeDisplayPluLabelRaw() + "," +
    "\"" + normalScaleOperation_->minSweetheartWeight() + "\"" + "," +
    normalScaleOperation_->selfServeDisplayBarcodeLabelRaw()  + "," +
    normalScaleOperation_->selfServeStartScreenStateRaw()  + "," +
    normalScaleOperation_->selfServeItemEntryScreenStateRaw()  + "," +
    normalScaleOperation_->selfServeSearchScreenStateRaw()  + "," +
    normalScaleOperation_->selfServePrintScreenStateRaw()  + "," +
    normalScaleOperation_->selfServeAfterPrintScreenStateRaw()  + "," +
    normalScaleOperation_->selfServeStartScreenClickableRaw()  + "," +
    normalScaleOperation_->selfServeIncludeKeypadRaw() + "," +
    normalScaleOperation_->selfServeHideButtonBarRaw() + "," +
    normalScaleOperation_->selfServePrintOnStableWeightRaw()  + "," +
    normalScaleOperation_->enableVariableShelfLifeTimeRaw()  + "," +

    normalScaleOperation_->secondaryLabelPlacementRaw() + "," +
    normalScaleOperation_->secondaryApplierRotationRaw() + "," +

    normalScaleOperation_->wrapperCameraEnabledRaw() + "," +
	
    //PJR413 (SOF-4772) - Add Fixed PLU to normalScaleOperation_
    normalScaleOperation_->selfServeFixedPluRaw() + "," +
    normalScaleOperation_->selfServeTimeoutRaw() + "," +
    normalScaleOperation_->selfServeEnableFreshServeUIRaw() + "," +
    normalScaleOperation_->selfServeFreshServeFlashkeyType0Raw() + "," +
    normalScaleOperation_->selfServeFreshServeFlashkeyType1Raw() + "," +
    normalScaleOperation_->selfServeFreshServeFlashkeyType2Raw() + "," +
    normalScaleOperation_->selfServeFreshServeFlashkeyType3Raw() + "," +
    normalScaleOperation_->selfServeFreshServeFlashkeyType4Raw() + "," +
    normalScaleOperation_->selfServeFreshServeFlashkeyText0Raw() + "," +
    normalScaleOperation_->selfServeFreshServeFlashkeyText1Raw() + "," +
    normalScaleOperation_->selfServeFreshServeFlashkeyText2Raw() + "," +
    normalScaleOperation_->selfServeFreshServeFlashkeyText3Raw() + "," +
    normalScaleOperation_->selfServeFreshServeFlashkeyText4Raw() + "," +
    normalScaleOperation_->selfServeFreshServeFlashkeyNum0Raw() + "," +
    normalScaleOperation_->selfServeFreshServeFlashkeyNum1Raw() + "," +
    normalScaleOperation_->selfServeFreshServeFlashkeyNum2Raw() + "," +
    normalScaleOperation_->selfServeFreshServeFlashkeyNum3Raw() + "," +
    normalScaleOperation_->selfServeFreshServeFlashkeyNum4Raw() + "," +
    normalScaleOperation_->selfServeFreshServeLabelOffsetRaw()  + "," +
    normalScaleOperation_->selfServeFreshServeFlashkeySecText0Raw() + "," +
    normalScaleOperation_->selfServeFreshServeFlashkeySecText1Raw() + "," +
    normalScaleOperation_->selfServeFreshServeFlashkeySecText2Raw() + "," +
    normalScaleOperation_->selfServeFreshServeFlashkeySecText3Raw() + "," +
    normalScaleOperation_->selfServeFreshServeFlashkeySecText4Raw();

    ui->txtFlashkeyNum_0->setEnabled( false );
    ui->txtFlashkeyNum_0->setEnabled( false );
    ui->txtFlashkeyNum_0->setEnabled( false );
    ui->txtFlashkeyNum_0->setEnabled( false );
    ui->txtFlashkeyNum_0->setEnabled( false );

    session_->send(ConfigurationDev,CfgScaleOperationSave,msg);     //	112,72,msg
    session_->send(ConfigurationDev,CfgScaleRead);                  //	112,20 - to make sure we update

    session_->send(ConfigurationDev,CfgScaleOperationRead);         // update ui

    // SOF-3846
    if( disableCuttingBetweenLabelsToggled )
    {
        // Need to tell the scale application to re-read the Printer Teach table

        session_->send( Printer, PrResetTeach );                    // 7,68

        disableCuttingBetweenLabelsToggled = false;
    }

    hasChanged = false;                                             // kind of started not using this

    if( hasChangedDisplayOptions )
        saveDisplayOptions();

    if( tabAccumHasChanged )
        saveAccumulateItems();

    QString priLabelplacement = normalScaleOperation_->labelPlacement();
    QString secLabelplacement = QString::number( normalScaleOperation_->secondaryLabelPlacement() );

    session_->scaleConfig->primaryPlacementSystem = normalScaleOperation_->labelPlacement().toInt();
    session_->scaleConfig->secondaryPlacementSystem = normalScaleOperation_->secondaryLabelPlacement();

    LOG_DEBUG( "Bottom of saveDataOnExit", NULL );

}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::saveDisplayOptions()
{
    // the upcNumber and product image are not manipulated here, just their original vals are passed back

    QString msg(
        operatorDisplayFields_->enableSellByRaw() + "," +                   // panels
        operatorDisplayFields_->enableBestBeforeRaw() + "," +               // panels
        operatorDisplayFields_->enableProdImgRaw() + "," +                  // not used
        operatorDisplayFields_->enableUpcNumberRaw() + "," +                // not used
        operatorDisplayFields_->enableFilmWidthRaw() + "," +                // Features->Wrapper Settings
        operatorDisplayFields_->enableFilmTensionRaw() + "," +              // Features->Wrapper Settings
        operatorDisplayFields_->enableWrapTypesRaw() + "," +                // not used
        operatorDisplayFields_->enableCountryOfOrginRaw() + "," +           // panels
        operatorDisplayFields_->enableTrackingNumberRaw() + "," +           // panels
        operatorDisplayFields_->enableProdEntryRaw() + "," +                // features->production entry
        operatorDisplayFields_->prodEntryContinuousPrintRaw() + "," +       // features->production entry
        operatorDisplayFields_->enableProdPlanRaw() + "," +                 // features->production entry
        operatorDisplayFields_->prodPlanningFrame() + "," +                 // features->production entry
        operatorDisplayFields_->prodPlanningUpdate() + "," +                // features->production entry
        operatorDisplayFields_->appendingPluNumberToNameRaw() + "," +       // features->flashkeys
        operatorDisplayFields_->numberOfCharactersRaw() + "," +             // features->flashkeys
        operatorDisplayFields_->enableRunTotalsRaw() + "," +                // not used
        operatorDisplayFields_->enablePackedOnRaw() + "," +                 // not used
        operatorDisplayFields_->dcaStartScreenTypeRaw() + "," +             // panels
        operatorDisplayFields_->dcaRunScreenTypeRaw() + "," +               // panels
        operatorDisplayFields_->dcaKeypadAreaTypeRaw() + "," +              // panels - center
        operatorDisplayFields_->capacityBarLocationRaw() + "," +            // not used
        operatorDisplayFields_->keypadLocationRaw() + "," +                 // not used
        operatorDisplayFields_->enableSelfServiceRaw() + "," +              // features->self service
        operatorDisplayFields_->rightScreenTypeRaw() + "," +                //
        operatorDisplayFields_->bottomScreenTypeRaw() + "," +               //
        operatorDisplayFields_->defaultFlashkeyFontPointsizeRaw() );        // features->flashkeys

    session_->send( ConfigurationDev, CfgPrepackSave, msg );            // 112,142 -> save Prepack Config (operator display fields)
    session_->send( ConfigurationDev, CfgPrepackRead );                 // update ui

    hasChangedDisplayOptions = false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::saveAccumulateItems()
{
    QString msg = accumItemsConfig_->enableAccumItemsRaw() + "," +
    accumItemsConfig_->enableItemStripModeRaw() + "," +
    accumItemsConfig_->accumItemsReturnToStartScreenRaw() + "," +
    accumItemsConfig_->enableAccumItemsTotalsOnlyRaw() + "," +
    accumItemsConfig_->accumItemsEANFormatString() + "," +
    accumItemsConfig_->accumItemsEANPrefix() + "," +
    accumItemsConfig_->accumItemsEANCode() + "," +
    accumItemsConfig_->accumItemsScaleId() + "," +
    accumItemsConfig_->accumItemsTransNum() + "," +
    accumItemsConfig_->readTransactionsCompatibilityRaw() + "," +
    accumItemsConfig_->accumBarcodeUseFirstItemRaw() + "," +
    accumItemsConfig_->enableAccumItemsAndFlashkeysRaw() + "," +
    accumItemsConfig_->readBarcodeFromCloudRaw() + "," +
    accumItemsConfig_->barcodeServerAddress();

    tabAccumHasChanged = false;

    session_->send(ConfigurationDev, CfgAccumulateItemsSave, msg);

    session_->send(ConfigurationDev, CfgAccumulateItemsRead ); // update ui

    // because weight required for meal items is part of this block.
    // Why we didnt add it to the config accumulate block I dont know.
    saveDataOnExit();
}

// Removed compress time fields 05-15-2015 during timesys 1.15 release
// Called when you press AM/PM or hours or minutes field
//void SuperNormalScaleOperation::saveCompressTime()
//{
//    QString AmPm = ui->butCompressTimeAMPM->text();
//    QString hourStr = "";
//    QString minsStr = "";
//    QString timeStr = "";
//
//    if( ui->txtCompressTimeHours->text().length() == 1 )
//        hourStr = "0" + ui->txtCompressTimeHours->text();
//    else
//        hourStr = ui->txtCompressTimeHours->text();
//
//    if( ui->txtCompressTimeMinutes->text().length() == 1 )
//        minsStr = "0" + ui->txtCompressTimeMinutes->text();
//    else
//        minsStr = ui->txtCompressTimeMinutes->text();
//
//    timeStr += hourStr;
//    timeStr += ":";
//    timeStr += minsStr;
//    timeStr += " ";
//    timeStr += AmPm;
//
//    normalScaleOperation_->compressTime() = timeStr;
//}
//
//////////////////////////////////////////////////////////////////////////////////////////////////////
int SuperNormalScaleOperation::makeValidatorTop( int digits )
{
    int top = 0;

    while ( digits-- )
    {
        top *= 10;
        top += 9;
    }
    return top;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::saveDateTimeFormat()
{
    QString msgStr  = scaleConfigData_->countryCode() + "," +
    scaleConfigData_->languageCode()     + "," +
    scaleConfigData_->secCountryCode()   + "," +
    scaleConfigData_->secLanguageCode();

    qDebug() << "msgStr: " + msgStr;

    if( dateTimeHasChanged )
    {
        msgStr +=  "," + scaleConfigData_->dateStyleRaw() + "," + scaleConfigData_->timeFormatRaw();
    }

    qDebug() << "msgStr: " + msgStr;

    session_->send(ConfigurationDev,CfgScaleWrite, msgStr);			// write (112,21)
    session_->send(ConfigurationDev,CfgScaleSave);                  // save  (112,22)

    langHasChanged = false;
    dateTimeHasChanged = false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::updateNormalScaleOperations( const SessionMessage& msg )
{
    /**
     * Update and display recieved configuration information
     */

    DomNormalScaleOperation m( msg );

    if ( normalScaleOperation_ )
       *normalScaleOperation_ = m;                                  // update normal scale operator var

    session_->send(Weigher, WgReqInquire);                          // 1,36 -> So we know the weigherMaximumWt & theWeigherModel

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // update TAB 0 - Product
    ///////////////////////////////////////////////////////////////////////////////////////////////

    ui->chkAutoDisplayProdNotes->setChecked( m.autoDisplayProductNotes() );

    ui->chkAutoDisplaySuggestiveSelling->setChecked( m.autoDisplaySuggestiveSelling() );
    ui->lblSuggSellFreq->setEnabled( m.autoDisplaySuggestiveSelling() );
    ui->txtSuggSellFreq->setEnabled( m.autoDisplaySuggestiveSelling() );

    ui->txtSuggSellFreq->setText( QString::number( qMax( 1, m.suggestiveSellingFreq() ) ) );

    if (( 0 <= m.suggestiveSellingStyle() ) && ( m.suggestiveSellingStyle() < 3 ))
        ui->cboSuggSellStyle->setCurrentIndex( m.suggestiveSellingStyle() );

    ui->cboSuggSellStyle->setEnabled( m.autoDisplaySuggestiveSelling() );
    ui->lblSuggSellStyle->setEnabled( m.autoDisplaySuggestiveSelling() );

    ui->cboBarcodeTypeOver99->setEnabled( m.autoUpdateUPCBarcodeOver99() );

    ui->chkCOOLTrackNum->setChecked( m.coolTrackingNum() );
    ui->chkAutoDisplayCookingNotes->setChecked( m.autoDisplayCookingNotes());
    ui->chkDoNotPrintNutrifacts->setChecked( m.autoDoNotPrintNutrifacts());

    ui->chkAutoUpdateUPCBarcodeOver99->setChecked( m.autoUpdateUPCBarcodeOver99());
    int upc99 = m.barcodeTypeOver99();
    int upcin = ui->cboBarcodeTypeOver99->findData( upc99 );
    ui->cboBarcodeTypeOver99->setCurrentIndex( upcin );

    if( upc99 <= 0 )
        ui->cboBarcodeTypeOver99->setCurrentIndex( 0 );

    if ( isMetric_ )
    {
        // HT-210 Weight Precision should ALWAYS be "3" for Metric
        ui->cboExternalWeightPrecision->setCurrentIndex( ui->cboExternalWeightPrecision->findData( "3" ) );
    }
    else
    {
        int ewp = m.externalWeightPrecision().toInt();

        if(ewp == 2 || ewp == 3)
            ui->cboExternalWeightPrecision->setCurrentIndex(ewp - 2);
        else
            ui->cboExternalWeightPrecision->setCurrentIndex(1);         // why index = 1? I guess that value of 3 is the default
    }


    // setup of MaximumTare is in setupWeigherFields()

    ui->chkFixedWeightVerification->setChecked( m.fixedWeightVerification() );

    if( isMetric_ == false )
        ui->cboFixedWeightUnits->setCurrentIndex( m.fixedWeightUnits().toInt() );

    // HT-365: Parse shelf life time and set hour/minutes/am-pm controls
    ui->chkEnableShelfLifeExtraDay->setChecked( m.enableVariableShelfLifeTime() );

    ui->leShelfLifeHour->setEnabled( m.enableVariableShelfLifeTime() );
    ui->leShelfLifeMinutes->setEnabled( m.enableVariableShelfLifeTime() );
    ui->butShelfLifeAmPm->setEnabled( m.enableVariableShelfLifeTime() );

    if( !m.enableVariableShelfLifeTime() && m.variableShelfLifeTime() == "00:00 AM" )
    {
        ui->leShelfLifeHour->setText( "00" );
        ui->leShelfLifeMinutes->setText( "00" );
        ui->butShelfLifeAmPm->setText( "AM" );

        ui->chkEnableShelfLifeExtraDay->setChecked( false );

        ui->leShelfLifeHour->setEnabled( false );
        ui->leShelfLifeMinutes->setEnabled( false );
        ui->butShelfLifeAmPm->setEnabled( false );
    }

        QStringList tmpTime;
        if( m.variableShelfLifeTime().contains( ":" ) )
        {
            tmpTime = m.variableShelfLifeTime().split( ":", QString::KeepEmptyParts );
            ui->leShelfLifeHour->setText( tmpTime.at( 0 ) );

            QString tmpTime2 = tmpTime.at( 1 ).trimmed();
            if( tmpTime2.contains( " ") )
            {
                // The time contains the AM/PM indicator
                ui->leShelfLifeMinutes->setText( tmpTime.at( 1 ).split( " ", QString::KeepEmptyParts ).at( 0 ) );
                ui->butShelfLifeAmPm->setText( tmpTime.at( 1 ).split( " ", QString::KeepEmptyParts).at( 1 ).toUpper() );
            }
            else if( tmpTime.at( 1 ).length() > 0 )
            {
                // The time contains the minutes, but not the AM/PM indicator. Default to AM
                ui->leShelfLifeMinutes->setText( tmpTime.at( 1 ).split( " ", QString::KeepEmptyParts ).at( 0 ) );
                ui->butShelfLifeAmPm->setText( "AM" );
            }
            else
            {
                // There are no minutes.  Default to 00 minutes and AM
                ui->leShelfLifeMinutes->setText( "00" );
                ui->butShelfLifeAmPm->setText( "AM" );
            }

            // SOF-2755: Do not allow "00" to be entered for extra shelf life hour
            shelfLifeHoursValidator_->setBottom( 1 );
        }

    isShelfLifeAmSelected = ( ui->butShelfLifeAmPm->text() == "AM" ? true : false );

    ui->chkCollectTransactions->setChecked( m.collectTransactions() );
    ui->chkTrackOperChanges->setChecked( m.trackingOperatorChanges() );
    ui->chkTrackSuperChanges->setChecked( m.trackingSupervisorChanges() );
    enableDaysToKeepTransactions();

    int daysForTrans = m.daysToKeepTransactions().toInt() - 1;
    ui->cboDaysToKeepTransactions->setCurrentIndex( daysForTrans );

    // SOF-43
    ui->lblTransHoursToView->setEnabled( m.collectTransactions() );
    ui->txtTransHoursToView->setEnabled( m.collectTransactions() );
    ui->txtTransHoursToView->setText( QString::number( qMax( 1, m.voidTransactionsViewHours() ) ) );
    setupHoursToViewRecordsValidator( ( daysForTrans + 1 ) );

    ui->txtMinSweetheartWeight->setText( m.minSweetheartWeight() );

    bool autoDays = m.useAutoLookup();
    changeAutoLookup(autoDays);
    ui->chkUseAutoLookup->setChecked( autoDays );                   // does this cause a SIGNAL to be fired? YES!

    ui->chkUseSecondTare->setChecked( m.useSecondTare() );
    
    //FT-132 The Number of Lookup digits drop down bar should not have the option to select either "1", or a, "2" 
    int digitsToLookup = m.numOfDigits().toInt();                   // b\c the digit value is +1 to the index of the combo
    if(digitsToLookup < 3 || digitsToLookup > 6)                    // enforce a valid range
        digitsToLookup = 3;
    digitsToLookup -= 3;                                            // the value is an offset of 3 to the index
    ui->cboNumOfDigits->setCurrentIndex( digitsToLookup );
    comboboxChangedNumOfDigits( digitsToLookup );

    if ( !session_->isPrepack() )
    {
        if ( m.autoLookupOverrideDigit().isEmpty() )
            ui->cboSpAutoLookupDigit->setCurrentIndex( 0 );
        else
            ui->cboSpAutoLookupDigit->setCurrentIndex( m.autoLookupOverrideDigit().toInt() );
    }

    ui->chkClearPluAfterPrinting->setChecked( m.clearPluAfterPrinting()  );              // clear PLU after printing

    // Removed compress time fields 05-15-2015 during timesys 1.15 release
    //updateCompressTimeInfo();                                       // go set the compress DB fields options

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // update TAB 1 - Scale Access
    ///////////////////////////////////////////////////////////////////////////////////////////////

    // Had to reverse the logic of the checkbar, if we get a 1, then it means it IS NOT CHECKED. A 0 means it IS CHECKED
    m.allowAddressBar() ? ui->chkRemoveAddressBar->setChecked(false) : ui->chkRemoveAddressBar->setChecked(true);

    ui->chkDisableOpFlashkeyEditor->setChecked( m.disableOperatorFlashkeyEditor() );

    ui->chkShowPrinterSettings->setChecked( m.showPrinterSettings() );

    ui->chkDisableLeftPanel->setChecked( m.disableStartScreen() );

    ui->chkShowDeleteAllFiles->setChecked( m.showDeleteAllFiles() );

    ui->txtLogoutRemoteIdleTime->setText( m.logoutRemoteIdleTime() );

    ui->txtMinSweetheartWeight->setText( m.minSweetheartWeight() );

    bool allowRemote =  m.remoteAccessPermitted();
    ui->chkRemoteAccessPermitted->setChecked( allowRemote );

    int logMethod = m.remoteAccessLoginMethod();
    ui->cboRemoteAccessLoginMethod->setCurrentIndex( logMethod );

    ui->txtRemoteAccessUserName->setText( m.remoteAccessUserName() );

    ui->txtRemoteAccessPassword->setText( m.remoteAccessPassword() );

    changeRemoteAccess( allowRemote );

    changeRemoteAccessLoginMethod( logMethod );

    ui->chkShowSelectAllWhenVoidingTrans->setChecked( m.showSelectAllWhenVoidingTrans() );

    ui->chkSearchMenuButton->setChecked( m.enableSearchMenuButton() );

    ui->chkShowSearchInKeypad->setChecked( m.enableSearchOnKeypad() );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // update TAB 2 - System
    ///////////////////////////////////////////////////////////////////////////////////////////////

    QString langCtry = session_->scaleConfig->getLanguageCode() + session_->scaleConfig->getCountryCode();
    QString secLangCtry = session_->scaleConfig->getSecLanguageCode() + session_->scaleConfig->getSecCountryCode();
    setComboBoxIndex(ui->cboLanguageCode, langCtry);

    if(langCtry == secLangCtry)
        ui->cboSecLanguageCode->setCurrentIndex(0);                 // if equal, means that there is NO second ctry and lang
    else
        setComboBoxIndex(ui->cboSecLanguageCode, secLangCtry);

    ui->cboDateStyle->setCurrentIndex(ui->cboDateStyle->findData(session_->scaleConfig->getDateStyle()));

    setComboBoxIndex(ui->cboTimeFormat, session_->scaleConfig->getTimeFormat() );

    int popStyle = m.popupKeypadStyle() - 1;
    if( popStyle < 1 || popStyle > 2)
        ui->cboPopupKeypadStyle->setCurrentIndex(0);
    else
        ui->cboPopupKeypadStyle->setCurrentIndex( popStyle );

    int logStyle = m.opLoginKeyboardStyle() - 1;
    if( logStyle < 1 || logStyle > 2)
        ui->cboOpLoginKeyboardStyle->setCurrentIndex( 0 );          // enforce valid data
    else
        ui->cboOpLoginKeyboardStyle->setCurrentIndex( logStyle );

    if ( !session_->isPrepack() )
        ui->chkClearPluAfterPrinting->setChecked( m.clearPluAfterPrinting()  );              // clear PLU after printing
    int bccm = m.byCountCalculationMethod();
    if( bccm < 1 || bccm > 3)
        bccm = 1;
    bccm = bccm - 1;
    ui->cboByCountCalculationMethod->setCurrentIndex( bccm );

    int isEnabled = ( m.alwaysEnableVirtualKeyboard() == true ) ? 1 :0;
    ui->cboAlwaysEnableVirtualKeyboard->setCurrentIndex( isEnabled );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // update TAB 3 - Flashkeys
    ///////////////////////////////////////////////////////////////////////////////////////////////

    // chkAppendingPluNumberToName in updateOperatorDisplayFields()
    // chkNumberOfCharacters in updateOperatorDisplayFields()
    // txtNumberOfCharacters in updateOperatorDisplayFields()
    // defaultFlashkeyFontPointsize in updateOperatorDisplayFields()

    ui->txtResetStatisticsDays->setText( m.resetStatisticsDays() );

    ui->cboDefaultSearchMethod->setCurrentIndex( m.defaultSearchMethod().toInt() );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // update TAB 4 - Network
    ///////////////////////////////////////////////////////////////////////////////////////////////

    ui->chkNetworkDetailsDisplay->setChecked( m.enableDisplayNetworkDetails() );

    signalMapper_->blockSignals(true);
    bool isChecked = (m.checkNetConnectivity().toInt() == 1 ? true : false);
    ui->chkCheckNetConnectivity->setChecked( isChecked );

    if(isChecked)
    {
        if ( session_->isServiceCounter() )
            ui->chkPingPrimary->setEnabled( true );
        ui->chkPingGateway->setEnabled( true );
    }
    else
    {
        if ( session_->isServiceCounter() )
            ui->chkPingPrimary->setEnabled( false );
        ui->chkPingGateway->setEnabled( false );
    }
    if ( session_->isServiceCounter() )
        ui->chkPingPrimary->setChecked( m.pingPrimary() );

    ui->chkPingGateway->setChecked( m.pingGateway() );

    ui->chkPingTarget->setChecked( (m.pingTarget().toInt() == 1 ? true : false) );

    signalMapper_->blockSignals(false);

    setPingTargetState();

    ui->cboNowServingType->setCurrentIndex( m.nowServingSysType() );

    // Split apart the string so I can update the following fields
    QString pingTargetIP = m.pingTargetIPAddress();
    QStringList pingList = pingTargetIP.split(".");

    if(pingList.length() == 4)
    {
        ui->txtPingTarget1->setText(pingList.at(0));
        ui->txtPingTarget2->setText(pingList.at(1));
        ui->txtPingTarget3->setText(pingList.at(2));
        ui->txtPingTarget4->setText(pingList.at(3));
    }
    else
    {
        ui->txtPingTarget1->setText("");
        ui->txtPingTarget2->setText("");
        ui->txtPingTarget3->setText("");
        ui->txtPingTarget4->setText("");
    }

    //QString pingTime = m.pingInterval();

    updatePingIntervalInfo();
    setPingIntervalState();

    ui->txtPeriodicOfflineBeep->setText( m.periodicOfflineBeep() );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // update TAB 5 - Panels
    ///////////////////////////////////////////////////////////////////////////////////////////////

    // chkEnableCountryOfOrgin is in updateOperatorDisplayFields()

    // chkEnableTrackingNumber is in updateOperatorDisplayFields()

    // chkEnableSellBy         is in updateOperatorDisplayFields()

    // chkEnableBestBefore     is in updateOperatorDisplayFields()

    // chkEnablePackedOn       is in updateOperatorDisplayFields()

    // FT-1466
    ui->chkEnableTransType->setChecked( m.showTotalTypeOnRunScreen() );

    // cboNumberPad is in updateOperatorDisplayFields()

    // chkEnableWrapTypes is in updateOperatorDisplayFields()

    ui->chkEnableEnhancedCenterScreen->setChecked( m.enableEnhancedCenterScreen() );

    int index = ui->cboAccessBrowserMode->findData( m.dcaStartScreenTypeRaw() );
    if ( 0 <= index )
        ui->cboAccessBrowserMode->setCurrentIndex( index );

    index = ui->cboRunScreen->findData( m.dcaRunScreenTypeRaw() );
    if ( 0 <= index )
        ui->cboRunScreen->setCurrentIndex( index );

    if( index == 2 )
        ui->lblDefaultBrowserHomePage->setEnabled( true );
    else
        ui->lblDefaultBrowserHomePage->setDisabled( true );

    if( session_->isPrepack() )
    {
        index = ui->cboRightPanelMode->findData( m.rightScreenTypeRaw() );

        if ( 0 <= index )
            ui->cboRightPanelMode->setCurrentIndex( index );
    }

    index = ui->cboBottomPanelMode->findData( m.bottomScreenTypeRaw() );
    if ( 0 <= index )
        ui->cboBottomPanelMode->setCurrentIndex( index );

    // FT-790
    ui->txtRunScreenHomePage->setText( m.runScreenHomePage() );

    ui->txtEnhancedCenterScreenAddress->setText( m.enhancedCenterHomePage() );

    int startHome = m.startScreenHome().toInt();
    ui->cboStartScreenHome->setCurrentIndex( startHome );

    ui->txtDefaultHomePage->setText( m.defaultHomePage() );
    enableBrowserInPanel(m.accessBrowserMode());

    // On HTs this feature is disabled
    if ( startHome == 0 )
        if ( !isLicensed_FullscreenFlashkeys_ && !isLicensed_WebBrowser_ )
            startHome = 1; // fix to center screen

    if ( startHome == 2 )
        if ( !isLicensed_MealBuilder_)
            startHome = 1; // fix to center screen

    updateClearPluOnPrint( startHome );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // update Tab 5 - Features
    ///////////////////////////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // update Tab5->Tab0 - Flashkeys
    ///////////////////////////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // update Tab5->Tab1 - Production Entry
    ///////////////////////////////////////////////////////////////////////////////////////////////

    ui->chkDisableCuttingBetweenLinerlessLabels->setChecked( m.disableCuttingBetweenLabels() );

    if( cutterAttached_ && cutterEnabled_ )
    {
        // SOF-3947: Force "Enable continuous label print in production entry" to be checked
        //           and disabled when "Disable cutting between linerless label printing"
        //           is checked.

        if( ui->chkDisableCuttingBetweenLinerlessLabels->isChecked() )
        {
            ui->chkProdEntryContinuousPrint->setChecked( true );
            ui->chkProdEntryContinuousPrint->setEnabled( false );
        }
    }


    ///////////////////////////////////////////////////////////////////////////////////////////////
    //  update Tab5->Tab2 - Accumulate Items - Comes in on a different message. See updateAccumulateItems()
    ///////////////////////////////////////////////////////////////////////////////////////////////

    ui->chkRequireWeightForMealItems->setChecked( m.mealsRequireWeight() );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // update Tab6->Tab3 - Now Serving
    ///////////////////////////////////////////////////////////////////////////////////////////////

    // FT-1458 Now Serving Server IP Address
    QString nowServingQueueIP = m.nowServingQueueIPAddress();
    QStringList nowServingList = nowServingQueueIP.split(".");

    if(nowServingList.length() == 4)
    {
        ui->txtNowServing1->setText(nowServingList.at(0));
        ui->txtNowServing2->setText(nowServingList.at(1));
        ui->txtNowServing3->setText(nowServingList.at(2));
        ui->txtNowServing4->setText(nowServingList.at(3));
    }
    else
    {
        ui->txtNowServing1->setText("");
        ui->txtNowServing2->setText("");
        ui->txtNowServing3->setText("");
        ui->txtNowServing4->setText("");
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Tab5->Tab4 - Self Service
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // update of chkEnableSelfService in updateOperatorDisplayFields()
    // START
    // 0 = None
    // 1 = begin on start panel ui->chkDisplaySelfServeStartScreen->setChecked( m.displaySelfServeStartScreen() );          // START HERE
    // 2 = ui->chkSelfServeWeightRequiredToSelectItem->setChecked( m.selfServeWeightRequiredToSelectItem() );               // START HERE
    // 3 = ui->chkSelfServeEnablePlaylists->setChecked( m.selfServeEnablePlaylists() );                                     // START HERE
    // 4 = LOGIN m.selfServeStartScreenState() = 4

    // PLU ENTRY
    // 0 = Category Flashkeys
    // 1 = ui->chkBestSellersOnSelfServeMainScreen->setChecked( m.bestSellersOnSelfServeMainScreen() );                     // START HERE
    // 2 = number pad

    // SEARCH
    // 1 = ui->chkUseCharacterSearching->setChecked( m.enableUseCharacterSearching() );                                     // SEARCH HERE

    // PRINT
    // 1 = ui->chkselfServeDisplayLabelOnScreenGO->setChecked( m.selfServeDisplayLabelOnScreenGO() );                           // PRINT WHAT (Amazon GO)

    // pjr383 - no longer needed
    // ui->chkSelfServeCombineSearchKeypadFlashkeys->setChecked( m.selfServeCombineSearchKeypadFlashkeys() );
    // ui->cboSelfServeIgnorePrinterErrors->setCurrentIndex( m.selfServeIgnorePrinterErrors() );
    // ui->chkSelfServeDisplayBarcodeLabel->setChecked( m.selfServeDisplayBarcodeLabel() );
    signalMapper_->blockSignals(true);
    bool booltester = m.enableHelpPrompts();
    if( booltester )
    {
        ui->chkHelpPrompts->setChecked( true );
        ui->cboPrintWhat->setCurrentIndex( 0 );
        ui->cboPrintWhat->setEnabled( false );
    }
    else
    {
        ui->cboPrintWhat->setEnabled( true );
        ui->chkHelpPrompts->setChecked( false );
    }
    signalMapper_->blockSignals(false);

    ui->chkSelfServeTareOnRandomWeight->setChecked( m.selfServeDisplayTareOnRandomWeight() );
    ui->chkSelfServeByCountItemsAllowed->setChecked( m.selfServeByCountItemsAllowed() );

    ui->chkStartScreenClickable->setChecked( m.selfServeStartScreenClickable() );
    ui->chkSelfServePrintOnStableWeight->setChecked( m.selfServePrintOnStableWeight() );
    ui->chkSelfServeDisplayPluLabel->setChecked( m.selfServeDisplayPluLabel() );
    ui->chkIncludeNumpad->setChecked( m.selfServeIncludeKeypad() );
    ui->chkSelfServeHideButtonBar->setChecked( m.selfServeHideButtonBar() );
    ui->chkEnableGen2UI->setChecked( m.selfServeEnableFreshServeUI() );

    // PJR413 (SOF-4772) - FixedPLU
    ui->txtSelfServeFixedPlu->setText( m.selfServeFixedPlu() );

    // update START WHERE
    updateSelfServStartState( m.displaySelfServeStartScreen(), m.selfServeWeightRequiredToSelectItem(), m.selfServeEnablePlaylists(), m.selfServeStartScreenState() );

    // update ITEM ENTRY WHO
    updateSelfServItemEntryState(m.bestSellersOnSelfServeMainScreen(), m.selfServeItemEntryScreenState() );

    // update SEARCH HOW
    updateSelfServSearchState( m.enableUseCharacterSearching(), m.selfServeSearchScreenState() );

    // update PRINT WHAT
    updateSelfServPrintState( m.selfServeIgnorePrinterErrors(), m.selfServeDisplayBarcodeLabel(), m.selfServePrintScreenState() );

    // udpate PRINTING WHEN
    updateSelfServPrintingState( m.selfServeAfterPrintScreenState() );

    // update Fresh Serve options
    updateSelfServFreshServOptions( msg );
    ui->lblFreshServeLabelOffset->setText( m.selfServeFreshServeLabelOffset() );

    // for remote session we need to disable edit button
    if (( !mainWindow_->isPrimarySessionType() ) && ( ui->butEditPlaylist->isEnabled() ))
    {
#ifndef QT_DEBUG
        // disable edit button
        ui->butEditPlaylist->setEnabled( false );
#endif
    }

    int playlistIndex;
    if ( (playlistIndex = ui->cboSelfServePlaylist->findText( m.selfServePlaylistFile() )) < 0 )
    {
        ui->cboSelfServePlaylist->addItem( m.selfServePlaylistFile(), m.selfServePlaylistFile() );
        playlistIndex = ui->cboSelfServePlaylist->count() - 1;
    }

    ui->cboSelfServePlaylist->blockSignals( true );
    ui->cboSelfServePlaylist->setCurrentIndex( playlistIndex );
    ui->cboSelfServePlaylist->blockSignals( false );

    // if enable 2nd Gen is true, hide everything else, at least for now
    if( ui->chkEnableGen2UI->isChecked() )
    {
        ui->stackSelfServFS->setCurrentIndex( 1 );
        ui->mainWidget->setVisible( false );
    }
    else
    {
        ui->stackSelfServFS->setCurrentIndex( 0 );
        ui->mainWidget->setVisible( true );
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Tab5->Tab7 - Cutting Test
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /* Update data on the Cut Test Tab */
    ui->cboCuttingTestType->setCurrentIndex( m.cuttingTestType() );
    ui->txtMaxSuspendedTests->setText( QString::number( m.maxSuspendedTests() ) );
    ui->txtMaxSuspendedTestsPerOperator->setText( QString::number( m.maxSuspendedTestsPerOperator() ) );
    ui->txtInactivitylogout->setText( QString::number( m.inactivitylogout() ) );

	if ( session_->isPrepack() )
        setupPrepackFields(m);

    m.maxTare();

    isUpdating = false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::updateOperatorDisplayFields( const SessionMessage& msg )
{
    // method represets "initPageFour()" in superEppConfigScripts.asp

    MadpOperatorDisplayFields m( msg );

    if ( operatorDisplayFields_ )
       *operatorDisplayFields_ = m;                                 // update operator display fields var

    // on update() - initialize operatorDisplayFields_

    ui->chkEnableProdEntry->setChecked( m.enableProdEntry() );
    ui->chkEnableSelfService->setChecked( m.enableSelfService() );
    ui->chkProdEntryContinuousPrint->setChecked( m.prodEntryContinuousPrint() );

    ui->chkEnableProdPlan->setChecked( m.enableProdPlan() );
    ui->txtProdPlanningFrame->setText( m.prodPlanningFrame() );
    ui->txtProdPlanningUpdate->setText( m.prodPlanningUpdate() );

    if (!session_->isPrepack())
        enableProdPlanSource( ui->chkEnableProdPlan->isChecked() );

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // update TAB 3 - Flashkeys
    ///////////////////////////////////////////////////////////////////////////////////////////////

    ui->chkAppendingPluNumberToName->setChecked( m.appendingPluNumberToName() );
    int numOfChars = m.numberOfCharacters();

    if(numOfChars > 0)
    {
        // means that the checkbox is UNchecked and a value goes into the checkbox
        ui->txtNumberOfCharacters->setText( m.numberOfCharactersRaw() );
        ui->chkNumberOfCharacters->setChecked(false);               // not a saved field per say, calculated by numberOfCharacters
        ui->lblNumOfChars->setEnabled( true );
    }
    else if( numOfChars == 0 )
    {
        // means that the checkbox is checked
        ui->txtNumberOfCharacters->setText("0");
        ui->txtNumberOfCharacters->setEnabled(false);
        ui->lblNumOfChars->setEnabled( false );
        ui->chkNumberOfCharacters->setChecked(true);                // not a saved field per say, calculated by numberOfCharacters
    }

    ui->txtDefaultFlashkeyFontPointsize->setText( m.defaultFlashkeyFontPointsizeRaw() );
    //ui->txtDefaultFlashkeyFontPointsize->setEnabled(true);

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // update TAB 5 - Features
    ///////////////////////////////////////////////////////////////////////////////////////////////
    int index;
    ui->chkEnableCountryOfOrgin->setChecked( m.enableCountryOfOrgin() );
    ui->chkEnableTrackingNumber->setChecked( m.enableTrackingNumber() );
    ui->chkEnableSellBy->setChecked( m.enableSellBy() );
    ui->chkEnableBestBefore->setChecked( m.enableBestBefore() );
    ui->chkEnablePackedOn->setChecked( m.enablePackedOn() );

    if( ui->chkEnableEnhancedCenterScreen->isChecked() )
    {
        ui->lblEnhancedCenterScreenAddress->setEnabled( true );
        ui->txtEnhancedCenterScreenAddress->setEnabled( true );
    }
    else
    {
        ui->lblEnhancedCenterScreenAddress->setDisabled( true );
        ui->txtEnhancedCenterScreenAddress->setDisabled( true );
    }

    index = ui->cboNumberPad->findData( m.dcaKeypadAreaTypeRaw() );
    if ( 0 <= index )
        ui->cboNumberPad->setCurrentIndex( index );

    ui->chkEnableWrapTypes->setChecked( m.enableWrapTypes() );

    index = ui->cboRunScreen->findData( m.dcaRunScreenTypeRaw() );
    if ( 0 <= index )
        ui->cboRunScreen->setCurrentIndex( index );

    // FT-790
    if( index == 2 )
    {
        ui->lblRunScreenHomePage->setEnabled( true );
        ui->txtRunScreenHomePage->setEnabled( true );
    }
    else
    {
        ui->lblRunScreenHomePage->setDisabled( true );
        ui->txtRunScreenHomePage->setDisabled( true );
    }

    if( ui->chkEnableEnhancedCenterScreen->isChecked() )
    {
        ui->lblEnhancedCenterScreenAddress->setEnabled( true );
        ui->txtEnhancedCenterScreenAddress->setEnabled( true );
    }
    else
    {
        ui->lblEnhancedCenterScreenAddress->setDisabled( true );
        ui->txtEnhancedCenterScreenAddress->setDisabled( true );
    }

    if ( session_->isPrepack() )
    {
        //	Update Wrapper Settings
        ui->chk_displayFilmWidth->setChecked( m.enableFilmWidth() );
        ui->chk_displayFilmTension->setChecked( m.enableFilmTension() );
    }

    isUpdating = false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::updateAccumulateItems( const SessionMessage& msg )
{
    MadpAccumItemsConfig m( msg );

    if ( accumItemsConfig_ )
       *accumItemsConfig_ = m;

    ui->chkEnableAccumItems->setChecked( m.enableAccumItems() );
    ui->chkEnableItemStripMode->setChecked( m.enableItemStripMode() );
    ui->chkAccumItemsReturnToStartScreen->setChecked( m.accumItemsReturnToStartScreen() );
    ui->chkEnableAccumItemsTotalsOnly->setChecked( m.enableAccumItemsTotalsOnly() );
    ui->chkReadTransactionsCompatibility->setChecked( m.readTransactionsCompatibility() );
    ui->chkBarcodeUseFirstItem->setChecked( m.accumBarcodeUseFirstItem() );
    ui->butAccumItemsEANFormatString->setText( m.accumItemsEANFormatString() );
    ui->txtAccumItemsEANPrefix->setText( m.accumItemsEANPrefix() );
    ui->txtAccumItemsEANCode->setText( m.accumItemsEANCode());
    ui->txtAccumItemsScaleId->setText( m.accumItemsScaleId() );
    ui->txtAccumItemsTransNum->setText( m.accumItemsTransNum() );
    ui->chkEnableAccumItemsAndFlashkeys->setChecked( m.enableAccumItemsAndFlashkeys() );
    ui->chkEnableCloudFeature->setChecked(m.readBarcodeFromCloud() );
    ui->txtAccumItemsServerAddress->setText(m.barcodeServerAddress());

    bool isEnabled = ui->chkEnableAccumItems->isChecked();
    checkEnabledEanFields( isEnabled );

    isUpdating = false;

    // if enable cloud feature is checked upon startup
    if(m.readBarcodeFromCloud() && m.enableAccumItems())
    {
        checkboxChangedEnableCloudFeature(1);
        ui->txtAccumItemsServerAddress->setCursorPosition(0);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::setupWeigherFields()
{
    LOG_CLASS_INFO( "SuperNormalScaleOperation", "setupWeigherFields" );

    // This method uses the current DomWeigherConfigRead values.  At boot this field
    // should have been received at least once if not multiple times. For init only

    DomWeigherConfigRead *weigherFields = &session_->scaleConfig->weigherConfigRead;

    int wayerModel = weigherModel_;                                // get the model (where the default is HLX)
    bool isInvalidWayerModel = false;
    int factor = weigherFields->weigherScaleFactor();

    ///////////////////////////////////////////////////////
    if( ( wayerModel < SessionMainWindow::HLX1 ) || ( wayerModel > SessionMainWindow::HTM ) )
    {
        isInvalidWayerModel = true;
        wayerModel = SessionMainWindow::None;
    }

    //////////////////////////////////////////////////////
    // FT-1699 - MAX Tare has all kinds of issues
    ui->txtMaxTare->setValidator(maxTare_ = new QIntValidator(0, INT_MAX, this));

    // HT-189 Set # of decimal places based on the Tare Entry Precision
    ui->txtMaxTare->setDecimalPlaces( normalScaleOperation_->tareEntryPrecision().toInt() );
//    if( factor == 1 )
//        ui->txtMaxTare->setDecimalPlaces( 2 );                      // scale factor = 1, 0.01
//    else
//        ui->txtMaxTare->setDecimalPlaces( 3 );
    ui->txtMaxTare->setDecimal( currencyData_->unitSeparator() );   // scalefactor = 5, 0.005
    ui->txtMaxTare->setInputWrapping(true);
    maxTare_->setTop( (2 == ui->txtMaxTare->decimalPlaces()) ? 999 : 9995 ); // FT-1496
    //////////////////////////////////////////////////////

    ui->txtMaxTare->setText( normalScaleOperation_->maxTare() );

    // recheck tare precision since it is previously set in line of code just above this comment.



    if ( session_->isPrepack() )
	{
	    ui->txtTareSetback->setText( normalScaleOperation_->tareSetback() );
	}

    //----------------------------------------------------
    // Set Tare Setback - for Pre-Pack only
    //----------------------------------------------------
    if( session_->isPrepack() )
    {
        //int precision = scaleConfigData_->tarePrecision();

        // HT-189 Set # of decimal places based on the Tare Entry Precision
        int precision = normalScaleOperation_->tareEntryPrecision().toInt();
        LOG_TRACE( "precision = %d", precision );

        ui->txtTareSetback->setValidator( tareSetback_ = new QIntValidator( 0, INT_MAX, this ) );
        ui->txtTareSetback->setDecimalPlaces( precision );
        ui->txtTareSetback->setDecimal( currencyData_->unitSeparator() );
        ui->txtTareSetback->setInputWrapping( true );
        LOG_TRACE( "tareSetback_->setTop(makeValidatorTop(%d)", ( precision + 1 ) );
        tareSetback_->setTop(makeValidatorTop( precision + 1) );
    }
    //////////////////////////////////////////////////////

    // FT-73 Max tare label in supervisor mode does not change to Kg.
    // moved from update of weigher message a 1,2.  Dont need the weigher to set this.
    ///////////////////////////////////////////////////////
    if(isMetric_)
    {
        ui->lblUnits->setText("kg");
        ui->lblUnits_2->setText("kg");  //  For tare setback
    }
    else
    {
        ui->lblUnits->setText("lb");
        ui->lblUnits_2->setText("lb");  //  For tare setback
    }

    ///////////////////////////////////////////////////////
    if( ( isMetric_ ) || ( isInvalidWayerModel) )
        ui->cboExternalWeightPrecision->setEnabled(false);
    else
        ui->cboExternalWeightPrecision->setEnabled(true);

    ///////////////////////////////////////////////////////
    if(wayerModel == SessionMainWindow::None)
    {
        ui->txtMaxTare->setEnabled( false );
        ui->lblUnits->setEnabled( false );

	    if ( session_->isPrepack() )
		{
	        ui->txtTareSetback->setEnabled( false );
		}
        ui->lblUnits_2->setEnabled( false );
    }
    else
    {
        ui->txtMaxTare->setEnabled( true );
        ui->lblUnits->setEnabled( true );

	    if ( session_->isPrepack() )
		{
	        ui->txtTareSetback->setEnabled( true );
		}
        ui->lblUnits_2->setEnabled( true );
    }

    // set up the weigher max information - AKA setTheWeigherMaxWt()
    double theMax = weigherFields->weigherMaximumWt().toDouble();
    double units = 0.01;

    if( isMetric_ )
        units = 0.001;

    units = units * factor;

    theWeigherMaxWt = theMax + (9 * units);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::updateWeigherFields( const SessionMessage& msg )
{
	LOG_CLASS_INFO( "SuperNormalScaleOperation", "updateWeigherFields" );
    // FT-180 Weight pricision compatibilty and Tare precision fields becoming inactive
    // Really restructured this code for this bug.  Moved stuff out, moved other in
    // changed some of the conditions and updated them from the Javascript code.

    DomWeigherConfigRead m( msg );

    bool isInvalidModel = false;
    weigherModel_ = m.weigherModel();                                // get the incoming model
    int factor = m.weigherScaleFactor();

    ///////////////////////////////////////////////////////
    if( ( weigherModel_ < SessionMainWindow::HLX1 ) || ( weigherModel_ > SessionMainWindow::HTM ) )
    {
        isInvalidModel = true;
        weigherModel_ = SessionMainWindow::None;
    }

    //bool noDualRange = true;
    //if( (weigherModel_ == SessionMainWindow::HLX1) || (weigherModel_ == SessionMainWindow::AWS ) )
    // HT-189
    bool noDualRange = ( scaleConfigData_->isSingleRangeWeigher() ) ? true : false;
	LOG_TRACE( "noDualRange = %d", noDualRange );

    //////////////////////////////////////////////////////
    // FT-1699 - MAX Tare has all kinds of issues
    ui->txtMaxTare->setValidator(maxTare_ = new QIntValidator(0, INT_MAX, this));

    // HT-189 Set # of decimal places based on the Tare Entry Precision
    ui->txtMaxTare->setDecimalPlaces( normalScaleOperation_->tareEntryPrecision().toInt() );

//    if( factor == 1 )
//        ui->txtMaxTare->setDecimalPlaces( 2 );                      // scale factor = 1, 0.01
//    else
//        ui->txtMaxTare->setDecimalPlaces( 3 );
    ui->txtMaxTare->setDecimal( currencyData_->unitSeparator() );   // scalefactor = 5, 0.005
    ui->txtMaxTare->setInputWrapping(true);
    maxTare_->setTop( (2 == ui->txtMaxTare->decimalPlaces()) ? 999 : 9995 ); // FT-1496
    //////////////////////////////////////////////////////

    ui->txtMaxTare->setText( normalScaleOperation_->maxTare() );
    maxTareHasBeenUpdated = true;                                   // FT-1087 Max tare being set to 0.00 - only after physically setting the field do I set this to true

    if ( session_->isPrepack() )
	{
	    ui->txtTareSetback->setText( normalScaleOperation_->tareSetback() );
	}

    ///////////////////////////////////////////////////////
    if( ( isMetric_ ) || ( isInvalidModel) )
        ui->cboExternalWeightPrecision->setEnabled(false);
    else
        ui->cboExternalWeightPrecision->setEnabled(true);

    ///////////////////////////////////////////////////////
    if(weigherModel_ == SessionMainWindow::None)
    {
        ui->txtMaxTare->setEnabled( false );
        ui->lblUnits->setEnabled( false );

	    if ( session_->isPrepack() )
		{
	        ui->txtTareSetback->setEnabled( false );
		}
        ui->lblUnits_2->setEnabled( false );
    }
    else
    {
        ui->txtMaxTare->setEnabled( true );
        ui->lblUnits->setEnabled(true);

	    if ( session_->isPrepack() )
		{
	        ui->txtTareSetback->setEnabled( true );
		}
        ui->lblUnits_2->setEnabled( true );
    }

    // set up the weigher max information - AKA setTheWeigherMaxWt()
    double theMax = m.weigherMaximumWt().toDouble();
    double units = 0.01;

    if( isMetric_ )
        units = 0.001;

    units = units * factor;

    theWeigherMaxWt = theMax + (9 * units);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::updateLanguageValues(int languageCode, int newIndex)
{
    if( languageCode == PRIMARY_LANGUAGE_CHANGED )
    {
        scaleConfigData_->languageCode() = getPrimaryLanguageCode(newIndex);
        scaleConfigData_->countryCode()  = getPrimaryCountryCode(newIndex);
    }
    else if( languageCode == SECONDARY_LANGUAGE_CHANGED )
    {        
        if(newIndex == 0)
        {
            scaleConfigData_->secLanguageCode() = scaleConfigData_->languageCode();
            scaleConfigData_->secCountryCode()  = scaleConfigData_->countryCode();
            updateAndResetFlashkeys();
        }
        else
        {
            scaleConfigData_->secLanguageCode() = getSecondaryLanguageCode(newIndex);
            scaleConfigData_->secCountryCode() = getSecondaryCountryCode(newIndex);
        }
    }

    if( (scaleConfigData_->languageCode() != scaleConfigData_->secLanguageCode()) || (scaleConfigData_->countryCode() != scaleConfigData_->secCountryCode()) )
        session_->scaleConfig->setIsChangeLanguageTurnedOn( true );
    else
        session_->scaleConfig->setIsChangeLanguageTurnedOn( false );

    // HT-418
    if( dateTimeHasChanged || langHasChanged )
        saveDateTimeFormat();

    translate();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::updateAndResetFlashkeys()
{
    LOG_CLASS_INFO( "SuperNormalScaleOperation", "updateAndResetFlashkeys" );

    ScaleConfiguration *scaleConfig( session_->scaleConfig );

    // we are absolutely going to set the currentLanguage to be 1, but need to
    // find out first if we are going to have to reset flashkeys if
    // the currentLangauge is equal to 2.
    if ( 2 == scaleConfig->currentLanguage )
    {
        // update backend with language change
        session_->send( SoftkeyDev, ExecuteSoftkey, "4,1" );

        // set language
        scaleConfig->currentLanguage = 1;                      // if secondary is set to None, this this always true

        LOG_TRACE( "set language %s...", qPrintable( scaleConfig->getFPPLanguage() ) );
        mainWindow_->setLanguage( scaleConfig->getFPPLanguage() );
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::updateClearPluOnPrint(int startPageIndex)
{
    if ( !session_->isPrepack() )
    {
        if( startPageIndex == 0)
        {
            // selected the START screen as the screen to return to. Set as checked and disable it
            ui->chkClearPluAfterPrinting->setChecked( true );
            ui->chkClearPluAfterPrinting->setEnabled( false );
        }
        else if( startPageIndex == 1)
        {
            // selected the RUN screen as the screen to stay on. Set as unchecked and enable it
            if( !isUpdating )
                ui->chkClearPluAfterPrinting->setChecked( false );

            ui->chkClearPluAfterPrinting->setEnabled( true );
        }
    }
    else
    {
        // selected the START screen as the screen to return to. Set as checked and disable it
        ui->chkClearPluAfterPrinting->setChecked( true );
        ui->chkClearPluAfterPrinting->setEnabled( false );
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//void SuperNormalScaleOperation::updateCompressTimeInfo()
//{
//    // This is used to display the initial compress time during an update
//
//    bool isCompressEnabled = normalScaleOperation_->compressFlag();
//    ui->chkCompressFlag->setChecked( isCompressEnabled );
//
//    QString myTime  = normalScaleOperation_->compressTime();
//
//    // 04:00 AM
//
//    if( myTime != "" && myTime != " ")
//    {
//        QString tmpHr = myTime.mid(0,2);
//        QString tmpMin =  myTime.mid(3,2);							// FT-129 The Enable DB Compress time will not save.
//        QString tmpAmPm = myTime.mid(6,2);
//
//        if(tmpAmPm.length() == 0)
//            tmpAmPm = "AM";                                         // default AM if blank
//
//        ui->txtCompressTimeHours->setText(tmpHr);
//        ui->txtCompressTimeMinutes->setText(tmpMin);
//        ui->butCompressTimeAMPM->setText( tmpAmPm );
//    }
//    else
//    {
//        ui->txtCompressTimeHours->setText("04");
//        ui->txtCompressTimeMinutes->setText("00");
//        ui->butCompressTimeAMPM->setText( "AM" );
//    }
//
//    changeCompressFlag( isCompressEnabled );
//}
//
//////////////////////////////////////////////////////////////////////////////////////////////////////
//void SuperNormalScaleOperation::changeCompressFlag(bool isEnabled)
//{
//    if(isEnabled)
//    {
//        ui->txtCompressTimeHours->setEnabled( true );
//        ui->txtCompressTimeMinutes->setEnabled( true );
//        ui->butCompressTimeAMPM->setEnabled( true );
//    }
//    else
//    {
//        ui->txtCompressTimeHours->setEnabled( false );
//        ui->txtCompressTimeMinutes->setEnabled( false );
//        ui->butCompressTimeAMPM->setEnabled( false );
//    }
//}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::setCollectTransactionsDays(bool isEnabled)
{
    ui->lblTransDays->setEnabled(isEnabled);
    ui->cboDaysToKeepTransactions->setEnabled(isEnabled);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::updateSelfServStartState(bool begin_on_start, bool weight_required, bool enable_playlist, int start_state)
{
    // this method is called when we receive a configuration message from the Application. This is NOT a save.

    // compatiability
    int state = 0;          // represents the index of the combo box

    if( begin_on_start )                // means Big Green Go Button
        state = 1;
    else if( weight_required )          // Is Weight Required
        state = 2;
    else if( enable_playlist )          // Enabled the playlists
        state = 3;
    else if( 1 == start_state )         // means set to None
        state = 0;
    else if( 4 == start_state)          // means we want to force an operator to login
        state = 4;
    else if( 0 == start_state )         // Means value has never been set, do default to None
    {
        // USE CASE
        // If we got clear to here, that means NOTHING was set. so by default, set the start state to None
        normalScaleOperation_->selfServeStartScreenStateRaw() = "1";
        scaleConfigData_->selfServeStartScreenStateRaw() = "1";
    }

    if( state > 0 && ( 1 == start_state ) )
    {
        // USE CASE
        // if start_state is equal to 1, this means the customer has changed it on this screen or they
        // have started sending data down to the scale with new tag, S4, setting the start screen to NONE.
        // The problem is, the customer continues to send legacy configurations to the scale thru COMM
        // or use legacy configs in a backup. For example, the customer has WeightRequired turned on in their COMM
        // file. The new config/tag, since it HAS been set or sent down, takes precedence over the older ones.
        // so lets reset the legacy configs so we can help eliminate conflicting issues

        normalScaleOperation_->displaySelfServeStartScreenRaw() = "0";              // AmazonGo     OFF
        normalScaleOperation_->selfServeWeightRequiredToSelectItemRaw() = "0";      // Weight Req   OFF
        normalScaleOperation_->selfServeEnablePlaylistsRaw() = "0";                 // Playlists    OFF

        scaleConfigData_->displaySelfServeStartScreenRaw() = "0";                   // AmazonGo     OFF
        scaleConfigData_->selfServeWeightRequiredToSelectItemRaw() = "0";           // Weight Req   OFF
        scaleConfigData_->selfServeEnablePlaylistsRaw() = "0";                      // Playlists    OFF
    }

    // if playlists selected
    if( 3 == state )
    {
        ui->butEditPlaylist->setEnabled( true );
        ui->cboSelfServePlaylist->setEnabled( true );
    }
    else
    {
        ui->butEditPlaylist->setEnabled( false );
        ui->cboSelfServePlaylist->setEnabled( false );
    }

    // if weight required
    if( 2 == state )     // weight required
    {
        ui->chkStartScreenClickable->setVisible( true );                    // clickable

        //ui->chkSelfServePrintOnStableWeight->setVisible( true );          // stablee weight // SOF-4401: ALWAYS HIDE
    }
    else if( 3 == state )
    {
        ui->chkStartScreenClickable->setVisible( false );                   // clickable
    }
    else
    {
        ui->chkStartScreenClickable->setVisible( false );
        ui->chkStartScreenClickable->setChecked( false );                   // clickable

        ui->chkSelfServePrintOnStableWeight->setVisible( false );           // stable weight
    }

    ui->cboStartWhere->setCurrentIndex( state);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::updateSelfServItemEntryState(bool enable_bestsellers, int start_state)
{
    // this method is called when we receive a update configuration message from the Application.

    // compatiability
    int state = 0;

    if( enable_bestsellers )
        state = 1;
    else if( 1 == start_state )                 // yes I know I could of directly set these as 0 or 2 with an else, but I wanted to but
        state = 0;                              // but doing it like this and breaking them out allowed to be debug better
    //else if( 2 == start_state )
    //    state = 2;                              // hide search button
    else if( 2 == start_state )
        state = 2;                              // show keypad button
    else if( 3 == start_state )
        state = 3;                              // PJR-383: Fixed Flashkeys
    else if( 4 == start_state )
        state = 4;                              // keypad only
    else if( 0 == start_state )
    {
        // USE CASE
        // If we got clear to here, that means NOTHING was set. so by default, set the start state to None
        normalScaleOperation_->selfServeItemEntryScreenStateRaw() = "1";
        scaleConfigData_->selfServeItemEntryScreenStateRaw() = "1";
    }

    if( enable_bestsellers && ( start_state > 0 ) )
    {
        // USE CASE
        // if start_state is equal to 1, this means the customer has changed it on this screen or they
        // have started sending data down to the scale with new tag, S5, setting the item entry screen to Categories or Keypad.
        // The problem is, the customer continues to send legacy configurations to the scale thru COMM
        // or use legacy configs in a backup. For example, the customer has Best Sellers  turned on in their COMM
        // file. The new config/tag, since it HAS been set or sent down, takes precedence over the older ones.
        // so lets reset the legacy configs so we can help eliminate conflicting issues

        normalScaleOperation_->bestSellersOnSelfServeMainScreenRaw() = "0";
        scaleConfigData_->bestSellersOnSelfServeMainScreenRaw() = "0";
    }

    if( ( 0 == start_state ) || ( 1 == start_state ) || ( 2 == start_state ) || ( 3 == start_state ) )
       ui->chkIncludeNumpad->setVisible( true );
    else
        ui->chkIncludeNumpad->setVisible( false );

    ui->cboItemEntryWho->setCurrentIndex( state );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::updateSelfServSearchState(bool enable_chars, int start_state)
{
    // this method is called when we receive a configuration message from the Application. This is NOT a save.
    // m.enableUseCharacterSearching(), m.selfServeSearchScreenState()
    // ComboIndex 0 =  A-Z Categories with Best Sellers
    // ComboIndex 1 = Character by Character
    // ComboIndex 2 = None (hide Search button)

    // compatiability
    int state = 0;

    if( enable_chars )
        state = 1;
    else if( 1 == start_state )
        state = 0;
    else if( 0 == start_state )
    {
        // USE CASE
        // If we got clear to here, that means NOTHING was set. so by default, set the start state to None
        normalScaleOperation_->selfServeSearchScreenStateRaw() = "1";
        scaleConfigData_->selfServeSearchScreenStateRaw() = "1";
    }
    else if( 2 == start_state )
        state = 2;

    if( enable_chars && ( 1 == start_state ) )
    {
        // USE CASE
        // if start_state is equal to 1, this means the customer has changed it on this screen or they
        // have started sending data down to the scale with new tag, S6, setting the search screen to A-Z.
        // The problem is, the customer continues to send legacy configurations to the scale thru COMM
        // or use legacy configs in a backup. For example, the customer has Char by Char tag enabled but enabled A-Z in the scale turned on in their COMM
        // file. The new config/tag, since it HAS been set or sent down, takes precedence over the older ones.
        // so lets reset the legacy configs so we can help eliminate conflicting issues

        normalScaleOperation_->enableUseCharacterSearchingRaw() = "0";
        scaleConfigData_->enableUseCharacterSearchingRaw() = "0";
    }

    ui->cboSearchHow->setCurrentIndex( state );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::updateSelfServPrintState(int ignore_option, bool barcode_option, int start_state)
{
    Q_UNUSED( start_state )

    // this method is called when we receive a configuration message from the Application. This is NOT a save.


    // compatiability
    int state = 0;

    if( barcode_option && ignore_option ) //PJR413 (SOF-4772) - If both are true, set to QR Code only state
    {
        state = 4;
    }
    else if( barcode_option )
    {
        state = 3;
    }
    else if( ignore_option <= 2 )
    {
        // Print, scan, or scan & print
        state = ignore_option;
    }
    else
    {
        // USE CASE
        // If we got clear to here, that means NOTHING was set. so by default, set the start state to Print
        normalScaleOperation_->selfServePrintScreenStateRaw() = "0";
        scaleConfigData_->selfServePrintScreenStateRaw() = "0";
    }

    if( ( 1 == state ) || ( 2 == state ) )
        ui->chkSelfServeDisplayPluLabel->setVisible( true );
    else
        ui->chkSelfServeDisplayPluLabel->setVisible( false );

    ui->cboPrintWhat->setCurrentIndex( state );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::updateSelfServFreshServOptions( const SessionMessage& msg )
{
    DomNormalScaleOperation m( msg );

    ui->cboFlashkeyType_0->setCurrentIndex( m.selfServeFreshServeFlashkeyType0().toInt() );
    ui->cboFlashkeyType_1->setCurrentIndex( m.selfServeFreshServeFlashkeyType1().toInt() );
    ui->cboFlashkeyType_2->setCurrentIndex( m.selfServeFreshServeFlashkeyType2().toInt() );
    ui->cboFlashkeyType_3->setCurrentIndex( m.selfServeFreshServeFlashkeyType3().toInt() );
    ui->cboFlashkeyType_4->setCurrentIndex( m.selfServeFreshServeFlashkeyType4().toInt() );

    // always set best sellers group to this group and disable it. dont even ask dont let them change it.
    // May need to add additional protections in case application has a non 888050 value
    if( 1 == m.selfServeFreshServeFlashkeyType0().toInt() )
    {
        ui->txtFlashkeyNum_0->setText( QString::number( hobart::softkeysWidget::FreshServeFixedButtons ) );
        ui->txtFlashkeyNum_0->setEnabled( false );
    }
    else
    {
        ui->txtFlashkeyNum_0->setText( m.selfServeFreshServeFlashkeyNum0() );
    }

    if( 1 == m.selfServeFreshServeFlashkeyType1().toInt() )
    {
        ui->txtFlashkeyNum_1->setText( QString::number( hobart::softkeysWidget::FreshServeFixedButtons ) );
        ui->txtFlashkeyNum_1->setEnabled( false );
    }
    else
    {
        ui->txtFlashkeyNum_1->setText( m.selfServeFreshServeFlashkeyNum1() );
    }

    if(1 == m.selfServeFreshServeFlashkeyType2().toInt() )
    {
        ui->txtFlashkeyNum_2->setText( QString::number( hobart::softkeysWidget::FreshServeFixedButtons ) );
        ui->txtFlashkeyNum_2->setEnabled( false );
    }
    else
    {
        ui->txtFlashkeyNum_2->setText( m.selfServeFreshServeFlashkeyNum2() );
    }

    if( 1 == m.selfServeFreshServeFlashkeyType3().toInt() )
    {
        ui->txtFlashkeyNum_3->setText( QString::number( hobart::softkeysWidget::FreshServeFixedButtons ) );
        ui->txtFlashkeyNum_3->setEnabled( false );
    }
    else
    {
        ui->txtFlashkeyNum_3->setText( m.selfServeFreshServeFlashkeyNum3() );
    }

    if( 1 == m.selfServeFreshServeFlashkeyType4().toInt() )
    {
        ui->txtFlashkeyNum_4->setText( QString::number( hobart::softkeysWidget::FreshServeFixedButtons ) );
        ui->txtFlashkeyNum_4->setEnabled( false );
    }
    else
    {
        ui->txtFlashkeyNum_4->setText( m.selfServeFreshServeFlashkeyNum4() );
    }

    ui->txtFlashkeyText_0->setText( m.selfServeFreshServeFlashkeyText0() );
    ui->txtFlashkeyText_1->setText( m.selfServeFreshServeFlashkeyText1() );
    ui->txtFlashkeyText_2->setText( m.selfServeFreshServeFlashkeyText2() );
    ui->txtFlashkeyText_3->setText( m.selfServeFreshServeFlashkeyText3() );
    ui->txtFlashkeyText_4->setText( m.selfServeFreshServeFlashkeyText4() );

    ui->txtFlashkeySecText_0->setText( m.selfServeFreshServeFlashkeySecText0() );
    ui->txtFlashkeySecText_1->setText( m.selfServeFreshServeFlashkeySecText1() );
    ui->txtFlashkeySecText_2->setText( m.selfServeFreshServeFlashkeySecText2() );
    ui->txtFlashkeySecText_3->setText( m.selfServeFreshServeFlashkeySecText3() );
    ui->txtFlashkeySecText_4->setText( m.selfServeFreshServeFlashkeySecText4() );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::updateSelfServPrintingState(int start_state)
{
    // this method is called when we receive a configuration message from the Application. This is NOT a save.

    ui->cboPrintingWhen->setCurrentIndex( start_state );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::saveSelfServeStartScreen(int state)
{
    // these are all mutually exclusive
    // 0 = No Start Screen
    // 1 = Green button
    // 2 = weight required (3 images)
    // 3 = playlists
    // 4 = Log into self service

    // always true
    normalScaleOperation_->selfServeCombineSearchKeypadFlashkeysRaw()   = "1";

    // This will save the new configuration value to the old config values
    if(  0 == state )                                                               // NONE
    {
        normalScaleOperation_->selfServeStartScreenStateRaw() = "1";                // so if NONE selected, the StartScreenState is 1.
        scaleConfigData_->selfServeStartScreenStateRaw() = "1";

        normalScaleOperation_->displaySelfServeStartScreenRaw() = "0";              // so false to start with either Big Green Go
        scaleConfigData_->displaySelfServeStartScreenRaw() = "0";

        normalScaleOperation_->selfServeWeightRequiredToSelectItemRaw() = "0";      // weight required
        scaleConfigData_->selfServeWeightRequiredToSelectItemRaw() = "0";

        normalScaleOperation_->selfServeEnablePlaylistsRaw() = "0";                    // playlists
        scaleConfigData_->selfServeEnablePlaylistsRaw() = "0";
    }
    else if( 1 == state )                                                           // BIG GREEN GO
    {
        normalScaleOperation_->selfServeStartScreenStateRaw() = "0";                // so if GREEN GO selected, the StartScreenState is 0.
        scaleConfigData_->selfServeStartScreenStateRaw() = "0";

        normalScaleOperation_->displaySelfServeStartScreenRaw() = "1";              // so TRUE to start with either Big Green Go
        scaleConfigData_->displaySelfServeStartScreenRaw() = "1";

        normalScaleOperation_->selfServeWeightRequiredToSelectItemRaw() = "0";      // weight required
        scaleConfigData_->selfServeWeightRequiredToSelectItemRaw() = "0";

        normalScaleOperation_->selfServeEnablePlaylistsRaw() = "0";                    // playlists
        scaleConfigData_->selfServeEnablePlaylistsRaw() = "0";
    }
    else if( 2 == state )                                                           // WEIGHT REQUIRED
    {
        normalScaleOperation_->selfServeStartScreenStateRaw() = "0";                // WtReq - doesnt use this (it prolly should in case new people
        scaleConfigData_->selfServeStartScreenStateRaw() = "0";                     // WtReq - want to use it and still support WtReq Config)

        normalScaleOperation_->displaySelfServeStartScreenRaw() = "0";              // so false to start with either Big Green Go
        scaleConfigData_->displaySelfServeStartScreenRaw() = "0";

        normalScaleOperation_->selfServeWeightRequiredToSelectItemRaw() = "1";      // weight required
        scaleConfigData_->selfServeWeightRequiredToSelectItemRaw() = "1";

        normalScaleOperation_->selfServeEnablePlaylistsRaw() = "0";                    // playlists
        scaleConfigData_->selfServeEnablePlaylistsRaw() = "0";
    }
    else if( 3 == state )                                                           // PLAYLISTS
    {
        normalScaleOperation_->selfServeStartScreenStateRaw() = "0";                // Playlists - doesnt use this (it prolly should in case new people
        scaleConfigData_->selfServeStartScreenStateRaw() = "0";                     // Playlists - want to use it and still support playlist Config)

        normalScaleOperation_->displaySelfServeStartScreenRaw() = "0";              // so false to start with either Big Green Go
        scaleConfigData_->displaySelfServeStartScreenRaw() = "0";

        normalScaleOperation_->selfServeWeightRequiredToSelectItemRaw() = "0";      // weight required
        scaleConfigData_->selfServeWeightRequiredToSelectItemRaw() = "0";

        normalScaleOperation_->selfServeEnablePlaylistsRaw() = "1";                    // playlists
        scaleConfigData_->selfServeEnablePlaylistsRaw() = "1";
    }
    else if( 4 == state )                                                           // SELFSERVICE LOGIN - new Dec 2021
    {
        normalScaleOperation_->selfServeStartScreenStateRaw() = "4";                // Should maybe make this 4
        scaleConfigData_->selfServeStartScreenStateRaw() = "4";

        normalScaleOperation_->displaySelfServeStartScreenRaw() = "0";              // // so false to start with either Big Green Go
        scaleConfigData_->displaySelfServeStartScreenRaw() = "0";

        normalScaleOperation_->selfServeWeightRequiredToSelectItemRaw() = "0";      // weight required
        scaleConfigData_->selfServeWeightRequiredToSelectItemRaw() = "0";

        normalScaleOperation_->selfServeEnablePlaylistsRaw() = "0";                 // playlists
        scaleConfigData_->selfServeEnablePlaylistsRaw() = "0";
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::saveSelfServeItemEntryScreen(int state)
{
    // 0 = category flashkeys - implied state if best sellers turned off, or if certain features checked which means tied to a specific customer
    // 1 = best sellers checked and should displayed. Also tied to specific customers
    // 2 = numpad - existing default state. It's the default if not tied to a customer and if best sellers not turned on. Now a state
    // 3 = Fixed Flashkeys
    // 4 = Keypad Only

    if( 0 == state )
    {
        normalScaleOperation_->selfServeItemEntryScreenStateRaw()       = "1";          // new state config
        scaleConfigData_->selfServeItemEntryScreenStateRaw()            = "1";

        normalScaleOperation_->bestSellersOnSelfServeMainScreenRaw()    = "0";          // show best sellers
        scaleConfigData_->bestSellersOnSelfServeMainScreenRaw()         = "0";
    }
    else if( 1 == state )
    {
        normalScaleOperation_->selfServeItemEntryScreenStateRaw()       = "0";          // new state
        scaleConfigData_->selfServeItemEntryScreenStateRaw()            = "0";

        normalScaleOperation_->bestSellersOnSelfServeMainScreenRaw()    = "1";          // show best sellers
        scaleConfigData_->bestSellersOnSelfServeMainScreenRaw()         = "1";   
    }
    else if( 2 == state )
    {
        normalScaleOperation_->selfServeItemEntryScreenStateRaw()       = "2";          // display the Categories, also show a Keypad button in the bottom button bar
        scaleConfigData_->selfServeItemEntryScreenStateRaw()            = "2";

        normalScaleOperation_->bestSellersOnSelfServeMainScreenRaw()    = "0";          // show best sellers
        scaleConfigData_->bestSellersOnSelfServeMainScreenRaw()         = "0";
    }
    else if( 3 == state )
    {
        normalScaleOperation_->selfServeItemEntryScreenStateRaw()       = "3";          // PJR-383: Fixed Flashkeys
        scaleConfigData_->selfServeItemEntryScreenStateRaw()            = "3";

        normalScaleOperation_->bestSellersOnSelfServeMainScreenRaw()    = "0";          // show best sellers
        scaleConfigData_->bestSellersOnSelfServeMainScreenRaw()         = "0";
    }
    else if( 4 == state )
    {
        normalScaleOperation_->selfServeItemEntryScreenStateRaw()       = "4";          // display keypad
        scaleConfigData_->selfServeItemEntryScreenStateRaw()            = "4";

        normalScaleOperation_->bestSellersOnSelfServeMainScreenRaw()    = "0";          // show best sellers
        scaleConfigData_->bestSellersOnSelfServeMainScreenRaw()         = "0";
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::saveSelfServeSearchScreen(int state)
{
    // comboIndex 0 = A-Z category flashkeys (Also displays Best Sellers)
    // comboIndex 1 = Character by Character
    // comboIndex 2 = None

    // Yea - I'm starting to see I should maybe be using ItemData instead of currentIndex for these combos.
    // But the implementation was changed after first pass and so this is what I got until I want to change
    // a lot of stuff I've already set up

    if( 0 == state )
    {
        // A-Z so enableUseCharacter=False so SearchScreenState=1
        normalScaleOperation_->selfServeSearchScreenStateRaw()     = "1";           // new state config
        scaleConfigData_->selfServeSearchScreenStateRaw()          = "1";

        normalScaleOperation_->enableUseCharacterSearchingRaw()    = "0";           // show char by char
        scaleConfigData_->enableUseCharacterSearchingRaw()         = "0";
    }
    else if( 1 == state )
    {
        // Character by Character so enableUseCharacter=True(1) so SearchScreenState=false(0)
        normalScaleOperation_->selfServeSearchScreenStateRaw()     = "0";           // new state
        scaleConfigData_->selfServeSearchScreenStateRaw()          = "0";

        normalScaleOperation_->enableUseCharacterSearchingRaw()    = "1";           // show character by character
        scaleConfigData_->enableUseCharacterSearchingRaw()         = "1";
    }
    else if( 2 == state )
    {
        // None so enableUseCharacter=False(0) so SearchScreenState=2
        normalScaleOperation_->selfServeSearchScreenStateRaw()     = "2";           // new state
        scaleConfigData_->selfServeSearchScreenStateRaw()          = "2";

        normalScaleOperation_->enableUseCharacterSearchingRaw()    = "0";           // show character by character
        scaleConfigData_->enableUseCharacterSearchingRaw()         = "0";
    }

}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::saveSelfServePrintScreen(int state)
{
    // 0 = Print Label (regular ole default)
    // 1 = Print Label to Screen (ignore errors) - This has become Amazon Go
    // 2 = Print Label to Screen and Print for real
    // 3 = Put Barcode and Print Key on screen at the same time
    // 4 = Display QR Code on screen but no Print Key

    // always true
    normalScaleOperation_->selfServeCombineSearchKeypadFlashkeysRaw()   = "1";

    // always true now
    normalScaleOperation_->selfServeDisplayLabelOnScreenGORaw()       = "1";          // 'Print label to screen' AmazonGo (worse name ever for a var) and same option as scan label
    scaleConfigData_->selfServeDisplayLabelOnScreenGORaw()            = "1";          // AmazonGo (worse name ever for a var) and same option as scan label

    if( 0 == state )
    {                                                                                     // [X] Print label to Screen
        normalScaleOperation_->selfServeDisplayBarcodeLabelRaw()          = "0";          // DO NOT display the barcode ONLY and print
        scaleConfigData_->selfServeDisplayBarcodeLabelRaw()               = "0";          // button on the screen at the same time

        normalScaleOperation_->selfServeIgnorePrinterErrorsRaw()          = "0";          // This was the combo box that said Print, Scan, Print and Scan
        scaleConfigData_->selfServeIgnorePrinterErrorsRaw()               = "0";
    }
    else if( 1 == state )
    {                                                                                     // [X] Scan Label
        normalScaleOperation_->selfServeDisplayBarcodeLabelRaw()          = "0";          // DO NOT display the barcode ONLY and print
        scaleConfigData_->selfServeDisplayBarcodeLabelRaw()               = "0";          // button on the screen at the same time

        normalScaleOperation_->selfServeIgnorePrinterErrorsRaw()          = "1";          // This was the combo box that said Print, Scan, Print and Scan
        scaleConfigData_->selfServeIgnorePrinterErrorsRaw()               = "1";
    }
    else if( 2 == state )
    {                                                                                     // [X] Scan and Print Label
        normalScaleOperation_->selfServeDisplayBarcodeLabelRaw()          = "0";          // DO NOT display the barcode ONLY and print
        scaleConfigData_->selfServeDisplayBarcodeLabelRaw()               = "0";          // button on the screen at the same time

        normalScaleOperation_->selfServeIgnorePrinterErrorsRaw()          = "2";          // This was the combo box that said Print, Scan, Print and Scan
        scaleConfigData_->selfServeIgnorePrinterErrorsRaw()               = "2";
    }
    else if( 3 == state)
    {                                                                                   // [X] Print Barcode and show print button together
        normalScaleOperation_->selfServeDisplayBarcodeLabelRaw()          = "1";          // display the barcode ONLY and print
        scaleConfigData_->selfServeDisplayBarcodeLabelRaw()               = "1";          // button on the screen at the same time

        normalScaleOperation_->selfServeIgnorePrinterErrorsRaw()          = "0";          // This was the combo box that said Print, Scan, Print and Scan
        scaleConfigData_->selfServeIgnorePrinterErrorsRaw()               = "0";          // make it be configured for Print Label
    }
    else if( 4 == state) // PJR413 (SOF-4772)
    {
        normalScaleOperation_->selfServeDisplayBarcodeLabelRaw()          = "1";          // display the barcode ONLY and print
        scaleConfigData_->selfServeDisplayBarcodeLabelRaw()               = "1";          // button on the screen at the same time

        normalScaleOperation_->selfServeIgnorePrinterErrorsRaw()          = "1";          // This needs to be 1 so no printing happens
        scaleConfigData_->selfServeIgnorePrinterErrorsRaw()               = "1";          //
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////
    // not really using this though
    normalScaleOperation_->selfServePrintScreenStateRaw()           = QString::number(state );
    scaleConfigData_->selfServePrintScreenStateRaw()                = QString::number(state );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::saveSelfServeAfterPrintScreen(int state)
{
    // 0 = play video
    // 1 = display image

    normalScaleOperation_->selfServeAfterPrintScreenStateRaw()    = QString::number(state );
    scaleConfigData_->selfServeAfterPrintScreenStateRaw()         = QString::number( state );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::saveSelfServeOptionsGen2Screen()
{
    //normalScaleOperation_->setCuttingTestType( ui->cboCuttingTestType->currentIndex() );

    normalScaleOperation_->selfServeFreshServeFlashkeyType0Raw()    = QString::number( ui->cboFlashkeyType_0->currentIndex() );
    scaleConfigData_->selfServeFreshServeFlashkeyType0Raw()         = QString::number( ui->cboFlashkeyType_0->currentIndex() );
    normalScaleOperation_->selfServeFreshServeFlashkeyType1Raw()    = QString::number( ui->cboFlashkeyType_1->currentIndex() );
    scaleConfigData_->selfServeFreshServeFlashkeyType1Raw()         = QString::number( ui->cboFlashkeyType_1->currentIndex() );
    normalScaleOperation_->selfServeFreshServeFlashkeyType2Raw()    = QString::number( ui->cboFlashkeyType_2->currentIndex() );
    scaleConfigData_->selfServeFreshServeFlashkeyType2Raw()         = QString::number( ui->cboFlashkeyType_2->currentIndex() );
    normalScaleOperation_->selfServeFreshServeFlashkeyType3Raw()    = QString::number( ui->cboFlashkeyType_3->currentIndex() );
    scaleConfigData_->selfServeFreshServeFlashkeyType3Raw()         = QString::number( ui->cboFlashkeyType_3->currentIndex() );
    normalScaleOperation_->selfServeFreshServeFlashkeyType4Raw()    = QString::number( ui->cboFlashkeyType_4->currentIndex() );
    scaleConfigData_->selfServeFreshServeFlashkeyType4Raw()         = QString::number( ui->cboFlashkeyType_4->currentIndex() );

    normalScaleOperation_->selfServeFreshServeFlashkeyText0Raw()    = ui->txtFlashkeyText_0->text();
    scaleConfigData_->selfServeFreshServeFlashkeyText0Raw()         = ui->txtFlashkeyText_0->text();
    normalScaleOperation_->selfServeFreshServeFlashkeyText1Raw()    = ui->txtFlashkeyText_1->text();
    scaleConfigData_->selfServeFreshServeFlashkeyText1Raw()         = ui->txtFlashkeyText_1->text();
    normalScaleOperation_->selfServeFreshServeFlashkeyText2Raw()    = ui->txtFlashkeyText_2->text();
    scaleConfigData_->selfServeFreshServeFlashkeyText2Raw()         = ui->txtFlashkeyText_2->text();
    normalScaleOperation_->selfServeFreshServeFlashkeyText3Raw()    = ui->txtFlashkeyText_3->text();
    scaleConfigData_->selfServeFreshServeFlashkeyText3Raw()         = ui->txtFlashkeyText_3->text();
    normalScaleOperation_->selfServeFreshServeFlashkeyText4Raw()    = ui->txtFlashkeyText_4->text();
    scaleConfigData_->selfServeFreshServeFlashkeyText4Raw()         = ui->txtFlashkeyText_4->text();

    normalScaleOperation_->selfServeFreshServeFlashkeySecText0Raw()    = ui->txtFlashkeySecText_0->text();
    scaleConfigData_->selfServeFreshServeFlashkeySecText0Raw()         = ui->txtFlashkeySecText_0->text();
    normalScaleOperation_->selfServeFreshServeFlashkeySecText1Raw()    = ui->txtFlashkeySecText_1->text();
    scaleConfigData_->selfServeFreshServeFlashkeySecText1Raw()         = ui->txtFlashkeySecText_1->text();
    normalScaleOperation_->selfServeFreshServeFlashkeySecText2Raw()    = ui->txtFlashkeySecText_2->text();
    scaleConfigData_->selfServeFreshServeFlashkeySecText2Raw()         = ui->txtFlashkeySecText_2->text();
    normalScaleOperation_->selfServeFreshServeFlashkeySecText3Raw()    = ui->txtFlashkeySecText_3->text();
    scaleConfigData_->selfServeFreshServeFlashkeySecText3Raw()         = ui->txtFlashkeySecText_3->text();
    normalScaleOperation_->selfServeFreshServeFlashkeySecText4Raw()    = ui->txtFlashkeySecText_4->text();
    scaleConfigData_->selfServeFreshServeFlashkeySecText4Raw()         = ui->txtFlashkeySecText_4->text();

    normalScaleOperation_->selfServeFreshServeFlashkeyNum0Raw()     = ui->txtFlashkeyNum_0->text();
    scaleConfigData_->selfServeFreshServeFlashkeyNum0Raw()          = ui->txtFlashkeyNum_0->text();
    normalScaleOperation_->selfServeFreshServeFlashkeyNum1Raw()     = ui->txtFlashkeyNum_1->text();
    scaleConfigData_->selfServeFreshServeFlashkeyNum1Raw()          = ui->txtFlashkeyNum_1->text();
    normalScaleOperation_->selfServeFreshServeFlashkeyNum2Raw()     = ui->txtFlashkeyNum_2->text();
    scaleConfigData_->selfServeFreshServeFlashkeyNum2Raw()          = ui->txtFlashkeyNum_2->text();
    normalScaleOperation_->selfServeFreshServeFlashkeyNum3Raw()     = ui->txtFlashkeyNum_3->text();
    scaleConfigData_->selfServeFreshServeFlashkeyNum3Raw()          = ui->txtFlashkeyNum_3->text();
    normalScaleOperation_->selfServeFreshServeFlashkeyNum4Raw()     = ui->txtFlashkeyNum_4->text();
    scaleConfigData_->selfServeFreshServeFlashkeyNum4Raw()          = ui->txtFlashkeyNum_4->text();

    normalScaleOperation_->selfServeFreshServeLabelOffsetRaw()      = ui->lblFreshServeLabelOffset->text();     // save on change
    scaleConfigData_->selfServeFreshServeLabelOffsetRaw()           = ui->lblFreshServeLabelOffset->text();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::enableDaysToKeepTransactions()
{
    bool isEnabled = false;

    if( ui->chkCollectTransactions->isChecked() )
        isEnabled = true;
    else if( ui->chkTrackOperChanges->isChecked() )
        isEnabled = true;
    else if( ui->chkTrackSuperChanges->isChecked() )
        isEnabled = true;

    setCollectTransactionsDays( isEnabled );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::changeAutoLookup(bool isEnabled)
{
    ui->lblAutoLookupDigits->setEnabled( isEnabled );
    ui->cboNumOfDigits->setEnabled( isEnabled );

    if ( !session_->isPrepack() )
    {
        ui->lblSpAutoLookupDigit->setEnabled( isEnabled );
        ui->cboSpAutoLookupDigit->setEnabled( isEnabled );
        ui->lblSpAutoLookupDigitX->setEnabled( isEnabled );
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::changeRemoteAccess(bool isEnabled)
{
    if( isEnabled )
    {
        ui->lblRestrictions->setEnabled( true );
        ui->cboRemoteAccessLoginMethod->setEnabled( true );
    }
    else
    {
        ui->lblRestrictions->setEnabled( false );
        ui->cboRemoteAccessLoginMethod->setEnabled( false );
    }
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::changeRemoteAccessLoginMethod( int loginMethod )
{
    if( loginMethod == 2 )
    {
        ui->lblPassword->setVisible( true );
        ui->lblUserName->setVisible( true);

        ui->txtRemoteAccessUserName->setVisible( true );
        ui->txtRemoteAccessPassword->setVisible( true );
    }
    else
    {
        ui->lblPassword->setVisible( false );
        ui->lblUserName->setVisible( false);

        ui->txtRemoteAccessUserName->setVisible( false );
        ui->txtRemoteAccessPassword->setVisible( false );
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::enableBrowserInPanel( int isEnabled )
{
    // HTs
    // this feature is disabled
    if ( !isLicensed_WebBrowser_ )
        return;

    if( 1 == isEnabled )
    {
        ui->txtDefaultHomePage->setEnabled( true );
        scaleConfigData_->operCanAccessBrowser() = "1";
    }
    else
    {
        ui->txtDefaultHomePage->setEnabled( false );
        scaleConfigData_->operCanAccessBrowser() = "0";
        //processEnabledMenusChangedBool("browserOn", "false");							// disables Browser Modde in Operate Mode menu
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::enableDesktopMenuItem(  bool isEnabled )
{
    Q_UNUSED( isEnabled );

    // WAIT - I thought we were NOT going to allow this. Have to check into this
    // This is an example of how it use to be done. Is there anything we want
    // to do here instead? Launch setup wizard?

    //if( isEnabled )
    //	processEnabledMenusChangedBool("desktopOn", "true");
    //else
    //	processEnabledMenusChangedBool("desktopOn", "false");
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::enableProdPlanSource( bool isEnabled )
{
    // HTs
    // this feature is disabled
    if ( !isLicensed_FPP_ )
        return;

    if( isEnabled )
    {
        ui->txtProdPlanningFrame->setEnabled( true );
        ui->txtProdPlanningUpdate->setEnabled( true );
        ui->lblProdPlanFrame->setEnabled( true );
        ui->lblProdPlanningUpdate->setEnabled( true );
    }
    else
    {
        ui->txtProdPlanningFrame->setEnabled( false );
        ui->txtProdPlanningUpdate->setEnabled( false );
        ui->lblProdPlanFrame->setEnabled( false );
        ui->lblProdPlanningUpdate->setEnabled( false );
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::setComboBoxIndex(QComboBox *widget, QString comboData)
{
    int len = widget->count();
    QString data = "";

    for(int j = 0; j < len; j++)
    {
        data = widget->itemData(j).toString();
        if(data == comboData)
        {
            widget->setCurrentIndex(j);
            break;
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::setFilmTensionComboBoxIndex(QComboBox *widget, QString comboData)
{
    int len = widget->count();      // for Film Tension, it better be 2.
    QString data = "";

    int combo_int = comboData.toInt();

    if( combo_int > 1 )
        combo_int = 1;              // so if above 1, make it 1
    else if( combo_int <= 0 )
        combo_int = 0;              // so if below 0, make it 0

    for(int j = 0; j < len; j++)
    {
        data = widget->itemData(j).toString();
        if(data.toInt() == combo_int)
        {
            widget->setCurrentIndex(j);
            break;
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::setPingTargetState()
{
    signalMapper_->blockSignals(true);								// FT-54 Network connectivity cannot be turned off, changes will not save
    bool doEnable = true;

    if ( ui->chkCheckNetConnectivity->isChecked() == false )
        doEnable = false;

    if (doEnable)
    {
        ui->chkPingTarget->setEnabled( true );

        ui->txtPingTarget1->setEnabled(true);
        ui->txtPingTarget2->setEnabled(true);
        ui->txtPingTarget3->setEnabled(true);
        ui->txtPingTarget4->setEnabled(true);
    }
    else
    {
        ui->chkPingTarget->setChecked( false);
        ui->chkPingTarget->setEnabled( false );
        ui->txtPingTarget1->setEnabled(false);
        ui->txtPingTarget2->setEnabled(false);
        ui->txtPingTarget3->setEnabled(false);
        ui->txtPingTarget4->setEnabled(false);
    }
    signalMapper_->blockSignals(false);								// FT-54 Network connectivity cannot be turned off, changes will not save
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::setPingIntervalState()
{
    bool doEnable = ui->chkCheckNetConnectivity->isChecked();
    signalMapper_->blockSignals(true);								// FT-54 Network connectivity cannot be turned off, changes will not save

    if( doEnable )
    {
        ui->txtPingHours->setEnabled( true );
        ui->txtPingMinutes->setEnabled( true );
    }
    else
    {
        ui->txtPingHours->setEnabled( false );
        ui->txtPingMinutes->setEnabled( false );
    }

    signalMapper_->blockSignals(false);								// FT-54 Network connectivity cannot be turned off, changes will not save
}

void SuperNormalScaleOperation::updatePingIntervalInfo()
{
    QString pingTime = normalScaleOperation_->pingInterval();

    if( pingTime == "" || pingTime == " ")
    {
        ui->txtPingHours->setText( "0" );
        ui->txtPingMinutes->setText( "0" );
    }
    else
    {
        // FT-764 Restore or Merge of Quantum database records. On the quantum if a time is in the
        // 12AM hour, and say for example, 30 minutes, they save it as just 30.  Where we would save
        // it in our DB as 0:30, which made our array/list fail. 1:30AM is also just 130 in a quantum

        QString hours = "0";
        QString mins =  "0";

        if(pingTime.contains(":") )
        {
            QStringList pingTimeList = pingTime.split(":");
            hours = pingTimeList.at(0);
            mins = pingTimeList.at(1);
        }
        else
        {
            // must of come fmom a quantum DB
            int len = pingTime.length();
            if( 2 == len )
            {
                mins = pingTime;
            }
            else if( 3 == len )
            {
                hours = pingTime.mid(0, 1);
                mins = pingTime.mid(1,2);
            }
            else if( 4 == len )
            {
                mins = pingTime.mid(2,2);
                hours = pingTime.mid(0, 2);

            }
        }

        int hoursInt = hours.toInt();
        int minsInt  = mins.toInt();

        if( hoursInt < 0 || hoursInt > 23 )
            ui->txtPingHours->setText( "0" );
        else
            ui->txtPingHours->setText( hours );

        if( minsInt < 0 || minsInt > 59 )
            ui->txtPingMinutes->setText( "0" );
        else
            ui->txtPingMinutes->setText( mins );

        if(ui->txtPingHours->text().toInt() == 0 && ui->txtPingMinutes->text().toInt() == 0)
        {
            ui->txtPingMinutes->setText("1");
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::setDomPingInterval()
{
    // build the timestring for ping interval out of the hours and minutes fields

    QString time = "";
    int hour = ui->txtPingHours->text().toInt();
    int minutes = ui->txtPingMinutes->text().toInt();

    if ( hour == 0 || ui->txtPingHours->text() == "")
        time = "00";
    else
    {
        if (hour < 10)
            time = "0";

        time = time.append( ui->txtPingHours->text() );
    }

    time = time.append(":");

    if ( minutes == 0 || ui->txtPingMinutes->text() == "" )
        time = time.append("00");
    else
        time = time.append( ui->txtPingMinutes->text() );

    if(time == "00:00") time = "00:01";
    normalScaleOperation_->pingInterval() = time;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::setDomPingTarget()
{
    // build the ping ip address out of the 4 ip address fields

    QString nib1 = "";
    QString nib2 = "";
    QString nib3 = "";
    QString nib4 = "";

    if (ui->txtPingTarget1->text() != " ")
        nib1 = ui->txtPingTarget1->text();

    if (ui->txtPingTarget2->text() != " ")
        nib2 = ui->txtPingTarget2->text();

    if (ui->txtPingTarget3->text() != " ")
        nib3 = ui->txtPingTarget3->text();

    if (ui->txtPingTarget4->text() != " ")
        nib4 = ui->txtPingTarget4->text();

    QString ipStr = nib1.append(".").append(nib2).append(".").append(nib3).append(".").append(nib4);
    normalScaleOperation_->pingTargetIPAddress() = ipStr;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::setDomNowServingServer()
{
    QString nib1 = "";
    QString nib2 = "";
    QString nib3 = "";
    QString nib4 = "";

    if (ui->txtNowServing1->text() != " ")
        nib1 = ui->txtNowServing1->text();

    if (ui->txtNowServing2->text() != " ")
        nib2 = ui->txtNowServing2->text();

    if (ui->txtNowServing3->text() != " ")
        nib3 = ui->txtNowServing3->text();

    if (ui->txtNowServing4->text() != " ")
        nib4 = ui->txtNowServing4->text();

    QString ipStr = nib1.append(".").append(nib2).append(".").append(nib3).append(".").append(nib4);
    normalScaleOperation_->nowServingQueueIPAddress() = ipStr;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
QString SuperNormalScaleOperation::getPrimaryCountryCode( int index )
{
    switch( index )
    {
        case 0:
            return "US";
            break;
        case 1:
            return "MX";
            break;
        case 2:
            return "CA";
            break;
        case 3:
            return "CA";
            break;
    }

    return "";
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
QString SuperNormalScaleOperation::getPrimaryLanguageCode( int index )
{
    switch( index )
    {
        case 0:
            return "en";
            break;
        case 1:
            return "es";
            break;
        case 2:
            return "en";
            break;
        case 3:
            return "fr";
            break;
    }

    return "";
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
QString SuperNormalScaleOperation::getSecondaryCountryCode( int index )
{
    switch( index )
    {
        case 0:
            return "";
            break;
        case 1:
            return "US";
            break;
        case 2:
            return "MX";
            break;
        case 3:
            return "CA";
            break;
        case 4:
            return "CA";
            break;
    }

    return "";
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
QString SuperNormalScaleOperation::getSecondaryLanguageCode( int index )
{
    switch( index )
    {
        case 0:
            return "";
            break;
        case 1:
            return "en";
            break;
        case 2:
            return "es";
            break;
        case 3:
            return "en";
            break;
        case 4:
            return "fr";
            break;
    }

    return "";
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::checkEnabledEanFields(bool isEnabled)
{
    if ( ui->chkEnableCloudFeature->isChecked())
        return; // don't bother checking these fields

    ui->chkEnableCloudFeature->setEnabled( isEnabled );
    ui->chkEnableItemStripMode->setEnabled( isEnabled );
    ui->chkAccumItemsReturnToStartScreen->setEnabled( isEnabled );
    ui->chkReadTransactionsCompatibility->setEnabled( isEnabled );
    ui->chkBarcodeUseFirstItem->setEnabled( isEnabled );
    ui->chkEnableAccumItemsAndFlashkeys->setEnabled( isEnabled );

    if( isEnabled )
    {
        if( ui->chkEnableItemStripMode->isChecked() )
        {
            ui->chkEnableAccumItemsTotalsOnly->setChecked( false );
            ui->chkEnableAccumItemsTotalsOnly->setEnabled( false );
        }
        else
            ui->chkEnableAccumItemsTotalsOnly->setEnabled( true );

        ui->butAccumItemsEANFormatString->setEnabled( true );
        ui->lblEanFormatString->setEnabled( true );

        QString eanStr = ui->butAccumItemsEANFormatString->text();
        int eanStrLen = eanStr.length();

        if( eanStrLen == 0 )
        {
            ui->txtAccumItemsEANPrefix->setEnabled( false );
            ui->txtAccumItemsEANCode->setEnabled( false );
            ui->txtAccumItemsScaleId->setEnabled( false );
            ui->txtAccumItemsTransNum->setEnabled( false );

            ui->lblEanPrefix->setEnabled( false );
            ui->lblEanCode->setEnabled( false );
            ui->lblScaleId->setEnabled( false );
            ui->lblTransNum->setEnabled( false );
        }
        else
        {
            int digits =  eanStr.count("X");                        // how many X's in EAN String?
            if(digits == 0)
            {
                ui->txtAccumItemsEANPrefix->setEnabled( false );
                ui->lblEanPrefix->setEnabled( false );
            }
            else
            {
                ui->txtAccumItemsEANPrefix->setEnabled( true );
                ui->lblEanPrefix->setEnabled( true );
            }

            digits = eanStr.count("E");                             // how many E's in EAN String?
            if(digits  == 0)
            {
                ui->txtAccumItemsEANCode->setEnabled( false );
                ui->lblEanCode->setEnabled( false );
            }
            else
            {
                ui->txtAccumItemsEANCode->setEnabled( true );
                ui->lblEanCode->setEnabled( true );
            }

            digits = eanStr.count("S");                             // how many S's in EAN String?
            if(digits  == 0)
            {
                ui->txtAccumItemsScaleId->setEnabled( false );
                ui->lblScaleId->setEnabled( false );
            }
            else
            {
                ui->txtAccumItemsScaleId->setEnabled( true );
                ui->lblScaleId->setEnabled( true );
            }

            digits = eanStr.count("T");                             // how many T's in EAN String?
            if(digits  == 0)
            {
                ui->txtAccumItemsTransNum->setEnabled( false );
                ui->lblTransNum->setEnabled( false );
            }
            else
            {
                ui->txtAccumItemsTransNum->setEnabled( true );
                ui->lblTransNum->setEnabled( true );
            }
        }
    }
    else
    {
        ui->chkEnableAccumItemsTotalsOnly->setEnabled( false );
        ui->butAccumItemsEANFormatString->setEnabled( false );
        ui->lblEanFormatString->setEnabled( false );
        ui->txtAccumItemsEANPrefix->setEnabled( false );
        ui->lblEanPrefix->setEnabled( false );
        ui->txtAccumItemsEANCode->setEnabled( false );
        ui->lblEanCode->setEnabled( false );
        ui->txtAccumItemsScaleId->setEnabled( false );
        ui->lblScaleId->setEnabled( false );
        ui->txtAccumItemsTransNum->setEnabled( false );
        ui->lblTransNum->setEnabled( false );
        ui->chkEnableAccumItemsAndFlashkeys->setEnabled( false );
    }
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                                          SLOT METHODS
//
//////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::checkboxChanged(int state)
{
    Q_UNUSED( state );

    if( isUpdating )
        return;

    if( session_->isPrepack() )
    {
        if( currentTab == ui->tabNormalScaleOperation->indexOf( ui->tab5 ) ) // Features tab
        {
            // weigherTabOffset = ui->testTabsWidget->indexOf( ui->weigherTab );
            // tabSubFeatures
            int wrapperSettingsTabOffset = ui->tabSubFeatures->indexOf( ui->tab5_5 ); // Wrapper Settings
            int currentTabOffset = ui->tabSubFeatures->currentIndex();

            if( wrapperSettingsTabOffset == currentTabOffset )
            {
                operatorDisplayFields_->enableFilmWidthRaw() = ui->chk_displayFilmWidth->isChecked() == true ? "1" : "0";
                operatorDisplayFields_->enableFilmTensionRaw() = ui->chk_displayFilmTension->isChecked() == true ? "1" : "0";

                hasChangedDisplayOptions = true;
            }
        }
    }

    if ( sender() == ui->chkAutoDisplaySuggestiveSelling )
    {
        ui->lblSuggSellFreq->setEnabled( ui->chkAutoDisplaySuggestiveSelling->isChecked() );
        ui->txtSuggSellFreq->setEnabled( ui->chkAutoDisplaySuggestiveSelling->isChecked() );

        ui->cboSuggSellStyle->setEnabled( ui->chkAutoDisplaySuggestiveSelling->isChecked() );
        ui->lblSuggSellStyle->setEnabled( ui->chkAutoDisplaySuggestiveSelling->isChecked() );
    }
    else if( sender() == ui->chkAutoUpdateUPCBarcodeOver99 )
    {
        bool isItChecked = ui->chkAutoUpdateUPCBarcodeOver99->isChecked();
        ui->cboBarcodeTypeOver99->setEnabled( isItChecked );

        int upc99 = normalScaleOperation_->barcodeTypeOver99();
        int upcin = ui->cboBarcodeTypeOver99->findData( upc99 );

        ui->cboBarcodeTypeOver99->setCurrentIndex( upcin );

        if( ( isItChecked ) && ( upc99 <= 0 ) )
            ui->cboBarcodeTypeOver99->setCurrentIndex( 0 );
        else if( !isItChecked )
            ui->cboBarcodeTypeOver99->setCurrentIndex( 0 );
    }
    else if( sender() == ui->chkHelpPrompts )
    {
        bool isItChecked = ui->chkHelpPrompts->isChecked();
        if( isItChecked )
        {
            ui->cboPrintWhat->setCurrentIndex( 0 );
            ui->cboPrintWhat->setEnabled( false );
        }
        else
        {
            ui->cboPrintWhat->setEnabled( true );
        }
        scaleConfigData_->enableHelpPromptsRaw()            = ui->chkHelpPrompts->isChecked() == true ? "1" : "0";
        normalScaleOperation_->enableHelpPromptsRaw()       = ui->chkHelpPrompts->isChecked() == true ? "1" : "0";
    }

    hasChanged = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::checkboxChangedDisplayOptions(int state)
{
    Q_UNUSED( state );

    if ( isUpdating )
        return;

    hasChangedDisplayOptions = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::checkboxChangedSelfServGenUIs(int state)
{
    Q_UNUSED( state );

    if ( isUpdating )
        return;

    hasChangedDisplayOptions = true;

    if( 0 == state )
    {
        ui->mainWidget->setVisible( true );
        ui->stackSelfServFS->setCurrentIndex( 0 );
    }
    else
    {
        ui->mainWidget->setVisible( false );
        ui->stackSelfServFS->setCurrentIndex( 1 );

        // Once enabled - these options become active
        ui->cboStartWhere->setCurrentIndex( 2 );
        ui->cboItemEntryWho->setCurrentIndex( 3 );
        ui->cboSearchHow->setCurrentIndex( 1 );
        ui->cboPrintWhat->setCurrentIndex( 3 );
        ui->chkStartScreenClickable->setChecked(true);
        ui->chkIncludeNumpad->setChecked( true );
        ui->txtSelfServeFixedPlu->setText(0);
        ui->chkSelfServeByCountItemsAllowed->setChecked( true );
        ui->chkSelfServeTareOnRandomWeight->setChecked( false );
        ui->chkHelpPrompts->setChecked( false );
        ui->chkSelfServeHideButtonBar->setChecked( false );

        // turn on transactions
        ui->chkCollectTransactions->setChecked( true );
        ui->lblTransDays->setEnabled( true );
        ui->cboDaysToKeepTransactions->setEnabled( true );

        // set some default flashkey properties
        DomSoftkeyGroup softkeyGroup;

        // Send: 136,86,999020,6,3,10,10
        softkeyGroup.device() = QString::number( SoftkeyDev );
        softkeyGroup.event() = QString::number( SaveSoftkeyGroup );
        softkeyGroup.setGroupNum( hobart::softkeysWidget::FreshServeFixedButtons );
        softkeyGroup.setHorizGridSize( 5 );
        softkeyGroup.setVertGridSize( 3 );
        softkeyGroup.setHorizSpacing( 1 );
        softkeyGroup.setVertSpacing( 1 );
        session_->send( softkeyGroup );

        // Send: 136,86,999017,7,2,10,10
        softkeyGroup.device() = QString::number( SoftkeyDev );
        softkeyGroup.event() = QString::number( SaveSoftkeyGroup );
        softkeyGroup.setGroupNum( hobart::softkeysWidget::SelfservCategoryResultsButtons );
        softkeyGroup.setHorizGridSize( 7 );
        softkeyGroup.setVertGridSize( 1 );
        softkeyGroup.setHorizSpacing( 1 );
        softkeyGroup.setVertSpacing( 1 );

        // (136,86)
        session_->send( softkeyGroup );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::comboboxChangedDisplayOptions(int newIndex)
{
    Q_UNUSED( newIndex );

    if ( isUpdating )
        return;

    hasChangedDisplayOptions = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::checkboxChangedCollectTrans(int state)
{
    if( isUpdating )
        return;

    isUpdating = true;
    hasChanged = true;

    // if we disable Collect Transactions and Cool Totals, Tracking Operator
    // Changes gets turned off too. Not sure if I really understand why though.
    if( state < 1 )
        ui->chkTrackOperChanges->setChecked( false );

    // see if anything is still enabled to enable or disable days to keep transactions
    enableDaysToKeepTransactions();

    // SOF-43
    ui->lblTransHoursToView->setEnabled( ( state > 0 ? true : false ) );
    ui->txtTransHoursToView->setEnabled( ( state > 0 ? true : false ) );

    isUpdating = false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::checkboxChangedTrackOperators(int state)
{
    if( isUpdating )
        return;

    // if we check Operator Changes, then we automatically check collect trans and cool Totals
    if(state > 0)
    {
        // Collect Trans and Cool Totals slot gets fired automatically when set
        ui->chkCollectTransactions->setChecked( true );
    }

    // see if anything is still enabled to enable or disable days to keep transactions
    enableDaysToKeepTransactions();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::checkboxChangedTrackSuperOperators(int state)
{
    Q_UNUSED( state );

    if( isUpdating )
        return;

    // see if anything is still enabled to enable or disable days to keep transactions
    enableDaysToKeepTransactions();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// SOF-43
void SuperNormalScaleOperation::comboboxDaysToKeepTrans( int newIndex )
{
    Q_UNUSED( newIndex )

    if( isUpdating )
        return;

    setupHoursToViewRecordsValidator( ui->cboDaysToKeepTransactions->itemData( ui->cboDaysToKeepTransactions->currentIndex()).toInt() );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::checkboxChangedAutoLookup(int state)
{
    if( isUpdating )
        return;

    hasChanged = true;
    bool isEnabled = state > 1 ? true : false;
    changeAutoLookup( isEnabled );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::comboboxChangedNumOfDigits(int newIndex)
{
    int value( ui->cboNumOfDigits->itemData( newIndex ).toInt() );
    QString x;

    if ( !session_->isPrepack() )
    {
        while ( value-- )
            x.append( "X" );

        ui->lblSpAutoLookupDigitX->setText( x );
    }

    if( isUpdating )
        return;

    hasChanged = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::checkboxChangedRemoteAccess(int state)
{
    if( isUpdating )
        return;

    hasChanged = true;
    bool isEnabled = state > 1 ? true : false;
    changeRemoteAccess( isEnabled );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::checkboxChangedDisableOpFlashkeyEditor(int state)
{
    if( isUpdating )
        return;

    hasChanged = true;
    bool isDisabled = state > 1 ? true : false;
    normalScaleOperation_->disableOperatorFlashkeyEditorRaw() = isDisabled ? "1" : "0";
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::checkboxChangedDisableLeftPanel(int state)
{
    if( isUpdating )
        return;

    hasChanged = true;
    bool isDisabled = state > 1 ? true : false;
    normalScaleOperation_->disableStartScreenRaw() = isDisabled ? "1" : "0";
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::checkboxChangedEnableTransType(int state)
{
    if( isUpdating )
        return;

    hasChanged = true;
    bool isDisabled = state > 1 ? true : false;
    normalScaleOperation_->showTotalTypeOnRunScreenRaw() = isDisabled ? "1" : "0";
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::checkboxChangedRemoveAddressBar( int state)
{
    Q_UNUSED( state );

    if( isUpdating )
        return;

    hasChanged = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::checkboxChangedProdPlan(int state)
{
    if( isUpdating )
        return;

    hasChangedDisplayOptions = true;
    bool isEnabled = state > 1 ? true : false;
    enableProdPlanSource( isEnabled );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::checkboxChangedUpdateDelete(int state)
{
    if( isUpdating )
        return;

    scaleConfigData_->showDeleteAllFilesRaw() = ( state == 0 ? "0" : "1" );
    hasChanged = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::checkboxChangedUpdateSearch(int state)
{
    if( isUpdating )
        return;

    scaleConfigData_->enableSearchMenuButtonRaw() = ( state == 0 ? "0" : "1" );
    hasChanged = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::checkboxChangedUpdateSearchOnKeypad(int state)
{
    if( isUpdating )
        return;

    scaleConfigData_->enableSearchOnKeypadRaw() = ( state == 0 ? "0" : "1" );
    hasChanged = true;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::checkboxChangedUpdatePrinter(int state)
{
    if( isUpdating )
        return;

    scaleConfigData_->showPrinterSettingsRaw() = ( state == 0 ? "0" : "1" );
    hasChanged = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::checkboxChangedUpdateProdEntry(int state)
{
    if( isUpdating )
        return;

    operatorDisplayFields_->enableProdEntryRaw() = ( state == 0 ? "0" : "1" );
    hasChangedDisplayOptions = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::checkboxChangedUpdateAccumulate(int state)
{
    if( isUpdating )
        return;

    // enable barcode cloud feature is checked, but they unchecked enable accumulate items
    if(state == 0 && ui->chkEnableCloudFeature->isChecked())
        checkboxChangedEnableCloudFeature(2);

    // re-enabled accum items, and cloud feature is checked
    else if (state == 2 && ui->chkEnableCloudFeature->isChecked())
        checkboxChangedEnableCloudFeature(1);

    else
        checkEnabledEanFields( state );

    tabAccumHasChanged = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::checkboxChangedAccumulate(int state)
{
    Q_UNUSED( state );

    if( isUpdating )
        return;

    tabAccumHasChanged = true;
}

// SOF-5386: PJR-454
void SuperNormalScaleOperation::checkboxChangedEnableCloudFeature(int state)
{
    if( isUpdating )
        return;

    bool enabled = true;
    bool visible = true;

    // if unchecked re-show ordinary barcode stuff
    if (state == 0)
    {
        // disable and hide all the widgets associated with barcode cloud feature
        ui->txtAccumItemsServerAddress->setVisible( false );
        ui->lblServerStatus->setVisible( false );
        ui->lblServerAddress->setVisible( false );
        ui->txtAccumItemsServerAddress->setEnabled(false);
        ui->testServerStatus->setVisible( false );
        ui->testServerStatus->setEnabled( false );
        ui->lblServerStatusTxt->setVisible( false );
    }
    // enable barcode cloud feature is checked, but they unchecked enable accumulate items
    else if (state == 2 && !ui->chkEnableAccumItems->isChecked())
    {
        ui->chkEnableCloudFeature->setEnabled( false );
        ui->lblServerAddress->setEnabled( false );
        ui->txtAccumItemsServerAddress->setEnabled( false );
        ui->testServerStatus->setEnabled( false );
        ui->lblServerStatusTxt->setVisible( false );

        // disable and hide the standard barcode text and input boxes
        enabled = false;
        visible = false;
    }
    // barcode cloud feature and enable accum items are both checked
    else
    {
        // enable and show all the widgets associated with barcode cloud feature
        ui->chkEnableCloudFeature->setEnabled( true );
        ui->txtAccumItemsServerAddress->setVisible( true );
        ui->lblServerStatus->setVisible( true );
        ui->lblServerAddress->setVisible( true );
        ui->lblServerAddress->setEnabled( true );
        ui->txtAccumItemsServerAddress->setEnabled(true);
        ui->testServerStatus->setVisible( true );
        ui->testServerStatus->setEnabled( true );
        ui->lblServerStatusTxt->setVisible( true );

        if(ui->txtAccumItemsServerAddress->text().size() > 0 )
        {
            ui->testServerStatus->setEnabled( true );
            ui->txtAccumItemsServerAddress->setCursorPosition(0);
        }

        // disable and hide the standard barcode text and input boxes
        enabled = false;
        visible = false;

    }
    QVector<QWidget*> hideThese = enableTheseBarcodeBoxes(0);
    QVector<QWidget*> disableThese = enableTheseBarcodeBoxes(1);
    QWidget *iter;

    foreach (iter, hideThese )
        iter->setVisible( visible );
    foreach (iter, disableThese){

        QLineEdit* lineEdit= qobject_cast<QLineEdit*>(iter);
        if(lineEdit)
            lineEdit->setEnabled(enabled);
        else
        {
            QLabel *label = qobject_cast<QLabel*>(iter);
            if(label)
                label->setEnabled(enabled);
        }
    }
    tabAccumHasChanged = true;

}

// SOF-5386: PJR-454
void SuperNormalScaleOperation::testServerStatusClicked()
{
    LOG_CLASS_INFO("SuperNormalScaleOperation", "testServerStatusClicked");
    LOG_TRACE("test server status btn clicked", NULL);

    if(ui->txtAccumItemsServerAddress->text().size() > 0)
    {
        QString destination = ui->txtAccumItemsServerAddress->text();

        QStringList args;
        args.append("1");
        args.append(destination);

        QTimer *connectingService = new QTimer();
        python *proc = new python();

        connectingService->start(3);    // if taking awhile to connect, notify user
        proc->execCommand("getBarcodeFromCloud", args);

        // command: python getBarcodeFromCloud 1 URL
        QString status = proc->readStdOut();
        LOG_TRACE(status.toStdString().c_str(), NULL);

        if(status == "1")
        {
            ui->lblServerStatusTxt->setText("Online");
        }
        else
        {
            ui->lblServerStatusTxt->setText("Error Reaching");
        }

        connectingService->stop();

        delete connectingService;
        delete proc;
    }
}
void SuperNormalScaleOperation::onTimeout()
{
    ui->lblServerStatusTxt->setText("Attempting to reach");
}

// SOF-5386: PJR-454
QVector<QWidget*> SuperNormalScaleOperation::enableTheseBarcodeBoxes(int param)
{
    QString eanStr = ui->butAccumItemsEANFormatString->text();
    int eanStrLen = eanStr.length();

    if(param == 1)
    {
    if(eanStrLen != 0)
    {
        QVector<QWidget*> widgets;

        int digits =  eanStr.count("X");
        if(digits > 0)
        {
            widgets.push_back(ui->txtAccumItemsEANPrefix);
            widgets.push_back(ui->lblEanPrefix);
        }
        digits = eanStr.count("E");
        if(digits  > 0)
        {
            widgets.push_back(ui->txtAccumItemsEANCode);
            widgets.push_back(ui->lblEanCode);
        }
        digits = eanStr.count("S");
        if(digits  > 0)
        {
            widgets.push_back(ui->txtAccumItemsScaleId);
            widgets.push_back(ui->lblScaleId);
        }
        digits = eanStr.count("T");
        if(digits  > 0)
        {
            widgets.push_back(ui->txtAccumItemsTransNum);
            widgets.push_back(ui->lblTransNum);
        }
        // which standard barcode boxes 'should' be enabled based on the current configuration
        widgets.push_back(ui->butAccumItemsEANFormatString);
        return widgets;
    }

    }

    // rval used to hide/show standard barcode widgets
    QVector<QWidget*> linuxWontAllowInitializerLists(QVector<QWidget*>() << ui->lblEanPrefix << ui->lblEanCode << ui->lblScaleId << ui->txtAccumItemsScaleId << ui->lblTransNum << ui->txtAccumItemsTransNum
                          << ui->lblEanFormatString << ui->butAccumItemsEANFormatString << ui->txtAccumItemsEANPrefix);
    return linuxWontAllowInitializerLists;

}
//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::checkboxChangedAccumulateStrip(int state)
{
    if( isUpdating )
        return;

    if(state == 0)
        ui->chkEnableAccumItemsTotalsOnly->setEnabled( true );
    else
    {
        ui->chkEnableAccumItemsTotalsOnly->setChecked( false );
        ui->chkEnableAccumItemsTotalsOnly->setEnabled( false );
    }

    tabAccumHasChanged = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::updateNetworkCheckboxesChanged(QWidget *checkBox)
{
    signalMapper_->blockSignals(true);								// FT-54 Network connectivity cannot be turned off, changes will not save

    // this was known as "specialCheckBoxCase" in operateNormalScaleOperations.
    if( !initNetworkCbos )
        hasChanged = true;

    if( checkBox == ui->chkCheckNetConnectivity )
    {
        if( ui->chkCheckNetConnectivity->isChecked() )
        {
            // just got checked to come in here, and Gateway gets checked by default
            if ( session_->isServiceCounter() )
                ui->chkPingPrimary->setEnabled( true );
            ui->chkPingGateway->setEnabled( true );

            if( !initNetworkCbos )
            {
                //PTR #674 - we want to default to ping Gateway now.... So if we toggle it on, set ping gateway
                normalScaleOperation_->pingGatewayRaw() = "1";
                ui->chkPingGateway->setChecked( true );
            }
        }
        else
        {
            if ( session_->isServiceCounter() )
            {
                ui->chkPingPrimary->setChecked( false );
                ui->chkPingPrimary->setEnabled( false );
            }
            normalScaleOperation_->pingPrimaryRaw() = "0";

            ui->chkPingGateway->setChecked( false );
            ui->chkPingGateway->setEnabled( false );
            normalScaleOperation_->pingGatewayRaw() = "0";
        }

        setPingTargetState();

        if( initNetworkCbos )
            normalScaleOperation_->pingTarget() = ui->chkPingTarget->isChecked() ? "1" : "0";

        setPingIntervalState();

    }
    else if( checkBox == ui->chkPingPrimary )
    {
        if ( session_->isServiceCounter() && ui->chkPingPrimary->isChecked() )
        {
            ui->chkPingGateway->setChecked( false );
            normalScaleOperation_->pingGatewayRaw() = "0";

            ui->chkPingTarget->setChecked( false );
            normalScaleOperation_->pingTarget() = "0";
        }

        setPingTargetState();
    }
    else if( checkBox == ui->chkPingGateway )
    {
        if( ui->chkPingGateway->isChecked() )
        {
            if ( session_->isServiceCounter() )
                ui->chkPingPrimary->setChecked( false );
            normalScaleOperation_->pingPrimaryRaw() = "0";

            ui->chkPingTarget->setChecked( false );
            normalScaleOperation_->pingTarget() = "0";
        }

        setPingTargetState();
    }
    else if( checkBox == ui->chkPingTarget )
    {
        if( ui->chkPingTarget->isChecked() )
        {
            ui->chkPingGateway->setChecked( false );
            normalScaleOperation_->pingGatewayRaw() = "0";
            if ( session_->isServiceCounter() )
                ui->chkPingPrimary->setChecked( false );
            normalScaleOperation_->pingPrimaryRaw() = "0";
        }

        setPingTargetState();
    }
    else if( checkBox == ui->chkNetworkDetailsDisplay )
    {
        normalScaleOperation_->enableDisplayNetworkDetailsRaw() = ui->chkNetworkDetailsDisplay->isChecked() == true ? "1" : "0";
    }

    initNetworkCbos = false;
    signalMapper_->blockSignals(false);								// FT-54 Network connectivity cannot be turned off, changes will not save
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::inputChanged(int newIndex)
{
    Q_UNUSED( newIndex );

    if( isUpdating )
        return;

    hasChanged = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::timeboxChanged(QTime newTime)
{
    Q_UNUSED( newTime );

    if( isUpdating )
        return;

    hasChanged = true;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::textboxChangedDisplayOptions(QString newText)
{
    Q_UNUSED(newText);

    // unlike textbox changed is called after a textbox loses focus, this is called while it has focus and has changed.
    if( isUpdating )
        return;

    hasChangedDisplayOptions = true;
    hasChanged = true;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::textboxChangedDefaultHomePage( )
{
    if( isUpdating )
        return;

    hasChanged = true;
    normalScaleOperation_->defaultHomePage() = ui->txtDefaultHomePage->text();
    scaleConfigData_->defaultHomePage() = ui->txtDefaultHomePage->text();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::textboxChangedMaxTare( )
{
    if( isUpdating )
        return;

    hasChanged = true;
    UiUtilities *util = session_->uiUtilities;

    double newMaxTare = ui->txtMaxTare->text().toDouble();

    if( newMaxTare > theWeigherMaxWt )
    {
        // I don't believe this is even possible with the QValidator, so see if at some point we can get rid of this
        QString tmpMaxTare = session_->uiUtilities->youGottaSetTheSeparator( normalScaleOperation_->maxTare() );
        ui->txtMaxTare->setText( tmpMaxTare );
        maxTareHasBeenUpdated = true;                                   // FT-1087 Max tare being set to 0.00 - only after physically setting the field do I set this to true
    }
    else
    {
        QString tmpMaxTare = ui->txtMaxTare->text();
        util->youGottaSetTheSeparator( tmpMaxTare );
        ui->txtMaxTare->setText( tmpMaxTare );
        maxTareHasBeenUpdated = true;                                   // FT-1087 Max tare being set to 0.00 - only after physically setting the field do I set this to true
        normalScaleOperation_->maxTare() = ui->txtMaxTare->text();
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::textboxChangedMinSweetheartWeight( )
{
    if( isUpdating )
        return;

    hasChanged = true;
    UiUtilities *util = session_->uiUtilities;

    QString tmpMSW = ui->txtMinSweetheartWeight->text();
    util->youGottaSetTheSeparator( tmpMSW );
    ui->txtMinSweetheartWeight->setText( tmpMSW );
    normalScaleOperation_->minSweetheartWeight() = ui->txtMinSweetheartWeight->text();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::textboxChangedTareSetback( )
{
    if( isUpdating )
        return;

    hasChanged = true;
    UiUtilities *util = session_->uiUtilities;

    double newTareSetback = ui->txtTareSetback->text().toDouble();

    if( newTareSetback > theWeigherMaxWt )
    {
        // I don't believe this is even possible with the QValidator, so see if at some point we can get rid of this
        QString tmpTareSetback = session_->uiUtilities->youGottaSetTheSeparator( normalScaleOperation_->tareSetback() );
        ui->txtTareSetback->setText( tmpTareSetback );
//        maxTareHasBeenUpdated = true;                                   // FT-1087 Max tare being set to 0.00 - only after physically setting the field do I set this to true
    }
    else
    {
        QString tmpTareSetback = ui->txtTareSetback->text();
        util->youGottaSetTheSeparator( tmpTareSetback );
        ui->txtTareSetback->setText( tmpTareSetback );
//        maxTareHasBeenUpdated = true;                                   // FT-1087 Max tare being set to 0.00 - only after physically setting the field do I set this to true
        normalScaleOperation_->tareSetback() = ui->txtTareSetback->text();
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::textboxChangedAccumulate()
{
    if( isUpdating )
        return;

    tabAccumHasChanged = true;
    ui->txtAccumItemsServerAddress->setCursorPosition(0);
    ui->testServerStatus->setEnabled(true);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::textboxChanged()
{
    // now that we have decided something has changed, when we change tabs, save everything off to DB

    if(ui->txtPingHours->text().toInt() == 0)
    {
        pingMinutes_->setBottom(1);
        if(ui->txtPingMinutes->toInt() == 0)
        {
            ui->txtPingMinutes->setText("1");
        }

    }
    else
    {
        pingMinutes_->setBottom(0);
    }

    if( isUpdating )
        return;

    hasChanged = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::comboboxChanged(int newIndex)
{
    Q_UNUSED( newIndex );

    if( isUpdating )
        return;

    hasChanged = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::comboboxLoginMethodChanged(int newIndex)
{
    if( isUpdating )
        return;

    scaleConfigData_->setRemoteAccessLoginMethod( newIndex );
    hasChanged = true;

    changeRemoteAccessLoginMethod( newIndex );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::comboboxDateChanged(int newIndex)
{
    Q_UNUSED( newIndex );

    if( isUpdating )
        return;

    dateTimeHasChanged = true;
    // FT-138 Date format will not save in Supervisor
    //scaleConfigData_->dateStyleRaw() = QString::number(newIndex);
    qDebug() << " previous date style raw is : " << scaleConfigData_->dateStyle();
    qDebug() << "new date style changed to is : " << ui->cboDateStyle->itemData(ui->cboDateStyle->currentIndex()).toString();
    scaleConfigData_->dateStyleRaw() = ui->cboDateStyle->itemData(ui->cboDateStyle->currentIndex()).toString();
    qDebug() << " new date style raw is : " << scaleConfigData_->dateStyle();

}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::comboboxTimeChanged(int newIndex)
{
    if( isUpdating )
        return;

    dateTimeHasChanged = true;
    // FT-136 Hours format does not save in Supervisor
    scaleConfigData_->setTimeFormat(newIndex);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::comboboxPriLangChanged(int newIndex)
{
    if( isUpdating )
        return;

    langHasChanged = true;

    updateLanguageValues(1, newIndex);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::comboboxSecLangChanged(int newIndex)
{
    if( isUpdating )
        return;

    langHasChanged = true;

    updateLanguageValues(2, newIndex);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::comboboxRightPanelMode(int newIndex)
{
    Q_UNUSED( newIndex );

    if( isUpdating )
        return;

    hasChanged = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::comboboxBottomPanelMode(int newIndex)
{
    Q_UNUSED( newIndex );

    if( isUpdating )
        return;

    hasChanged = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::comboboxChangedOperateStartScreen(int newIndex)
{
    if( isUpdating )
        return;

    hasChanged = true;
    updateClearPluOnPrint( newIndex );

    // FT-394: save config for home button
    session_->scaleConfig->scaleConfig.startScreenHome() = QString::number( newIndex );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::comboboxChangedNowServing(int newIndex)
{
    if( isUpdating )
        return;

    hasChanged = true;

    session_->scaleConfig->scaleConfig.setNowServingSysType( newIndex );
    session_->scaleConfig->normalScaleOperation.setNowServingSysType( newIndex );

    normalScaleOperation_->setNowServingSysType( newIndex );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::comboboxChangedCutTest(int newIndex)
{
    /**
     * SLOT method invoked when Cut Test Type combo box index changed
     */

    if( isUpdating )
        return;

    hasChanged = true;

    /**
     * Save to scaleConfiguration and NormalScaleOperation COnfiguration the new index
     */
    session_->scaleConfig->scaleConfig.setCuttingTestType( newIndex );              ///< MADP class
    session_->scaleConfig->normalScaleOperation.setCuttingTestType( newIndex );     ///< NormalScaleOps class

    normalScaleOperation_->setCuttingTestType( newIndex );                          ///< NormalScaleOps class var
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// PJR383
//void SuperNormalScaleOperation::comboboxChangedIgnorePrinterErrors(int newIndex)
//{
//    /**
//     * SLOT method invoked when Cut Test Type combo box index changed
//     */
//
//    if( isUpdating )
//        return;
//
//    hasChanged = true;
//
//    /**
//     * Save to scaleConfiguration and NormalScaleOperation COnfiguration the new index
//     */
//    session_->scaleConfig->scaleConfig.setSelfServeIgnorePrinterErrors( newIndex );              ///< MADP class
//    session_->scaleConfig->normalScaleOperation.setSelfServeIgnorePrinterErrors( newIndex );     ///< NormalScaleOps class
//
//    normalScaleOperation_->setSelfServeIgnorePrinterErrors( newIndex );                                       ///< NormalScaleOps class var
//}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::comboboxChangedSelfServStartScreen(int newIndex)
{
    // 0 = None (straight to PLU entry)
    // 1 = Begin users on Start Screen (Big Green Button)
    // 2 = Weight required (3 images - weight required on platter to proceed - can also be clickable)
    // 3 = Playlists (weight required on this one too. Clickable?)

    if( isUpdating )
        return;

    // if playlists selected
    if( 3 == newIndex )
    {
        ui->butEditPlaylist->setEnabled( true );
        ui->cboSelfServePlaylist->setEnabled( true );
    }
    else
    {
        ui->butEditPlaylist->setEnabled( false );
        ui->cboSelfServePlaylist->setEnabled( false );
    }

    // if weight required
    if( 2 == newIndex )     // weight required
    {
        ui->chkStartScreenClickable->setVisible( true );
        ui->chkStartScreenClickable->setChecked( true );                    // if you select this as an option, it's automatically enabled

        //ui->chkSelfServePrintOnStableWeight->setVisible( true );          // stable weight  // SOF-4401: ALWAYS HIDE
    }
    else if( 3 == newIndex )
    {
        ui->chkStartScreenClickable->setVisible( false );
    }
    else
    {                
        ui->chkStartScreenClickable->setVisible( false );
        ui->chkStartScreenClickable->setChecked( false );                   // if you select something else, no more clickablesness

        //ui->chkSelfServePrintOnStableWeight->setVisible( true );          // stable weight  // SOF-4401: ALWAYS HIDE
    }

    hasChangedDisplayOptions = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::comboboxChangedSelfServItemEntryScreen(int newIndex)
{
    // 0 = Category Flashkeys
    // 1 = Best Sellers
    // 2 = Number Pad (Red Apple) - Keyed Entry
    if( isUpdating )
        return;

    if( ( 0 == newIndex ) || ( 1 == newIndex ) || ( 2 == newIndex ) || ( 3 == newIndex ) )
    {
        ui->chkIncludeNumpad->setVisible( true );
        ui->chkIncludeNumpad->setChecked( true );       // if you select this as an option, it's automatically enabled
    }
    else
    {
        ui->chkIncludeNumpad->setVisible( false );
        ui->chkIncludeNumpad->setChecked( false );      // if unselected, then this is unchecked too
    }


    hasChangedDisplayOptions = true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::comboboxChangedSelfServSearchScreen(int newIndex)
{
    Q_UNUSED( newIndex);

    // if 0 - Default search method, A-Z category search, and displays Best Sellers.
    // if 1 - Character by character

    if( isUpdating )
        return;

    hasChangedDisplayOptions = true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::comboboxChangedSelfServPrintScreen(int newIndex)
{
    // 0 = Print label
    // 1 = Best Sellers
    // 2 = Number Pad (Red Apple) - Keyed Entry

    if( isUpdating )
        return;

    if( ( 1 == newIndex ) || ( 2 == newIndex ) )
        ui->chkSelfServeDisplayPluLabel->setVisible( true );
    else
    {
        ui->chkSelfServeDisplayPluLabel->setVisible( false );
        ui->chkSelfServeDisplayPluLabel->setChecked( false );
    }

    hasChangedDisplayOptions = true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::comboboxChangedSelfServAfterPrintScreen(int newIndex)
{
    Q_UNUSED( newIndex);

    // if 0 - Display image
    // if 1 - Play video

    if( isUpdating )
        return;

    hasChangedDisplayOptions = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::comboboxChangedAccessBrowser(int newIndex)
{
    if( isUpdating )
        return;

    hasChanged = true;
    hasChangedDisplayOptions = true;

    QString abmValue = ui->cboAccessBrowserMode->itemData( newIndex ).toString();
    enableBrowserInPanel( (hobart::dynamicContentWidget::WebBrowser == abmValue.toInt()) ? 1 : 0 );

    if( newIndex == 2 )
        ui->lblDefaultBrowserHomePage->setEnabled( true );
    else
        ui->lblDefaultBrowserHomePage->setDisabled( true );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::checkboxChangedEnableEnhancedCenterScreen(int state)
{
    if( isUpdating )
        return;

    hasChanged = true;

    bool isChked = state > 1 ? true : false;
    normalScaleOperation_->enableEnhancedCenterScreenRaw() = isChked ? "1" : "0";

    if( isChked )
    {
        ui->lblEnhancedCenterScreenAddress->setEnabled( true );
        ui->txtEnhancedCenterScreenAddress->setEnabled( true );
    }
    else
    {
        ui->lblEnhancedCenterScreenAddress->setDisabled( true );
        ui->txtEnhancedCenterScreenAddress->setDisabled( true );
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// FT-790
void SuperNormalScaleOperation::comboboxChangedRunScreenBrowser(int newIndex)
{
    if( isUpdating )
        return;

    hasChanged = true;
    hasChangedDisplayOptions = true;

    if( newIndex == 2 )
    {
        ui->lblRunScreenHomePage->setEnabled( true );
        ui->txtRunScreenHomePage->setEnabled( true );
    }
    else
    {
        ui->lblRunScreenHomePage->setDisabled( true );
        ui->txtRunScreenHomePage->setDisabled( true );
    }
}

void SuperNormalScaleOperation::comboboxChangedSearchMethod(int newIndex)
{
    Q_UNUSED( newIndex );

    if( isUpdating )
        return;

    hasChanged = true;
    hasChangedDisplayOptions = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::comboboxChangedFlashkeyType(int newIndex)
{
    Q_UNUSED( newIndex );

    if ( isUpdating )
        return;

    hasChanged = true;
    hasChangedDisplayOptions = true;

    if( 1 == newIndex )
	{
        if( ui->cboFlashkeyType_0 == sender() )
        {
            ui->txtFlashkeyNum_0->setText( QString::number( hobart::softkeysWidget::FreshServeFixedButtons ) );
            ui->txtFlashkeyNum_0->setEnabled( false );
        }
        else if( ui->cboFlashkeyType_1 == sender() )
        {
            ui->txtFlashkeyNum_1->setText( QString::number( hobart::softkeysWidget::FreshServeFixedButtons ) );
            ui->txtFlashkeyNum_1->setEnabled( false );
        }
        else if( ui->cboFlashkeyType_2 == sender() )
        {
            ui->txtFlashkeyNum_2->setText( QString::number( hobart::softkeysWidget::FreshServeFixedButtons ) );
            ui->txtFlashkeyNum_2->setEnabled( false );
        }
        else if( ui->cboFlashkeyType_3 == sender() )
        {
            ui->txtFlashkeyNum_3->setText( QString::number( hobart::softkeysWidget::FreshServeFixedButtons ) );
            ui->txtFlashkeyNum_3->setEnabled( false );
        }
        else if( ui->cboFlashkeyType_4 == sender() )
        {
            ui->txtFlashkeyNum_4->setText( QString::number( hobart::softkeysWidget::FreshServeFixedButtons ) );
            ui->txtFlashkeyNum_4->setEnabled( false );
        }
	}
    else
    {
        if( ui->cboFlashkeyType_0 == sender() )
        {
            ui->txtFlashkeyNum_0->setEnabled( true );
            if( QString::number( hobart::softkeysWidget::FreshServeFixedButtons ) == ui->txtFlashkeyNum_0->text() )
                ui->txtFlashkeyNum_0->setText( "" );
        }
        else if( ui->cboFlashkeyType_1 == sender() )
        {
            ui->txtFlashkeyNum_1->setEnabled( true );
            if( QString::number( hobart::softkeysWidget::FreshServeFixedButtons ) == ui->txtFlashkeyNum_1->text() )
                ui->txtFlashkeyNum_1->setText( "" );
        }
        else if( ui->cboFlashkeyType_2 == sender() )
        {
            ui->txtFlashkeyNum_2->setEnabled( true );
            if( QString::number( hobart::softkeysWidget::FreshServeFixedButtons ) == ui->txtFlashkeyNum_2->text() )
                ui->txtFlashkeyNum_2->setText( "" );
        }
        else if( ui->cboFlashkeyType_3 == sender() )
        {
            ui->txtFlashkeyNum_3->setEnabled( true );
            if( QString::number( hobart::softkeysWidget::FreshServeFixedButtons ) == ui->txtFlashkeyNum_3->text() )
                ui->txtFlashkeyNum_3->setText( "" );
        }
        else if( ui->cboFlashkeyType_4 == sender() )
        {
            ui->txtFlashkeyNum_4->setEnabled( true );
            if( QString::number( hobart::softkeysWidget::FreshServeFixedButtons ) == ui->txtFlashkeyNum_4->text() )
                ui->txtFlashkeyNum_4->setText( "" );
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::editLabelOffsetLeft()
{
    int curOffset = ui->lblFreshServeLabelOffset->text().toInt();

    if ( curOffset > -50 )
    {
        hasChangedDisplayOptions = true;
        ui->lblFreshServeLabelOffset->setText( QString::number( curOffset - 1 ) );
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::editLabelOffsetRight()
{
    int curOffset = ui->lblFreshServeLabelOffset->text().toInt();

    if ( curOffset < 50 )
    {
        hasChangedDisplayOptions = true;
        ui->lblFreshServeLabelOffset->setText( QString::number( curOffset + 1 ) );
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// FT-790
void SuperNormalScaleOperation::textboxChangedEnhancedCenterHomePage( )
{
    if( isUpdating )
        return;

    hasChanged = true;

    normalScaleOperation_->enhancedCenterHomePage() = ui->txtEnhancedCenterScreenAddress->text();
    scaleConfigData_->enhancedCenterHomePage()      = ui->txtEnhancedCenterScreenAddress->text();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// FT-790
void SuperNormalScaleOperation::textboxChangedRunScreenHomePage( )
{
    if( isUpdating )
        return;

    hasChanged = true;

    normalScaleOperation_->runScreenHomePage()  = ui->txtRunScreenHomePage->text();
    scaleConfigData_->runScreenHome()           = ui->txtRunScreenHomePage->text();
}

void SuperNormalScaleOperation::textboxChangedResetStatistics( )
{
    if( isUpdating )
        return;

    hasChanged = true;
    hasChangedDisplayOptions = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// PJR383
//void SuperNormalScaleOperation::checkboxChangedPrintToScreen( int state )
//{
//    if ( isUpdating )
//        return;
//
//    if( state > 0 )
//        ui->chkSelfServeDisplayPluLabel->setEnabled( true );
//    else
//        ui->chkSelfServeDisplayPluLabel->setEnabled( false );
//
//    hasChangedDisplayOptions = true;
//}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// PJR383
//void SuperNormalScaleOperation::checkboxChangedCombinedOptions( int state )
//{
//    if ( isUpdating )
//        return;
//
//    // SOF-3997 Big Y Self-Serve: Print Label to Screen needs to be automatically selected
//    // means the checkbox is unchecked
//    if( state > 0)
//    {
//        // check the state of Print to Screen. If its unchecked, then check it too
//        if( !ui->chkselfServeDisplayLabelOnScreenGO->isChecked() )
//            ui->chkselfServeDisplayLabelOnScreenGO->setChecked( true );
//    }
//
//    hasChangedDisplayOptions = true;
//}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// HT-365
void SuperNormalScaleOperation::checkboxChangedEnableExtraShelfLifeDay( int state )
{
    Q_UNUSED( state );

    if( isUpdating )
        return;

    if( session_->scaleConfig->extraShelfLifeDayAvail )
    {
        // means someone tried to set the value in Supervisor but we have toggleable flashkeys available in Operate Mode to do it. Pop msg but enable time.
        session_->send( Error, ErrSessionDevice, QString::number( SessionDevError::SessionDevWillNotDisableExtraDay ) + ",5" );
    }

    hasChanged = true;

    if( state )
    {
        // ENABLED Sent to App when checkbox checked
        normalScaleOperation_->enableVariableShelfLifeTimeRaw() = "1";
        scaleConfigData_->enableVariableShelfLifeTimeRaw() = "1";
        QString param;
        param += "," + QString::number( fieldOffsetCfgEnableVariableShelfLifeTime );
        param += ",1,0";        // Enables extra shelf life day
        session_->send( ConfigurationDev, CfgSaveSingleField, param );
    }
    else
    {
        // DISABLED Sent to App when checkbox unchecked
        normalScaleOperation_->enableVariableShelfLifeTimeRaw() = "0";
        scaleConfigData_->enableVariableShelfLifeTimeRaw() = "0";
        QString param;
        param += "," + QString::number( fieldOffsetCfgEnableVariableShelfLifeTime );
        param += ",0,1";        // Disables extra shelf life day
        session_->send( ConfigurationDev, CfgSaveSingleField, param );
    }

    if( (ui->chkEnableShelfLifeExtraDay->isChecked() ) )
    {
        // SOF-4928 PJR 420 - Flashkey Options to Toggle Extra Day
        // 4928 takes precedence over 2755. No longer am I going to blank the field if
        // it is disabled in Super. Just gonna leave it alone
        // SOF-2755: When toggled to enabled - Do not allow "00" to be entered
        //           for extra shelf life hour, and set the time to 12:00 AM
        if( ui->leShelfLifeHour->text() == "00")
        {
            // otherwise if it had a value, leave as is
            shelfLifeHoursValidator_->setBottom( 1 );
            ui->leShelfLifeHour->setText( "12" );
            ui->leShelfLifeMinutes->setText( "00" );
            ui->butShelfLifeAmPm->setText( "AM" );
        }

        ui->leShelfLifeHour->setEnabled( true );
        ui->leShelfLifeMinutes->setEnabled( true );
        ui->butShelfLifeAmPm->setEnabled( true );
    }
    else
    {
        shelfLifeHoursValidator_->setBottom( 0 );

         // SOF-4928 PJR 420 - Flashkey Options to Toggle Extra Day
        // 4928 takes precedence over 2755. No longer am I going to blank the field if
        // it is disabled in Super. Just gonna leave it alone
        // SOF 2755: Set to default values when disabled

        if( ( ui->leShelfLifeHour->text() == "12" ) && ( ui->leShelfLifeMinutes->text() == "00" ) && ( ui->butShelfLifeAmPm->text() == "AM" ) )
        {
            ui->leShelfLifeHour->setText( "00" );
            ui->leShelfLifeMinutes->setText( "00" );
            ui->butShelfLifeAmPm->setText( "AM" );
        }

        ui->leShelfLifeHour->setEnabled( false );
        ui->leShelfLifeMinutes->setEnabled( false );
        ui->butShelfLifeAmPm->setEnabled( false );
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// HT-3846
void SuperNormalScaleOperation::checkboxChangedDisableCuttingBetweenLabels( int state )
{
    Q_UNUSED( state );

    //----------------------------------------------------------------------------------
    // SOF-3947: Force "Enable continuous label print in production entry" to be checked
    //           and disabled when "Disable cutting between linerless label printing"
    //           is checked.
    //
    //           The "Disable cutting between linerless label printing" checkbox is
    //           only enabled when the cutter is present and enabled.
    //----------------------------------------------------------------------------------
    if( ui->chkDisableCuttingBetweenLinerlessLabels->isChecked() )
    {
        if( cutterAttached_ && cutterEnabled_ )
        {
            ui->chkProdEntryContinuousPrint->setChecked( true );
            ui->chkProdEntryContinuousPrint->setEnabled( false );
        }
    }
    else
    {
        ui->chkProdEntryContinuousPrint->setEnabled( true );
    }

    if( isUpdating )
        return;

    disableCuttingBetweenLabelsToggled = true;
}

void SuperNormalScaleOperation::onPrimaryLabelChange(int index)
{
    Q_UNUSED( index );
    populateSecondaryPositions();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::currentChangeTab(int tabNum)
{
    //LOG_CLASS_INFO( "SuperNormalScaleOperation", "currentChangeTab" );
    // if anything needs to be done on the previous tab we left. currentTab is
    // actually the previous Tab index until we update it with tabNum

    //if( !dataHasBeenInitialized )
    //{
    //    ui->tabNormalScaleOperation->setCurrentIndex(0);
    //}
    if( currentTab != tabNum )
    {
        if( currentTab == ui->tabNormalScaleOperation->indexOf( ui->tab0 ) )
        {
            // PRODUCT TAB

            // PRODUCTS - Field Options
            normalScaleOperation_->fixedWeightUnits() = QString::number( ui->cboFixedWeightUnits->currentIndex() );
            int ewp = ui->cboExternalWeightPrecision->currentIndex() + 2;
            normalScaleOperation_->externalWeightPrecision() = QString::number( ewp );

            // FT-908 max tare not saving when "," separator
            if( maxTareHasBeenUpdated && ui->txtMaxTare->isEnabled() )
            {
                QString maxTare( ui->txtMaxTare->text() );
                if ( "." != currencyData_->unitSeparator() )
                    maxTare.replace( currencyData_->unitSeparator(), "." );
                normalScaleOperation_->maxTare() = maxTare;
            }

            if( ui->txtTareSetback->isEnabled() )
            {
                QString tareSetback( ui->txtTareSetback->text() );
                if ( "." != currencyData_->unitSeparator() )
                    tareSetback.replace( currencyData_->unitSeparator(), "." );
                normalScaleOperation_->tareSetback() = tareSetback;
            }

            normalScaleOperation_->useAutoLookupRaw() = ui->chkUseAutoLookup->isChecked() == true ? "1" : "0";
            int tmpDigitsToLookup = ui->cboNumOfDigits->currentIndex() + 3;
            normalScaleOperation_->numOfDigits() = QString::number(tmpDigitsToLookup);
            if ( !session_->isPrepack() )
                normalScaleOperation_->autoLookupOverrideDigit() = QString::number( ui->cboSpAutoLookupDigit->currentIndex() );

            normalScaleOperation_->useSecondTareRaw() = ui->chkUseSecondTare->isChecked() == true ? "1" : "0";
            normalScaleOperation_->fixedWeightVerificationRaw() = ui->chkFixedWeightVerification->isChecked() == true ? "1" : "0";

            //if( ui->chkEnableShelfLifeExtraDay->isChecked() )
            //{
            //    normalScaleOperation_->enableVariableShelfLifeTimeRaw() = "1";
            //    scaleConfigData_->enableVariableShelfLifeTimeRaw() = "1";
            //}
            //else
            //{
            //    normalScaleOperation_->enableVariableShelfLifeTimeRaw() = "0";
            //    scaleConfigData_->enableVariableShelfLifeTimeRaw() = "0";
            //}

            normalScaleOperation_->variableShelfLifeTime() = ui->leShelfLifeHour->text();
            normalScaleOperation_->variableShelfLifeTime() += ":";
            normalScaleOperation_->variableShelfLifeTime() += ui->leShelfLifeMinutes->text();
            normalScaleOperation_->variableShelfLifeTime() += " ";
            normalScaleOperation_->variableShelfLifeTime() += ui->butShelfLifeAmPm->text();

            // PRODUCTS - Content Options
            normalScaleOperation_->autoDisplaySuggestiveSellingRaw() = ui->chkAutoDisplaySuggestiveSelling->isChecked() == true ? "1" : "0";
            normalScaleOperation_->suggestiveSellingFreqRaw() = QString::number( qMax( 1, ui->txtSuggSellFreq->toInt() ) );
            normalScaleOperation_->suggestiveSellingStyleRaw() = QString::number( ui->cboSuggSellStyle->currentIndex() );
            normalScaleOperation_->autoDisplayProductNotesRaw() = ui->chkAutoDisplayProdNotes->isChecked() == true ? "1" : "0";
            normalScaleOperation_->autoDisplayCookingNotesRaw() = ui->chkAutoDisplayCookingNotes->isChecked() == true ? "1" : "0";
			normalScaleOperation_->autoDoNotPrintNutrifactsRaw() = ui->chkDoNotPrintNutrifacts->isChecked() == true ? "1" : "0";
            normalScaleOperation_->autoUpdateUPCBarcodeOver99Raw() = ui->chkAutoUpdateUPCBarcodeOver99->isChecked() == true ? "1" : "0";
            QString upcdata = ui->cboBarcodeTypeOver99->itemData(ui->cboBarcodeTypeOver99->currentIndex()).toString();
            normalScaleOperation_->barcodeTypeOver99Raw() = upcdata;
            normalScaleOperation_->clearPluAfterPrintingRaw() = ui->chkClearPluAfterPrinting->isChecked() == true ? "1" : "0";

            // PRODUCTS - Tracking Options
            normalScaleOperation_->trackingOperatorChangesRaw() = ui->chkTrackOperChanges->isChecked() == true ? "1" : "0";
            normalScaleOperation_->trackingSupervisorChangesRaw() = ui->chkTrackSuperChanges->isChecked() == true ? "1" : "0";
            normalScaleOperation_->collectTransactionsRaw() = ui->chkCollectTransactions->isChecked() == true ? "1" : "0";
            QString tmpDaysForTrans = ui->cboDaysToKeepTransactions->itemData( ui->cboDaysToKeepTransactions->currentIndex()).toString();
            normalScaleOperation_->daysToKeepTransactions() = tmpDaysForTrans;
            normalScaleOperation_->voidTransactionsViewHoursRaw() = QString::number( qMax( 1, ui->txtTransHoursToView->toInt() ) );
            normalScaleOperation_->coolTrackingNumRaw() = ui->chkCOOLTrackNum->isChecked() == true ? "1" : "0";
            normalScaleOperation_->minSweetheartWeight() = ui->txtMinSweetheartWeight->text();
        }
        else if( currentTab == ui->tabNormalScaleOperation->indexOf( ui->tab1 ) )
        {
            // SCALE ACCESS TAB
            normalScaleOperation_->disableOperatorFlashkeyEditorRaw() = ui->chkDisableOpFlashkeyEditor->isChecked() == true ? "1" : "0";
            // Had to reverse the logic of the checkbar, if we get a 1, then it means it IS NOT checked. A 0 means it IS CHECKED
            normalScaleOperation_->allowAddressBarRaw() = ui->chkRemoveAddressBar->isChecked() == true ? "0" : "1";
            normalScaleOperation_->disableStartScreenRaw() = ui->chkDisableLeftPanel->isChecked() == true ? "1" : "0";
            normalScaleOperation_->showPrinterSettingsRaw() = ui->chkShowPrinterSettings->isChecked() == true ? "1" : "0";
            normalScaleOperation_->showSelectAllWhenVoidingTransRaw() = ui->chkShowSelectAllWhenVoidingTrans->isChecked() == true ? "1" : "0";
            normalScaleOperation_->showDeleteAllFilesRaw() = ui->chkShowDeleteAllFiles->isChecked() == true ? "1" : "0";
            normalScaleOperation_->enableSearchMenuButtonRaw()        = ui->chkSearchMenuButton->isChecked()              == true ? "1" : "0";
            normalScaleOperation_->enableSearchOnKeypadRaw()          = ui->chkShowSearchInKeypad->isChecked()            == true ? "1" : "0";
            normalScaleOperation_->remoteAccessPermittedRaw() = ui->chkRemoteAccessPermitted->isChecked() == true ? "1" : "0";
            normalScaleOperation_->remoteAccessLoginMethodRaw() = QString::number( ui->cboRemoteAccessLoginMethod->currentIndex());
            normalScaleOperation_->remoteAccessUserName() = ui->txtRemoteAccessUserName->text();
            normalScaleOperation_->remoteAccessPassword() = ui->txtRemoteAccessPassword->text();
            normalScaleOperation_->logoutRemoteIdleTime() = ui->txtLogoutRemoteIdleTime->text();
            // defaultbrowserhomepage is saved in textboxChangedDefaultHomePage()
        }
        else if( currentTab == ui->tabNormalScaleOperation->indexOf( ui->tab2 ) )
        {
            // SYSTEM TAB
            // language and country are saved in updateLanguageValues()
            // date style is saved in comboboxDateChanged()
            // hours format is saved in comboboxTimeChanged()

            QString tmpKeypadStyle = (ui->cboPopupKeypadStyle->itemData( ui->cboPopupKeypadStyle->currentIndex())).toString();
            normalScaleOperation_->popupKeypadStyleRaw() =  tmpKeypadStyle;

            // SOF-5086: this is needed otherwise number pad style will revert with first popup dialog after change made in thsi menu
            mainWindow_->numberPad()->setKeyLayoutStyle( (NumberPadWidget::KeyLayoutStyle) tmpKeypadStyle.toInt());

            int kpStyle = ui->cboOpLoginKeyboardStyle->currentIndex() + 1;         // the numeric vals are +1 the current index
            QString kpStyleStr = QString::number(kpStyle);
            normalScaleOperation_->opLoginKeyboardStyleRaw() = kpStyleStr;

            int bccm = ui->cboByCountCalculationMethod->currentIndex();
            bccm = bccm + 1;
            QString bccmStr = QString::number(bccm);
            normalScaleOperation_->byCountCalculationMethodRaw() = bccmStr;

            int isEnabled = ui->cboAlwaysEnableVirtualKeyboard->currentIndex();
            normalScaleOperation_->alwaysEnableVirtualKeyboardRaw() = isEnabled == 1 ? "1" : "0";
        }
        else if( currentTab == ui->tabNormalScaleOperation->indexOf( ui->tab3 ) )
        {
            // NETWORK TAB
            // The Enable network details display in Oeprate Mode has it's own thing in - updateNetworkCheckboxesChanged()
            normalScaleOperation_->checkNetConnectivity() = ui->chkCheckNetConnectivity->isChecked() == true ? "1" : "0";
            normalScaleOperation_->pingPrimaryRaw() = ui->chkPingPrimary->isChecked() == true ? "1" : "0";
            normalScaleOperation_->pingGatewayRaw() = ui->chkPingGateway->isChecked() == true ? "1" : "0";
            normalScaleOperation_->pingTarget() = ui->chkPingTarget->isChecked() == true ? "1" : "0";

            setDomPingInterval();
            setDomPingTarget();

            normalScaleOperation_->periodicOfflineBeep() = ui->txtPeriodicOfflineBeep->text();
        }
        else if( currentTab == ui->tabNormalScaleOperation->indexOf( ui->tab4 ) )
        {
            // PANELS TAB
            QString dcaStartScreenType = (ui->cboAccessBrowserMode->itemData( ui->cboAccessBrowserMode->currentIndex())).toString();
            normalScaleOperation_->dcaStartScreenTypeRaw() = dcaStartScreenType;
            scaleConfigData_->dcaStartScreenTypeRaw() = dcaStartScreenType;
            operatorDisplayFields_->dcaStartScreenTypeRaw() = dcaStartScreenType;

            normalScaleOperation_->startScreenHome() = QString::number( ui->cboStartScreenHome->currentIndex() );

            QString dcaKeypadAreaType  = (ui->cboNumberPad->itemData( ui->cboNumberPad->currentIndex())).toString();
            QString dcaRunScreenType   = (ui->cboRunScreen->itemData( ui->cboRunScreen->currentIndex())).toString();
            QString rightScreenType = (ui->cboRightPanelMode->itemData( ui->cboRightPanelMode->currentIndex())).toString();
            QString bottomScreenType = (ui->cboBottomPanelMode->itemData( ui->cboBottomPanelMode->currentIndex())).toString();

            normalScaleOperation_->startScreenHome()       = QString::number( ui->cboStartScreenHome->currentIndex() );
            normalScaleOperation_->dcaStartScreenTypeRaw() = dcaStartScreenType;
            normalScaleOperation_->dcaRunScreenTypeRaw()   = dcaRunScreenType;
            normalScaleOperation_->dcaKeypadAreaTypeRaw()  = dcaKeypadAreaType;
            normalScaleOperation_->accessBrowserModeRaw()  = ((hobart::dynamicContentWidget::WebBrowser == dcaStartScreenType.toInt()) ? "1" : "0");

            scaleConfigData_->dcaStartScreenTypeRaw() = dcaStartScreenType;
            scaleConfigData_->dcaRunScreenTypeRaw()   = dcaRunScreenType;
            scaleConfigData_->dcaKeypadAreaTypeRaw()  = dcaKeypadAreaType;

            operatorDisplayFields_->dcaStartScreenTypeRaw() = dcaStartScreenType;
            operatorDisplayFields_->dcaRunScreenTypeRaw()   = dcaRunScreenType;
            operatorDisplayFields_->dcaKeypadAreaTypeRaw()  = dcaKeypadAreaType;

            normalScaleOperation_->rightScreenTypeRaw() = rightScreenType;
            scaleConfigData_->rightScreenTypeRaw() = rightScreenType;
            operatorDisplayFields_->rightScreenTypeRaw() = rightScreenType;

            normalScaleOperation_->bottomScreenTypeRaw() = bottomScreenType;
            scaleConfigData_->bottomScreenTypeRaw() = bottomScreenType;
            operatorDisplayFields_->bottomScreenTypeRaw() = bottomScreenType;

            operatorDisplayFields_->enableCountryOfOrginRaw()       = ui->chkEnableCountryOfOrgin->isChecked() == true ? "1" : "0";
            operatorDisplayFields_->enableTrackingNumberRaw()       = ui->chkEnableTrackingNumber->isChecked() == true ? "1" : "0";
            operatorDisplayFields_->enableWrapTypesRaw()            = ui->chkEnableWrapTypes->isChecked() == true ? "1" : "0";
            normalScaleOperation_->showTotalTypeOnRunScreenRaw()    = ui->chkEnableTransType->isChecked() == true ? "1" : "0";
            scaleConfigData_->showTotalTypeOnRunScreenRaw()         = ui->chkEnableTransType->isChecked() == true ? "1" : "0";
            operatorDisplayFields_->enableSellByRaw()               = ui->chkEnableSellBy->isChecked() == true ? "1" : "0";
            operatorDisplayFields_->enableBestBeforeRaw()           = ui->chkEnableBestBefore->isChecked() == true ? "1" : "0";
            operatorDisplayFields_->enablePackedOnRaw()             = ui->chkEnablePackedOn->isChecked() == true ? "1" : "0";

            normalScaleOperation_->enableEnhancedCenterScreenRaw()  = ui->chkEnableEnhancedCenterScreen->isChecked() == true ? "1" : "0";
            scaleConfigData_->enableEnhancedCenterScreenRaw()       = ui->chkEnableEnhancedCenterScreen->isChecked() == true ? "1" : "0";

            // HT-418
            if( dateTimeHasChanged || langHasChanged )
                saveDateTimeFormat();

            saveDisplayOptions();
        }
        else if( currentTab == ui->tabNormalScaleOperation->indexOf( ui->tab5 ) )
        {
            // FEATURES TAB

            if( hasChangedDisplayOptions )
            {
                // 5.0 FLASHKEYS
                operatorDisplayFields_->appendingPluNumberToNameRaw()     = ui->chkAppendingPluNumberToName->isChecked() == true ? "1" : "0";
                operatorDisplayFields_->numberOfCharactersRaw()           = ui->txtNumberOfCharacters->text();
                normalScaleOperation_->resetStatisticsDays()              = ui->txtResetStatisticsDays->text();
                normalScaleOperation_->defaultSearchMethod()              = QString::number( ui->cboDefaultSearchMethod->currentIndex() );
                operatorDisplayFields_->defaultFlashkeyFontPointsizeRaw() = ui->txtDefaultFlashkeyFontPointsize->text();
            }
            
            /* Selected one of the main top tabs away from the Cutting Test tab so now update Normal Scale Operations Config of their values */
            // 5.7 Cutting Test - bc we exist in both modes
            normalScaleOperation_->setCuttingTestType(                 ui->cboCuttingTestType->currentIndex() );        ///< Save cutting test type
            normalScaleOperation_->maxSuspendedTestsRaw()            = ui->txtMaxSuspendedTests->text();                ///< Save suspended tests
            normalScaleOperation_->maxSuspendedTestsPerOperatorRaw() = ui->txtMaxSuspendedTestsPerOperator->text();     ///< Save suspended test/operator
            normalScaleOperation_->inactivitylogoutRaw()             = ui->txtInactivitylogout->text();                 ///< Save inactivity timeout

            if ( !session_->isPrepack() )
            {
                if( hasChangedDisplayOptions )
                {
                    QString rightScreenType =  (ui->cboRightPanelMode->itemData( ui->cboRightPanelMode->currentIndex())).toString();
                    QString bottomScreenType = (ui->cboBottomPanelMode->itemData( ui->cboBottomPanelMode->currentIndex())).toString();

                    // 5.0 FLASHKEYS
                    operatorDisplayFields_->appendingPluNumberToNameRaw()     = ui->chkAppendingPluNumberToName->isChecked() == true ? "1" : "0";
                    operatorDisplayFields_->numberOfCharactersRaw()           = ui->txtNumberOfCharacters->text();
                    normalScaleOperation_->resetStatisticsDays()              = ui->txtResetStatisticsDays->text();
                    normalScaleOperation_->defaultSearchMethod()              = QString::number( ui->cboDefaultSearchMethod->currentIndex() );
                    operatorDisplayFields_->defaultFlashkeyFontPointsizeRaw() = ui->txtDefaultFlashkeyFontPointsize->text();

                    // 5.1 PRODUCTION ENTRY
                    operatorDisplayFields_->enableProdEntryRaw()          = ui->chkEnableProdEntry->isChecked() == true ? "1" : "0";
                    operatorDisplayFields_->prodEntryContinuousPrintRaw() = ui->chkProdEntryContinuousPrint->isChecked() == true ? "1" : "0";
                    normalScaleOperation_->disableCuttingBetweenLabelsRaw() = ui->chkDisableCuttingBetweenLinerlessLabels->isChecked() == true ? "1" : "0";
                    operatorDisplayFields_->enableProdPlanRaw()           = ui->chkEnableProdPlan->isChecked() == true ? "1" : "0";
                    operatorDisplayFields_->prodPlanningFrameRaw()        = ui->txtProdPlanningFrame->text();
                    operatorDisplayFields_->prodPlanningUpdateRaw()       = ui->txtProdPlanningUpdate->text();
                    operatorDisplayFields_->rightScreenTypeRaw() = rightScreenType;
                    operatorDisplayFields_->bottomScreenTypeRaw() = bottomScreenType;

                    // 5.4 SELF SERVICE - currentChangeTab()
                    operatorDisplayFields_->enableSelfServiceRaw()                  = ui->chkEnableSelfService->isChecked() == true ? "1" : "0";

                    // PJR383 - double check all of these saves. I mean look at all of those configs. Did we cover them all?
                    saveSelfServeStartScreen( ui->cboStartWhere->currentIndex() );
                    saveSelfServeItemEntryScreen( ui->cboItemEntryWho->currentIndex() );
                    saveSelfServeSearchScreen( ui->cboSearchHow->currentIndex() );
                    saveSelfServePrintScreen( ui->cboPrintWhat->currentIndex() );
                    saveSelfServeAfterPrintScreen( ui->cboPrintingWhen->currentIndex() );

                    saveSelfServeOptionsGen2Screen();

                    normalScaleOperation_->enableHelpPromptsRaw()                   = ui->chkHelpPrompts->isChecked()           == true ? "1" : "0";
                    normalScaleOperation_->selfServeDisplayTareOnRandomWeightRaw()  = ui->chkSelfServeTareOnRandomWeight->isChecked() == true ? "1" : "0";
                    normalScaleOperation_->selfServeByCountItemsAllowedRaw()        = ui->chkSelfServeByCountItemsAllowed->isChecked() == true ? "1" : "0";

                    normalScaleOperation_->selfServePlaylistFileRaw()               = ui->cboSelfServePlaylist->itemData( ui->cboSelfServePlaylist->currentIndex() ).toString();
                    normalScaleOperation_->selfServeDisplayPluLabelRaw()            = ui->chkSelfServeDisplayPluLabel->isChecked() == true ? "1" : "0";
                    normalScaleOperation_->selfServeStartScreenClickableRaw()       = ui->chkStartScreenClickable->isChecked() == true ? "1" : "0";
                    normalScaleOperation_->selfServePrintOnStableWeightRaw()        = ui->chkSelfServePrintOnStableWeight->isChecked() == true ? "1" : "0";
                    normalScaleOperation_->selfServeIncludeKeypadRaw()              = ui->chkIncludeNumpad->isChecked() == true ? "1" : "0";
                    normalScaleOperation_->selfServeHideButtonBarRaw()              = ui->chkSelfServeHideButtonBar->isChecked() == true ? "1" : "0";
                    normalScaleOperation_->selfServeEnableFreshServeUIRaw()         = ui->chkEnableGen2UI->isChecked() == true ? "1" : "0";

                    //PJR413 (SOF-4772) - FixedPLU
                    normalScaleOperation_->selfServeFixedPluRaw()                   = ui->txtSelfServeFixedPlu->text();

                    scaleConfigData_->selfServePlaylistFileRaw()                    = ui->cboSelfServePlaylist->itemData( ui->cboSelfServePlaylist->currentIndex() ).toString();
                    scaleConfigData_->enableHelpPromptsRaw()                        = ui->chkHelpPrompts->isChecked()           == true ? "1" : "0";
                    scaleConfigData_->selfServeDisplayTareOnRandomWeightRaw()       = ui->chkSelfServeTareOnRandomWeight->isChecked() == true ? "1" : "0";
                    scaleConfigData_->selfServeDisplayPluLabelRaw()                 = ui->chkSelfServeDisplayPluLabel->isChecked() == true ? "1" : "0";
                    scaleConfigData_->selfServeStartScreenClickableRaw()            = ui->chkStartScreenClickable->isChecked() == true ? "1" : "0";
                    scaleConfigData_->selfServePrintOnStableWeightRaw()             = ui->chkSelfServePrintOnStableWeight->isChecked() == true ? "1" : "0";
                    scaleConfigData_->selfServeIncludeKeypadRaw()                   = ui->chkIncludeNumpad->isChecked() == true ? "1" : "0";
                    scaleConfigData_->selfServeHideButtonBarRaw()                   = ui->chkSelfServeHideButtonBar->isChecked() == true ? "1" : "0";
                    scaleConfigData_->selfServeEnableFreshServeUIRaw()              = ui->chkEnableGen2UI->isChecked() == true ? "1" : "0";

                    //PJR413 (SOF-4772) - FixedPLU
                    scaleConfigData_->selfServeFixedPluRaw()                        = ui->txtSelfServeFixedPlu->text();

                    // HT-418
                    if( dateTimeHasChanged || langHasChanged )
                        saveDateTimeFormat();

                    saveDisplayOptions();
                }

                if( tabAccumHasChanged )
                {
                    // 5.2 ACCUMULATE ITEMS
                    accumItemsConfig_->enableAccumItemsRaw() =   ui->chkEnableAccumItems->isChecked() == true ? "1" : "0";
                    normalScaleOperation_->mealsRequireWeightRaw() =   ui->chkRequireWeightForMealItems->isChecked() == true ? "1" : "0";
                    accumItemsConfig_->accumItemsReturnToStartScreenRaw() =   ui->chkAccumItemsReturnToStartScreen->isChecked() == true ? "1" : "0";
                    accumItemsConfig_->readTransactionsCompatibilityRaw() =   ui->chkReadTransactionsCompatibility->isChecked() == true ? "1" : "0";
                    accumItemsConfig_->accumBarcodeUseFirstItemRaw() =   ui->chkBarcodeUseFirstItem->isChecked() == true ? "1" : "0";
                    accumItemsConfig_->enableItemStripModeRaw() =   ui->chkEnableItemStripMode->isChecked() == true ? "1" : "0";
                    accumItemsConfig_->enableAccumItemsTotalsOnlyRaw() =   ui->chkEnableAccumItemsTotalsOnly->isChecked() == true ? "1" : "0";
                    accumItemsConfig_->accumItemsEANFormatString() = ui->butAccumItemsEANFormatString->text();
                    accumItemsConfig_->accumItemsEANPrefix() = ui->txtAccumItemsEANPrefix->text();
                    accumItemsConfig_->accumItemsEANCode() = ui->txtAccumItemsEANCode->text();
                    accumItemsConfig_->accumItemsScaleId() = ui->txtAccumItemsScaleId->text();
                    accumItemsConfig_->accumItemsTransNum() = ui->txtAccumItemsTransNum->text();
                    accumItemsConfig_->enableAccumItemsAndFlashkeysRaw() =   ui->chkEnableAccumItemsAndFlashkeys->isChecked() == true ? "1" : "0";
                    accumItemsConfig_->readBarcodeFromCloudRaw() = ui->chkEnableCloudFeature->isChecked() == true ? "1" : "0";
                    accumItemsConfig_->barcodeServerAddress() = ui->txtAccumItemsServerAddress->text();

                    // HT-418
                    if( dateTimeHasChanged || langHasChanged )
                        saveDateTimeFormat();

                    saveAccumulateItems();
                }

                // 5.3 NOW SERVING
                normalScaleOperation_->setNowServingSysType( ui->cboNowServingType->currentIndex() );
                setDomNowServingServer();
            }
            else
            {
                // 5.5 WRAPPER SETTINGS
                normalScaleOperation_->intakeMode()         = (ui->cboIntakeMode->itemData( ui->cboIntakeMode->currentIndex() )).toString();
                //normalScaleOperation_->labelPlacement()     = (ui->cboLabelPlacement->itemData( ui->cboLabelPlacement->currentIndex() )).toString();
                //normalScaleOperation_->secondaryLabelPlacementRaw()     = (ui->cboLabelPlacement_2->itemData( ui->cboLabelPlacement_2->currentIndex() )).toString();
				// replaced by
                QString priLabelplacement = (ui->cboLabelPlacement->itemData( ui->cboLabelPlacement->currentIndex() )).toString();
                QString secLabelplacement = (ui->cboLabelPlacement_2->itemData( ui->cboLabelPlacement_2->currentIndex() )).toString();

                normalScaleOperation_->labelPlacement()     = priLabelplacement;
                normalScaleOperation_->secondaryLabelPlacementRaw()     = secLabelplacement;
                normalScaleOperation_->filmTension()        = (ui->cboFilmTension->itemData( ui->cboFilmTension->currentIndex() )).toString();
                normalScaleOperation_->tensionFrontToBack() = (ui->cboTensionFront2Back->itemData( ui->cboTensionFront2Back->currentIndex() )).toString();
                normalScaleOperation_->tensionLeftToRight() = (ui->cboTensionLeft2Right->itemData( ui->cboTensionLeft2Right->currentIndex() )).toString();

                // 5.6 SEALER BELT
                normalScaleOperation_->timeOnSun() = ui->le_timeOnSunday->text();
                normalScaleOperation_->timeOnMon() = ui->le_timeOnMonday->text();
                normalScaleOperation_->timeOnTue() = ui->le_timeOnTuesday->text();
                normalScaleOperation_->timeOnWed() = ui->le_timeOnWednesday->text();
                normalScaleOperation_->timeOnThu() = ui->le_timeOnThursday->text();
                normalScaleOperation_->timeOnFri() = ui->le_timeOnFriday->text();
                normalScaleOperation_->timeOnSat() = ui->le_timeOnSaturday->text();

                normalScaleOperation_->timeOffSun() = ui->le_timeOffSunday->text();
                normalScaleOperation_->timeOffMon() = ui->le_timeOffMonday->text();
                normalScaleOperation_->timeOffTue() = ui->le_timeOffTuesday->text();
                normalScaleOperation_->timeOffWed() = ui->le_timeOffWednesday->text();
                normalScaleOperation_->timeOffThu() = ui->le_timeOffThursday->text();
                normalScaleOperation_->timeOffFri() = ui->le_timeOffFriday->text();
                normalScaleOperation_->timeOffSat() = ui->le_timeOffSaturday->text();
            }
        }

        currentTab = tabNum;
    }
}

void SuperNormalScaleOperation::currentChangeSubTab( int tabNum )
{
    // not really doing anything in this method right now beause we are doing it when they press the Features tab instead
    if( currentSubTab != tabNum )
    {
        if( hasChangedDisplayOptions )
        {
            // 5.0 FLASHKEYS
            operatorDisplayFields_->appendingPluNumberToNameRaw()     = ui->chkAppendingPluNumberToName->isChecked() == true ? "1" : "0";
            operatorDisplayFields_->numberOfCharactersRaw()           = ui->txtNumberOfCharacters->text();
            normalScaleOperation_->resetStatisticsDays()              = ui->txtResetStatisticsDays->text();
            normalScaleOperation_->defaultSearchMethod()              = QString::number( ui->cboDefaultSearchMethod->currentIndex() );
            operatorDisplayFields_->defaultFlashkeyFontPointsizeRaw() = ui->txtDefaultFlashkeyFontPointsize->text();

            if ( !session_->isPrepack() )
            {
                // 5.1 PRODUCTION ENTRY
                operatorDisplayFields_->enableProdEntryRaw()          = ui->chkEnableProdEntry->isChecked() == true ? "1" : "0";
                operatorDisplayFields_->prodEntryContinuousPrintRaw() = ui->chkProdEntryContinuousPrint->isChecked() == true ? "1" : "0";
                normalScaleOperation_->disableCuttingBetweenLabelsRaw() = ui->chkDisableCuttingBetweenLinerlessLabels->isChecked() == true ? "1" : "0";
                operatorDisplayFields_->enableProdPlanRaw()           = ui->chkEnableProdPlan->isChecked() == true ? "1" : "0";

                if ( ui->txtProdPlanningFrame->text().isEmpty() )
                    operatorDisplayFields_->prodPlanningFrameRaw()    = "\"\"";
                else
                    operatorDisplayFields_->prodPlanningFrameRaw()    = ui->txtProdPlanningFrame->text();

                if ( ui->txtProdPlanningUpdate->text().isEmpty() )
                    operatorDisplayFields_->prodPlanningUpdateRaw()   = "\"\"";
                else
                    operatorDisplayFields_->prodPlanningUpdateRaw()   = ui->txtProdPlanningUpdate->text();

                // 5.4 SELF SERVICE - currentChangeTab()
                operatorDisplayFields_->enableSelfServiceRaw() = ui->chkEnableSelfService->isChecked() == true ? "1" : "0";

                // jhc TODO pjr383 - make sure there calls are everywhere they should be
                saveSelfServeStartScreen( ui->cboStartWhere->currentIndex() );
                saveSelfServeItemEntryScreen( ui->cboItemEntryWho->currentIndex() );
                saveSelfServeSearchScreen( ui->cboSearchHow->currentIndex() );
                saveSelfServePrintScreen( ui->cboPrintWhat->currentIndex() );
                saveSelfServeAfterPrintScreen( ui->cboPrintingWhen->currentIndex() );

                saveSelfServeOptionsGen2Screen();

                // PJR383 - This is just all kinds of wrong
                //normalScaleOperation_->enableHelpPromptsRaw()                       = ui->chkBestSellersOnSelfServeMainScreen->isChecked() == true ? "1" : "0";

                normalScaleOperation_->enableHelpPromptsRaw()                   = ui->chkHelpPrompts->isChecked() == true ? "1" : "0";
                normalScaleOperation_->selfServeDisplayTareOnRandomWeightRaw()  = ui->chkSelfServeTareOnRandomWeight->isChecked() == true ? "1" : "0";
                normalScaleOperation_->selfServeByCountItemsAllowedRaw()        = ui->chkSelfServeByCountItemsAllowed->isChecked() == true ? "1" : "0";

                normalScaleOperation_->selfServePlaylistFileRaw()               = ui->cboSelfServePlaylist->itemData( ui->cboSelfServePlaylist->currentIndex() ).toString();
                normalScaleOperation_->selfServeDisplayPluLabelRaw()            = ui->chkSelfServeDisplayPluLabel->isChecked() == true ? "1" : "0";
                normalScaleOperation_->selfServeStartScreenClickableRaw()       = ui->chkStartScreenClickable->isChecked() == true ? "1" : "0";
                normalScaleOperation_->selfServePrintOnStableWeightRaw()        = ui->chkSelfServePrintOnStableWeight->isChecked() == true ? "1" : "0";
                normalScaleOperation_->selfServeIncludeKeypadRaw()              = ui->chkIncludeNumpad->isChecked() == true ? "1" : "0";
                normalScaleOperation_->selfServeHideButtonBarRaw()              = ui->chkSelfServeHideButtonBar->isChecked() == true ? "1" : "0";
                normalScaleOperation_->selfServeEnableFreshServeUIRaw()         = ui->chkEnableGen2UI->isChecked() == true ? "1" : "0";

                //PJR413 (SOF-4772) - FixedPLU
                normalScaleOperation_->selfServeFixedPluRaw()                   = ui->txtSelfServeFixedPlu->text();

                // PJR383 - This is just all kinds of wrong
                //scaleConfigData_->enableHelpPromptsRaw()                        = ui->chkBestSellersOnSelfServeMainScreen->isChecked() == true ? "1" : "0";
                scaleConfigData_->enableHelpPromptsRaw()                        = ui->chkHelpPrompts->isChecked() == true ? "1" : "0";
                scaleConfigData_->selfServeDisplayTareOnRandomWeightRaw()       = ui->chkSelfServeTareOnRandomWeight->isChecked() == true ? "1" : "0";
                scaleConfigData_->selfServeDisplayPluLabelRaw()                 = ui->chkSelfServeDisplayPluLabel->isChecked() == true ? "1" : "0";
                scaleConfigData_->selfServeStartScreenClickableRaw()            = ui->chkStartScreenClickable->isChecked() == true ? "1" : "0";
                scaleConfigData_->selfServePrintOnStableWeightRaw()             = ui->chkSelfServePrintOnStableWeight->isChecked() == true ? "1" : "0";
                scaleConfigData_->selfServeIncludeKeypadRaw()                   = ui->chkIncludeNumpad->isChecked() == true ? "1" : "0";
                scaleConfigData_->selfServeHideButtonBarRaw()                   = ui->chkSelfServeHideButtonBar->isChecked() == true ? "1" : "0";
                scaleConfigData_->selfServeEnableFreshServeUIRaw()              = ui->chkEnableGen2UI->isChecked() == true ? "1" : "0";

                //PJR413 (SOF-4772) - FixedPLU
                scaleConfigData_->selfServeFixedPluRaw()                        = ui->txtSelfServeFixedPlu->text();

                // jhc TODO pjr383 SOF-3997 Big Y Self-Serve: Print Label to Screen needs to be automatically selected
                //if( ui->chkSelfServeCombineSearchKeypadFlashkeys->isChecked() )
                //{
                //    // do a quick check if the user unchecked the Print label to screen button, but still has the Combine Search, Keypad, and Flashkeys checked
                //    if( !ui->chkselfServeDisplayLabelOnScreenGO->isChecked() )
                //    {
                //        ui->chkselfServeDisplayLabelOnScreenGO->setChecked( true );
                //        normalScaleOperation_->selfServeDisplayLabelOnScreenGORaw()   = "1";
                //        scaleConfigData_->selfServeDisplayLabelOnScreenGORaw()        = "1";
                //    }
                //}

                //if( 1 == ui->cboPrintWhat->currentIndex() )
                //    ui->chkSelfServeDisplayPluLabel->setVisible( true );
                //else
                //    ui->chkSelfServeDisplayPluLabel->setVisible( false );

                scaleConfigData_->selfServePlaylistFile()                  = ui->cboSelfServePlaylist->itemData( ui->cboSelfServePlaylist->currentIndex() ).toString();

                // HT-418
                if( dateTimeHasChanged || langHasChanged )
                    saveDateTimeFormat();

                saveDisplayOptions();
            }
        }

        if ( !session_->isPrepack() )
        {
            if( tabAccumHasChanged )
            {
                // 5.2 ACCUMULATE ITEMS
                accumItemsConfig_->enableAccumItemsRaw() =   ui->chkEnableAccumItems->isChecked() == true ? "1" : "0";
                normalScaleOperation_->mealsRequireWeightRaw() =   ui->chkRequireWeightForMealItems->isChecked() == true ? "1" : "0";
                accumItemsConfig_->accumItemsReturnToStartScreenRaw() =   ui->chkAccumItemsReturnToStartScreen->isChecked() == true ? "1" : "0";
                accumItemsConfig_->readTransactionsCompatibilityRaw() =   ui->chkReadTransactionsCompatibility->isChecked() == true ? "1" : "0";
                accumItemsConfig_->accumBarcodeUseFirstItemRaw() =   ui->chkBarcodeUseFirstItem->isChecked() == true ? "1" : "0";
                accumItemsConfig_->enableItemStripModeRaw() =   ui->chkEnableItemStripMode->isChecked() == true ? "1" : "0";
                accumItemsConfig_->enableAccumItemsTotalsOnlyRaw() =   ui->chkEnableAccumItemsTotalsOnly->isChecked() == true ? "1" : "0";
                accumItemsConfig_->accumItemsEANFormatString() = ui->butAccumItemsEANFormatString->text();
                accumItemsConfig_->accumItemsEANPrefix() = ui->txtAccumItemsEANPrefix->text();
                accumItemsConfig_->accumItemsEANCode() = ui->txtAccumItemsEANCode->text();
                accumItemsConfig_->accumItemsScaleId() = ui->txtAccumItemsScaleId->text();
                accumItemsConfig_->accumItemsTransNum() = ui->txtAccumItemsTransNum->text();
                accumItemsConfig_->enableAccumItemsAndFlashkeysRaw() =   ui->chkEnableAccumItemsAndFlashkeys->isChecked() == true ? "1" : "0";
                accumItemsConfig_->readBarcodeFromCloudRaw() = ui->chkEnableCloudFeature->isChecked() == true ? "1" : "0";
                accumItemsConfig_->barcodeServerAddress() = ui->txtAccumItemsServerAddress->text();

                // HT-418
                if( dateTimeHasChanged || langHasChanged )
                    saveDateTimeFormat();

                saveAccumulateItems();
            }

            // 5.3 NOW SERVING
            normalScaleOperation_->setNowServingSysType( ui->cboNowServingType->currentIndex() );
            setDomNowServingServer();
        }


        /* Selected another tab under the features tab so now update Normal Scale Operations Config of their values */
        // 5.7 Cutting Test - bc we exist in both modes
        normalScaleOperation_->setCuttingTestType(                 ui->cboCuttingTestType->currentIndex() );    ///< Set the cut Test Type
        normalScaleOperation_->maxSuspendedTestsRaw()            = ui->txtMaxSuspendedTests->text();            ///< Set the number of suspended tests
        normalScaleOperation_->maxSuspendedTestsPerOperatorRaw() = ui->txtMaxSuspendedTestsPerOperator->text(); ///< Set the number of suspended tests/operator
        normalScaleOperation_->inactivitylogoutRaw()             = ui->txtInactivitylogout->text();             ///< Set the inactivity timeout
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Removed compress time fields 05-15-2015 during timesys 1.15 release
//void SuperNormalScaleOperation::flipAmPm()
//{
//    if( isUpdating )
//        return;
//
//    if(ui->butCompressTimeAMPM->text() == "AM")
//        ui->butCompressTimeAMPM->setText("PM");
//    else
//        ui->butCompressTimeAMPM->setText("AM");
//}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::appendingFlashkeyCheckboxChanged( int state )
{
    Q_UNUSED( state );

    if( isUpdating )
        return;

    // used when selecting "appending PLU number to flashkey"
    hasChangedDisplayOptions = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::numOfCharsOnFlashkeyCheckboxChanged(int state)
{
    // These fields are part of the main operatorDisplayFields from tab 5, so they
    // are sharing the same bool even though these are physically on tab 3.
    // In legacy this is "almostClickCheckboxLabel".

    if( isUpdating )
        return;

    hasChangedDisplayOptions = true;

    if( state  == 0)
    {
        // OLD COMMENT FOR REFERENCE
        // a number of 0 in the current database field "numberOfCharacters" means the "Display entire description
        // on flashkey" checkbox is checked and the "Number of Chars" text field is blank and dispabled. But NOW
        // we are going to do the opposite of that AND uncheck the checkbox and initialize the text field to 50
        // UPDATE - "use entire description actually already unchecked by the time we get here. So enable the
        // the "Number of Chars field" and init to 50

        ui->txtNumberOfCharacters->setEnabled( true );          // was it previously disabled? Shoulda been
        ui->lblNumOfChars->setEnabled( true );
        ui->txtNumberOfCharacters->setText("50");
        operatorDisplayFields_->numberOfCharactersRaw() = "50";
    }
    else if( state > 0)
    {
        // OLD COMMENT FOR REFERENCE
        // a > 0 value means the checkbox is currently UNCHECKED and the text field is set to some value
        // so we need to check the checkbox, clear the text field, and then disable it.
        ui->txtNumberOfCharacters->setText("0");
        ui->txtNumberOfCharacters->setEnabled( false );          // was it previously enabled? Shoulda been
        ui->lblNumOfChars->setEnabled( false );
        operatorDisplayFields_->numberOfCharactersRaw() = "0";
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::numOfCharsOnFlashkeyTextChanged()
{
    // These fields are part of the main operatorDisplayFields from tab 5, so they
    // are sharing the same bool even though these are physically on tab 3.

    if( isUpdating )
        return;

    hasChanged = true;
    hasChangedDisplayOptions = true;
    normalScaleOperation_->numOfDigits() = ui->txtNumberOfCharacters->text();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::defaultFlashkeyFontPointsizeChanged()
{
    // This field is part of the main operatorDisplayFields from tab 5, so they
    // are sharing the same bool even though these are physically on tab 3.

    if( isUpdating )
        return;

    hasChanged = true;
    hasChangedDisplayOptions = true;
    operatorDisplayFields_->defaultFlashkeyFontPointsizeRaw() = ui->txtDefaultFlashkeyFontPointsize->text();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::onEanEditorDoneClicked(QString eanString)
{
    ui->butAccumItemsEANFormatString->setText( eanString );

    // Need to find out if we need to update any checkboxes depending on the values in the EAN String
    bool isEnabled = ui->chkEnableAccumItems->isChecked();
    checkEnabledEanFields( isEnabled );

    // need to save this to the class data
    tabAccumHasChanged = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::onEanEditorCancelClicked(QString eanString)
{
    ui->butAccumItemsEANFormatString->setText( eanString );
    // need to save this to the class data
    tabAccumHasChanged = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::resetStatistics()
{
    session_->send(ProductStatisticsDev, ProdStatsDtDeleteAll);     // 126,5 -> reset statistics for flashkeys NOW
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::launchEanEditor()
{
    eanConfigEditor_ = new hobart::eanEditor(this, ui->butAccumItemsEANFormatString->text(), MAX_DIGITS_EAN13, EAN_ACCUM, mainWindow_);
    eanConfigEditor_->move( scalePointForScreen( QPoint( 32, 20 ) ) );
    eanConfigEditor_->show();

    // listening for SIGNALS from the EAN editor
    connect(eanConfigEditor_, SIGNAL( eanEditorDoneClicked( QString ) ), SLOT( onEanEditorDoneClicked( QString )) );
    connect(eanConfigEditor_, SIGNAL( eanEditorCancelClicked( QString ) ), SLOT( onEanEditorCancelClicked( QString )) );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::checkScreenSize()
{
    QDesktopWidget *desktop = QApplication::desktop();

    // retrieve screen resolution
    if ( !desktop )
        QMessageBox::information(this, "OH NO!!!!", "Houston we have a problem!!!!\n\nNo desktop found", QMessageBox::Ok);
    else
    {
        int screen = desktop->primaryScreen();

        if (( 0 <= screen ) && ( screen < desktop->screenCount() ))
        {
            QRect g( desktop->screenGeometry( screen ) );

            QSize resolution_ = g.size();
            QMessageBox::information(this, "Resolution", "Screen resolution is\n\nWidth: " + QString::number(resolution_.width()) + "\nHeight: " + QString::number(resolution_.height()), QMessageBox::Ok);
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::setupPrepackFields(const SessionMessage& msg)
{
    Q_UNUSED( msg );

	QString myTime;
    QString timeFormat = "hh:mm AP";
    QString defaultTimeStr = tr("None");   // HA-189. Was "12:00 AM";

    myTime  = normalScaleOperation_->timeOnSun();
    if (myTime == "")
        myTime = defaultTimeStr;
    ui->le_timeOnSunday->setText(myTime);

    myTime  = normalScaleOperation_->timeOffSun();
    if (myTime == "")
        myTime = defaultTimeStr;
    ui->le_timeOffSunday->setText(myTime);

    myTime  = normalScaleOperation_->timeOnMon();
    if (myTime == "")
        myTime = defaultTimeStr;
    ui->le_timeOnMonday->setText(myTime);

    myTime  = normalScaleOperation_->timeOffMon();
    if (myTime == "")
        myTime = defaultTimeStr;
    ui->le_timeOffMonday->setText(myTime);

    myTime  = normalScaleOperation_->timeOnTue();
    if (myTime == "")
        myTime = defaultTimeStr;
    ui->le_timeOnTuesday->setText(myTime);

    myTime  = normalScaleOperation_->timeOffTue();
    if (myTime == "")
        myTime = defaultTimeStr;
    ui->le_timeOffTuesday->setText(myTime);

    myTime  = normalScaleOperation_->timeOnWed();
    if (myTime == "")
        myTime = defaultTimeStr;
    ui->le_timeOnWednesday->setText(myTime);

    myTime  = normalScaleOperation_->timeOffWed();
    if (myTime == "")
        myTime = defaultTimeStr;
    ui->le_timeOffWednesday->setText(myTime);

    myTime  = normalScaleOperation_->timeOnThu();
    if (myTime == "")
        myTime = defaultTimeStr;
    ui->le_timeOnThursday->setText(myTime);

    myTime  = normalScaleOperation_->timeOffThu();
    if (myTime == "")
        myTime = defaultTimeStr;
    ui->le_timeOffThursday->setText(myTime);

    myTime  = normalScaleOperation_->timeOnFri();
    if (myTime == "")
        myTime = defaultTimeStr;
    ui->le_timeOnFriday->setText(myTime);

    myTime  = normalScaleOperation_->timeOffFri();
    if (myTime == "")
        myTime = defaultTimeStr;
    ui->le_timeOffFriday->setText(myTime);

    myTime  = normalScaleOperation_->timeOnSat();
    if (myTime == "")
        myTime = defaultTimeStr;
    ui->le_timeOnSaturday->setText(myTime);

    myTime  = normalScaleOperation_->timeOffSat();
    if (myTime == "")
        myTime = defaultTimeStr;
    ui->le_timeOffSaturday->setText(myTime);

    setComboBoxIndex(ui->cboIntakeMode, normalScaleOperation_->intakeMode());

    QString priLabelplacement = normalScaleOperation_->labelPlacement();
    QString secLabelplacement = normalScaleOperation_->secondaryLabelPlacementRaw();

    setComboBoxIndex(ui->cboLabelPlacement, normalScaleOperation_->labelPlacement());
    setComboBoxIndex(ui->cboLabelPlacement_2, normalScaleOperation_->secondaryLabelPlacementRaw());
    setFilmTensionComboBoxIndex(ui->cboFilmTension, normalScaleOperation_->filmTension());
    setComboBoxIndex(ui->cboTensionFront2Back, normalScaleOperation_->tensionFrontToBack());
    setComboBoxIndex(ui->cboTensionLeft2Right, normalScaleOperation_->tensionLeftToRight());
}

QString SuperNormalScaleOperation::getTheTime(QDateTimeEdit *myTime)
{
    QString theTime = myTime->sectionText(QDateTimeEdit::HourSection) + ":" + myTime->sectionText(QDateTimeEdit::MinuteSection) + " " + myTime->sectionText(QDateTimeEdit::AmPmSection);
	return theTime;
}

void SuperNormalScaleOperation::editOnOffTimes(QWidget *widget)
{
    if (teDialog_)
        return;
    timeEntryDialog d( offsetMap_, offsetMap_[widget], mainWindow_ );
    teDialog_ = &d;
    d.exec();
    teDialog_ = NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::toggleShelfLifeAmPm()
{
    if( isUpdating )
        return;

    if( isShelfLifeAmSelected )
    {
        ui->butShelfLifeAmPm->setText( "PM" );
        isShelfLifeAmSelected = false;
    }
    else
    {
        ui->butShelfLifeAmPm->setText( "AM" );
        isShelfLifeAmSelected = true;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool SuperNormalScaleOperation::eventFilter( QObject *obj, QEvent *event )
{
    // check for clicking one of the sealer belt time boxes
    if (( sundayOnTime_ == obj ) || ( mondayOnTime_ == obj ) || ( tuesdayOnTime_ == obj ) ||
        ( wednesdayOnTime_ == obj ) || ( thursdayOnTime_ == obj ) || ( fridayOnTime_ == obj ) || ( saturdayOnTime_ == obj ) ||
        ( sundayOffTime_ == obj ) || ( mondayOffTime_ == obj ) || ( tuesdayOffTime_ == obj ) ||
        ( wednesdayOffTime_ == obj ) || ( thursdayOffTime_ == obj ) || ( fridayOffTime_ == obj ) || ( saturdayOffTime_ == obj ))
    {
        if ( QEvent::FocusIn == event->type() )
        {
            editOnOffTimes((QWidget*) obj);
            return true;
        }
    }

    return _Mybase::eventFilter( obj, event );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::onSessionMessage( const SessionMessage& msg )
{
    if (( ConfigurationDev == msg[0].toInt() ) && ( CfgScaleOperationWrite == msg[1].toInt() ))
    {
        // 112,71 - Update normal scale operations, send a 112,70
        isUpdating = true;

        if(!alreadyReceivedScaleOpWriteMessage)
        {
            updateNormalScaleOperations(msg);
            alreadyReceivedScaleOpWriteMessage = true;
        }
        msgCounter++;
    }
    else if( ( ConfigurationDev == msg[0].toInt() ) && ( CfgCurrencyWrite == msg[1].toInt() ) )
    {
        // 112,101 - cant set up validators until we are guaranteed to have received the currency config msg
        isUpdating = true;

        if( neverCallThisCodeAgain == false )
            setupValidators();

        neverCallThisCodeAgain = true;

        isUpdating = false;
        msgCounter++;
    }
    else if (( ConfigurationDev == msg[0].toInt() ) && ( CfgPrepackWrite == msg[1].toInt() ))
    {
        // 112,141 - update operator display fields, send 112,140 (saw a note in legacy that this is never received but
        // received as an overall configuraion field at boot up and when configuration changes. TEST. Will have
        // to set up access like scaleConfig if so
        isUpdating = true;
        if (!alreadyReceivedCfpPrepackMessage)
        {
            updateOperatorDisplayFields(msg);
            alreadyReceivedCfpPrepackMessage = true;
        }
        isUpdating = false;
        msgCounter++;
    }
    else if( ( ConfigurationDev == msg[0].toInt() ) && ( CfgAccumulateItemsWrite == msg[1].toInt() ) )
    {
        // 112,241 - Update normal scale operations, send a 112,240
        isUpdating = true;
        updateAccumulateItems(msg);
        msgCounter++;
        isUpdating = false;
    }
    else if (( Weigher == msg[0].toInt() ) && ( WgInquire == msg[1].toInt() ))
    {
        // 1,2 - update weigher information - only enable maxtare on having a weigher
        updateWeigherFields(msg);
        isUpdating = false;
    }
    else if (( Printer == msg[0].toInt() ) && ( PrCutterStatus == msg[1].toInt() ))  // 7,64
    {
        //--------------------------------------------
        // SOF-3846: Determine if a Cutter is attached
        //--------------------------------------------

        DomPrinterCutterStatus m( msg );

        cutterAttached_ = m.installed();        // based on a wire - physical presence, its there or not

        if( cutterAttached_ )
        {
            ScaleConfiguration *scaleConfig( session_->scaleConfig );
            cutterEnabled_ = scaleConfig->scaleConfig.isCutterEnabled();

            if( cutterEnabled_ )
            {
                ui->chkDisableCuttingBetweenLinerlessLabels->setEnabled( true );

                // SOF-3947: Force "Enable continuous label print in production entry" to be checked
                //           and disabled when "Disable cutting between linerless label printing"
                //           is checked.

                if( ui->chkDisableCuttingBetweenLinerlessLabels->isChecked() )
                {
                    ui->chkProdEntryContinuousPrint->setChecked( true );
                    ui->chkProdEntryContinuousPrint->setEnabled( false );
                }
            }
        }
    }

    if( 4 == msgCounter )
    {
        dataHasBeenInitialized = true;          // only true after all 4 configuration messages received
        ui->tabNormalScaleOperation->setTabEnabled(1,true);
        ui->tabNormalScaleOperation->setTabEnabled(2,true);
        ui->tabNormalScaleOperation->setTabEnabled(3,true);
        ui->tabNormalScaleOperation->setTabEnabled(4,true);
        ui->tabNormalScaleOperation->setTabEnabled(5,true);
        ui->tabNormalScaleOperation->setTabEnabled(6,true);
    }
    else if(6 == msgCounter)
    {
        alreadyReceivedScaleOpWriteMessage = false;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::doxygenImpactAndRegressionTestingComments()
{
    /**
     * The following method will contain only comments to discuss any and all newly added code
     * and its impact to other code modules to identify regression test requirements
     */

    ///< Cutting Test feature impact on existing code.
    ///< Impact: None known. There are times when reading and new configuration can have an impact
    ///< on existing code.  This is especialy true when it comes to modifying/reusing/inserting new
    ///< configuration values in the middle of a configuration structure.  In this case the 4 new
    ///< values were added to the very end of Normal Scale Operations and Scale Configuration structures
    ///< so we are either going to read and save these values, or not, with no negative impact forseen.

    ///
}

void SuperNormalScaleOperation::populateSecondaryPositions()
{
    int primaryIndex = ui->cboLabelPlacement->currentData().toInt();

    // need to check if secondary is not in a conflicting position, if its not then we dont need to set the secondary to system default
    int currentSecondaryIndex = ui->cboLabelPlacement_2->currentIndex();
    int currentSecondaryData = ui->cboLabelPlacement_2->currentData().toInt();
    switch( primaryIndex )
    {
        case upperRight:
        {
            ui->cboLabelPlacement_2->clear();
            ui->cboLabelPlacement_2->addItem(tr("Upper Left"), "1");
            ui->cboLabelPlacement_2->addItem(tr("Lower Right"), "3");
            ui->cboLabelPlacement_2->addItem(tr("Lower Left"), "4");
            ui->cboLabelPlacement_2->addItem(tr("Left of Primary"), "6");
            ui->cboLabelPlacement_2->addItem(tr("Below Primary"), "8");

            ui->cboLabelPlacement_2->setItemText(0, tr("Upper Left"));
            ui->cboLabelPlacement_2->setItemText(1, tr("Lower Right"));
            ui->cboLabelPlacement_2->setItemText(2, tr("Lower Left"));
            ui->cboLabelPlacement_2->setItemText(3, tr("Left of Primary"));
            ui->cboLabelPlacement_2->setItemText(4, tr("Below Primary"));

            if( currentSecondaryData != upperRight && currentSecondaryData != rightOfPrimary && currentSecondaryData != abovePrimary )
            {
                ui->cboLabelPlacement_2->setCurrentIndex( currentSecondaryIndex );
            }
            break;
        }
        case upperLeft:
        {
            ui->cboLabelPlacement_2->clear();
            ui->cboLabelPlacement_2->addItem(tr("Upper Right"), "0");
            ui->cboLabelPlacement_2->addItem(tr("Lower Right"), "3");
            ui->cboLabelPlacement_2->addItem(tr("Lower Left"), "4");
            ui->cboLabelPlacement_2->addItem(tr("Right of Primary"), "5");
            ui->cboLabelPlacement_2->addItem(tr("Below Primary"), "8");

            ui->cboLabelPlacement_2->setItemText(0, tr("Upper Right"));
            ui->cboLabelPlacement_2->setItemText(1, tr("Lower Right"));
            ui->cboLabelPlacement_2->setItemText(2, tr("Lower Left"));
            ui->cboLabelPlacement_2->setItemText(3, tr("Right of Primary"));
            ui->cboLabelPlacement_2->setItemText(4, tr("Below Primary"));

            if( currentSecondaryData != upperLeft && currentSecondaryData != leftOfPrimary && currentSecondaryData != abovePrimary )
            {
                ui->cboLabelPlacement_2->setCurrentIndex( currentSecondaryIndex );
            }
            break;
        }
        case lowerRight:
        {
            ui->cboLabelPlacement_2->clear();
            ui->cboLabelPlacement_2->addItem(tr("Upper Right"), "0");
            ui->cboLabelPlacement_2->addItem(tr("Upper Left"), "1");
            ui->cboLabelPlacement_2->addItem(tr("Lower Left"), "4");
            ui->cboLabelPlacement_2->addItem(tr("Left of Primary"), "6");
            ui->cboLabelPlacement_2->addItem(tr("Above Primary"), "7");

            ui->cboLabelPlacement_2->setItemText(0, tr("Upper Right"));
            ui->cboLabelPlacement_2->setItemText(1, tr("Upper Left"));
            ui->cboLabelPlacement_2->setItemText(2, tr("Lower Left"));
            ui->cboLabelPlacement_2->setItemText(3, tr("Left of Primary"));
            ui->cboLabelPlacement_2->setItemText(4, tr("Above Primary"));

            if( currentSecondaryData != lowerRight && currentSecondaryData != rightOfPrimary && currentSecondaryData != belowPrimary )
            {
                ui->cboLabelPlacement_2->setCurrentIndex( currentSecondaryIndex );
            }
            break;
        }
        case lowerLeft:
        {
            ui->cboLabelPlacement_2->clear();
            ui->cboLabelPlacement_2->addItem(tr("Upper Right"), "0");
            ui->cboLabelPlacement_2->addItem(tr("Upper Left"), "1");
            ui->cboLabelPlacement_2->addItem(tr("Lower Right"), "3");
            ui->cboLabelPlacement_2->addItem(tr("Right of Primary"), "5");
            ui->cboLabelPlacement_2->addItem(tr("Above Primary"), "7");

            ui->cboLabelPlacement_2->setItemText(0, tr("Upper Right"));
            ui->cboLabelPlacement_2->setItemText(1, tr("Upper Left"));
            ui->cboLabelPlacement_2->setItemText(2, tr("Lower Right"));
            ui->cboLabelPlacement_2->setItemText(3, tr("Right of Primary"));
            ui->cboLabelPlacement_2->setItemText(4, tr("Above Primary"));

            if( currentSecondaryData != lowerLeft && currentSecondaryData != leftOfPrimary && currentSecondaryData != belowPrimary )
            {
                ui->cboLabelPlacement_2->setCurrentIndex( currentSecondaryIndex );
            }
            break;
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void SuperNormalScaleOperation::calibrateWrapper()
{
    WrapperCalibrationDialog d( mainWindow_ );
    d.exec();
}
