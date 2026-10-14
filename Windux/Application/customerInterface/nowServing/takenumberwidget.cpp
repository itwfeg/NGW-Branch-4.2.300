/**
 * @file takenumberwidget.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "fontutil.h"
#include "gradientpushbutton.h"
#include "pixmaputil.h"
#include "takenumberwidget.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>

///////////////////////////////////////////////////////////////////////////////////////////////////
takeNumberWidget::takeNumberWidget( QWidget *parent ) :
    _Mybase( parent )
{
    // init
    initialize();
    createLayout();
    translate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
takeNumberWidget::~takeNumberWidget()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void takeNumberWidget::translate()
{
    printTicketLabel_->setText( tr( "Please take a number" ) );
    printTicket_->setText( tr( "Press Here" ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void takeNumberWidget::resizeEvent( QResizeEvent *e )
{
    QPalette p;

    _Mybase::resizeEvent( e );

    // adjust button size
    printTicket_->setFixedSize( width() / 2, height() / 2 );

    // adjust font sizes
    QFont f( font() );
    f.setPointSize( fontUtil::calcFontPointSize( f, QSize( printTicket_->width(), printTicket_->height() / 2 ), printTicket_->text() ) );

    printTicketLabel_->setFont( f );

    f.setBold( true );
    printTicket_->setFont( f );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void takeNumberWidget::onDisablePrintTicket()
{
    // disable button
    printTicket_->setEnabled( false );

    // set cooldown timer
    enablePrintTicketTimer_->setSingleShot( true );
    enablePrintTicketTimer_->start( 3000 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void takeNumberWidget::onEnablePrintTicket()
{
    printTicket_->setEnabled( true );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void takeNumberWidget::initialize()
{
    QPalette p;

    // print ticket label
    printTicketLabel_ = new QLabel( this );
    printTicketLabel_->setAlignment( Qt::AlignCenter );

    p = printTicketLabel_->palette();
    p.setBrush( printTicketLabel_->foregroundRole(), Qt::black );
    printTicketLabel_->setPalette( p );

    // print ticket button
    printTicket_ = new gradientPushButton( this );
    printTicket_->setAutoRepeat( false );

    p = printTicket_->palette();
    p.setBrush( QPalette::ButtonText, Qt::white );
    printTicket_->setPalette( p );

    connect( printTicket_, SIGNAL( clicked() ), SLOT( onDisablePrintTicket() ) );
    connect( printTicket_, SIGNAL( clicked() ), SIGNAL( printTicket() ) );

    // enable button timer
    enablePrintTicketTimer_ = new QTimer( this );

    connect( enablePrintTicketTimer_, SIGNAL( timeout() ), SLOT( onEnablePrintTicket() ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void takeNumberWidget::createLayout()
{
    if ( layout() )
        delete layout();

    QHBoxLayout *printTicketLayout( new QHBoxLayout );
    printTicketLayout->addStretch();
    printTicketLayout->addWidget( printTicket_ );
    printTicketLayout->addStretch();

    QVBoxLayout *formLayout( new QVBoxLayout( this ) );
    formLayout->setContentsMargins( QMargins() );
    formLayout->addStretch( 2 );
    formLayout->addWidget( printTicketLabel_ );
    formLayout->addStretch( 1 );
    formLayout->addLayout( printTicketLayout );
    formLayout->addStretch( 2 );
}
