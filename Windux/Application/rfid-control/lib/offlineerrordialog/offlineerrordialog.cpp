
#include "offlineerrordialog.h"

#include <QApplication>
#include <QFont>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPalette>
#include <QPixmap>
#include <QPushButton>
#include <QScreen>
#include <QTimer>
#include <QVBoxLayout>

///////////////////////////////////////////////////////////////////////////////////////////////////
OfflineErrorDialog::OfflineErrorDialog( QWidget *parent, Qt::WindowFlags f ) :
    _Mybase( parent, f ),
    accept_( nullptr ),
    reject_( nullptr )
{
    setWindowTitle( tr( "HookErrorDialog" ) );

    setModal( true );
    setWindowModality( Qt::ApplicationModal );
    setWindowFlags( Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint );

    initialize();
    createLayout();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OfflineErrorDialog::setErrorBackgroundColor( const QString& value )
{
    QColor c;
    c.setNamedColor( value );

    QPalette p( form_->palette() );
    p.setBrush( QPalette::Window, c );
    p.setBrush( QPalette::Button, c );

    form_->setPalette( p );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OfflineErrorDialog::setErrorFontFamily( const QString& value )
{
    QFont f( form_->font() );
    f.setFamily( value );

    form_->setFont( f );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OfflineErrorDialog::setErrorFontPointSize( int value )
{
    QFont f( form_->font() );
    f.setPointSize( value );

    form_->setFont( f );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OfflineErrorDialog::setErrorForegroundColor( const QString& value )
{
    QColor c;
    c.setNamedColor( value );

    QPalette p( form_->palette() );
    p.setBrush( QPalette::WindowText, c );
    p.setBrush( QPalette::ButtonText, c );

    form_->setPalette( p );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OfflineErrorDialog::capturePrimaryScreen()
{
    QScreen *screen( QApplication::primaryScreen() );

    QPixmap pix( screen->grabWindow( 0 ) );

    QPainter painter;
    painter.begin( &pix );
    painter.fillRect( pix.rect(), QColor( 0, 0, 0, 128 ) );
    painter.end();

    QPalette p( palette() );
    p.setBrush( QPalette::Window, pix );

    setAutoFillBackground( true );
    setFixedSize( pix.size() );
    setPalette( p );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OfflineErrorDialog::generateForm()
{
    msg_.replace( "\\n", "<br>" );

    // message
    QLabel *msg = new QLabel( form_ );
    msg->setAlignment( Qt::AlignTop | Qt::AlignLeft );
    msg->setTextFormat( Qt::RichText );
    msg->setText( msg_ );

    // accept
    if ( acceptText_.length() )
    {
        accept_ = new QPushButton( form_ );
        accept_->setMinimumHeight( 32 );
        accept_->setText( acceptText_ );

        connect( accept_, SIGNAL(clicked()), SLOT(onButtonClicked()) );
    }

    // reject
    if ( rejectText_.length() )
    {
        reject_ = new QPushButton( form_ );
        reject_->setMinimumHeight( 32 );
        reject_->setText( rejectText_ );

        connect( reject_, SIGNAL(clicked()), SLOT(onButtonClicked()) );
    }

    // ---- //

    QVBoxLayout *vlayout = new QVBoxLayout( form_ );
    vlayout->addWidget( msg );
    vlayout->addStretch();

    if (( accept_ ) || ( reject_ ))
    {
        QHBoxLayout *hlayout = new QHBoxLayout;
        hlayout->addStretch();

        if ( reject_ )
        {
            hlayout->addWidget( reject_ );
            hlayout->addStretch();
        }

        if ( accept_ )
        {
            hlayout->addWidget( accept_ );
            hlayout->addStretch();
        }

        vlayout->addLayout( hlayout );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OfflineErrorDialog::forceTopWindow()
{
    activateWindow();
    raise();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OfflineErrorDialog::onButtonClicked()
{
    if ( accept_ == sender() )
        QApplication::exit( 0 );
    else if ( reject_ == sender() )
        QApplication::exit( -1 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OfflineErrorDialog::showEvent( QShowEvent *e )
{
    if ( !t_ )
    {
        t_ = new QTimer( this );
        t_->setSingleShot( false );
        t_->setInterval( RAISE_WINDOW_TIMER );

        connect( t_, SIGNAL(timeout()), SLOT(forceTopWindow()) );

        t_->start();
    }

    _Mybase::showEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OfflineErrorDialog::initialize()
{
    form_ = new QWidget( this );
    form_->setAutoFillBackground( true );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void OfflineErrorDialog::createLayout()
{
    QHBoxLayout *hlayout = new QHBoxLayout;
    hlayout->addStretch();
    hlayout->addWidget( form_ );
    hlayout->addStretch();

    QVBoxLayout *vlayout = new QVBoxLayout( this );
    vlayout->addStretch();
    vlayout->addLayout( hlayout );
    vlayout->addStretch();
}
