/**
 * @file queuestatewidget.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "fontutil.h"
#include "queuestatewidget.h"
#include "servercomm.h"

#include <QDateTime>
#include <QLabel>
#include <QPainter>
#include <QPen>
#include <QPixmap>
#include <QString>
#include <QTimer>
#include <QVBoxLayout>

///////////////////////////////////////////////////////////////////////////////////////////////////
queueStateWidget::queueStateWidget( QWidget *parent ) :
    _Mybase( parent ),
    showQueueSize_( false )
{
    // init
    initialize();
    createLayout();
    translate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
queueStateWidget::~queueStateWidget()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void queueStateWidget::setNowServingText( const QString &value )
{
    nowServingLabel_->setText( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void queueStateWidget::setQueueSizeText( const QString &value )
{
    queueSizeLabel_->setText( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void queueStateWidget::setShowQueueSize( bool value )
{
    showQueueSize_ = value;

    queueSizeLabel_->setVisible( showQueueSize_ );
    queueSize_->setVisible( showQueueSize_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void queueStateWidget::translate()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void queueStateWidget::resizeEvent( QResizeEvent *e )
{
    // adjust font sizes
    QFont f( font() );

    f.setPointSize( fontUtil::calcFontPointSize( f, QSize( nowServingLabel_->width(), height() / 7 ), nowServingLabel_->text() ) );
    nowServingLabel_->setFont( f );
    queueSizeLabel_->setFont( f );

    f.setPointSize( fontUtil::calcFontPointSize( f, QSize( nowServing_->width(), height() / 4 ), "XXX", true ) );
    nowServing_->setFont( f );
    queueSize_->setFont( f );

    f.setPointSize( fontUtil::calcFontPointSize( f, QSize( dateTime_->width(), height() / 8 ), "XXXXXXXXX XXX XX XX:XX:XX XX" ) );
    dateTime_->setFont( f );

    // adjust background
    QPalette p;

    p = palette();
    p.setBrush( backgroundRole(), transparentBackground( size() ) );
    setPalette( p );

    _Mybase::resizeEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void queueStateWidget::onUpdateCurrentlyServing( int currentlyServing, int queueSize )
{
    Q_UNUSED( queueSize );

    nowServing_->setText( QString::number( currentlyServing ) );
    queueSize_->setText( QString::number( queueSize ) );
}

//////////////////////////////////////////////////////////////////////////////////////////////////
void queueStateWidget::onUpdateCurrentTime()
{
    QDateTime now( QDateTime::currentDateTime() );

    dateTime_->setText( now.toString( "dddd MMM d h:mm:ss ap" ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void queueStateWidget::initialize()
{
    QPalette p;

    setAutoFillBackground( true );

    // now serving label
    nowServingLabel_ = new QLabel( this );
    nowServingLabel_->setAlignment( Qt::AlignCenter );

    p = nowServingLabel_->palette();
    p.setBrush( nowServingLabel_->foregroundRole(), Qt::black );
    nowServingLabel_->setPalette( p );

    // now serving
    nowServing_ = new QLabel( this );
    nowServing_->setAlignment( Qt::AlignCenter );

    p = nowServing_->palette();
    p.setBrush( nowServing_->foregroundRole(), Qt::black );
    nowServing_->setPalette( p );

    // queue size label
    queueSizeLabel_ = new QLabel( this );
    queueSizeLabel_->setAlignment( Qt::AlignCenter );
    queueSizeLabel_->setVisible( false );

    p = queueSizeLabel_->palette();
    p.setBrush( queueSizeLabel_->foregroundRole(), Qt::black );
    queueSizeLabel_->setPalette( p );

    // queue size
    queueSize_ = new QLabel( this );
    queueSize_->setAlignment( Qt::AlignCenter );
    queueSize_->setVisible( false );

    p = queueSize_->palette();
    p.setBrush( queueSize_->foregroundRole(), Qt::black );
    queueSize_->setPalette( p );

    // date/time
    dateTime_ = new QLabel( this );
    dateTime_->setAlignment( Qt::AlignCenter );

    p = dateTime_->palette();
    p.setBrush( dateTime_->foregroundRole(), Qt::black );
    dateTime_->setPalette( p );

    // setup comm
    server_ = serverComm::instance();

    connect( server_, SIGNAL( updateCurrentlyServing(int,int) ), SLOT( onUpdateCurrentlyServing(int,int) ) );

    // timer
    updateCurrentTime_ = new QTimer( this );

    connect( updateCurrentTime_, SIGNAL( timeout() ), SLOT( onUpdateCurrentTime() ) );

    updateCurrentTime_->start( 1000 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void queueStateWidget::createLayout()
{
    static const int a( 8 );

    if ( layout() )
        delete layout();

    QVBoxLayout *formLayout( new QVBoxLayout( this ) );
    formLayout->setContentsMargins( QMargins( 0, a, 0, a ) );
    formLayout->setSpacing( a );
    formLayout->addStretch();
    formLayout->addWidget( nowServingLabel_ );
    formLayout->addWidget( nowServing_ );
    formLayout->addStretch();
    formLayout->addWidget( queueSizeLabel_ );
    formLayout->addWidget( queueSize_ );
    formLayout->addStretch();
    formLayout->addWidget( dateTime_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QPixmap queueStateWidget::transparentBackground( const QSize& size )
{
    static const int a( 6 );

    // generate pixmap for weights and measures area
    QPixmap pixmap( size );
    pixmap.fill( Qt::transparent );

    QPainter painter( &pixmap );
    painter.setRenderHint( QPainter::Antialiasing, true );

    // create background
    painter.setBrush( QColor( 255, 255, 255, 127 ) );
    painter.setPen( QPen( Qt::black, 2 ) );

    painter.drawRoundedRect( QRect( 0, 0, size.width(), size.height() ).adjusted( a, a, -a, -a ), 4, 4 );

    return pixmap;
}
