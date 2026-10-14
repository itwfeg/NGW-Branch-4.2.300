/**
 * @file webview.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "webpage.h"
#include "webview.h"

#include <QMouseEvent>
#include <QTimer>
#include <QWebElement>
#include <QWebFrame>
#include <QWebHitTestResult>
#include <QWebPage>

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////
webView::webView( bool allowCookies, QWidget *parent ) :
    _Mybase( parent ),
    allowCookies_( allowCookies ),
    allowLocalContent_( false ),
    allowPlugins_( true ),
    allowPopups_( false )
{
    setObjectName( "hobartWebView" );

    // init
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
webView::~webView()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webView::mousePressEvent( QMouseEvent *e )
{
    QPoint p( e->pos() );
    QWebFrame *frame( page()->frameAt( p ) );

    if ( frame )
    {
        p -= frame->geometry().topLeft();

        qDebug( "checking for web hit at %d,%d", p.x(), p.y() );

        QWebHitTestResult result( frame->hitTestContent( p ) );

        // check for editable
        if ( result.isContentEditable() )
        {
            qDebug( "content is editable, popping input device" );
            Q_EMIT showInputDevice( this );
        }
        else
        {
            Q_EMIT hideInputDevice();
        }
    }

    return _Mybase::mousePressEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QWebView *webView::createWindow( QWebPage::WebWindowType type )
{
    const QString currentUrl( url().toString() );

    qDebug( "page '%s' requests new window of type %d...!", qPrintable( currentUrl ), type );

    // check popups allowed
    if ( allowPopups_ )
    {
/*
    sessionInfo info( this );

    // retrieve pointer to session
    SessionMainWindow *mainWindow( info() );

    if ( !mainWindow )
    {
        LOG_WARN( "could not retrieve main window!", NULL );
        return this;
    }

    // create popup
    LOG_TRACE( "creating popup...", NULL );

    WebBrowserDialog *dialog( new WebBrowserDialog( mainWindow, true ) );
    dialog->setUrlVisible( true, false );
    dialog->open();

    // minimize browser window
    LOG_TRACE( "minimize browser...", NULL );

    mainWindow->minimizeHelpDialog();

    LOG_TRACE( "done", NULL );
    return dialog->webView();
*/
    }

    return _Mybase::createWindow( type );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webView::onLinkClicked( const QUrl &url )
{
    qDebug( "link clicked: '%s'", qPrintable( url.toString() ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webView::onLoadFinished( bool ok )
{
    if ( ok )
        qDebug( "end of page loading" );
    else
        qWarning( "end of page loading, have errors!" );

    // stop timer
    pageLoadTimer_->stop();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webView::onLoadProgress( int progress )
{
    qDebug( "progress: %d", progress );

    // reset timer (since we are making progress)
    if ( pageLoadTimerProgress_ < progress )
    {
        pageLoadTimer_->setSingleShot( true );
        pageLoadTimer_->start( PAGE_LOAD_TIMEOUT );

        pageLoadTimerProgress_ = progress;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webView::onLoadStarted()
{
    qDebug( "start of page loading..." );

    // start timer
    pageLoadTimer_->setSingleShot( true );
    pageLoadTimer_->start( PAGE_LOAD_TIMEOUT );

    pageLoadTimerProgress_ = 0;

    Q_EMIT hideInputDevice();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webView::onLoadTimeout()
{
    qWarning( "page did not load in a timely manner!" );
    stop();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webView::onUrlChanged( const QUrl &url )
{
    qDebug( "url changing to: '%s'", qPrintable( url.toString() ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webView::initialize()
{
    setAcceptDrops( false );
    setContextMenuPolicy( Qt::NoContextMenu );
    setFocusPolicy( Qt::ClickFocus );
    setPage( new webPage( allowCookies_, this ) );

    settings()->setAttribute( QWebSettings::PluginsEnabled, allowPlugins_ );
    settings()->setAttribute( QWebSettings::LocalContentCanAccessFileUrls, allowLocalContent_ );

    settings()->setAttribute( QWebSettings::JavascriptEnabled, true );
    settings()->setAttribute( QWebSettings::JavascriptCanOpenWindows, allowPopups_ );
    settings()->setAttribute( QWebSettings::JavaEnabled, true );

    settings()->setAttribute( QWebSettings::DeveloperExtrasEnabled, true );
    settings()->setAttribute( QWebSettings::AutoLoadImages, true );

    connect( this, SIGNAL(linkClicked(QUrl)), SLOT(onLinkClicked(QUrl)) );
    connect( this, SIGNAL(loadFinished(bool)), SLOT(onLoadFinished(bool)) );
    connect( this, SIGNAL(loadProgress(int)), SLOT(onLoadProgress(int)) );
    connect( this, SIGNAL(loadStarted()), SLOT(onLoadStarted()) );
    connect( this, SIGNAL(urlChanged(QUrl)), SLOT(onUrlChanged(QUrl)) );

    pageLoadTimer_ = new QTimer( this );
    connect( pageLoadTimer_, SIGNAL(timeout()), SLOT(onLoadTimeout()) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webView::injectJavascript()
{
    QWebFrame *mainFrame( page()->mainFrame() );

    if ( mainFrame )
    {
        static const QString focusHandler(
            "\n"
            "function isTextElement(el) {\n"
            "  return el.tagName == \"INPUT\" && ((el.type == \"text\") || (el.type == \"password\"));\n"
            "}\n"
            "this.addEventListener(\"DOMFocusIn\", function(e) {\n"
            "  if (isTextElement(e.target)) {\n"
            "    BrowserView.handleFocusIn(e.target.name);\n"
            "  }\n"
            "}, false);\n"
            "this.addEventListener(\"DOMFocusOut\", function(e) {\n"
            "  if (isTextElement(e.target)) {\n"
            "    BrowserView.handleFocusOut(e.target.name);\n"
            "  }\n"
            "}, false)\n" );

        QWebElement document( mainFrame->documentElement() );

        // inject!
        document.evaluateJavaScript( focusHandler );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart

