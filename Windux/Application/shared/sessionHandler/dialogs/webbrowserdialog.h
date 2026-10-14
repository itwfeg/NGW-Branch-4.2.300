/**
 * @file webbrowserdialog.h
 * Web browser dialog.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef WEBBROWSERDIALOG_H
#define WEBBROWSERDIALOG_H

#include "../sessiondialog.h"

class QWebView;

namespace hobart {
class stackedWebViewer;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Web browser dialog.
class WebBrowserDialog : public SessionDialog
{
    Q_OBJECT

    typedef WebBrowserDialog _Myt;
    typedef SessionDialog _Mybase;
    
public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     * @param[in] isPopup  @c true if popup dialog, @c false otherwise
     */
    explicit WebBrowserDialog( QWidget *parent = 0, bool isPopup = false );

    /// Destructor.
    virtual ~WebBrowserDialog();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Whether or not the dialog may be minimized.
    /**
     * @note
     * Currently this functionality is only available and supported for the help/video dialog!
     *
     * @return  @c true if allowed, @c false otherwise
     */
    virtual bool allowMinimize() const {return !popup_;}

    /// Set browser name.
    /**
     * @param[in] value name
     */
    virtual void setBrowserName( const QString& value );

    /// Set browser HTML.
    /**
     * @param[in] value HTML
     */
    virtual void setHtml( const QString& value );

    /// Set browser URL.
    /**
     * @param[in] value url
     */
    virtual void setURL( const QString& value );

    /// Set URL visibility.
    /**
     * @param[in] yes should be visible
     * @param[in] url field is editable
     */
    virtual void setUrlVisible( bool yes, bool editable );

    /// Retrieve size hint.
    /**
     * @return  size hint
     */
    virtual QSize sizeHint() const;

    /// Retrieve web view widget.
    /**
     * @return  web view
     */
    virtual QWebView *webView() const;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Play a video.
    /**
     * @param[in] filename  filename of video to play
     * @return  @c true if video starts playing, @c false otherwise
     */
    virtual bool playVideo( const QString& filename );

    /// Show an HTML document.
    /**
     * @param[in] url  document link
     * @return  @c true if displayed, @c false otherwise
     */
    virtual bool showHtml( const QString& url );

    /// Show document.
    /**
     * @param[in] filename  filename of document
     * @return  @c true if document shown, @c false otherwise
     */
    virtual bool showDocument( const QString& filename );

    /// Show document.
    /**
     * @param[in] pdf  byte array of pdf contents to display
     * @return  @c true if document shown, @c false otherwise
     */
    virtual bool showDocument( const QByteArray& pdfData );

    /// Translate the widget fields.
    virtual void translate();

protected:

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve renderer appearance.
    /**
     * @return  appearance object
     */
    virtual hobart::widgetRendererAppearanceBase *rendererAppearance() const;

    /// Retrieve renderer style.
    /**
     * @return  style
     */
    virtual int rendererStyle() const;

private Q_SLOTS:

    /// Slot for dialog active.
    void onActive( bool value );

private:

    hobart::stackedWebViewer *webViewer_;

    QString documentFilename_;
    QString htmlFilename_;
    QString mediaFilename_;
    QByteArray documentMemoryContents_;

    bool popup_;

    /// Initialize.
    void initialize();

    /// Play a video.
    bool playVideoInternal( const QString& filename );

    /// Show a document.
    bool showDocumentInternal( const QString& filename );

    /// Show a document based on contents of document in byte array
    bool showDocumentInternal( const QByteArray& pdfData );

    /// Show an HTML document
    bool showHtmlInternal( const QString& url );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // WEBBROWSERDIALOG_H
