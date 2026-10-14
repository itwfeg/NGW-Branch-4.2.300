/**
 * @file capacitybarwidgets.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "backgroundwidgettypes.h"
#include "capacitybarwidgets.h"

#include <logmanager.h>
#include <pixmap.h>

#include <Messages.h>
#include <Session.h>

#include <QMouseEvent>
#include <QPainter>
#include <QTimer>
#include <QTime>

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// indicatorLabel
//
///////////////////////////////////////////////////////////////////////////////////////////////////
indicatorLabel::indicatorLabel( QWidget *parent ) :
    _Mybase( parent ),
    indicatorVisible_( false ),
    indicatorDirection_( Up )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
indicatorLabel::~indicatorLabel()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void indicatorLabel::setIndicatorDirection( Direction value )
{
    indicatorDirection_ = value;
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void indicatorLabel::setIndicatorVisible( bool value )
{
    indicatorVisible_ = value;
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QSize indicatorLabel::sizeHint() const
{
    QSize size( _Mybase::sizeHint() );
    size.rwidth() += 16;

    return size;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void indicatorLabel::paintEvent( QPaintEvent *event )
{
    Q_UNUSED( event );

    static int indentAmount = 4;

    QPainter painter( this );
    QRect size( QPoint( 0, 0 ), sizeHint() );

    painter.setRenderHint( QPainter::Antialiasing, true );
    size.adjust( 1, 3, -1, -3 );

    painter.drawText( size, alignment(), text() );

    if ( indicatorVisible_ )
    {
        if ( Up == indicatorDirection_ )
        {
            painter.drawLine( size.left(), size.bottom(), size.right(), size.bottom() );
            painter.drawLine( size.right(), size.bottom(), size.right()-indentAmount, size.top() );
            painter.drawLine( size.right()-indentAmount, size.top(), size.left()+indentAmount, size.top() );
            painter.drawLine( size.left()+indentAmount, size.top(), size.left(), size.bottom() );
        }
        else if ( Down == indicatorDirection_ )
        {
            painter.drawLine( size.left()+indentAmount, size.bottom(), size.right()-indentAmount, size.bottom() );
            painter.drawLine( size.right()-indentAmount, size.bottom(), size.right(), size.top() );
            painter.drawLine( size.right(), size.top(), size.left(), size.top() );
            painter.drawLine( size.left(), size.top(), size.left()+indentAmount, size.bottom() );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// screenIndicatorIcon
//
///////////////////////////////////////////////////////////////////////////////////////////////////
screenIndicatorIcon::screenIndicatorIcon( int screenCount, QWidget *parent ) :
    _Mybase( parent ),
    screenCount_( -1 )
{
    // initialize
    setScreenCount( qMax( 1, screenCount ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
screenIndicatorIcon::~screenIndicatorIcon()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void screenIndicatorIcon::setScreen( int value )
{
    if (( 0 <= value ) && ( value < screenCount() ))
        if ( screen_ != value )
        {
            setPixmap( images_.at( screen_ = value ) );
            update();
        }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void screenIndicatorIcon::setScreenCount( int value )
{
    if ( 0 < value )
        if ( screenCount_ != value )
        {
            initialize( screenCount_ = value );

            setPixmap( images_.at( screen_ = 0 ) );
            update();
        }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QSize screenIndicatorIcon::calculateSize() const
{
    const int stepping( ((height() * 65) / 100) + 3);

    return QSize( 2 + ((stepping * screenCount()) - 3), height() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void screenIndicatorIcon::mouseReleaseEvent( QMouseEvent *event )
{
    const QPoint pos( event->pos() );

    if ( pos.x() < (width() / 2) )
        Q_EMIT swipeScreenRight();
    else
        Q_EMIT swipeScreenLeft();

    _Mybase::mouseReleaseEvent( event );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void screenIndicatorIcon::resizeEvent( QResizeEvent *event )
{
    initialize( screenCount_ );
    setPixmap( images_.at( screen_ ) );
    update();

    _Mybase::resizeEvent( event );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void screenIndicatorIcon::initialize( int screens )
{
    // allocate room for pixmaps
    images_.resize( screens );

    // when only one screen this widget should show nothing
    if ( 1 == screens )
    {
        images_[0] = QPixmap();
        return;
    }

    const int boxLength( (height() * 65) / 100 );
    const int stepping( boxLength + 3 );

    // draw pixmap for each screen
    for ( int i( 0 ); i < screens; ++i )
    {
        // create pixmap
        QPixmap pixmap( size() );
        pixmap.fill( Qt::transparent );

        // create painter
        QPainter painter( &pixmap );
        painter.setRenderHint( QPainter::Antialiasing, true );

        painter.setBrush( QBrush( Qt::transparent ) );
        painter.setPen( hobart::colorPalette::white );

        // draw boxes
        QPoint pos( 1, (height() - boxLength) / 2 );

        for ( int box( 0 ); box < screens; ++box )
        {
            // current screen should be solid box
            if ( box == i )
                painter.setBrush( QBrush( hobart::colorPalette::white ) );

            // draw!
            painter.drawRoundedRect( pos.x(), pos.y(), boxLength, boxLength, 2.0, 2.0 );

            if ( box == i )
                painter.setBrush( QBrush( Qt::transparent ) );

            // prepare for next box
            pos.rx() += stepping;
        }

        // set pixmap
        images_[i] = pixmap;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// recallIcon
//
///////////////////////////////////////////////////////////////////////////////////////////////////
recallIcon::recallIcon( QWidget *parent ) :
    _Mybase( parent )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
recallIcon::~recallIcon()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void recallIcon::resizeEvent( QResizeEvent *event )
{
    setPixmap( scalePixmap( QPixmap( ":/images/widgets/recallIcon.png" ) ) );
    setAlertPixmap( scalePixmap( QPixmap( ":/images/widgets/recallIconAlert.png" ) ) );

    _Mybase::resizeEvent( event );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// printerIcon
//
///////////////////////////////////////////////////////////////////////////////////////////////////
printerIcon::printerIcon( QWidget *parent ) :
    _Mybase( parent )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
printerIcon::~printerIcon()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void printerIcon::resizeEvent( QResizeEvent *event )
{
    setPixmap( scalePixmap( QPixmap( ":/images/widgets/printerIcon.png" ) ) );
    setAlertPixmap( scalePixmap( QPixmap( ":/images/widgets/printerIconAlert.png" ) ) );

    _Mybase::resizeEvent( event );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// planogramIcon
//
///////////////////////////////////////////////////////////////////////////////////////////////////
planogramIcon::planogramIcon( QWidget *parent ) :
    _Mybase( parent )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
planogramIcon::~planogramIcon()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void planogramIcon::resizeEvent( QResizeEvent *event )
{
    setPixmap( scalePixmap( QPixmap( ":/images/widgets/planogramIcon.png" ) ) );
    setAlertPixmap( scalePixmap( QPixmap( ":/images/widgets/planogramIconAlert.png" ) ) );

    _Mybase::resizeEvent( event );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// wirelessSignalIcon
//
///////////////////////////////////////////////////////////////////////////////////////////////////
wirelessSignalIcon::wirelessSignalIcon( QWidget *parent ) :
    _Mybase( parent ),
    connectionType_( WiredConnectionType ),
    signalStrength_( 0 )
{
    // set pixmaps
    signalStrength0_ = QPixmap( ":/images/widgets/wirelessIcon0.png" );
    signalStrength1_ = QPixmap( ":/images/widgets/wirelessIcon1.png" );
    signalStrength2_ = QPixmap( ":/images/widgets/wirelessIcon2.png" );
    signalStrength3_ = QPixmap( ":/images/widgets/wirelessIcon3.png" );
    signalStrength4_ = QPixmap( ":/images/widgets/wirelessIcon4.png" );
    signalStrength5_ = QPixmap( ":/images/widgets/wirelessIcon5.png" );
    wirelessAlert_ = QPixmap( ":/images/widgets/wirelessIconAlert.png" );

    current_ = wired_ = QPixmap( ":/images/widgets/wiredIcon.png" );
    currentAlert_ = wiredAlert_ = QPixmap( ":/images/widgets/wiredIconAlert.png" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
wirelessSignalIcon::~wirelessSignalIcon()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void wirelessSignalIcon::setConnectionType( ConnectionType value )
{
    // nothing to do
    //if ( connectionType() == value )
    //    return;

    // set proper icons
    if ( WirelessConnectionType == value )
    {
        connectionType_ = value;

        setSignalStrength( signalStrength() );
        setAlertPixmap( scalePixmap( currentAlert_ = wirelessAlert_ ) );
    }
    else if ( WiredConnectionType == value )
    {
        connectionType_ = value;

        setPixmap( scalePixmap( current_ = wired_ ) );
        setAlertPixmap( scalePixmap( currentAlert_ = wiredAlert_ ) );
    }

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void wirelessSignalIcon::setSignalStrength( int value )
{
    // nothing to do
    //if ( signalStrength() == value )
    //    return;

    signalStrength_ = value;

    // set proper icon
    if ( WirelessConnectionType == connectionType() )
    {
        if ( 0 == value )
            setPixmap( scalePixmap( current_ = signalStrength0_ ) );
        else if ( 1 == value )
            setPixmap( scalePixmap( current_ = signalStrength1_ ) );
        else if ( 2 == value )
            setPixmap( scalePixmap( current_ = signalStrength2_ ) );
        else if ( 3 == value )
            setPixmap( scalePixmap( current_ = signalStrength3_ ) );
        else if ( 4 == value )
            setPixmap( scalePixmap( current_ = signalStrength4_ ) );
        else if ( 5 == value )
            setPixmap( scalePixmap( current_ = signalStrength5_ ) );
    }

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void wirelessSignalIcon::invertWiredIcon()
{
    invertPixmap inverter;
    wired_ = inverter( wired_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void wirelessSignalIcon::updateState( const MadpNetworkConnectivity *msg )
{
    // sanity check
    if ( !msg )
        return;

    // network type
    setConnectionType( (ConnectionType) msg->type() );

    // signal strength
    setSignalStrength( msg->strength() );

    // alert state
    setAlertState( msg->isNetworkConnected() ? false : true );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void wirelessSignalIcon::updateState( const MadpNetworkDeviceType *msg )
{
    // sanity check
    if ( !msg )
        return;

    // set network device type icon
    // 0 = wired
    // 1 = wireless
    if ( "1" == const_cast<MadpNetworkDeviceType*>( msg )->deviceType() )
    {
        // if we are switching from some device type to wireless reset signal strength
        bool resetSignalStrength( WirelessConnectionType != connectionType() );

        setConnectionType( WirelessConnectionType );

        if ( resetSignalStrength )
            setSignalStrength( 0 ); //strength is unknown so sohow gray bars
    }
    else
    {
        setConnectionType( WiredConnectionType );
    }

    setAlertState( false );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void wirelessSignalIcon::resizeEvent( QResizeEvent *event )
{
    setPixmap( scalePixmap( current_ ) );
    setAlertPixmap( scalePixmap( currentAlert_ ) );

    _Mybase::resizeEvent( event );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// dropDownArrowIcon
//
///////////////////////////////////////////////////////////////////////////////////////////////////
dropDownArrowIcon::dropDownArrowIcon( QWidget *parent ) :
    _Mybase( parent )
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
dropDownArrowIcon::~dropDownArrowIcon()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void dropDownArrowIcon::resizeEvent( QResizeEvent *event )
{
    QPixmap arrowIcon( size() );
    arrowIcon.fill( Qt::transparent );

    QPainter painter( &arrowIcon );
    painter.setRenderHint( QPainter::Antialiasing, true );

    // draw arrow
    QPainterPath path;
    path.moveTo( 1.0, (height() / 2) - (width() / 4) );
    path.lineTo( width() / 2, (height() / 2) + (width() / 4) );
    path.lineTo( width() - 1, (height() / 2) - (width() / 4) );
    path.closeSubpath();

    painter.setPen( Qt::white );
    painter.setBrush( Qt::white );
    painter.drawPath( path );

    setPixmap( arrowIcon );

    _Mybase::resizeEvent( event );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace
