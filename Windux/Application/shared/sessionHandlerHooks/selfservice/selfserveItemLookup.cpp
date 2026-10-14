/**
 * @file selfserveItemLookup.cpp
 *
 * @copyright Copyright (c) 2014, ITW Hobart Food Equipment Group, U.S.A.
 * @author Doug Antonides
 */

#include "selfserveItemLookup.h"
#include "ui_selfserveItemLookup.h"

#include "selfservicenumberpad.h"
#include "selfservicesoftkeybutton.h"

#include <filesystem.h>
#include <selfservicehook.h>
#include <softkeyswidget.h>

#include <ScaleConfiguration.h>
#include <ScaleClasses.h>

#include <QIntValidator>
#include <QDesktopWidget>

//const bool TURN_ON_LARRY_FEATURE = true;

// Hobart namespace
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////
selfserveItemLookup::selfserveItemLookup( selfServiceHook *h, QWidget *parent ) :
    _Mybase( parent ),
    ui(new Ui::selfserveItemLookup),
    scaleConfig_(NULL),
    hook_( h )
{
    ui->setupUi(this);

    // Main Lookup Screen, display flashkeys(MCI or Cats) with or without fixed rows and cols, flashkeys & keypad, or just a kaypad
    initialize();
    translate();

    connect( hook_, SIGNAL( reloadImages() ), SLOT( onReloadImages() ) );

    // selfServeIncludeKeypad() here is including the keypad along with the flashkeys at the same time on the screen

    // pjr383 - block timer - only if there is no keypad even available - not based on feature
    if ( ( !scaleConfig_->scaleConfig.selfServeIncludeKeypad() ) && ( SelfServicePrintOption::SelfServOption_LookupKeypadOnly != scaleConfig_->scaleConfig.selfServeItemEntryScreenState() ) )
        connect( ui->numPad, SIGNAL( textChanged(QString) ), hook_, SLOT( resetIdleTimer() ) );

    // selfServeIncludeKeypad          = checkbox include Numpad
    // SelfServOption_LookupKeypadOnly = keypad only (red apple). That is your entry method
    // Need third method, ui->cboItemEntryWho = Product Flashkeys with KeypadMenu (item 2 in menu)
    if ( ( scaleConfig_->scaleConfig.selfServeIncludeKeypad() ) || ( SelfServicePrintOption::SelfServOption_LookupKeypadOnly == scaleConfig_->scaleConfig.selfServeItemEntryScreenState() ) || ( SelfServicePrintOption::SelfServOption_LookupCategoriesShowKeypad == scaleConfig_->scaleConfig.selfServeItemEntryScreenState() ) )
        connect( ui->numPad, SIGNAL( lookup() ), SIGNAL( lookup() ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
selfserveItemLookup::~selfserveItemLookup()
{
    if (scaleConfig_ )
        delete scaleConfig_;

    delete ui;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString selfserveItemLookup::itemCode() const
{
    QString text( ui->numPad->text() );

    // clear contents
    ui->numPad->clear();
    ui->numPad->setDisplayBarcodeOnUserInterface( hook_->fieldValue( sessionHooks::selfServiceDisplayBarcodeLabel ).toInt() );
    ui->numPad->displayStackedWidget(0);
    return text;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveItemLookup::loadSoftkeyGroup( const DomLoadSoftkeyGroup* m )
{
    bestSellers_->create( m );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveItemLookup::refresh()
{
    // OPTIONS
    // Lookup											Search
    // ========================================================================
    // Category Flashkeys (1)							A-Z
    // Best Sellers Flashkeys (0)						Char by Char
    // Categories - Show Keypad (2)					    None
    // Fixed Flashkeys  (3)
    // Number Pad Only  (4) (Red Apple - Chile)


    // pjr383
    // SOF-4354: Keypad Option does not work - Completely rework these tests

    // combine the keypad on the screen at the same time with flashkeys
    bool includeNumpadWithFlashkeys = scaleConfig_->scaleConfig.selfServeIncludeKeypad();
    int categories = scaleConfig_->scaleConfig.selfServeItemEntryScreenState();

    ui->numPad->setStyle( hook_->fieldValue( sessionHooks::numberPadStyle ) );
    // due to adding Print/Enter key to numPad. The key is an ENTER key(0) to do a PLU Lookup or a Print Key(1)
    ui->numPad->setEnterText(0);            // 0 = Enter Text
    ui->numPad->setEnterResponse(0);        // 1 = Enter Action

    // state if we need the numpad with 2021 refresh, ie, LARRYS_FEATURE?
    ui->numPad->setDisplayBarcodeOnUserInterface( hook_->fieldValue( sessionHooks::selfServiceDisplayBarcodeLabel ).toInt() );

    // update auto lookup parameters
    ui->numPad->setAutoLookupProduct( ("1" == hook_->fieldValue( sessionHooks::autoLookupProduct )) );
    ui->numPad->setAutoLookupNumOfDigits( hook_->fieldValue( sessionHooks::autoLookupNumOfDigits ).toInt() );
    ui->numPad->setAutoLookupOverrideDigit( hook_->fieldValue( sessionHooks::autoLookupOverrideDigit ).toInt() );

    if( SelfServicePrintOption::SelfServOption_LookupKeypadOnly == categories )
    {
        // OPTION: Keypad only - no flashkeys - show a red apple and keypad
        ui->stackedWidget->setCurrentWidget( ui->keyedItemPage );       // hide flashkeys
        ui->numPad->show();                                             // show keypad
    }
    else if( SelfServicePrintOption::SelfServOption_LookupBestSellers == categories )
    {
        ui->stackedWidget->setCurrentWidget( ui->bestSellersPage );

        // now are we displaying Best Sellers with a keypad with it at the same time - or not
        if( includeNumpadWithFlashkeys )
            ui->numPad->show();             // show keypad
        else
            ui->numPad->hide();
    }
    else if( SelfServicePrintOption::SelfServOption_LookupCategories == categories )
    {
        ui->stackedWidget->setCurrentWidget( ui->bestSellersPage );

        // now are we displaying Best Sellers with a keypad with it at the same time - or not
        if( includeNumpadWithFlashkeys )
            ui->numPad->show();             // show keypad
        else
            ui->numPad->hide();
    }
    else if( SelfServicePrintOption::SelfServOption_LookupFixedFlashkeys == categories )
    {
        ui->stackedWidget->setCurrentWidget( ui->bestSellersPage );

        // now are we displaying Best Sellers with a keypad with it at the same time - or not
        if( includeNumpadWithFlashkeys )
            ui->numPad->show();             // show keypad
        else
            ui->numPad->hide();
    }
    else if( SelfServicePrintOption::SelfServOption_LookupCategoriesShowKeypad == categories )
    {
        ui->stackedWidget->setCurrentWidget( ui->bestSellersPage );

        // now are we displaying Best Sellers with a keypad with it at the same time - or not

        // This is going to become its own checkbox, not an Item Lookup index.  This will need updated.
        // With this option, having the menu button option to click a button and get flashkeys or numpad,
        // the who intent was to have a full screen flashkeys then a button. So I think I ALWAYS have to hide
        // the numpad, unless the numpad button was clicked.  Disable the option [X] Include Keypad

        // show keypad(1) only or flashkeys(0) only
        if(  "0" == hook_->fieldValue( sessionHooks::selfServiceKeyEntryStyle ) )
        {
            // selfServiceKeyEntryStyle equals 0 which is what it has RECENTLY been set to, so now display the flashkeys(0)
            ui->stackedWidget->setCurrentWidget( ui->bestSellersPage );
            ui->numPad->hide();             // show flashkeys hide keypad
        }
        else
        {
            // selfServiceKeyEntryStyle equals 0 which is what it has RECENTLY been set to, so now display the numberpad(1)
            ui->stackedWidget->setCurrentWidget( ui->keyedItemPage );
            ui->numPad->show();             // show keypad hide flashkeys
        }
    }
    else
    {
        // OPTION: This is the actual original default if everything falls though
        // OPTION: Keypad only - no flashkeys - show a red apple and keypad
        ui->stackedWidget->setCurrentWidget( ui->keyedItemPage );   // hide flashkeys
        ui->numPad->show();                                         // show keypad
    }

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveItemLookup::translate()
{
    ui->retranslateUi(this);

    // pjr383
    if( !scaleConfig_->scaleConfig.selfServeIncludeKeypad() &&
            SelfServicePrintOption::SelfServOption_LookupKeypadOnly != scaleConfig_->scaleConfig.selfServeItemEntryScreenState()  )
    {
        ui->numPad->hide();
    }
    else
    {
        ui->numPad->setInstructions( tr( "Enter Item Code, then press Enter." ) );
        ui->numPad->translate();
    }

    bestSellers_->sendResetMessage( true );

    update();
}

void selfserveItemLookup::resetEntry()
{
    ui->numPad->clear();
    ui->numPad->setInstructions(tr ("Enter Item Code, then press Enter."));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveItemLookup::showEvent( QShowEvent *e )
{
    // OPTIONS
    // Lookup											Search
    // ========================================================================
    // Category Flashkeys (1)							A-Z
    // Best Sellers Flashkeys (0)						Char by Char
    // Categories - Show Keypad (2)					    None
    // Fixed Flashkeys  (3)
    // Number Pad Only  (4) (Red Apple - Chile)


    // pjr383
    // SOF-4354: Keypad Option does not work - Completely rework these tests

    // combine the keypad on the screen at the same time with flashkeys
    bool includeNumpadWithFlashkeys = scaleConfig_->scaleConfig.selfServeIncludeKeypad();
    int categories = scaleConfig_->scaleConfig.selfServeItemEntryScreenState();

    ui->numPad->setStyle( hook_->fieldValue( sessionHooks::numberPadStyle ) );
    // due to adding Print/Enter key to numPad. The key is an ENTER key(0) to do a PLU Lookup or a Print Key(1)
    ui->numPad->setEnterText(0);            // 0 = Enter Text
    ui->numPad->setEnterResponse(0);        // 1 = Enter Action

    // state if we need the numpad with 2021 refresh, ie, LARRYS_FEATURE?
    ui->numPad->setDisplayBarcodeOnUserInterface( hook_->fieldValue( sessionHooks::selfServiceDisplayBarcodeLabel ).toInt() );

    // update auto lookup parameters
    ui->numPad->setAutoLookupProduct( ("1" == hook_->fieldValue( sessionHooks::autoLookupProduct )) );
    ui->numPad->setAutoLookupNumOfDigits( hook_->fieldValue( sessionHooks::autoLookupNumOfDigits ).toInt() );
    ui->numPad->setAutoLookupOverrideDigit( hook_->fieldValue( sessionHooks::autoLookupOverrideDigit ).toInt() );

    if( SelfServicePrintOption::SelfServOption_LookupKeypadOnly == categories )
    {
        // OPTION: Keypad only - no flashkeys - show a red apple and keypad
        ui->stackedWidget->setCurrentWidget( ui->keyedItemPage );       // hide flashkeys
        ui->numPad->show();                                             // show keypad
    }
    else if( SelfServicePrintOption::SelfServOption_LookupBestSellers == categories )
    {
        ui->stackedWidget->setCurrentWidget( ui->bestSellersPage );

        // now are we displaying Best Sellers with a keypad with it at the same time - or not
        if( includeNumpadWithFlashkeys )
            ui->numPad->show();             // show keypad
        else
            ui->numPad->hide();
    }
    else if( SelfServicePrintOption::SelfServOption_LookupCategories == categories )
    {
        ui->stackedWidget->setCurrentWidget( ui->bestSellersPage );

        // now are we displaying Best Sellers with a keypad with it at the same time - or not
        if( includeNumpadWithFlashkeys )
            ui->numPad->show();             // show keypad
        else
            ui->numPad->hide();
    }
    else if( SelfServicePrintOption::SelfServOption_LookupFixedFlashkeys == categories )
    {
        ui->stackedWidget->setCurrentWidget( ui->bestSellersPage );

        // now are we displaying Best Sellers with a keypad with it at the same time - or not
        if( includeNumpadWithFlashkeys )
            ui->numPad->show();             // show keypad
        else
            ui->numPad->hide();
    }
    else if( SelfServicePrintOption::SelfServOption_LookupCategoriesShowKeypad == categories )
    {
        ui->stackedWidget->setCurrentWidget( ui->bestSellersPage );

        // now are we displaying Best Sellers with a keypad with it at the same time - or not

        // This is going to become its own checkbox, not an Item Lookup index.  This will need updated.
        // With this option, having the menu button option to click a button and get flashkeys or numpad,
        // the who intent was to have a full screen flashkeys then a button. So I think I ALWAYS have to hide
        // the numpad, unless the numpad button was clicked.  Disable the option [X] Include Keypad

        // show keypad(1) only or flashkeys(0) only
        if(  "0" == hook_->fieldValue( sessionHooks::selfServiceKeyEntryStyle ) )
        {
            // selfServiceKeyEntryStyle equals 0 which is what it has RECENTLY been set to, so now display the flashkeys(0)
            ui->stackedWidget->setCurrentWidget( ui->bestSellersPage );
            ui->numPad->hide();             // show flashkeys hide keypad
        }
        else
        {
            // selfServiceKeyEntryStyle equals 0 which is what it has RECENTLY been set to, so now display the numberpad(1)
            ui->stackedWidget->setCurrentWidget( ui->keyedItemPage );
            ui->numPad->show();             // show keypad hide flashkeys
        }
    }
    else
    {
        // OPTION: This is the actual original default if everything falls though
        // OPTION: Keypad only - no flashkeys - show a red apple and keypad
        ui->stackedWidget->setCurrentWidget( ui->keyedItemPage );   // hide flashkeys
        ui->numPad->show();                                         // show keypad
    }

    bestSellers_->sendResetMessage( true );

    scaleConfig_->internalConfig.selfServicePreventClicks = false;

    _Mybase::showEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveItemLookup::onReloadImages()
{
    QPixmap logo( hook_->storeLogo() );

    // set store logo
    if ( logo.isNull() )
        ui->lblStoreLogo->hide();
    else
    {
        // set size
        ui->lblStoreLogo->setFixedSize( logo.size() );

        // set image
        QPalette palette( ui->lblStoreLogo->palette() );
        palette.setBrush( ui->lblStoreLogo->backgroundRole(), logo );
        ui->lblStoreLogo->setPalette( palette );

        // show!
        ui->lblStoreLogo->setAutoFillBackground( true );
        ui->lblStoreLogo->show();
    }

    QPixmap itemLookup( filesystem::absoluteFilePath( "itemLookup.png", folderType::SelfServeImages ) );

    // set item lookup image
    if ( itemLookup.isNull() )
        ui->lblItemSticker->hide();
    else
    {
        // set size
        ui->lblItemSticker->setFixedSize( itemLookup.size() );

        // set image
        QPalette palette( ui->lblItemSticker->palette() );
        palette.setBrush( ui->lblItemSticker->backgroundRole(), itemLookup );
        ui->lblItemSticker->setPalette( palette );

        // show!
        ui->lblItemSticker->setAutoFillBackground( true );
        ui->lblItemSticker->show();
    }

    // refresh ui
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveItemLookup::initialize()
{
    // init scaleConfig
    Session *s = hook_->session();
    scaleConfig_ = s->scaleConfig;

    // load images
    onReloadImages();

    // pjr383
    // set number pad validator
    //if ( scaleConfig_->scaleConfig.SelfServeWeightRequiredToSelectItem() || scaleConfig_->scaleConfig.SelfServeEnablePlaylists() )
    if( !scaleConfig_->scaleConfig.selfServeIncludeKeypad() )
        ui->numPad->hide();
    else
        ui->numPad->setValidator( new QIntValidator( 0, 999999, this ) );

    // best sellers
    bestSellers_ = new hobart::softkeysWidget( hook_->session(), ui->bestSellersPage );
    bestSellers_->setAutoFillBackground( true );

    // pjr383
    //int tester1 = scaleConfig_->scaleConfig.selfServeItemEntryScreenState();
    //int tester2 = scaleConfig_->scaleConfig.selfServeSearchScreenState();
    if( ( SelfServicePrintOption::SelfServOption_LookupCategories == scaleConfig_->scaleConfig.selfServeItemEntryScreenState() ) ||
        //( SelfServicePrintOption::SelfServOption_SearchNone == scaleConfig_->scaleConfig.selfServeSearchScreenState() ) || // Removed for SOF-4357
        ( SelfServicePrintOption::SelfServOption_LookupCategoriesShowKeypad == scaleConfig_->scaleConfig.selfServeItemEntryScreenState() ) )
    {
        bestSellers_->setCreateFilter( hobart::softkeysWidget::SelfservProductGrid );           // Search on Preprogrammed Flashkeys
    }
    else if( ( SelfServicePrintOption::SelfServOption_LookupFixedFlashkeys == scaleConfig_->scaleConfig.selfServeItemEntryScreenState() ) )
    {
        bestSellers_->setCreateFilter( hobart::softkeysWidget::FixedRowColumnGrid );           // Search on Fixed Flashkeys
    }
    else
    {
        bestSellers_->setCreateFilter( hobart::softkeysWidget::MostCommonlyUsedItems );         // Search for all MCI
    }

    bestSellers_->setFactory( new selfServiceSoftkeyButtonFactory() );
    connect( bestSellers_, SIGNAL( clicked(QWidget*,QWidget*,int,QString,int) ), hook_, SLOT( emitSoftkeyClicked(QWidget*,QWidget*,int,QString,int) ) );

    ui->bestSellersPage->layout()->addWidget( bestSellers_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace
