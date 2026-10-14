/**
 * @file webviewer.h
 * Web view class with more control than the standard one.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Scott Davis
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef WEBVIEWER_H
#define WEBVIEWER_H

#include <filedownloader.h>
#include "../sessionmainwindow.h"
#include <QWebElement>
#include <QWebView>
#include <QWidget>

#include "../widgets/printPrompt.h"

class SessionMainWindow;

class QHBoxLayout;
class QTimer;
class QVBoxLayout;
class QWebElement;
class QWebView;
class QWebViewSelectionSuppressor;

#define EMPTY_URL "http://"

namespace Ui {
class webViewer;
}

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class webViewer
 * @brief Web viewer class with more control than the standard one.
 */
class webViewer : public QWidget
{
    Q_OBJECT

    typedef webViewer _Myt;
    typedef QWidget _Mybase;

signals:
    void webViewerLinkClicked( QString url );
    void webViewerVideoLinkClicked( QString url );
    void webViewerPdfLinkClicked( QString url );
    void webViewerDoneClicked();

public:

    ///////////////////////////////////////////////////////////////////////////
    // CTOR / DTOR

    /**
     * Constructor.
     *
     * @param[in] parent  parent widget
     */
    explicit webViewer( QWidget *parent = 0, bool displayAsDialog=false, bool toolBarVisible=true, SessionMainWindow *sessionMainWindow=0, bool showLoadProgress=true );

    /**
     * Destructor.
     */
    virtual ~webViewer();
    
    void setBrowserName(const QString &name);
    void setURL(const QString &url, bool force = false);
    void setHtml(const QString &htmlText);
    void translate();
    void scroll( int dx, int dy );
    void setUrlVisible(bool yes, bool editable);
    void setBrowserNameVisible(bool yes);
    void setHomeUrl(const QString &url);
    QWebElement findFormInputWithName(QString name);
    void clear();
    void dismiss();
    QString getWebElementName(QWebElement element);
    void click(const QString &selectorQuery);
    void loadTestPage();
    void setDisplayAsDialog(bool value);
    void setToolBarVisible(bool value);
    void setFocusOnNavigate(bool value) {focusOnNavigate = value;}
    void runJavaScript(QString &command);
    bool onlineOrderingOK();

    QWebView *webView() const;

protected:

    void setURLInternal(const QUrl &url, bool force = false);

    void resizeEvent(QResizeEvent *e);

//public Q_SLOTS:
public slots:
    /**
     * Slot for when link is clicked.
     */
    void onLinkClicked( const QUrl &url );
    void done();

    void pdfDownloaded( bool success );
    void convertToPdfDownloaded( bool success );
    void videoDownloaded( bool success );

    void updateDownloadProgress( int value );

    /**
      must be public for javascript access however should not
      be called by other classes
      */
    void handleFocusIn();
    void handleFocusOut();
    
    Q_INVOKABLE void jsSwipeAndLookupPlu(QString name, QString urlStr);
    
    Q_INVOKABLE void lookupPLU(QString plu, QString customerName, QString pickupTime, QString orderNumber, QString orderNumId, QString byCount);
    Q_INVOKABLE void print(void);

    Q_INVOKABLE void setMainURL(QString URL);
    Q_INVOKABLE void setEnhancedURL(QString URL);

    Q_INVOKABLE void swipeLeft(void);
    Q_INVOKABLE void swipeRight(void);
    
    Q_INVOKABLE int getDept(void);
    Q_INVOKABLE int getZone(void);

    Q_INVOKABLE bool getIsOperatorLoggedIn(void);
    Q_INVOKABLE QString getLoggedOperatorId(void);
    Q_INVOKABLE int getLoggedOperatorAccess(void);
    Q_INVOKABLE void noSwipe(void);
    
    Q_INVOKABLE void printOrderReport(QString orderNumber, QString customerName, QString itemCount);

private slots:
    void internalFocusChanged();

    void onUrlChanged(const QUrl &url);
    void onLoadStarted();
    void onLoadProgress(int progress);
    void onLoadFinished(bool ok);
    void onLoadTimeout();

    void populateJavaScriptWindowObject();
    void onUrlEntered();
    void onHomeClicked();
    void onZoomButtonClicked();
    void onZoomChanged(int value);
    void onZoomReleased();
    void onRaiseKeyboard();
    void onDownload( const QNetworkRequest& request );
    void onUnsupportedContent( QNetworkReply *reply );
    void onPrintWebClicked();
    void onPrintPromptDoneClicked( int promptIndex );

private:

    enum {PAGE_LOAD_TIMEOUT = 45000}; // timeout for page load (in milliseconds)

    Ui::webViewer *ui;

    QString testHtml;
    QUrl homeUrl_;
    QString webFileNameToPrint;

    QTimer *pageLoadTimer_;
    bool showProgress;
    bool focusOnNavigate;
    bool loadingDoneOkToPrint;
    bool isDialog_;

    SessionMainWindow *mainWindow_;
    Session *session_;

    hobart::printPrompt *printerPrompter_;

    void emitDoneClicked();
    void emitLinkClicked( const QString &url );
    void emitVideoLinkClicked( const QString &url );
    void emitPdfLinkClicked(const QString &url );

    void setFocusFirstTextInput();
    bool eventFilter( QObject *obj, QEvent *e );

    QWebElement findFocusedPageInput();
    void maybeShowKeyboard(QWebElement inputElement);
    QWebViewSelectionSuppressor *suppressor;

    QWidget *toolbar_;
    QVBoxLayout *screenLayout_;

    int iconSize_;
    void setToolbarState();
    void createLayout();
    bool downloadFile( const QUrl& url );

    bool convertToPdf( const QString& oldFile, const QString& newFile );

    void showFeatureNotSupportedDialog( const QString& feature );
    void showFeatureNotInstalledDialog( const QString& feature );
    void showDocumentConversionFailedDialog();

    bool checkFeatureEnabled( const QString& feature ) const;
    bool checkFeatureInstalled( const QString& feature ) const;
    int howManyNetworkPrintersAttached();
    void sendWebPagePrintMsg(int printerNumber);
    void checkLicensing();

    QList<QString> videoSuffixList;
    QList<QString> documentSuffixList;
    QList<QString> htmlSuffixList;
    QList<QString> imageSuffixList;

    QList<QString> pdfSuffixList;
    QList<QString> msofficeSuffixList;
    QList<QString> allowedDocSuffixList;

    FileDownloader *fileDownloader_;
    double fileDownloadProgressMultiplier_;
    bool fileDownloadInProgress_;

    bool allowPortableDocumentFormat_;
    bool allowMicrosoftOfficeExtensions_;

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

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

/** @} */

#endif // WEBVIEWER_H
