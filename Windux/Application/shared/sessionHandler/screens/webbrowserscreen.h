/**
 * @file webbrowserscreen.h
 * Screen with a web browser in it.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef WEBBROWSERSCREEN_H
#define WEBBROWSERSCREEN_H

#include "screenwidget.h"

namespace hobart {
class dynamicContentWidget;
class webViewer;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

class WebBrowserScreen : public ScreenWidget
{
    Q_OBJECT

    typedef WebBrowserScreen _Myt;
    typedef ScreenWidget _Mybase;

public:

    ///////////////////////////////////////////////////////////////////////////
    // CTOR / DTOR

    /**
     * Constructor.
     *
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
     */
    explicit WebBrowserScreen( SessionMainWindow *mainWindow, ModeWidget *parent = 0 );

    /**
     * Destructor.
     */
    virtual ~WebBrowserScreen();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Retrieve list of visible and editable features.
     *
     * @returns  list of features
     */
    virtual QStringList editableFeatures() const;

    ///////////////////////////////////////////////////////////////////////////
    // Methods

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

    /**
     * Reset flashkeys to default (initial) group.
     *
     * @param[in] force  @c true to force reset all flashkey banks, @c false otherwise
     */
    virtual void resetFlashkeys( bool force = false );

    /**
     * Set the HTML of the browser screen to HTML.
     *
     * @param[in] html  HTML syntax to display
     */
    virtual void setHtml( const QString& html );

    /**
     * Loads a browser test page
     */
    virtual void loadTestPage();
    
    virtual void runJavascript(QString& javascript);
    
    virtual QUrl url(void);

public Q_SLOTS:

    /**
     * Loads the specified url and displays it.
     *
     * @param[in] url  url to display
     */
    virtual void load( const QString& url );

protected:

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Screen that gets displayed when swiping (gesturing) the screen down.
     *
     * @returns  next screen in sequence, @c NULL if none
     */
    virtual ScreenWidget *swipeScreenDown() const;

    /**
     * Screen that gets displayed when swiping (gesturing) the screen left.
     *
     * @returns  next screen in sequence, @c NULL if none
     */
    virtual ScreenWidget *swipeScreenLeft() const;

    /**
     * Screen that gets displayed when swiping (gesturing) the screen up.
     *
     * @returns  next screen in sequence, @c NULL if none
     */
    virtual ScreenWidget *swipeScreenUp() const;

    ///////////////////////////////////////////////////////////////////////////
    // Methods

    /**
     * Handle screen changing.
     *
     * This method is called before the screen is changed to something new. If for some reason the
     * screen should not be unloaded and destroyed then a value of @c false should be returned.
     *
     * @returns  @c true to unload screen, @c false to keep screen resident
     */
    virtual bool onScreenChanging();

    /**
     * Handle a session message.
     *
     * @param[in] msg  session message
     */
    virtual void onSessionMessage( const SessionMessage& msg );

private Q_SLOTS:

    /**
     * Slot for link clicked.
     */
    void onLinkClicked( const QString& url );

    /**
     * Slot for pdf link clicked.
     */
    void onPdfLinkClicked( const QString& filename );

    /**
     * Slot for pdf link clicked.
     */
    void onPdfLinkClickedLoadFromMemory( const QByteArray& pdfData );

    /**
     * Slot for when screen changes.
     */
    void onScreenChanged();

    /**
     * Slot for video link clicked.
     */
    void onVideoLinkClicked( const QString& filename );

    /**
     * Slot for lock/unlock of widget settings.
     */
    void onWidgetSettingsLockChanged( bool locked );

private:

    hobart::dynamicContentWidget *dca_;
    hobart::webViewer *webView_;

    QString homeUrl_;
    QString url_;

    bool registered_;

    static const int dcaWidgets_;
    static const int dcaWidgetsExcluded_;
    static const int dcaSoftkeyGroup_;

    /**
     * Initialize.
     */
    void initialize();

    /**
     * Show/hide operator fields from configuration.
     */
    void setOperatorDisplayFields();

    /**
     * Write screen configuration data.
     */
    void writeConfiguration();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // WEBBROWSERSCREEN_H
