/**
 * @file selfservicesoftkeyswidgetG2.cpp
 *
 * @copyright Copyright (c) 2023 Hobart Corporation
 * @author Job E. Crew
 * originalwork Randy Blankley
 */

#include <logmanager.h>

#include "selfservicesoftkeyswidgetG2.h"
#include "selfservicesoftkeybuttonG2.h"
#include "selfservicetoggledsoftkeybutton.h"

#include <backgroundwidgettypes.h>

#include <filesystem.h>
#include <pushbutton.h>
#include <selfservicehook.h>
#include <softkeyswidget.h>

#include <Messages.h>
#include <Session.h>
#include <ScaleConfiguration.h>
#include <ScaleClasses.h>

#include <QHBoxLayout>
#include <QLabel>
#include <QPalette>
#include <QPushButton>
#include <QLineEdit>
#include <QFont>
#include <QSignalMapper>
#include <QSplitter>
#include <QStackedWidget>
#include <QVBoxLayout>

#include <QDesktopWidget>


static const int ABC_STYLE( 0 );
static const int QWERTY_STYLE( 1 );

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////
selfServiceSoftkeysWidgetG2::selfServiceSoftkeysWidgetG2( Session *s, selfServiceHook *h, QWidget *parent ) :
    _Mybase( parent ),
    session_( s ),
    hook_( h ),
    scaleConfig_(NULL),
    showingBestSellers_( false ),
    searchPanelSize_( 0.16 ),
    searchFieldWidth_( 0 ),
    resultsFieldWidth_( 0 )
{
    // set object name
    setObjectName( "selfServiceSoftkeysWidgetG2" );

    // initialize
    initialize();

    // create layouts
    createLayout();

    // reset state to initial state
    resetState();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
selfServiceSoftkeysWidgetG2::~selfServiceSoftkeysWidgetG2()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
selfServiceSoftkeysWidgetG2::itemSearchMode selfServiceSoftkeysWidgetG2::searchMode() const
{
    int searchMethod( searchPanelSoftkeys_->currentIndex() );

    // check search method
    if ( searchByAZIndex_ == searchMethod )
        return searchByAZ;
    else if ( searchByCatIndex_ == searchMethod )
        return searchByCat;

    // should not get here!
    return searchByAZ;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceSoftkeysWidgetG2::setSearchMode( itemSearchMode value )
{
    // nothing to do
    if ( searchMode() == value )
        return;

    if ( searchByAZ == value )
        searchPanelSoftkeys_->setCurrentIndex( searchByAZIndex_ );
    else if ( searchByCat == value )
        searchPanelSoftkeys_->setCurrentIndex( searchByCatIndex_ );
    else
    {
        // should not get here!
    }

    // reset state to initial state
    resetState();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceSoftkeysWidgetG2::create( const DomLoadSoftkeyGroup* m )
{
    LOG_CLASS_INFO( "selfServiceSoftkeysWidgetG2", "create" );

    LOG_TRACE( "have load softkey group message", NULL );

    //if ( results_->create( m ) )
    //{
    //    //; // nothing to do - there is now
    //    if( scaleConfig_->scaleConfig.enableUseCharacterSearching() )
    //        updateResultsString( m ->featureNum() );
    //}
    //else if (( searchByAZ_->create( m ) ) || ( searchByCat_->create( m ) ))
    if (( searchByAZ_->create( m ) ) || ( searchByCat_->create( m ) ))
    {
        if ( showingBestSellers_ )
            toggleBestSellersButton();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceSoftkeysWidgetG2::reloadImages()
{
    const QPixmap prevArrow( filesystem::absoluteFilePath( "prevArrow.png", folderType::SelfServeImages ) );
    const QPixmap nextArrow( filesystem::absoluteFilePath( "nextArrow.png", folderType::SelfServeImages ) );

    prevPage_->setBackground( prevArrow );
    prevPage_->setBackgroundPressed( QPixmap() ); // this will force inverted image to be used

    nextPage_->setBackground(  nextArrow );
    nextPage_->setBackgroundPressed( QPixmap() ); // this will force inverted image to be used
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceSoftkeysWidgetG2::resetState()
{
    LOG_CLASS_INFO( "selfServiceSoftkeysWidgetG2", "resetState" );

    LOG_DEBUG( "reset state...", NULL );

    clear( true );

    // Request Most Common Items as long as we are not:
    //  1. Walmart Chili
    //  2. Walmart Canada
    //  3. Character by Character tuned on (tied to #2)

    // pjr383 - test if NOT keypad, which means we have flashkeys - refresh it
    //if ( session_ && !scaleConfig_->scaleConfig.SelfServeWeightRequiredToSelectItem() && ( !scaleConfig_->scaleConfig.SelfServeEnablePlaylists() || !scaleConfig_->scaleConfig.enableUseCharacterSearching() || !scaleConfig_->scaleConfig.selfServeCombineSearchKeypadFlashkeys() ) )
    // if( scaleConfig_->scaleConfig.selfServeItemEntryScreenState() < SelfServicePrintOption::SelfServOption_LookupKeypadOnly )
    if( scaleConfig_->scaleConfig.bestSellersOnSelfServeMainScreen() )
        session_->send( SoftkeyDev, ExecuteSoftkey, "42,," + QString::number( hobart::softkeysWidget::SelfservCategoryResultsButtons ) );

    // toggle button
    showingBestSellers_ = true;
    toggleBestSellersButton();
	
    updateCurrentState();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceSoftkeysWidgetG2::translate()
{
    LOG_CLASS_INFO( "selfServiceSoftkeysWidgetG2", "translate" );

    LOG_DEBUG( "translate...", NULL );

    // only do this if we are on the image/keyboard/search screen.
    if( scaleConfig_->scaleConfig.enableUseCharacterSearching() )
    {
        selfServiceHook::state s( hook_->currentState() );

        // This only pertains to lookups and the character keyboard.
        // only when selecting the Language button after boot
        if ( selfServiceHook::itemEntryImagesGen2 == s )
            if ( scaleConfig_->internalConfig.scaleBootComplete )
            {
                scaleConfig_->changeLanguage = 1;       // means we are changing language
                onCharacterSearchReturnPressed();       // this will do a new search with what is in the search text box
            }

        //translate flashkeys
        sendResetMessage(true);
    }
    else
    {
        // also need to get flashkeys again in new language...
        sendResetMessage( true );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceSoftkeysWidgetG2::resizeEvent( QResizeEvent *e )
{
    // update panel width
    updateSearchPanelWidth();

    _Mybase::resizeEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceSoftkeysWidgetG2::showEvent( QShowEvent *e )
{
    // refresh flashkeys
    sendResetMessage( true );

    if( characterSearch_ == NULL )
        return;

    if( scaleConfig_->scaleConfig.enableUseCharacterSearching() )
        characterSearch_->setFocus();

    _Mybase::showEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceSoftkeysWidgetG2::sendResetMessage( bool force )
{
    LOG_CLASS_INFO( "selfServiceSoftkeysWidgetG2", "sendResetMessage" );

    resetState();

    if( session_ && ( scaleConfig_->scaleConfig.enableUseCharacterSearching() ) )
    {
        LOG_DEBUG( "sending Char by Char reset message %d...", force );
        searchByCat_->sendResetMessage( force );
    }
    else if( session_ && ( SelfServicePrintOption::SelfServOption_AtoZ == scaleConfig_->scaleConfig.selfServeSearchScreenState() ) )
    {
        // SOF-4430 Self-Serve: A-Z Categories with Best Sellers does not automatically display Best Sellers, despite showing it selected
        // If A-Z searching turned on, this requests the current Most Common Items for you automatically when you enter the search screen
        LOG_DEBUG( "sending A-Z reset message %d...", force );
        searchByAZ_->sendResetMessage( force );
        session_->send( SoftkeyDev, ExecuteSoftkey, "42,," + QString::number( hobart::softkeysWidget::SelfservCategoryResultsButtons ) );
    }
    else if( session_ && ( SelfServicePrintOption::SelfServOption_LookupFixedFlashkeys == scaleConfig_->scaleConfig.selfServeItemEntryScreenState() ) ){
        LOG_DEBUG("resetting fixed flashkeys", NULL);
        session_->send( SoftkeyDev, ExecuteSoftkey, "42,," + QString::number( hobart::softkeysWidget::FreshServeFixedButtons ) );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceSoftkeysWidgetG2::updateCurrentState()
{
    // best sellers does not use prev/next icons
    prevPage_->setDisabled( showingBestSellers_ );
    nextPage_->setDisabled( showingBestSellers_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceSoftkeysWidgetG2::updateSearchPanelWidth()
{
    // fix search panel width to be fraction of total width
    //searchPanel_->setFixedWidth( qMax( ( searchFieldWidth_ + resultsFieldWidth_) , (int)(width() * searchPanelSize_) ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceSoftkeysWidgetG2::onButtonClicked( QWidget *w )
{
    LOG_CLASS_INFO( "selfServiceSoftkeysWidgetG2", "onButtonClicked" );

    // previous page of flashkeys
    if ( prevPage_ == w )
    {
        LOG_DEBUG( "prev button pressed", NULL );

        if ( session_ )
            session_->send( SoftkeyDev, ExecuteSoftkey, "41" );
    }

    // next page of flashkeys
    else if ( nextPage_ == w )
    {
        LOG_DEBUG( "next button pressed", NULL );

        if ( session_ )
            session_->send( SoftkeyDev, ExecuteSoftkey, "40" );
    }

    LOG_TRACE( "update state...", NULL );
    updateCurrentState();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceSoftkeysWidgetG2::onKeyButtonClicked( QWidget *w )
{
    LOG_CLASS_INFO( "selfServiceSoftkeysWidgetG2", "onKeyButtonClicked" );

    // we can only do these actions if we have a keyboard attached.
    if( !scaleConfig_->scaleConfig.enableUseCharacterSearching() )
        return;

    //if ( clear_ == w )
    //{
    //    clear(true);        // clear and show results string
    //    return;
    //}
    //else if( space_ == w )
    //{
    //    if( characterSearch_ == NULL )
    //        return;
    //
    //    QString value( characterSearch_->text() );
    //    value.append( " " );
    //
    //    characterSearch_->setText( value );
    //    characterSearch_->repaint();
    //    return;
    //}
    //else if( backspace_ == w )
    //{
    //    if( characterSearch_ == NULL )
    //        return;
    //
    //    QString value( characterSearch_->text() );
    //    value.chop(1);
    //
    //    characterSearch_->setText( value );
    //    characterSearch_->repaint();
    //    return;
    //}

    hobart::pushButton *button( dynamic_cast<hobart::pushButton*>( w ) );

    // append button text to line edit
    if ( button )
    {
        if( characterSearch_ == NULL )
            return;

        QString value( characterSearch_->text() );
        value.append( button->text() );

        characterSearch_->setText( value );
        characterSearch_->repaint();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceSoftkeysWidgetG2::onSoftkeyClicked( QWidget *w, QWidget *b, int action, const QString& param, int feature )
{
    // make sure search results visible and enabled
    if (( searchByAZ_ == w ) || ( searchByCat_ == w ))
    {
        showingBestSellers_ = (42 == action);

        if ( searchByAZ_ == w )
            searchByAZ_->setToggled( dynamic_cast<hobart::softkeyButton*>( b ) );
        else if ( searchByCat_ == w )
            searchByCat_->setToggled( dynamic_cast<hobart::softkeyButton*>( b ) );

        updateCurrentState();
    }

    // emit signal
    Q_EMIT softkeyClicked( w, b, action, param, feature );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceSoftkeysWidgetG2::clear(bool resetAll)
{
    Q_UNUSED( resetAll );

    if( characterSearch_ == NULL )
        return;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceSoftkeysWidgetG2::onCharacterSearchReturnPressed()
{
    if( characterSearch_ == NULL )
        return;

    QString text( characterSearch_->text() );


    // request items

    session_->send( SoftkeyDev, ExecuteSoftkey, "39," + text.toUpper() + "," + QString::number( hobart::softkeysWidget::SelfservCategoryResultsButtons ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceSoftkeysWidgetG2::onCharacterSearchChanged( const QString& text )
{
    // when the customer hits enter
    Q_UNUSED( text );

    onCharacterSearchReturnPressed();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceSoftkeysWidgetG2::initialize()
{
    characterSearch_ = NULL;
    keyboardSignalMapper_ = NULL;
    buttonsArea_ = NULL;
    //clear_ = NULL;
    //space_ = NULL;
    //backspace_ = NULL;

    scaleConfig_ = session_->scaleConfig;

    if( !scaleConfig_->scaleConfig.enableUseCharacterSearching() )
    {
        //------------------
        // A-Z Search ranges
        //------------------

        buttonSignalMapper_ = new QSignalMapper( this );
        connect( buttonSignalMapper_, SIGNAL( mapped(QWidget*) ), SLOT( onButtonClicked(QWidget*) ) );

        searchPanel_ = new QWidget();

        resultsPanel_ = new QWidget();

        splitter_ = new QSplitter( Qt::Horizontal, this );
        splitter_->addWidget( searchPanel_ );
        splitter_->addWidget( resultsPanel_ );

        splitter_->setStretchFactor( 0, 0 );
        splitter_->setStretchFactor( 1, 1 );

        // hide the splitter handle
        splitter_->setHandleWidth( 0 );

        QSplitterHandle *splitterHandle( splitter_->handle( 1 ) );
        splitterHandle->setFixedWidth( 0 );

        //
        // search panel
        //


        searchByAZ_ = new hobart::softkeysWidget( session_ ) ;
        searchByAZ_->setAutoFillBackground( true );
        searchByAZ_->setCreateFilter( hobart::softkeysWidget::SelfservAZCategoryButtons );
        searchByAZ_->setFactory( new selfServiceToggledSoftkeyButtonFactory() );
        connect( searchByAZ_, SIGNAL( clicked(QWidget*,QWidget*,int,QString,int) ), SLOT( onSoftkeyClicked(QWidget*,QWidget*,int,QString,int) ) );

        QPalette searchByAZPalette( searchByAZ_->palette() );
        searchByAZPalette.setColor( searchByAZ_->backgroundRole(), QColor( 32, 32, 32 ) );
        searchByAZ_->setPalette( searchByAZPalette );

        searchByCat_ = new hobart::softkeysWidget( session_ );
        searchByCat_->setAutoFillBackground( true );
        searchByCat_->setCreateFilter( hobart::softkeysWidget::SelfservCategoryButtons );
        searchByCat_->setFactory( new selfServiceToggledSoftkeyButtonFactory() );
        connect( searchByCat_, SIGNAL( clicked(QWidget*,QWidget*,int,QString,int) ), SLOT( onSoftkeyClicked(QWidget*,QWidget*,int,QString,int) ) );

        QPalette searchByCatPalette( searchByCat_->palette() );
        searchByCatPalette.setColor( searchByCat_->backgroundRole(), QColor( 32, 32, 32 ) );
        searchByCat_->setPalette( searchByCatPalette );

        searchPanelSoftkeys_ = new QStackedWidget( searchPanel_ );
        searchByAZIndex_ = searchPanelSoftkeys_->addWidget( searchByAZ_ );
        searchByCatIndex_ = searchPanelSoftkeys_->addWidget( searchByCat_ );

        //
        // results panel
        //

        results_ = new hobart::softkeysWidget( session_ );
        results_->setCreateFilter( hobart::softkeysWidget::SelfservCategoryResultsButtons );
        results_->setFactory( new selfServiceSoftkeyButtonFactoryG2() );
        connect( results_, SIGNAL( clicked(QWidget*,QWidget*,int,QString,int) ), SIGNAL( softkeyClicked(QWidget*,QWidget*,int,QString,int) ) );

        resultsPanelSoftkeys_ = new QStackedWidget( resultsPanel_ );
        resultsIndex_ = resultsPanelSoftkeys_->addWidget( results_ );

        prevPage_ = new hobart::pushButton( resultsPanel_ );
        connect( prevPage_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );

        nextPage_ = new hobart::pushButton( resultsPanel_ );
        connect( nextPage_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );

        buttonSignalMapper_->setMapping( prevPage_, prevPage_ );
        buttonSignalMapper_->setMapping( nextPage_, nextPage_ );
    }
    else
    {
        //------------------------------
        // Character-by-Character Search
        //------------------------------

        buttonSignalMapper_ = new QSignalMapper( this );
        keyboardSignalMapper_ = new QSignalMapper( this );
        connect( buttonSignalMapper_, SIGNAL( mapped(QWidget*) ), SLOT( onKeyButtonClicked(QWidget*) ) );

        searchPanel_ = new QWidget();
        resultsPanel_ = new QWidget();

        splitter_ = new QSplitter( Qt::Horizontal, this );
        splitter_->addWidget( searchPanel_ );
        splitter_->addWidget( resultsPanel_ );

        splitter_->setStretchFactor( 0, 0 );
        splitter_->setStretchFactor( 1, 1 );

        // hide the splitter handle
        splitter_->setHandleWidth( 0 );

        QSplitterHandle *splitterHandle( splitter_->handle( 1 ) );
        splitterHandle->setFixedWidth( 0 );

        //
        // search panel
        //

        QFont textFont;
        textFont.setPointSize( 24 );

        // input char by char search field
        characterSearch_ = new QLineEdit( searchPanel_ );
        characterSearch_->setFixedSize( 400,51);
        characterSearch_->setObjectName( "characterSearch");
        characterSearch_->setPalette( QApplication::palette() );
        characterSearch_->setFont( textFont);
        connect( characterSearch_, SIGNAL( returnPressed() ), SLOT( onCharacterSearchEnterPressed() ) );
        connect( characterSearch_, SIGNAL( textChanged(QString) ), SLOT( onCharacterSearchChanged(QString) ) );

        // A-Z Categories that I am going to hide
        searchByAZ_ = new hobart::softkeysWidget( session_ ) ;
        searchByAZ_->setAutoFillBackground( true );
        searchByAZ_->setCreateFilter( hobart::softkeysWidget::SelfservAZCategoryButtons );
        searchByAZ_->setFactory( new selfServiceToggledSoftkeyButtonFactory() );
        connect( searchByAZ_, SIGNAL( clicked(QWidget*,QWidget*,int,QString,int) ), SLOT( onSoftkeyClicked(QWidget*,QWidget*,int,QString,int) ) );

        QPalette searchByAZPalette( searchByAZ_->palette() );
        searchByAZPalette.setColor( searchByAZ_->backgroundRole(), QColor( 32, 32, 32 ) );
        searchByAZ_->setPalette( searchByAZPalette );

        // Walmart Chili request
        searchByCat_ = new hobart::softkeysWidget( session_ );
        searchByCat_->setAutoFillBackground( true );
        searchByCat_->setCreateFilter( hobart::softkeysWidget::SelfservCategoryButtons );
        searchByCat_->setFactory( new selfServiceToggledSoftkeyButtonFactory() );
        connect( searchByCat_, SIGNAL( clicked(QWidget*,QWidget*,int,QString,int) ), SLOT( onSoftkeyClicked(QWidget*,QWidget*,int,QString,int) ) );

        QPalette searchByCatPalette( searchByCat_->palette() );
        searchByCatPalette.setColor( searchByCat_->backgroundRole(), QColor( 32, 32, 32 ) );
        searchByCat_->setPalette( searchByCatPalette );

        // Stack the searches
        searchPanelSoftkeys_ = new QStackedWidget( searchPanel_ );
        searchByAZIndex_ = searchPanelSoftkeys_->addWidget( searchByAZ_ );
        searchByCatIndex_ = searchPanelSoftkeys_->addWidget( searchByCat_ );

        //
        // results panel
        //

        results_ = new hobart::softkeysWidget( session_ );
        results_->setCreateFilter( hobart::softkeysWidget::SelfservCategoryResultsButtons );
        results_->setFactory( new selfServiceSoftkeyButtonFactoryG2() );
        connect( results_, SIGNAL( clicked(QWidget*,QWidget*,int,QString,int) ), SIGNAL( softkeyClicked(QWidget*,QWidget*,int,QString,int) ) );

        resultsPanelSoftkeys_ = new QStackedWidget( resultsPanel_ );
        resultsIndex_ = resultsPanelSoftkeys_->addWidget( results_ );

        prevPage_ = new hobart::pushButton( resultsPanel_ );
        connect( prevPage_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );

        nextPage_ = new hobart::pushButton( resultsPanel_ );
        connect( nextPage_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );

        buttonSignalMapper_->setMapping( prevPage_, prevPage_ );
        buttonSignalMapper_->setMapping( nextPage_, nextPage_ );
    }

    reloadImages();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceSoftkeysWidgetG2::createLayout()
{
    QVBoxLayout *widgetLayout( new QVBoxLayout( this ) );
    widgetLayout->setContentsMargins( QMargins() );
    widgetLayout->addWidget( splitter_ );

    //
    // search panel
    //

    if( !scaleConfig_->scaleConfig.enableUseCharacterSearching() )
    {
        //------------------
        // A-Z Search ranges
        //------------------

        QVBoxLayout *searchPanelLayout( new QVBoxLayout( searchPanel_ ) );
        searchPanelLayout->setContentsMargins( QMargins() );

        //searchPanelLayout->addLayout( logoLayout );
        searchPanelLayout->addWidget( searchPanelSoftkeys_, 1 );

        //
        // results panel
        //

        QVBoxLayout *navigationLayout( new QVBoxLayout() );
        navigationLayout->addStretch();
        navigationLayout->addWidget( prevPage_ );
        navigationLayout->addStretch();
        navigationLayout->addWidget( nextPage_ );
        navigationLayout->addStretch();

        QHBoxLayout *resultsPanelLayout( new QHBoxLayout( resultsPanel_ ) );
        resultsPanelLayout->setContentsMargins( QMargins() );
        resultsPanelLayout->addWidget( resultsPanelSoftkeys_, 1 );
        resultsPanelLayout->addLayout( navigationLayout );
    }
    else
    {
        //------------------------------
        // Character-by-Character Search
        //------------------------------

        if( characterSearch_ == NULL )
            return;

        //QHBoxLayout *logoLayout( new QHBoxLayout() );
        //logoLayout->setContentsMargins( QMargins() );

        //logoLayout->addWidget( storeLogo_);
        //logoLayout->setSpacing( spSize );
        //logoLayout->addStretch();
        //logoLayout->addWidget( characterSearch_ , 0, Qt::AlignRight );
        //logoLayout->addWidget(resultsTag_);
        //logoLayout->addStretch();

        // The search panel is for A-Z/Category buttons. We have to create it, but we won't use it.
        QVBoxLayout *searchPanelLayout( new QVBoxLayout( searchPanel_ ) );
        searchPanelLayout->setContentsMargins( QMargins() );
        searchPanelLayout->addWidget( searchPanelSoftkeys_, 1 );
        searchPanel_->hide();                   // we don't need this panel

        //
        // results panel
        //

        // we had to create these, but we hide them too.  May have to bring them back.
        prevPage_->hide();
        nextPage_->hide();

        // ----- keyboard ----- //
        // create some buttons and use that instead of using spaceFiller

        // ----- Results Flashkey Area ----- //
        QVBoxLayout *resultsPanelLayout( new QVBoxLayout() );
        resultsPanelLayout->setContentsMargins( QMargins() );
        resultsPanelLayout->addWidget( resultsPanelSoftkeys_);
        resultsPanelLayout->addWidget( buttonsArea_ , 0, Qt::AlignCenter);

        // ----- Results Flashkey Area ----- //
        QVBoxLayout *formLayout( new QVBoxLayout( resultsPanel_ ) );
        formLayout->setContentsMargins( QMargins() );
        //formLayout->addLayout( logoLayout);
        formLayout->addLayout( resultsPanelLayout);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceSoftkeysWidgetG2::toggleBestSellersButton()
{
    softkeyButton *b;

    if ( (b = searchByAZ_->findButton( 42 )) )
        searchByAZ_->setToggled( b );

    if ( (b = searchByCat_->findButton( 42 )) )
        searchByCat_->setToggled( b );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace
