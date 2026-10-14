/**
 * @file marqueeeditorscreen.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "marqueeeditorscreen.h"

#include "../sessionmainwindow.h"
#include "../sessionmainwindowimpl.h"

#include "../modes/operatemode.h"
#include "../modes/supervisormode.h"

#include "../screens/marqueeconfigscreen.h"

#include <marqueetexteditorwidget.h>
#include <marqueelabelwidget.h>

#include <Messages.h>

#include <QVBoxLayout>
#include <QHBoxLayout>

#include <QPushButton>

///////////////////////////////////////////////////////////////////////////////////////////////////
MarqueeEditorScreen::MarqueeEditorScreen( SessionMainWindow *mainWindow, ModeWidget *parent ) :
    _Mybase( mainWindow, parent ),
    marqueeConfig_( NULL ),
    dirty_( false )
{
    // initialize screen
    initialize();
    translate();

    // retrieve config object
    if ( mainWindow_->isSupervisorMode() )
    {
        marqueeConfig_ = &dynamic_cast<SupervisorMode*>( mode_ )->marqueeConfig_;

        // set marquee text
        marqueeEditor_->setText( marqueeConfig_->permanentMarqueeText() );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
MarqueeEditorScreen::~MarqueeEditorScreen()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool MarqueeEditorScreen::helpText( QString& title, QString& description ) const
{
    Q_UNUSED( title );
    Q_UNUSED( description );

    /*
    title = tr( "Help Page for Marquee Editor" );

    description = tr(
        "Marquee Help Data Here" );
	*/
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MarqueeEditorScreen::translate()
{
    cancelButton_->setText( tr( "Cancel" ) );
    doneButton_->setText( tr( "Done" ) );

    if( marqueeEditor_ )
        marqueeEditor_->translate();

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ScreenWidget *MarqueeEditorScreen::swipeScreenRight() const
{
    if ( mainWindow_->isSupervisorMode() )
    {
        saveMarqueeConfig();

        return new MarqueeConfigScreen( mainWindow_, mode_ );
    }

    return _Mybase::swipeScreenRight();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MarqueeEditorScreen::onCancelButtonClicked()
{
    if ( mainWindow_->isSupervisorMode() )
        mainWindow_->switchToModeScreen<SupervisorMode, MarqueeConfigScreen>();
    else
    {
        close();
    }
    marqueeEditor_ = NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MarqueeEditorScreen::onDoneButtonClicked()
{
    saveMarqueeConfig();

    onCancelButtonClicked();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MarqueeEditorScreen::onMarqueeEditorTextChanged()
{
    marqueeLabel_->setText( marqueeEditor_->toPlainText() );
    dirty_ = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MarqueeEditorScreen::initialize()
{
    QHBoxLayout *horizontalLayout;

    QVBoxLayout *screenLayout = new QVBoxLayout( this );

    screenLayout->setMargin( marginSpacing_ );

    ///////////////////////////////////////////////////////////////////////////
    // marquee editor

    marqueeEditor_ = new MarqueeTextEditorWidget( this );
    marqueeEditor_->setObjectName( "marqueeEditor" );

    screenLayout->addWidget( marqueeEditor_ );

    ///////////////////////////////////////////////////////////////////////////
    // marquee label

    marqueeLabel_ = new MarqueeLabelWidget( this );
    marqueeLabel_->setObjectName( "marqueeLabel" );

    marqueeLabel_->setAutoFillBackground( true );
    marqueeLabel_->setMinimumHeight( 60 );

    QPalette p = marqueeLabel_->palette();
    p.setColor( marqueeLabel_->backgroundRole(), Qt::black );
    p.setColor( marqueeLabel_->foregroundRole(), Qt::green );
    marqueeLabel_->setPalette( p );

    QFont f = marqueeLabel_->font();
    f.setPointSize( 32 );
    marqueeLabel_->setFont( f );

    screenLayout->addWidget( marqueeLabel_ );

    connect( marqueeEditor_, SIGNAL( textChanged() ), this, SLOT( onMarqueeEditorTextChanged() ) );

    screenLayout->addStretch();

    ///////////////////////////////////////////////////////////////////////////
    // buttons

    cancelButton_ = new QPushButton( this );
    cancelButton_->setObjectName( "cancelButton" );

    doneButton_ = new QPushButton( this );
    doneButton_->setObjectName( "doneButton" );

    cancelButton_->setFocusPolicy( Qt::NoFocus );
    doneButton_->setFocusPolicy( Qt::NoFocus );

    connect( cancelButton_, SIGNAL( clicked() ), this, SLOT( onCancelButtonClicked() ) );
    connect( doneButton_, SIGNAL( clicked() ), this, SLOT( onDoneButtonClicked() ) );

    horizontalLayout = new QHBoxLayout();
    horizontalLayout->addStretch();
    horizontalLayout->addWidget( cancelButton_ );
    horizontalLayout->addWidget( doneButton_ );

    screenLayout->addLayout( horizontalLayout );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void MarqueeEditorScreen::saveMarqueeConfig() const
{
    // no reason to save if nothing changed
    if ( !dirty_ )
        return;

    // save settings
    if ( marqueeConfig_ )
    {
        // save the values
        marqueeConfig_->setField( marqueeConfig_->permanentMarqueeText(), marqueeEditor_->text() );
        marqueeConfig_->setField( marqueeConfig_->event(), CfgMarqueeSave );

        // send!!
        session_->send( (SessionMessage) *marqueeConfig_ );
    }
 }
