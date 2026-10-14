/**
 * @file selfservicebuttonbar.cpp
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 */

#include "selfservicebuttonbar.h"

#include <backgroundwidgettypes.h>
#include <logmanager.h>
#include <pushbutton.h>
#include <softkeyswidget.h>
#include <selfservicehook.h>

#include <ScaleConfiguration.h>
#include <ScaleClasses.h>
#include <QDesktopWidget>
#include <QHBoxLayout>
#include <QPalette>
#include <QPushButton>
#include <QSignalMapper>
#include <QVBoxLayout>

///////////////////////////////////////////////////////////////////////////////////////////////////
selfServiceButtonBar::selfServiceButtonBar( selfServiceHook *h, QWidget *parent ) :
    _Mybase( parent ),
    hook_( h ),
    scaleConfig_(NULL),
    buttonTypesMask_( MAX_TYPES - 1 )
{
    s = hook_->session();
    scaleConfig_ = s->scaleConfig;

    // initialize
    initialize();
    createLayout();
    translate();

    // initial state is no buttons
    showButtons( none );

    // pjr383 - Need to hide the button bar if we South America / Chile.  But now they are going to have to select the option.
    if (( s ) && ( s->scaleConfig ) && ( s->scaleConfig->scaleConfig.selfServeHideButtonBar() ))
        buttonTypesMask_ = showCancel | showOkay | showPrintLabel;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
selfServiceButtonBar::~selfServiceButtonBar()
{
    if (scaleConfig_ )
        delete scaleConfig_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceButtonBar::refresh()
{
    QString buttonText( tr( "Switch\nLanguage" ) );
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

    // set the text
    toggleLanguage_->setText( buttonText );

    if(  "0" == hook_->fieldValue( sessionHooks::selfServiceKeyEntryStyle ) )
        keypad_->setText( tr( "Keypad" ) );
    else
        keypad_->setText( tr( "Flashkeys") );

}

void selfServiceButtonBar::setBackButtonOn()
{
    back_->setVisible( true );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceButtonBar::showButtons( showButtonTypes value )
{
    // update layout
    if ( buttons_->layout() )
        delete buttons_->layout();

    QHBoxLayout *buttonsLayout( new QHBoxLayout( buttons_ ) );
    buttonsLayout->setContentsMargins( QMargins() );

    buttonsLayout->addStretch();

    // mask off buttons
    value = (showButtonTypes) ( value & buttonTypesMask_ );

    // enumerate types
    for ( int t = 1; t < MAX_TYPES; t <<= 1 )
    {
        QWidget *b( buttonTypes_[ (showButtonTypes) t ] );
        bool visible( value & t );

        if ( b )
        {
            // update visibility
            if( b == back_ )
                b->setVisible( false );
            else
            {
                b->setVisible( visible );

                // this just wasnt working out like I hoped so I just commented it out and may revisit it from time to time
                //if( ( selfServiceHook::processByCountItem == hook_->currentState() ) && ( b == startOver_ ) )
                //{
                //    buttonSpacer_ = new QSpacerItem( 150, 20, QSizePolicy::Minimum, QSizePolicy::Minimum);
                //    buttonsLayout->addItem( buttonSpacer_ );
                //}
            }

            // update layout
            if ( visible )
            {
                buttonsLayout->addWidget( b );
                buttonsLayout->addStretch();
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceButtonBar::translate()
{
    rezero_->setText( tr( "Rezero" ) );
    itemEntryKeyed_->setText( tr( "Start Over" ) ); // HT-494
    itemEntryImages_->setText( tr( "Search" ) );
    startOver_->setText( tr( "Start Over" ) );
    back_->setText( tr( "Back" ) );
    printLabel_->setText( tr( "Print Label" ) );
    cancel_->setText( tr( "Cancel" ) );
    okay_->setText( tr( "Done" ) );
    keypad_->setText( tr("Keypad") );

    refresh();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceButtonBar::onButtonClicked( QWidget *w )
{
    // handle button
    if ( itemEntryKeyed_ == w )
    {
        selfServiceHook::state localstate = hook_->currentState();
        if( selfServiceHook::itemEntryImages == localstate  && ( SelfServicePrintOption::SelfServOption_AtoZ == scaleConfig_->scaleConfig.selfServeSearchScreenState() ) )
        {
            // SOF-4430 Self-Serve: A-Z Categories with Best Sellers does not automatically display Best Sellers, despite showing it selected
            // So when we click Start Over from the Search screen, this will ask for the most common items
            s->send( SoftkeyDev, ExecuteSoftkey, "42,," + QString::number( hobart::softkeysWidget::SelfservCategoryResultsButtons ) );
        }

        hook_->setCurrentState( selfServiceHook::itemEntryKeyed );
    }
    else if ( itemEntryImages_ == w )
        hook_->setCurrentState( selfServiceHook::itemEntryImages );
    else if ( toggleLanguage_ == w )
        hook_->emitChangeLanguage();
    else if ( rezero_ == w )
        hook_->emitZeroScale();
    else if ( startOver_ == w || back_ == w )
    {
        if( hook_->fieldValue( sessionHooks::selfServiceDisplayBarcodeLabel ).toInt() )
        {
            selfServiceHook::state localstate = hook_->currentState();
            if( ( back_ == w) && ( ( selfServiceHook::processByCountItem == localstate ) && ( 1 == hook_->getKeypadStackState() ) ) )
            {
                // just redisplay the keypad instead of the SCAN/PRINT options
                hook_->resetByCountScreen();
                back_->setVisible( false );
            }
            else
            {
                // dump current item
                hook_->emitChangeFieldValue( sessionHooks::itemCode, "0" );
            }
        }
        else
        {
            // dump current item
            hook_->emitChangeFieldValue( sessionHooks::itemCode, "0" );
        }
    }
    else if ( printLabel_ == w )
    {
        // SOF-4679 Pressing Print while in Self Service, and an invalid label assigned to the PLU locks up scale on take a label screen
        if( scaleConfig_->internalConfig.selfservInvalidPlu )
        {
            s->send( Error, ErrProductDevice, QString::number( ProductDevError::ProductErrLabelType ) + ",5" );
            return;
        }

        // have to handle clicks and double clicks even if we are ready or not. But I dont want to handle them again
        // if I havent drawn the print screen in the first place. so only allow again once Ive displayed the print screen

        if( scaleConfig_->scaleConfig.selfServeCombineSearchKeypadFlashkeys() )
        {
            selfServiceHook::state localstate = hook_->currentState();
            if( selfServiceHook::takeLabel == localstate )
                scaleConfig_->internalConfig.clickedPrintLabelFromFinishScreen = true;      // happens only while in Print and Scan Mode

            if ( scaleConfig_->internalConfig.selfServicePreventClicks )
                return;

            scaleConfig_->internalConfig.selfServicePreventClicks = true;
        }

        hook_->emitPrint();
    }
    else if ( keypad_ == w )
        hook_->setCurrentState( selfServiceHook::itemEntryKeyed );
    else if ( cancel_ == w )
    {
        Q_EMIT cancel();
    }
    else if ( okay_ == w )
    {
        if( scaleConfig_->internalConfig.selfServiceSendReimageLabelsMsg )
            scaleConfig_->internalConfig.selfServiceSendReimageLabelsMsg = false;
        Q_EMIT okay();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceButtonBar::initialize()
{
    const hobart::backgroundWidgetRenderer renderDarkButton(
        new hobart::panelButtonBackgroundWidget( 4 ),
        new hobart::darkButtonBackgroundWidgetRendererAppearance() );

    const hobart::backgroundWidgetRenderer renderGreenButton(
        new hobart::panelButtonBackgroundWidget( 4 ),
        new hobart::greenButtonBackgroundWidgetRendererAppearance() );

    const hobart::backgroundWidgetRenderer renderRedButton(
        new hobart::panelButtonBackgroundWidget( 4 ),
        new hobart::redButtonBackgroundWidgetRendererAppearance() );

    int buttonwidth = 150;
    if( 10 == checkScreenSize() && ( SelfServicePrintOption::SelfServOption_LookupCategoriesShowKeypad == scaleConfig_->scaleConfig.selfServeItemEntryScreenState() ) )
        buttonwidth = 115;

    const QPixmap darkButtonPixmap( renderDarkButton.drawControl( QSize( buttonwidth, 64 ), hobart::ReadOnly ) );
    const QPixmap greenButtonPixmap( renderGreenButton.drawControl( QSize( buttonwidth, 64 ), hobart::ReadOnly ) );
    const QPixmap redButtonPixmap( renderRedButton.drawControl( QSize( buttonwidth, 64 ), hobart::ReadOnly ) );

    buttonSignalMapper_ = new QSignalMapper( this );
    connect( buttonSignalMapper_, SIGNAL( mapped(QWidget*) ), SLOT( onButtonClicked(QWidget*) ) );

    buttons_ = new QWidget( this );

    QPalette palette( buttons_->palette() );
    palette.setColor( QPalette::ButtonText, hobart::colorPalette::white );
    buttons_->setPalette( palette );

    QFont font( buttons_->font() );
    font.setPointSize( 16 );
    buttons_->setFont( font );

    itemEntryKeyed_ = new hobart::pushButton( buttons_, darkButtonPixmap );
    connect( itemEntryKeyed_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( itemEntryKeyed_, itemEntryKeyed_ );

    itemEntryImages_ = new hobart::pushButton( buttons_, darkButtonPixmap );
    connect( itemEntryImages_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( itemEntryImages_, itemEntryImages_ );

    toggleLanguage_ = new hobart::pushButton( buttons_, darkButtonPixmap );
    connect( toggleLanguage_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( toggleLanguage_, toggleLanguage_ );

    rezero_ = new hobart::pushButton( buttons_, darkButtonPixmap );
    connect( rezero_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( rezero_, rezero_ );

    startOver_ = new hobart::pushButton( buttons_, darkButtonPixmap );
    connect( startOver_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( startOver_, startOver_ );

    back_ = new hobart::pushButton( buttons_, darkButtonPixmap );
    connect( back_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( back_, back_ );


    printLabel_ = new hobart::pushButton( buttons_, greenButtonPixmap );
    connect( printLabel_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( printLabel_, printLabel_ );

    cancel_ = new hobart::pushButton( buttons_, redButtonPixmap );
    connect( cancel_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( cancel_, cancel_ );

    okay_ = new hobart::pushButton( buttons_, greenButtonPixmap );
    connect( okay_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( okay_, okay_ );

    keypad_ = new hobart::pushButton( buttons_, greenButtonPixmap );
    connect( keypad_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
    buttonSignalMapper_->setMapping( keypad_, keypad_ );

    // create mapping
    buttonTypes_.insert( showItemEntryKeyed, itemEntryKeyed_ );
    buttonTypes_.insert( showItemEntryImages, itemEntryImages_ );
    buttonTypes_.insert( showToggleLanguage, toggleLanguage_ );
    buttonTypes_.insert( showRezero, rezero_ );
    buttonTypes_.insert( showStartOver, startOver_ );
    buttonTypes_.insert( showBack, back_ );
    buttonTypes_.insert( showKeypad, keypad_ );


    if ( scaleConfig_ && ( scaleConfig_->scaleConfig.selfServeDisplayLabelOnScreenGO() ) && ( SelfServicePrintOption::SelfServOption_PrintToScreenAndLabel == scaleConfig_->scaleConfig.selfServeIgnorePrinterErrors() ) )
    {
        buttonTypes_.insert( showCancel, cancel_ );
        buttonTypes_.insert( showOkay, okay_ );
        buttonTypes_.insert( showPrintLabel, printLabel_ );
    }
    else
    {
        //buttonTypes_.insert( showPrintLabel, printLabel_ );
        printLabel_->setVisible( false );
        buttonTypes_.insert( showCancel, cancel_ );
        buttonTypes_.insert( showOkay, okay_ );
    }

}

//////////////////////////////////////////////////////////////////////////////////////////////////////
int selfServiceButtonBar::checkScreenSize()
{
    // laptop (1920x1080)
    int screen_size = 10;

    QDesktopWidget *desktop = QApplication::desktop();

    int screen = desktop->primaryScreen();

    if (( 0 <= screen ) && ( screen < desktop->screenCount() ))
    {
        QRect g( desktop->screenGeometry( screen ) );

        QSize resolution_ = g.size();

        if( resolution_.width() > 1034 && resolution_.height() > 610 )  // gave it 10 more but do I need to?
            screen_size = 14;
    }

    return screen_size;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceButtonBar::createLayout()
{
    QHBoxLayout *layout( new QHBoxLayout( this ) );
    layout->setContentsMargins( QMargins() );

    layout->addWidget( buttons_ );
}
