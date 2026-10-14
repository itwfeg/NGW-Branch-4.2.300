/**
 * @file webpage.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "webpage.h"

#include <logmanager.h>

#include <QNetworkRequest>
#include <QWebView>

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////
webPage::webPage( QObject *parent ) :
    _Mybase( parent )
{
    setObjectName( "hobartWebPage" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
webPage::~webPage()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool webPage::acceptNavigationRequest( QWebFrame *frame, const QNetworkRequest &request, NavigationType type )
{
    LOG_CLASS_INFO( "webPage", "acceptNavigationRequest" );

    LOG_INFO( "frame 0x%08x type %d url '%s'", frame, type, qPrintable( request.url().toString() ) );

    if ( !frame )
    {
        QWebView *webView( dynamic_cast<QWebView*>( view() ) );

        LOG_INFO( "navigation to new window not supported, redirecting!", NULL );

        if ( !webView)
            LOG_WARN( "invalid view", NULL );
        else
            webView->setUrl( request.url() );

        return false;
    }

    if ( NavigationTypeOther == type )
        return true;

    return _Mybase::acceptNavigationRequest( frame, request, type );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart

