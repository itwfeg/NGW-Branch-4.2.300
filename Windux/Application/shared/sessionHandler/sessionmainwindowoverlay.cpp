/**
 * @file sessionmainwindowoverlay.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "sessionmainwindow.h"
#include "sessionmainwindowimpl.h"

#include <ScaleConfiguration.h>
#include <UiUtilities.h>

#include <pixmap.h>

#include <QApplication>
#include <QMouseEvent>
#include <QPainter>

const QMargins SessionMainWindow::Overlay::buttonWhitespace_( 15, 18, 15, 18 );

///////////////////////////////////////////////////////////////////////////////////////////////////
SessionMainWindow::Overlay::Overlay( Qt::DockWidgetArea area, QWidget *parent ) :
    _Mybase( parent ),
    mainWindow_( qobject_cast<SessionMainWindow*>( parent ) ),
    area_( area ),
    bottomMargin_( 10 ),
    openCounter_( 0 ),
    dialogIconCounter_( 0 ),
    messageVisible_( false ),
    ejectMediaVisible_( false )
{
    flipPixmap horizontalPixmapFlipper( true );

    QPixmap p;
    QRect r;

    // load button pixmaps
    button_.insert( makeButtonId( Qt::LeftDockWidgetArea, NormalPanelButtonState ), QPixmap( ":/images/dockArrowLeftMenu.png" ) );
    button_.insert( makeButtonId( Qt::LeftDockWidgetArea, OpenPanelButtonState ), QPixmap( ":/images/dockArrowLeftMenuPressed.png" ) );
    button_.insert( makeButtonId( Qt::LeftDockWidgetArea, NormalHomeButtonState ), QPixmap( ":/images/dockArrowLeftHome.png" ) );
    button_.insert( makeButtonId( Qt::LeftDockWidgetArea, OverlayHomeButtonState ), QPixmap( ":/images/dockArrowLeftHome.png" ) );
    button_.insert( makeButtonId( Qt::LeftDockWidgetArea, NormalZeroScaleButtonState ), QPixmap( ":/images/dockArrowLeftRezero.png" ) );
    button_.insert( makeButtonId( Qt::LeftDockWidgetArea, NormalEjectMediaButtonState ), QPixmap( ":/images/ejectButton.png" ) );

    button_.insert( makeButtonId( Qt::RightDockWidgetArea, NormalPanelButtonState ), QPixmap( ":/images/dockArrowRightMenu.png" ) );
    button_.insert( makeButtonId( Qt::RightDockWidgetArea, OpenPanelButtonState ), QPixmap( ":/images/dockArrowRightMenuPressed.png" ) );
    button_.insert( makeButtonId( Qt::RightDockWidgetArea, NormalHomeButtonState ), QPixmap( ":/images/dockArrowRightHome.png" ) );
    button_.insert( makeButtonId( Qt::RightDockWidgetArea, OverlayHomeButtonState ), QPixmap( ":/images/dockArrowRightHome.png" ) );
    button_.insert( makeButtonId( Qt::RightDockWidgetArea, NormalZeroScaleButtonState ), QPixmap( ":/images/dockArrowRightRezero.png" ) );
    button_.insert( makeButtonId( Qt::RightDockWidgetArea, NormalEjectMediaButtonState ), QPixmap( ":/images/ejectButton.png" ) );

    // panel button size
    buttonInfo( PanelButtonType, p, r );
    r = p.rect();

    buttonRect_.insert( makeButtonRectId( Qt::LeftDockWidgetArea, PanelButtonType ), r );
    buttonRect_.insert( makeButtonRectId( Qt::RightDockWidgetArea, PanelButtonType ), r );

    // home button size
    buttonInfo( HomeButtonType, p, r );
    r = p.rect();

    buttonRect_.insert( makeButtonRectId( Qt::LeftDockWidgetArea, HomeButtonType ), r );
    buttonRect_.insert( makeButtonRectId( Qt::RightDockWidgetArea, HomeButtonType ), r );

    // zero scale button size
    buttonInfo( ZeroScaleButtonType, p, r );
    r = p.rect();

    buttonRect_.insert( makeButtonRectId( Qt::LeftDockWidgetArea, ZeroScaleButtonType ), r );
    buttonRect_.insert( makeButtonRectId( Qt::RightDockWidgetArea, ZeroScaleButtonType ), r );

    // eject media button size
    buttonInfo( EjectMediaButtonType, p, r );
    r = p.rect();

    buttonRect_.insert( makeButtonRectId( Qt::LeftDockWidgetArea, EjectMediaButtonType ), r );
    buttonRect_.insert( makeButtonRectId( Qt::RightDockWidgetArea, EjectMediaButtonType ), r );

    // load icon pixmaps
    icon_.insert( makeIconId( Qt::LeftDockWidgetArea, MinimizeIconType ), QPixmap( ":/images/dockMinimizeIcon.png" ) );
    icon_.insert( makeIconId( Qt::LeftDockWidgetArea, RestoreIconType ), QPixmap( ":/images/dockMaximizeIcon.png" ) );

    icon_.insert( makeIconId( Qt::RightDockWidgetArea, MinimizeIconType ), horizontalPixmapFlipper( QPixmap( ":/images/dockMinimizeIcon.png" ) ) );
    icon_.insert( makeIconId( Qt::RightDockWidgetArea, RestoreIconType ), horizontalPixmapFlipper( QPixmap( ":/images/dockMaximizeIcon.png" ) ) );

    // icon size
    r = QRect( QPoint( 0, 0 ), QSize( 28, 28 ) );
    iconRect_.insert( makeIconRectId( Qt::LeftDockWidgetArea ), r );
    iconRect_.insert( makeIconRectId( Qt::RightDockWidgetArea ), r );

    // calculate graphic rects
    calcGraphicRects();

    // font
    QFont f( font() );
    f.setPointSize( (mainWindow_->height() * 9) / SessionMainWindow::scalingDividerY_ );
    setFont( f );

    // install event filter
    qApp->installEventFilter( this );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
SessionMainWindow::Overlay::~Overlay()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::Overlay::setArea( Qt::DockWidgetArea value )
{
    if ( area_ == value )
        return;

    // set area
    area_ = value;

    // repaint
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::Overlay::setBottomMargin( int value )
{
    if ( bottomMargin_ == value )
        return;

    // set margin
    bottomMargin_ = value;

    // calculate new rects and update
    calcGraphicRects();
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::Overlay::setOpen( bool value )
{
    // update open counter
    if ( value )
        ++openCounter_;
    else
        --openCounter_;

    // repaint
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::Overlay::setDialogIconVisible( bool value )
{
    // update help icon counter
    if ( value )
        ++dialogIconCounter_;
    else
        --dialogIconCounter_;

    // repaint
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::Overlay::setMessageVisible( bool value, const QString& text )
{
    // update flags
    messageVisible_ = value;
    message_ = text;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool SessionMainWindow::Overlay::eventFilter( QObject *obj, QEvent *event )
{
    QWidget *widget( qobject_cast<QWidget*>( obj ) );

    // only handle children of main window
    if (( widget ) && ( mainWindow_->isAncestorOf( widget ) ))
    {
        // handle mouse events
        if ( QEvent::MouseButtonPress == event->type() )
        {
            QMouseEvent *mouseEvent( dynamic_cast<QMouseEvent*>( event ) );

            // retrieve mouse click position
            const QPoint pos( widget->mapTo( mainWindow_, mouseEvent->pos() ) );

            // check for menu button press
            if ( handleMenuButtonPress( pos ) )
                return true;
        }
    }

    return _Mybase::eventFilter( obj, event );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::Overlay::paintEvent( QPaintEvent *event )
{
    Q_UNUSED( event )

    QPixmap p;
    QRect r;

    QPainter painter( this );
    painter.setRenderHint( QPainter::Antialiasing );

    // draw scale zero button
    buttonInfo( ZeroScaleButtonType, p, r );
    painter.drawPixmap( r, p );

    // draw panel button
    buttonInfo( PanelButtonType, p, r );
    painter.drawPixmap( r, p );

    // draw home button
    buttonInfo( HomeButtonType, p, r );
    painter.drawPixmap( r, p );

    // draw eject media button
    if ( ejectMediaVisible_ )
    {
        buttonInfo( EjectMediaButtonType, p, r );
        painter.drawPixmap( r, p );
    }

    // draw icon
    if ( iconInfo( p, r ) )
        painter.drawPixmap( r, p );

    // draw status message
    if (( messageVisible_ ) && ( message_.length() ))
    {
        QFontMetrics fontMetrics( font() );

        QRect boundingRect( fontMetrics.boundingRect( message_ ) );
        boundingRect.moveRight( width() );
        boundingRect.moveTop( 0 );

        painter.fillRect( boundingRect, Qt::white );
        painter.drawText( boundingRect, 0, message_ );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::Overlay::resizeEvent( QResizeEvent *event )
{
    Q_UNUSED( event )

    // calculate new rects and update
    calcGraphicRects();
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::Overlay::calcGraphicRects()
{
    if ( !mainWindow_ )
        return;

    int mainWindowWidth = mainWindow_->contentsRect().width();
    int mainWindowHeight = mainWindow_->contentsRect().height();

    int bottom = mainWindowHeight - bottomMargin_;

    // amount to adjust menu button rectangle by (remove the whitespace)
    int adjustment = buttonWhitespace_.top() + buttonWhitespace_.bottom();

    QRect temp;
    QRect *h;
    QRect *p;
    QRect *z;
    QRect *i;

    //
    // home button
    //

    // left
    h = &buttonRect_[ makeButtonRectId( Qt::LeftDockWidgetArea, HomeButtonType ) ];
    h->moveTo( 10 - buttonWhitespace_.left(), bottom - h->height() );

    // right
    h = &buttonRect_[ makeButtonRectId( Qt::RightDockWidgetArea, HomeButtonType ) ];
    h->moveTo( mainWindowWidth - (10 - buttonWhitespace_.right()) - h->width(), bottom - h->height() );

    //
    // panel button
    //

    // left
    p = &buttonRect_[ makeButtonRectId( Qt::LeftDockWidgetArea, PanelButtonType ) ];
    p->moveTo( 10 - buttonWhitespace_.left(), h->top() - p->height() + adjustment );

    // right
    p = &buttonRect_[ makeButtonRectId( Qt::RightDockWidgetArea, PanelButtonType ) ];
    p->moveTo( mainWindowWidth - (10 - buttonWhitespace_.right()) - p->width(), h->top() - p->height() + adjustment );

    //
    // zero scale button
    //

    // left
    z = &buttonRect_[ makeButtonRectId( Qt::LeftDockWidgetArea, ZeroScaleButtonType ) ];
    z->moveTo( 10 - buttonWhitespace_.left(), p->top() - z->height() + adjustment );

    // right
    z = &buttonRect_[ makeButtonRectId( Qt::RightDockWidgetArea, ZeroScaleButtonType ) ];
    z->moveTo( mainWindowWidth - (10 - buttonWhitespace_.right()) - z->width(), p->top() - z->height() + adjustment );

    //
    // eject media button
    //

    // left
    temp = buttonRect_[ makeButtonRectId( Qt::LeftDockWidgetArea, HomeButtonType ) ];
    buttonRect_[ makeButtonRectId( Qt::LeftDockWidgetArea, EjectMediaButtonType ) ] = QRect(
        QPoint( temp.left() + ((temp.width() - 32) / 2), mainWindowHeight - 32 ),
        QSize( 32, 32 ) );

    // right
    temp = buttonRect_[ makeButtonRectId( Qt::RightDockWidgetArea, HomeButtonType ) ];
    buttonRect_[ makeButtonRectId( Qt::RightDockWidgetArea, EjectMediaButtonType ) ] = QRect(
        QPoint( temp.left() + ((temp.width() - 32) / 2), mainWindowHeight - 32 ),
        QSize( 32, 32 ) );

    //
    // icons
    //

    // left
    temp = buttonRect_[ makeButtonRectId( Qt::LeftDockWidgetArea, HomeButtonType ) ];
    i = &iconRect_[ makeIconRectId( Qt::LeftDockWidgetArea ) ];
    i->moveTo( temp.left() + ((temp.width() - i->width()) / 2), temp.bottom() - i->height() - adjustment );

    // right
    temp = buttonRect_[ makeButtonRectId( Qt::RightDockWidgetArea, HomeButtonType ) ];
    i = &iconRect_[ makeIconRectId( Qt::RightDockWidgetArea ) ];
    i->moveTo( temp.left() + ((temp.width() - i->width()) / 2), temp.bottom() - i->height() - adjustment );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool SessionMainWindow::Overlay::handleMenuButtonPress( const QPoint& pos )
{
    ScaleConfiguration *config = mainWindow_->session()->scaleConfig;

    // handle icon click
    IconType t = currentIconType();

    if (( t ) && ( iconRect_[ makeIconRectId( area() ) ].contains( pos ) ))
    {
        // minimize dialog currently on screen
        if ( MinimizeIconType == t )
            mainWindow_->minimizeHelpDialog();

        // reject whatever is on the screen and restore dialog
        else if ( RestoreIconType == t )
        {
            bool handle = true;

            // collapse panel
            if ( open() )
                mainWindow_->rejectDialog();

            // try to reject current open dialog
            else if ( mainWindow_->isDialogOpen() )
                handle = mainWindow_->rejectDialog();

            // restore help
            if ( handle )
                mainWindow_->restoreHelpDialog();
        }

        return true;
    }

    // handle eject media button click
    if ( ejectMediaVisible_ )
    {
        QRect ejectMediaButtonRect( buttonRect_[ makeButtonRectId( area(), EjectMediaButtonType ) ] );

        if ( ejectMediaButtonRect.contains( pos ) )
        {
            mainWindow_->ejectMedia();
            return true;
        }
    }

    // handle menu button click
    QList<ButtonType> menuButtons;
    menuButtons.push_back( ZeroScaleButtonType );
    menuButtons.push_back( PanelButtonType );
    menuButtons.push_back( HomeButtonType );

    foreach ( ButtonType b, menuButtons )
    {
        QRect r( buttonRect_[ makeButtonRectId( area(), b ) ] );

        // allow click in blank area to trigger button
        if ( Qt::LeftDockWidgetArea == area() )
            r.adjust( 0, buttonWhitespace_.top(), -buttonWhitespace_.right(), -buttonWhitespace_.bottom() );
        else if ( Qt::RightDockWidgetArea == area() )
            r.adjust( buttonWhitespace_.left(), buttonWhitespace_.top(), 0, -buttonWhitespace_.bottom() );

        if ( r.contains( pos ) )
        {
            bool handle = false;

            // collapse panel
            if ( open() )
                mainWindow_->rejectDialog();

            // try to reject current open dialog
            else if ( mainWindow_->isDialogOpen() )
                handle = mainWindow_->rejectDialog();

            // handle button press
            else
            {
                handle = true;
            }

            if ( handle )
            {
                switch ( b )
                {
                case ZeroScaleButtonType:
                    mainWindow_->session()->uiUtilities->rezeroWeigher();
                    break;
                case PanelButtonType:
                    mainWindow_->showMenuPanel();
                    break;
                case HomeButtonType:
                    mainWindow_->setIsHeadingToHome( true );
                    config->internalConfig.updateNavigateHomeFlashkeys = true;
                    mainWindow_->navigateHome();
                    break;
                default:
                    break;
                }
            }

            return true;
        }
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SessionMainWindow::Overlay::buttonInfo( ButtonType t, QPixmap& pixmap, QRect& rect ) const
{
    LOG_CLASS_INFO( "SessionMainWindowOverlay", "buttonInfo" );

    ButtonState s = NormalPanelButtonState;

    // figure out state
    if ( ZeroScaleButtonType == t )
    {
        s = NormalZeroScaleButtonState;
    }
    else if ( PanelButtonType == t )
    {
        if ( open() )
            s = OpenPanelButtonState;
        else
            s = NormalPanelButtonState;
    }
    else if ( HomeButtonType == t )
    {
        if ( currentIconType() )
            s = OverlayHomeButtonState;
        else
            s = NormalHomeButtonState;
    }
    else if ( EjectMediaButtonType == t )
    {
        s = NormalEjectMediaButtonState;
    }
    else
    {
        LOG_ERROR( "bad button type %d", t );
        return;
    }

    // return the info
    pixmap = button_[ makeButtonId( area(), s ) ];
    rect = buttonRect_[ makeButtonRectId( area(), t ) ];
}

///////////////////////////////////////////////////////////////////////////////////////////////////
SessionMainWindow::Overlay::IconType SessionMainWindow::Overlay::currentIconType() const
{
    if ( dialogIconVisible() )
        return (mainWindow_->isWebBrowserVisible() ? MinimizeIconType : RestoreIconType);

    return NoIconType;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool SessionMainWindow::Overlay::iconInfo( QPixmap& pixmap, QRect& rect ) const
{
    IconType t( currentIconType() );

    // check for icon to display
    if ( NoIconType == t )
        return false;

    // return the info
    pixmap = icon_[ makeIconId( area(), t ) ];
    rect = iconRect_[ makeIconRectId( area() ) ];

    return true;
}
