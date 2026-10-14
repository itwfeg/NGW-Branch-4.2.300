/**
 * @file selfservicesoftkeyswidget.cpp
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "selfservicesoftkeyswidget.h"
#include "selfservicesoftkeybutton.h"
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
selfServiceSoftkeysWidget::selfServiceSoftkeysWidget( Session *s, selfServiceHook *h, QWidget *parent ) :
    _Mybase( parent ),
    session_( s ),
    hook_( h ),
    scaleConfig_(NULL),
    showingBestSellers_( false ),
    searchPanelSize_( 0.16 ),
    storeLogoWidth_( 0 ),
    searchFieldWidth_( 0 ),
    resultsFieldWidth_( 0 )
{
    // set object name
    setObjectName( "selfServiceSoftkeysWidget" );

    // initialize
    initialize();

    // create layouts
    createLayout();

    // reset state to initial state
    resetState();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
selfServiceSoftkeysWidget::~selfServiceSoftkeysWidget()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
selfServiceSoftkeysWidget::itemSearchMode selfServiceSoftkeysWidget::searchMode() const
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
void selfServiceSoftkeysWidget::setSearchMode( itemSearchMode value )
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
void selfServiceSoftkeysWidget::setStoreLogo( const QPixmap& value )
{
    // bad image
    if ( value.isNull() )
    {
        storeLogo_->hide();
        storeLogoWidth_ = 0;
        searchFieldWidth_ = 0;
    }
    else
    {
        // set size
        storeLogo_->setFixedSize( value.size() );
        storeLogoWidth_ = value.width();

        // set image
        QPalette palette( storeLogo_->palette() );
        palette.setBrush( storeLogo_->backgroundRole(), value );
        storeLogo_->setPalette( palette );

        // show!
        storeLogo_->setAutoFillBackground( true );
        storeLogo_->show();

        if( scaleConfig_->scaleConfig.enableUseCharacterSearching() )
        {
            if ( characterSearch_ == NULL )
                return;

            // Add the search text deminsions here too
            characterSearch_->setFixedSize( 400,51 );
            characterSearch_->setPalette( QApplication::palette() );
            searchFieldWidth_ = value.width();

            // output to users
            resultsTag_->setFixedSize( 400,51 );
            resultsTag_->setPalette( QApplication::palette() );
            resultsFieldWidth_ = value.width();
        }
        else
        {
            searchFieldWidth_ = 0;
            resultsFieldWidth_ = 0;
        }
    }

    // update panel width
    updateSearchPanelWidth();
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceSoftkeysWidget::create( const DomLoadSoftkeyGroup* m )
{
    LOG_CLASS_INFO( "selfServiceSoftkeysWidget", "create" );

    LOG_TRACE( "have load softkey group message", NULL );

    if ( results_->create( m ) )
    {
        //; // nothing to do - there is now
        if( scaleConfig_->scaleConfig.enableUseCharacterSearching() )
            updateResultsString( m ->featureNum() );
    }
    else if (( searchByAZ_->create( m ) ) || ( searchByCat_->create( m ) ))
    {
        if ( showingBestSellers_ )
            toggleBestSellersButton();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceSoftkeysWidget::reloadImages()
{
    const QPixmap prevArrow( filesystem::absoluteFilePath( "prevArrow.png", folderType::SelfServeImages ) );
    const QPixmap nextArrow( filesystem::absoluteFilePath( "nextArrow.png", folderType::SelfServeImages ) );

    prevPage_->setBackground( prevArrow );
    prevPage_->setBackgroundPressed( QPixmap() ); // this will force inverted image to be used

    nextPage_->setBackground(  nextArrow );
    nextPage_->setBackgroundPressed( QPixmap() ); // this will force inverted image to be used
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceSoftkeysWidget::resetState()
{
    LOG_CLASS_INFO( "selfServiceSoftkeysWidget", "resetState" );

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
void selfServiceSoftkeysWidget::translate()
{
    LOG_CLASS_INFO( "selfServiceSoftkeysWidget", "translate" );

    LOG_DEBUG( "translate...", NULL );

    // only do this if we are on the image/keyboard/search screen.
    if( scaleConfig_->scaleConfig.enableUseCharacterSearching() )
    {
        selfServiceHook::state s( hook_->currentState() );

        // This only pertains to lookups and the character keyboard.
        // only when selecting the Language button after boot
        if ( selfServiceHook::itemEntryImages == s )
            if ( scaleConfig_->internalConfig.scaleBootComplete )
            {
                scaleConfig_->changeLanguage = 1;       // means we are changing language
                onCharacterSearchReturnPressed();       // this will do a new search with what is in the search text box
            }

        if ( clear_ )
            clear_->setText( tr("clear") );

        if ( space_ )
            space_->setText( tr("space") );

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
void selfServiceSoftkeysWidget::resizeEvent( QResizeEvent *e )
{
    // update panel width
    updateSearchPanelWidth();

    _Mybase::resizeEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceSoftkeysWidget::showEvent( QShowEvent *e )
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
void selfServiceSoftkeysWidget::sendResetMessage( bool force )
{
    LOG_CLASS_INFO( "selfServiceSoftkeysWidget", "sendResetMessage" );

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
    else if( session_ && ( SelfServicePrintOption::SelfServOption_LookupFixedFlashkeys == scaleConfig_->scaleConfig.selfServeItemEntryScreenState() ) )
    {
        // WATCH THIS
        LOG_DEBUG("resetting fixed flashkeys", NULL);
        session_->send( SoftkeyDev, ExecuteSoftkey, "42,," + QString::number( hobart::softkeysWidget::FixedRowColumnGrid ) );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceSoftkeysWidget::updateCurrentState()
{
    // best sellers does not use prev/next icons
    prevPage_->setDisabled( showingBestSellers_ );
    nextPage_->setDisabled( showingBestSellers_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceSoftkeysWidget::updateSearchPanelWidth()
{
    // fix search panel width to be fraction of total width
    searchPanel_->setFixedWidth( qMax( (storeLogoWidth_ + searchFieldWidth_ + resultsFieldWidth_) , (int)(width() * searchPanelSize_) ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceSoftkeysWidget::onButtonClicked( QWidget *w )
{
    LOG_CLASS_INFO( "selfServiceSoftkeysWidget", "onButtonClicked" );

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
void selfServiceSoftkeysWidget::onKeyButtonClicked( QWidget *w )
{
    LOG_CLASS_INFO( "selfServiceSoftkeysWidget", "onKeyButtonClicked" );

    // we can only do these actions if we have a keyboard attached.
    if( !scaleConfig_->scaleConfig.enableUseCharacterSearching() )
        return;

    if ( clear_ == w )
    {
        clear(true);        // clear and show results string
        return;
    }
    else if( space_ == w )
    {
        if( characterSearch_ == NULL )
            return;

        QString value( characterSearch_->text() );
        value.append( " " );

        characterSearch_->setText( value );
        characterSearch_->repaint();
        return;
    }
    else if( backspace_ == w )
    {
        if( characterSearch_ == NULL )
            return;

        QString value( characterSearch_->text() );
        value.chop(1);

        characterSearch_->setText( value );
        characterSearch_->repaint();
        return;
    }

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
void selfServiceSoftkeysWidget::onSoftkeyClicked( QWidget *w, QWidget *b, int action, const QString& param, int feature )
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
void selfServiceSoftkeysWidget::clear(bool resetAll)
{
    if( characterSearch_ == NULL )
        return;

    if( scaleConfig_->scaleConfig.enableUseCharacterSearching() )
    {
        resultsTag_->setText(tr(" No results available "));

        if( resetAll )
            characterSearch_->setText("");
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceSoftkeysWidget::onCharacterSearchReturnPressed()
{
    if( characterSearch_ == NULL )
        return;

    QString text( characterSearch_->text() );


    // request items

    session_->send( SoftkeyDev, ExecuteSoftkey, "39," + text.toUpper() + "," + QString::number( hobart::softkeysWidget::SelfservCategoryResultsButtons ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceSoftkeysWidget::onCharacterSearchChanged( const QString& text )
{
    // when the customer hits enter
    Q_UNUSED( text );

    onCharacterSearchReturnPressed();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceSoftkeysWidget::initialize()
{
    characterSearch_ = NULL;
    keyboardSignalMapper_ = NULL;
    resultsTag_ = NULL;
    buttonsArea_ = NULL;
    clear_ = NULL;
    space_ = NULL;
    backspace_ = NULL;

    int screensize = checkScreenSize();
    //int spSize = 10;
    int spSize = 5;

    if( screensize == 10 )
    {
        spSize = 5;
    }

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

        storeLogo_ = new QLabel( searchPanel_ );
        storeLogo_->hide();

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
        results_->setFactory( new selfServiceSoftkeyButtonFactory() );
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

        // Logo
        storeLogo_ = new QLabel( searchPanel_ );
        storeLogo_->hide();

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

        // searching results details to consumer
        resultsTag_ = new QLabel(searchPanel_);
        resultsTag_->setFont( textFont );

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
        results_->setFactory( new selfServiceSoftkeyButtonFactory() );
        connect( results_, SIGNAL( clicked(QWidget*,QWidget*,int,QString,int) ), SIGNAL( softkeyClicked(QWidget*,QWidget*,int,QString,int) ) );

        resultsPanelSoftkeys_ = new QStackedWidget( resultsPanel_ );
        resultsIndex_ = resultsPanelSoftkeys_->addWidget( results_ );

        prevPage_ = new hobart::pushButton( resultsPanel_ );
        connect( prevPage_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );

        nextPage_ = new hobart::pushButton( resultsPanel_ );
        connect( nextPage_, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );

        buttonSignalMapper_->setMapping( prevPage_, prevPage_ );
        buttonSignalMapper_->setMapping( nextPage_, nextPage_ );

        // ----- Keyboard Setup Area ----- //

        buttonsArea_ = new QWidget();
        buttonsArea_->setAutoFillBackground( false );

        // The background behind the buttons appearance
        QPalette buttonsAreaPalette( buttonsArea_->palette() );
        buttonsAreaPalette.setColor( buttonsArea_->backgroundRole(), QColor( 32, 32, 32 ) );

        buttonsArea_->setPalette( buttonsAreaPalette );
        buttonsArea_->setFixedSize( (window()->width() * 800) / 1024, (window()->height() * 250) / 600 );

        // Set up the different button styles

        // Regular size button - A-Z

        QPixmap button_1_1;
        QPixmap button_3_1;
        QPixmap button_4_1;

        if( screensize == 14 )
        {
            const hobart::backgroundWidgetRenderer renderKeyPixmap(
                new hobart::panelButtonBackgroundWidget( 4 ),
                new hobart::darkButtonBackgroundWidgetRendererAppearance );

            const QSize renderKeyPixmapSize( 75, 75 );
            const QSize renderKeyHorzPixmapSize( 155, 75 );
            const QSize renderKey3XHorzPixmapSize( 239, 75 );

            button_1_1 = renderKeyPixmap.drawControl( renderKeyPixmapSize, hobart::ReadOnly );
            button_3_1 = renderKeyPixmap.drawControl( renderKeyHorzPixmapSize, hobart::ReadOnly );
            button_4_1 = renderKeyPixmap.drawControl( renderKey3XHorzPixmapSize, hobart::ReadOnly );
        }
        else
        {
            const hobart::backgroundWidgetRenderer renderKeyPixmap(
                new hobart::panelButtonBackgroundWidget( 4 ),
                new hobart::darkButtonBackgroundWidgetRendererAppearance );

            const QSize renderKeyPixmapSize( 60, 60 );
            const QSize renderKeyHorzPixmapSize( 185, 60 );
            const QSize renderKey3XHorzPixmapSize( 244, 60 );

            button_1_1 = renderKeyPixmap.drawControl( renderKeyPixmapSize, hobart::ReadOnly );
            button_3_1 = renderKeyPixmap.drawControl( renderKeyHorzPixmapSize, hobart::ReadOnly );
            button_4_1 = renderKeyPixmap.drawControl( renderKey3XHorzPixmapSize, hobart::ReadOnly );
        }

        typedef QPair<QString, QPixmap> buttonInfo;
        QList<buttonInfo> buttonInfoList;

        CLEAR_KEY = tr("clear");
        SPACE_KEY = tr("space");
        BACKSPACE_KEY = "<<";

        // list of buttons to create
        buttonInfoList.append( buttonInfo( "A", button_1_1 ) );
        buttonInfoList.append( buttonInfo( "B", button_1_1 ) );
        buttonInfoList.append( buttonInfo( "C", button_1_1 ) );
        buttonInfoList.append( buttonInfo( "D", button_1_1 ) );
        buttonInfoList.append( buttonInfo( "E", button_1_1 ) );
        buttonInfoList.append( buttonInfo( "F", button_1_1 ) );
        buttonInfoList.append( buttonInfo( "G", button_1_1 ) );
        buttonInfoList.append( buttonInfo( "H", button_1_1 ) );
        buttonInfoList.append( buttonInfo( "I", button_1_1 ) );
        buttonInfoList.append( buttonInfo( "J", button_1_1 ) );

        buttonInfoList.append( buttonInfo( "K", button_1_1 ) );
        buttonInfoList.append( buttonInfo( "L", button_1_1 ) );
        buttonInfoList.append( buttonInfo( "M", button_1_1 ) );
        buttonInfoList.append( buttonInfo( "N", button_1_1 ) );
        buttonInfoList.append( buttonInfo( "O", button_1_1 ) );
        buttonInfoList.append( buttonInfo( "P", button_1_1 ) );
        buttonInfoList.append( buttonInfo( "Q", button_1_1 ) );
        buttonInfoList.append( buttonInfo( "R", button_1_1 ) );
        buttonInfoList.append( buttonInfo( "S", button_1_1 ) );
        buttonInfoList.append( buttonInfo( "T", button_1_1 ) );

        buttonInfoList.append( buttonInfo( "U", button_1_1 ) );
        buttonInfoList.append( buttonInfo( "V", button_1_1 ) );
        buttonInfoList.append( buttonInfo( "W", button_1_1 ) );
        buttonInfoList.append( buttonInfo( "X", button_1_1 ) );
        buttonInfoList.append( buttonInfo( "Y", button_1_1 ) );
        buttonInfoList.append( buttonInfo( "Z", button_1_1 ) );

        buttonInfoList.append( buttonInfo( CLEAR_KEY, button_3_1 ) );
        buttonInfoList.append( buttonInfo( SPACE_KEY, button_4_1 ) );
        buttonInfoList.append( buttonInfo( BACKSPACE_KEY, button_3_1 ) );

        foreach ( const buttonInfo& bi, buttonInfoList )
        {
            hobart::pushButton *button( new hobart::pushButton( buttonsArea_, bi.second ) );
            button->setFocusPolicy( Qt::NoFocus );
            button->setText( bi.first );

            QPalette buttonPalette( button->palette() );
            buttonPalette.setColor( button->foregroundRole(), hobart::colorPalette::white );
            button->setPalette( buttonPalette );

            QFont buttonFont( button->font() );
            buttonFont.setPointSize( 24 );
            button->setFont( buttonFont );

            connect( button, SIGNAL( clicked() ), buttonSignalMapper_, SLOT( map() ) );
            buttonSignalMapper_->setMapping( button, button );

            buttons_[bi.first] = button;
        }

        clear_ = buttons_[CLEAR_KEY];
        space_ = buttons_[SPACE_KEY];
        backspace_ = buttons_[BACKSPACE_KEY];
    }

    reloadImages();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceSoftkeysWidget::createLayout()
{
    QVBoxLayout *widgetLayout( new QVBoxLayout( this ) );
    widgetLayout->setContentsMargins( QMargins() );
    widgetLayout->addWidget( splitter_ );

    int screensize = checkScreenSize();
    //int spSize = 10;
    int spSize = 5;
    int fixedSizeWidth = 1200;
    int fixedSizeHeight = 240;

    if( screensize == 10 )
    {
        spSize = 5;
        fixedSizeWidth = 960;
        fixedSizeHeight = 200;
    }

    //
    // search panel
    //

    if( !scaleConfig_->scaleConfig.enableUseCharacterSearching() )
    {
        //------------------
        // A-Z Search ranges
        //------------------

        QHBoxLayout *logoLayout( new QHBoxLayout() );
        logoLayout->setContentsMargins( QMargins() );

        logoLayout->addWidget( storeLogo_ );
        logoLayout->addStretch();

        QVBoxLayout *searchPanelLayout( new QVBoxLayout( searchPanel_ ) );
        searchPanelLayout->setContentsMargins( QMargins() );

        searchPanelLayout->addLayout( logoLayout );
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

        QHBoxLayout *logoLayout( new QHBoxLayout() );
        logoLayout->setContentsMargins( QMargins() );

        logoLayout->addWidget( storeLogo_);
        logoLayout->setSpacing( spSize );
        logoLayout->addStretch();
        logoLayout->addWidget( characterSearch_ , 0, Qt::AlignRight );
        logoLayout->addWidget(resultsTag_);
        logoLayout->addStretch();

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

        QVBoxLayout *buttonsAreaLayout( new QVBoxLayout( buttonsArea_ ) );
        buttonsAreaLayout->setObjectName( "buttonsAreaLayout");
        buttonsAreaLayout->setContentsMargins( QMargins( spSize, spSize, spSize, spSize ) );        // These were each 10
        buttonsAreaLayout->setSpacing( spSize );
        buttonsArea_->setFixedSize( fixedSizeWidth, fixedSizeHeight );

        // if we get a real configuration value on this, we sessionHooks::numberPadStyle as an example

        QHBoxLayout *row_1( new QHBoxLayout() );
        row_1->setContentsMargins( QMargins() );
        row_1->setSpacing( spSize );     // These were each 10

        QHBoxLayout *row_2( new QHBoxLayout() );
        row_2->setContentsMargins( QMargins() );
        row_2->setSpacing( spSize );     // These were each 10

        QHBoxLayout *row_3( new QHBoxLayout() );
        row_3->setContentsMargins( QMargins() );
        row_3->setSpacing( spSize );     // These were each 10

        buttonsAreaLayout->addLayout( row_1 );
        buttonsAreaLayout->addLayout( row_2 );
        buttonsAreaLayout->addLayout( row_3 );

        row_1->addStretch();
        row_1->addWidget( buttons_["Q"] );
        row_1->addWidget( buttons_["W"] );
        row_1->addWidget( buttons_["E"] );
        row_1->addWidget( buttons_["R"] );
        row_1->addWidget( buttons_["T"] );
        row_1->addWidget( buttons_["Y"] );
        row_1->addWidget( buttons_["U"] );
        row_1->addWidget( buttons_["I"] );
        row_1->addWidget( buttons_["O"] );
        row_1->addWidget( buttons_["P"] );
        row_1->addWidget( buttons_[BACKSPACE_KEY] );
        row_1->addStretch();

        row_2->addStretch();
        row_2->addWidget( buttons_["A"] );
        row_2->addWidget( buttons_["S"] );
        row_2->addWidget( buttons_["D"] );
        row_2->addWidget( buttons_["F"] );
        row_2->addWidget( buttons_["G"] );
        row_2->addWidget( buttons_["H"] );
        row_2->addWidget( buttons_["J"] );
        row_2->addWidget( buttons_["K"] );
        row_2->addWidget( buttons_["L"] );
        row_2->addWidget( buttons_[CLEAR_KEY] );
        row_2->addStretch();

        row_3->addStretch();
        row_3->addWidget( buttons_["Z"] );
        row_3->addWidget( buttons_["X"] );
        row_3->addWidget( buttons_["C"] );
        row_3->addWidget( buttons_["V"] );
        row_3->addWidget( buttons_["B"] );
        row_3->addWidget( buttons_["N"] );
        row_3->addWidget( buttons_["M"] );
        row_3->addWidget( buttons_[SPACE_KEY] );
        row_3->addStretch();


        // ----- Results Flashkey Area ----- //
        QVBoxLayout *formLayout( new QVBoxLayout( resultsPanel_ ) );
        formLayout->setContentsMargins( QMargins() );
        formLayout->addLayout( logoLayout);
        formLayout->addLayout( resultsPanelLayout);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceSoftkeysWidget::toggleBestSellersButton()
{
    softkeyButton *b;

    if ( (b = searchByAZ_->findButton( 42 )) )
        searchByAZ_->setToggled( b );

    if ( (b = searchByCat_->findButton( 42 )) )
        searchByCat_->setToggled( b );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void selfServiceSoftkeysWidget::updateResultsString( int featureNum )
{
    if( characterSearch_ == NULL )
        return;

    if ( featureNum != hobart::softkeysWidget::SelfservCategoryResultsButtons )
        return;

    if ( scaleConfig_->charSearchSoftkeyCount == 0 )
    {
        if( characterSearch_->text().size() > 0)
            clear(false);
        else
            clear(true);
    }
    else if( scaleConfig_->charSearchSoftkeyCount == 1 )
        resultsTag_->setText(tr(" 1 result available "));
    else if( scaleConfig_->charSearchSoftkeyCount >= 18 )
    {
        // pjr383 - dont need this
        //if( scaleConfig_->scaleConfig.selfServeCombineSearchKeypadFlashkeys() )
        //    QString::number(scaleConfig_->charSearchSoftkeyCount) + tr(" results available ");
        //else
            resultsTag_->setText(tr(" 18 results available "));
    }
    else
        resultsTag_->setText( QString::number(scaleConfig_->charSearchSoftkeyCount) + tr(" results available "));
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
int selfServiceSoftkeysWidget::checkScreenSize()
{
    int screen_size = 10;

    QDesktopWidget *desktop = QApplication::desktop();

    int screen = desktop->primaryScreen();

    if (( 0 <= screen ) && ( screen < desktop->screenCount() ))
    {
        QRect g( desktop->screenGeometry( screen ) );

        QSize resolution_ = g.size();

        //QMessageBox::information(this, "Resolution", "Screen resolution is\n\nWidth: " + QString::number(resolution_.width()) + "\nHeight: " + QString::number(resolution_.height()), QMessageBox::Ok);

        if( resolution_.width() > 1034 && resolution_.height() > 610 )  // gave it 10 more but do I need to?
            screen_size = 14;
    }

    return screen_size;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace
