/**
 * @file supertextfileeditor.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>
#include <ScaleConfiguration.h>
#include <ScaleClasses.h>

#include "supertextfileeditor.h"

#include "../sessionmainwindow.h"
#include "../sessionmainwindowimpl.h"

#include "../modes/supervisormode.h"

#include "../screens/superCustomerDisplay.h"
#include "../screens/superNormalScaleOperation.h"

#include <QVBoxLayout>
#include <QHBoxLayout>

#include <QPlainTextEdit>
#include <QPushButton>

///////////////////////////////////////////////////////////////////////////////////////////////////
superTextFileEditor::superTextFileEditor( SessionMainWindow *mainWindow, ModeWidget *parent ) :
    _Mybase( mainWindow, parent )
{
    setObjectName( "superTextFileEditor" );

    // initialize screen
    initialize();
    translate();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
superTextFileEditor::~superTextFileEditor()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void superTextFileEditor::translate()
{
    doneButton_->setText( tr( "Done" ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void superTextFileEditor::onDoneButtonClicked()
{
    // save file
    saveFile();

    // reload GCD playlist depending on who called it
    if( OriginalScreen::TEXT_EDITOR_GCD_SCREEN == session_->scaleConfig->returnHomeLocation ) //.scaleConfig->returnHomeLocation )
    {
        if ( mainWindow_->switchToModeScreen<SupervisorMode, superCustomerDisplay>() )
            session_->send( GCDSessionDev, GcdSessionUpdatePlaylist );
    }
    else if( OriginalScreen::TEXT_EDITOR_SELF_SERV_SCREEN == session_->scaleConfig->returnHomeLocation )
    {
        mainWindow_->switchToModeScreen<SupervisorMode, SuperNormalScaleOperation>();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void superTextFileEditor::onTextCursorPositionChanged()
{
    // relocate keyboard from top/bottom
    if ( text_->hasFocus() )
        mainWindow_->showKeyboard();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void superTextFileEditor::initialize()
{
    // done button
    doneButton_ = new QPushButton( this );
    doneButton_->setFocusPolicy( Qt::NoFocus );
    doneButton_->setObjectName( "doneButton" );

    connect( doneButton_, SIGNAL( clicked() ), SLOT( onDoneButtonClicked() ) );

    // text
    text_ = new QPlainTextEdit( this );
    text_->setObjectName( "text" );

    connect( text_, SIGNAL( cursorPositionChanged() ), SLOT( onTextCursorPositionChanged() ) );

    // layouts
    QVBoxLayout *screenLayout = new QVBoxLayout( this );
    QHBoxLayout *hlayout;

    screenLayout->setMargin( marginSpacing_ );

    screenLayout->addWidget( text_ );

    hlayout = new QHBoxLayout();
    hlayout->addStretch();
    hlayout->addWidget( doneButton_ );
    screenLayout->addLayout( hlayout );

    // FT-499
    QMargins m( screenLayout->contentsMargins() );
    m.setLeft( m.left() + 16 );
    m.setRight( m.right() + 16 );
    screenLayout->setContentsMargins( m );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void superTextFileEditor::loadFile( const QString& filename )
{
    LOG_CLASS_INFO( "superTextFileEditor", "loadFile" );

    if ( QFile::exists( filename_ = filename ) )
    {
        QFile f( filename_ );

        if ( !f.open( QIODevice::ReadOnly | QIODevice::Text ) )
            LOG_WARN( "failed to open file '%s'", qPrintable( filename_ ) );
        else
        {
            text_->setPlainText( f.readAll() );
            f.close();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void superTextFileEditor::saveFile()
{
    LOG_CLASS_INFO( "superTextFileEditor", "saveFile" );

    if ( !filename_.length() )
    {
        LOG_WARN( "bad filename", NULL );
        return;
    }

    QFile f( filename_ );

    LOG_TRACE( "opening file '%s'...", qPrintable( filename_ ) );

    if ( !f.open( QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text ) )
        LOG_WARN( "failed to open file '%s'", qPrintable( filename_ ) );
    else
    {
        LOG_TRACE( "writing file...", NULL );
        f.write( text_->toPlainText().toLatin1().constData() );

        f.close();
    }

    LOG_TRACE( "done", NULL );
}

