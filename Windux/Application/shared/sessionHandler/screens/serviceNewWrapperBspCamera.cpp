/**
 * @file serviceNewWrapperBspCamera.cpp
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 */

#include "serviceNewWrapperBspCamera.h"

#include "../sessionmainwindow.h"

#include "../modes/servicemode.h"

#include <backgroundwidgetstyles.h>
#include <logmanager.h>

#include <Messages.h>
#include <ScaleConfiguration.h>
#include <UiUtilities.h>

#include <QApplication>
#include <QPaintEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>

///////////////////////////////////////////////////////////////////////////////////////////////////
intakeCameraGraphWidget::intakeCameraGraphWidget( QWidget *parent ) :
    _Mybase( parent ),
    threshold_( 0 )
{
    cameraLeftNominal_.reserve( CAMERA_POINTS );
    cameraRightNominal_.reserve( CAMERA_POINTS );

    cameraLeft_.reserve( CAMERA_POINTS );
    cameraRight_.reserve( CAMERA_POINTS );

    // nominal camera points, from Matt Bridge 6/26/06
    static const int nomLeft[] = { 8,8,8,8,9,8,9,9,10,10,11,10,11,11,11,11,12,12,14,14,66,16,243,66,243,243,243,243,243,243,243,243,243,243,243,243,243,243,243,243,243,243,243,243,243,243,243,243,243,243,243,243,243,243,243,243,57,98,243,57,243,243,243,243,243,243,243,243,243,243,243,243,243,243,243,243,243,243,254,243,201,201,243,243,243,243,243,243,243,243,243,243,243,243,55,243,242,55,243,243,243,243,97,34,243,243,243,243,243,243,212,212,206,55,243,206,113,113,176,24,170,176,133,133,10,72,0,10 };
    static const int nomRight[] = { 61,20,105,61,48,48,199,78,235,199,242,242,243,239,40,243,52,52,243,239,161,243,24,24,243,243,243,243,243,243,243,243,243,243,243,243,85,243,43,43,243,243,243,243,243,243,243,243,243,243,243,243,243,243,243,243,243,243,68,243,68,68,243,243,243,243,243,243,243,243,243,243,243,243,243,243,243,243,243,243,243,243,243,243,243,243,243,243,243,243,243,243,243,243,243,243,239,243,24,239,19,19,15,17,16,15,14,14,13,16,14,13,12,12,11,14,12,11,10,10,9,12,10,9,10,10,0,9 };

    // populate mapping
    for ( int i( 0 ); i < CAMERA_POINTS; ++i )
    {
        cameraLeftNominal_.append( nomLeft[i] );
        cameraRightNominal_.append( nomRight[i] );
    }

    // render data
    renderNominalData();
    renderData();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
intakeCameraGraphWidget::~intakeCameraGraphWidget()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void intakeCameraGraphWidget::setCameraData( camera c, const cameraImageList &value )
{
    LOG_CLASS_INFO( "intakeCameraGraphWidget", "setCameraData" );

    if (( leftCamera != c ) && ( rightCamera != c ))
        return;
    else if ( CAMERA_POINTS != value.size() )
        return;

    LOG_TRACE( "new camera data!", NULL );

    QMutexLocker guard( &m_ );

    // copy dataset
    if ( leftCamera == c )
        cameraLeft_ = value;
    else if ( rightCamera == c )
        cameraRight_ = value;

    // render
    renderData();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void intakeCameraGraphWidget::setThreshold( int value )
{
    LOG_CLASS_INFO( "intakeCameraGraphWidget", "setThreshold" );

    if (( value < MIN_THRESHOLD ) && ( 0 != value ))
        return;
    else if ( MAX_THRESHOLD < value )
        return;

    LOG_TRACE( "new camera threshold!", NULL );

    QMutexLocker guard( &m_ );

    // copy threshold
    threshold_ = value;

    // render
    renderData();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void intakeCameraGraphWidget::paintEvent( QPaintEvent *e )
{
    LOG_CLASS_INFO( "intakeCameraGraphWidget", "paintEvent" );

    QMutexLocker guard( &m_ );

    // paint rendered data
    QPainter painter;

    if ( !painter.begin( this ) )
        LOG_WARN( "failed to begin painter!", NULL );
    else
    {
        painter.drawPixmap( e->rect(), pixmap_, e->rect() );

        // end painting
        if ( !painter.end() )
            LOG_WARN( "failed to end painter!", NULL );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void intakeCameraGraphWidget::resizeEvent( QResizeEvent *e )
{
    QMutexLocker guard( &m_ );

    // render data
    renderNominalData();
    renderData();

    _Mybase::resizeEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void intakeCameraGraphWidget::renderNominalData()
{
    LOG_CLASS_INFO( "intakeCameraGraphWidget", "renderNominalData" );

    // generate pixmap for nominal data
    QPixmap pixmap( size() );
    pixmap.fill( Qt::white );

    // start painting
    QPainter painter;

    if ( !painter.begin( &pixmap ) )
        LOG_WARN( "failed to begin painter!", NULL );
    else
    {
        painter.setRenderHint( QPainter::Antialiasing, true );

        // translate such that bottom middle is 0,0
        painter.translate( width() / 2, height() );

        // scale such that our canvas dimension is camera width by 256 height
        static const int cameraWidth( CAMERA_POINTS + CAMERA_OFFSET );

        qreal sx( width() );
        sx /= NUM_CAMERAS * cameraWidth;

        qreal sy( height() );
        sy /= 256;

        painter.scale( -sx, -sy );

        QPen pen( painter.pen() );
        pen.setCosmetic( true );

        // render nominal data
        pen.setBrush( hobart::colorPalette::green );
        pen.setWidth( 2 );

        painter.setPen( pen );
        drawCameraData( painter, cameraLeftNominal_, cameraRightNominal_ );

        // end painting
        if ( !painter.end() )
            LOG_WARN( "failed to end painter!", NULL );
    }

    // save pixmap for later use
    pixmapNominal_ = pixmap;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void intakeCameraGraphWidget::renderData()
{
    LOG_CLASS_INFO( "intakeCameraGraphWidget", "renderData" );

    LOG_TRACE( "render", NULL );

    // copy nominal data
    QPixmap pixmap( pixmapNominal_ );
    pixmap.detach();

    // start painting
    QPainter painter;

    if ( !painter.begin( &pixmap ) )
        LOG_WARN( "failed to begin painter!", NULL );
    else
    {
        painter.setRenderHint( QPainter::Antialiasing, true );

        // translate such that bottom middle is 0,0
        painter.translate( width() / 2, height() );

        // scale such that our canvas dimension is camera width by 256 height
        static const int cameraWidth( CAMERA_POINTS + CAMERA_OFFSET );

        qreal sx( width() );
        sx /= NUM_CAMERAS * cameraWidth;

        qreal sy( height() );
        sy /= 256;

        painter.scale( -sx, -sy );

        QPen pen( painter.pen() );
        pen.setCosmetic( true );

        // render threshold (limit bar)
        if ( threshold_ )
        {
            pen.setBrush( hobart::colorPalette::black );
            pen.setWidth( 3 );

            painter.setPen( pen );
            painter.drawLine( QPoint( -cameraWidth, threshold_ ), QPoint( cameraWidth, threshold_ ) );
        }

        // render data
        pen.setBrush( hobart::colorPalette::black );
        pen.setWidth( 1 );

        painter.setPen( pen );
        drawCameraData( painter, cameraLeft_, cameraRight_ );

        // end painting
        if ( !painter.end() )
            LOG_WARN( "failed to end painter!", NULL );
    }

    LOG_TRACE( "render... done", NULL );

    // save pixmap
    pixmap_ = pixmap;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void intakeCameraGraphWidget::drawCameraData( QPainter &painter, const cameraImageList &left, const cameraImageList &right )
{
    drawCameraDataEx( painter, left, -(CAMERA_POINTS + CAMERA_OFFSET) );
    drawCameraDataEx( painter, right, CAMERA_OFFSET );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void intakeCameraGraphWidget::drawCameraDataEx( QPainter &painter, const cameraImageList &data, int offset )
{
    if ( CAMERA_POINTS != data.size() )
        return;

    QPoint prev( offset++, data[0] );

    for ( int i( 1 ); i < CAMERA_POINTS; ++i )
    {
        const QPoint p( offset++, data[i] );

        painter.drawLine( prev, p );
        prev = p;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
serviceNewWrapperBspCamera::serviceNewWrapperBspCamera( SessionMainWindow *mainWindow, ModeWidget *parent ) :
    _Mybase( mainWindow, parent ),
    origThreshold_( 0 ),
    threshold_( 0 )
{
    setObjectName( "serviceNewWrapperBspCamera" );

    LOG_CLASS_INFO( "serviceNewWrapperBspCamera", "serviceNewWrapperBspCamera" );

    LOG_DEBUG( "TOP of serviceNewWrapperBspCamera()", NULL );

    // initialize
    initialize();
    createLayout();
    translate();

    LOG_DEBUG( "serviceNewWrapperBspCamera(): Start QTimer", NULL );

    // SOF-1205
    startCameraTestTimer_ = new QTimer( this );
    startCameraTestTimer_->setSingleShot( true );
    startCameraTestTimer_->setInterval( START_TEST_TIMER );
    startCameraTestTimer_->start();

    connect( startCameraTestTimer_, SIGNAL(timeout()), SLOT(startCameraTest()) );

    // SOF-1264
    requestCameraTracking_ = new QTimer( this );
    requestCameraTracking_->setSingleShot( true );

    connect( requestCameraTracking_, SIGNAL(timeout()), SLOT(requestCameraTracking()) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
serviceNewWrapperBspCamera::~serviceNewWrapperBspCamera()
{
    // save!
    saveCurrentThreshold();

    // turn off camera tracking
    session_->send( Wrapper, WrEvolCamerasTracking, "0" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperBspCamera::translate()
{
    inc_->setText( tr( "Up" ) );
    dec_->setText( tr( "Down" ) );
    save_->setText( tr( "Save" ) );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperBspCamera::onSessionMessage( const SessionMessage& msg )
{
    LOG_CLASS_INFO( "serviceNewWrapperBspCamera", "onSessionMessage" );

    int device( msg[0].toInt() );
    int event( msg[1].toInt() );

    // camera image
    if (( Wrapper == device ) && ( WrCameraScan == event ))
    {
        LOG_TRACE( "processing camera scan", NULL );

        const DomEvolWrapperCameraImage m( msg );

        const int cameraNum( m.cameraNum() );
        const integerList cameraImage( m.cameraImage() );

        // ---- //

        intakeCameraGraphWidget::camera num( (intakeCameraGraphWidget::camera) cameraNum );
        intakeCameraGraphWidget::cameraImageList image;

        image.reserve( 256 );

        foreach ( int i, cameraImage )
            image.append( i );

        // set camera data
        graph_->setCameraData( num, image );

        // ---- //

        static qint64 counter( 0 );

        const qint64 now( QDateTime::currentMSecsSinceEpoch() );

        if ( !counter )
            counter = now;

        // limit refresh to once every second
        if ( 1000 <= (now - counter) )
        {
            graph_->update();

            counter = now;
        }

        // ---- //

        requestCameraTracking_->start( CAMERA_TRACKING_TIMER );
    }

    // camera threshold
    else if (( Wrapper == device ) && ( WrEvolCameraThreshold == event ))
    {
        LOG_TRACE( "processing camera threshold", NULL );

        const DomEvolWrapperCameraThreshold m( msg );

        origThreshold_ = m.threshold();
        LOG_DEBUG( "original threshold %d", origThreshold_ );

        // enable the ui
        if (( MIN_THRESHOLD <= origThreshold_ ) && ( origThreshold_ <= MAX_THRESHOLD ))
        {
            inc_->setEnabled( true );
            dec_->setEnabled( true );
            save_->setEnabled( true );

            graph_->setThreshold( threshold_ = origThreshold_ );
            graph_->update();
        }
    }

    _Mybase::onSessionMessage( msg );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperBspCamera::saveCurrentThreshold()
{
    LOG_CLASS_INFO( "serviceNewWrapperBspCamera", "onSessionMessage" );

    // save threshold if valid and different from original
    if (( MIN_THRESHOLD <= threshold_ ) && ( threshold_ <= MAX_THRESHOLD ))
        if ( origThreshold_ != threshold_ )
        {
            LOG_INFO( "saving new bsp camera threshold %d", threshold_ );
            session_->send( Wrapper, WrEvolWriteCameraThreshold, QString::number( origThreshold_ = threshold_ ) );
        }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperBspCamera::onPushButtonPressed()
{
    // increment threshold
    if ( inc_ == sender() )
    {
        if ( MAX_THRESHOLD < (threshold_ += 2) )
            threshold_ = MAX_THRESHOLD;

        graph_->setThreshold( threshold_ );
    }

    // decrement threshold
    else if ( dec_ == sender() )
    {
        if ( (threshold_ -= 2) < MIN_THRESHOLD )
            threshold_ = MIN_THRESHOLD;

        graph_->setThreshold( threshold_ );
    }

    // save threshold
    else if ( save_ == sender() )
    {
        saveCurrentThreshold();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperBspCamera::requestCameraTracking()
{
    LOG_CLASS_INFO( "serviceNewWrapperBspCamera", "requestCameraTracking" );

    LOG_WARN( "request camera tracking...", NULL );

    // turn on camera tracking
    session_->send( Wrapper, WrEvolCamerasTracking, "1" );

    requestCameraTracking_->start( CAMERA_TRACKING_TIMER );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// SOF-1205
void serviceNewWrapperBspCamera::startCameraTest()
{
    LOG_CLASS_INFO( "serviceNewWrapperBspCamera", "startCameraTest" );

    LOG_DEBUG( "TOP of startCameraTest()", NULL );

    // Enter Wrapper Service mode ...
    if ( !mainWindow_->isServiceMode() )
        session_->send( "9,7,1" );

    LOG_DEBUG( "startCameraTest(): Sent Camera Test messages", NULL );

    // retrieve current camera threshold
    session_->send( Wrapper, WrEvolRequestCameraThreshold );

    // retrieve camera data
    requestCameraTracking();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperBspCamera::initialize()
{
    // graph widget
    graph_ = new intakeCameraGraphWidget( this );
    graph_->setObjectName( "graph" );
    graph_->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

    // increment
    inc_ = new QPushButton( this );
    inc_->setEnabled( false );
    inc_->setObjectName( "inc" );

    connect( inc_, SIGNAL( pressed() ), SLOT( onPushButtonPressed() ) );

    // decrement
    dec_ = new QPushButton( this );
    dec_->setEnabled( false );
    dec_->setObjectName( "dec" );

    connect( dec_, SIGNAL( pressed() ), SLOT( onPushButtonPressed() ) );

    // save
    save_ = new QPushButton( this );
    save_->setEnabled( false );
    save_->setObjectName( "save" );

    connect( save_, SIGNAL( pressed() ), SLOT( onPushButtonPressed() ) );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void serviceNewWrapperBspCamera::createLayout()
{
    QVBoxLayout *buttons( new QVBoxLayout() );
    buttons->addWidget( inc_ );
    buttons->addWidget( dec_ );
    buttons->addWidget( save_ );
    buttons->addStretch();

    QHBoxLayout *screenLayout( new QHBoxLayout( this ) );

    // put graph on opposite side of menu button
    if ( Qt::LeftDockWidgetArea == mainWindow_->numberPadArea() )
    {
        screenLayout->addWidget( graph_, 1 );
        screenLayout->addLayout( buttons );
    }
    else
    {
        screenLayout->addLayout( buttons );
        screenLayout->addWidget( graph_, 1 );
    }
}
