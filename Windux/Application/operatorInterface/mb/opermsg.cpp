#include "opermsg.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QResizeEvent>
#include <QTimer>
#include <QVBoxLayout>
#include <QFileInfo>

///////////////////////////////////////////////////////////////////////////////////////////////////
operMsg::operMsg( QWidget *parent ) :
    _Mybase( parent, Qt::Tool | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint )
{
    initialize();
    createLayout();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
operMsg::~operMsg()
{    
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void operMsg::setLogoVisible( bool value )
{
    logo_->setVisible( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void operMsg::setMessage( const QString& value )
{
    message_->setText( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString operMsg::message() const
{
    return message_->text();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void operMsg::setMessageFontFamily( const QString& value )
{
    QFont f( message_->font() );
    f.setFamily( value );
    message_->setFont( f );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QString operMsg::messageFontFamily() const
{
    QFont f( message_->font() );
    return f.family();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void operMsg::setMessageFontPointSize( int value )
{
    QFont f( message_->font() );
    f.setPointSize( value );
    message_->setFont( f );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int operMsg::messageFontPointSize() const
{
    QFont f( message_->font() );
    return f.pointSize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void operMsg::setTimer( int value )
{
    timer_->setSingleShot( true );
    timer_->start( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QSize operMsg::defaultSize()
{
    if (isSlicerDevice())
        return QSize( 800, 480);
    else
        return QSize( 800, 600 );
}

bool operMsg::isSlicerDevice()
{
    QFileInfo liveSlicerFile("/media/mmcblk0p1/etc/hobart/slicer");
    QFileInfo recoverySlicerFile("/media/mmcblk0p2/etc/hobart/slicer");

    bool slicerDevice=false;

    if (liveSlicerFile.exists() && liveSlicerFile.isFile())
        slicerDevice = true;
    else if (recoverySlicerFile.exists() && recoverySlicerFile.isFile())
        slicerDevice = true;
    return slicerDevice;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void operMsg::resizeEvent( QResizeEvent *e )
{
    Q_UNUSED( e );

    //
    // scale logo to dialog width
    //

    QPixmap logo( ":/images/hobartMetal.png" );

    QSize scaledSize( logo.size() );
    scaledSize.scale( size(), Qt::KeepAspectRatio );

    QPixmap background( scaledSize );
    background.fill( Qt::transparent );

    QPainter p( &background );
    p.setRenderHint( QPainter::Antialiasing, true );

    p.drawPixmap( QRect( QPoint( 0, 0 ), scaledSize ), logo );

    //
    // map logo to widget
    //

    QPalette newPalette( logo_->palette() );
    newPalette.setBrush( logo_->backgroundRole(), background );

    logo_->setFixedSize( scaledSize );
    logo_->setPalette( newPalette );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void operMsg::initialize()
{
    logo_ = new QLabel( this );
    logo_->setAutoFillBackground( true );
    logo_->setObjectName( "logo" );

    message_ = new QLabel( this );
    message_->setObjectName( "message" );

    timer_ = new QTimer( this );
    timer_->setObjectName( "timer" );
    connect( timer_, SIGNAL( timeout() ), SLOT( reject() ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void operMsg::createLayout()
{
    // delete old layout
    if ( layout() )
        delete layout();

    QHBoxLayout *hlayout( new QHBoxLayout() );
    hlayout->setContentsMargins( QMargins( 0, 0, 0, 0 ) );

    hlayout->addStretch();
    hlayout->addWidget( message_ );
    hlayout->addStretch();

    QVBoxLayout *vlayout( new QVBoxLayout( this ) );
    vlayout->setContentsMargins( QMargins( 0, 0, 0, 0 ) );

    vlayout->addWidget( logo_ );
    vlayout->addStretch();
    vlayout->addLayout( hlayout );
    vlayout->addStretch();
}
