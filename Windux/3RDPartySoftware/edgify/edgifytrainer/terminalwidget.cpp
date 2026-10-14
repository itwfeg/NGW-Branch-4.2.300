/**
 * @file terminalwidget.cpp
 *
 * @copyright Copyright (c) 2022 Hobart Corporation
 * @author Randy Blankley
 */

#include "terminalwidget.h"

#include <QDebug>
#include <QFile>
#include <QFont>
#include <QFontDatabase>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPushButton>
#include <QTextStream>
#include <QTimer>
#include <QVBoxLayout>

#if defined( Q_OS_WIN )
static const QString INPUT_FILE( "cmdin.txt" );
static const QString OUTPUT_FILE( "cmdout.txt" );
#elif defined( Q_OS_LINUX )
static const QString INPUT_FILE( "/usr/share/edgify-apphook/cmdin.txt" );
static const QString OUTPUT_FILE( "/usr/share/edgify-apphook/cmdout.txt" );
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
TerminalWidget::TerminalWidget( QWidget *parent ) :
    _Mybase( parent )
{
    // initialize
    initialize();
    createLayout();

    // refresh timer
    refresh_ = new QTimer( this );
    refresh_->setSingleShot( false );

    connect( refresh_, &QTimer::timeout, this, &_Myt::onTimeout );

    refresh_->start( REFRESH_TIMER );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void TerminalWidget::onButtonClicked()
{
    if ( run_ == sender() )
    {
        const QString cmd( cmd_->text() );

        if ( !cmd.isEmpty() )
        {
            if ( QFile::exists( INPUT_FILE ) )
                QFile::remove( INPUT_FILE );

            QFile f( INPUT_FILE );

            if ( !f.open( QFile::WriteOnly | QFile::Text ) )
                qWarning() << "failed to open input file for write";
            else
            {
                QTextStream os( &f );
                os << cmd;

                f.close();
            }
        }
    }
    else if ( clearPrompt_ == sender() )
    {
        cmd_->clear();
    }
    else if ( clearResults_ == sender() )
    {
        results_->clear();
    }
    else if ( exit_ == sender() )
    {
        emit exitScreen();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void TerminalWidget::initialize()
{
    // cmd label
    cmdLabel_ = new QLabel( tr( "Edgify>" ), this );
    cmdLabel_->setObjectName( "cmdLabel" );

    // cmd
    cmd_ = new QLineEdit( this );
    cmd_->setObjectName( "server" );

    // results
    results_ = new QListWidget( this );
    results_->setFocusPolicy( Qt::NoFocus );
    results_->setFont( QFontDatabase::systemFont( QFontDatabase::FixedFont ) );
    results_->setObjectName( "servers" );

    // run
    run_ = new QPushButton( this );
    run_->setMinimumHeight( 64 );
    run_->setFocusPolicy( Qt::NoFocus );
    run_->setObjectName( "run" );
    run_->setText( tr( "Run" ) );
    connect( run_, &QPushButton::clicked, this, &_Myt::onButtonClicked );

    // clear prompt
    clearPrompt_ = new QPushButton( this );
    clearPrompt_->setMinimumHeight( 64 );
    clearPrompt_->setFocusPolicy( Qt::NoFocus );
    clearPrompt_->setObjectName( "clearPrompt" );
    clearPrompt_->setText( tr( "Clear Prompt" ) );
    connect( clearPrompt_, &QPushButton::clicked, this, &_Myt::onButtonClicked );

    // clear results
    clearResults_ = new QPushButton( this );
    clearResults_->setMinimumHeight( 64 );
    clearResults_->setFocusPolicy( Qt::NoFocus );
    clearResults_->setObjectName( "clearResults" );
    clearResults_->setText( tr( "Clear Results" ) );
    connect( clearResults_, &QPushButton::clicked, this, &_Myt::onButtonClicked );

    // exit
    exit_ = new QPushButton( this );
    exit_->setMinimumHeight( 64 );
    exit_->setFocusPolicy( Qt::NoFocus );
    exit_->setObjectName( "exit" );
    exit_->setText( tr( "Back" ) );
    connect( exit_, &QPushButton::clicked, this, &_Myt::onButtonClicked );

    // ---- //

    connect( cmd_, &QLineEdit::returnPressed, run_, &QPushButton::click );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void TerminalWidget::onTimeout()
{
    if ( QFile::exists( OUTPUT_FILE ) )
    {
        QFile f( OUTPUT_FILE );

        if ( !f.open( QFile::ReadOnly | QFile::Text ) )
            qWarning() << "failed to open input file for read";
        else
        {
            results_->clear();

            // ---- //

            QTextStream is( &f );

            while ( !is.atEnd() )
                results_->addItem( is.readLine() );

            f.close();

            QFile::remove( OUTPUT_FILE );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void TerminalWidget::createLayout()
{
    QHBoxLayout *cmd( new QHBoxLayout );
    cmd->addWidget( cmdLabel_ );
    cmd->addWidget( cmd_, 1 );
    cmd->addStretch();

    QVBoxLayout *upper( new QVBoxLayout );
    upper->addLayout( cmd );
    upper->addWidget( results_, 1 );

    QHBoxLayout *buttons( new QHBoxLayout );
    buttons->addWidget( run_ );
    buttons->addStretch();
    buttons->addWidget( clearPrompt_ );
    buttons->addWidget( clearResults_ );
    buttons->addStretch();
    buttons->addWidget( exit_ );

    QVBoxLayout *layout( new QVBoxLayout( this ) );
    layout->addLayout( upper, 1 );
    layout->addLayout( buttons );
}
