/**
 * @file sessiondialog.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "sessiondialog.h"
#include "sessionmainwindow.h"
#include "sessionmainwindowimpl.h"
#include "sessionmenupanel.h"

#include "modes/modewidget.h"
#include "modes/operatemode.h"

#include "panels/panelwidget.h"

#include "screens/screenwidget.h"
#include "screens/plumainscreen.h"

#include <logmanager.h>
#include <palette.h>

#include <ScaleConfiguration.h>

#include <QApplication>
#include <QEvent>
#include <QEventLoop>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QPushButton>

bool SessionDialog::scalingTableInit_ = false;

int SessionDialog::scalingTableX_[2048];
int SessionDialog::scalingTableY_[1024];

///////////////////////////////////////////////////////////////////////////////////////////////////
SessionDialog::Overlay::Overlay( SessionDialog *dialog, QWidget *parent ) :
    _Mybase( parent ),
    dialog_( dialog )
{
    // make sure the dialog is above us
    stackUnder( dialog );

    // when clicking the overlay we dismiss the dialog
    setAttribute( Qt::WA_TransparentForMouseEvents, false );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
SessionDialog::Overlay::~Overlay()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionDialog::Overlay::paintEvent( QPaintEvent *e )
{
    Q_UNUSED( e );

    // darken the background
    QPainter painter( this );
    painter.fillRect( rect(), QColor( 0, 0, 0, 127 ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
SessionDialog::SessionDialog( DialogLocation location, QWidget *parent, Qt::WindowFlags f ) :
    _Mybase( parent, f ),
    location_( location ),
    overlay_( NULL ),
    dismissedFromOverlay_( false ),
    blocking_( false ),
    done_( false ),
    dialogAlignment_( Qt::AlignCenter ),
    dialogAlignmentOffset_( 0 ),
    dialogOffset_( 0 ),
    blockFocusNextPrev_( true )
{
    LOG_CLASS_INFO( "SessionDialog", "SessionDialog" );

    // set object name
    if ( objectName().isEmpty() )
        setObjectName( "SessionDialog" );

    // hook into session
    if ( (mainWindow_ = qobject_cast<SessionMainWindow*>( parent )) )
    {
        session_ = mainWindow_->session();

        // connect signals
        connect( session_, SIGNAL( receive(SessionMessage) ), SLOT( onSessionMessageReceiveInternal(SessionMessage) ) );
    }
    else
    {
        LOG_WARN( "parent is not main window", NULL );
        session_ = NULL;
    }

    // init scaling table
    if ( !scalingTableInit_ )
    {
        for ( int x( 2048 ); x--; )
            scalingTableX_[x] = (x * mainWindow_->width()) / SessionMainWindow::scalingDividerX_;

        for ( int y( 1024 ); y--; )
            scalingTableY_[y] = (y * mainWindow_->height()) / SessionMainWindow::scalingDividerY_;

        scalingTableInit_ = true;
    }

    // set window flags
    setWindowFlags( Qt::FramelessWindowHint );

    // install an event filter
    parent->installEventFilter( this );

    // create our overlay
    overlay_ = new Overlay( this, parent );
    overlay_->setVisible( false );

    // connect some signals
    connect( this, SIGNAL( finished(int) ), this, SLOT( onFinished(int) ) );

    // set foreground palette

    modifyPaletteForeground setForegroundPalette( modifyPaletteForeground::createPalette( hobart::colorPalette::gray ) );
    setForegroundPalette( this );


    QPalette p( palette() );
    p.setColor( QPalette::Active, QPalette::ButtonText, hobart::colorPalette::black );
    p.setColor( QPalette::Inactive, QPalette::ButtonText, hobart::colorPalette::gray );
    setPalette( p );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
SessionDialog::~SessionDialog()
{
    if ( overlay_ )
    {
        overlay_->deleteLater();
        overlay_ = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QSize SessionDialog::sizeHint() const
{
    return QSize(
        (mainWindow_->width() * 800) / SessionMainWindow::scalingDividerX_,
        (mainWindow_->height() * 550) / SessionMainWindow::scalingDividerY_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
int SessionDialog::exec()
{
    // we are blocking
    blocking_ = true;

    // open this dialog
    open();

    // wait for the finished signal
    QEventLoop eventLoop;

    do
    {
        eventLoop.processEvents( QEventLoop::WaitForMoreEvents, 50 );

    } while ( !finishedSignal_.tryLock() );

    // reset our lock
    finishedSignal_.unlock();

    // return the result
    return _Mybase::result();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionDialog::open()
{
    const ScaleConfiguration *scaleConfig( mainWindow_->session()->scaleConfig );

    if ( scaleConfig->quietDialogs )
        return;

    bool openDialog( true );

    // reset signal
    finishedSignal_.lock();

    // managed dialog
    if ( mainWindow_ )
    {
        SessionMainWindow::SessionDialogList *dialogList( &mainWindow_->dialogList_ );
        QMutex *dialogListMutex( &mainWindow_->dialogListMutex_ );

        // lock the mutex
        QMutexLocker lock( dialogListMutex );

        // add dialog to list
        dialogList->push_back( this );

        // when we are the only one queued we should open
        openDialog = (1 == dialogList->size());

        // others queued... check priorities
        if ( !openDialog )
        {
            // Note:
            // There are two cases we need to handle:
            // 1) This dialog is lower priority than what is in the list
            //    Just reject the dialog immediatley
            // 2) This dialog is higher priority than what is in the list
            //    Queue it up and reject the top dialog. When the dialog is closed the slot
            //    will handle any other dialogs in the list

            // if there are dialogs in the queue with higher priority we should reject this one
            bool shouldReject = false;

            // check priority
            foreach ( SessionDialog *queued, *dialogList )
                if ( queued->priority() > priority() )
                {
                    shouldReject = true;
                    break;
                }

            // this dialog low priority (case 1 above), reject this dialog
            if ( shouldReject )
            {
                // unlock list
                lock.unlock();

                // reject!!
                reject();
            }

            // check for lower priority (case 2 above)
            else if ( dialogList->front()->priority() < priority() )
            {
                SessionDialog *dialog = dialogList->front();

                // list has lower priority item, reject it
                lock.unlock();

                // reject!!
                dialog->reject();
            }
        }
    }

    // open the dialog
    if ( openDialog )
        openImpl();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionDialog::hideEvent( QHideEvent *event )
{
    // hide the overlay
    if ( overlay_ )
        overlay_->hide();

    // emit signal
    Q_EMIT active( false );

    _Mybase::hideEvent( event );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionDialog::paintEvent( QPaintEvent *e )
{
    Q_UNUSED( e );

    // render background
    QPainter painter( this );
    painter.drawPixmap( 0, 0, b_ );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionDialog::resizeEvent( QResizeEvent *e )
{
    Q_UNUSED( e );

    const hobart::backgroundWidgetRenderer renderer( rendererTraits(), rendererAppearance() );

    // render background
    b_ = renderer.drawControl( size(), (hobart::rendererState) rendererStyle() );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionDialog::scaleWidget( QWidget *w )
{
    QFont f( w->font() );

    //
    // set new geometry
    //

    const QRect r( w->geometry() );

    const int width( scalingTableX_[r.width()] );
    const int height( scalingTableY_[r.height()] );


    //some screens use negative values
    int top(r.top());
    int left(r.left());
    if(top < 0){
        top = -scalingTableY_[-top];
    } else {
        top = scalingTableY_[top];
    }
    if(left < 0){
        left = -scalingTableX_[-left];
    } else {
        left = scalingTableX_[left];
    }

    w->setGeometry( left, top, width, height );

    if ( w->minimumWidth() == w->maximumWidth() )
        w->setFixedWidth( width );

    if ( w->minimumHeight() == w->maximumHeight() )
        w->setFixedHeight( height );



    //
    // adjust all children
    //
    const QObjectList children( w->children() );



    f.setPointSize( scalingTableY_[f.pointSize()] );


    foreach ( QObject *child, children )
        if ( child->isWidgetType() )
            scaleWidget( qobject_cast<QWidget*>( child ) );

    //set the font at the end to avoid a cascading increase in point size to the children.
    w->setFont( f );

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionDialog::showEvent( QShowEvent *event )
{
    // show the overlay
    if ( overlay_ )
        overlay_->show();

    // size the dialog
    // 1) for menu panel just use size hint
    // 2) otherwise groom
    if ( qobject_cast<SessionMenuPanel*>( this ) )
        setFixedSize( sizeHint() );

    else if ( mainWindow_ )
    {
        QSize s( sizeHint() );

        // for main run screen we should popup in the dynamic content area
        if (( mainWindow_->isOperateMode() ) && ( allowDynamicContentArea() ))
        {
            const PluMainScreen *screen( qobject_cast<PluMainScreen*>( mainWindow_->mode()->screen() ) );

            if ( screen )
            {
                QRect screenDcaGeometry( screen->dcaGeometry() );
                screenDcaGeometry.adjust( -5, -5, 5, 5 );

                // set dialog location
                if ( Qt::LeftDockWidgetArea == mainWindow_->numberPadArea() )
                {
                    setDialogOffset( mainWindow_->width() - screenDcaGeometry.right() );
                    setLocation( RightDialogLocation );
                }
                else
                {
                    setDialogOffset( screenDcaGeometry.left() );
                    setLocation( LeftDialogLocation );
                }

                const QPoint pos( screen->mapTo( mainWindow_, screenDcaGeometry.topLeft() ) );

                // set dialog alignment
                setDialogAlignment( Qt::AlignBottom );
                setDialogAlignmentOffset( mainWindow_->height() - pos.y() - screenDcaGeometry.height() );

                // set dialog size
                s = screenDcaGeometry.size();
            }
        }

        setFixedSize( s );
    }

    // position the dialog
    move( calcPosition() );

    // hide input widgets
    if ( mainWindow_ )
    {
        mainWindow_->hideInputWidgets();

        // manage z-order of overlays
        if ( overlay_ )
            overlay_->stackUnder( mainWindow_->overlay_ );
    }

    // emit signal
    Q_EMIT active( true );

    _Mybase::showEvent( event );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool SessionDialog::eventFilter( QObject *obj, QEvent *event )
{
    if ( parent() == obj )
    {
        // parent is resizing
        if ( QEvent::Resize == event->type() )
        {
            // position the dialog
            move( calcPosition() );
        }

        // mouse button press
        else if (( QEvent::MouseButtonPress == event->type() ) && ( isVisible() ))
        {
            bool isMenuPanel = qobject_cast<SessionMenuPanel*>( this ) ? true : false;

            QRect dialogRect( geometry() );

            // we need to check for mouse click outside the dialog, there are two cases:
            // 1) we are a menu panel
            //    here we need to iterate through all the child panel widgets and check for a
            //    mouse click inside each panel
            // 2) we are a dialog
            //    here we can just check the dialog geometry

            // panel widget (case 1)
            if ( isMenuPanel )
            {
                bool found( false );

                // check each child panel
                foreach ( const QObject *child, children() )
                {
                    const PanelWidget *panel( qobject_cast<const PanelWidget*>( child ) );

                    if ( panel )
                    {
                        const QRect rect( dialogRect.topLeft() + panel->geometry().topLeft(), panel->size() );

                        // check for mouse click inside the dialog
                        if ( rect.contains( dynamic_cast<QMouseEvent*>( event )->pos() ) )
                        {
                            found = true;
                            break;
                        }
                    }
                }

                // did not find click within child widget
                if (( !found ) && ( allowOverlayClickDismiss() ))
                {
                    dismissedFromOverlay_ = true;

                    reject();
                    return true;
                }
            }

            // dialog (case 2)
            else
            {
                // check for mouse click outside the dialog
                if ( !dialogRect.contains( dynamic_cast<QMouseEvent*>( event )->pos() ) )
                    if ( allowOverlayClickDismiss() )
                    {
                        dismissedFromOverlay_ = true;

                        reject();
                        return true;
                    }
            }
        }
    }

    return _Mybase::eventFilter( obj, event );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool SessionDialog::focusNextChild()
{
    // FT-595
    if ( blockFocusNextPrev_ )
        return true;

    return _Mybase::focusNextChild();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool SessionDialog::focusNextPrevChild( bool next )
{
    // FT-595
    if ( blockFocusNextPrev_ )
        return true;

    return _Mybase::focusNextPrevChild( next );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool SessionDialog::focusPreviousChild()
{
    // FT-595
    if ( blockFocusNextPrev_ )
        return true;

    return _Mybase::focusPreviousChild();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
hobart::widgetRendererAppearanceBase *SessionDialog::rendererAppearance() const
{
    return new hobart::lightOperatorBackgroundWidgetRendererAppearance();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
hobart::backgroundWidgetBase *SessionDialog::rendererTraits() const
{
    return new hobart::rectangleBackgroundWidget();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionDialog::onSessionMessageReceive( const SessionMessage& msg )
{
    Q_UNUSED( msg );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionDialog::onFinished( int result )
{
    Q_UNUSED( result );

    bool restoreNumberPad( false );

    // nothing to do (we already finished)
    if ( done_ )
        return;

    done_ = true;

    // mark dialog for deletion
    if ( !blocking_ )
        deleteLater();

    // non-managed dialog
    if ( !mainWindow_ )
    {
        // signal caller we are finished
        finishedSignal_.unlock();
    }

    // managed dialog
    else
    {
        SessionMainWindow::SessionDialogList *dialogList( &mainWindow_->dialogList_ );
        QMutex *dialogListMutex( &mainWindow_->dialogListMutex_ );

        // lock the mutex
        QMutexLocker lock( dialogListMutex );

        // Note:
        // There are two cases we need to handle:
        // 1) We are closing a dialog at the head of the list
        //    For this case we need to close the dialog and open the next dialog. We need to be
        //    carefull here, because the next dialog is really the next dialog we should show is
        //    the one with highest priority.
        // 2) We are closing a dialog in the middle of the list
        //    This is the easy case, just remove the dialog from the list. The dialog being removed
        //    is not being displayed.

        // check if we are at front of list
        bool isVisible( (dialogList->empty() ? false : (this == dialogList->front())) );

        // if there are dialogs after this one with a higher priority we should reject next in line
        SessionDialog *dialog( NULL );
        bool shouldReject( false );

        // pop this dialog off stack
        dialogList->removeOne( this );

        // when no more dialogs we should restore number pad
        restoreNumberPad = dialogList->empty();

        // retrieve next dialog and check priorities (case 1 above)
        if (( isVisible ) && ( dialogList->size() ))
        {
            dialog = dialogList->front();

            // check priority
            foreach ( SessionDialog *queued, *dialogList )
                if ( queued->priority() > dialog->priority() )
                {
                    shouldReject = true;
                    break;
                }

            // open dialog
            if ( !shouldReject )
            {
                // unlock
                lock.unlock();

                // open!!
                dialog->openImpl();
                dialog->setFocus();
            }
        }

        // signal caller we are finished
        finishedSignal_.unlock();

        // reject dialog
        if ( shouldReject )
        {
            // unlock
            lock.unlock();

            // reject!!
            dialog->reject();
        }
    }

    // for operate mode check if we should show the number pad
    if (( restoreNumberPad ) && ( mainWindow_ ) && ( mainWindow_->isOperateMode() ))
    {
        OperateMode *operateMode = mainWindow_->retrieveMode<OperateMode>();

        bool restoreNumberPad( !operateMode->isForcedFields() );

        // continue processing forced fields!
        operateMode->processForcedFields();

        // restore numberpad when no forced fields
        if ( restoreNumberPad )
            operateMode->setFocusForPopupNumberPad();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionDialog::onSessionMessageReceiveInternal( const SessionMessage& msg )
{
    onSessionMessageReceive( msg );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QPoint SessionDialog::calcPosition() const
{
    const int hLeft( 0 );
    const int hRight( parentWidget()->width() - width() );
    const int hCenter( hRight / 2 );

    const int vTop( 0 );
    const int vBottom( parentWidget()->height() - height() );
    const int vCenter( vBottom / 2 );

    switch ( location() )
    {
    case BottomDialogLocation:
        if ( Qt::AlignLeft & dialogAlignment_ )
            return QPoint( hLeft + dialogAlignmentOffset_, vBottom - dialogOffset_ );
        else if ( Qt::AlignRight & dialogAlignment_ )
            return QPoint( hRight - dialogAlignmentOffset_, vBottom - dialogOffset_ );
        return QPoint( hCenter, vBottom - dialogOffset_ );

    case CenterDialogLocation:
        return QPoint( hCenter, vCenter );

    case LeftDialogLocation:
        if ( Qt::AlignTop & dialogAlignment_ )
            return QPoint( hLeft + dialogOffset_, vTop + dialogAlignmentOffset_ );
        else if ( Qt::AlignBottom & dialogAlignment_ )
            return QPoint( hLeft + dialogOffset_, vBottom - dialogAlignmentOffset_ );
        return QPoint( hLeft + dialogOffset_, vCenter );

    case RightDialogLocation:
        if ( Qt::AlignTop & dialogAlignment_ )
            return QPoint( hRight - dialogOffset_, vTop + dialogAlignmentOffset_ );
        else if ( Qt::AlignBottom & dialogAlignment_ )
            return QPoint( hRight - dialogOffset_, vBottom - dialogAlignmentOffset_ );
        return QPoint( hRight - dialogOffset_, vCenter );

    case TopDialogLocation:
        if ( Qt::AlignLeft & dialogAlignment_ )
            return QPoint( hLeft + dialogAlignmentOffset_, vTop + dialogOffset_ );
        else if ( Qt::AlignRight & dialogAlignment_ )
            return QPoint( hRight - dialogAlignmentOffset_, vTop + dialogOffset_ );
        return QPoint( hCenter, vTop + dialogOffset_ );

    default:
        break;
    }

    return QPoint();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionDialog::openImpl()
{
    _Mybase::open();
    done_ = false;
}
