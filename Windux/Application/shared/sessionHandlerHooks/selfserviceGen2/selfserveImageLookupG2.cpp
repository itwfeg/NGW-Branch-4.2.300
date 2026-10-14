/**
 * @file selfserveImageLookupG2.cpp
 *
 * @copyright Copyright (c) 2023, ITW Hobart Food Equipment Group, U.S.A.
 * @author Joe E. Crew
 * copiedfrom Doug Antonides
 */

#include "selfserveImageLookupG2.h"
#include "ui_selfserveImageLookupG2.h"

#include "selfservicesoftkeyswidgetG2.h"
#include "selfservicesoftkeybuttonG2.h"
#include "selfserveImageLookupViewAllG2.h"

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

const static int MAX_FLASHKEYS = 7;

///////////////////////////////////////////////////////////////////////////////////////////////////
selfserveImageLookupG2::selfserveImageLookupG2( selfServiceHook *h, QWidget *parent ) :
    _Mybase( parent ),
    ui(new Ui::selfserveImageLookupG2),
    hook_( h ),
    scaleConfig_( NULL ),
    activeKey_( NULL )
{
    ui->setupUi(this);

    initialize();

    connect( hook_, SIGNAL( reloadImages() ), SLOT( onReloadImages() ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
selfserveImageLookupG2::~selfserveImageLookupG2()
{
    delete ui;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveImageLookupG2::loadSoftkeyGroup( const DomLoadSoftkeyGroup* m )
{
    results_->create( m );


    if( m->featureNum() != hobart::softkeysWidget::SelfservCategoryResultsButtons )
    {
        // we only update this screen with search results - not fixed flashkey results
        return;
    }

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
void selfserveImageLookupG2::refresh()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveImageLookupG2::translate()
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
void selfserveImageLookupG2::updateNetworkConnectivity(bool online)
{
    if( online )
        screen_->setStyleSheet( QLatin1String("background-color: rgb(244,247,250);" ) );
    else
        screen_->setStyleSheet( QLatin1String("background-color: rgb(255,0,0);" ) );

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveImageLookupG2::hideEvent( QHideEvent *e )
{
    // reset softkeys state

    _Mybase::hideEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveImageLookupG2::refreshWeight()
{
    // update all text and weight
    weight_->setText( hook_->fieldValue( sessionHooks::netWeight ) );
    //capacityLabel_->setText( hook_->fieldValue( sessionHooks::capacityString ) );
    capacityHighRange_->setText( hook_->fieldValue( sessionHooks::capacityIndicatorString ) );
    capacityLowRange_->setText( hook_->fieldValue( sessionHooks::dualCapacityIndicatorString ) );
    //zero_->setText( hook_->fieldValue( sessionHooks::zeroIndicatorString ) );

    // set and highlight the capacity range
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
void selfserveImageLookupG2::dimScreenOpacity()
{
    QGraphicsOpacityEffect* opacityEffect = new QGraphicsOpacityEffect(this);
    opacityEffect->setOpacity(.30);
    ui->screenContainer->setGraphicsEffect( opacityEffect );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveImageLookupG2::resetScreenOpacity()
{
    QGraphicsOpacityEffect* opacityEffect = new QGraphicsOpacityEffect( this );
    opacityEffect->setOpacity(1.0);
    ui->screenContainer->setGraphicsEffect( opacityEffect );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveImageLookupG2::resetScreen()
{
    // Send: 136,40,999017
    hook_->session()->send( SoftkeyDev, ReadSoftkeyFeature, QString::number( hobart::softkeysWidget::SelfservCategoryResultsButtons ) );
    // Send: 136,80,999017
    hook_->session()->send( SoftkeyDev, ReadSoftkeyGroup, QString::number( hobart::softkeysWidget::SelfservCategoryResultsButtons ) );
    // Send: 136,86,999017,7,1,10,10
    hook_->session()->send( SoftkeyDev, SaveSoftkeyGroup, QString::number( hobart::softkeysWidget::SelfservCategoryResultsButtons ) + ",7,1,10,10" );

    characterSearch_->clear();
    ui->stackKeyboards->setCurrentIndex( 0 );
    butSearchView123_->setVisible( false );
    butSearchViewABC_->setVisible( false );
    butSearchView123_->setEnabled( false );
    butSearchViewABC_->setEnabled( false );

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

    // reset the screen by requesting a blank search
    QString text( characterSearch_->text() );
    hook_->session()->send( SoftkeyDev, ExecuteSoftkey, "39," + text + "," + QString::number( hobart::softkeysWidget::SelfservCategoryResultsButtons ) );

    ui->charByCharPage->show();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveImageLookupG2::showEvent( QShowEvent *e )
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
void selfserveImageLookupG2::onKeypadTimeout()
{
    if( activeKey_ )
    {
        activeKey_->setStyleSheet(QLatin1String("background-color: rgb(255,255,255);\n" "color: rgb(25,34,46);\n" "border-style: solid;\n" "border-radius: 18px;\n" "border-width: 0px;") );
        activeKey_ = NULL;
    }
    else
        back_->setEnabled( true );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveImageLookupG2::onCharacterSearchReturnPressed()
{
    if( characterSearch_ == NULL )
        characterSearch_ = ui->txtSearchChars;

    QString text( characterSearch_->text() );

    if( text.contains("\'") )
        text.replace("\'","\'\'");

    // request items
    hook_->session()->send( SoftkeyDev, ExecuteSoftkey, "39," + text.toUpper() + "," + QString::number( hobart::softkeysWidget::SelfservCategoryResultsButtons ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveImageLookupG2::onCharacterSearchChanged( const QString& text )
{
    // when the customer hits enter
    Q_UNUSED( text );

    onCharacterSearchReturnPressed();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveImageLookupG2::onButtonClicked( QWidget *w )
{
    if ( startOver_ == w )
    {
        // dump current item (should not have one here)
        hook_->emitChangeFieldValue( sessionHooks::itemCode, "0" );
        hook_->setCurrentState( selfServiceHook::itemEntryKeyedGen2 );
    }
    else if ( rezero_ == w )
    {
        hook_->emitZeroScale();
    }
    else if ( toggleLanguage_ == w )
    {
        hook_->emitChangeLanguage();
        characterSearch_->clear();
        translate();
    }
    else if ( back_ == w )
    {
        // dump current item (should not have one here)
        //hook_->emitChangeFieldValue( sessionHooks::itemCode, "0" );       // we dont need to do this
        back_->setEnabled( false );
        hook_->setCurrentState( selfServiceHook::itemEntryKeyedGen2 );
        cycleNumpadTimer_->start(2000);
        cycleNumpadTimer_->setSingleShot(true);
    }
    else if ( clear_ == w )
    {
        characterSearch_->clear();
    }
    else if( ( enter_ == w ) || ( enterNums_ == w ) )
    {
        onCharacterSearchReturnPressed();
    }
    else if ( ( backspace_ == w ) || ( backspaceNums_ == w ) )
    {
        QString value( characterSearch_->text() );
        if( value.length() > 1 )
        {
            value.chop(1);
        }
        else if( value.length() == 1 )
        {
            value = "";
            lowerLimit_->setText( "1" );
            upperLimit_->setText( "1" );
        }

        characterSearch_->setText( value );
    }
    else if ( butSearchView123_ == w )
    {
        //ui->stackKeyboards->setCurrentIndex( 1 );
        //butSearchView123_->setVisible( false );
        //butSearchViewABC_->setVisible( true );
        return;
    }
    else if( butSearchViewABC_ == w )
    {
        //ui->stackKeyboards->setCurrentIndex( 0 );
        //butSearchViewABC_->setVisible( false );
        //butSearchView123_->setVisible( true );
        return;
    }
    else if ( butSearchViewAll_ == w )
    {
        hook_->setCurrentState( selfServiceHook::itemEntryImagesViewAllGen2 );
        ui->charByCharPage->hide();
    }
    else if( digitSPACE_ == w )
    {
        QString value( characterSearch_->text() );
        value.append( " " );

        characterSearch_->setText( value );
        characterSearch_->repaint();
    }
    else if( prevPage_ == w )
    {
        // Send: 136,34,41
        hook_->session()->send( SoftkeyDev, ExecuteSoftkey, QString::number( hobart::softkeysWidget::Action_LoadAlphabeticSearch_Previous ) );
    }
    else if( nextPage_ == w )
    {
        // Send: 136,34,40
        hook_->session()->send( SoftkeyDev, ExecuteSoftkey, QString::number( hobart::softkeysWidget::Action_LoadAlphabeticSearch_Next ) );
    }
    else
    {
        // must be the keyboard

        QPushButton *button( dynamic_cast<QPushButton*>( w ) );

        if ( button )
        {
            // RESET: in case there is a key already pressed and it hasn't timed out yet
            if( activeKey_ )
            {
                activeKey_->setStyleSheet(QLatin1String("background-color: rgb(255,255,255);\n" "color: rgb(25,34,46);\n" "border-style: solid;\n" "border-radius: 18px;\n" "border-width: 0px;") );

                if( cycleNumpadTimer_->isActive() )
                    cycleNumpadTimer_->stop();
            }

            // trigger the pressed button
            button->setStyleSheet(QLatin1String("background-color: rgb(246,248,250);\n" "color: rgb(133,138,145);\n" "border-style: solid;\n" "border-radius: 15px;\n" "border-width: 0px;") );
            activeKey_ = button;

            cycleNumpadTimer_->start(500);
            cycleNumpadTimer_->setSingleShot(true);

            QString value( characterSearch_->text() );
            value.append( button->text() );

            characterSearch_->setText( value );
            characterSearch_->repaint();
        }
    }

    emit hook_->resetIdleTimer();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveImageLookupG2::onReloadImages()
{
    // refresh ui
    results_->repaint();

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfserveImageLookupG2::mousePressEvent( QMouseEvent *e )
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
int selfserveImageLookupG2::getScreenHeight()
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
void selfserveImageLookupG2::initialize()
{
    // LookupItem bestsellers method
    results_ = new hobart::softkeysWidget( hook_->session(), ui->charByCharPage );
    results_->setAutoFillBackground( true );
    results_->setCreateFilter( hobart::softkeysWidget::SelfservCategoryResultsButtons );

    results_->setFactory( new selfServiceSoftkeyButtonFactoryG2() );
    connect( results_, SIGNAL( clicked(QWidget*,QWidget*,int,QString,int) ), hook_, SLOT( emitSoftkeyClicked(QWidget*,QWidget*,int,QString,int) ) );

    ui->charByCharPage->layout()->addWidget( results_ );

    screen_             = ui->screenContainer;
    weight_             = ui->lblWeight;
    capacityLabel_      = ui->lblCapacity;
    zero_               = ui->lblZero;
    lowerLimit_         = ui->lblCurPg;
    upperLimit_         = ui->lblTotPg;
    capacityLowRange_   = ui->lblLowRange;
    capacityHighRange_  = ui->lblHighRange;

    characterSearch_    = ui->txtSearchChars;
    back_               = ui->butBack;
    clear_              = ui->butClear;
    enter_              = ui->butEnter;
    prevPage_           = ui->butPrev;
    nextPage_           = ui->butNext;
    enterNums_          = ui->butkbEnterNums;
    rezero_             = ui->butRezero;
    startOver_          = ui->butStartOver;
    toggleLanguage_     = ui->butLanguage;
    backspace_          = ui->butBackspace;
    backspaceNums_      = ui->butkbBackspaceNums;
    butSearchView123_   = ui->butSearchView123;
    butSearchViewAll_   = ui->butSearchViewAll;
    butSearchViewABC_   = ui->butSearchViewABC;

    digitA_ = ui->butA;
    digitB_ = ui->butB;
    digitC_ = ui->butC;
    digitD_ = ui->butD;
    digitE_ = ui->butE;
    digitF_ = ui->butF;
    digitG_ = ui->butG;
    digitH_ = ui->butH;
    digitI_ = ui->butI;
    digitJ_ = ui->butJ;
    digitK_ = ui->butK;
    digitL_ = ui->butL;
    digitM_ = ui->butM;
    digitN_ = ui->butN;
    digitO_ = ui->butO;
    digitP_ = ui->butP;
    digitQ_ = ui->butQ;
    digitR_ = ui->butR;
    digitS_ = ui->butS;
    digitT_ = ui->butT;
    digitU_ = ui->butU;
    digitV_ = ui->butV;
    digitW_ = ui->butW;
    digitX_ = ui->butX;
    digitY_ = ui->butY;
    digitZ_ = ui->butZ;
    digitAPOS_ = ui->butAPOS;
    digitDASH_ = ui->butDASH;
    digitSPACE_ = ui->butSpace;

    digit0_ = ui->but0;
    digit1_ = ui->but1;
    digit2_ = ui->but2;
    digit3_ = ui->but3;
    digit4_ = ui->but4;
    digit5_ = ui->but5;
    digit6_ = ui->but6;
    digit7_ = ui->but7;
    digit8_ = ui->but8;
    digit9_ = ui->but9;

    cycleNumpadTimer_ = new QTimer(this);
    cycleNumpadTimer_->setInterval(500);         // half sec timeout

    connect( cycleNumpadTimer_, SIGNAL( timeout() ), this, SLOT( onKeypadTimeout() ) );

    buttonSignalMapper_ = new QSignalMapper( this );
    connect( buttonSignalMapper_, SIGNAL( mapped(QWidget*) ), SLOT( onButtonClicked(QWidget*) ) );

    connect( back_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( back_, back_ );

    connect( clear_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( clear_, clear_ );

    connect( enter_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( enter_, enter_ );

    connect( prevPage_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( prevPage_, prevPage_ );

    connect( nextPage_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( nextPage_, nextPage_ );

    connect( enterNums_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( enterNums_, enterNums_ );

    connect( startOver_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( startOver_, startOver_ );

    connect( backspace_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( backspace_, backspace_ );

    connect( backspaceNums_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( backspaceNums_, backspaceNums_ );

    connect( butSearchView123_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( butSearchView123_, butSearchView123_ );

    connect( butSearchViewABC_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( butSearchViewABC_, butSearchViewABC_ );

    connect( butSearchViewAll_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( butSearchViewAll_, butSearchViewAll_ );

    connect( rezero_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( rezero_, rezero_ );

    connect( toggleLanguage_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( toggleLanguage_, toggleLanguage_ );

    connect( digitA_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( digitA_, digitA_ );
    connect( digitB_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( digitB_, digitB_ );
    connect( digitC_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( digitC_, digitC_ );
    connect( digitD_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( digitD_, digitD_ );
    connect( digitE_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( digitE_, digitE_ );
    connect( digitF_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( digitF_, digitF_ );
    connect( digitG_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( digitG_, digitG_ );
    connect( digitH_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( digitH_, digitH_ );
    connect( digitI_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( digitI_, digitI_ );
    connect( digitJ_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( digitJ_, digitJ_ );

    connect( digitK_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( digitK_, digitK_ );
    connect( digitL_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( digitL_, digitL_ );
    connect( digitM_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( digitM_, digitM_ );
    connect( digitN_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( digitN_, digitN_ );
    connect( digitO_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( digitO_, digitO_ );
    connect( digitP_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( digitP_, digitP_ );
    connect( digitQ_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( digitQ_, digitQ_ );
    connect( digitR_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( digitR_, digitR_ );
    connect( digitS_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( digitS_, digitS_ );
    connect( digitT_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( digitT_, digitT_ );

    connect( digitU_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( digitU_, digitU_ );
    connect( digitV_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( digitV_, digitV_ );
    connect( digitW_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( digitW_, digitW_ );
    connect( digitX_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( digitX_, digitX_ );
    connect( digitY_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( digitY_, digitY_ );
    connect( digitZ_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( digitZ_, digitZ_ );
    connect( digitAPOS_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( digitAPOS_, digitAPOS_ );
    connect( digitDASH_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( digitDASH_, digitDASH_ );
    connect( digitSPACE_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( digitSPACE_, digitSPACE_ );

    // 0-9 digits
    connect( digit0_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( digit0_, digit0_ );
    connect( digit1_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( digit1_, digit1_ );
    connect( digit2_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( digit2_, digit2_ );
    connect( digit3_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( digit3_, digit3_ );
    connect( digit4_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( digit4_, digit4_ );
    connect( digit5_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( digit5_, digit5_ );
    connect( digit6_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( digit6_, digit6_ );
    connect( digit7_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( digit7_, digit7_ );
    connect( digit8_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( digit8_, digit8_ );
    connect( digit9_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( digit9_, digit9_ );

    // The Text field characters are entered into
    //connect( characterSearch_, SIGNAL( returnPressed() ),      SLOT( onCharacterSearchEnterPressed() ) );
    connect( characterSearch_, SIGNAL( textChanged(QString) ), SLOT( onCharacterSearchChanged(QString) ) );

    rezero_->setIcon( QIcon( ":/images/screens/rezero-icon.png" ) );
    rezero_->setIconSize(QSize(27,27));

    toggleLanguage_->setIcon( QIcon(":/images/screens/language-Icon.png" ) );
    toggleLanguage_->setIconSize(QSize(27,27));

    startOver_->setIcon( QIcon(":/images/screens/startover-icon.png" ) );
    startOver_->setIconSize(QSize(27,27));

    // load images
    onReloadImages();

    lowerLimit_->setText( "1" );
    upperLimit_->setText( "1 " );

    screen_->setStyleSheet( QString::fromUtf8("QWidget#screenContainer { background-color: rgb(244,247,250,255); }" ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace
