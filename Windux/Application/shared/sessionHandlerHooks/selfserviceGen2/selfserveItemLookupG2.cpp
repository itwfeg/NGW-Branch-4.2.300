/**
 * @file selfserveItemLookup.cpp
 *
 * @copyright Copyright (c) 2023, ITW Hobart Food Equipment Group, U.S.A.
 * @author Joe E. Crew
 * @borrowedfrom Doug Antonides
 */

#include "selfserveItemLookupG2.h"
#include "ui_selfserveItemLookupG2.h"

#include "selfservicesoftkeybuttonG2.h"

#include <logmanager.h>
#include <filesystem.h>
#include <selfservicehook.h>
#include <softkeyswidget.h>

#include <ScaleConfiguration.h>
#include <UiUtilities.h>
#include <ScaleClasses.h>
#include <QMouseEvent>
#include <QDesktopWidget>
#include <QGraphicsOpacityEffect>
#include <QSize>
#include <QIntValidator>

// Hobart namespace
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////
selfserveItemLookupG2::selfserveItemLookupG2( selfServiceHook *h, QWidget *parent ) :
    _Mybase( parent ),
    ui(new Ui::selfserveItemLookupG2),
    scaleConfig_(NULL),
    hook_( h ),
    curSeparator_("."),         // make decimal the default
    activeKey_( NULL ),
    autoLookupProduct_( false ),
    autoLookupNumOfDigits_( 0 ),
    autoLookupOverrideDigit_( 0 ),
    validator_( NULL )
{
    ui->setupUi(this);

    // Main Lookup Screen, display flashkeys with fixed rows and cols with a kaypad
    initialize();
    translate();

    connect( hook_, SIGNAL( reloadImages() ), SLOT( onReloadImages() ) );

    //connect( pluNum_, SIGNAL( textChanged(QString) ), SLOT( onNumberPadTextChanged(QString) ) );
    connect( pluNum_,           SIGNAL( textChanged(QString) ), hook_, SLOT( resetIdleTimer() ) );
    //connect( pluNum_, SIGNAL( lookup() ), SIGNAL( lookup() ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
selfserveItemLookupG2::~selfserveItemLookupG2()
{
    if ( scaleConfig_ )
        delete scaleConfig_;

    delete ui;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString selfserveItemLookupG2::itemCode() const
{
    QString text( pluNum_->text() );

    // clear contents
    pluNum_->clear();

    return text;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveItemLookupG2::loadSoftkeyGroup( const DomLoadSoftkeyGroup* m )
{
    bestSellers_->create( m );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveItemLookupG2::refresh()
{
    // update auto lookup parameters
    setAutoLookupProduct( ("1" == hook_->fieldValue( sessionHooks::autoLookupProduct )) );
    setAutoLookupNumOfDigits( hook_->fieldValue( sessionHooks::autoLookupNumOfDigits ).toInt() );
    setAutoLookupOverrideDigit( hook_->fieldValue( sessionHooks::autoLookupOverrideDigit ).toInt() );

    QString buttonText( tr( "Language" ) );
    QString lang;

    // figure out what the other/toggled language is
    QString currentLang = hook_->currentLanguage();
    QString priLang = hook_->fieldValue( sessionHooks::primaryLanguage );
    QString secLang = hook_->fieldValue( sessionHooks::secondaryLanguage );

    if ( hook_->currentLanguage() == hook_->fieldValue( sessionHooks::secondaryLanguage ) )
        lang = hook_->fieldValue( sessionHooks::primaryLanguage );
    else
        lang = hook_->fieldValue( sessionHooks::secondaryLanguage );

    // create text with native language
    if ( "ES.MX" == lang )
        buttonText = QString::fromUtf8( "Español" );
    else if ( "FR.CA" == lang )
        buttonText = QString::fromUtf8( "Français" );
    else if ( "EN.US" == lang )
        buttonText = QString::fromUtf8( "English" );

    toggleLanguage_->setText( buttonText );
    QString butTextNow = toggleLanguage_->text();

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveItemLookupG2::translate()
{
    ui->retranslateUi(this);

    QString buttonText( tr( "Language" ) );
    QString lang;

    // figure out what the other/toggled language is
    if ( hook_->currentLanguage() == hook_->fieldValue( sessionHooks::secondaryLanguage ) )
        lang = hook_->fieldValue( sessionHooks::primaryLanguage );
    else
        lang = hook_->fieldValue( sessionHooks::secondaryLanguage );

    // create text with native language
    if ( "ES.MX" == lang )
        buttonText = QString::fromUtf8( "Español" );
    else if ( "FR.CA" == lang )
        buttonText = QString::fromUtf8( "Français" );
    else if ( "EN.US" == lang )
        buttonText = QString::fromUtf8( "English" );

    toggleLanguage_->setText( buttonText );

    if ( hook_->currentLanguage() == hook_->fieldValue( sessionHooks::primaryLanguage ) )
    {
        ui->butSearchBestSellers->setText( scaleConfig_->scaleConfig.selfServeFreshServeFlashkeyText0() );
        ui->butSearchFruits->setText( scaleConfig_->scaleConfig.selfServeFreshServeFlashkeyText1() );
        ui->butSearchVeggies->setText( scaleConfig_->scaleConfig.selfServeFreshServeFlashkeyText2() );
        ui->butSearchOrganic->setText( scaleConfig_->scaleConfig.selfServeFreshServeFlashkeyText3() );
        ui->butSearchOther->setText( scaleConfig_->scaleConfig.selfServeFreshServeFlashkeyText4() );
    }
    else
    {
        ui->butSearchBestSellers->setText( scaleConfig_->scaleConfig.selfServeFreshServeFlashkeySecText0() );
        ui->butSearchFruits->setText( scaleConfig_->scaleConfig.selfServeFreshServeFlashkeySecText1() );
        ui->butSearchVeggies->setText( scaleConfig_->scaleConfig.selfServeFreshServeFlashkeySecText2() );
        ui->butSearchOrganic->setText( scaleConfig_->scaleConfig.selfServeFreshServeFlashkeySecText3() );
        ui->butSearchOther->setText( scaleConfig_->scaleConfig.selfServeFreshServeFlashkeySecText4() );
    }

    refresh();

    // added for fixed flashkeys
    //bestSellers_->sendResetMessage( true );
    resetBestSellers();

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveItemLookupG2::updateNetworkConnectivity(bool online)
{
    if( online )
        screen_->setStyleSheet( QLatin1String("background-color: rgb(244,247,250);" ) );
    else
        screen_->setStyleSheet( QLatin1String("background-color: rgb(255,0,0);" ) );

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveItemLookupG2::showEvent( QShowEvent *e )
{
    bool metricScale( "1" == hook_->fieldValue( sessionHooks::metricScale ) );

    QFont ssfont;
    ssfont.setFamily( "Poppins" );
    ssfont.setBold( true );
    if( metricScale )
    {
        ssfont.setPointSize( 36 );
        weight_->setFont( ssfont );
        weight_->move( 20,33 );
    }
    else
    {
        ssfont.setPointSize( 42 );
        weight_->setFont( ssfont );
        weight_->move( 20,36 );
    }

    resetScreen();

    //resetBestSellers();    // may not need this, because its done in previous line with resetScreen()

    refreshWeight();

    scaleConfig_->internalConfig.selfServicePreventClicks = false;

    search_->setEnabled( false );               // set disabled for 1 second
    cycleNumpadTimer_->start( 1000 );
    cycleNumpadTimer_->setSingleShot(true);

    if ( hook_->currentLanguage() == hook_->fieldValue( sessionHooks::primaryLanguage ) )
    {
        ui->butSearchBestSellers->setText( scaleConfig_->scaleConfig.selfServeFreshServeFlashkeyText0() );
        ui->butSearchFruits->setText( scaleConfig_->scaleConfig.selfServeFreshServeFlashkeyText1() );
        ui->butSearchVeggies->setText( scaleConfig_->scaleConfig.selfServeFreshServeFlashkeyText2() );
        ui->butSearchOrganic->setText( scaleConfig_->scaleConfig.selfServeFreshServeFlashkeyText3() );
        ui->butSearchOther->setText( scaleConfig_->scaleConfig.selfServeFreshServeFlashkeyText4() );
    }
    else
    {
        ui->butSearchBestSellers->setText( scaleConfig_->scaleConfig.selfServeFreshServeFlashkeySecText0() );
        ui->butSearchFruits->setText( scaleConfig_->scaleConfig.selfServeFreshServeFlashkeySecText1() );
        ui->butSearchVeggies->setText( scaleConfig_->scaleConfig.selfServeFreshServeFlashkeySecText2() );
        ui->butSearchOrganic->setText( scaleConfig_->scaleConfig.selfServeFreshServeFlashkeySecText3() );
        ui->butSearchOther->setText( scaleConfig_->scaleConfig.selfServeFreshServeFlashkeySecText4() );
    }

    update();

    _Mybase::showEvent( e );
}

void selfserveItemLookupG2::resetEntry()
{
    pluNum_->clear();
}

//////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveItemLookupG2::dimScreenOpacity()
{
    QGraphicsOpacityEffect* opacityEffect = new QGraphicsOpacityEffect(this);
    opacityEffect->setOpacity(.30);
    ui->screenContainer->setGraphicsEffect( opacityEffect );

    // ui->screenContainer->setStyleSheet("background: rgb(18,20,28,128);" );      // some widgets didnt inherit
    // this->setStyleSheet("background: rgb(18,20,28,128);" );                     // ditto
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveItemLookupG2::resetScreenOpacity()
{
    QGraphicsOpacityEffect* opacityEffect = new QGraphicsOpacityEffect( this );
    opacityEffect->setOpacity(1.0);
    ui->screenContainer->setGraphicsEffect( opacityEffect );

    // ui->screenContainer->setStyleSheet("background: rgb(255,255,255,255);" );       // some widgets didnt inherit
    // this->setStyleSheet("background: rgb(18,20,28,128);" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveItemLookupG2::resetScreen()
{
    // unselect all buttons
    currSearch_ = 0;
    searchBestSellers_->setStyleSheet(QLatin1String("background-color: rgb(244,247,250);\n" "border-width: 0px;\n" "margin: 0 0 0 0;\n" "border-style: none;\n" "border-radius: 20px;\n" "border-width: 2px;") );
    searchFruits_->setStyleSheet(     QLatin1String("background-color: rgb(244,247,250);\n" "border-width: 0px;\n" "margin: 0 0 0 0;\n" "border-style: none;\n" "border-radius: 20px;\n" "border-width: 2px;") );
    searchVeggies_->setStyleSheet(    QLatin1String("background-color: rgb(244,247,250);\n" "border-width: 0px;\n" "margin: 0 0 0 0;\n" "border-style: none;\n" "border-radius: 20px;\n" "border-width: 2px;") );
    searchOrganic_->setStyleSheet(    QLatin1String("background-color: rgb(244,247,250);\n" "border-width: 0px;\n" "margin: 0 0 0 0;\n" "border-style: none;\n" "border-radius: 20px;\n" "border-width: 2px;") );
    searchOther_->setStyleSheet(      QLatin1String("background-color: rgb(244,247,250);\n" "border-width: 0px;\n" "margin: 0 0 0 0;\n" "border-style: none;\n" "border-radius: 20px;\n" "border-width: 2px;") );


    // update auto lookup parameters
    setAutoLookupProduct( ("1" == hook_->fieldValue( sessionHooks::autoLookupProduct )) );
    setAutoLookupNumOfDigits( hook_->fieldValue( sessionHooks::autoLookupNumOfDigits ).toInt() );
    setAutoLookupOverrideDigit( hook_->fieldValue( sessionHooks::autoLookupOverrideDigit ).toInt() );
    resetEntry();

    // reset and select Best Sellers
    searchBestSellers_->setStyleSheet(QLatin1String("background-color: rgb(220,234,254);\n" "color: rgb(0,89,223);\n" "margin: 0 0 0 0;\n" "border-color: rgb(24,113,248);\n" "border-style: solid;\n" "border-radius: 20px;\n" "border-width: 2px;") );

    //scaleConfig_->internalConfig.selfServicePreventClicks = false;

    activeKey_ = NULL;

    QString buttonText( tr( "Language" ) );
    QString lang;

    // figure out what the other/toggled language is
    if ( hook_->currentLanguage() == hook_->fieldValue( sessionHooks::secondaryLanguage ) )
        lang = hook_->fieldValue( sessionHooks::primaryLanguage );
    else
        lang = hook_->fieldValue( sessionHooks::secondaryLanguage );

    // create text with native language
    if ( "ES.MX" == lang )
        buttonText = QString::fromUtf8( "Español" );
    else if ( "FR.CA" == lang )
        buttonText = QString::fromUtf8( "Français" );
    else if ( "EN.US" == lang )
        buttonText = QString::fromUtf8( "English" );

    toggleLanguage_->setText( buttonText );

    // figure out what the other/toggled language is
    translate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveItemLookupG2::refreshWeight()
{
    //QString tmp1 = hook_->fieldValue( sessionHooks::capacityIndicatorString );
    //QString tmp2 = hook_->fieldValue( sessionHooks::dualCapacityIndicatorString );
    //QString tmp3 = hook_->fieldValue( sessionHooks::capacityString );

    // Attempt 1 - dont update when bycount items on selected - didnt work bc they might add more weight to the bycount item and screen doesnt update
    //if( scaleConfig_->currentPLU.productType() == ProductType::PRODUCTTYPE_BYCOUNT )
    //    return;

    // Attempt 2 - had to go early like when the msg is received .  See weightbarwidget onsessionmessage()
    //QString test_wgt = hook_->fieldValue( sessionHooks::netWeight );
    //if( ( "" == test_wgt ) || test_wgt.contains( curSeparator_ ) )
    //    weight_->setText( hook_->fieldValue( sessionHooks::netWeight ) );   // valid weight
    //else
    //    return;     // we must have a bycount in the netweight field which seems evil to me.  EVIL

    weight_->setText( hook_->fieldValue( sessionHooks::netWeight ) );   // valid weight

    bool isMetric( "1" == hook_->fieldValue( sessionHooks::metricScale ) );
    int factor = weigherConfig_->weigherScaleFactor();

    // update all text and weight
    capacityHighRange_->setText( hook_->fieldValue( sessionHooks::capacityIndicatorString ) );
    capacityLowRange_->setText( hook_->fieldValue( sessionHooks::dualCapacityIndicatorString ) );

    // set and highlight the capacity range
    if( "1" == hook_->fieldValue( sessionHooks::capacityIndicator ) )
    {
        capacityHighRange_->setVisible( true );
        capacityHighRange_->setStyleSheet("background: rgb(55,60,75); border-radius: 9px; color: rgb(255,255,255);" );   // show capacity
    }
    else
    {
        //capacityHighRange_->setVisible( false );
        capacityHighRange_->setStyleSheet("background: rgb(18, 20, 29); color: rgb(255,255,255);");                      // hide capacity
    }

    if( "1" == hook_->fieldValue( sessionHooks::dualCapacityIndicator ) )
    {
       capacityLowRange_->setStyleSheet("background: rgb(55,60,75); border-radius: 9px; color: rgb(255,255,255);" );    // show capacity
       capacityLowRange_->setVisible( true );
    }
    else
    {
        if( !isMetric && ( 1 == factor ) )
            capacityLowRange_->setVisible( false );
        else
            capacityLowRange_->setStyleSheet("background: rgb(18, 20, 29); color: rgb(255,255,255); ");                      // hide capacity
    }

    // hide/show zero
    zero_->setVisible( "1" == hook_->fieldValue( sessionHooks::zeroIndicator ) );
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveItemLookupG2::onReloadImages()
{
    QPixmap itemLookup( filesystem::absoluteFilePath( "itemLookup.png", folderType::SelfServeImages ) );

    // refresh ui
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveItemLookupG2::resetBestSellers()
{
    searchBestSellers_->setStyleSheet(QLatin1String("background-color: rgb(244,247,250);\n" "border-width: 0px;\n" "margin: 0 0 0 0;\n" "border-style: none;\n" "border-radius: 20px;\n" "border-width: 2px;") );
    searchFruits_->setStyleSheet(     QLatin1String("background-color: rgb(244,247,250);\n" "border-width: 0px;\n" "margin: 0 0 0 0;\n" "border-style: none;\n" "border-radius: 20px;\n" "border-width: 2px;") );
    searchVeggies_->setStyleSheet(    QLatin1String("background-color: rgb(244,247,250);\n" "border-width: 0px;\n" "margin: 0 0 0 0;\n" "border-style: none;\n" "border-radius: 20px;\n" "border-width: 2px;") );
    searchOrganic_->setStyleSheet(    QLatin1String("background-color: rgb(244,247,250);\n" "border-width: 0px;\n" "margin: 0 0 0 0;\n" "border-style: none;\n" "border-radius: 20px;\n" "border-width: 2px;") );
    searchOther_->setStyleSheet(      QLatin1String("background-color: rgb(244,247,250);\n" "border-width: 0px;\n" "margin: 0 0 0 0;\n" "border-style: none;\n" "border-radius: 20px;\n" "border-width: 2px;") );

    currSearch_ = 0;

    searchBestSellers_->setStyleSheet(QLatin1String("background-color: rgb(220,234,254);\n" "color: rgb(0,89,223);\n" "margin: 0 0 0 0;\n" "border-color: rgb(24,113,248);\n" "border-style: solid;\n" "border-radius: 20px;\n" "border-width: 2px;") );

    ScaleConfiguration *scaleConfig = hook_->session()->scaleConfig;
    scaleConfig->internalConfig.fixedFlashkeyDefaultGroup = scaleConfig->softkeyFeature.defaultGroupNum();

    //int test1 = scaleConfig->internalConfig.fixedFlashkeyDefaultGroup;
    //QString test2 = scaleConfig_->scaleConfig.selfServeFreshServeFlashkeyNum0();

    scaleConfig->toggledRow = 0;
    scaleConfig->toggledCol = 0;
    scaleConfig->toggledState = true;

    // (136,20)
    // SS Send: 136,20,999020,40000,0,0
     hook_->session()->send(
       SoftkeyDev, ReadSoftkey,
       QString::number( hobart::softkeysWidget::FreshServeFixedButtons ) + "," +               // QString::number( hobart::softkeysWidget::FixedRowColumnGrid ) + "," +
       QString::number( hobart::softkeysWidget::FreshServeFixedButtons ) + "," +               // scaleConfig_->scaleConfig.selfServeFreshServeFlashkeyNum0() + "," +
       QString::number( 0 ) + "," +     // from config
       QString::number( 0 ) );          // from config

     // SS Send: 136,34,42,42,999020
     hook_->session()->send(
       SoftkeyDev, ExecuteSoftkey,
       QString::number( hobart::softkeysWidget::Action_LoadMostCommon_First ) + "," +
       QString::number( hobart::softkeysWidget::Action_LoadMostCommon_First ) + "," +
       QString::number( hobart::softkeysWidget::FreshServeFixedButtons ) );

     refreshWeight();

     update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveItemLookupG2::onButtonClicked( QWidget *w )
{
    // any button click or flashkey resets the idle timer
    emit hook_->resetIdleTimer();

    // handle button
    if ( rezero_ == w )
    {
        hook_->emitZeroScale();
    }
    else if ( toggleLanguage_ == w )
    {
        hook_->emitChangeLanguage();
        translate();
    }
    else if ( startOver_ == w )
    {
        // dump current item (should not have one here)
        hook_->emitChangeFieldValue( sessionHooks::itemCode, "0" );
        hook_->setCurrentState( selfServiceHook::attractGen2 );
    }
    else if ( search_ == w )
    {
        hook_->setCurrentState( selfServiceHook::itemEntryImagesGen2 );
    }
    else if ( searchBestSellers_ == w )
    {
        if( 0 == currSearch_ )
            return;

        resetBestSellers();
    }
    else if ( searchFruits_ == w )
    {
        searchBestSellers_->setStyleSheet(QLatin1String("background-color: rgb(244,247,250);\n" "border-width: 0px;\n" "margin: 0 0 0 0;\n" "border-style: none;\n" "border-radius: 20px;\n" "border-width: 2px;") );
        searchFruits_->setStyleSheet(     QLatin1String("background-color: rgb(244,247,250);\n" "border-width: 0px;\n" "margin: 0 0 0 0;\n" "border-style: none;\n" "border-radius: 20px;\n" "border-width: 2px;") );
        searchVeggies_->setStyleSheet(    QLatin1String("background-color: rgb(244,247,250);\n" "border-width: 0px;\n" "margin: 0 0 0 0;\n" "border-style: none;\n" "border-radius: 20px;\n" "border-width: 2px;") );
        searchOrganic_->setStyleSheet(    QLatin1String("background-color: rgb(244,247,250);\n" "border-width: 0px;\n" "margin: 0 0 0 0;\n" "border-style: none;\n" "border-radius: 20px;\n" "border-width: 2px;") );
        searchOther_->setStyleSheet(      QLatin1String("background-color: rgb(244,247,250);\n" "border-width: 0px;\n" "margin: 0 0 0 0;\n" "border-style: none;\n" "border-radius: 20px;\n" "border-width: 2px;") );

        currSearch_ = 1;

        searchFruits_->setStyleSheet(QLatin1String("background-color: rgb(220,234,254);\n" "color: rgb(0,89,223);\n" "margin: 0 0 0 0;\n" "border-color: rgb(24,113,248);\n" "border-style: solid;\n" "border-radius: 20px;\n" "border-width: 2px;") );

        ScaleConfiguration *scaleConfig = hook_->session()->scaleConfig;
        scaleConfig->internalConfig.fixedFlashkeyDefaultGroup = scaleConfig->softkeyFeature.defaultGroupNum();

        //int test1 = scaleConfig->internalConfig.fixedFlashkeyDefaultGroup;
        //QString test2 = scaleConfig_->scaleConfig.selfServeFreshServeFlashkeyNum1();

        scaleConfig->toggledRow = 1;
        scaleConfig->toggledCol = 0;
        scaleConfig->toggledState = true;

        // (136,20)
        // Send: 136,20,999020,40000,1,0
         hook_->session()->send(
                     SoftkeyDev, ReadSoftkey,
                     QString::number( hobart::softkeysWidget::FreshServeFixedButtons ) + "," +              //QString::number( hobart::softkeysWidget::FixedRowColumnGrid ) + "," +
                     QString::number( hobart::softkeysWidget::FreshServeFixedButtons ) + "," +              //QString::number( scaleConfig->internalConfig.fixedFlashkeyDefaultGroup ) + "," +
                     QString::number( 1 ) + "," +
                     QString::number( 0 ) );        // FS 3 - Fruits

         // Send: 136,34,9,41000,999020
         hook_->session()->send(
                     SoftkeyDev, ExecuteSoftkey,
                     QString::number( hobart::softkeysWidget::Action_LoadSoftkeyGroup ) + "," +
                     scaleConfig_->scaleConfig.selfServeFreshServeFlashkeyNum1() + "," +   //"41000" + "," +
                     QString::number( hobart::softkeysWidget::FreshServeFixedButtons ) );
    }
    else if ( searchVeggies_ == w )
    {
        searchBestSellers_->setStyleSheet(QLatin1String("background-color: rgb(244,247,250);\n" "border-width: 0px;\n" "margin: 0 0 0 0;\n" "border-style: none;\n" "border-radius: 20px;\n" "border-width: 2px;") );
        searchFruits_->setStyleSheet(     QLatin1String("background-color: rgb(244,247,250);\n" "border-width: 0px;\n" "margin: 0 0 0 0;\n" "border-style: none;\n" "border-radius: 20px;\n" "border-width: 2px;") );
        searchVeggies_->setStyleSheet(    QLatin1String("background-color: rgb(244,247,250);\n" "border-width: 0px;\n" "margin: 0 0 0 0;\n" "border-style: none;\n" "border-radius: 20px;\n" "border-width: 2px;") );
        searchOrganic_->setStyleSheet(    QLatin1String("background-color: rgb(244,247,250);\n" "border-width: 0px;\n" "margin: 0 0 0 0;\n" "border-style: none;\n" "border-radius: 20px;\n" "border-width: 2px;") );
        searchOther_->setStyleSheet(      QLatin1String("background-color: rgb(244,247,250);\n" "border-width: 0px;\n" "margin: 0 0 0 0;\n" "border-style: none;\n" "border-radius: 20px;\n" "border-width: 2px;") );

        currSearch_ = 2;
        searchVeggies_->setStyleSheet(QLatin1String("background-color: rgb(220,234,254);\n" "color: rgb(0,89,223);\n" "margin: 0 0 0 0;\n" "border-color: rgb(24,113,248);\n" "border-style: solid;\n" "border-radius: 20px;\n" "border-width: 2px;") );

        ScaleConfiguration *scaleConfig = hook_->session()->scaleConfig;
        scaleConfig->internalConfig.fixedFlashkeyDefaultGroup = scaleConfig->softkeyFeature.defaultGroupNum();

        //int test1 = scaleConfig->internalConfig.fixedFlashkeyDefaultGroup;
        //QString test2 = scaleConfig_->scaleConfig.selfServeFreshServeFlashkeyNum2();

        scaleConfig->toggledRow = 2;
        scaleConfig->toggledCol = 0;
        scaleConfig->toggledState = true;

        // (136,20)
        // Send: 136,20,999020,40000,2,0
        
        hook_->session()->send(
           SoftkeyDev, ReadSoftkey,
           QString::number( hobart::softkeysWidget::FreshServeFixedButtons ) + "," + //QString::number( hobart::softkeysWidget::FixedRowColumnGrid ) + "," +
           QString::number( hobart::softkeysWidget::FreshServeFixedButtons ) + "," + // QString::number( scaleConfig->internalConfig.fixedFlashkeyDefaultGroup ) + "," +
           QString::number( 2 ) + "," +     // FS UI TODO from config
           QString::number( 0 ) );          // FS UI TODO from config       // FS 6

        // Send: 136,34,9,42000,999020
        hook_->session()->send(
           SoftkeyDev, ExecuteSoftkey,
           QString::number( hobart::softkeysWidget::Action_LoadSoftkeyGroup ) + "," +
           scaleConfig_->scaleConfig.selfServeFreshServeFlashkeyNum2() + "," +   // "42000" + "," +                 // FS UI TODO from config
           QString::number( hobart::softkeysWidget::FreshServeFixedButtons ) );
    }
    else if ( searchOrganic_ == w )
    {
        searchBestSellers_->setStyleSheet(QLatin1String("background-color: rgb(244,247,250);\n" "border-width: 0px;\n" "margin: 0 0 0 0;\n" "border-style: none;\n" "border-radius: 20px;\n" "border-width: 2px;") );
        searchFruits_->setStyleSheet(     QLatin1String("background-color: rgb(244,247,250);\n" "border-width: 0px;\n" "margin: 0 0 0 0;\n" "border-style: none;\n" "border-radius: 20px;\n" "border-width: 2px;") );
        searchVeggies_->setStyleSheet(    QLatin1String("background-color: rgb(244,247,250);\n" "border-width: 0px;\n" "margin: 0 0 0 0;\n" "border-style: none;\n" "border-radius: 20px;\n" "border-width: 2px;") );
        searchOrganic_->setStyleSheet(    QLatin1String("background-color: rgb(244,247,250);\n" "border-width: 0px;\n" "margin: 0 0 0 0;\n" "border-style: none;\n" "border-radius: 20px;\n" "border-width: 2px;") );
        searchOther_->setStyleSheet(      QLatin1String("background-color: rgb(244,247,250);\n" "border-width: 0px;\n" "margin: 0 0 0 0;\n" "border-style: none;\n" "border-radius: 20px;\n" "border-width: 2px;") );

        currSearch_ = 3;
        searchOrganic_->setStyleSheet(QLatin1String("background-color: rgb(220,234,254);\n" "color: rgb(0,89,223);\n" "margin: 0 0 0 0;\n" "border-color: rgb(24,113,248);\n" "border-style: solid;\n" "border-radius: 20px;\n" "border-width: 2px;") );

        ScaleConfiguration *scaleConfig = hook_->session()->scaleConfig;
        scaleConfig->internalConfig.fixedFlashkeyDefaultGroup = scaleConfig->softkeyFeature.defaultGroupNum();

        //int test1 = scaleConfig->internalConfig.fixedFlashkeyDefaultGroup;
        //QString test2 = scaleConfig_->scaleConfig.selfServeFreshServeFlashkeyNum3();

        scaleConfig->toggledRow = 3;
        scaleConfig->toggledCol = 0;
        scaleConfig->toggledState = true;

        // (136,20)
        // 136,20,999020,40000,3,0
         hook_->session()->send(
           SoftkeyDev, ReadSoftkey,
           QString::number( hobart::softkeysWidget::FreshServeFixedButtons ) + "," +
           QString::number( hobart::softkeysWidget::FreshServeFixedButtons ) + "," +
           QString::number( 3 ) + "," +     // FS UI TODO from config
           QString::number( 0 ) );          // FS UI TODO from config

         // Send: 136,34,9,43000,999020
         hook_->session()->send(
           SoftkeyDev, ExecuteSoftkey,
           QString::number( hobart::softkeysWidget::Action_LoadSoftkeyGroup ) + "," +
           scaleConfig_->scaleConfig.selfServeFreshServeFlashkeyNum3() + "," +   // "43000" + "," +                 // FS UI TODO from config
           QString::number( hobart::softkeysWidget::FreshServeFixedButtons ) );
    }
    else if ( searchOther_ == w )
    {
        searchBestSellers_->setStyleSheet(QLatin1String("background-color: rgb(244,247,250);\n" "border-width: 0px;\n" "margin: 0 0 0 0;\n" "border-style: none;\n" "border-radius: 20px;\n" "border-width: 2px;") );
        searchFruits_->setStyleSheet(     QLatin1String("background-color: rgb(244,247,250);\n" "border-width: 0px;\n" "margin: 0 0 0 0;\n" "border-style: none;\n" "border-radius: 20px;\n" "border-width: 2px;") );
        searchVeggies_->setStyleSheet(    QLatin1String("background-color: rgb(244,247,250);\n" "border-width: 0px;\n" "margin: 0 0 0 0;\n" "border-style: none;\n" "border-radius: 20px;\n" "border-width: 2px;") );
        searchOrganic_->setStyleSheet(    QLatin1String("background-color: rgb(244,247,250);\n" "border-width: 0px;\n" "margin: 0 0 0 0;\n" "border-style: none;\n" "border-radius: 20px;\n" "border-width: 2px;") );
        searchOther_->setStyleSheet(      QLatin1String("background-color: rgb(244,247,250);\n" "border-width: 0px;\n" "margin: 0 0 0 0;\n" "border-style: none;\n" "border-radius: 20px;\n" "border-width: 2px;") );

        currSearch_ = 4;
        searchOther_->setStyleSheet(QLatin1String("background-color: rgb(220,234,254);\n" "color: rgb(0,89,223);\n" "margin: 0 0 0 0;\n" "border-color: rgb(24,113,248);\n" "border-style: solid;\n" "border-radius: 20px;\n" "border-width: 2px;") );

        ScaleConfiguration *scaleConfig = hook_->session()->scaleConfig;
        scaleConfig->internalConfig.fixedFlashkeyDefaultGroup = scaleConfig->softkeyFeature.defaultGroupNum();

        //int test1 = scaleConfig->internalConfig.fixedFlashkeyDefaultGroup;
        //QString test2 = scaleConfig_->scaleConfig.selfServeFreshServeFlashkeyNum4();

        scaleConfig->toggledRow = 4;
        scaleConfig->toggledCol = 0;
        scaleConfig->toggledState = true;

        // (136,20)
        // Send: 136,20,999020,40000,4,0
         hook_->session()->send(
           SoftkeyDev, ReadSoftkey,
           QString::number( hobart::softkeysWidget::FreshServeFixedButtons ) + "," +
           QString::number( hobart::softkeysWidget::FreshServeFixedButtons ) + "," +
           QString::number( 4 ) + "," +     // FS UI TODO from config
           QString::number( 0 ) );          // FS UI TODO from config

         // Send: 136,34,10,70,999020
         hook_->session()->send(
           SoftkeyDev, ExecuteSoftkey,
           QString::number( hobart::softkeysWidget::Action_LoadSoftkeyGroup ) + "," +
           scaleConfig_->scaleConfig.selfServeFreshServeFlashkeyNum4() + "," +   // "70" + "," +                 // FS UI TODO from config
           QString::number( hobart::softkeysWidget::FreshServeFixedButtons ) );
    }
    else if( backspace_ == w )
    {
        // Do I need to do a 50% opacity on this button?  They didn't mention this fact and didn't provide one
        QString value( pluNum_->text() );
        if( value.length() > 1 )
            value.chop(1);
        else if( value.length() == 1 )
            value = "";

        if ( validator_ )
        {
            if ( value.toInt() < validator_->bottom() )
                return;
            else if ( validator_->top() < value.toInt() )
                return;
        }

        pluNum_->setText( value );
    }
    else if ( enter_ == w )
    {
        // Do I need to do a 50% opacity on this button?  They didn't mention this fact and didn't provide one
        pluNum_->repaint();

        Q_EMIT lookup();                // added enter key

        return;
    }
    else
    {
        QPushButton *button( dynamic_cast<QPushButton*>( w ) );

        // append button text to line edit
        if ( button )
        {
            // in case there is a key already pressed and it hasn't timed out yet
            if( activeKey_ )
            {
                activeKey_->setStyleSheet(QLatin1String("background-color: rgb(239,242,246);\n" "color: rgb(25,34,46);\n" "border-style: solid;\n" "border-radius: 15px;\n" "border-width: 0px;") );

                if( cycleNumpadTimer_->isActive() )
                    cycleNumpadTimer_->stop();
            }
            // trigger the pressed button
            button->setStyleSheet(QLatin1String("background-color: rgb(247,248,250);\n" "color: rgb(128,128,128);\n" "border-style: solid;\n" "border-radius: 15px;\n" "border-width: 0px;") );
            activeKey_ = button;

            cycleNumpadTimer_->start(500);
            cycleNumpadTimer_->setSingleShot(true);

            //QString value( pluNum_->text() );
            //value.append( button->text() );

            QString plunum( pluNum_->text() );
            QString value( "" );
            if( ( "0" == plunum ) || ( "" == plunum ) )
            {
                value = button->text();
            }
            else
            {
                value = plunum.append( button->text() );
                if( value.length() > 6 )
                    value.remove(0, 1);
            }

            value.remove( QRegExp("^[0]*") );

            pluNum_->setText( value );

            if ( validator_ )
            {
                if ( value.toInt() < validator_->bottom() )
                    return;
                else if ( validator_->top() < value.toInt() )
                    return;
            }

            pluNum_->setText( value );
            // force repaint!
            pluNum_->repaint();

            // check for auto lookup
            if ( autoLookupProduct_ )
            {
                if (( 0 < autoLookupNumOfDigits_ ) && ( autoLookupNumOfDigits_ <= value.length() ))
                {
                    if (( 0 < autoLookupOverrideDigit_ ) &&
                        ( autoLookupNumOfDigits_ == value.length() ) &&
                        ( QString::number( autoLookupOverrideDigit_ ) == QString( value[0] ) ))
                    {
                        // nothing to do, first digit is override digit
                    }
                    else
                    {
                        // force repaint!
                        pluNum_->repaint();

                        Q_EMIT lookup();
                    }
                }
            }
        }       // if a button
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveItemLookupG2::mousePressEvent( QMouseEvent *e )
{
    LOG_CLASS_INFO( "selfserveItemLookupG2", "mousePressEvent" );
    LOG_DEBUG( "Got a mousePressEvent on selfserveItemLookupG2 Screen", NULL );

    // this is just for the weight field, the capacity string is handled over in the selfserviceweightbar area

    LOG_DEBUG( "Get our click location", NULL );
    int clickX = e->globalX();
    int clickY = e->globalY();
    int screen_h = getScreenHeight();

    // See if the user clicked capacity and allow a leeway
    int wb_w = ui->wgtWeightBox->geometry().width();

    LOG_DEBUG( "Click location: (%d, %d)", clickX, clickY );
    LOG_DEBUG( "Screen height: %d", screen_h);
    LOG_DEBUG( "Weight box width: %d", wb_w );

#ifdef QT_DEBUG
    screen_h = 768;
    LOG_DEBUG( "QT_DEBUG screen_h NOW = 768", NULL );
#endif

    LOG_DEBUG( "Is clickX (%d) <= wb_w (%d)?", clickX, wb_w );
    LOG_DEBUG( "Is clickY (%d) < screen_h (%d)?", clickY, screen_h );
    LOG_DEBUG( "Is clickY (%d) >= screen_h - 40 (%d)?", clickY, screen_h - 40 );

    if( ( clickX <= wb_w ) && ( clickY < screen_h ) && ( clickY >= ( screen_h - 40 ) ) )
    {
        LOG_DEBUG( "Met all the rules - POP Weights and Measures", NULL );
        hook_->setCurrentState( selfServiceHook::weightsMeasuresDialogGen2 );
    }
    else
        LOG_DEBUG( "Mess something up DO NOT POP W&M Screen", NULL );


    LOG_DEBUG( "END mousePressedEvent()", NULL );

    _Mybase::mousePressEvent( e );
}

void selfserveItemLookupG2::onKeypadTimeout()
{
    if( activeKey_ )
    {
        activeKey_->setStyleSheet(QLatin1String("background-color: rgb(239,242,246);\n" "color: rgb(25,34,46);\n" "border-style: solid;\n" "border-radius: 15px;\n" "border-width: 0px;") );
        activeKey_ = NULL;
    }
    else
        search_->setEnabled( true );
}

void selfserveItemLookupG2::onNumberPadTextChanged( const QString& text )
{
    Q_UNUSED( text );
    emit hook_->resetIdleTimer();
}
///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveItemLookupG2::setAutoLookupProduct( bool value )
{
    autoLookupProduct_ = value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveItemLookupG2::setAutoLookupNumOfDigits( int value )
{
    autoLookupNumOfDigits_ = value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveItemLookupG2::setAutoLookupOverrideDigit( int value )
{
    autoLookupOverrideDigit_ = value;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
int selfserveItemLookupG2::getScreenHeight()
{
    LOG_CLASS_INFO( "selfserveItemLookupG2", "getScreenHeight" );

    LOG_DEBUG( "getScreenHeight() START", NULL );

    int screen_height = 768;        // 14 inch since thats what we are starting with

    QDesktopWidget *desktop = QApplication::desktop();

    int screen = desktop->primaryScreen();

    if (( 0 <= screen ) && ( screen < desktop->screenCount() ))
    {
        QRect g( desktop->screenGeometry( screen ) );

        QSize resolution_ = g.size();

        //QMessageBox::information(this, "Resolution", "Screen resolution is\n\nWidth: " + QString::number(resolution_.width()) + "\nHeight: " + QString::number(resolution_.height()), QMessageBox::Ok);

        screen_height = resolution_.height();

        LOG_DEBUG( "getScreenHeight() resolution.height: %d", resolution_.height() );
        LOG_DEBUG( "getScreenHeight() resolution.width: %d", resolution_.width() );
        LOG_DEBUG( "getScreenHeight() screen_width: %d", screen_height );
    }
    else
         LOG_DEBUG( "getScreenHeight() get screen_width FAILED. Return the default: %d", screen_height );

    // No idea why this is not working but it is not and it returns 1366
    // in the the width from the function. Come back to it later but now, this is 768 period

#ifdef QT_DEBUG
    screen_height = 768;
#endif

    LOG_DEBUG( "getScreenHeight() END with returned screen_height: %d", screen_height );

    return screen_height;
}

int selfserveItemLookupG2::makeValidatorTop( int digits )
{
    int top = 0;

    while ( digits-- )
    {
        top *= 10;
        top += 9;
    }
    return top;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveItemLookupG2::initialize()
{
    // init scaleConfig
    Session *s      = hook_->session();
    scaleConfig_    = s->scaleConfig;
    weigherConfig_  = &scaleConfig_->weigherConfigRead;

    MadpCurrencySettings *currencySettings( &scaleConfig_->currencySettings );
    if( currencySettings->unitSeparator().length() > 0 )
        curSeparator_ = currencySettings->unitSeparator();

    // load images
    onReloadImages();

    screen_             = ui->screenContainer;

    weight_             = ui->lblWeight;
    weightTitle_        = ui->lblWeightTitle;
    pluNumTitle_        = ui->lblEnterItem;
    capacityLabel_      = ui->lblCapacity;
    zero_               = ui->lblZero;
    capacityLowRange_   = ui->lblLowRange;
    capacityHighRange_  = ui->lblHighRange;

    rezero_             = ui->butRezero;
    startOver_          = ui->butStartOver;
    toggleLanguage_     = ui->butLanguage;

    search_             = ui->butSearch;
    searchBestSellers_  = ui->butSearchBestSellers;
    searchFruits_       = ui->butSearchFruits;
    searchVeggies_      = ui->butSearchVeggies;
    searchOrganic_      = ui->butSearchOrganic;
    searchOther_        = ui->butSearchOther;

    digitZero_          = ui->butZero;
    digitOne_           = ui->butOne;
    digitTwo_           = ui->butTwo;
    digitThree_         = ui->butThree;
    digitFour_          = ui->butFour;
    digitFive_          = ui->butFive;
    digitSix_           = ui->butSix;
    digitSeven_         = ui->butSeven;
    digitEight_         = ui->butEight;
    digitNine_          = ui->butNine;

    backspace_          = ui->butBackspace;
    enter_              = ui->butEnter;
    pluNum_             = ui->txtPluNum;

    prevSearch_ = 0;
    currSearch_ = 0;

    buttonSignalMapper_ = new QSignalMapper( this );
    connect( buttonSignalMapper_, SIGNAL( mapped(QWidget*) ), SLOT( onButtonClicked(QWidget*) ) );

    connect( rezero_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( rezero_, rezero_ );

    connect( toggleLanguage_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( toggleLanguage_, toggleLanguage_ );

    connect( startOver_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( startOver_, startOver_ );

    connect( search_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( search_, search_ );

    connect( searchBestSellers_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( searchBestSellers_, searchBestSellers_ );

    connect( searchFruits_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( searchFruits_, searchFruits_ );

    connect( searchVeggies_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( searchVeggies_, searchVeggies_ );

    connect( searchOrganic_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( searchOrganic_, searchOrganic_ );

    connect( searchOther_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( searchOther_, searchOther_ );

    connect( enter_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( enter_, enter_ );

    connect( backspace_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( backspace_, backspace_ );

    connect( digitZero_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( digitZero_, digitZero_ );
    connect( digitOne_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( digitOne_, digitOne_ );
    connect( digitTwo_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( digitTwo_, digitTwo_ );
    connect( digitThree_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( digitThree_, digitThree_ );
    connect( digitFour_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( digitFour_, digitFour_ );
    connect( digitFive_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( digitFive_, digitFive_ );
    connect( digitSix_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( digitSix_, digitSix_ );
    connect( digitSeven_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( digitSeven_, digitSeven_ );
    connect( digitEight_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( digitEight_, digitEight_ );
    connect( digitNine_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( digitNine_, digitNine_ );

    pluNum_->setValidator(numValidator_ = new QIntValidator(0, INT_MAX, this));
    pluNum_->setDecimalPlaces(0);
    pluNum_->setInputWrapping(true);
    numValidator_->setTop(makeValidatorTop(6));

    rezero_->setIcon( QIcon( ":/images/screens/rezero-icon.png" ) );
    rezero_->setIconSize(QSize(27,27));

    toggleLanguage_->setIcon( QIcon(":/images/screens/language-Icon.png" ) );
    toggleLanguage_->setIconSize(QSize(27,27));

    startOver_->setIcon( QIcon(":/images/screens/startover-icon.png" ) );
    startOver_->setIconSize(QSize(27,27));

    // best sellers
    bestSellers_ = new hobart::softkeysWidget( hook_->session(), ui->bestSellersPage );
    bestSellers_->setAutoFillBackground( true );

    searchFruits_->setStyleSheet(     QLatin1String("background-color: rgb(244,247,250);\n" "border-width: 0px;\n" "margin: 0 0 0 0;\n" "border-style: none;\n" "border-radius: 20px;\n" "border-width: 2px;") );
    searchVeggies_->setStyleSheet(    QLatin1String("background-color: rgb(244,247,250);\n" "border-width: 0px;\n" "margin: 0 0 0 0;\n" "border-style: none;\n" "border-radius: 20px;\n" "border-width: 2px;") );
    searchOrganic_->setStyleSheet(    QLatin1String("background-color: rgb(244,247,250);\n" "border-width: 0px;\n" "margin: 0 0 0 0;\n" "border-style: none;\n" "border-radius: 20px;\n" "border-width: 2px;") );
    searchOther_->setStyleSheet(      QLatin1String("background-color: rgb(244,247,250);\n" "border-width: 0px;\n" "margin: 0 0 0 0;\n" "border-style: none;\n" "border-radius: 20px;\n" "border-width: 2px;") );

    bestSellers_->setCreateFilter( hobart::softkeysWidget::FreshServeFixedButtons );           // SS Search on Fixed Flashkeys

    bestSellers_->setFactory( new selfServiceSoftkeyButtonFactoryG2() );
    connect( bestSellers_, SIGNAL( clicked(QWidget*,QWidget*,int,QString,int) ), hook_, SLOT( emitSoftkeyClicked(QWidget*,QWidget*,int,QString,int) ) );

    ui->bestSellersPage->layout()->addWidget( bestSellers_ );

    cycleNumpadTimer_ = new QTimer(this);
    cycleNumpadTimer_->setInterval(500);         // half sec timeout

    connect( cycleNumpadTimer_, SIGNAL( timeout() ), this, SLOT( onKeypadTimeout() ) );

    // set up top line buttons button text
    searchBestSellers_->setText( scaleConfig_->scaleConfig.selfServeFreshServeFlashkeyText0() );
    searchFruits_->setText( scaleConfig_->scaleConfig.selfServeFreshServeFlashkeyText1() );
    searchVeggies_->setText( scaleConfig_->scaleConfig.selfServeFreshServeFlashkeyText2() );
    searchOrganic_->setText( scaleConfig_->scaleConfig.selfServeFreshServeFlashkeyText3() );
    searchOther_->setText( scaleConfig_->scaleConfig.selfServeFreshServeFlashkeyText4() );

    screen_->setStyleSheet( QString::fromUtf8("QWidget#screenContainer { background-color: rgb(244,247,250,255); }" ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace
