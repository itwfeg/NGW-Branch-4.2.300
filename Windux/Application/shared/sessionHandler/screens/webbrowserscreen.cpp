/**
 * @file webbrowserscreen.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "appsscreen.h"
#include "buildamealscreen.h"
#include "meatspecsheetscreen.h"
#include "plumainscreen.h"
#include "webbrowserscreen.h"

#include "../sessionmainwindow.h"

#include "../modes/operatemode.h"

#include "../widgets/dynamiccontentwidget.h"
#include "../widgets/webViewer.h"

#include <backgroundwidgettypes.h>
#include <palette.h>
#include <softkeyswidget.h>

#include <Messages.h>
#include <ScaleConfiguration.h>
#include <UiUtilities.h>

#include <QHBoxLayout>
#include <QSettings>
#include <QVBoxLayout>
#include <QWebFrame>

const int WebBrowserScreen::dcaWidgets_ =
    hobart::dynamicContentWidget::Flashkeys |
    hobart::dynamicContentWidget::BestSellerList |
    hobart::dynamicContentWidget::WebBrowser |
    hobart::dynamicContentWidget::FixedFlashkeys;

const int WebBrowserScreen::dcaWidgetsExcluded_ = 0;

const int WebBrowserScreen::dcaSoftkeyGroup_ = hobart::softkeysWidget::ProductGrid;

static const int BUILD_A_MEAL_SCREEN = 1;
static const int CUT_TEST_ONE_SCREEN = 2;

///////////////////////////////////////////////////////////////////////////////////////////////////
WebBrowserScreen::WebBrowserScreen( SessionMainWindow *mainWindow, ModeWidget *parent ) :
    _Mybase( mainWindow, parent ),
    registered_( false )
{
    setObjectName( "WebBrowserScreen" );

    // set palette
    modifyPaletteBackground setBackground( Qt::transparent );
    setBackground( this );

    modifyPaletteForeground setForeground( modifyPaletteForeground::createPalette( hobart::colorPalette::black ) );
    setForeground( this );

    // hide weight bar
    OperateMode *operateMode = dynamic_cast<OperateMode*>( mode_ );

    if ( operateMode )
    {
        operateMode->hideWeightBar();

        connect( operateMode, SIGNAL( widgetSettingsLockChanged(bool) ), SLOT( onWidgetSettingsLockChanged(bool) ) );
    }



    // initialize
    initialize();

    translate();

    // setup operator fields
    setOperatorDisplayFields();

    // TODO
    // need to get configuration value as to whether URL can be entered
    if ( webView_ )
    {
        webView_->setUrlVisible(true, true);

        // connect slots
        connect( webView_, SIGNAL(webViewerLinkClicked(QString)), SLOT(onLinkClicked(QString)));
    }

    // connect slots
    connect( mode_, SIGNAL( screenChanged() ), SLOT( onScreenChanged() ) );

}

///////////////////////////////////////////////////////////////////////////////////////////////////
WebBrowserScreen::~WebBrowserScreen()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QStringList WebBrowserScreen::editableFeatures() const
{
    QStringList list;

    if (( hobart::dynamicContentWidget::Flashkeys == dca_->currentIndex() ) ||
        ( hobart::dynamicContentWidget::BestSellerList == dca_->currentIndex() ) ||
        ( hobart::dynamicContentWidget::FixedFlashkeys == dca_->currentIndex() ) )
    {
        list.append( QString::number( dcaSoftkeyGroup_ ) );
    }

    return list;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WebBrowserScreen::translate()
{
    // reload flashkeys (for new language)
    resetFlashkeys( true );

    if ( webView_ )
        webView_->translate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WebBrowserScreen::setHtml( const QString& htmlText )
{
    if ( webView_ )
        webView_->setHtml( htmlText );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WebBrowserScreen::loadTestPage()
{
    if ( webView_ )
        webView_->loadTestPage();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WebBrowserScreen::resetFlashkeys( bool force )
{
    // no sense requesting anything if we are not registered yet
    if ( !registered_ )
        return;

    dca_->resetFlashkeys( force );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WebBrowserScreen::load( const QString& url )
{
    // load the url
    if ( isVisible() )
    {
        if ( webView_ )
            webView_->setURL( url );
    }
    else
    {
        // when not visible cache the url until we are the active screen
        url_ = url;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ScreenWidget *WebBrowserScreen::swipeScreenDown() const
{
    OperateMode *operateMode( dynamic_cast<OperateMode*>( mode_ ) );

    // no apps installed
    if ( !AppsScreen::isExternalAppInstalled() )
        return NULL;

    // screen does not exist
    else if (( !operateMode ) || ( !operateMode->isCached<AppsScreen>() ))
        return NULL;

    return mode_->createScreen<AppsScreen>();
}

void WebBrowserScreen::runJavascript(QString& javascript)
{
    if (webView_)
    {
        webView_->webView()->page()->mainFrame()->evaluateJavaScript(javascript);
    };
};

QUrl WebBrowserScreen::url(void)
{
    if (webView_)
    {
        return webView_->webView()->url();
    };
    
    return QUrl("");
};

///////////////////////////////////////////////////////////////////////////////////////////////////
ScreenWidget *WebBrowserScreen::swipeScreenLeft() const
{
    ScreenWidget *screen = mode_->createScreen<PluMainScreen>();
	
	return screen;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ScreenWidget *WebBrowserScreen::swipeScreenUp() const
{
    OperateMode *operateMode( dynamic_cast<OperateMode*>( mode_ ) );
    ScreenWidget *screen( NULL );
    int bottomScreen( session_->scaleConfig->scaleConfig.bottomScreenType() );

    if ( bottomScreen == BUILD_A_MEAL_SCREEN )
    {
        // screen does not exist
        if ( !operateMode->isCached<MealBuilderScreen>() )
            return NULL;

        screen = mode_->createScreen<MealBuilderScreen>();
    }
    else if ( bottomScreen == CUT_TEST_ONE_SCREEN )
    {
        // screen does not exist
        if ( !operateMode->isCached<MeatSheetScreen>() )
            return NULL;

        screen = mode_->createScreen<MeatSheetScreen>();
    }

    return screen;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool WebBrowserScreen::onScreenChanging()
{
    // when leaving the screen reset the flashkeys to default page
    resetFlashkeys();

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WebBrowserScreen::onSessionMessage( const SessionMessage& msg )
{
    LOG_CLASS_INFO( "WebBrowserScreen", "onSessionMessage" );

    int device = msg[0].toInt();
    int event = msg[1].toInt();

    if(  session_->scaleConfig->internalConfig.ssactive_lock )
        return;

    // softkeys - get a single softkey
    if( ( SoftkeyDev == device ) && ( WriteSoftkey == event ) )
    {
        // toggle extra sell by day
        // PJR 420 - Flashkey Options to Toggle Extra Day
        // Sequence of Steps:
        // 1: CLICK Toggle Extra Day Flashkey
        // 2: Send Flashkey action to toggle day or remove it (not done yet)
        // 3: Send 136,40 request to get the default group
        // 4: Receive 136,42 reply with default group
        // 5: Send 136,20 to read the Flashkey clicked
        // 6: Receive 136,22 (this msg) with Flashkey information including secondary image
        // 7: Toggle/Flip the background image

        ScaleConfiguration *config = session_->scaleConfig;

        if( config->toggleExtraLeftPanelDayNow )
        {
            DomSoftkey m( msg );

            int featNum = config->softkeyFeature.featureNum();

            if( mainWindow_->isOperateMode() && ( QString::number( hobart::dynamicContentWidget::FixedFlashkeys ) != config->scaleConfig.dcaRunScreenTypeRaw() ) )
            {
                if ( hobart::softkeysWidget::ProductGrid == featNum )
                {
                    dca_->setPrimaryToggledFlashkey( featNum, &m );
                }
            }
        }

        config->toggleExtraLeftPanelDayNow = false;                  // any toggling that may have been happening with Add Extra Sell By, is done
    }

    // softkeys - get a softkey feature
    else if ( ( SoftkeyDev == device ) && ( WriteSoftkeyFeature == event ) )
    {
        ScaleConfiguration *config = session_->scaleConfig;
        DomSoftkeyFeature m( msg );

        if( mainWindow_->isOperateMode() && ( hobart::dynamicContentWidget::FixedFlashkeys != config->scaleConfig.dcaRunScreenType() ) )
        {
            int featNum = config->softkeyFeature.featureNum();

            if( hobart::softkeysWidget::ProductGrid == featNum )
            {
                // PJR 420 - Flashkey Options to Toggle Extra Day
                // Sequence of Step:
                // 1: CLICK Toggle Extra Day Flashkey
                // 2: Send Flashkey action to toggle day or remove it
                // 3: Send 136,40 request to get the default group
                // 4: Receive 136,42 (this msg) reply with default group
                // 5: Send 136,20 to read the Flashkey clicked
                // 6: Receive 136,22 msg with Flashkey information including secondary image
                // 7: Toggle/Flip the background image

                if( config->toggleExtraDayNow )
                {
                    session_->send(
                       SoftkeyDev,
                       ReadSoftkey,
                       QString::number( m.featureNum() ) + "," +
                       QString::number( m.defaultGroupNum() ) + "," +
                       QString::number( config->toggleExtraDayRowLeft ) + "," +
                       QString::number( config->toggleExtraDayColLeft ) );
                }
            }
        }
    }

    // softkeys - load entire softkey group
    else if (( SoftkeyDev == device ) && ( LoadSoftkeyGroup == event ))
    {
        ScaleConfiguration *config = session_->scaleConfig;
        if( config->internalConfig.ssactive_lock )
            return;

        DomLoadSoftkeyGroup m( msg );

        // set softkeys
        LOG_DEBUG( "loading flashkeys...", NULL );
        dca_->setFlashkeys( &m );
    }

    // scale configuration
    else if (( ConfigurationDev == device ) && ( CfgScaleWrite == event ))
    {
        MadpScaleConfig m( msg );

        QString this_url = m.defaultHomePage();
        QString defaultHomePage( m.defaultHomePage().trimmed() );

        // save off home url
        if ( homeUrl_ != defaultHomePage )
            load( homeUrl_ = defaultHomePage );

        // enable/disable address bar
        if ( webView_ )
            webView_->setUrlVisible( true, m.allowAddressBar() ); // FT-1451


        if ( webView_ && hobart::dynamicContentWidget::WebBrowser != (hobart::dynamicContentWidget::WidgetType) m.dcaStartScreenType() )
            webView_->setURL( EMPTY_URL );
    }

    // scale configuration
    else if (( ConfigurationDev == device ) && ( CfgPrepackWrite == event ))
    {
        setOperatorDisplayFields();
    }

    // register session
    else if (( SessionDev == device ) && ( SnRegister == event ))
    {
        LOG_INFO( "session has been registered!", NULL );
        registered_ = true;

        // request softkeys
        resetFlashkeys( true );
    }

    // database restore
    else if (( BackupRestoreDev == device ) && ( Terminate  == event ))
    {
        // FT-1115 request configuration after db restore
        resetFlashkeys( true );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WebBrowserScreen::onScreenChanged()
{
    // only execute code when we are active screen
    if ( mode_->screen() != this )
        return;

    // check for cached page
    MadpOperatorDisplayFields *config = &session_->scaleConfig->operatorDisplayFields;
    hobart::dynamicContentWidget::WidgetType index = hobart::dynamicContentWidget::Empty;
    if (config)
        // start screen dca type
        index = (hobart::dynamicContentWidget::WidgetType) config->dcaStartScreenType();

    if (( hobart::dynamicContentWidget::WebBrowser == index ) && ( isVisible() ) && ( url_.length() ))
    {
        // repaint screen (do not update(), need repaint() instead)
        repaint();

        // navigate to url
        if ( webView_ )
            webView_->setURL( url_ );

        url_.clear();
    }

    OperateMode *operateMode = dynamic_cast<OperateMode*>( mode_ );

    // make sure weight bar is hidden
    if ( operateMode )
    {
        operateMode->hideWeightBar();

        // FT-1311 force resize to ensure page uses entire screen
        if ( webView_ )
            webView_->adjustSize();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WebBrowserScreen::onLinkClicked( const QString& url )
{
    if ( webView_ )
        webView_->setURL( url );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WebBrowserScreen::onPdfLinkClicked( const QString& filename )
{
    // display PDF
    mainWindow_->showDocument( filename );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WebBrowserScreen::onPdfLinkClickedLoadFromMemory( const QByteArray& pdfData )
{
    // display PDF
    mainWindow_->showDocument( pdfData );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WebBrowserScreen::onVideoLinkClicked( const QString& filename )
{
    // show the video
    mainWindow_->showVideo( filename );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WebBrowserScreen::onWidgetSettingsLockChanged( bool locked )
{
    // locking controls
    if ( locked )
    {
        dca_->hideContentSelectors();

        // save!
        writeConfiguration();
    }

    // unlocking controls
    else
    {
        // hide input widgets
        mainWindow_->hideInputWidgets();

        dca_->showContentSelectors();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WebBrowserScreen::initialize()
{
    // this margin applies to flashkeys (start screen and best seller) on the left screen... it was
    // set by marketing on 8/7/2013 when they decided they did not like the softkeys being full
    // screen and some of the text being cut off
    static const QMargins softkeyMargins( 40, 25, 40, 25 );

    OperateMode *operateMode = dynamic_cast<OperateMode*>( mode_ );

    QVBoxLayout *screenLayout = new QVBoxLayout( this );
    screenLayout->setContentsMargins( QMargins() );

    // create dynamic content area
    int dcaWidgets( dcaWidgets_ );
    int dcaWidgetsExcluded( dcaWidgetsExcluded_ );

    // Randy - hook for NEW DELI VISION license
    // SCALE_ENHANCED_CENTERSCREEN

    // HTs scale
    // this feature is disabled
    if ( !session_->uiUtilities->isFeatureLicensed( SCALE_WEB_BROWSER ) )
        dcaWidgets &= ~hobart::dynamicContentWidget::WebBrowser;
    else if ( !session_->uiUtilities->isFeatureLicensed( SCALE_FULLSCREEN_FLASHKEYS ) )
    {
        dcaWidgets &= ~hobart::dynamicContentWidget::Flashkeys;
        dcaWidgets &= ~hobart::dynamicContentWidget::BestSellerList;
        dcaWidgets &= ~hobart::dynamicContentWidget::FixedFlashkeys;
    }

    dca_ = new hobart::dynamicContentWidget( (hobart::dynamicContentWidget::WidgetType) dcaWidgets, this );
    dca_->setContentsMargins( QMargins( 0, 2, 0, 2 ) );
    dca_->setExcludedContent( (hobart::dynamicContentWidget::WidgetType) dcaWidgetsExcluded );
    dca_->setFlashkeysFeature( dcaSoftkeyGroup_ );
    dca_->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    dca_->setObjectName( "dynamicContent" );

    connect( dca_, SIGNAL( flashkeyClicked(QWidget*,QWidget*,int,QString,int) ), operateMode, SLOT( handleFlashkey(QWidget*,QWidget*,int,QString,int) ) );

    screenLayout->addWidget( dca_ );

    // set best seller list margins
    QWidget *bestSellerList( dca_->widget( hobart::dynamicContentWidget::BestSellerList ) );

    if ( bestSellerList )
        bestSellerList->setContentsMargins( softkeyMargins );

    // set flashkeys margins
    QWidget *flashkeys( dca_->widget( hobart::dynamicContentWidget::Flashkeys ) );

    if ( flashkeys )
        flashkeys->setContentsMargins( softkeyMargins );

    // set fixed flashkeys margins
    QWidget *fixedflashkeys( dca_->widget( hobart::dynamicContentWidget::FixedFlashkeys ) );

    if ( fixedflashkeys )
        fixedflashkeys->setContentsMargins( softkeyMargins );

    // retrieve web view
    if ( (webView_ = dynamic_cast<hobart::webViewer*>( dca_->widget( hobart::dynamicContentWidget::WebBrowser ) )) )
    {
        webView_->setToolBarVisible( true );
        webView_->setDisplayAsDialog( false );

        connect( webView_, SIGNAL( webViewerPdfLinkClicked(QString) ), SLOT( onPdfLinkClicked(QString) ) );
        connect( webView_, SIGNAL( webViewerVideoLinkClicked(QString) ), SLOT( onVideoLinkClicked(QString) ) );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WebBrowserScreen::setOperatorDisplayFields()
{
    LOG_CLASS_INFO( "WebBrowserScreen", "setOperatorDisplayFields" );

    MadpOperatorDisplayFields *config = &session_->scaleConfig->operatorDisplayFields;
    hobart::dynamicContentWidget::WidgetType index;

    // start screen dca type
    index = (hobart::dynamicContentWidget::WidgetType) config->dcaStartScreenType();

    // do not restore if excluded
    if ( dcaWidgetsExcluded_ & index )
        LOG_DEBUG( "not restoring dca excluded", NULL );
    else
    {
        dca_->setCurrentIndex( index );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WebBrowserScreen::writeConfiguration()
{
    LOG_CLASS_INFO( "WebBrowserScreen", "writeConfiguration" );

    MadpOperatorDisplayFields *operatorDisplayFields = &session_->scaleConfig->operatorDisplayFields;

    // start screen dca type
    // do not save if excluded
    if ( dcaWidgetsExcluded_ & dca_->currentIndex() )
        LOG_DEBUG( "not saving dca excluded", NULL );
    else
    {
        operatorDisplayFields->setDcaStartScreenType( dca_->currentIndex() );
    }

    LOG_DEBUG( "sending screen configuration...", NULL );
    session_->send( *operatorDisplayFields );
    session_->send( ConfigurationDev, CfgPrepackSave );
}

