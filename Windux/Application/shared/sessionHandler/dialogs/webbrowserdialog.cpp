/**
 * @file webbrowserdialog.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "webbrowserdialog.h"

#include "../widgets/stackedwebviewer.h"

#include "../sessioninfo.h"
#include "../sessionmainwindow.h"

#include <backgroundwidgettypes.h>
#include <filesystem.h>
#include <palette.h>

#include <QTimer>
#include <QVBoxLayout>

#include <logmanager.h>


///////////////////////////////////////////////////////////////////////////////////////////////////
WebBrowserDialog::WebBrowserDialog( QWidget *parent, bool isPopup ) :
    _Mybase( LeftDialogLocation, parent ),
    popup_( isPopup )
{
    setObjectName( "WebBrowserDialog" );

    // set palette
    modifyPaletteForeground setForeground( modifyPaletteForeground::createPalette( hobart::colorPalette::black ) );
    setForeground( this );

    // initialize
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
WebBrowserDialog::~WebBrowserDialog()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WebBrowserDialog::setBrowserName( const QString& value )
{
    Q_UNUSED( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WebBrowserDialog::setHtml( const QString& value )
{
    Q_UNUSED( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WebBrowserDialog::setURL( const QString& value )
{
    Q_UNUSED( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QSize WebBrowserDialog::sizeHint() const
{
    return QSize(
        (mainWindow_->width() * 925) / SessionMainWindow::scalingDividerX_,
        (mainWindow_->height() * 550) / SessionMainWindow::scalingDividerY_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool WebBrowserDialog::playVideo( const QString& filename )
{
    QString fullFilename( filesystem::absoluteFilePath( filename, folderType::Videos ) );

    // check to cache video
    if ( !isVisible() )
    {
        mediaFilename_ = fullFilename;
        return true;
    }

    // play!
    return playVideoInternal( fullFilename );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool WebBrowserDialog::showDocument( const QString& filename )
{
    QString fullFilename( filesystem::absoluteFilePath( filename, folderType::Documents ) );

    // check to cache document
    if ( !isVisible() )
    {
        documentFilename_ = fullFilename;
        return true;
    }

    return showDocumentInternal( fullFilename );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool WebBrowserDialog::showHtml( const QString& url )
{
    // check to cache document
    if ( !isVisible() )
    {
        htmlFilename_ = url;
        return true;
    }

    return showHtmlInternal( url );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool WebBrowserDialog::showDocument( const QByteArray& pdfData )
{
    // check to cache document
    if ( !isVisible() )
    {
        documentMemoryContents_ = pdfData;
        return true;
    }

    return showDocumentInternal( pdfData );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WebBrowserDialog::translate()
{
    webViewer_->translate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
hobart::widgetRendererAppearanceBase *WebBrowserDialog::rendererAppearance() const
{
    return new hobart::panelBackgroundWidgetRendererAppearance();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int WebBrowserDialog::rendererStyle() const
{
    return hobart::Normal;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WebBrowserDialog::setUrlVisible( bool yes, bool editable )
{
    webViewer_->setUrlVisible( yes, editable );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QWebView *WebBrowserDialog::webView() const
{
    if ( !webViewer_ )
        return NULL;

    return webViewer_->webView();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WebBrowserDialog::onActive( bool value )
{
    LOG_CLASS_INFO( "WebBrowserDialog", "onActive" );
    LOG_WARN("got here", NULL);
    // dialog active, check for cached document or video
    if ( value )
    {
        LOG_WARN("value", NULL);
        // show PDF document contained in memory
        if (documentMemoryContents_.length())
        {
            LOG_WARN("documentMemoryContents_", NULL);
            showDocumentInternal( documentMemoryContents_ );
            documentMemoryContents_.clear();
        }

        // show PDF document
        else if ( documentFilename_.length() )
        {
            LOG_WARN("documentFilename_", NULL);
            showDocumentInternal( documentFilename_ );
            documentFilename_.clear();
        }

        // show HTML
        else if (htmlFilename_.length())
        {
            LOG_WARN("htmlFilename_", NULL);
            showHtmlInternal(htmlFilename_);
            htmlFilename_.clear();
        }

        // play video
        else if ( mediaFilename_.length() )
        {
            LOG_WARN("mediaFilename_", NULL);
            playVideoInternal( mediaFilename_ );
            mediaFilename_.clear();
        }
    }

    // dialog inactive, pause video
    else
    {
        LOG_WARN("documentMemoryContents_", NULL);
        webViewer_->pauseVideo();

        // restore main browser window when popup dismiss
        if ( popup_ )
        {
            LOG_WARN("else popup_", NULL);
            sessionInfo info( this );

            // retrieve pointer to session
            SessionMainWindow *mainWindow( info() );

            if ( mainWindow )
            {
                LOG_WARN("else popup_ mainWindow", NULL);
                mainWindow->restoreHelpDialog();
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WebBrowserDialog::initialize()
{
    connect( this, SIGNAL( active(bool) ), SLOT( onActive(bool) ) );

    QVBoxLayout *screenLayout = new QVBoxLayout( this );

    // web viewer
    webViewer_ = new hobart::stackedWebViewer( this );
    webViewer_->setObjectName( "webViewer" );
    screenLayout->addWidget( webViewer_ );

    connect( webViewer_, SIGNAL( finished() ), SLOT( accept() ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool WebBrowserDialog::playVideoInternal( const QString& filename )
{
    return webViewer_->playVideo( filename );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool WebBrowserDialog::showDocumentInternal( const QString& filename )
{
    webViewer_->viewPdf( filename );
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool WebBrowserDialog::showDocumentInternal( const QByteArray& pdfData )
{
    webViewer_->viewPdf( pdfData );
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool WebBrowserDialog::showHtmlInternal( const QString& url )
{
    webViewer_->setURL(url);
    return true;
}
