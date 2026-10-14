/**
 * @file nowservingoverlay.cpp
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 */

#include "nowservingoverlay.h"

#include "gcdlanguage.h"

#include <python.h>

#include <QtConcurrentRun>

#include <QPainter>
#include <QTimer>

///////////////////////////////////////////////////////////////////////////////////////////////////
nowServingOverlay::nowServingOverlay( const QString& customer, QWidget *parent ) :
    _Mybase( parent ),
    _Myimpl(),
    customer_( customer )
{
    // setup dialog
    setAttribute( Qt::WA_TranslucentBackground );
    setGeometry( QRect( QPoint( 0, 0 ), parent->size() ) );
    setObjectName( "nowServingOverlay" );

    // initialize
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
nowServingOverlay::~nowServingOverlay()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QSize nowServingOverlay::sizeHint() const
{
    if ( parentWidget() )
        parentWidget()->size();

    return _Mybase::sizeHint();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool nowServingOverlay::initializeOverlay()
{
    overlayCompleteTimer_->start();
    updateOverlayTimer_->start();

    setOverlayState( Ready );
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void nowServingOverlay::showOverlay()
{
    show();
    setOverlayState( Active );

    // speak the now serving number
    speakServingNumber();
    speakTimer_->start();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void nowServingOverlay::hideOverlay()
{
    // stop speaking the now serving number
    speakTimer_->stop();

    hide();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void nowServingOverlay::paintEvent( QPaintEvent* e )
{
    Q_UNUSED( e );

    QPainter painter( this );
    painter.setRenderHint( QPainter::Antialiasing );

    // Note:
    // James Meyer and Tom Jones do not like the darkened background due to possible W&M
    // implications... so commenting for now...
/*
    // darken background
    painter.fillRect( rect(), QColor( 0, 0, 0, 128 ) );
*/
    // compute message dimensions
    QRect dialog;

    if ( location_.isNull() )
        dialog = QRect( width() / 4, height() / 4, width() / 2, height() / 2 );
    else
        dialog = location_;

    QRect upperRegion( dialog );
    QRect lowerRegion( dialog );

    upperRegion.setBottom( dialog.top() + (dialog.height() / 3) );
    lowerRegion.setTop( upperRegion.bottom() );

    int radius( 10 );

    // set color scheme
    if ( 0 == (state_ % 2) )
    {
        painter.setPen( QPen( Qt::black, radius / 2 ) );
        painter.setBrush( Qt::white );
    }
    else
    {
        painter.setPen( QPen( Qt::white, radius / 2 ) );
        painter.setBrush( Qt::black );
    }

    painter.drawRoundedRect( dialog, radius, radius );

    QString upperRegionText( gcdLanguage::get( gcdLanguage::NowServingCustomer, gcdWeighModeType::Avoir, true ) );
    QString lowerRegionText( customer_ );

    QFont f( painter.font() );

    f.setPointSize( calcFontPointSize( 16, painter.font(), upperRegion, upperRegionText ) );
    painter.setFont( f );
    painter.drawText( upperRegion, Qt::AlignCenter, upperRegionText );

    f.setPointSize( calcFontPointSize( 36, painter.font(), lowerRegion, lowerRegionText ) );
    painter.setFont( f );
    painter.drawText( lowerRegion, Qt::AlignCenter, lowerRegionText );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void nowServingOverlay::overlayComplete()
{
    hideOverlay();

    setOverlayState( Complete );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void nowServingOverlay::speakServingNumber()
{
    QString nowServingNumber;

    // format up the serving number
    foreach( const QChar& c, customer_ )
    {
        nowServingNumber.append( " " );
        nowServingNumber.append( c );
    }

    if ( !nowServingNumber.length() )
        return;

    // go!
    QtConcurrent::run( speakServingNumberInternal, nowServingNumber );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void nowServingOverlay::updateOverlay()
{
    ++state_;
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void nowServingOverlay::initialize()
{
    state_ = 0;

    overlayCompleteTimer_ = new QTimer( this );
    overlayCompleteTimer_->setInterval( 30000 ); // FT-1488: increased to 30s, can be dismissed now
    overlayCompleteTimer_->setSingleShot( true );

    connect( overlayCompleteTimer_, SIGNAL( timeout() ), SLOT( overlayComplete() ) );

    updateOverlayTimer_ = new QTimer( this );
    updateOverlayTimer_->setInterval( 1000 );
    updateOverlayTimer_->setSingleShot( false );

    connect( updateOverlayTimer_, SIGNAL( timeout() ), SLOT( updateOverlay() ) );

    speakTimer_ = new QTimer( this );
    speakTimer_->setInterval( 5000 );
    speakTimer_->setSingleShot( false );

    connect( speakTimer_, SIGNAL( timeout() ), SLOT( speakServingNumber() ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int nowServingOverlay::calcFontPointSize( int size, const QFont& font, const QRect& rect, const QString& text )
{
    QFont f( font );

    int step( 4 );

    for ( ;; )
    {
        // bump point size
        f.setPointSize( size + step );

        // compute bounding rect
        QFontMetrics metrics( f );
        QRect boundingRect( metrics.boundingRect( QRect( 0, 0, INT_MAX, INT_MAX ), Qt::AlignLeft | Qt::AlignTop, text ) );

        // check bounds
        if (( rect.width() < boundingRect.width() ) || ( rect.height() < boundingRect.height() ))
            break;

        size += step;
    }

    return size;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void nowServingOverlay::speakServingNumberInternal( const QString& number )
{
    QString command( "tts" );

    QStringList args;
    args.append( "-p" );
    args.append( "\"" + number + "\"" );

    // go!
    python tts;

    tts.execCommand( command, args );
}
