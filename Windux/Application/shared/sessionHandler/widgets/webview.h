/**
 * @file webview.h
 * Hobart web view widget.
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef WEBVIEW
#define WEBVIEW

#include <QWebView>

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Hobart web view widget.
class webView : public QWebView
{
    Q_OBJECT

    typedef webView _Myt;
    typedef QWebView _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     */
    explicit webView( QWidget* parent = 0 );

    /// Destructor.
    virtual ~webView();

protected:

    // ========================================================================
    // Methods
    // ========================================================================

    /// Create new web view window.
    /**
     * @param[in] type  web window type
     * @return  pointer to new window
     */
    virtual QWebView *createWindow( QWebPage::WebWindowType type );

private Q_SLOTS:

    /// Slot for url changed.
    void onUrlChanged( const QUrl &url );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

} // hobart

#endif // WEBVIEW

