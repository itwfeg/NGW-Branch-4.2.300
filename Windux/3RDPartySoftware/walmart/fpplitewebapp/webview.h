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

class QTimer;

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
     * @param[in] allowCookies  @c true to allow cookies, @c false otherwise
     * @param[in] parent  parent object
     */
    explicit webView( bool allowCookies = false, QWidget *parent = 0 );

    /// Destructor.
    virtual ~webView();

Q_SIGNALS:

    /// Emitted when input device needs hidden.
    void hideInputDevice();

    /// Emitted when input device needs shown.
    /**
     * @param[in] widget  focus widget
     */
    void showInputDevice( QWidget *widget );

protected:

    virtual void mousePressEvent(QMouseEvent *e);

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

    /// Slot for link clicked
    void onLinkClicked( const QUrl &url );

    /// Slot for load finished.
    void onLoadFinished( bool ok );

    /// Slot for load progress.
    void onLoadProgress( int progress );

    /// Slot for load started.
    void onLoadStarted();

    /// Slot for load timeout.
    void onLoadTimeout();

    /// Slot for url changed.
    void onUrlChanged( const QUrl &url );

private:

    enum {PAGE_LOAD_TIMEOUT = 45000};

    int pageLoadTimerProgress_;
    QTimer *pageLoadTimer_;

    bool allowCookies_;
    bool allowLocalContent_;
    bool allowPlugins_;
    bool allowPopups_;

    /// Initialize.
    void initialize();

    /// Inject javascript into web document.
    void injectJavascript();

};

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart

#endif // WEBVIEW

