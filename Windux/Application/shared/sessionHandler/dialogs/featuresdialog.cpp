/**
 * @file featuresdialog.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "featuresdialog.h"

#include "../sessioninfo.h"

#include <backgroundwidgettypes.h>
#include <font.h>
#include <palette.h>

#include <Messages.h>
#include <ScaleConfiguration.h>
#include <UiUtilities.h>

#include <QPlainTextEdit>
#include <QTextCursor>
#include <QVBoxLayout>

///////////////////////////////////////////////////////////////////////////////////////////////////
FeaturesDialog::FeaturesDialog( const QStringList &features, QWidget *parent, Qt::WindowFlags f ) :
    _Mybase( CenterDialogLocation, parent, f )
{
    // set object name
    setObjectName( "FeaturesDialog" );

    // initialize interface
    initialize();

    // FT-415
    // while this widget is open do not show system dialogs
    session_->send( SessionDev, SnStopDialogs );

    QString text;

    if ( features.isEmpty() )
        text.append( tr( "No optional features installed." ) + "\n" );
    else
    {
        text.append( tr( "Currently installed software features:" ) + "\n" );
        text.append( "\n" );

        foreach ( const QString& f, features )
            text.append( f.toUpper() + "\n" );
    }

    text_->appendPlainText( text );

    // scroll to top
    text_->moveCursor( QTextCursor::Start );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
FeaturesDialog::~FeaturesDialog()
{
    // FT-415
    // resume dialogs
    session_->send( SessionDev, SnRestartDialogs );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void FeaturesDialog::initialize()
{
    // create widgets
    text_ = new QPlainTextEdit( this );
    text_->setObjectName( "text" );
    text_->setReadOnly( true );
    text_->setFocusPolicy( Qt::NoFocus );
    text_->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

    // create layouts
    QVBoxLayout *screenLayout = new QVBoxLayout( this );
    screenLayout->addWidget( text_ );
}
