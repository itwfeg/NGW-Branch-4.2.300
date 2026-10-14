/**
 * @file selfserveImageLookupViewAllG2.cpp
 *
 * @copyright Copyright (c) 2023, ITW Hobart Food Equipment Group, U.S.A.
 * @author Joe E. Crew
 * copiedfrom Doug Antonides
 */

#include "selfserveImageLookupViewAllG2.h"
#include "ui_selfserveImageLookupViewAllG2.h"

#include "selfservicesoftkeyswidgetG2.h"
#include "selfservicesoftkeybuttonG2.h"

#include <filesystem.h>
#include <selfservicehook.h>
#include <Messages.h>
#include <Session.h>
#include <ScaleConfiguration.h>
#include <UiUtilities.h>
#include <ScaleClasses.h>
#include <softkeyswidget.h>
#include <QMouseEvent>
#include <QDesktopWidget>
#include <QGraphicsOpacityEffect>

// Hobart namespace
namespace hobart {

const static int MAX_FLASHKEYS = 14;

///////////////////////////////////////////////////////////////////////////////////////////////////
selfserveImageLookupViewAllG2::selfserveImageLookupViewAllG2( selfServiceHook *h, QWidget *parent ) :
    _Mybase( parent ),
    ui(new Ui::selfserveImageLookupViewAllG2),
    hook_( h ),
    scaleConfig_( NULL ),
    activeKey_( NULL )
{
    ui->setupUi(this);

    initialize();

    connect( hook_, SIGNAL( reloadImages() ), SLOT( onReloadImages() ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
selfserveImageLookupViewAllG2::~selfserveImageLookupViewAllG2()
{   
    if (scaleConfig_ )
        delete scaleConfig_;

    delete ui;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveImageLookupViewAllG2::loadSoftkeyGroup( const DomLoadSoftkeyGroup* m )
{
    results_->create( m );

    // update page counters
    int currentFlaskey = m->currentPageFirstSoftkey();
    int totalFlashkeys =  m->totalNumOfSoftkeys();
    int numOfSoftkeys = m->numOfSoftkeys();

    if( (numOfSoftkeys > 0 ) && ( 0 == currentFlaskey ) )
    {
        // then we are on the very first page
        lowerLimit_->setText( "1" );
    }
    else if( ( numOfSoftkeys > 0 ) && ( currentFlaskey > 0 ) )
    {
        int curpage = currentFlaskey / MAX_FLASHKEYS;
        lowerLimit_->setText( QString::number( curpage + 1 ) );
    }
    else if( ( 0 == numOfSoftkeys ) && ( 0 == totalFlashkeys ) && ( 0 == currentFlaskey ) )
    {
        lowerLimit_->setText( "1" );
        upperLimit_->setText( "1" );
        return;
    }

    if( totalFlashkeys <= MAX_FLASHKEYS )
    {
        upperLimit_->setText( "1" );
    }
    else if( totalFlashkeys >= MAX_FLASHKEYS )
    {
        int numpages = totalFlashkeys / MAX_FLASHKEYS;
        upperLimit_->setText( QString::number( numpages + 1 ) );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveImageLookupViewAllG2::refresh()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveImageLookupViewAllG2::translate()
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

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveImageLookupViewAllG2::updateNetworkConnectivity(bool online)
{
    if( online )
        screen_->setStyleSheet( QLatin1String("background-color: rgb(244,247,250);" ) );
    else
        screen_->setStyleSheet( QLatin1String("background-color: rgb(255,0,0);" ) );

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveImageLookupViewAllG2::hideEvent( QHideEvent *e )
{
    _Mybase::hideEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveImageLookupViewAllG2::refreshWeight()
{
    // update all text and weight
    weight_->setText( hook_->fieldValue( sessionHooks::netWeight ) );
    //capacityLabel_->setText( hook_->fieldValue( sessionHooks::capacityString ) );
    capacityHighRange_->setText( hook_->fieldValue( sessionHooks::capacityIndicatorString ) );
    capacityLowRange_->setText( hook_->fieldValue( sessionHooks::dualCapacityIndicatorString ) );
    //zero_->setText( hook_->fieldValue( sessionHooks::zeroIndicatorString ) );

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
        if( hook_->session()->uiUtilities->isDualRangeCapable() )
            capacityLowRange_->setStyleSheet("background: rgb(18, 20, 29); color: rgb(255,255,255); ");                      // hide capacity
        else
            capacityLowRange_->setVisible( false );
    }

    // hide/show zero
    zero_->setVisible( "1" == hook_->fieldValue( sessionHooks::zeroIndicator ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveImageLookupViewAllG2::dimScreenOpacity()
{
    QGraphicsOpacityEffect* opacityEffect = new QGraphicsOpacityEffect(this);
    opacityEffect->setOpacity(.30);
    ui->screenContainer->setGraphicsEffect( opacityEffect );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveImageLookupViewAllG2::resetScreenOpacity()
{
    QGraphicsOpacityEffect* opacityEffect = new QGraphicsOpacityEffect( this );
    opacityEffect->setOpacity(1.0);
    ui->screenContainer->setGraphicsEffect( opacityEffect );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveImageLookupViewAllG2::resetScreen()
{
    // Send: 136,40,999017
    hook_->session()->send( SoftkeyDev, ReadSoftkeyFeature, QString::number( hobart::softkeysWidget::SelfservCategoryResultsButtons ) );
    // Send: 136,80,999017
    hook_->session()->send( SoftkeyDev, ReadSoftkeyGroup, QString::number( hobart::softkeysWidget::SelfservCategoryResultsButtons ) );
    // Send: 136,86,999017,7,2,10,10
    hook_->session()->send( SoftkeyDev, SaveSoftkeyGroup, QString::number( hobart::softkeysWidget::SelfservCategoryResultsButtons ) + ",7,2,10,10" );

    if( bestsellers_ )
    {
        activeKey_ = bestsellers_;
        bestsellers_->setStyleSheet(QLatin1String("background-color: rgb(24,113,248);\n" "color: rgb(255,255,255);\n" "border-style: solid;\n" "border-radius: 18px;\n" "border-width: 0px;") );
    }

    // reset all keys
    keyA_->setStyleSheet(QLatin1String("background-color: rgb(255,255,255);\n" "color: rgb(25,34,46);\n" "border-style: solid;\n" "border-radius: 18px;\n" "border-width: 0px;") );
    keyBC_->setStyleSheet(QLatin1String("background-color: rgb(255,255,255);\n" "color: rgb(25,34,46);\n" "border-style: solid;\n" "border-radius: 18px;\n" "border-width: 0px;") );
    keyDJ_->setStyleSheet(QLatin1String("background-color: rgb(255,255,255);\n" "color: rgb(25,34,46);\n" "border-style: solid;\n" "border-radius: 18px;\n" "border-width: 0px;") );
    keyKM_->setStyleSheet(QLatin1String("background-color: rgb(255,255,255);\n" "color: rgb(25,34,46);\n" "border-style: solid;\n" "border-radius: 18px;\n" "border-width: 0px;") );
    keyNO_->setStyleSheet(QLatin1String("background-color: rgb(255,255,255);\n" "color: rgb(25,34,46);\n" "border-style: solid;\n" "border-radius: 18px;\n" "border-width: 0px;") );
    keyP_->setStyleSheet(QLatin1String("background-color: rgb(255,255,255);\n" "color: rgb(25,34,46);\n" "border-style: solid;\n" "border-radius: 18px;\n" "border-width: 0px;") );
    keyQS_->setStyleSheet(QLatin1String("background-color: rgb(255,255,255);\n" "color: rgb(25,34,46);\n" "border-style: solid;\n" "border-radius: 18px;\n" "border-width: 0px;") );
    keyTZ_->setStyleSheet(QLatin1String("background-color: rgb(255,255,255);\n" "color: rgb(25,34,46);\n" "border-style: solid;\n" "border-radius: 18px;\n" "border-width: 0px;") );

    /////////////////////////////////////////////////////
    lowerLimit_->setText( "1" );
    upperLimit_->setText( "1" );

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

    // Send: 136,34,42,42,999020
    resetBestSellers();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveImageLookupViewAllG2::showEvent( QShowEvent *e )
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

    _Mybase::showEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveImageLookupViewAllG2::onKeypadTimeout()
{
    if( activeKey_ )
    {
        activeKey_->setStyleSheet(QLatin1String("background-color: rgb(255,255,255);\n" "color: rgb(25,34,46);\n" "border-style: solid;\n" "border-radius: 18px;\n" "border-width: 0px;") );
        activeKey_ = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveImageLookupViewAllG2::onButtonClicked( QWidget *w )
{
    if ( back_ == w )
    {
        hook_->setCurrentState( selfServiceHook::itemEntryImagesGen2 );

        if( activeKey_ )
        {
            activeKey_->setStyleSheet(QLatin1String("background-color: rgb(255,255,255);\n" "color: rgb(25,34,46);\n" "border-style: solid;\n" "border-radius: 18px;\n" "border-width: 0px;") );
            activeKey_ = NULL;
        }
    }
    else if ( rezero_ == w )
    {
        hook_->emitZeroScale();
    }
    else if ( toggleLanguage_ == w )
    {
        hook_->emitChangeLanguage();
        translate();

        // reset page counters selecting best sellers
        lowerLimit_->setText( "1" );
        upperLimit_->setText( "1" );

        resetBestSellers();     // reset to best sellers on change
    }
    else if ( startOver_ == w )
    {
        hook_->emitChangeFieldValue( sessionHooks::itemCode, "0" );
        hook_->setCurrentState( selfServiceHook::itemEntryKeyedGen2 );

        if( activeKey_ )
        {
            activeKey_->setStyleSheet(QLatin1String("background-color: rgb(255,255,255);\n" "color: rgb(25,34,46);\n" "border-style: solid;\n" "border-radius: 18px;\n" "border-width: 0px;") );
            activeKey_ = NULL;
        }
    }
    else if ( bestsellers_ == w )
    {
        // reset page counters selecting best sellers
        //lowerLimit_->setText( "1" );
        //upperLimit_->setText( "1" );

        resetBestSellers();
    }
    else if ( search_ == w )
    {
        hook_->setCurrentState( selfServiceHook::itemEntryImagesGen2 );
    }
    else if( prevPage_ == w )
    {
        if( activeKey_ == bestsellers_ )
        {
            // Send: 136,34,41
            hook_->session()->send( SoftkeyDev, ExecuteSoftkey, QString::number( hobart::softkeysWidget::Action_LoadMostCommon_Previous ) );
        }
        else
        {
            // Send: 136,34,41
            hook_->session()->send( SoftkeyDev, ExecuteSoftkey, QString::number( hobart::softkeysWidget::Action_LoadAlphabeticSearch_Previous ) );
        }
    }
    else if( nextPage_ == w )
    {
        if( activeKey_ == bestsellers_ )
        {
            // Send: 136,34,40
            hook_->session()->send( SoftkeyDev, ExecuteSoftkey, QString::number( hobart::softkeysWidget::Action_LoadMostCommon_Next ) );
        }
        else
        {
            // Send: 136,34,40
            hook_->session()->send( SoftkeyDev, ExecuteSoftkey, QString::number( hobart::softkeysWidget::Action_LoadAlphabeticSearch_Next ) );
        }
    }
    else
    {
        QPushButton *button( dynamic_cast<QPushButton*>( w ) );

        if ( button )
        {
            if( activeKey_ )
                activeKey_->setStyleSheet(QLatin1String("background-color: rgb(255,255,255);\n" "color: rgb(25,34,46);\n" "border-style: solid;\n" "border-radius: 18px;\n" "border-width: 0px;") );

            // trigger the pressed button
            button->setStyleSheet(QLatin1String("background-color: rgb(24,113,248);\n" "color: rgb(255,255,255);\n" "border-style: solid;\n" "border-radius: 18px;\n" "border-width: 0px;") );
            activeKey_ = button;

            // reset page counters selecting new A-Z Category
            //lowerLimit_->setText( "1" );
            //upperLimit_->setText( "1" );
        }

        // must be A-Z categories
        if( keyA_ == w )
        {
            // Send: 136,34,39,"A",999017
            hook_->session()->send( SoftkeyDev, ExecuteSoftkey,
                            QString::number( hobart::softkeysWidget::Action_LoadAlphabeticSearch_First ) +
                            ",\"" + "A" + "\"," +
                            QString::number( hobart::softkeysWidget::SelfservCategoryResultsButtons ) );

        }
        else if( keyBC_ == w )
        {
            // Send: 136,34,39,"B,C",999017
            hook_->session()->send( SoftkeyDev, ExecuteSoftkey,
                            QString::number( hobart::softkeysWidget::Action_LoadAlphabeticSearch_First ) +
                            ",\"" + "B,C" + "\"," +
                            QString::number( hobart::softkeysWidget::SelfservCategoryResultsButtons ) );

        }
        else if( keyDJ_ == w )
        {
            // Send: 136,34,39,"D,E,F,G,H,I,J",999017
            hook_->session()->send( SoftkeyDev, ExecuteSoftkey,
                            QString::number( hobart::softkeysWidget::Action_LoadAlphabeticSearch_First ) +
                            ",\"" + "D,E,F,G,H,I,J" + "\"," +
                            QString::number( hobart::softkeysWidget::SelfservCategoryResultsButtons ) );

        }
        else if( keyKM_ == w )
        {
            // Send: 136,34,39,"K,L,M",999017
            hook_->session()->send( SoftkeyDev, ExecuteSoftkey,
                            QString::number( hobart::softkeysWidget::Action_LoadAlphabeticSearch_First ) +
                            ",\"" + "K,L,M" + "\"," +
                            QString::number( hobart::softkeysWidget::SelfservCategoryResultsButtons ) );

        }
        else if( keyNO_ == w )
        {
            // Send: 136,34,39,"N,O",999017
            hook_->session()->send( SoftkeyDev, ExecuteSoftkey,
                            QString::number( hobart::softkeysWidget::Action_LoadAlphabeticSearch_First ) +
                            ",\"" + "N,O" + "\"," +
                            QString::number( hobart::softkeysWidget::SelfservCategoryResultsButtons ) );

        }
        else if( keyP_ == w )
        {
            // Send: 136,34,39,"P",999017
            hook_->session()->send( SoftkeyDev, ExecuteSoftkey,
                            QString::number( hobart::softkeysWidget::Action_LoadAlphabeticSearch_First ) +
                            ",\"" + "P" + "\"," +
                            QString::number( hobart::softkeysWidget::SelfservCategoryResultsButtons ) );

        }
        else if( keyQS_ == w )
        {
            // Send: 136,34,39,"Q,R,S",999017
            hook_->session()->send( SoftkeyDev, ExecuteSoftkey,
                            QString::number( hobart::softkeysWidget::Action_LoadAlphabeticSearch_First ) +
                            ",\"" + "Q,R,S" + "\"," +
                            QString::number( hobart::softkeysWidget::SelfservCategoryResultsButtons ) );

        }
        else if( keyTZ_ == w )
        {
            // Send: 136,34,39,"T,U,V,W,X,Y,Z",999017
            hook_->session()->send( SoftkeyDev, ExecuteSoftkey,
                            QString::number( hobart::softkeysWidget::Action_LoadAlphabeticSearch_First ) +
                            ",\"" + "T,U,V,W,X,Y,Z" + "\"," +
                            QString::number( hobart::softkeysWidget::SelfservCategoryResultsButtons ) );

        }
    }

    emit hook_->resetIdleTimer();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveImageLookupViewAllG2::onReloadImages()
{
    results_->repaint();

    // refresh ui
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveImageLookupViewAllG2::resetBestSellers()
{
    if( activeKey_ )
        activeKey_->setStyleSheet(QLatin1String("background-color: rgb(255,255,255);\n" "color: rgb(25,34,46);\n" "border-style: solid;\n" "border-radius: 18px;\n" "border-width: 0px;") );

    // trigger the pressed button
    bestsellers_->setStyleSheet(QLatin1String("background-color: rgb(24,113,248);\n" "color: rgb(255,255,255);\n" "border-style: solid;\n" "border-radius: 18px;\n" "border-width: 0px;") );
    activeKey_ = bestsellers_;

     // Send: 136,34,42,42,999017
     hook_->session()->send(
                 SoftkeyDev, ExecuteSoftkey,
                 QString::number( hobart::softkeysWidget::Action_LoadMostCommon_First ) + "," +
                 + "," +
                 QString::number( hobart::softkeysWidget::SelfservCategoryResultsButtons ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveImageLookupViewAllG2::mousePressEvent( QMouseEvent *e )
{
    // this is just for the weight field, the capacity string is handled over in the selfserviceweightbar area

    int clickX = e->globalX();
    int clickY = e->globalY();
    int screen_h = getScreenHeight();

    // See if the user clicked capacity and allow a leeway
    int wb_w = ui->wgtWeightBox->geometry().width();

#ifdef QT_DEBUG
    screen_h = 768;
#endif

    if( ( clickX <= wb_w ) && ( clickY < screen_h ) && ( clickY >= ( screen_h - 40 ) ) )
    {
        hook_->setCurrentState( selfServiceHook::weightsMeasuresDialogGen2 );
    }

    _Mybase::mousePressEvent( e );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
int selfserveImageLookupViewAllG2::getScreenHeight()
{
    int screen_height = 768;        // 14 inch since thats what we are starting with

    QDesktopWidget *desktop = QApplication::desktop();

    int screen = desktop->primaryScreen();

    if (( 0 <= screen ) && ( screen < desktop->screenCount() ))
    {
        QRect g( desktop->screenGeometry( screen ) );

        QSize resolution_ = g.size();

        //QMessageBox::information(this, "Resolution", "Screen resolution is\n\nWidth: " + QString::number(resolution_.width()) + "\nHeight: " + QString::number(resolution_.height()), QMessageBox::Ok);

        screen_height = resolution_.height();
    }


    // No idea why this is not working but it is not and it returns 1366
    // in the the width from the function. Come back to it later but now, this is 768 period

#ifdef QT_DEBUG
    screen_height = 768;
#endif

    return screen_height;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveImageLookupViewAllG2::initialize()
{
    // LookupItem bestsellers method
    results_ = new hobart::softkeysWidget( hook_->session(), ui->azPage );
    results_->setAutoFillBackground( true );
    results_->setCreateFilter( hobart::softkeysWidget::SelfservCategoryResultsButtons );

    results_->setFactory( new selfServiceSoftkeyButtonFactoryG2() );
    connect( results_, SIGNAL( clicked(QWidget*,QWidget*,int,QString,int) ), hook_, SLOT( emitSoftkeyClicked(QWidget*,QWidget*,int,QString,int) ) );

    ui->azPage->layout()->addWidget( results_ );

    // init scaleConfig
    Session *s = hook_->session();
    scaleConfig_ = s->scaleConfig;

    // load images
    onReloadImages();

    weight_             = ui->lblWeight;
    capacityLabel_      = ui->lblCapacity;
    zero_               = ui->lblZero;
    capacityLowRange_   = ui->lblLowRange;
    capacityHighRange_  = ui->lblHighRange;

    screen_             = ui->screenContainer;

    rezero_             = ui->butRezero;
    startOver_          = ui->butStartOver;
    toggleLanguage_     = ui->butLanguage;

    back_             = ui->butBack;
    prevPage_         = ui->butPrev;
    nextPage_         = ui->butNext;
    search_           = ui->butSearch;
    bestsellers_      = ui->butBestSellers;
    lowerLimit_       = ui->lblCurPg;
    upperLimit_       = ui->lblTotPg;

    keyA_  = ui->butA;
    keyBC_ = ui->butBC;
    keyDJ_ = ui->butDJ;
    keyKM_ = ui->butKM;
    keyNO_ = ui->butNO;
    keyP_  = ui->butP;
    keyQS_ = ui->butQS;
    keyTZ_ = ui->butTZ;

    buttonSignalMapper_ = new QSignalMapper( this );
    connect( buttonSignalMapper_, SIGNAL( mapped(QWidget*) ), SLOT( onButtonClicked(QWidget*) ) );

    connect( back_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( back_, back_ );

    connect( nextPage_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( nextPage_, nextPage_ );

    connect( prevPage_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( prevPage_, prevPage_ );

    connect( rezero_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( rezero_, rezero_ );

    connect( toggleLanguage_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( toggleLanguage_, toggleLanguage_ );

    connect( startOver_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( startOver_, startOver_ );

    connect( search_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( search_, search_ );

    connect( bestsellers_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( bestsellers_, bestsellers_ );

    connect( keyA_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( keyA_, keyA_ );
    connect( keyBC_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( keyBC_, keyBC_ );
    connect( keyDJ_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( keyDJ_, keyDJ_ );
    connect( keyKM_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( keyKM_, keyKM_ );
    connect( keyNO_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( keyNO_, keyNO_ );
    connect( keyP_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( keyP_, keyP_ );
    connect( keyQS_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( keyQS_, keyQS_ );
    connect( keyTZ_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( keyTZ_, keyTZ_ );

    rezero_->setIcon( QIcon( ":/images/screens/rezero-icon.png" ) );
    rezero_->setIconSize(QSize(27,27));

    toggleLanguage_->setIcon( QIcon(":/images/screens/language-Icon.png" ) );
    toggleLanguage_->setIconSize(QSize(27,27));

    startOver_->setIcon( QIcon(":/images/screens/startover-icon.png" ) );
    startOver_->setIconSize(QSize(27,27));

    lowerLimit_->setText( "1" );
    upperLimit_->setText( "1" );

    screen_->setStyleSheet( QString::fromUtf8("QWidget#screenContainer { background-color: rgb(244,247,250,255); }" ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace
