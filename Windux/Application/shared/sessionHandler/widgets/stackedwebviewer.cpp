/**
 * @file stackedwebviewer.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "pdfviewer.h"
#include "stackedwebviewer.h"
#include "webViewer.h"

#include "../sessioninfo.h"

#include <backgroundwidgettypes.h>
#include <font.h>
#include <palette.h>
#include <logmanager.h>
#include <videoplayerwidget.h>

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>
#include <QVBoxLayout>

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////
stackedWebViewer::stackedWebViewer( QWidget *parent ) :
    _Mybase( parent ),
    cameFromWebViewer_( false )
{
    setObjectName( "hobartStackedWebViewer" );

    // initialize
    initialize();
    translate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
stackedWebViewer::~stackedWebViewer()
{
    LOG_CLASS_INFO( "stackedWebViewer", "~stackedWebViewer" );

    LOG_DEBUG( "done", NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool stackedWebViewer::isVideoPlayerActive() const
{
    LOG_CLASS_INFO( "stackedWebViewer", "isVideoPlayerActive" );
    LOG_WARN("got here", NULL);
    if ( videoPlayer_ )
        return videoPlayer_->active();

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QWebView *stackedWebViewer::webView() const
{
    LOG_CLASS_INFO( "stackedWebViewer", "webView" );
    LOG_WARN("got here", NULL);
    if ( !webViewer_ )
        return NULL;

    return webViewer_->webView();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void stackedWebViewer::pauseVideo()
{
    LOG_CLASS_INFO( "stackedWebViewer", "pauseVideo" );
    LOG_WARN("got here", NULL);
    if ( videoPlayer_ )
        videoPlayer_->pause();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void stackedWebViewer::viewPdf( const QString& filename )
{
    // set focus to pdf control
    setCurrentIndex( PdfViewerPage );

    // load pdf file and display
    pdfViewer_->loadPdf(filename);

    // TODO what if load fails
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void stackedWebViewer::viewPdf( QByteArray pdfData )
{
    // set focus to pdf control
    setCurrentIndex( PdfViewerPage );

    // load pdf file and display
    pdfViewer_->loadPdf(pdfData);

    // TODO what if load fails
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool stackedWebViewer::playVideo( const QString& filename )
{
    if ( !videoPlayer_ )
        return false;

    // set focus to video control
    setCurrentIndex( VideoPlayerPage );
    repaint();

    // play!
    return videoPlayer_->play( filename );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void stackedWebViewer::translate()
{
    webViewer_->translate();
    videoPlayer_->translate();
    pdfViewer_->translate();

    errorPageTitle_->setText( tr( "Playback Error" ) );
    errorPageExitButton_->setText( tr( "E&xit" ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void stackedWebViewer::onCurrentChanged( int index )
{
    // make sure input widgets are hidden
    if ( VideoPlayerPage == index )
        hideInputWidgets();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void stackedWebViewer::onErrorPageExitButtonClicked()
{
    onVideoPlayerFinished( true );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void stackedWebViewer::onVideoPlayerFinished( bool exitPressed )
{
    LOG_CLASS_INFO( "stackedWebViewer", "onVideoPlayerFinished" );

    Q_UNUSED( exitPressed );

    if ( cameFromWebViewer_ )
    {
        // set focus back to web control
        setCurrentIndex( WebViewerPage );

        cameFromWebViewer_ = false;
        return;
    }

    LOG_DEBUG( "finished", NULL );

    // propigate signal
    Q_EMIT finished();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void stackedWebViewer::onPdfViewingFinished()
{
    LOG_CLASS_INFO( "stackedWebViewer", "onPdfViewingFinished" );

    if ( cameFromWebViewer_ )
    {
        // set focus back to web control
        setCurrentIndex( WebViewerPage );

        cameFromWebViewer_ = false;
        return;
    }

    LOG_DEBUG( "finished", NULL );

    // propigate signal
    Q_EMIT finished();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void stackedWebViewer::onVideoPlayerPlaybackError( const QString &desc )
{
    // set description
    errorPageDesc_->setText( desc );

    // set focus back to web control
    setCurrentIndex( PlaybackErrorPage );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void stackedWebViewer::onWebPlayVideo( const QString& filename )
{
    cameFromWebViewer_ = true;
    playVideo( filename );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void stackedWebViewer::onWebViewPdf( const QString& filename )
{
    cameFromWebViewer_ = true;
    viewPdf(filename);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void stackedWebViewer::onWebViewPdf( QByteArray pdfData )
{
    cameFromWebViewer_ = true;
    viewPdf(pdfData);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void stackedWebViewer::onWebDone()
{
    LOG_CLASS_INFO( "stackedWebViewer", "onWebDone" );

    LOG_DEBUG( "finished", NULL );
    Q_EMIT finished();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void stackedWebViewer::initialize()
{
      LOG_CLASS_INFO( "stackedWebViewer", "initialize" );

    LOG_DEBUG( "finished", NULL );

    sessionInfo info( this );

    connect( this, SIGNAL( currentChanged(int) ), SLOT( onCurrentChanged(int) ) );

    // retrieve pointer to session
    SessionMainWindow *mainWindow = info();

    // setup web viewer
    webViewer_ = new webViewer( this, true, true, mainWindow);
    webViewer_->setObjectName( "webViewer" );
    addWidget( webViewer_ );

    connect( webViewer_, SIGNAL( webViewerDoneClicked() ), SLOT( onWebDone() ));
    connect( webViewer_, SIGNAL( webViewerVideoLinkClicked(QString) ), SLOT( onWebPlayVideo(QString) ) );
    connect( webViewer_, SIGNAL( webViewerPdfLinkClicked(QString) ), SLOT( onWebViewPdf(QString) ) );

    // setup video player
    videoPlayer_ = new videoPlayerWidget( videoPlayerDevice::Operator, this );
    videoPlayer_->setObjectName( "videoPlayer" );
    addWidget( videoPlayer_ );

    // set palette
    foreach ( QObject *child, videoPlayer_->children() )
    {
        modifyPaletteForeground setForeground( modifyPaletteForeground::createPalette( hobart::colorPalette::white ) );

        QLabel *label = NULL;

        if ( (label = dynamic_cast<QLabel*>( child )) )
            setForeground( label );
    }

    connect( videoPlayer_, SIGNAL( finished(bool) ), SLOT( onVideoPlayerFinished(bool) ) );
    connect( videoPlayer_, SIGNAL( playbackError(QString) ), SLOT( onVideoPlayerPlaybackError(QString) ) );

    // setup video player error page
    errorPage_ = allocErrorPage();
    addWidget( errorPage_ );

    connect( errorPageExitButton_, SIGNAL( clicked() ), SLOT( onErrorPageExitButtonClicked() ) );

    pdfViewer_ = new pdfviewer( this, true, mainWindow );
    pdfViewer_->setObjectName( "pdfViewer" );
    addWidget(pdfViewer_);

    connect( pdfViewer_, SIGNAL( pdfViewerDoneClicked() ), SLOT( onPdfViewingFinished() ));

    // web viewer page is default page
    setCurrentIndex( WebViewerPage );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void stackedWebViewer::setUrlVisible( bool yes, bool editable )
{
    webViewer_->setUrlVisible( yes, editable );
}

void stackedWebViewer::setURL(QString url)
{
    webViewer_->setURL(url);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void stackedWebViewer::hideInputWidgets() const
{
    sessionInfo info( this );

    // retrieve pointer to session
    SessionMainWindow *mainWindow = info();

    mainWindow->hideInputWidgets();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QWidget *stackedWebViewer::allocErrorPage()
{
    QHBoxLayout *hlayout;

    QWidget *page = new QWidget( this );
    QVBoxLayout *pageLayout = new QVBoxLayout( page );

    // setup layout
    pageLayout->addStretch();

    hlayout = new QHBoxLayout();
    hlayout->addStretch();
    hlayout->addWidget( errorPageTitle_ = new QLabel( page ) );
    hlayout->addStretch();
    pageLayout->addLayout( hlayout );

    hlayout = new QHBoxLayout();
    hlayout->addStretch();
    hlayout->addWidget( errorPageDesc_ = new QLabel( page ) );
    hlayout->addStretch();
    pageLayout->addLayout( hlayout );

    pageLayout->addStretch();

    hlayout = new QHBoxLayout();
    hlayout->addStretch();
    hlayout->addWidget( errorPageExitButton_ = new QPushButton( page ) );
    hlayout->addStretch();
    pageLayout->addLayout( hlayout );

    pageLayout->addStretch();

    // set font
    modifyFontPointSize setFontPointSize( (window()->height() * 28) / 600 );

    setFontPointSize( errorPageTitle_ );

    // set palette
    modifyPaletteForeground setForeground( modifyPaletteForeground::createPalette( hobart::colorPalette::white ) );

    setForeground( errorPageTitle_ );
    setForeground( errorPageDesc_ );

    // set object names
    page->setObjectName( "errorPage" );
    errorPageTitle_->setObjectName( "title" );
    errorPageDesc_->setObjectName( "desc" );
    errorPageExitButton_->setObjectName( "exitButton" );

    return page;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace
