/**
 * @file queuesignagewidget.h
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef QUEUESIGNAGEWIDGET_H
#define QUEUESIGNAGEWIDGET_H

#include <QMap>
#include <QString>
#include <QWidget>
#include <QWebView>
#include <QNetworkReply>

#include <mainwindow.h>

#include <../playlistWidget/playlistwidget.h>

class currentlyServingWidget;
class serverComm;

class MarqueeLabelWidget;

class QLabel;
class QStackedWidget;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Widget for digital queue signage.
class queueSignageWidget : public QWidget
{
    Q_OBJECT

    typedef queueSignageWidget _Myt;
    typedef QWidget _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] params  configuration
     * @param[in] parent  parent widget
     */
    queueSignageWidget( const QMap<QString, QString>& params, mainWindow::modeType mode, QWidget *parent = 0 );

    /// Destructor.
    virtual ~queueSignageWidget();

    // ========================================================================
    // Methods
    // ========================================================================

    /// Translate widgets.
    virtual void translate();

protected:

    // ========================================================================
    // Events
    // ========================================================================

    /// Resize event.
    /**
     * @param[in,out] e  event
     */
    virtual void resizeEvent( QResizeEvent *e );

private Q_SLOTS:

    /// Slot for heartbeat.
    void onHeartbeat();

    void handleSslErrors(QNetworkReply* reply, const QList<QSslError> &errors);

private:

    QMap<QString, QString> params_;

    mainWindow::modeType thisMode;

    serverComm *server_;

    QTimer *heartbeat_;

    QWidget *logo_;
    QLabel *dept_;

    QStackedWidget *screens_;
    currentlyServingWidget *currentlyServing_;
    QWidget *adverts_;

    // SOF-4529 - Support multiple layouts for Online Ordering
    playlistWidget *playList1_;
    playlistWidget *playList2_;
    playlistWidget *playList3_;

    QWebView *qWebView1_;
    QWebView *qWebView2_;
    QWebView *qWebView3_;

    QWidget *regionA_Widget;
    QWidget *regionB_Widget;
    QWidget *regionC_Widget;

    playlistWidget *createPlayListWidget( QString playListName );
    QWebView *createWebViewWidget( QString defaultUrl );


    MarqueeLabelWidget *marquee_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

    // SOF-4529 - Support multiple layouts for Online Ordering
    QString myLayout;
    QString regionA;
    QString regionB;
    QString regionC;

    QString urlRegionA;
    QString urlRegionB;
    QString urlRegionC;

    QString playlistRegionA;
    QString playlistRegionB;
    QString playlistRegionC;

    // Used to retrieve the config values from the 'settings' table
    QString getTvLayoutSettings( QString settingsValue );
    void retrieveAllTvLayoutSettings();

    QString strKioskTextColor;
    QString strKioskLightColor;
    QString strTvMarqueeText;
    QString strTvLayout;

    QString strTvRegionA;
    QString strTVRegionAURL;
    QString strTVRegionAPlaylist;

    QString strTvRegionB;
    QString strTVRegionBURL;
    QString strTVRegionBPlaylist;

    QString strTvRegionC;
    QString strTVRegionCURL;
    QString strTVRegionCPlaylist;
};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // QUEUESIGNAGEWIDGET_H
