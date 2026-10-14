/**
 * @file meatspecsheetscreen.cpp
 * Screen for Cutting Test Meat Specification Sheets.
 *
 * @copyright Copyright (c) 2016 Hobart Corporation
 * @author Joe E. Crew
 * @inspired by Randy Blankley
 */

#include "meatspecsheetscreen.h"
#include "plumainscreen.h"

#include "../sessionmainwindow.h"
#include "../sessionmainwindowimpl.h"

#include "modes/modewidget.h"
#include "modes/operatemode.h"

#include "widgets/meatspecsheetwidget.h"

#include <logmanager.h>
#include <softkeyswidget.h>

#include <Session.h>
#include <ScaleConfiguration.h>
#include <UiUtilities.h>
#include <Messages.h>

#include <QHBoxLayout>
#include <QStackedWidget>

///////////////////////////////////////////////////////////////////////////////////////////////////
MeatSheetScreen::MeatSheetScreen( SessionMainWindow *mainWindow, ModeWidget *parent ) :
    _Mybase( mainWindow, parent ),
    operateMode_( NULL ),
    pages_( NULL ),
    meatPage_( NULL ),
    primals_( NULL ),
    subPrimals_( NULL ),
    editor_( editor_ ),
    subPrimalPage_( NULL ),
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
MeatSheetScreen::~MeatSheetScreen()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MeatSheetScreen::translate()
{
    // force reset
    resetFlashkeys( true );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MeatSheetScreen::resetFlashkeys( bool force )
{
    Q_UNUSED( force );

    //primals_->sendResetMessage( force );
    if( subPrimals_ )
    {
        // subPrimals_->sendResetMessage( force );      // did not handle change language HT-223

        ///< create some pointer to session data
        ScaleConfiguration *scaleConfiguration( session_->scaleConfig );
        MadpCutTestPlu *cutTestPlu( &scaleConfiguration->cutTestPlu );

        int mean_sheet_int = cutTestPlu->meatSpecSheet;
        QString meat_sheet = QString::number( cutTestPlu->meatSpecSheet );
        bool isActive = mainWindow_->isCuttingTestActive();

        if( isActive && mean_sheet_int > 0 )
        {
            QString feature = QString::number( hobart::softkeysWidget::MeatSpecSheetScreeenButtons );
            session_->send( SoftkeyDev, LoadSoftkeyGroup, feature + "," + meat_sheet );       // (136,93)
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MeatSheetScreen::clearFlashkeys()
{
    ScaleConfiguration *scaleConfiguration( session_->scaleConfig );

    scaleConfiguration->meatSheetScreenTimedOut = true;

    QString feature = QString::number( hobart::softkeysWidget::MeatSpecSheetScreeenButtons );
    session_->send( SoftkeyDev, LoadSoftkeyGroup, feature + "," + feature );       // (136,93,default group)
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MeatSheetScreen::resizeEvent( QResizeEvent *e )
{
    //editor_->setFixedWidth( (30 * width()) / 100 );

    // retrieve operate mode
    if ( !operateMode_ )
        operateMode_ = dynamic_cast<OperateMode*>( mode_ );

    _Mybase::resizeEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MeatSheetScreen::onSessionMessage( const SessionMessage& msg )
{
    LOG_CLASS_INFO( "MeatSheetScreen", "onSessionMessage" );

    const int device( msg[0].toInt() );
    const int event( msg[1].toInt() );


    // softkeys
    if (( SoftkeyDev == device ) && ( LoadSoftkeyGroup == event ))
    {
        DomLoadSoftkeyGroup m( msg );

        const int featureNum( msg[2].toInt() );
        bool isActive = mainWindow_->isCuttingTestActive();

        // set softkeys
        LOG_DEBUG( "loading flashkeys...", NULL );

        // kinda confused about this. Will it put any softkey group in here? And the same
        // for both primals and subs?  Should i test for a group number?

        //primals_->create( &m );         // MeatSpecSheetScreeenButtons   = 999033,
        subPrimals_->create( &m );      // MeatSpecSheetSideItemButtons  = 999034

        if(  featureNum == 999033 &&  isActive )
        {
            // 136,93
            ScaleConfiguration *scaleConfiguration( session_->scaleConfig );
            scaleConfiguration->cuttingTestMeatSpecActive = true;
            pages_->setCurrentWidget( meatPage_ );                     // mealPage_
            mainWindow_->switchToModeScreen<OperateMode, MeatSheetScreen>();
        }
    }

    // weigh mode
    else if (( SessionDev == device ) && ( SnWriteWeighModeTransactionType == event ))
    {
        //if ( OpModeMeatSheets != weighModeTotalType->weighMode() )

        bool isActive = mainWindow_->isCuttingTestActive();

        if( isActive && meatPage_ )
            pages_->setCurrentWidget( meatPage_ );
    }

    _Mybase::onSessionMessage( msg );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ScreenWidget *MeatSheetScreen::swipeScreenDown() const
{
    ScreenWidget *screen( mode_->createScreen<PluMainScreen>() );

    return screen;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MeatSheetScreen::onScreenChanged()
{
    // only execute code when we are active screen
    if ( mode_->screen() != this )
        return;

    // hide weight bar
    if ( operateMode_ )
        operateMode_->hideWeightBar();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MeatSheetScreen::onSoftkeyClicked( QWidget *w, QWidget *b, int action, const QString& param, int feature )
{
    Q_UNUSED( w );
    Q_UNUSED( b );
    Q_UNUSED( action );
    Q_UNUSED( feature );
    QString plunum( param.trimmed() );

    if ( param.isEmpty() )
        plunum = "0";

    session_->send( SessionDev, SnReadWeighModeTransactionType );
    session_->send( ProductDev, ProductRead, plunum + ",3" );

    // switch back to the main screen to display the PLU even if its empty (zero)
    mainWindow_->switchToModeScreen<OperateMode, PluMainScreen>();

    ScaleConfiguration *scaleConfiguration( session_->scaleConfig );
    scaleConfiguration->cuttingTestMeatSpecActive = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MeatSheetScreen::initialize()
{
    setContentsMargins( QMargins() );

    meatPage_ = new QWidget( this );                                                         // mealPage_
    meatPage_->setContentsMargins( QMargins() );                                             // mealPage_
    meatPage_->setObjectName( "primalPage" );                                                // mealPage_

    subPrimalPage_ = new QWidget( this );                                                    // sideItemPage_
    subPrimalPage_->setContentsMargins( QMargins() );                                        // sideItemPage_
    subPrimalPage_->setObjectName( "subPrimalPage" );                                        // sideItemPage_

    subPrimals_ = new hobart::softkeysWidget( session_, subPrimalPage_ ) ;                   // sideItemPage_ sideItems_
    subPrimals_->setAutoFillBackground( true );                                              // sideItems_
    subPrimals_->setCreateFilter( hobart::softkeysWidget::MeatSpecSheetScreeenButtons );
    subPrimals_->setObjectName( "subPrimals" );                                              // sideItems_
    connect( subPrimals_,       SIGNAL( clicked(QWidget*,QWidget*,int,QString,int) ),   SLOT( onSoftkeyClicked(QWidget*,QWidget*,int,QString,int) ) );     // sideItems_

    pages_ = new QStackedWidget( this );
    pages_->setObjectName( "pages" );

    pages_->addWidget( subPrimalPage_ );                         // sideItemPage_

    // connect slots
    connect( mode_, SIGNAL( screenChanged() ), SLOT( onScreenChanged() ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MeatSheetScreen::createLayout()
{
    static const QMargins screenMargins( 40, 25, 40, 25 );

    QHBoxLayout *mealPageLayout( new QHBoxLayout( meatPage_ ) );          // mealPage_
    mealPageLayout->setContentsMargins( QMargins() );
    
    QHBoxLayout *sideItemPageLayout( new QHBoxLayout( subPrimalPage_ ) );  // sideItemPage_
    sideItemPageLayout->setContentsMargins( QMargins() );

    sideItemPageLayout->addWidget( subPrimals_, 1 );                       // sideItems_

    QHBoxLayout *screen( new QHBoxLayout( this ) );
    screen->setContentsMargins( screenMargins );

    screen->addWidget( pages_ );
}
