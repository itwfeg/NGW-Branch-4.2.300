/**
 * @file mainwindow.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "colors.h"
#include "mainwindow.h"
#include "webview.h"

#include <QApplication>
#include <QByteArray>
#include <QColor>
#include <QDir>
#include <QHBoxLayout>
#include <QHostAddress>
#include <QLabel>
#include <QTcpSocket>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidget>

///////////////////////////////////////////////////////////////////////////////////////////////////
mainWindow::mainWindow( const QMap<QString, QString> &params, QWidget *parent ) :
    _Mybase( parent ),
    params_( params ),
    style_( StyleNormal ),
    store_( 0 ),
    dept_( 0 )
{
    QString url( params_["url"] );

    // check style
    if ( "1" == params_["style"] )
        style_ = StyleCompact;
    else if ( "2" == params_["style"] )
        style_ = StyleFloatingLeft;
    else if ( "3" == params_["style"] )
        style_ = StyleFloatingCenter;
    else if ( "4" == params_["style"] )
        style_ = StyleFloatingRight;
    else if ( "5" == params_["style"] )
        style_ = StyleFloatingOffsetLeft;
    else if ( "6" == params_["style"] )
        style_ = StyleFloatingOffsetRight;

    // init
    initialize();
    createLayout();
    translate();

    // retrieve store settings
    if (( "1" == params_["sendstorenumber"] ) || ( "1" == params_["senddeptnumber"] ))
        if ( requestStoreSettings() )
        {
            if (( "1" == params_["sendstorenumber"] ) && ( 0 < store_ ))
            {
                url.append( url.contains( "?" ) ? "&" : "?" );
                url.append( "store=" );
                url.append( QString::number( store_ ) );
            }

            if (( "1" == params_["senddeptnumber"] ) && ( 0 < dept_ ))
            {
                url.append( url.contains( "?" ) ? "&" : "?" );
                url.append( "dept=" );
                url.append( QString::number( dept_ ) );
            }
        }

    // browse to page
    webView_->setUrl( QUrl( url ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
mainWindow::~mainWindow()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mainWindow::translate()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mainWindow::resizeEvent( QResizeEvent *e )
{
    if (( isFloatingButtons() ) && ( buttonBar_ ))
    {
        const QColor c( params_["background"] );

        // update button bar size and placement
        buttonBar_->setFixedSize( QSize( width() / 5, 40 ) );

        if ( StyleFloatingLeft == style_ )
            buttonBar_->move( 0, 0 );
        else if ( StyleFloatingCenter == style_ )
            buttonBar_->move( (width() - buttonBar_->width()) / 2, 0 );
        else if ( StyleFloatingRight == style_ )
            buttonBar_->move( width() - buttonBar_->width(), 0 );
        else if ( StyleFloatingOffsetLeft == style_ )
            buttonBar_->move( (width() / 4) - (buttonBar_->width() / 2), 0 );
        else if ( StyleFloatingOffsetRight == style_ )
            buttonBar_->move( ((3 * width()) / 4) - (buttonBar_->width() / 2), 0 );

        buttonBar_->raise();

        // generate 'hobart' look for floating bar
        QPixmap pixmap( buttonBar_->size() );
        pixmap.fill( Qt::transparent );

        QPainter painter( &pixmap );
        painter.setBrush( QBrush( c, Qt::SolidPattern ) );
        painter.setPen( c );
        painter.setRenderHint( QPainter::Antialiasing, true );

        QPainterPath path( QPoint( 0, 0 ) );

        if ( StyleFloatingLeft == style_ )
            path.lineTo( 0, pixmap.height() );
        else
            path.lineTo( (pixmap.height() * 40) / 100, pixmap.height() );

        if ( StyleFloatingRight == style_ )
            path.lineTo( pixmap.width(), pixmap.height() );
        else
            path.lineTo( pixmap.width() - ((pixmap.height() * 40) / 100), pixmap.height() );

        path.lineTo( pixmap.width(), 0 );
        path.closeSubpath();

        painter.drawPath( path );

        // set floating bar graphic
        QPalette p( buttonBar_->palette() );
        p.setBrush( buttonBar_->backgroundRole(), pixmap );
        buttonBar_->setPalette( p );
    }

    _Mybase::resizeEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mainWindow::onButtonClicked()
{
    if ( toggleKeyboard_ == sender() )
        Q_EMIT showInputDevice( webView_, true );

    else if ( refresh_ == sender() )
        webView_->reload();

    else if ( minimize_ == sender() )
        setWindowState( Qt::WindowMinimized );

    else if ( exit_ == sender() )
        close();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mainWindow::initialize()
{
    // setup font
    QFont f( font() );
    f.setPointSize( params_["fontpointsize"].toInt() );
    f.setFamily( params_["fontfamily"] );
    setFont( f );

    // setup palette
    QPalette p( palette() );
    p.setColor( backgroundRole(), QColor( params_["background"] ) );
    p.setColor( QPalette::ButtonText, QColor( params_["fontcolor"] ) );
    p.setColor( QPalette::Text, QColor( params_["fontcolor"] ) );
    p.setColor( QPalette::WindowText, QColor( params_["fontcolor"] ) );
    setPalette( p );

    // ---- //

    const QPixmap logo( params_["logo"] );

    centralWidget_ = new QWidget( this );
    centralWidget_->setObjectName( "centralWidget" );

    buttonBar_ = new QWidget( centralWidget_ );
    buttonBar_->setAutoFillBackground( true );
    buttonBar_->setObjectName( "buttonBar" );

    logo_ = new QLabel( buttonBar_ );
    logo_->setObjectName( "logo" );

    if ( logo.isNull() )
        logo_->hide();
    else
    {
        logo_->setPixmap( logo );
        logo_->setFixedSize( logo.size() );
    }

    title_ = new QLabel( buttonBar_ );
    title_->setAlignment( Qt::AlignCenter );
    title_->setObjectName( "title" );
    title_->setText( params_["title"] );

    toggleKeyboard_ = new QToolButton( buttonBar_ );
    toggleKeyboard_->setFocusPolicy( Qt::NoFocus );
    toggleKeyboard_->setIcon( QPixmap( "res/showIcon.png") );
    toggleKeyboard_->setIconSize( QSize( 40, 40 ) );
    toggleKeyboard_->setObjectName( "exit" );
    toggleKeyboard_->setStyleSheet( "background: transparent; border: none;" );
    toggleKeyboard_->setVisible( "0" != params_["allowkeyboard"] );
    connect( toggleKeyboard_, SIGNAL(clicked()), SLOT(onButtonClicked()) );

    refresh_ = new QToolButton( buttonBar_ );
    refresh_->setFocusPolicy( Qt::NoFocus );
    refresh_->setIcon( QPixmap( "res/refreshIcon.png") );
    refresh_->setIconSize( QSize( 40, 40 ) );
    refresh_->setObjectName( "exit" );
    refresh_->setStyleSheet( "background: transparent; border: none;" );
    refresh_->setVisible("0" != params_["allowrefresh"]);
    connect( refresh_, SIGNAL(clicked()), SLOT(onButtonClicked()) );

    minimize_ = new QToolButton( buttonBar_ );
    minimize_->setFocusPolicy( Qt::NoFocus );
    minimize_->setIcon( QPixmap( "res/minimizeIcon.png") );
    minimize_->setIconSize( QSize( 40, 40 ) );
    minimize_->setObjectName( "exit" );
    minimize_->setStyleSheet( "background: transparent; border: none;" );
    minimize_->setVisible( "0" != params_["allowminimize"] );
    connect( minimize_, SIGNAL(clicked()), SLOT(onButtonClicked()) );

    exit_ = new QToolButton( buttonBar_ );
    exit_->setFocusPolicy( Qt::NoFocus );
    exit_->setIcon( QPixmap( "res/exitIcon.png") );
    exit_->setIconSize( QSize( 40, 40 ) );
    exit_->setObjectName( "exit" );
    exit_->setStyleSheet( "background: transparent; border: none;" );
    exit_->setVisible( "0" != params_["allowclose"] );
    connect( exit_, SIGNAL(clicked()), SLOT(onButtonClicked()) );

    webView_ = new hobart::webView( ("1" == params_["allowcookies"]), centralWidget_ );
    webView_->setObjectName( "webView" );
    webView_->setPalette( QApplication::palette() );
    connect( webView_, SIGNAL(hideInputDevice()), SIGNAL(hideInputDevice()) );
    connect( webView_, SIGNAL(showInputDevice(QWidget*)), SIGNAL(showInputDevice(QWidget*)) );

    // ---- //

    setCentralWidget( centralWidget_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void mainWindow::createLayout()
{
    QHBoxLayout *buttons( new QHBoxLayout( buttonBar_ ) );

    if ( StyleNormal != style_ )
    {
        buttons->setContentsMargins( QMargins() );
        buttons->setSpacing( 4 );

        if (( isFloatingButtons() ) && ( StyleFloatingLeft != style_ ))
            buttons->addSpacing( 16 );

        buttons->addWidget( logo_ );
        buttons->addWidget( refresh_ );
        buttons->addWidget( toggleKeyboard_ );
        buttons->addWidget( title_ );
        buttons->addStretch();
        buttons->addWidget( minimize_ );
        buttons->addWidget( exit_ );

        if (( isFloatingButtons() ) && ( StyleFloatingRight != style_ ))
            buttons->addSpacing( 16 );

        logo_->hide();
        title_->hide();
        toggleKeyboard_->setIconSize( QSize( 32, 32 ) );
        refresh_->setIconSize( QSize( 32, 32 ) );
        minimize_->setIconSize( QSize( 32, 32 ) );
        exit_->setIconSize( QSize( 32, 32 ) );
    }
    else
    {
        buttons->setContentsMargins( QMargins( 5, 5, 5, 5 ) );
        buttons->setSpacing( 8 );

        buttons->addWidget( logo_ );
        buttons->addWidget( title_, 1 );
        buttons->addWidget( toggleKeyboard_ );
        buttons->addWidget( refresh_ );
        buttons->addWidget( minimize_ );
        buttons->addWidget( exit_ );
    }

    QVBoxLayout *layout( new QVBoxLayout( centralWidget_ ) );
    layout->setContentsMargins( QMargins() );
    layout->setSpacing( 0 );

    if ( !isFloatingButtons() )
        layout->addWidget( buttonBar_ );

    layout->addWidget( webView_, 1 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool mainWindow::requestStoreSettings()
{
    const QString server( params_["server"] );
    const quint16 port( params_["port"].toUShort() );

    if (( server.isEmpty() ) || ( port <= 0 ))
        return false;

    // setup buffers
    QByteArray buffer( "RT73\x1e" );

    // connect to server
    QTcpSocket s;
    s.connectToHost( QHostAddress( server ), port );

    if ( !s.waitForConnected( 2000 ) )
        return false;
    else if ( !s.write( buffer ) )
        return false;
    else if ( !s.waitForReadyRead( 3000 ) )
        return false;

    buffer = s.readAll();
    s.close();

    // parse buffer
    if (( 8 < buffer.length() ) && ( 0x1e == buffer[buffer.length()-1] ))
    {
        buffer.replace( '\x1e', '\x1f' );

        // split buffer into tags
        const QList<QByteArray> tags( buffer.split( '\x1f' ) );

        if ( 5 <= tags.size() )
            if ( "RT8A" == tags[0] )
            {
                // parse out data we are interested in
                foreach ( const QByteArray &tag, tags )
                    if ( 2 < tag.length() )
                    {
                        // store number
                        if ( "si" == tag.left( 2 ) )
                            store_ = tag.mid( 2 ).toInt();

                        // dept number
                        else if ( "Dn" == tag.left( 2 ) )
                            dept_ = tag.mid( 2 ).toInt();
                    }

                return true;
            }
    }

    return false;
}
