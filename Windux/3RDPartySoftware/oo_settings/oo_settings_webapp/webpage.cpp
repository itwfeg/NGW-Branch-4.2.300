/**
 * @file webpage.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "webpage.h"
#include "webviewcookiejar.h"
#include "webviewnetwork.h"

#include <QNetworkRequest>
#include <QWebView>

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////
webPage::webPage( bool allowCookies, QObject* parent ) :
    _Mybase( parent )
{
    setObjectName( "hobartWebPage" );

    // install network manager
    webViewNetworkAccessManager *net( new webViewNetworkAccessManager( this ) );

    if ( allowCookies )
        net->setCookieJar( new webViewCookieJar( this ) );

    setNetworkAccessManager( net );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
webPage::~webPage()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool webPage::acceptNavigationRequest( QWebFrame *frame, const QNetworkRequest &request, NavigationType type )
{
    qDebug( "frame 0x%08x type %d url '%s'", (qintptr) frame, type, qPrintable( request.url().toString() ) );

    if ( !frame )
    {
        QWebView *webView( dynamic_cast<QWebView*>( view() ) );

        qDebug( "navigation to new window not supported, redirecting!" );

        if ( !webView)
            qWarning( "invalid view" );
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

