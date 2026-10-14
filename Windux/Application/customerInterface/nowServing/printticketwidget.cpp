/**
 * @file printticketwidget.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "asuraticketprinter.h"
#include "errormessagewidget.h"
#include "fontutil.h"
#include "pixmaputil.h"
#include "printticketwidget.h"
#include "pythonticketprinter.h"
#include "takenumberwidget.h"
#include "ticketprinter.h"
#include "servercomm.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QStackedWidget>
#include <QTimer>
#include <QVBoxLayout>

///////////////////////////////////////////////////////////////////////////////////////////////////
printTicketWidget::printTicketWidget( const QMap<QString, QString>& params, QWidget *parent ) :
    _Mybase( parent ),
    params_( params )
{
    // init
    initialize();
    createLayout();
    translate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
printTicketWidget::~printTicketWidget()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void printTicketWidget::translate()
{
    takeNumber_->translate();
    errorMessage_->translate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void printTicketWidget::resizeEvent( QResizeEvent *e )
{
    _Mybase::resizeEvent( e );

    // adjust header size
    logo_->setFixedHeight( height() / 5 );
    dept_->setFixedHeight( height() / 5 );

    // adjust font sizes
    QFont f( font() );
    f.setPointSize( fontUtil::calcFontPointSize( f, QSize( dept_->width(), dept_->height() / 2 ), dept_->text() ) );

    dept_->setFont( f );

    // adjust pixmap sizes
    QPalette p;

    p = palette();
    p.setBrush( backgroundRole(), pixmapUtil::scalePixmap( QPixmap( "res/background.png" ), size() ) );
    setPalette( p );

    p = logo_->palette();
    p.setBrush( logo_->backgroundRole(), pixmapUtil::scalePixmap( QPixmap( "res/storeLogo.png" ), logo_->size(), Qt::KeepAspectRatio ) );
    logo_->setPalette( p );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void printTicketWidget::onPrintTicket()
{
    QString logo( "res/storeLogo.png" );
    int custNumber( 0 );

    bool success;
    QString errorText;

    // check for printer
    if ( !printer_ )
    {
        success = false;

        // format error
        errorText.append( tr( "No printer configured." ) );
    }

    // make sure printer is ready to print
    else if ( !(success = printer_->isReadyPrint()) )
    {
        // format error
        errorText.append( printer_->errorString() );
        errorText.append( " (" );
        errorText.append( QString::number( printer_->error() ) );
        errorText.append( ")" );
    }

    // retrieve new number
    else if ( !(success = server_->incrementServingQueue( custNumber )) )
    {
        // format error
        errorText.append( server_->errorString() );
        errorText.append( " (" );
        errorText.append( QString::number( server_->error() ) );
        errorText.append( ")" );
    }

    // print ticket
    else if ( !(success = printer_->print( logo, custNumber )) )
    {
        // format error
        errorText.append( printer_->errorString() );
        errorText.append( " (" );
        errorText.append( QString::number( printer_->error() ) );
        errorText.append( ")" );
    }

    // handle error
    if ( !success )
    {
        // show error
        errorMessage_->setErrorText( errorText );
        screens_->setCurrentWidget( errorMessage_ );

        // setup timer for dismissal
        dismissErrorMessage_->setSingleShot( true );
        dismissErrorMessage_->start( 5000 );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void printTicketWidget::onDismissErrorMessage()
{
    screens_->setCurrentWidget( takeNumber_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void printTicketWidget::onHeartbeat()
{
#if defined( Q_OS_LINUX )
    system( qPrintable( "ping -c 8 " + server_->server() ) );
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void printTicketWidget::initialize()
{
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

    // take number screen
    takeNumber_ = new takeNumberWidget();

    connect( takeNumber_, SIGNAL( printTicket() ), SLOT( onPrintTicket() ) );

    // error message screen
    errorMessage_ = new errorMessageWidget();
    errorMessage_->setErrorTextVisible( "1" == params_["showerrordetails"] );
    errorMessage_->setText( params_["errormsg"] );

    // screens
    screens_ = new QStackedWidget( this );
    screens_->addWidget( takeNumber_ );
    screens_->addWidget( errorMessage_ );

    // setup comm
    server_ = serverComm::instance();

    // setup printer
#ifdef ASURA_CPRNT
    printer_ = new asuraTicketPrinter( this );
#else
    if ( pythonTicketPrinter::isInstalled() )
        printer_ = new pythonTicketPrinter( this );
    else
    {
        printer_ = NULL;
    }
#endif

    // setup timer for error message handling
    dismissErrorMessage_ = new QTimer( this );

    connect( dismissErrorMessage_, SIGNAL( timeout() ), SLOT( onDismissErrorMessage() ) );

    // setup timer for heartbeat
    heartbeat_ = new QTimer( this );
    heartbeat_->setSingleShot( false );

    connect( heartbeat_, SIGNAL( timeout() ), SLOT( onHeartbeat() ) );

#ifdef ASURA_CPRNT
    heartbeat_->start( 300000 );
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void printTicketWidget::createLayout()
{
    if ( layout() )
        delete layout();

    QHBoxLayout *header( new QHBoxLayout );
    header->setContentsMargins( QMargins() );
    header->setSpacing( 0 );
    header->addWidget( logo_, 1 );
    header->addWidget( dept_, 2 );

    QVBoxLayout *formLayout( new QVBoxLayout( this ) );
    formLayout->setContentsMargins( QMargins() );
    formLayout->addLayout( header );
    formLayout->addWidget( screens_, 1 );
}
