/**
 * @file webViewer.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Scott Davis
 */


#include "ui_webViewer.h"
#include "webview.h"
#include "webViewer.h"
#include "webviewernetwork.h"
#include "qwebviewselectionsuppressor.h"

#include "../sessionmainwindow.h"
#include "../sessionmainwindowimpl.h"

#include "../dialogs/messageboxdialog.h"

#include "../modes/operatemode.h"

#include "../platform/platform.h"
#include "../platform/filesystem.h"

#include "../screens/plumainscreen.h"

#include <logmanager.h>
#include <process.h>

#include <ScaleConfiguration.h>
#include <UiUtilities.h>

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QTimer>
//#include <QUrlInfo>
#include <QVBoxLayout>
#include <QWebElement>
#include <QWebFrame>
#include <QWebHistory>
#include <QWebHistoryItem>
#include <QWebPage>
#include <QPrinter>
#include <QMessageBox>
#include <QScreen>

#include "../../source/HFC/CfgBaseFieldOffsets.h"

#include "../../platform/python.h"

/// Hobart namespace
namespace hobart {

const bool TEST_ONLINE_ORDERING = false;
static const QString pdfFeatureKey( "SCALE_PDF_VIEWER" );
static const QString docConversionFeatureKey( "SCALE_OFFICE_VIEWER" );

///////////////////////////////////////////////////////////////////////////////////////////////////
webViewer::webViewer( QWidget *parent, bool displayAsDialog, bool toolbarVisible, SessionMainWindow *mainWindow, bool showLoadProgress ) :
    _Mybase( parent ),
    ui( new Ui::webViewer ),
    isDialog_(displayAsDialog),
    iconSize_(32),
    fileDownloadInProgress_( false ),
    allowPortableDocumentFormat_( true ),
    allowMicrosoftOfficeExtensions_( false )
{   
    LOG_CLASS_INFO( "webViewer", "webViewer" );

    setObjectName( "hobartWebViewer" );
    mainWindow_ = mainWindow;

    ui->setupUi(this);
    ui->webView->resize(QSize(mainWindow->width(), mainWindow->height()));

    toolbar_ = new QWidget( this );
    createLayout();

    showProgress = showLoadProgress;
    focusOnNavigate = true;

    setPalette( QApplication::palette() );
    setAutoFillBackground( true );

    fileDownloader_ = NULL;

    session_ = mainWindow->session();           // Connect into session

    pageLoadTimer_ = new QTimer( this );
    connect(pageLoadTimer_, SIGNAL(timeout()), SLOT(onLoadTimeout()));

    // FT-1468: removed web suppressor
    suppressor = NULL;                          // new QWebViewSelectionSuppressor(ui->webView);
    printerPrompter_ = NULL;                    // access to network printer dialog

    webFileNameToPrint = "";                    // somewhere I need to update this.  Who knows about it? Where? 

    ui->homeButton->setFocusPolicy(Qt::NoFocus);
    ui->backButton->setFocusPolicy(Qt::NoFocus);
    ui->forwardButton->setFocusPolicy(Qt::NoFocus);
    ui->reloadButton->setFocusPolicy(Qt::NoFocus);
    ui->doneButton->setFocusPolicy(Qt::NoFocus);
    ui->raiseKeyboard->setFocusPolicy(Qt::NoFocus);
    ui->progressBar->setFocusPolicy(Qt::NoFocus);
    ui->printWebButton->setFocusPolicy(Qt::NoFocus);
    ui->zoomSlider->hide();

    ui->currentURL->setInsertPolicy(QComboBox::NoInsert);

    setToolBarVisible(toolbarVisible);
    setDisplayAsDialog(displayAsDialog);

    ui->progressBar->setFixedHeight( 4 );
    ui->progressBar->setTextVisible( false );

    ui->webView->setContextMenuPolicy(Qt::NoContextMenu);
    ui->webView->setMaximumSize( QSize( QWIDGETSIZE_MAX, QWIDGETSIZE_MAX ) );
    ui->webView->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
    ui->webView->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
    qDebug() << "WEB VIEWER" << ui->webView->height() << ui->webView->width();

    ui->webView->settings()->setAttribute(QWebSettings::PluginsEnabled, true);
    ui->webView->settings()->setAttribute(QWebSettings::LocalContentCanAccessFileUrls,true);

    ui->webView->settings()->setAttribute(QWebSettings::JavascriptEnabled,true);
    ui->webView->settings()->setAttribute(QWebSettings::JavascriptCanOpenWindows, true);

    // enable support for the localStorage javascript functionality
    ui->webView->settings()->setAttribute(QWebSettings::LocalStorageEnabled, true);
    ui->webView->settings()->setLocalStoragePath("/tmp/localStorage");

    // Test Java Applet execution, currently not working on Windows and would require
    // Java backend,  Javascript will work which is not based on Java believe it or not
    // Old Netscape misnomer.
    // URL location used to test if Java Applet in browser is located here.
    // http://www.java.com/en/download/testjava.jsp
    ui->webView->settings()->setAttribute(QWebSettings::JavaEnabled,true);

    ui->webView->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled,true);
    ui->webView->settings()->setAttribute(QWebSettings::AutoLoadImages, true);

    //ui->webView->installEventFilter(this);
    ui->webView->page()->installEventFilter(this);

    // install network manager
    ui->webView->page()->setNetworkAccessManager( new webViewerNetworkAccessManager( this ) );

    connect(ui->homeButton,SIGNAL(clicked()),this,SLOT(onHomeClicked()));
    connect(ui->backButton,SIGNAL(clicked()),ui->webView, SLOT(back()));
    connect(ui->forwardButton,SIGNAL(clicked()),ui->webView, SLOT(forward()));
    connect(ui->reloadButton,SIGNAL(clicked()),ui->webView, SLOT(reload()));
    connect(ui->zoomButton, SIGNAL(clicked()),this,SLOT(onZoomButtonClicked()));
    connect(ui->zoomSlider, SIGNAL(valueChanged(int)),this,SLOT(onZoomChanged(int)));
    connect(ui->zoomSlider, SIGNAL(sliderReleased()),this,SLOT(onZoomReleased()));
    connect(ui->doneButton,SIGNAL(clicked()),this,SLOT(done()));
    connect(ui->raiseKeyboard,SIGNAL(clicked()),SLOT(onRaiseKeyboard()));

    connect(ui->webView, SIGNAL(linkClicked(QUrl)), this, SLOT(onLinkClicked(QUrl)));
    connect(ui->webView, SIGNAL(urlChanged(QUrl)), SLOT(onUrlChanged(QUrl)));
    connect(ui->webView, SIGNAL(loadStarted()), SLOT(onLoadStarted()));
    connect(ui->webView, SIGNAL(loadFinished(bool)), SLOT(onLoadFinished(bool)));
    connect(ui->webView, SIGNAL(loadProgress(int)), SLOT(onLoadProgress(int)));

    connect(ui->printWebButton,SIGNAL(clicked()),this,SLOT(onPrintWebClicked()));

    connect(ui->currentURL->lineEdit(), SIGNAL(returnPressed()), this, SLOT( onUrlEntered() ) );
    connect(ui->currentURL, SIGNAL(activated(QString)),this,SLOT(onUrlEntered()));
    connect(ui->webView->page()->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()),
                 this, SLOT(populateJavaScriptWindowObject()));

    ui->webView->page()->setForwardUnsupportedContent( true );
    connect( ui->webView->page(), SIGNAL( downloadRequested(QNetworkRequest) ), SLOT( onDownload(QNetworkRequest) ) );
    connect( ui->webView->page(), SIGNAL( unsupportedContent(QNetworkReply*) ), SLOT( onUnsupportedContent(QNetworkReply*) ) );

    QFile file;
    file.setFileName(filesystem::absoluteFilePath("about.html",folderType::Html));
    LOG_DEBUG( "filename is %s", qPrintable(file.fileName()) );
    file.open(QIODevice::ReadOnly);
    testHtml = file.readAll();
    file.close();
    setHtml(testHtml);
    setToolbarState();

    // ==========

    videoSuffixList.append( "mp4" );
    videoSuffixList.append( "avc" );
    videoSuffixList.append( "avi" );

    if ( platformType::Windows == PLATFORM )
    {
        videoSuffixList.append( "wmv" );
    }

    // ==========

    // PDF
    // ---

    pdfSuffixList.append( "pdf" );

    // Word
    // ----

    // Legacy
    // Legacy filename extensions denote binary Microsoft Word formats that are deprecated with
    // the release of Microsoft Office 2007. Although the latest version of Microsoft Word can
    // still open them, they are no longer developed. Legacy filename extensions include:
    msofficeSuffixList.append( "doc" ); // Legacy Word document; Microsoft Office refers to them as "Microsoft Word 97 - 2003 Document"
    msofficeSuffixList.append( "dot" ); // Legacy Word templates; officially designated "Microsoft Word 97 - 2003 Template"

    // OOXML
    // Office Open XML (OOXML) format was introduced with Microsoft Office 2007 and became the
    // default format of Microsoft Word ever since. Pertaining file extensions include:
    msofficeSuffixList.append( "docx" ); // Word document
    msofficeSuffixList.append( "docm" ); // Word macro-enabled document; same as docx, but may contain macros and scripts
    msofficeSuffixList.append( "dotx" ); // Word template
    msofficeSuffixList.append( "dotm" ); // Word macro-enabled template; same as dotx, but may contain macros and scripts

    // Excel
    // -----

    // Legacy
    // Legacy filename extensions denote binary Microsoft Excel formats that are deprecated
    // with the release of Microsoft Office 2007. Although the latest version of Microsoft Excel
    // can still open them, they are no longer developed. Legacy filename extensions include:
    msofficeSuffixList.append( "xls" ); // Legacy Excel worksheets; officially designated "Microsoft Excel 97-2003 Worksheet"
    msofficeSuffixList.append( "xlt" ); // Legacy Excel templates; officially designated "Microsoft Excel 97-2003 Template"
    msofficeSuffixList.append( "xlm" ); // Excel macro

    // OOXML
    // Office Open XML (OOXML) format was introduced with Microsoft Office 2007 and became the
    // default format of Microsoft Excel ever since. Excel-related file extensions of this
    // format include:
    msofficeSuffixList.append( "xlsx" ); // Excel workbook
    msofficeSuffixList.append( "xlsm" ); // Excel macro-enabled workbook; same as xlsx but may contain macros and scripts
    msofficeSuffixList.append( "xltx" ); // Excel template
    msofficeSuffixList.append( "xltm" ); // Excel macro-enabled template; same as xltx but may contain macros and scripts

    // Other formats
    // Microsoft Excel uses dedicated file format that are not part of OOXML and use the
    // following extensions:
    msofficeSuffixList.append( "xlsb" ); // Excel binary worksheet (BIFF12)
    msofficeSuffixList.append( "xla" ); // Excel add-in or macro
    msofficeSuffixList.append( "xlam" ); // Excel add-in
    msofficeSuffixList.append( "xll" ); // Excel XLL add-in; a form of DLL-based add-in[1]
    msofficeSuffixList.append( "xlw" ); // Excel workspace; previously known as "workbook"

    // PowerPoint
    // ----------

    // Legacy
    msofficeSuffixList.append( "ppt" ); // Legacy PowerPoint presentation
    msofficeSuffixList.append( "pps" ); // Legacy PowerPoint slideshow

    // OOXML
    msofficeSuffixList.append( "pptx" ); // PowerPoint presentation
    msofficeSuffixList.append( "pptm" ); // PowerPoint macro-enabled presentation
    msofficeSuffixList.append( "potx" ); // PowerPoint template
    msofficeSuffixList.append( "potm" ); // PowerPoint macro-enabled template
    msofficeSuffixList.append( "ppam" ); // PowerPoint add-In
    msofficeSuffixList.append( "ppsx" ); // PowerPoint slideshow
    msofficeSuffixList.append( "ppsm" ); // PowerPoint macro-enabled slideshow
    msofficeSuffixList.append( "sldx" ); // PowerPoint slide
    msofficeSuffixList.append( "sldm" ); // PowerPoint macro-enabled slide

    documentSuffixList.append( pdfSuffixList );
    documentSuffixList.append( msofficeSuffixList );

    // ==========

    htmlSuffixList.append( "html");
    htmlSuffixList.append( "htm" );

    // ==========

    imageSuffixList.append( "png" );
    imageSuffixList.append( "bmp" );
    imageSuffixList.append( "gif" );
    imageSuffixList.append( "jpg" );
    imageSuffixList.append( "jpeg" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
webViewer::~webViewer()
{
    if ( fileDownloader_ )
        delete fileDownloader_;

    if( printerPrompter_ )
        delete printerPrompter_;

    delete ui;
}

void webViewer::runJavaScript(QString &command)
{

    LOG_CLASS_INFO( "webViewer", "runJavaScript" );
    LOG_DEBUG( "webViewer::runJavaScript() - command: %s", qPrintable(command) );

    if ( !onlineOrderingOK() )
        return;

    ui->webView->page()->mainFrame()->evaluateJavaScript(command);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewer::onPrintWebClicked()
{
    LOG_CLASS_INFO( "webViewer", "onPrintWebClicked" );

#ifndef QT_DEBUG
    // allow only for primary session
    if ( !mainWindow_->isPrimarySessionType() )
        return;
#endif

    if ( !loadingDoneOkToPrint )
        return;

    if ( !webFileNameToPrint.length() )
    {
        LOG_DEBUG("onPrintWebClicked: !webFileNameToPrint.length().",NULL);
        session_->send( Error, ErrPrinterDevice, "7561,5" );
        return;
    }

    // how many do we have?  Any? Can only print to a network printer but not a zebra variety
    int networkedPrinters = howManyNetworkPrintersAttached();

    LOG_DEBUG("onPrintWebClicked networkedPrinters: %d", networkedPrinters);

    if( networkedPrinters > 0 )
    {
        ScaleConfiguration *config     = session_->scaleConfig;
        DomPrWriteAllExternalPrinter   *externalPrinters = &config->allExternalPrinters;

        LOG_DEBUG("onPrintWebClicked networkedPrinters > 0", NULL);

        if ( networkedPrinters == 2 )
        {
            LOG_DEBUG("onPrintWebClicked networkedPrinters == 2", NULL);

            // OK so we have a 2 network printers so we need to prompt which one to print on.
            QStringList printerNumsList    = externalPrinters->extPrinterNumList().split(",");        // list of printer numbers
            QStringList printerNamesList   = externalPrinters->extPrinterNamesList().split(",");      // list of printer names
            QStringList printerTypesList   = externalPrinters->extPrinterTypeList().split(",");       // list of printer types
            QStringList printerDriversList = externalPrinters->extPrinterDriverList().split(",");

            // Then prompt the user which network printer to print to
            printerPrompter_ = new hobart::printPrompt(this, printerNumsList, printerNamesList, printerTypesList, printerDriversList, mainWindow_);
            printerPrompter_->move( (mainWindow_->width() * 275) / SessionMainWindow::scalingDividerX_, (mainWindow_->height() * 200) / SessionMainWindow::scalingDividerY_ );        // left,top
            printerPrompter_->show();

            connect(printerPrompter_, SIGNAL( printPromptDoneClicked( int ) ),    SLOT( onPrintPromptDoneClicked( int )) );
        }
        else
        {
            LOG_DEBUG("onPrintWebClicked else networkedPrinters == 0 FAIL", NULL);

            // We know we have a network printer to print on, so now render the web page to print
            QStringList printerNumsList    = externalPrinters->extPrinterNumList().split(",");        // list of printer numbers
            int printerNumber = printerNumsList.at(0).toInt();

            sendWebPagePrintMsg( printerNumber );
        }
    }
    else
    {
        // OK so we dont have a network printer so tell the user we cant print it.
        LOG_DEBUG("onPrintWebClicked: no network printers available.",NULL);
        session_->send( Error, ErrPrinterDevice, "7559,5" );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewer::onPrintPromptDoneClicked( int promptIndex )
{
    // Get the correct prompted network printer and call a method
    // that sends the message to the backend, a method everyone can use.

    sendWebPagePrintMsg( promptIndex );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int webViewer::howManyNetworkPrintersAttached()
{
    LOG_CLASS_INFO( "webViewer", "howManyNetworkPrintersAttached" );

    ScaleConfiguration *config   = session_->scaleConfig;
    DomPrWriteAllExternalPrinter *externalPrinters = &config->allExternalPrinters;

    QString tmpTypeList   = externalPrinters->extPrinterTypeList();
    QString tmpDriverList = externalPrinters->extPrinterDriverList();
    int numOfPrinterTypes = externalPrinters->extPrinterTypeCount();

    QStringList printerTypeList   = tmpTypeList.split(",");
    QStringList printerDriverList = tmpDriverList.split(",");

    int printerTypeOne = 0;
    int printerTypeTwo = 0;
    QString printerDriverOne = "";
    QString printerDriverTwo = "";
    int numOfNetworkPrintersAttached = 0;

    LOG_DEBUG("howMany numOfPrinterTypes: %d", numOfPrinterTypes);

    if ( numOfPrinterTypes > 0 )
    {
        printerTypeOne   = printerTypeList.at(0).toInt();
        printerDriverOne = printerDriverList.at(0);

        LOG_DEBUG("howMany printerTypeOne: %d", printerTypeOne);

        if ( 2 == numOfPrinterTypes )
        {
            printerTypeTwo   = printerTypeList.at(1).toInt();
            printerDriverTwo = printerDriverList.at(1);
        }

        LOG_DEBUG("howMany printerTypeTwo: %d", printerTypeTwo);

        if( printerTypeOne == NetworkPrinter && !printerDriverOne.contains("zebra", Qt::CaseInsensitive) )
            numOfNetworkPrintersAttached = 1;

        if( printerTypeTwo == NetworkPrinter && !printerDriverTwo.contains("zebra", Qt::CaseInsensitive) )
            numOfNetworkPrintersAttached = 2;


         LOG_DEBUG("howMany numOfNetworkPrintersAttached %d", numOfNetworkPrintersAttached);
    }

    LOG_DEBUG("howMany printers returned: %d", numOfNetworkPrintersAttached);

    return numOfNetworkPrintersAttached;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewer::sendWebPagePrintMsg( int printerNumber )
{
    LOG_CLASS_INFO( "webViewer", "sendWebPagePrintMsg" );

    if( printerNumber > 0 )
    {
        if ( loadingDoneOkToPrint )
        {
            // General example on printing with QPrinter
            // http://www.qtcentre.org/threads/25880-QWebView-prints-PDF-file-OK-with-QPrintDialog-only

            // NOTE: For some reason the instance of QPrinter creates an error in DEBUG MODE only.  When the error pops in DEBUG
            // MODE, the error indicated is: RPC Server unavailable
            // Researching the issue I have found that this is a WIndows 7 thing, a Windows 64-bit thing, a Visual Studio thing
            // and you can just turn off the debug exception.   It could also, perhaps, maybe, be a QT 4.8 thing and 5.0+ will fix
            // it since there are known issues with QPrinter. In any case, clicking OK on the exception and clicking RUN will
            // allow the program to continue on and printing will occur as expected and required.
            // *** In run mode, everything works fine and there is no exception or window popping up indicating an issue.
            QString appFolder = filesystem::folder( folderType::RootFolder, false);

            QPrinter printer;

            printer.setResolution(QPrinter::HighResolution);
            printer.setOutputFormat(QPrinter::PdfFormat);
            printer.setPaperSize(QPrinter::Letter);
            printer.setPrintRange(QPrinter::AllPages);
            printer.setOrientation(QPrinter::Portrait);
            printer.setFullPage(true);
            printer.setOutputFileName("webTemp.pdf");

            ui->webView->print(&printer);

            QString printArgs = QString::number(printerNumber) + "," + appFolder + "/webTemp.pdf";
            session_->send( Printer, PrPrintWebDocument,  printArgs);

            LOG_DEBUG("sendWebPagePrintMsg: Selected file sent to printer",NULL);
        }
        else
        {
            LOG_DEBUG("sendWebPagePrintMsg: Attempting to print but the web page is not completely loaded yet.",NULL);
            session_->send( Error, ErrPrinterDevice, "7561,5" );
        }
    }
    else
    {
        LOG_DEBUG("sendWebPagePrintMsg: no network printers available.",NULL);
        session_->send( Error, ErrPrinterDevice, "7559,5" );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewer::onZoomChanged(int value)
{
    qreal zoomValue = value * 1.0;
    ui->webView->setZoomFactor(zoomValue);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewer::onZoomButtonClicked()
{
    if (ui->zoomSlider->isHidden())
        ui->zoomSlider->show();
    else
        ui->zoomSlider->hide();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewer::onZoomReleased()
{
    ui->zoomSlider->hide();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewer::onRaiseKeyboard()
{
    // first press spawns keyboard
    // second press moves keyboard to top
    mainWindow_->showKeyboard(
        mainWindow_->isKeyboardVisible() ?
           SessionMainWindow::KeyboardLocationTop :
           SessionMainWindow::KeyboardLocationBottom );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewer::onDownload( const QNetworkRequest& request )
{
    LOG_CLASS_INFO( "webViewer", "onDownload" );

    LOG_INFO( "download requested '%s'", qPrintable( request.url().toString() ) );
    downloadFile( request.url() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewer::onUnsupportedContent( QNetworkReply *reply )
{
    LOG_CLASS_INFO( "webViewer", "onUnsupportedContent" );

    LOG_INFO( "unsupported content '%s'", qPrintable( reply->url().toString() ) );
    downloadFile( reply->url() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool webViewer::downloadFile( const QUrl& url )
{
    LOG_CLASS_INFO( "webViewer", "downloadFile" );

    QString path( url.path() );

    QFileInfo fileInfoData = QFileInfo( path );
    QString fileExtension( fileInfoData.suffix() );

    // make sure file extension is lowercase
    fileExtension = fileExtension.toLower();

    // update licensing
    checkLicensing();

    //
    // check to handle unsupported content manually
    //

    // garbage collect
    if ( fileDownloader_ != NULL )
    {
        delete fileDownloader_;
        fileDownloader_ = NULL;
    }

    // documents (i.e. pdf)
    if ( documentSuffixList.contains( fileExtension ) )
    {
        LOG_INFO( "detected document file", NULL );

        bool isPDF( pdfSuffixList.contains( fileExtension ) );
        QString featureKey( isPDF ? pdfFeatureKey : docConversionFeatureKey );

        // check if allowed
        if ( !allowedDocSuffixList.contains( fileExtension ) )
            showFeatureNotSupportedDialog( featureKey );
        // check if installed
        else if ( !checkFeatureInstalled( featureKey ) )
            showFeatureNotInstalledDialog( featureKey );
        else
        {
            // show progress indicator
            fileDownloadInProgress_ = true;

            // reset progress
            ui->progressBar->setValue( 0 );
            ui->progressBar->show();

            // for pdf files our multiplier is 0.9
            fileDownloadProgressMultiplier_ = 0.9;

            // set up downloader to retrieve pdf file and load into memory
            // block
            fileDownloader_ = new FileDownloader( url, this );

            connect( fileDownloader_, SIGNAL( downloadProgress(int) ), SLOT( updateDownloadProgress(int) ) );

            // when not pdf file we need to convert it first!
            if ( isPDF )
                connect( fileDownloader_, SIGNAL( downloaded(bool) ), SLOT( pdfDownloaded(bool) ) );
            else
            {
                LOG_INFO( "need to convert to pdf", NULL );
                connect( fileDownloader_, SIGNAL( downloaded(bool) ), SLOT( convertToPdfDownloaded(bool) ) );
            }
        }

        return true;
    }

    // videos (i.e. .mov)
    else if ( videoSuffixList.contains( fileExtension ) )
    {
        // show progress indicator
        fileDownloadInProgress_ = true;

        // reset progress
        ui->progressBar->setValue( 0 );
        ui->progressBar->show();

        // for movie files our multiplier is 1.0
        fileDownloadProgressMultiplier_ = 1.0;

        // set up downloader to retrieve video file and load into memory
        // block
        fileDownloader_ = new FileDownloader( url, this );

        connect( fileDownloader_, SIGNAL( downloadProgress(int) ), SLOT( updateDownloadProgress(int) ) );
        connect( fileDownloader_, SIGNAL( downloaded(bool) ), SLOT( videoDownloaded(bool) ) );

        return true;
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewer::setToolbarState()
{
    QWebHistory *history( ui->webView->history() );
    QList<QWebHistoryItem> back( history->backItems( 32 ) );
    QList<QWebHistoryItem> forward( history->forwardItems( 32 ) );

    ui->backButton->setEnabled( history->canGoBack() );
    ui->forwardButton->setEnabled( history->canGoForward() );
    ui->reloadButton->setDisabled( ui->currentURL->currentText().isEmpty() );

    ui->currentURL->clear();

    foreach ( const QWebHistoryItem& h, back )
        ui->currentURL->insertItem( -1, h.url().toString(), QVariant( h.url() ) );

    const QUrl current( ui->webView->url() );
    ui->currentURL->insertItem( -1, current.toString(), QVariant( current ) );
    ui->currentURL->setCurrentIndex( 0 );
    ui->currentURL->setEditText( current.toString() );

    foreach ( const QWebHistoryItem& h, forward )
        ui->currentURL->insertItem( -1, h.url().toString(), QVariant( h.url() ) );

    //if (ui->currentURL->count() > 0)
    //    ui->homeButton->setDisabled(false);
    //else
    //    ui->homeButton->setDisabled(true);

    ui->webView->setZoomFactor(1.0);
    ui->zoomSlider->setValue(1);
    ui->zoomSlider->hide();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewer::setHomeUrl(const QString &url)
{
    homeUrl_ = url;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewer::clear()
{
    LOG_CLASS_INFO( "webViewer", "clear" );

    QWebElement focusedElement = findFocusedPageInput();

    if (!focusedElement.isNull()) 
    {
        LOG_INFO("got a focused element, clearing", NULL);
        // for some reason you can't just set the attribute, have to use JS
        focusedElement.evaluateJavaScript("this.value = ''").toString();
    }
    else
    {
        LOG_INFO("no focused element, not clearing", NULL);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewer::dismiss()
{
    LOG_CLASS_INFO( "webViewer", "dismiss" );
    //ui->currentURL->setFocus();
    //ui->webView->setFocus();
    //ui->currentURL->setFocus();
    // was here
    //ui->webView->page()->focusNextPrevChild(childToggle);
    //childToggle = !childToggle;

    //ui->webView->page()->mainFrame()->setFocus();
    LOG_DEBUG( "in dismiss callback", NULL );
    setFocusFirstTextInput();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool webViewer::eventFilter( QObject *obj, QEvent *e )
{
    LOG_CLASS_INFO( "webViewer", "eventFilter" );

    bool result = _Mybase::eventFilter( obj, e );
    if ( ui->webView == obj )
    {
        if ( QEvent::FocusIn == e->type() )
        {
            maybeShowKeyboard(findFocusedPageInput());
        }
    }

    return result;
    // default event filter
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewer::done()
{
    emitDoneClicked();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewer::onHomeClicked()
{
    UiUtilities *util( mainWindow_->session()->uiUtilities );

    if ( homeUrl_.isEmpty() )
        homeUrl_ =  QUrl::fromUserInput( util->ascertainHomepage() );

    setURLInternal(homeUrl_);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QWebElement webViewer::findFormInputWithName(QString name)
{
    QWebElement document(ui->webView->page()->mainFrame()->documentElement());


    QString query;
    query = "input[name=" + name + "]";

    // find all text input fields
    QWebElement element = document.findFirst( query );

    if (element.isNull())
    {
        QList<QWebFrame *> frameList = ui->webView->page()->mainFrame()->childFrames();
        if (frameList.count() > 0)
        {
            foreach (QWebFrame *curFrame, frameList)
            {
                QWebElement document(curFrame->documentElement());
                QWebElement element = document.findFirst( query );
                if (!element.isNull())
                    break;
            }
        }
    }
    return element;

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewer::setFocusFirstTextInput()
{
    LOG_CLASS_INFO( "webViewer", "setFocusFirstTextInput" );

    //QWebElement document(ui->webView->page()->currentFrame()->documentElement());
    QWebElement document(ui->webView->page()->mainFrame()->documentElement());
    QWebElement element;
    bool foundElement = false;

    QList<QString> queryList;

    queryList.append(QString("input[type=text]"));
    queryList.append(QString("textarea"));
    queryList.append(QString("input[type=password]"));

    foreach (QString curQuery, queryList)
    {
        // find all text input fields in main frame that match query list items
        element = document.findFirst( curQuery );

        // if nothing found in main frame
        if (element.isNull())
        {
            // TODO
            // looking at child frames, this only goes one level deep to children not
            // children frames of child frames
            QList<QWebFrame *> frameList = ui->webView->page()->mainFrame()->childFrames();
            if (frameList.count() > 0)
            {
                // for each child frame
                foreach (QWebFrame *curFrame, frameList)
                {
                    LOG_DEBUG("Frame name is %s",qPrintable(curFrame->baseUrl().toString()));
                    QWebElement document(curFrame->documentElement());
                    element = document.findFirst( curQuery );

                    if (!element.isNull())
                    {
                        foundElement = true;
                        LOG_DEBUG( "found text or password element", NULL );
                        break;
                    }
                }
            }
        }
        if (foundElement)
        {
            LOG_DEBUG( "found text or password element", NULL );
            break;
        }
    }

    if (!element.isNull())
    {
        element.setFocus();
        maybeShowKeyboard(element);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString webViewer::getWebElementName(QWebElement element)
{
    return element.attribute("name");
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QWebElement webViewer::findFocusedPageInput()
{
    LOG_CLASS_INFO( "webViewer", "findFocusedPageInput" );

    QWebElement document(ui->webView->page()->mainFrame()->documentElement());

    // find all text input fields
    QWebElementCollection elements = document.findAll( QString( "input[type=text]" ) );
    elements.append(document.findAll( QString( "input[type=password]" ) ));
    elements.append(document.findAll( QString( "textarea" ) ));

    // for each field found
    foreach ( QWebElement element, elements )
    {
        // determine if the field has focus
        if ( element.hasFocus() )
        {
            LOG_DEBUG( "Input field has focus, specifics of item below:", NULL );
            QStringList attributesList = element.attributeNames();
            foreach (QString attributeName, attributesList)
            {
                LOG_DEBUG( "%s: %s", qPrintable(attributeName), qPrintable(element.attribute(attributeName)) );
            }
            return element;
        }
    }

    return QWebElement();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewer::internalFocusChanged()
{
    LOG_CLASS_INFO( "webViewer", "internalFocusChanged" );

    QWebElement document(ui->webView->page()->mainFrame()->documentElement());

    // find all text input fields
    QWebElementCollection elements = document.findAll( QString( "input[type=text]" ) );
    elements.append(document.findAll( QString( "textarea" ) ));

    // for each field found
    foreach ( QWebElement element, elements )
    {
        // determine if the field has focus
        if ( element.hasFocus() )
        {
            LOG_DEBUG( "Input field has focus, specifics of item below:", NULL );
            QStringList attributesList = element.attributeNames();
            foreach (QString attributeName, attributesList)
            {
                LOG_DEBUG( "%s: %s", qPrintable(attributeName), qPrintable(element.attribute(attributeName)) );
            }
        }
    }

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewer::maybeShowKeyboard(QWebElement inputElement)
{
  LOG_CLASS_INFO( "webViewer", "maybeShowKeyboard" );

  if (!inputElement.isNull())
  {
      // screen height (pixels)
      int screenY = QGuiApplication::primaryScreen()->geometry().height();
      // how far down the center of the element is on the screen (pixels)
      int elementY = webView()->mapToGlobal(inputElement.geometry().center()).y();

      // if we're more than halfway down the screen, put the keyboard on the top
      SessionMainWindow::KeyboardLocation location = SessionMainWindow::KeyboardLocationBottom;
      if (elementY > (screenY/2))
      {
          location = SessionMainWindow::KeyboardLocationTop;
      }

      if (inputElement.hasAttribute("readonly"))
      {
          LOG_INFO("clicked a readonly element, not popping keyboard", NULL);
      }
      else
      {
          mainWindow_->showKeyboard(location);
      }
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewer::handleFocusIn()
{
    LOG_CLASS_INFO( "webViewer", "handleFocusIn" );

    LOG_TRACE( "in slot handleFocusIn", NULL );

    // only pop the keyboard if we are booted up AND the webbrowser is actually in the window
    // if this doesn't work completely, we will do it ONLY if the left panel is active

    ScaleConfiguration *config = session_->scaleConfig;
    MadpOperatorDisplayFields *display_fields = &config->operatorDisplayFields;

    int index = display_fields->dcaStartScreenType();
    bool booted = session_->scaleConfig->internalConfig.scaleBootComplete;

    if (  booted && index == 8 )
    {
        maybeShowKeyboard(findFocusedPageInput());
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewer::handleFocusOut()
{
    LOG_CLASS_INFO( "webViewer", "handleFocusOut" );

    LOG_TRACE( "in slot handleFocusOut", NULL );
    mainWindow_->hideKeyboard();
}

void webViewer::lookupPLU(QString plu, QString customerName, QString pickupTime, QString orderNumber, QString orderNumId, QString byCount)
{
    LOG_CLASS_INFO( "webViewer", "lookupPLU" );
    QString msgData(plu + "," + orderNumber + "," + orderNumId + "," + customerName + "," + pickupTime + "," + byCount);

    LOG_DEBUG("webViewer::lookupPLU  %s", qPrintable(msgData));

    if ( !onlineOrderingOK() )
        return;

    OperateMode *mode( mainWindow_->retrieveMode<OperateMode>() );
    PluMainScreen *pluMainScreen( mode ? mode->isCached<PluMainScreen>() : NULL );

    // SOF-4922 reset OO url
    if ( pluMainScreen )
        pluMainScreen->setOnlineOrderingProductUrl( QString() );

   session_->send(ProductDev, ProductReadOnlineOrderingProduct, msgData);
}

void webViewer::print(void)
{
    LOG_CLASS_INFO( "webViewer", "print" );
    
    LOG_DEBUG("webViewer print", NULL);

    if ( !onlineOrderingOK() )
        return;

    session_->send(SessionDev, SnPrintKeyPressed);
}

void webViewer::setMainURL(QString URL)
{
    LOG_CLASS_INFO( "webViewer", "setMainURL" );
    
    LOG_WARN( "got into setMainURL", NULL);
    
    LOG_WARN( "setting to %s", URL.toStdString().c_str() );

    if ( !onlineOrderingOK() )
    {
        LOG_WARN( "cancelling because OO is not licensed/enabled", NULL );
        return;
    }

    const bool useCfgMethod( false );

    if ( useCfgMethod )
        session_->send(ConfigurationDev, CfgSaveSingleField, "," + QString::number(fieldOffsetCfgDefaultRunScreenHomePage) + "," + URL);
    else
    {
        OperateMode *mode( mainWindow_->retrieveMode<OperateMode>() );
        PluMainScreen *pluMainScreen( mode ? mode->isCached<PluMainScreen>() : NULL );

        if ( pluMainScreen )
            pluMainScreen->setOnlineOrderingProductUrl( URL );
    }
}

void webViewer::setEnhancedURL(QString URL)
{
    LOG_CLASS_INFO( "webViewer", "setEnhancedURL" );

    if ( !onlineOrderingOK() )
    {
        LOG_WARN( "cancelling because OO is not licensed/enabled", NULL );
        return;
    }

    LOG_INFO("setting to %s", URL.toStdString().c_str());

    session_->send(ConfigurationDev, CfgSaveSingleField, "," + QString::number(fieldOffsetCfgEnhancedCenterHomePage) + "," + URL);
}

void webViewer::jsSwipeAndLookupPlu(QString name, QString urlStr)
{
    LOG_CLASS_INFO( "webViewer", "jsSwipeAndLookupPlu" );

    LOG_DEBUG( "in slot jsSwipeAndLookupPlu", NULL );

    if ( !onlineOrderingOK() )
        return;

    LOG_DEBUG( "SCALE_ONLINE_ORDERING is licensed.", NULL );

    if( "printLabelButton" == name )
    {

        ScaleConfiguration *config     = session_->scaleConfig;
        DomNormalScaleOperation *normalScaleOperation = &config->normalScaleOperation;
        const QString bbIpAddress( normalScaleOperation->nowServingQueueIPAddress() );

        //QMessageBox::information(NULL,"jsSwipeAndLookupPlu","I'm called by js with plu: "  + urlStr);
        //QString baseUrl("http://" + bbIpAddress + ":8000/OnlineOrdering/default/orderDetailsMain.html");     // works from laptop
        QString baseUrl("http://" + bbIpAddress + "/OnlineOrdering/default/orderDetailsMain.html");     // works from scale

        QStringList urlParams( urlStr.split( '&' ) );
        QString pluNum("");
        if( 6 == urlParams.length() )
        {
            QStringList pair( urlParams[5].split( '=' ) );
            if( 2 == pair.size() )
                pluNum = pair[1];
        }

        // update the configuration value for the main screen URL.  Setting this config will make the main screen flashkey area display the URL (current details)
        // Todo: UI #1 - If flashkeys are the current display option, display the web widget instead.  Hmmmmmmmmmmm
        // might have to set a new scaleConfig value indicating what is going on so we can switch the display option if needed in pluMainScreen. Hmmmmmmmmmm
        session_->send(ConfigurationDev , CfgSaveSingleField, "," + QString::number( fieldOffsetCfgDefaultRunScreenHomePage ) + "," + baseUrl + urlStr );

        if( pluNum.toInt() > 0 )
            session_->send( ProductDev, ProductRead, pluNum );

        mainWindow_->swipeTheScreen( ScreenWidget::Left );
    }
    else if( "mainScreenPrint" == name)
    {
        session_->send( SessionDev, SnPrintKeyPressed );
        mainWindow_->swipeTheScreen( ScreenWidget::Right );
    }
    else if( "mainScreenCancel" == name)
    {
        mainWindow_->swipeTheScreen( ScreenWidget::Right );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewer::onUrlChanged( const QUrl &url )
{
    // ignore the about page
    if ( "about:blank" == url.toString() )
        return;

    // update current url
    ui->currentURL->setEditText( url.toString() );
    setToolbarState();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewer::onLoadStarted()
{
    LOG_CLASS_INFO( "webViewer", "onLoadStarted" );

    LOG_DEBUG( "start of page loading...", NULL );

    // reset progress
    if ( showProgress )
    {
        ui->progressBar->setValue( 0 );
        ui->progressBar->show();
    }

    // start timer
    pageLoadTimer_->setSingleShot( true );
    pageLoadTimer_->start( PAGE_LOAD_TIMEOUT );
	
    // we cannot print until loadFinished has been called and the page is completely loaded
    // so in that case we have to eliminate the possibility from allowing the user to print
    loadingDoneOkToPrint = false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewer::onLoadProgress( int progress )
{
    LOG_CLASS_INFO( "onLoadProgress", "onLoadProgress" );

    if (( showProgress ) && ( !fileDownloadInProgress_ ))
    {
        LOG_TRACE( "setting progress to %d", progress );
        ui->progressBar->setValue( progress );

        // make sure visible
        if ( 100 != progress )
        {
            ui->progressBar->show();

            // reset timer (since we are making progress)
            pageLoadTimer_->setSingleShot( true );
            pageLoadTimer_->start( PAGE_LOAD_TIMEOUT );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewer::onLoadFinished( bool ok )
{
    LOG_CLASS_INFO( "webViewer", "onLoadFinished" );

    // TODO: may need closing semicolon on end of this
    static const QString javascriptFocusHandler(
        "\n"
        "function isTextElement(el) {\n"
        "  return (el.tagName == \"INPUT\" && ((el.type == \"text\") || (el.type == \"password\"))) || el.tagName == \"TEXTAREA\";\n"
        "}\n"
        "function isButtonElement(el) {\n"
        "  return el.tagName == \"BUTTON\";\n"
        "}\n"
        "this.addEventListener(\"DOMFocusIn\", function(e) {\n"
        "  if (isTextElement(e.target)) {\n"
        "    BrowserView.handleFocusIn(e.target.name);\n"
        "  }\n"
        "}, false);\n"
        "this.addEventListener(\"DOMFocusOut\", function(e) {\n"
        "  if (isTextElement(e.target)) {\n"
        "    BrowserView.handleFocusOut(e.target.name);\n"
        "  }\n"
        "}, false)\n" );

    // cancel any gestures when we get a scroll event
    // only works for scrolling on the main body, unfortunately
    static const QString preventScrollSwipes("document.documentElement.addEventListener(\"scroll\", BrowserView.noSwipe);");

    if ( ok )
        LOG_DEBUG( "end of page loading", NULL );
    else
        LOG_WARN( "end of page loading, have errors!", NULL );

    LOG_TRACE( "in onloadfinished", NULL );

    QWebFrame *mainFrame( ui->webView->page()->mainFrame() );

    QWebElement document( mainFrame->documentElement() );
    document.evaluateJavaScript( javascriptFocusHandler );
    document.evaluateJavaScript( preventScrollSwipes );

    // stop timer
    pageLoadTimer_->stop();

    // hide progress bar
    if (( showProgress ) && ( !fileDownloadInProgress_ ))
    {
        LOG_TRACE( "hiding progress bar", NULL );
        ui->progressBar->hide();
    }
	
    // we cannot print until loadFinished has been called and the page is completely loaded
    // so in that case we have to eliminate the possibility from allowing the user to print
    // Wanted to also use this if multiple frames existed since they will all have their own
    // loadFinished events and triggers.  This SLOT is signaled off of the mainframe (supposebly)

    if ( ok )
        loadingDoneOkToPrint = true;

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewer::onLoadTimeout()
{
    LOG_CLASS_INFO( "webViewer", "onLoadTimeout" );

    LOG_WARN( "page did not load in a timely manner!", NULL );
    ui->webView->stop();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewer::populateJavaScriptWindowObject()
{
    QWebFrame *mainFrame;
    QWebElement document;

    mainFrame = ui->webView->page()->mainFrame();
    document = mainFrame->documentElement();
    ui->webView->page()->mainFrame()->addToJavaScriptWindowObject("BrowserView", this);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewer::setBrowserName(const QString &name)
{
    ui->currentURL->setEditText(name);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewer::setUrlVisible(bool yes, bool editable)
{
    if (yes)
    {
        if( TEST_ONLINE_ORDERING )
            editable = true;

        ui->currentURL->show();
        if (editable)
            ui->currentURL->setEnabled(true);
        else
            ui->currentURL->setDisabled(true);
    }
    else
    {
        ui->currentURL->hide();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewer::setBrowserNameVisible(bool yes)
{
    if (yes)
    {
        ui->currentURL->show();
        ui->currentURL->setDisabled(true);
    }
    else
    {
        ui->currentURL->hide();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewer::setHtml(const QString &htmlText)
{
    ui->webView->setHtml(htmlText);
    setToolbarState();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewer::setURL( const QString &newUrl, bool force )
{
    setURLInternal( QUrl::fromUserInput( newUrl ), force );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewer::setURLInternal( const QUrl &newUrl, bool force )
{
    LOG_CLASS_INFO( "webViewer", "setURLInternal" );

    // ignore same value
    if ( !force )
    {
        QUrl oldUrl( ui->webView->url() );

        if ( newUrl == oldUrl )
            return;
    }

    // validate url
    if ( newUrl.isValid() )
    {
        if ( homeUrl_.isEmpty() )
            homeUrl_ = newUrl;

        if ( showProgress )
        {
            LOG_TRACE( "showing progress bar", NULL );
            ui->progressBar->show();
        }

        ui->currentURL->setEditText( newUrl.toString() );

        ui->webView->setUrl(newUrl);
        setToolbarState();

        mainWindow_->hideKeyboard();

        if ( focusOnNavigate )
            ui->webView->setFocus();
		
        webFileNameToPrint = newUrl.toString();
    }
    else
    {
        if ( showProgress )
        {
            LOG_TRACE( "hiding progress bar", NULL );
            ui->progressBar->hide();
        }

        // TODO
        // URL is invalid
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewer::setDisplayAsDialog(bool value)
{
    ui->doneButton->setVisible(value);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewer::setToolBarVisible(bool value)
{
    toolbar_->setVisible( value );
    ui->line->setVisible( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QWebView *webViewer::webView() const
{
    return ui->webView;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewer::resizeEvent( QResizeEvent *e )
{
    _Mybase::resizeEvent( e );

    // move zoom slider below zoom button
    QRect g( ui->zoomSlider->geometry() );
    g.moveTopLeft( ui->zoomButton->geometry().bottomLeft() );
    ui->zoomSlider->setGeometry( g );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewer::loadTestPage()
{
    setHtml(testHtml);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewer::scroll( int dx, int dy )
{
    ui->webView->page()->mainFrame()->scroll( dx, dy );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// link interception
void webViewer::onLinkClicked( const QUrl& url )
{
    LOG_CLASS_INFO( "webViewer", "onLinkClicked" );

    QString checkUrl = url.toString();
    LOG_DEBUG("%s", qPrintable(checkUrl));

    if (url.isValid())
    {
        QString path = url.path();
        LOG_DEBUG( "%s", qPrintable(path) );

        QFileInfo fileInfoData = QFileInfo(url.path());

        LOG_DEBUG("absolute path is %s", qPrintable(fileInfoData.absoluteFilePath()));
        QString fileExtension = fileInfoData.suffix();
        QString filePath;

        // make sure file extension is lowercase
        fileExtension = fileExtension.toLower();

        // update licensing
        checkLicensing();

        // check check scheme of URL
        // if local file or file://
        LOG_DEBUG("fileInfoData filename is %s", qPrintable(fileInfoData.filePath()));
        LOG_DEBUG("url scheme is :%s:", qPrintable(url.scheme()));

        // if the url to process is a
        // file:// url
        if ((url.scheme().compare("file",Qt::CaseInsensitive) == 0) || (url.scheme() == ""))
        {
            // check file extension and remap to appropriate folder on local storage media (drive, sd card, etc.)
            if (videoSuffixList.contains(fileExtension))
            {
                filePath = filesystem::absoluteFilePath(fileInfoData.fileName(),folderType::Videos);
            }
            else if (imageSuffixList.contains(fileExtension))
            {
                filePath = filesystem::absoluteFilePath(fileInfoData.fileName(),folderType::Images);

            }
            else if (documentSuffixList.contains(fileExtension))
            {
                filePath = filesystem::absoluteFilePath(fileInfoData.fileName(),folderType::Documents);
            }

            else if (htmlSuffixList.contains(fileExtension))
            {
                filePath = filesystem::absoluteFilePath(fileInfoData.fileName(),folderType::Html);
            }

            LOG_DEBUG("filePath is %s", qPrintable(filePath));

            // if the file is a video based on its extension then
            if (videoSuffixList.contains(fileExtension))
            {
                //LOG_DEBUG( "Selected video link is %s", qPrintable(filePath) );
                LOG_DEBUG( "Selected file path is %s", qPrintable(filePath) );
                LOG_DEBUG( "video link signal emitted", NULL );
                //ui->currentURL->setEditText(url.toString());
                // emit video link clicked in browser
                emitVideoLinkClicked(filePath);
            }

            // if it is a pdf based on extension
            else if (documentSuffixList.contains(fileExtension))
            {
                LOG_DEBUG( "Selected filename is %s", qPrintable(filePath) );
                LOG_DEBUG( "pdf link signal emitted", NULL );

                bool isPDF( pdfSuffixList.contains( fileExtension ) );
                QString featureKey( isPDF ? pdfFeatureKey : docConversionFeatureKey );

                if ( !allowedDocSuffixList.contains( fileExtension ) )
                    showFeatureNotSupportedDialog( featureKey );
                // check if installed
                else if ( !checkFeatureInstalled( featureKey ) )
                    showFeatureNotInstalledDialog( featureKey );
                else if ( !isPDF )
                {
                    QDir folder( QDir::temp() );
                    QString convertedFileName( folder.absoluteFilePath( fileInfoData.fileName() ) );

                    // generate new file name
                    int pos( convertedFileName.lastIndexOf( '.' ) );

                    if ( 0 < pos )
                    {
                        convertedFileName.truncate( pos );
                        convertedFileName.append( ".pdf" );
                    }
                    else
                    {
                        showDocumentConversionFailedDialog();
                        return;
                    }

                    LOG_INFO( "converting '%s'' to '%s'...", qPrintable( filePath ), qPrintable( convertedFileName ) );

                    // do conversion
                    if ( !convertToPdf( filePath, convertedFileName ) )
                    {
                        LOG_WARN( "conversion failed '%s'' to '%s'...", qPrintable( filePath ), qPrintable( convertedFileName ) );
                        showDocumentConversionFailedDialog();
                        return;
                    }

                    emitPdfLinkClicked(convertedFileName);
                }
                else
                {
                    emitPdfLinkClicked(filePath);
                }
            }
            // else html file on local drive and need to remap it
            // to html folder on local media
            else
            {
                // navigate to html, asp, etc page
                setURLInternal(url);

                // emit link in case parent wants to do something else
                emitLinkClicked(url.toString());
            }

        }

        // else http:// url
        else if ((url.scheme().compare("http",Qt::CaseInsensitive) == 0) ||
                 (url.scheme().compare("https",Qt::CaseInsensitive) == 0) )
        {
            // check for file download
            if ( downloadFile( url ) )
            {
                LOG_INFO( "detected downloadable file!", NULL );
            }

            // else http url and not PDF then let webkit handle the file
            // on its own
            else
            {
                filePath = url.toString();

                LOG_TRACE( "setting url to '%s'...", qPrintable( filePath ) );

                // navigate to html, asp, etc page
                setURLInternal(url);

                // emit link in case parent wants to do something else
                emitLinkClicked(filePath);
            }

        }
        else
        {
            // TODO:
            // Need to emit error to operator
            LOG_WARN("Unsupported URL scheme: %s", qPrintable(url.scheme()));
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewer::translate()
{
    ui->retranslateUi(this);
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewer::pdfDownloaded( bool success )
{
    LOG_CLASS_INFO( "webViewer", "pdfDownloaded" );

    LOG_INFO( "download complete %d", success );

    if ( success )
    {
        QString downloadedFile( fileDownloader_->downloadedFile() );
        LOG_DEBUG( "showing pdf '%s'...", qPrintable( downloadedFile ) );

        ui->progressBar->setValue( 95 );
        QApplication::processEvents();

        // show!
        emitPdfLinkClicked( downloadedFile );
        ui->progressBar->setValue( 100 );
    }

    fileDownloadInProgress_ = false;
    ui->progressBar->hide();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewer::convertToPdfDownloaded( bool success )
{
    LOG_CLASS_INFO( "webViewer", "convertToPdfDownloaded" );

    LOG_INFO( "download complete %d", success );
    bool conversionFailed = false;

    if ( success )
    {
        QString originalFileName( fileDownloader_->downloadedFile() );
        QString convertedFileName( originalFileName );

        // generate new file name
        int pos( convertedFileName.lastIndexOf( '.' ) );

        if ( 0 < pos )
        {
            convertedFileName.truncate( pos );
            convertedFileName.append( ".pdf" );
        }
        else
        {
            LOG_WARN( "badly formed file name '%s'", qPrintable( originalFileName ) );
            conversionFailed = true;
            goto done;
        }

        LOG_INFO( "converting '%s' to '%s'...", qPrintable( originalFileName ), qPrintable( convertedFileName ) );

        ui->progressBar->setValue( 90 );
        QApplication::processEvents();

        // do conversion
        if ( !convertToPdf( originalFileName, convertedFileName ) )
        {
            LOG_WARN( "conversion failed!", NULL );
            conversionFailed = true;
            goto done;
        }

        ui->progressBar->setValue( 95 );
        QApplication::processEvents();

        // show!
        emitPdfLinkClicked( convertedFileName );
        ui->progressBar->setValue( 100 );
    }

done:

    // reset download progress
    fileDownloadInProgress_ = false;
    ui->progressBar->hide();

    // conversion failure!
    if ( conversionFailed )
        showDocumentConversionFailedDialog();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool webViewer::convertToPdf( const QString& originalFileName, const QString& convertedFileName )
{
    LOG_CLASS_INFO( "webViewer", "convertToPdf" );

    LOG_INFO( "converting '%s'' to '%s'...", qPrintable( originalFileName ), qPrintable( convertedFileName ) );

    QDir folder( QDir::temp() );

    // do conversion
#if defined( Q_OS_WIN )

    // Windows
    // use libreoffice to convert

    QString cmd( "\"C:/Program Files/LibreOffice 4/program/soffice.exe\" --headless --convert-to pdf --outdir " + folder.path() + " " + originalFileName );

#elif defined( Q_OS_LINUX )

    // linux
    // use libreoffice to convert

    QString cmd( "/usr/local/bin/libreofficedev4.4 --headless --convert-to pdf --outdir " + folder.path() + " \"" + originalFileName + "\"" );

#endif

    process p;

    LOG_TRACE( "converting file... cmd is '%s'", qPrintable( cmd ) );

    if ( !p.start( cmd ) )
    {
        LOG_WARN( "failed to spawn process '%s'", qPrintable( cmd ) );
        return false;
    }
    else if ( !p.waitForFinished( 60000 ) )
    {
        LOG_WARN( "process '%s' did not complete in a timely manner", qPrintable( cmd ) );
        return false;
    }

    LOG_TRACE( "conversion rc %d", p.getExitCode() );



    // verify conversion
    if ( !QFile::exists( convertedFileName ))
    {
        LOG_WARN( "conversion failed, output file '%s' does not exist", qPrintable( convertedFileName ) );
        return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewer::videoDownloaded( bool success )
{
    LOG_CLASS_INFO( "webViewer", "videoDownloaded" );

    LOG_INFO( "download complete %d", success );

    if ( success )
    {
        emitVideoLinkClicked( fileDownloader_->downloadedFile() );
    }

    fileDownloadInProgress_ = false;
    ui->progressBar->hide();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewer::updateDownloadProgress( int value )
{
    LOG_CLASS_INFO( "webViewer", "updateDownloadProgress" );

    if ( !fileDownloadInProgress_ )
        return;

    // adjust progress because some post-processing (i.e. conversion, saving) may occur
    int newValue( value * fileDownloadProgressMultiplier_ );

    if ( ui->progressBar->value() < newValue )
    {
        LOG_TRACE( "progress update %d", newValue );
        ui->progressBar->setValue( newValue );

        // make sure visible
        if ( 100 != value )
            ui->progressBar->show();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewer::emitDoneClicked()
{
    LOG_CLASS_INFO( "webViewer", "emitDoneClicked" );

    LOG_TRACE( "emitting signal", NULL );
    emit webViewerDoneClicked();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewer::emitLinkClicked(const QString &url)
{
    LOG_CLASS_INFO( "webViewer", "emitLinkClicked" );

    LOG_TRACE( "emitting signal", NULL );
    emit webViewerLinkClicked(url);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewer::emitVideoLinkClicked(const QString &url)
{
    LOG_CLASS_INFO( "webViewer", "emitVideoLinkClicked" );

    LOG_TRACE( "emitting signal", NULL );
    emit webViewerVideoLinkClicked(url);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewer::emitPdfLinkClicked(const QString &url)
{
    LOG_CLASS_INFO( "webViewer", "emitPdfLinkClicked" );

    LOG_TRACE( "emitting signal", NULL );
    emit webViewerPdfLinkClicked(url);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewer::onUrlEntered()
{
    const QString s( ui->currentURL->currentText() );

    int index( ui->currentURL->findText( s ) );

    // we have new url
    if ( -1 == index )
        setURL( ui->currentURL->currentText() );
    else
    {
        QVariant v( ui->currentURL->itemData( index ) );

        setURLInternal( v.toUrl() );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewer::showFeatureNotSupportedDialog( const QString& feature )
{
    QString msg1( tr( "Feature %1 not enabled." ) );
    QString msg2( tr( "Contact HT.support@itwfeg.com to enable this product feature." ) );

    msg1 = msg1.arg( feature );

    // pop nagware about how this feature is not supported but may be purchased
    setHtml(
        "<html>\n"
        "<body>\n"
        "<CENTER>\n"
        "<b>" + msg1 + "<b><br>\n" + msg2 + "\n"
        "</CENTER>\n"
        "</body>\n"
        "</html>\n" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewer::showFeatureNotInstalledDialog( const QString& feature )
{
    QString msg1( tr( "Feature %1 not installed." ) );
    QString msg2( tr( "Please install the feature and try again." ) );

    msg1 = msg1.arg( feature );

    // pop nagware about how this feature is not supported but may be purchased
    setHtml(
        "<html>\n"
        "<body>\n"
        "<CENTER>\n"
        "<b>" + msg1 + "<b><br>\n" + msg2 + "\n"
        "</CENTER>\n"
        "</body>\n"
        "</html>\n" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewer::showDocumentConversionFailedDialog()
{
    QString msg1( tr( "Document conversion has failed." ) );
    QString msg2( tr( "Verify version compatibility of office document to be displayed." ) );

    setHtml(
        "<html>\n"
        "<body>\n"
        "<CENTER>\n"
        "<b>" + msg1 + "<b><br>\n" + msg2 + "\n"
        "</CENTER>\n"
        "</body>\n"
        "</html>\n" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool webViewer::checkFeatureEnabled( const QString& feature ) const
{
    LOG_CLASS_INFO( "webViewer", "checkFeatureEnabled" );

    UiUtilities *util( mainWindow_->session()->uiUtilities );
    bool enabled( false );

    if ( pdfFeatureKey == feature )
        enabled = util->isFeatureLicensed( SCALE_PDF_VIEWER, true );
    else if ( docConversionFeatureKey == feature )
        enabled = util->isFeatureLicensed( SCALE_OFFICE_VIEWER, true );

    if ( !enabled )
        LOG_INFO( "feature '%s' is DISABLED!", qPrintable( feature ) );

    return enabled;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool webViewer::checkFeatureInstalled( const QString& feature ) const
{
    LOG_CLASS_INFO( "webViewer", "checkFeatureInstalled" );

    // PDF VIEWER
    if ( pdfFeatureKey == feature )
    {
#if defined( Q_OS_WIN )

        // Windows
        // look for installed ghostscript fonts
        if ( !QFile::exists( "C:/Program Files/Hobart/features/pdf_viewer/fonts/README" ) )
        {
            LOG_WARN( "Windows ghostscript fonts not found!", NULL );
            return false;
        }

#elif defined( Q_OS_LINUX )

        // linux
        // look for installed ghostscript fonts
        if ( !QFile::exists( "/usr/local/hobart/features/pdf_viewer/fonts/README" ) )
        {
            LOG_WARN( "ghostscript fonts not found!", NULL );
            return false;
        }

#endif
    }

    // MS DOC CONVERSION
    else if ( docConversionFeatureKey == feature )
    {
#if defined( Q_OS_WIN )

        // Windows
        // check for libreoffice conversion
        if ( !QFile::exists( "C:/Program Files/LibreOffice 4/program/soffice.exe" ) )
        {
            LOG_WARN( "Windows conversion tool not found!", NULL );
            return false;
        }

#elif defined( Q_OS_LINUX )

        // linux
        // check for libreoffice conversion
        if ( !QFile::exists( "/usr/local/bin/libreofficedev4.4" ) )
        {
            LOG_WARN( "conversion tool not found!", NULL );
            return false;
        }

#endif
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewer::checkLicensing()
{
    LOG_CLASS_INFO( "webViewer", "checkLicensing" );

    allowedDocSuffixList.clear();

    allowPortableDocumentFormat_ = true;
    allowMicrosoftOfficeExtensions_ = true;

    //
    // Test for support of Microsoft Office Documents/Spreadsheets/Etc
    //

#if defined( Q_OS_WIN )

    LOG_TRACE( "detected Windows platform", NULL );

    // Windows
    // assume we have conversion tool... for now...

#elif defined( Q_OS_LINUX )

    LOG_TRACE( "detected linux platform", NULL );

    // linux
    // assume we have conversion tool... for now...

#endif

    LOG_INFO( "pdf docs %s allowed", allowPortableDocumentFormat_ ? "ARE" : "ARE NOT" );
    LOG_INFO( "microsoft office docs %s allowed", allowMicrosoftOfficeExtensions_ ? "ARE" : "ARE NOT" );

    //
    // Update allowed formats with licensing information
    //

    // When PDF is allowed these extenstions are okay...
    if ( allowPortableDocumentFormat_ )
    {
        // check licensing
        if ( checkFeatureEnabled( pdfFeatureKey ) )
            allowedDocSuffixList.append( pdfSuffixList );
    }

    // When Microsoft Office is allowed these extensions are okay too...
    if ( allowMicrosoftOfficeExtensions_ )
    {
        // check licensing
        if ( checkFeatureEnabled( docConversionFeatureKey ) )
            allowedDocSuffixList.append( msofficeSuffixList );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void webViewer::createLayout()
{
    const QSize qIconSize( QSize( iconSize_, iconSize_ ) );

    // re-parent widgets on toolbar
    ui->homeButton->setParent( toolbar_ );
    ui->backButton->setParent( toolbar_ );
    ui->forwardButton->setParent( toolbar_ );
    ui->reloadButton->setParent( toolbar_ );
    ui->zoomButton->setParent( toolbar_ );
    ui->printWebButton->setParent( toolbar_ );
    ui->raiseKeyboard->setParent( toolbar_ );
    ui->currentURL->setParent( toolbar_ );
    ui->doneButton->setParent( toolbar_ );

    // destroy existing layout
    if ( layout() )
        delete layout();

    QHBoxLayout *toolbarLayout( new QHBoxLayout( toolbar_ ) );
    toolbarLayout->setContentsMargins( QMargins( 8, 8, 8, 8 ) );
    toolbarLayout->setSpacing( 5 );
    toolbarLayout->addWidget( ui->homeButton );
    toolbarLayout->addWidget( ui->backButton );
    toolbarLayout->addWidget( ui->forwardButton );
    toolbarLayout->addWidget( ui->reloadButton );
    toolbarLayout->addWidget( ui->zoomButton );
    toolbarLayout->addWidget( ui->printWebButton );
    toolbarLayout->addWidget( ui->raiseKeyboard );
    toolbarLayout->addWidget( ui->currentURL );
    toolbarLayout->addWidget( ui->doneButton );

    screenLayout_ = new QVBoxLayout( this );
    screenLayout_->setContentsMargins( QMargins() );
    screenLayout_->setSpacing( 0 );
    screenLayout_->addWidget( toolbar_ );
    screenLayout_->addWidget( ui->line );
    screenLayout_->addWidget( ui->webView );
    screenLayout_->addWidget( ui->progressBar );

    //quick fix for larger printer icon to be scaled correctly to the button
    ui->printWebButton->setIcon(QIcon(":/images/widgets/printerIconBlack.png"));
    ui->printWebButton->setIconSize(qIconSize);
    ui->backButton->setIconSize(qIconSize);
    ui->forwardButton->setIconSize(qIconSize);
    ui->reloadButton->setIconSize(qIconSize);
    ui->zoomButton->setIconSize(qIconSize);
    ui->raiseKeyboard->setIconSize(qIconSize);
    ui->doneButton->setIconSize(qIconSize);
    ui->homeButton->setIconSize(qIconSize);
    ui->currentURL->setFixedHeight(iconSize_);

    //qDebug() << ui->printWebButton->size().height() << ui->printWebButton->size().width();
    //qDebug() << ui->homeButton->size().height() << ui->homeButton->size().width();
}

int webViewer::getDept(void)
{
    LOG_CLASS_INFO( "webViewer", "getDept" );

    if ( !onlineOrderingOK() )
        return -1;

    return session_->scaleConfig->currentDept.deptNum();
}

void webViewer::swipeLeft(void)
{
    mainWindow_->swipeTheScreen( ScreenWidget::Left );
}

void webViewer::swipeRight(void)
{
    mainWindow_->swipeTheScreen( ScreenWidget::Right );
}

int webViewer::getZone(void)
{
  LOG_CLASS_INFO( "webViewer", "getZone" );
  
  int zone = session_->scaleConfig->scaleConfig.onlineOrderingZone();

  LOG_WARN("current zone: %d", zone);

  return zone;
}

bool webViewer::getIsOperatorLoggedIn(void)
{
    LOG_CLASS_INFO( "webViewer", "getIsOperatorLoggedIn" );

    if ( !onlineOrderingOK() )
        return false;

    return session_->scaleConfig->getIsOperatorLoggedIn();
}

QString webViewer::getLoggedOperatorId(void)
{
    LOG_CLASS_INFO( "webViewer", "getLoggedOperatorId" );

    if ( !onlineOrderingOK() )
        return "";

    return session_->scaleConfig->getLoggedOperatorId();
}

int webViewer::getLoggedOperatorAccess(void)
{
    LOG_CLASS_INFO( "webViewer", "getLoggedOperatorAccess" );

    if ( !onlineOrderingOK() )
        return 0;

    return session_->scaleConfig->getLoggedOperatorAccess();
}

bool webViewer::onlineOrderingOK()
{
    LOG_CLASS_INFO( "webViewer", "onlineOrderingOK" );

    if( !session_->uiUtilities->isFeatureLicensed( SCALE_ONLINE_ORDERING ) )
    {
        LOG_DEBUG( "webViewer::getIsOperatorLoggedIn() - error SessionDevOnlineOrderingActive 100062", NULL );
        session_->send( Error, ErrSessionDevice, QString::number(SessionDevError::SessionDevOnlineOrderingActive) + ",5" );
        return false;
    }
    bool ok = session_->scaleConfig->scaleConfig.onlineOrderingEnabled();
    if( !ok /*session_->scaleConfig->scaleConfig.onlineOrderingEnabled()*/ )
    {
        LOG_DEBUG( "webViewer::runJavaScript() - error SessionDevOnlineOrderingEnabled 100063", NULL );
        session_->send( Error, ErrSessionDevice, QString::number(SessionDevError::SessionDevOnlineOrderingEnabled) + ",5" );
        return false;
    }
    return true;
}

void webViewer::noSwipe(void)
{
  mainWindow_->cancelActiveGesture();
}

void webViewer::printOrderReport(QString orderNumber, QString customerName, QString itemCount)
{
  QString largeFont =  QString("\x001B") + "\x001B" + "fn=Hobart;ps=24" + "\x001B";
  QString mediumFont = QString("\x001B") + "\x001B" + "fn=Hobart;ps=14" + "\x001B";
  QString smallFont =  QString("\x001B") + "\x001B" + "fn=Hobart;ps=8" + "\x001B";
  QString tinyFont = QString("\x001B") + "\x001B" + "fn=Hobart;ps=5" + "\x001B";

  QString newline("\x000A");

  QDateTime now(QDateTime::currentDateTimeUtc().toLocalTime());
  
  // print a tiny space to work around a bug(?) in the backend(?)
  // without a space first, the first two characters print on top of each other
  QString labelData(tinyFont + " " + largeFont + orderNumber + newline
                    + mediumFont + customerName + newline + newline
                    + smallFont + "Printed: " + now.time().toString("h:mm AP") + newline);

  session_->send( ReportManagerDev, StartReport, QString("2,0,0,0,0,,\"") + labelData.remove("\"") + "\"" ); // 116,0
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace


