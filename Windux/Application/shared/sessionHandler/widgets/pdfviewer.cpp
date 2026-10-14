#include <logmanager.h>

#include "pdfviewer.h"
#include "ui_pdfviewer.h"
#include "../sessionHandler/sessionmainwindow.h"
#include "../sessioninfo.h"
#include <Messages.h>
#include <ScaleConfiguration.h>

#include "../platform/platform.h"
#include <QVBoxLayout>
#include <QDebug>

/// Hobart namespace
namespace hobart {

pdfviewer::pdfviewer( QWidget *parent, bool displayAsDialog, SessionMainWindow *mainWindow ) :
    _Mybase( parent ),
    ui(new Ui::pdfviewer)
{
    ui->setupUi(this);

    // Connect into session
    session_ = mainWindow->session();
    printerPrompter_ = NULL;                    // access to network printer dialog

    setPalette( QApplication::palette() );
    setAutoFillBackground( true );

    mainWindow_ = mainWindow;
    pdfFileNameToPrint = "";
    ui->homeButton->setFocusPolicy(Qt::NoFocus);
    ui->prevPageButton->setFocusPolicy(Qt::NoFocus);
    ui->nextPageButton->setFocusPolicy(Qt::NoFocus);
    ui->printPdfButton->setFocusPolicy(Qt::NoFocus);
    ui->doneButton->setFocusPolicy(Qt::NoFocus);
    ui->printPdfButton->setIcon(QIcon(":/images/widgets/printerIconBlack.png"));
    QSize qIconSize = QSize(32, 32);
    ui->printPdfButton->setIconSize(qIconSize);
    ui->printPdfButton->setFixedSize(qIconSize);
    ui->zoomSlider->hide();

    if (displayAsDialog)
    {
        connect(ui->doneButton,SIGNAL(clicked()),this,SLOT(done()));
    }
    else
    {
        // TODO
        // Need to use screen constants for width/height of screen versus hardwiring values
        // display full screen
        // Change to layout
        ui->xPdfView->resize( (mainWindow_->width() * 1005) / SessionMainWindow::scalingDividerX_, (mainWindow_->height() * 515) / SessionMainWindow::scalingDividerY_ );
        ui->xPdfView->update();

        ui->doneButton->hide();
    }

    ui->xPdfView->setContinuousMode(true);

    // TODO:
    // PDF Viewer currently does not support selecting hyperlinks in PDF document
    // that link to a URL
    // Could be added but I have heard no one state it is required
    ui->xPdfView->enableHyperlinks(false);

    ui->xPdfView->enablePan(true);
    ui->xPdfView->enableSelect(false);

    connect(ui->prevPageButton,SIGNAL(clicked()),this,SLOT(onPrevPageClicked()));
    connect(ui->nextPageButton,SIGNAL(clicked()),this,SLOT(onNextPageClicked()));
    connect(ui->printPdfButton,SIGNAL(clicked()),this,SLOT(onPrintPdfClicked()));
    connect(ui->homeButton,    SIGNAL(clicked()),this,SLOT(onHomeClicked()));
    connect(ui->zoomButton,    SIGNAL(clicked()),this,SLOT(onZoomButtonClicked()));
    connect(ui->zoomSlider,    SIGNAL(valueChanged(int)),  this, SLOT(onZoomChanged(int)));
    connect(ui->zoomSlider,    SIGNAL(sliderReleased()),   this, SLOT(onZoomReleased()));
    connect(ui->xPdfView,      SIGNAL(midPageChange(int)), this, SLOT(onMidPageChange(int)));

    setToolbarState();
}

bool pdfviewer::loadPdf(QByteArray pdfData)
{
    LOG_CLASS_INFO( "pdfviewer", "loadPdf" );

    XpdfWidget::ErrorCode err;

    if (!pdfData.isEmpty())
    {
        if ((err = ui->xPdfView->loadMem(pdfData, pdfData.length())) != XpdfWidget::pdfOk)
        {
            LOG_WARN( "empty pdf memory block to display or erroneous data", NULL );
            return false;
         }
    }
    else
        return false;

    // zoom so that the current pages(s) fill the window width
    ui->xPdfView->zoomToCurrentWidth();
    ui->xPdfView->gotoFirstPage();

    return true;
}

bool pdfviewer::loadPdf(QString fileName)
{
    LOG_CLASS_INFO( "pdfviewer", "loadPdf" );
    XpdfWidget::ErrorCode err;

    LOG_INFO( "loadPdf filename is '%s'", fileName.toLatin1().constData() );

    if (!fileName.isEmpty())
    {
        pdfFileNameToPrint = fileName;
        if ((err = ui->xPdfView->loadFile(QString(fileName))) != XpdfWidget::pdfOk)
        {
            LOG_WARN( "invalid pdf filename location or format", NULL );
            return false;
        }
    }
    else
    {
        pdfFileNameToPrint = "";
        return false;
    }
    // zoom so that the current pages(s) fill the window width
    ui->xPdfView->zoomToCurrentWidth();
    ui->xPdfView->gotoFirstPage();

    return true;
}

pdfviewer::~pdfviewer()
{
    if( printerPrompter_ )
        delete printerPrompter_;
        
    delete ui->xPdfView;
    delete ui;
}

void pdfviewer::onPrevPageClicked()
{
    ui->xPdfView->gotoPreviousPage();
}

void pdfviewer::onNextPageClicked()
{
    ui->xPdfView->gotoNextPage();
}

void pdfviewer::onPrintPdfClicked()
{
#ifndef QT_DEBUG
    // allow only for primary session
    if ( !mainWindow_->isPrimarySessionType() )
        return;
#endif

    if ( !pdfFileNameToPrint.length() )
    {
        session_->send( Error, ErrPrinterDevice, "7561,5" );
        return;
    }

    // how many do we have?  Any? Can only print to a network printer but not a zebra variety
    int networkedPrinters = howManyNetworkPrintersAttached();

    if( networkedPrinters > 0 )
    {
        ScaleConfiguration *config     = session_->scaleConfig;
        DomPrWriteAllExternalPrinter   *externalPrinters = &config->allExternalPrinters;

        if ( 2 == networkedPrinters )
        {
            // OK so we have a 2 network printers so we need to prompt which one to print on.
            QStringList printerNumsList    = externalPrinters->extPrinterNumList().split(",");        // list of printer numbers
            QStringList printerNamesList   = externalPrinters->extPrinterNamesList().split(",");      // list of printer names
            QStringList printerTypesList   = externalPrinters->extPrinterTypeList().split(",");
            QStringList printerDriversList = externalPrinters->extPrinterDriverList().split(",");

            // Then prompt the user which network printer to print to
            printerPrompter_ = new hobart::printPrompt(this, printerNumsList, printerNamesList, printerTypesList, printerDriversList, mainWindow_);
            printerPrompter_->move( (mainWindow_->width() * 275) / SessionMainWindow::scalingDividerX_, (mainWindow_->height() * 200) / SessionMainWindow::scalingDividerY_ );
            printerPrompter_->show();

            connect(printerPrompter_, SIGNAL( printPromptDoneClicked( int ) ),    SLOT( onPrintPromptDoneClicked( int )) );
        }
        else
        {
            // We know we have a network printer to print on, so now render the web page to print
            QStringList printerNumsList    = externalPrinters->extPrinterNumList().split(",");        // list of printer numbers
            int printerNumber = printerNumsList.at(0).toInt();
            sendPdfPagePrintMsg( printerNumber );
        }
    }
    else
    {
        // OK so we dont have a network printer so tell the user we cant print it.
        session_->send( Error, ErrPrinterDevice, "7559,5" );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void pdfviewer::onPrintPromptDoneClicked( int promptIndex )
{
    // Get the correct prompted network printer and call a method
    // that sends the message to the backend, a method everyone can use.

    sendPdfPagePrintMsg( promptIndex );
}

int pdfviewer::howManyNetworkPrintersAttached()
{
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

    if ( numOfPrinterTypes > 0 )
    {
        printerTypeOne   = printerTypeList.at(0).toInt();
        printerDriverOne = printerDriverList.at(0);

        if ( numOfPrinterTypes == 2)
        {
            printerTypeTwo   = printerTypeList.at(1).toInt();
            printerDriverTwo = printerDriverList.at(1);
        }

        if( printerTypeOne == NetworkPrinter && !printerDriverOne.contains("zebra", Qt::CaseInsensitive) )
            numOfNetworkPrintersAttached = 1;

        if( printerTypeTwo == NetworkPrinter && !printerDriverTwo.contains("zebra", Qt::CaseInsensitive) )
            numOfNetworkPrintersAttached = 2;
    }

    return numOfNetworkPrintersAttached;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void pdfviewer::sendPdfPagePrintMsg( int printerNumber )
{
    LOG_CLASS_INFO( "pdfviewer", "sendPdfPagePrintMsg" );
     
    if( printerNumber > 0 )
    {
        LOG_DEBUG("printHtml successfully called", NULL);

        // 2nd printer NOTE: On a windows machine, the pdfFileNameToPrint will look like
        // Windows:
        // C:/Users/pevoala/AppData/Local/Temp/somePdfFileSavedInCacheFromWeb.pdf
        // LINUX:
        // dont know
        QString printArgs = QString::number(printerNumber) + "," + pdfFileNameToPrint;

        session_->send( Printer, PrPrintPdfDocument, printArgs );

        session_->send( Error, ErrPrinterDevice, "7562,5" );

    }
    else
    {
        session_->send( Error, ErrPrinterDevice, "7559,5" );
    }
}

void pdfviewer::onZoomChanged(int value)
{
    double zoomValue = (value * 50.0) + 100.0;
    ui->xPdfView->setZoom(zoomValue);
}

void pdfviewer::onZoomButtonClicked()
{
    if (ui->zoomSlider->isHidden())
        ui->zoomSlider->show();
    else
        ui->zoomSlider->hide();
}

void pdfviewer::onZoomReleased()
{
    ui->zoomSlider->hide();
}

void pdfviewer::setToolbarState()
{

    if (ui->xPdfView->getCurrentPage() == ui->xPdfView->getNumPages())
        ui->nextPageButton->setDisabled(true);
    else
        ui->nextPageButton->setDisabled(false);

    if (ui->xPdfView->getCurrentPage() == 1)
        ui->prevPageButton->setDisabled(true);
    else
        ui->prevPageButton->setDisabled(false);


    ui->xPdfView->setZoom(1.0);
    ui->zoomSlider->setValue(1);
    ui->zoomSlider->hide();
}

void pdfviewer::done()
{
    emitDoneClicked();
}

void pdfviewer::onHomeClicked()
{
    ui->xPdfView->gotoFirstPage();
}

void pdfviewer::translate()
{
    ui->retranslateUi(this);
    update();
}

void pdfviewer::emitDoneClicked()
{
    emit pdfViewerDoneClicked();
}

void pdfviewer::onMidPageChange(int pageNum)
{
    QString pageString;
    pageString = QString::number(pageNum);

    ui->pageNumLabel->setText(pageString);
}


}  // hobart namespace
