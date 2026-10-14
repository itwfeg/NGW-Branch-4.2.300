/**
 * @file screenwidget.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include <logmanager.h>

#include "screenwidget.h"

#include "../sessionmainwindow.h"

#include "../modes/modewidget.h"

#include <QApplication>
#include <QMouseEvent>
#include <QLine>

#include <QPropertyAnimation>
#include <QParallelAnimationGroup>

const int ScreenWidget::marginSpacing_ = 30;


// these settings have no bounce
const int ScreenWidget::animationSpeed_( 250 );
const QEasingCurve::Type ScreenWidget::animationType_( QEasingCurve::Linear );

// these settings will bounce
//const int ScreenWidget::animationSpeed_( 500 );
//const QEasingCurve::Type ScreenWidget::animationType_( QEasingCurve::OutBack );

bool ScreenWidget::animating_ = false;
bool ScreenWidget::scalingTableInit_ = false;

int ScreenWidget::scalingTableX_[2048];
int ScreenWidget::scalingTableY_[1024];

///////////////////////////////////////////////////////////////////////////////////////////////////
ScreenWidget::ScreenWidget( SessionMainWindow *mainWindow, ModeWidget *parent ) :
    _Mybase( parent ),
    mainWindow_( mainWindow ),
    mode_( parent ),
    screen_( NULL ),
    scaling_( false )
{
    // screens take up as much room as possible
    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

    // retrieve session
    session_ = mainWindow_->session();

    // connect into session
    connect( session_, SIGNAL( receive(SessionMessage) ), this, SLOT( onSessionMessageReceive(SessionMessage) ) );

    // init scaling table
    if ( !scalingTableInit_ )
    {
        for ( int x( 2048 ); x--; )
            scalingTableX_[x] = (x * mainWindow->width()) / SessionMainWindow::scalingDividerX_;

        for ( int y( 1024 ); y--; )
            scalingTableY_[y] = (y * mainWindow->height()) / SessionMainWindow::scalingDividerY_;

        scalingTableInit_ = true;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
ScreenWidget::~ScreenWidget()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool ScreenWidget::helpText( QString& title, QString& description ) const
{
    Q_UNUSED( title );
    Q_UNUSED( description );

    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QMargins ScreenWidget::margins() const
{
    return QMargins();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenWidget::translate()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool ScreenWidget::handleSwipe( SwipeDirection direction )
{
    LOG_CLASS_INFO( "ScreenWidget", "handleSwipe" );

    LOG_TRACE( "handling swipe of direction %d", direction );

    // we are already animating (in a swipe)!
    if ( animating_ )
    {
        LOG_DEBUG( "swipe while already processing a swipe... ignoring...", NULL );
        return true;
    }

    screen_ = NULL;

    // determine swipe direction and screen
    if ( Down == direction )
        screen_ = swipeScreenDown();
    else if ( Left == direction )
        screen_ = swipeScreenLeft();
    else if ( Right == direction )
        screen_ = swipeScreenRight();
    else if ( Up == direction )
        screen_ = swipeScreenUp();
    else
    {
        LOG_DEBUG( "no screen to swipe into", NULL );
    }

    // no screen to process
    if ( !screen_ )
        return false;

    LOG_INFO( "swiping to screen '%s'", screen_->objectName().toLatin1().constData() );

    // we must unload the current screen first!
    if ( onScreenChanging() )
        animating_ = true;
    else
    {
        LOG_WARN( "current screen '%s' may not be unloaded!", objectName().toLatin1().constData() );

        screen_->deleteLater();
        screen_ = NULL;

        return false;
    }

    QRect rect( geometry() );

    // save positions
    QPoint leftPos( -rect.width(), rect.top() );
    QPoint lowerPos( 0, rect.bottom() );
    QPoint middlePos( 0, rect.top() );
    QPoint rightPos( rect.width(), rect.top() );
    QPoint upperPos( 0, rect.top() - rect.height() );

    // setup screen animations
    QPropertyAnimation *animationOld = new QPropertyAnimation( this, "pos" );
    QPropertyAnimation *animationNew = new QPropertyAnimation( screen_, "pos" );

    animationOld->setDuration( animationSpeed_ );
    animationOld->setEasingCurve( animationType_ );

    animationNew->setDuration( animationSpeed_ );
    animationNew->setEasingCurve( animationType_ );

    // position new screen just outside viewable so it can be moved/animated in
    switch ( direction )
    {
    case Down:
        screen_->setGeometry( QRect( upperPos, rect.size() ) );

        animationOld->setStartValue( middlePos );
        animationOld->setEndValue( lowerPos );

        animationNew->setStartValue( upperPos );
        animationNew->setEndValue( middlePos );

        break;

    case Left:
        screen_->setGeometry( QRect( rightPos, rect.size() ) );

        animationOld->setStartValue( middlePos );
        animationOld->setEndValue( leftPos );

        animationNew->setStartValue( rightPos );
        animationNew->setEndValue( middlePos );

        break;

    case Right:
        screen_->setGeometry( QRect( leftPos, rect.size() ) );

        animationOld->setStartValue( middlePos );
        animationOld->setEndValue( rightPos );

        animationNew->setStartValue( leftPos );
        animationNew->setEndValue( middlePos );

        break;

    case Up:
        screen_->setGeometry( QRect( lowerPos, rect.size() ) );

        animationOld->setStartValue( middlePos );
        animationOld->setEndValue( upperPos );

        animationNew->setStartValue( lowerPos );
        animationNew->setEndValue( middlePos );

        break;

    default:
        break;
    }

    // setup the animaion
    QParallelAnimationGroup *animation = new QParallelAnimationGroup;
    animation->addAnimation( animationOld );
    animation->addAnimation( animationNew );

    connect( animation, SIGNAL( finished() ), this, SLOT( onAnimationFinished() ) );

    // go!!
    LOG_DEBUG( "starting swipe!", NULL );

    mainWindow_->hideInputWidgets();
    screen_->show();

    animation->start( QAbstractAnimation::DeleteWhenStopped );

    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool ScreenWidget::eventFilter( QObject *obj, QEvent *e )
{
    if ( scaling_ )
    {
        if ( obj == this )
            return true;
        else if (( obj->isWidgetType() ) && ( isAncestorOf( qobject_cast<QWidget*>( obj ) ) ))
            return true;
    }

    return _Mybase::eventFilter( obj, e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool ScreenWidget::onScreenChanging()
{
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenWidget::onSessionMessage( const SessionMessage& msg )
{
    Q_UNUSED( msg );
}



///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenWidget::scaleScreen()
{
    // nothing to do
    if ( mainWindow_->width() <= SessionMainWindow::scalingDividerX_ )
        return;
    else if ( mainWindow_->height() <= SessionMainWindow::scalingDividerY_ )
        return;

    // block all events via event filter
    qApp->installEventFilter( this );
    scaling_ = true;

    // scale widget
    scaleWidget( this );

    //qApp->processEvents();

    // stop blocking events
    qApp->removeEventFilter( this );
    scaling_ = false;

    // force update
    update();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QPoint ScreenWidget::scalePointForScreen( const QPoint &value ) const
{
    return QPoint( scalingTableX_[value.x()], scalingTableY_[value.y()] );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QRect ScreenWidget::scaleRectForScreen( const QRect &value ) const
{
    return QRect(
        scalePointForScreen( value.topLeft() ),
        scaleSizeForScreen( value.size() ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
QSize ScreenWidget::scaleSizeForScreen( const QSize &value ) const
{
    return QSize( scalingTableX_[value.width()], scalingTableY_[value.height()] );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenWidget::onSessionMessageReceive( const SessionMessage& msg )
{
    // call derived class method
    onSessionMessage( msg );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenWidget::onAnimationFinished()
{
    LOG_CLASS_INFO( "ScreenWidget", "onAnimationFinished" );

    LOG_TRACE( "swipe complete", NULL );
    animating_ = false;

    // set the new screen
    mode_->setScreen( screen_ );

    // cleanup
    screen_ = NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenWidget::scaleWidget( QWidget *w )
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
void ScreenWidget::expandTabs(QTabWidget *tabWidget){

    //Note: the blue border (2px) that surrounds most tabs is actually
    //      rendered outside of the traditional tab bounds, making it
    //      4px wider than it ordinarily would be.


    int tabCount = tabWidget->count();
    int scaledWidth = (tabWidget->contentsRect().width()*96)/100;
    int tabWidth = (scaledWidth/tabCount);
    int offset = ((scaledWidth - (tabWidth * tabCount))/2) + (tabCount*4);
    QString tabWidthPx = QString("%1px").arg(tabWidth);


    tabWidget->setStyleSheet(tabWidget->styleSheet().append(QString("\nQTabBar::tab { min-width: %1; }").arg(tabWidthPx).arg(offset)));

}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenWidget::hideAll(QWidget *widget){
    widget->setVisible(false);
    widget->hide();
    const QObjectList children(widget->children());
    foreach(QObject *child, children)
        if( child->isWidgetType())
            hideAll(qobject_cast<QWidget*>(child));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void ScreenWidget::fixComboBoxStyleSheet(QComboBox *cboTemp){

    //for setup utility (and maybe advanced diagnostics)
    //
    int cboHeight = cboTemp->contentsRect().height();
    QString cboDimensions = QString("QComboBox#%1::drop-down { min-width: %2px; height: %2px; }").arg(cboTemp->objectName()).arg(cboHeight);
    cboTemp->setStyleSheet(cboTemp->styleSheet().append(cboDimensions));

}
