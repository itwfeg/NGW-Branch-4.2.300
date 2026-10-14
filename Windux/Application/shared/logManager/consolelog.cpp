/**
 * @file consolelog.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "consolelog.h"

#include <QApplication>
#include <QClipboard>

#include <QPlainTextEdit>
#include <QPushButton>

#include <QVBoxLayout>
#include <QHBoxLayout>

#include <QTextStream>
#include <QXmlStreamReader>

static const char TITLE[] = "Log Console";

static const QSize DEFAULT_SIZE( 600, 600 );

///////////////////////////////////////////////////////////////////////////////////////////////////
consoleLog::consoleLog()
{
    Qt::WindowFlags flags = windowFlags();
    flags |= Qt::WindowMinimizeButtonHint;
    setWindowFlags( flags );

    setWindowTitle( tr( TITLE ) );

    resize( DEFAULT_SIZE );

    ///////////////////////////////////////////////////////////////////////////
    // setup layout

    QVBoxLayout *layout = new QVBoxLayout( this );

    ///////////////////////////////////////////////////////////////////////////
    // setup log

    log_ = new QPlainTextEdit( this );

    log_->setReadOnly( true );

    layout->addWidget( log_ );

    ///////////////////////////////////////////////////////////////////////////
    // setup buttons

    QHBoxLayout *buttons = new QHBoxLayout();

    clearButton_ = new QPushButton( tr( "C&lear" ), this );
    copyButton_ = new QPushButton( tr( "C&opy" ), this );
    closeButton_ = new QPushButton( tr( "&Close" ), this );

    // add them to the layout
    buttons->addSpacerItem( new QSpacerItem( 0, 0, QSizePolicy::Expanding ) );
    buttons->addWidget( clearButton_ );
    buttons->addWidget( copyButton_ );
    buttons->addWidget( closeButton_ );
    buttons->addSpacerItem( new QSpacerItem( 0, 0, QSizePolicy::Expanding ) );

    layout->addLayout( buttons );

    // connect up the signals and slots
    connect( clearButton_, SIGNAL( clicked() ), this, SLOT( onClearButtonClicked() ) );
    connect( copyButton_, SIGNAL( clicked() ), this, SLOT( onCopyButtonClicked() ) );
    connect( closeButton_, SIGNAL( clicked() ), this, SLOT( onCloseButtonClicked() ) );

    setVisible( false );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
consoleLog::~consoleLog()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
consoleLog::_Myt *consoleLog::create( QXmlStreamReader& xml )
{
    QString format;

    int depth = 0;
    QString node;

    while ( !xml.hasError() )
    {
        xml.readNext();

        if (( xml.isEndElement() ) && ( 0 > --depth ))
            break;
        else if (( xml.isStartElement() ) && ( 0 == depth++ ))
            node = xml.name().toString();
        else if (( xml.isCharacters() ) && ( 1 == depth ))
        {
            if ( "format" == node )
                format = xml.text().toString();
        }
    }

    consoleLog *p = new consoleLog;

    if ( format.length() )
        p->setLineFormat( format );

    return p;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
consoleLog::_Myt *consoleLog::create()
{
    return new consoleLog;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void consoleLog::writeString( const QString& line )
{
    log_->appendPlainText( line );
    open();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void consoleLog::onClearButtonClicked()
{
    log_->clear();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void consoleLog::onCopyButtonClicked()
{
    QApplication::clipboard()->setText( log_->toPlainText() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void consoleLog::onCloseButtonClicked()
{
    close();
}
