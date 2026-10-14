/**
 * @file stackedwebviewer.h
 * Widget with a web viewer and video player.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef STACKEDWEBVIEWER_H
#define STACKEDWEBVIEWER_H

#include <QStackedWidget>

class QLabel;
class QPushButton;
class QWebView;
class videoPlayerWidget;

namespace hobart {
class pdfviewer;
class webViewer;
}

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class stackedWebViewer
 * @brief Widget with a web viewer and video player.
 */
class stackedWebViewer : public QStackedWidget
{
    Q_OBJECT

    typedef stackedWebViewer _Myt;
    typedef QStackedWidget _Mybase;

Q_SIGNALS:

    /**
     * Signal for finished.
     */
    void finished();

public:

    ///////////////////////////////////////////////////////////////////////////
    // CTOR / DTOR

    /**
     * Constructor.
     *
     * @param[in] parent  parent widget
     */
    explicit stackedWebViewer(
        QWidget *parent = 0 );

    /**
     * Destructor.
     */
    virtual ~stackedWebViewer();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Check if player active.
     *
     * @returns  @c true if player is active, @c false otherwise
     */
    virtual bool isVideoPlayerActive() const;

    /**
     * Retrieve web view widget.
     *
     * @returns  web view
     */
    virtual QWebView *webView() const;

    ///////////////////////////////////////////////////////////////////////////
    // Methods

    /**
     * Pause the video.
     */
    virtual void pauseVideo();

    /**
     * Play a video.
     *
     * @param[in] filename filename of video to play
     * @returns  @c true if video starts playing, @c false otherwise
     */
    virtual bool playVideo( const QString& filename );

    /**
     * View a PDF file.
     *
     * @param[in] filename filename of PDF
     */
    virtual void viewPdf( const QString& filename );

    /**
     * View a PDF memory block
     *
     * @param[in] pdfData data in memory
     */
    virtual void viewPdf( QByteArray pdfData );

    /**
      * Sets URL in web browser to that specified
      * @param[in] url link to display
      */
    void setURL(QString url);

    /**
     * Translate the widget fields.
     */
    virtual void translate();

    /**
     * Set URL visibility.
     *
     * @param[in] yes whether url is visible
     * @param[in] editable whether url can be edited
     */
    virtual void setUrlVisible( bool yes, bool editable );

private Q_SLOTS:

    /**
     * Slot for when stacked widget page changes.
     */
    void onCurrentChanged( int index );

    /**
     * Slot for exiting error page.
     */
    void onErrorPageExitButtonClicked();

    /**
     * Slot for when video player finishes.
     */
    void onVideoPlayerFinished( bool exitPressed );

    /**
     * Slot for when PDF viewing is done.
     */
    void onPdfViewingFinished();

    /**
     * Slot for playback error.
     */
    void onVideoPlayerPlaybackError( const QString& desc );

    /**
     * Slot for playing a video from a web link.
     */
    void onWebPlayVideo( const QString& filename );

    /**
     * Slot for viewing a PDF from a web link.
     */
    void onWebViewPdf( const QString& filename );

    /**
     * Slot for viewing a PDF from memory.
     */
    void onWebViewPdf( QByteArray pdfData );

    /**
     * Slot for playing a video from a web link.
     */
    void onWebDone();

private:

    enum Page
    {
        WebViewerPage       = 0,
        VideoPlayerPage     = 1,
        PlaybackErrorPage   = 2,
        PdfViewerPage       = 3
    };

    pdfviewer *pdfViewer_;
    webViewer *webViewer_;
    videoPlayerWidget *videoPlayer_;

    QWidget *errorPage_;
    QLabel *errorPageTitle_;
    QLabel *errorPageDesc_;
    QPushButton *errorPageExitButton_;

    bool cameFromWebViewer_;

    /**
     * Initialize.
     */
    void initialize();

    /**
     * Hide input widgets.
     */
    void hideInputWidgets() const;

    /**
     * Allocate error page widget.
     */
    QWidget *allocErrorPage();

};

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

/** @} */

#endif // STACKEDWEBVIEWER_H
