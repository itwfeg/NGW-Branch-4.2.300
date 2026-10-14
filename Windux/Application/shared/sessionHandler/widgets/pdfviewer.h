/**
 * @file pdfViewer.h
 * PDF viewer class based on Xpdf/QtWidget 3RD party Qt widget
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Scott Davis
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef PDFVIEWER_H
#define PDFVIEWER_H

#include "../sessionHandler/sessionmainwindow.h"
#include "../screens/screenwidget.h"

#include "../widgets/printPrompt.h"

#include <xpdf-qt/XpdfWidget.h>

#include <QWidget>

namespace Ui {
class pdfviewer;
}

/// Hobart namespace.
namespace hobart {

class pdfviewer : public QWidget
{
    Q_OBJECT
    typedef pdfviewer _Myt;
    typedef QWidget _Mybase;

signals:
    void pdfViewerDoneClicked();

public slots:
    void done();

private slots:
    void onPrevPageClicked();
    void onNextPageClicked();
    void onPrintPdfClicked();
    void onHomeClicked();
    void onZoomButtonClicked();
    void onZoomChanged(int value);
    void onZoomReleased();
    void onMidPageChange(int pageNum);
    void onPrintPromptDoneClicked( int promptIndex );

public:
    /**
     * Constructor.
     *
     * @param[in] parent  parent widget
     */
    explicit pdfviewer( QWidget *parent = 0, bool displayAsDialog=false, SessionMainWindow *sessionMainWindow=0 );
    ~pdfviewer();
    void translate();
    bool loadPdf(QString fileName);
    bool loadPdf(QByteArray pdfData);

private:
    Ui::pdfviewer *ui;
    SessionMainWindow *mainWindow_;
    Session *session_;      ///< Pointer to the session.

    hobart::printPrompt *printerPrompter_;

    void emitDoneClicked();
    void setToolbarState();
    XpdfWidget xPdfView;
    QString pdfFileNameToPrint;

    int howManyNetworkPrintersAttached();
    void sendPdfPagePrintMsg(int printerNumber);

    enum PrinterModel
    {
        NoPrinter,                          // 0
        EppPrinter,                         // not used, no parts for this EPP - EOL
        EssPrinter,                         // not used
        EscPrinter,                         // not used
        ApcPrinter,                         // not used
        Epp2Printer,                        // 5 This is the EPP 2nd printer will use
        UsbPrinter,                         // 6
        NetworkPrinter                      // 7
    };
};

}

/** @} */

#endif // PDFVIEWER_H
