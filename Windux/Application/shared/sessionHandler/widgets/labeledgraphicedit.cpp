/**
 * @file labeledgraphicedit.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "labeledgraphicedit.h"

#include "../dialogs/graphicchooserdialog.h"

#include "../sessionmainwindow.h"
#include "../sessioninfo.h"

#include <QTimer>

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////
labeledGraphicEdit::labeledGraphicEdit( QWidget *parent, const QPixmap& image ) :
    _Mybase( parent, image ),
    dialogOpen_( false ),
    timer_( NULL ),
    mainWindow_( NULL )
{
    setObjectName( "hobartLabeledGraphicEdit" );

    // initialize
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
labeledGraphicEdit::~labeledGraphicEdit()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool labeledGraphicEdit::eventFilter( QObject *obj, QEvent *event )
{
    // for focus in we handle the event ourselves
    if ( child_ == obj )
        if ( QEvent::FocusIn == event->type() )
        {
            // check access level
            if ( !checkOperatorAccessLevel() )
            {
                Q_EMIT invalidAccessLevel();
            }
            else
            {
                // pop the date picker dialog
                timer_->setSingleShot( true );
                timer_->start( 0 );
            }

            // eat the event
            return true;
        }

    return _Mybase::eventFilter( obj, event );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledGraphicEdit::onSelectionChanged()
{
    // do not allow text selection in this widget
    if ( child_->hasSelectedText() )
        child_->deselect();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledGraphicEdit::onTimeout()
{
    GraphicChooserDialog dialog( mainWindow_ );

    // set graphic
    dialog.setSelectedGraphic( child_->text() );

    // open the dialog
    dialogOpen_ = true;
    updateBackgroundPalette();

    // get graphic
    if ( QDialog::Accepted == dialog.exec() )
    {
        child_->setText( dialog.selectedGraphic() );

        // emit signal
        Q_EMIT editingFinished();
    }

    // done
    dialogOpen_ = false;
    updateBackgroundPalette();

    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void labeledGraphicEdit::initialize()
{
    // create timer
    timer_ = new QTimer( this );

    connect( timer_, SIGNAL( timeout() ), SLOT( onTimeout() ) );

    // retrieve main window
    sessionInfo mySession( this );
    mainWindow_ = mySession();

    // connect signals
    connect( child_, SIGNAL( selectionChanged() ), SLOT( onSelectionChanged() ) );

    // install event filters
    child_->installEventFilter( this );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace


