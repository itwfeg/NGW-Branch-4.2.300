/**
 * @file buildamealscreen.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "buildamealscreen.h"
#include "plumainscreen.h"

#include "../sessionmainwindow.h"
#include "../sessionmainwindowimpl.h"

#include "modes/modewidget.h"
#include "modes/operatemode.h"

#include "widgets/buildamealwidget.h"

#include <logmanager.h>
#include <softkeyswidget.h>

#include <Session.h>
#include <ScaleConfiguration.h>
#include <UiUtilities.h>
#include <Messages.h>

#include <QHBoxLayout>
#include <QStackedWidget>

///////////////////////////////////////////////////////////////////////////////////////////////////
MealBuilderScreen::MealBuilderScreen( SessionMainWindow *mainWindow, ModeWidget *parent ) :
    _Mybase( mainWindow, parent ),
    operateMode_( NULL ),
    pages_( NULL ),
    mealPage_( NULL ),
    meals_( NULL ),
    sideItemPage_( NULL ),
    sideItems_( NULL ),
    editor_( NULL ),
    weighMode_( -1 )
{
    // init
    initialize();
    createLayout();
    translate();

    // retrieve operate mode
    operateMode_ = dynamic_cast<OperateMode*>( mode_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
MealBuilderScreen::~MealBuilderScreen()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MealBuilderScreen::translate()
{
    // force reset
    resetFlashkeys( true );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MealBuilderScreen::resetFlashkeys( bool force )
{
    if( meals_ )
        meals_->sendResetMessage( force );
    //sideItems_->sendResetMessage( force );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MealBuilderScreen::resizeEvent( QResizeEvent *e )
{
    editor_->setFixedWidth( (30 * width()) / 100 );

    // retrieve operate mode
    if ( !operateMode_ )
        operateMode_ = dynamic_cast<OperateMode*>( mode_ );

    _Mybase::resizeEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MealBuilderScreen::onSessionMessage( const SessionMessage& msg )
{
    LOG_CLASS_INFO( "MealBuilderScreen", "onSessionMessage" );

    const int device( msg[0].toInt() );
    const int event( msg[1].toInt() );

    // softkeys
    if (( SoftkeyDev == device ) && ( LoadSoftkeyGroup == event ))
    {
        ScaleConfiguration *config = session_->scaleConfig;
        if( config->internalConfig.ssactive_lock )
            return;

        DomLoadSoftkeyGroup m( msg );

        // set softkeys
        LOG_DEBUG( "loading flashkeys...", NULL );
        meals_->create( &m );
        sideItems_->create( &m );
    }

    // weigh mode
    else if (( SessionDev == device ) && ( SnWriteWeighModeTransactionType == event ))
    {
        DomWeighModeTotalType *weighModeTotalType( &session_->scaleConfig->weighModeTotalType );

        // set proper operator page
        // 1) when in meal build mode we should show side items
        // 2) otherwise show meals
        if ( OpModeMealBuilder != weighModeTotalType->weighMode() )
            pages_->setCurrentWidget( mealPage_ );
        else
        {
            pages_->setCurrentWidget( sideItemPage_ );

            // when switching modes into meal builder show screen
            if ( OpModeMealBuilder != weighMode_ )
                mainWindow_->switchToModeScreen<OperateMode, MealBuilderScreen>();
        }

        // save off current mode
        weighMode_ = weighModeTotalType->weighMode();
    }

    _Mybase::onSessionMessage( msg );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ScreenWidget *MealBuilderScreen::swipeScreenDown() const
{
    ScreenWidget *screen( mode_->createScreen<PluMainScreen>() );

    return screen;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MealBuilderScreen::onScreenChanged()
{
    // only execute code when we are active screen
    if ( mode_->screen() != this )
        return;

    // hide weight bar
    if ( operateMode_ )
        operateMode_->hideWeightBar();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MealBuilderScreen::onSoftkeyClicked( QWidget *w, QWidget *b, int action, const QString& param, int feature )
{
    // for meals just send to operate mode (they may have other actions than just items)
    if (( meals_ == w ) && ( operateMode_ ))
        operateMode_->handleFlashkey( w, b, action, param, feature );

    // for side items page send directly to session (it should only be items and maybe back/forward buttons)
    else if ( sideItems_ == w )
    {
        session_->send( SoftkeyDev, ExecuteSoftkey, QString::number( action ) + "," + param + "," + QString::number( feature ) );
        session_->send( ProductDev, ProductRequestReadAllAccumulatedItems );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MealBuilderScreen::initialize()
{
    setContentsMargins( QMargins() );

    mealPage_ = new QWidget( this );
    mealPage_->setContentsMargins( QMargins() );
    mealPage_->setObjectName( "mealPage" );

    meals_ = new hobart::softkeysWidget( session_, mealPage_ ) ;
    meals_->setAutoFillBackground( true );
    meals_->setCreateFilter( hobart::softkeysWidget::BuildAMealScreenButtons );
    meals_->setObjectName( "meals" );
    connect( meals_, SIGNAL( clicked(QWidget*,QWidget*,int,QString,int) ), SLOT( onSoftkeyClicked(QWidget*,QWidget*,int,QString,int) ) );

    sideItemPage_ = new QWidget( this );
    sideItemPage_->setContentsMargins( QMargins() );
    sideItemPage_->setObjectName( "sideItemPage" );

    sideItems_ = new hobart::softkeysWidget( session_, sideItemPage_ ) ;
    sideItems_->setAutoFillBackground( true );
    sideItems_->setCreateFilter( hobart::softkeysWidget::BuildAMealSideItemButtons );
    sideItems_->setObjectName( "sideItems" );
    connect( sideItems_, SIGNAL( clicked(QWidget*,QWidget*,int,QString,int) ), SLOT( onSoftkeyClicked(QWidget*,QWidget*,int,QString,int) ) );

    editor_ = new hobart::mealBuilderWidget( sideItemPage_ );
    editor_->setObjectName( "editor" );

    pages_ = new QStackedWidget( this );
    pages_->setObjectName( "pages" );

    pages_->addWidget( mealPage_ );
    pages_->addWidget( sideItemPage_ );

    // connect slots
    connect( mode_, SIGNAL( screenChanged() ), SLOT( onScreenChanged() ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MealBuilderScreen::createLayout()
{
    static const QMargins screenMargins( 40, 25, 40, 25 );

    QHBoxLayout *mealPageLayout( new QHBoxLayout( mealPage_ ) );
    mealPageLayout->setContentsMargins( QMargins() );

    mealPageLayout->addWidget( meals_ );

    QHBoxLayout *sideItemPageLayout( new QHBoxLayout( sideItemPage_ ) );
    sideItemPageLayout->setContentsMargins( QMargins() );

    sideItemPageLayout->addWidget( sideItems_, 1 );
    sideItemPageLayout->addWidget( editor_ );

    QHBoxLayout *screen( new QHBoxLayout( this ) );
    screen->setContentsMargins( screenMargins );

    screen->addWidget( pages_ );
}
