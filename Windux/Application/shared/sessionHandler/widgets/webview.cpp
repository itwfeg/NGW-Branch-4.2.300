/**
 * @file webview.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "webpage.h"
#include "webview.h"

#include "../dialogs/webbrowserdialog.h"

#include "../sessioninfo.h"
#include "../sessionmainwindow.h"

#include <logmanager.h>

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////
webView::webView( QWidget *parent ) :
    _Mybase( parent )
{
    setAcceptDrops( false );
    setObjectName( "hobartWebView" );
    setPage( new webPage( this ) );

    connect( this, SIGNAL(urlChanged(QUrl)), SLOT(onUrlChanged(QUrl)) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
webView::~webView()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QWebView *webView::createWindow( QWebPage::WebWindowType type )
{
    LOG_CLASS_INFO( "webView", "createWindow" );

    sessionInfo info( this );

    // retrieve pointer to session
    SessionMainWindow *mainWindow( info() );

    const QString currentUrl( url().toString() );
    LOG_INFO( "page '%s' requests new window of type %d...!", qPrintable( currentUrl ), type );

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
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webView::onUrlChanged( const QUrl &url )
{
    LOG_CLASS_INFO( "webView", "onUrlChanged" );

    LOG_TRACE( "url changing to: '%s'", qPrintable( url.toString() ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart

