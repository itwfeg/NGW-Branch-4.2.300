/**
 * @file firmwareupgradedialog.cpp
 *
 * @copyright Copyright (c) 2019 Hobart Corporation
 * @author Randy Blankley
 */

#include "firmwareupgradedialog.h"

#include <QLabel>
#include <QProgressBar>
#include <QVBoxLayout>

///////////////////////////////////////////////////////////////////////////////////////////////////
firmwareUpgradeDialog::firmwareUpgradeDialog( QWidget *parent ) :
    _Mybase( parent )
{
    // frameless window
    Qt::WindowFlags flags( windowFlags() );
    flags |= Qt::FramelessWindowHint;

    // increase font size
    QFont f( font() );
    f.setPointSize( 14 );

    // darken background
    QPalette p( palette() );
    QColor c( p.color( QPalette::Window ) );
    c = c.darker( 500 );

    p.setColor( QPalette::Window, c );

    setFont( f );
    setPalette( p );
    setWindowFlags( flags );
    setModal( true );

    // init
    initialize();
    createLayout();
    translate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
firmwareUpgradeDialog::~firmwareUpgradeDialog()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void firmwareUpgradeDialog::setProgress( int value )
{
    progress_->setValue( value );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void firmwareUpgradeDialog::translate()
{
    setWindowTitle( tr( "Firmware Update" ) );

    title_->setText( tr( "FIRMWARE UPDATE" ) );

    message_->setText( tr( "Firmware update in progress." ) );

    warning_->setText( tr( "Please do not exit or reboot until the firmware update is complete!" ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void firmwareUpgradeDialog::initialize()
{
    QPalette red( palette() );
    red.setColor( QPalette::ButtonText, Qt::red );
    red.setColor( QPalette::Text, Qt::red );
    red.setColor( QPalette::WindowText, Qt::red );

    QPalette white( palette() );
    white.setColor( QPalette::ButtonText, Qt::white );
    white.setColor( QPalette::Text, Qt::white );
    white.setColor( QPalette::WindowText, Qt::white );

    // ---- //

    title_ = new QLabel( this );
    title_->setAlignment( Qt::AlignCenter );
    title_->setObjectName( "title" );
    title_->setPalette( white );

    message_ = new QLabel( this );
    message_->setObjectName( "message" );
    message_->setPalette( white );

    warning_ = new QLabel( this );
    warning_->setObjectName( "warning" );
    warning_->setPalette( red );

    progress_ = new QProgressBar( this );
    progress_->setMaximum( 100 );
    progress_->setMinimum( 0 );
    warning_->setObjectName( "progress" );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void firmwareUpgradeDialog::createLayout()
{
    QVBoxLayout *formLayout( new QVBoxLayout( this ) );
    formLayout->addWidget( title_ );
    formLayout->addStretch();
    formLayout->addWidget( message_ );
    formLayout->addWidget( warning_ );
    formLayout->addStretch();
    formLayout->addWidget( progress_ );
}
