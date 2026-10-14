/**
 * @file queuesignagewidget.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "currentlyservingwidget.h"
#include "queuesignagewidget.h"
#include "fontutil.h"
#include "pixmaputil.h"
#include "servercomm.h"

#include <marqueelabelwidget.h>

#include <QHBoxLayout>
#include <QLabel>
#include <QStackedWidget>
#include <QVBoxLayout>

#include <logmanager.h>
#include <mainwindow.h>
#include <QUrl>
#include <python.h>

///////////////////////////////////////////////////////////////////////////////////////////////////
queueSignageWidget::queueSignageWidget( const QMap<QString, QString>& params, mainWindow::modeType mode, QWidget *parent ) :
    _Mybase( parent ),
    params_( params ),
    thisMode( mode )
{
    // init
    initialize();
    createLayout();
    translate();

    // setup comm
    server_ = serverComm::instance();

    // setup timer for heartbeat
    heartbeat_ = new QTimer( this );
    heartbeat_->setSingleShot( false );

    connect( heartbeat_, SIGNAL( timeout() ), SLOT( onHeartbeat() ) );

    heartbeat_->start( 300000 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
queueSignageWidget::~queueSignageWidget()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void queueSignageWidget::translate()
{
    if( currentlyServing_ )
        currentlyServing_->translate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void queueSignageWidget::resizeEvent( QResizeEvent *e )
{
    if( mainWindow::signage == thisMode )
    {
        //--------
        // TV mode
        //--------

        // adjust header size
        logo_->setFixedHeight( height() / 6 );
        dept_->setFixedHeight( height() / 6 );

        // adjust footer size
        marquee_->setFixedHeight( height() / 12 );

        // adjust font sizes
        QFont f( font() );

        f.setPointSize( fontUtil::calcFontPointSize( f, QSize( dept_->width(), dept_->height() / 2 ), dept_->text() ) );
        dept_->setFont( f );

        f.setPointSize( fontUtil::calcFontPointSize( f, marquee_->size(), "X" ) );
        marquee_->setFont( f );

        // adjust pixmap sizes
        QPalette p;

        p = palette();
        p.setBrush( backgroundRole(), pixmapUtil::scalePixmap( QPixmap( "res/background.png" ), size() ) );
        setPalette( p );

        p = logo_->palette();
        p.setBrush( logo_->backgroundRole(), pixmapUtil::scalePixmap( QPixmap( "res/storeLogo.png" ), logo_->size(), Qt::KeepAspectRatio ) );
        logo_->setPalette( p );

        _Mybase::resizeEvent( e );
    }
    else
    {
        //---------------------
        // Online Ordering mode
        //---------------------

        // adjust footer size
        marquee_->setFixedHeight( height() / 12 );

        // adjust font sizes
        QFont f( font() );

        f.setPointSize( fontUtil::calcFontPointSize( f, marquee_->size(), "X" ) );
        marquee_->setFont( f );

        // adjust pixmap sizes
        QPalette p;

        p = palette();
        p.setBrush( backgroundRole(), pixmapUtil::scalePixmap( QPixmap( "res/background.png" ), size() ) );
        setPalette( p );

        _Mybase::resizeEvent( e );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void queueSignageWidget::onHeartbeat()
{
#if defined( Q_OS_LINUX )
    system( qPrintable( "ping -c 8 " + server_->server() ) );
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void queueSignageWidget::initialize()
{
    LOG_CLASS_INFO( "queueSignageWidget", "initialize");

    regionA_Widget = NULL;
    regionB_Widget = NULL;
    regionC_Widget = NULL;

    currentlyServing_ = NULL;

    if( mainWindow::signage == thisMode )
    {
        //-------------------
        // TV mode (standard)
        //-------------------

        QPalette p;

        setAutoFillBackground( true );

        // logo
        logo_ = new QWidget( this );
        logo_->setAutoFillBackground( true );

        // dept
        dept_ = new QLabel( this );
        dept_->setAlignment( Qt::AlignCenter );
        dept_->setAutoFillBackground( true );
        dept_->setText( params_["dept"] );

        p = dept_->palette();
        p.setBrush( dept_->backgroundRole(), Qt::black );
        p.setBrush( dept_->foregroundRole(), Qt::white );
        dept_->setPalette( p );

        // queue screen
        currentlyServing_ = new currentlyServingWidget();
        currentlyServing_->setNowServingText( params_["serving"] );
        currentlyServing_->setPlaylist( params_["playlist"] );
        currentlyServing_->setQueueSizeText( params_["queuesize"] );
        currentlyServing_->setShowQueueSize( "1" == params_["showqueuesize"] );

        // advert screen
        adverts_ = new QWidget();

        // screens
        screens_ = new QStackedWidget( this );
        screens_->addWidget( currentlyServing_ );
        screens_->addWidget( adverts_ );

        // marquee
        marquee_ = new MarqueeLabelWidget( this );
        marquee_->setAutoFillBackground( true );
        marquee_->setText( params_["marquee"] );

        p = marquee_->palette();
        p.setBrush( marquee_->backgroundRole(), Qt::black );
        p.setBrush( marquee_->foregroundRole(), Qt::white );
        marquee_->setPalette( p );
    }
    else if ( mainWindow::onlineOrdering == thisMode )
    {
        //---------------------
        // Online Ordering mode
        //---------------------

        QPalette p;

        //----------------------------------------
        // Get the configs from the settings table
        //----------------------------------------
        retrieveAllTvLayoutSettings();

        setAutoFillBackground( true );

        // Regions: P=Playlist, N=Now Serving; W=Web page ??? is "Now Serving" option dead??? <<<<<<<<<<<<<<<<<<<<<
        myLayout = strTvLayout;
        regionA = strTvRegionA;
        regionB = strTvRegionB;
        regionC = strTvRegionC;

        LOG_DEBUG( "initialize(): myLayout=%s", qPrintable( myLayout) );
        LOG_DEBUG( "initialize(): regionA=%s", qPrintable( regionA ) );
        LOG_DEBUG( "initialize(): regionB=%s", qPrintable( regionB ) );
        LOG_DEBUG( "initialize(): regionC=%s", qPrintable( regionC ) );

        // Get the default URL for the web pages
        if( regionA == "W" )
            urlRegionA = strTVRegionAURL;
        if( regionB == "W" )
            urlRegionB = strTVRegionBURL;
        if( regionC == "W" )
            urlRegionC = strTVRegionCURL;

        LOG_DEBUG( "initialize(): urlRegionA=%s", qPrintable( urlRegionA ) );
        LOG_DEBUG( "initialize(): urlRegionB=%s", qPrintable( urlRegionB ) );
        LOG_DEBUG( "initialize(): urlRegionC=%s", qPrintable( urlRegionC ) );

        // Get the playlist names
        if( regionA == "P" )
            playlistRegionA = strTVRegionAPlaylist;
        if( regionB == "P" )
            playlistRegionB = strTVRegionBPlaylist;
        if( regionC == "P" )
            playlistRegionC = strTVRegionCPlaylist;

        LOG_DEBUG( "initialize(): playlistRegionA=%s", qPrintable( playlistRegionA ) );
        LOG_DEBUG( "initialize(): playlistRegionB=%s", qPrintable( playlistRegionB ) );
        LOG_DEBUG( "initialize(): playlistRegionC=%s", qPrintable( playlistRegionC ) );

        //-----------------------------------
        // Create the widgets for each region
        //-----------------------------------
        if( regionA == "P" )
        {
            playList1_ = createPlayListWidget( playlistRegionA );
            regionA_Widget = playList1_;
        }
        else if( regionA == "W" )
        {
            qWebView1_ = createWebViewWidget( urlRegionA );
            regionA_Widget = qWebView1_;
        }


        if( myLayout.toInt() != 7 && myLayout.toInt() != 9 )
        {
            //--------------------------------
            // Only if the layout has region B
            //--------------------------------

            if( regionB == "P" )
            {
                playList2_ = createPlayListWidget( playlistRegionB );
                regionB_Widget = playList2_;
            }
            else if( regionB == "W" )
            {
                qWebView2_ = createWebViewWidget( urlRegionB );
                regionB_Widget = qWebView2_;
            }
        }

        if( myLayout.toInt() < 6 )
        {
            //--------------------------------
            // Only if the layout has region C
            //--------------------------------

            if( regionC == "P" )
            {
                playList3_ = createPlayListWidget( playlistRegionC );
                regionC_Widget = playList3_;
            }
            else if( regionC == "W" )
            {
                qWebView3_ = createWebViewWidget( urlRegionC );
                regionC_Widget = qWebView3_;
            }
        }

        // marquee
        marquee_ = new MarqueeLabelWidget( this );

        if( myLayout != "9" )
        {
            marquee_->setAutoFillBackground( true );
            marquee_->setText( strTvMarqueeText );

            p = marquee_->palette();

            // Set the marquee colors based on the settings
            p.setBrush( marquee_->backgroundRole(), QColor( strKioskLightColor ) );
            p.setBrush( marquee_->foregroundRole(), QColor( strKioskTextColor ) );

            marquee_->setPalette( p );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void queueSignageWidget::createLayout()
{
    LOG_CLASS_INFO( "queueSignageWidget", "createLayout");

    if ( layout() )
        delete layout();

    if( mainWindow::signage == thisMode )
    {
        //-------------------
        // TV mode (standard)
        //-------------------

        //  +----------+----------------------+
        //  |  Logo    |  Dept                |
        //  +----------+----------------------+
        //  |          |                      |
        //  |  Now     |   Playlist           |
        //  |  Serving |                      |
        //  |          |                      |
        //  |          |                      |
        //  +----------+----------------------+
        //  |  Marquee                        |
        //  +---------------------------------+

        QHBoxLayout *header( new QHBoxLayout );
        header->setContentsMargins( QMargins() );
        header->setSpacing( 0 );
        header->addWidget( logo_, 1 );
        header->addWidget( dept_, 2 );

        QHBoxLayout *footer( new QHBoxLayout );
        footer->setContentsMargins( QMargins() );
        footer->setSpacing( 0 );
        footer->addWidget( marquee_ );

        QVBoxLayout *formLayout( new QVBoxLayout( this ) );
        formLayout->setContentsMargins( QMargins() );
        formLayout->addLayout( header );
        formLayout->addWidget( screens_, 1 );
        formLayout->addLayout( footer );
    }
    else
    {
        //---------------------
        // Online Ordering mode
        //---------------------

        QHBoxLayout *layoutMarquee_( new QHBoxLayout );

        if( myLayout != "9" )
        {
            layoutMarquee_->setContentsMargins( QMargins() );
            layoutMarquee_->setSpacing( 0 );
            layoutMarquee_->addWidget( marquee_, 1 );
        }

        if( myLayout == "1" )
        {
            //  +---------------------------------+
            //  |             Marquee             |
            //  +---------------------------------+
            //  |                                 |
            //  |            Region A             |
            //  |                                 |
            //  +----------------+----------------+
            //  |                |                |
            //  |    Region B    |    Region C    |
            //  |                |                |
            //  +----------------+----------------+

            LOG_DEBUG( "createLayout(): Layout #1", NULL );

            QHBoxLayout *regionA( new QHBoxLayout );
            regionA->setContentsMargins( QMargins() );
            regionA->setSpacing( 0 );
            regionA->addWidget( regionA_Widget );

            LOG_DEBUG( "Added Region A", NULL );

            QHBoxLayout *regionB_and_C( new QHBoxLayout );
            regionB_and_C->setContentsMargins( QMargins() );
            regionB_and_C->setSpacing( 0 );
            regionB_and_C->addWidget( regionB_Widget, 1 );
            regionB_and_C->addWidget( regionC_Widget, 1 );

            LOG_DEBUG( "Added Region B and C", NULL );

            QVBoxLayout *formLayout( new QVBoxLayout( this ) );
            formLayout->setContentsMargins( QMargins() );
            formLayout->addLayout( layoutMarquee_ );
            formLayout->addLayout( regionA, 1 );
            formLayout->addLayout( regionB_and_C, 1 );

            LOG_DEBUG( "Added Regions to Form Layout #1", NULL );
        }
        else if( myLayout == "2" )
        {
            //  +---------------------------------+
            //  |             Marquee             |
            //  +----------------+----------------+
            //  |                |                |
            //  |                |    Region B    |
            //  |                |                |
            //  |    Region A    +----------------+
            //  |                |                |
            //  |                |    Region C    |
            //  |                |                |
            //  +----------------+----------------+

            LOG_DEBUG( "createLayout(): Layout #2", NULL );

            QHBoxLayout *regionA( new QHBoxLayout );
            regionA->setContentsMargins( QMargins() );
            regionA->setSpacing( 0 );
            regionA->addWidget( regionA_Widget );

            LOG_DEBUG( "Added Region A", NULL );

            QVBoxLayout *regionB_and_C( new QVBoxLayout );
            regionB_and_C->setContentsMargins( QMargins() );
            regionB_and_C->setSpacing( 0 );
            regionB_and_C->addWidget( regionB_Widget, 1 );
            regionB_and_C->addWidget( regionC_Widget, 1 );

            LOG_DEBUG( "Added Region B and C", NULL );

            QHBoxLayout *regionA_B_C( new QHBoxLayout );
            regionA_B_C->setContentsMargins( QMargins() );
            regionA_B_C->setSpacing( 0 );
            regionA_B_C->addWidget( regionA_Widget, 1 );
            regionA_B_C->addLayout( regionB_and_C, 1 );

            QVBoxLayout *formLayout( new QVBoxLayout( this ) );
            formLayout->setContentsMargins( QMargins() );
            formLayout->addLayout( layoutMarquee_, 1 );
            formLayout->addLayout( regionA_B_C, 2 );

            LOG_DEBUG( "Added Regions to Form Layout #2", NULL );
        }
        else if( myLayout == "3" )
        {
            //  +---------------------------------+
            //  |                                 |
            //  |            Region A             |
            //  |                                 |
            //  +----------------+----------------+
            //  |                |                |
            //  |    Region B    |    Region C    |
            //  |                |                |
            //  +----------------+----------------+
            //  |             Marquee             |
            //  +---------------------------------+

            LOG_DEBUG( "createLayout(): Layout #3", NULL );

            QHBoxLayout *regionA( new QHBoxLayout );
            regionA->setContentsMargins( QMargins() );
            regionA->setSpacing( 0 );
            regionA->addWidget( regionA_Widget );

            LOG_DEBUG( "Added Region A", NULL );

            QHBoxLayout *regionB_and_C( new QHBoxLayout );
            regionB_and_C->setContentsMargins( QMargins() );
            regionB_and_C->setSpacing( 0 );
            regionB_and_C->addWidget( regionB_Widget, 1 );
            regionB_and_C->addWidget( regionC_Widget, 1 );

            LOG_DEBUG( "Added Region B and C", NULL );

            QVBoxLayout *formLayout( new QVBoxLayout( this ) );
            formLayout->setContentsMargins( QMargins() );
            formLayout->addLayout( regionA, 1 );
            formLayout->addLayout( regionB_and_C, 1 );
            formLayout->addLayout( layoutMarquee_ );

            LOG_DEBUG( "Added Regions to Form Layout #3", NULL );
        }
        else if( myLayout == "4" )
        {
            //  Layout 4:
            //  +----------------+----------------+
            //  |                |                |
            //  |    Region A    |    Region B    |
            //  |                |                |
            //  +----------------+----------------+
            //  |                                 |
            //  |            Region C             |
            //  |                                 |
            //  +---------------------------------+
            //  |             Marquee             |
            //  +---------------------------------+

            LOG_DEBUG( "createLayout(): Layout #4", NULL );

            QHBoxLayout *regionA_and_B( new QHBoxLayout );
            regionA_and_B->setContentsMargins( QMargins() );
            regionA_and_B->setSpacing( 0 );
            regionA_and_B->addWidget( regionA_Widget, 1 );
            regionA_and_B->addWidget( regionB_Widget, 1 );

            LOG_DEBUG( "Added Region A and B", NULL );

            QHBoxLayout *regionC( new QHBoxLayout );
            regionC->setContentsMargins( QMargins() );
            regionC->setSpacing( 0 );
            regionC->addWidget( regionC_Widget );

            LOG_DEBUG( "Added Region C", NULL );

            QVBoxLayout *formLayout( new QVBoxLayout( this ) );
            formLayout->setContentsMargins( QMargins() );
            formLayout->addLayout( regionA_and_B, 1 );
            formLayout->addLayout( regionC, 1 );
            formLayout->addLayout( layoutMarquee_ );

            LOG_DEBUG( "Added Regions to Form Layout #4", NULL );

        }
        else if( myLayout == "5" )
        {
            //  Layout 5:
            //  +--------+---------------+--------+
            //  |        |               |        |
            //  |        |               |        |
            //  | Region |     Region    | Region |
            //  |   A    |       B       |   C    |
            //  |        |               |        |
            //  |        |               |        |
            //  |        |               |        |
            //  +--------+---------------+--------+
            //  |             Marquee             |
            //  +---------------------------------+

            LOG_DEBUG( "createLayout(): Layout #5", NULL );

            QHBoxLayout *regionA_B_C( new QHBoxLayout );
            regionA_B_C->setContentsMargins( QMargins() );
            regionA_B_C->setSpacing( 0 );
            regionA_B_C->addWidget( regionA_Widget, 1 );
            regionA_B_C->addWidget( regionB_Widget, 2 );
            regionA_B_C->addWidget( regionC_Widget, 1 );

            LOG_DEBUG( "Added Regions A, B and C", NULL );

            QVBoxLayout *formLayout( new QVBoxLayout( this ) );
            formLayout->setContentsMargins( QMargins() );
            formLayout->addLayout( regionA_B_C, 2 );
            formLayout->addLayout( layoutMarquee_, 1 );

            LOG_DEBUG( "Added Regions to Form Layout #5", NULL );
        }
        else if( myLayout == "6" )
        {
            //  Layout 6:
            //  +----------------+----------------+
            //  |    Marquee     |                |
            //  +----------------+                |
            //  |                |                |
            //  |    Region A    |    Region B    |
            //  |                |                |
            //  |                |                |
            //  |                |                |
            //  |                |                |
            //  |                |                |
            //  +----------------+----------------+

            LOG_DEBUG( "createLayout(): Layout #6", NULL );

            QVBoxLayout *regionA( new QVBoxLayout );
            regionA->setContentsMargins( QMargins() );
            regionA->setSpacing( 0 );

            marquee_->setMinimumWidth( 1 );

            regionA->addWidget( marquee_, 1 );
            regionA->addWidget( regionA_Widget, 4 );

            LOG_DEBUG( "Added Region A", NULL );

            QVBoxLayout *regionB( new QVBoxLayout );
            regionB->setContentsMargins( QMargins() );
            regionB->setSpacing( 0 );
            regionB->addWidget( regionB_Widget );

            LOG_DEBUG( "Added Region B", NULL );

            QHBoxLayout *formLayout( new QHBoxLayout( this ) );
            formLayout->setContentsMargins( QMargins() );
            formLayout->addLayout( regionA, 1 );
            formLayout->addLayout( regionB, 1 );

            LOG_DEBUG( "Added Regions to Form Layout #6", NULL );
        }
        else if( myLayout == "7" )
        {
            //  Layout 7:
            //  +---------------------------------+
            //  |             Marquee             |
            //  +---------------------------------|
            //  |                                 |
            //  |            Region A             |
            //  |                                 |
            //  |                                 |
            //  |                                 |
            //  |                                 |
            //  |                                 |
            //  +---------------------------------+

            LOG_DEBUG( "createLayout(): Layout #7", NULL );

            QHBoxLayout *regionA( new QHBoxLayout );
            regionA->setContentsMargins( QMargins() );
            regionA->setSpacing( 0 );
            regionA->addWidget( regionA_Widget );

            LOG_DEBUG( "Added Region A", NULL );

            QVBoxLayout *formLayout( new QVBoxLayout( this ) );
            formLayout->setContentsMargins( QMargins() );
            formLayout->addLayout( layoutMarquee_ );
            formLayout->addLayout( regionA, 1 );

            LOG_DEBUG( "Added Regions to Form Layout #7", NULL );
        }
        else if( myLayout == "8" )
        {
            //  Layout 8:
            //  +----------------------+----------+
            //  |    Marquee           |          |
            //  +----------------------+          |
            //  |                      |          |
            //  |    Region A          | Region B |
            //  |                      |          |
            //  |                      |          |
            //  |                      |          |
            //  |                      |          |
            //  |                      |          |
            //  +----------------------+----------+

            LOG_DEBUG( "createLayout(): Layout #8", NULL );

            QVBoxLayout *regionA( new QVBoxLayout );
            regionA->setContentsMargins( QMargins() );
            regionA->setSpacing( 0 );
            regionA->addWidget( marquee_, 1 );
            regionA->addWidget( regionA_Widget, 4 );

            LOG_DEBUG( "Added Region A", NULL );

            QVBoxLayout *regionB( new QVBoxLayout );
            regionB->setContentsMargins( QMargins() );
            regionB->setSpacing( 0 );
            regionB->addWidget( regionB_Widget );

            LOG_DEBUG( "Added Region B", NULL );

            QHBoxLayout *formLayout( new QHBoxLayout( this ) );
            formLayout->setContentsMargins( QMargins() );
            formLayout->addLayout( regionA, 7 );
            formLayout->addLayout( regionB, 3 );

            LOG_DEBUG( "Added Regions to Form Layout #8", NULL );
        }
        else if( myLayout == "9" )
        {
            //  Layout 9:
            //  +---------------------------------+
            //  |                                 |
            //  |                                 |
            //  |                                 |
            //  |            Region A             |
            //  |                                 |
            //  |                                 |
            //  |                                 |
            //  |                                 |
            //  |                                 |
            //  +---------------------------------+

            LOG_DEBUG( "createLayout(): Layout #9", NULL );

            QHBoxLayout *regionA( new QHBoxLayout );
            regionA->setContentsMargins( QMargins() );
            regionA->setSpacing( 0 );
            regionA->addWidget( regionA_Widget );

            LOG_DEBUG( "Added Region A", NULL );

            QHBoxLayout *formLayout( new QHBoxLayout( this ) );
            formLayout->setContentsMargins( QMargins() );
            formLayout->addLayout( regionA );

            LOG_DEBUG( "Added Regions to Form Layout #9", NULL );
        }
    }
}

QString queueSignageWidget::getTvLayoutSettings( QString settingsValue )
{
    python pythonProcess;
    QStringList args;
    QString strResult = NULL;

    args.clear();
    args.append( settingsValue );

    if ( pythonProcess.execCommand("get_oo_settings_value", args, false, false, ( 30 * 1000 ) ) )
    {
        strResult = pythonProcess.readStdOut();
    }

    return strResult;
}

void queueSignageWidget::retrieveAllTvLayoutSettings()
{
    LOG_CLASS_INFO( "queueSignageWidget", "retrieveAllTvLayoutSettings");

    strKioskTextColor = getTvLayoutSettings( "KioskTextColor" );
    LOG_DEBUG( "strKioskTextColor = %s", qPrintable( strKioskTextColor ) );

    strKioskLightColor = getTvLayoutSettings( "KioskLightColor" );
    LOG_DEBUG( "strKioskLightColor = %s", qPrintable( strKioskLightColor ) );

    strTvMarqueeText = getTvLayoutSettings( "TVMarqueeText" );
    LOG_DEBUG( "strTvMarqueeText = %s", qPrintable( strTvMarqueeText ) );

    strTvLayout = getTvLayoutSettings( "TVLayout" );
    LOG_DEBUG( "strTvLayout = %s", qPrintable( strTvLayout ) );

    // Region A
    strTvRegionA = getTvLayoutSettings( "TVRegionA" );
    LOG_DEBUG( "strTvRegionA = %s", qPrintable( strTvRegionA ) );

    if( strTvRegionA == "W" )
    {
        strTVRegionAURL = getTvLayoutSettings( "TVRegionAURL" );
        LOG_DEBUG( "strTVRegionAURL = %s", qPrintable( strTVRegionAURL ) );

    }
    else if( strTvRegionA == "P" )
    {
        strTVRegionAPlaylist = getTvLayoutSettings( "TVRegionAPlaylist" );
        LOG_DEBUG( "strTVRegionAPlaylist = %s", qPrintable( strTVRegionAPlaylist ) );
    }

    // Region B
    strTvRegionB = getTvLayoutSettings( "TVRegionB" );
    LOG_DEBUG( "strTvRegionB = %s", qPrintable( strTvRegionB ) );

    if( strTvRegionB == "W" )
    {
        strTVRegionBURL = getTvLayoutSettings( "TVRegionBURL" );
        LOG_DEBUG( "strTVRegionBURL = %s", qPrintable( strTVRegionBURL ) );

    }
    else if( strTvRegionB == "P" )
    {
        strTVRegionBPlaylist = getTvLayoutSettings( "TVRegionBPlaylist" );
        LOG_DEBUG( "strTVRegionBPlaylist = %s", qPrintable( strTVRegionBPlaylist ) );
    }


    // Region C
    strTvRegionC = getTvLayoutSettings( "TVRegionC" );
    LOG_DEBUG( "strTvRegionC = %s", qPrintable( strTvRegionC ) );

    if( strTvRegionC == "W" )
    {
        strTVRegionCURL = getTvLayoutSettings( "TVRegionCURL" );
        LOG_DEBUG( "strTVRegionCURL = %s", qPrintable( strTVRegionCURL ) );

    }
    else if( strTvRegionC == "P" )
    {
        strTVRegionCPlaylist = getTvLayoutSettings( "TVRegionCPlaylist" );
        LOG_DEBUG( "strTVRegionCPlaylist = %s", qPrintable( strTVRegionCPlaylist ) );
    }
}

playlistWidget *queueSignageWidget::createPlayListWidget( QString playListName )
{
    playlistWidget *playList = new playlistWidget;
    playList->setAutoFillBackground( true );
    playList->setDisplay( playlistWidget::Operator );
    playList->setImageBackground( Qt::transparent );
    playList->setVideoBackground( Qt::black );
    playList->setCurrentPlaylist( playListName );

    QPalette p;
    p = playList->palette();
    p.setBrush( playList->backgroundRole(), Qt::transparent );
    playList->setPalette( p );

    return playList;
}

QWebView *queueSignageWidget::createWebViewWidget( QString defaultUrl )
{
    QWebView *webViewer_;
    webViewer_ = new QWebView( this );
    webViewer_->setGeometry( 0, 0, 200, 200 );
    webViewer_->load( ( QUrl( defaultUrl ) ) );

    connect( webViewer_->page()->networkAccessManager(),
                SIGNAL(sslErrors(QNetworkReply*, const QList<QSslError> & )), this,
                SLOT(handleSslErrors(QNetworkReply*, const QList<QSslError> & )));

    webViewer_->show();

    return webViewer_;
}

void queueSignageWidget::handleSslErrors(QNetworkReply* reply, const QList<QSslError> &errors)
{
    qDebug() << "handleSslErrors: ";
    foreach (QSslError e, errors)
    {
        qDebug() << "ssl error: " << e;
    }

    qDebug( "ignoring ssl errors" );
    reply->ignoreSslErrors();
}
