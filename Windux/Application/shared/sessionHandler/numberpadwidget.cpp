/**
 * @file numberpadwidget.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "numberpadwidget.h"

#include "sessionmainwindow.h"
#include "sessionmainwindowimpl.h"
#include "sessioninfo.h"

#include "modes/operatemode.h"

#include <backgroundwidgettypes.h>
#include <logmanager.h>
#include <pushbutton.h>

#include <Messages.h>
#include <ScaleConfiguration.h>
#include <UiUtilities.h>

#include <QApplication>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QSignalMapper>
#include <QTimer>
#include <QVBoxLayout>

static const QString KEY_BACKSPACE( "backspace" );
static const QString KEY_CLEAR( "clear" );
static const QString KEY_PRINT( "print" );
static const QString KEY_RETURN( "return" );

static const QString KEY_SEARCH( "search" );
static const QString KEY_BACKSPACE_SMALL( "backspacesmall" );

const QMargins NumberPadWidget::margins_( 10, 0, 10, 10 );

const QSize NumberPadWidget::buttonIconSize_( 64, 64 );
const int NumberPadWidget::buttonFontPointSize_ = 48;

const int NumberPadWidget::layoutSpacing_( 10 );

///////////////////////////////////////////////////////////////////////////////////////////////////
NumberPadWidget::NumberPadWidget( QWidget *parent ) :
    _Mybase( parent ),
    signalMapper_( NULL ),
    keyLayoutStyle_( (KeyLayoutStyle) -1 ),
    keyLayoutStyleHasChanged_( false ),
    moving_( false ),
    isDeliVisionLicensed_( false ),
    operateMode_( NULL )
{
    setObjectName( "NumberPadWidget" );

    // set font
    QFont f( font() );
    f.setPointSize( buttonFontPointSize_ );
    setFont( f );

    if ( (mainWindow_ = qobject_cast<SessionMainWindow*>( parent )) )
        session_ = mainWindow_->session();
    else
        session_ = NULL;

    // initialize
    initialize();
    createLayout();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
NumberPadWidget::~NumberPadWidget()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void NumberPadWidget::setKeyLayoutStyle( KeyLayoutStyle value )
{
    // nothing to do
    //if ( value == keyLayoutStyle_ )
    //   return;

    keyLayoutStyle_ = value;

    // create layout
    createLayout();

    // emit signal
    if ( isVisible() || value != KeyLayoutStyle(session_->scaleConfig->normalScaleOperation.popupKeypadStyle() ) )
        emitKeyLayoutStyleChanged();

    else
    {
        // mark flag for later processing when visible
        keyLayoutStyleHasChanged_ = true;
    }
}

void NumberPadWidget::setBackKeyLayoutStyle( BackKeyLayoutStyle value )
{
    // Called when we receive Configuration from Scale Application

    // nothing to do
    if ( value == backKeyLayoutStyle_ )
        return;

    backKeyLayoutStyle_ = value;

    // create layout
    createBackspaceLayout();

    // emit signal
    if ( isVisible() )
        emitKeyLayoutStyleChanged();

    // mark flag for later processing when visible
    backKeyLayoutStyleHasChanged_ = true;

}

void NumberPadWidget::updateBackKeyLayoutStyle()
{
    // create layout
    createBackspaceLayout();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void NumberPadWidget::mousePressEvent( QMouseEvent *e )
{
    LOG_CLASS_INFO( "NumberPadWidget", "mousePressEvent" );

    if ( Qt::LeftButton == e->button() )
    {
        // get operate mode
        if ( !operateMode_ )
        {
            sessionInfo info( this );
            SessionMainWindow *mainWindow( info() );

            operateMode_ = mainWindow->retrieveMode<OperateMode>();
        }

        LOG_DEBUG( "Num pad mouse press", NULL );
        position_ = mapToGlobal( e->pos() );
    }

    _Mybase::mousePressEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void NumberPadWidget::mouseMoveEvent( QMouseEvent *e )
{
    LOG_CLASS_INFO( "NumberPadWidget", "mouseMoveEvent" );

    // allow drag only when left button pressed and dragging enabled in operate mode
    if ( e->buttons().testFlag( Qt::LeftButton ) )
        if (( operateMode_ ) && ( !operateMode_->widgetDragLock() ))
        {
            QPoint mousePos = mapToGlobal( e->pos() );

            // check for drag
            if ( !moving_ )
            {
                int distance = (mousePos - position_).manhattanLength();

                // drag initiated
                if ( distance >= QApplication::startDragDistance() )
                {
                    LOG_INFO( "Num pad drag initiated", NULL );
                    moving_ = true;
                }
            }

            // move window
            if ( moving_ )
            {
                int x = pos().x() + (mousePos.x() - position_.x());
                int y = pos().y();

                // do not allow outside parent
                if ( parentWidget() )
                {
                    int min = parentWidget()->mapToGlobal( QPoint( 0, 0 ) ).x();
                    int max = min + (parentWidget()->width() - width());

                    min += margins_.left();
                    max -= margins_.right();

                    if ( x < min )
                        x = min;
                    else if ( x > max )
                        x = max;
                }

                move( x, y );
                position_ = mousePos;
            }
        }

    _Mybase::mouseMoveEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void NumberPadWidget::mouseReleaseEvent( QMouseEvent *e )
{
    // complete drag (if initiated)
    checkDragCompletion();

    _Mybase::mouseReleaseEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void NumberPadWidget::resizeEvent( QResizeEvent *e )
{
    // set background
    const hobart::backgroundWidgetRenderer renderer(
        new hobart::panelButtonBackgroundWidget( 5 ),
        new hobart::inputBackgroundWidgetRendererAppearance() );

    const QPixmap background( renderer.drawControl( size(), hobart::Normal ) );

    modifyPaletteBackground setBackground( background );
    setBackground( this );

    setAutoFillBackground( true );

    // set button backgrounds
    const int buttonLen( (height() * 80) / 370 );

    const hobart::backgroundWidgetRenderer buttonRenderer(
        new hobart::panelButtonBackgroundWidget( 5 ),
        new hobart::darkButtonBackgroundWidgetRendererAppearance() );

    const QPixmap key( buttonRenderer.drawControl( QSize( buttonLen, buttonLen ), hobart::ReadOnly ) );
    const QPixmap keyHorz( buttonRenderer.drawControl( QSize( (2 * buttonLen) + layoutSpacing_, buttonLen ), hobart::ReadOnly ) );
    const QPixmap keyVert( buttonRenderer.drawControl( QSize( buttonLen, (2 * buttonLen) + layoutSpacing_ ), hobart::ReadOnly ) );

    keys_["0"]->setBackground( key );
    keys_["1"]->setBackground( key );
    keys_["2"]->setBackground( key );
    keys_["3"]->setBackground( key );
    keys_["4"]->setBackground( key );
    keys_["5"]->setBackground( key );
    keys_["6"]->setBackground( key );
    keys_["7"]->setBackground( key );
    keys_["8"]->setBackground( key );
    keys_["9"]->setBackground( key );

    keys_[KEY_BACKSPACE]->setBackground( keyHorz );
    keys_[KEY_PRINT]->setBackground( keyVert );
    keys_[KEY_RETURN]->setBackground( keyVert );

    keys_[KEY_SEARCH]->setBackground( key );
    keys_[KEY_BACKSPACE_SMALL]->setBackground( key );

    // set pressed button backgrounds
    invertPixmap pixmapInverter;

    const QPixmap invKey( pixmapInverter( key ) );
    const QPixmap invKeyHorz( pixmapInverter( keyHorz ) );
    const QPixmap invKeyVert( pixmapInverter( keyVert ) );

    keys_["0"]->setBackgroundPressed( invKey );
    keys_["1"]->setBackgroundPressed( invKey );
    keys_["2"]->setBackgroundPressed( invKey );
    keys_["3"]->setBackgroundPressed( invKey );
    keys_["4"]->setBackgroundPressed( invKey );
    keys_["5"]->setBackgroundPressed( invKey );
    keys_["6"]->setBackgroundPressed( invKey );
    keys_["7"]->setBackgroundPressed( invKey );
    keys_["8"]->setBackgroundPressed( invKey );
    keys_["9"]->setBackgroundPressed( invKey );

    keys_[KEY_BACKSPACE]->setBackgroundPressed( invKeyHorz );
    keys_[KEY_PRINT]->setBackgroundPressed( invKeyVert );
    keys_[KEY_RETURN]->setBackgroundPressed( invKeyVert );

    keys_[KEY_SEARCH]->setBackgroundPressed( invKey );
    keys_[KEY_BACKSPACE_SMALL]->setBackgroundPressed( invKey );

    // invoke base
    _Mybase::resizeEvent( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void NumberPadWidget::showEvent( QShowEvent *e )
{
    _Mybase::showEvent( e );

    // process layout style change
    if ( keyLayoutStyleHasChanged_ )
    {
        emitKeyLayoutStyleChanged();
        keyLayoutStyleHasChanged_ = false;
    }
}
void NumberPadWidget::hideEvent( QHideEvent *e)
{
    _Mybase::hideEvent( e );
    // SOF-5086: reinforce current layout
    setKeyLayoutStyle(keyLayoutStyle_);

}
///////////////////////////////////////////////////////////////////////////////////////////////////
void NumberPadWidget::emitKeyLayoutStyleChanged()
{
    // repaint!
    repaint();

    // schedule timer for signal
    emitKeyLayoutStyleChangedTimer_->setSingleShot( true );
    emitKeyLayoutStyleChangedTimer_->start( 0 );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void NumberPadWidget::onEmitKeyLayoutStyleChanged()
{
    // emit signal
    Q_EMIT keyLayoutStyleChanged();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void NumberPadWidget::onKeyClicked( QWidget *widget )
{
    LOG_CLASS_INFO( "NumberPadWidget", "onKeyClicked" );

    // Note:
    // There is an issue where I drag the numberpad around by starting the drag on a button and
    // ending the drag on a button. In this case the mouse event is eaten by the button and never
    // sent to the numberpad widget. This results in a numberpad that is floating in the middle
    // of the screen wherever you released the mouse. To resolve this issue let us attempt to
    // complete the drag...
    checkDragCompletion();

    // handle key
    if ( ( widget == keys_[KEY_BACKSPACE] ) || ( widget == keys_[KEY_BACKSPACE_SMALL] ))
    {
        LOG_TRACE( "backspace", NULL );

        // emit backspace key
        Q_EMIT characterGenerated( Qt::Key_Backspace, Qt::NoModifier );
    }
    else if ( widget == keys_[KEY_CLEAR] )
    {
        // clear the focused input widget
        clearFocusWidget();
    }
    else if ( widget == keys_[KEY_SEARCH] )
    {
        // do something like pop the search dialog
        if( widget->isEnabled() )
        {
            mainWindow_->searchKeyPressed();
        }
    }
    else if ( widget == keys_[KEY_PRINT] )
    {
        static const QChar c = 'P';

        // accept (print key pressed!)
        acceptFocusWidget( true );

        // emit ctrl+p
        Q_EMIT characterGenerated( c.unicode(), Qt::ControlModifier, QString( c ) );
    }
    else if ( widget == keys_[KEY_RETURN] )
    {
        // accept
        acceptFocusWidget();
    }
    else
    {
        QPushButton *pushButton = dynamic_cast<QPushButton*>( widget );

        // emit what key was pressed
        if (( pushButton ) && ( 1 <= pushButton->text().length() ))
        {
            LOG_TRACE( "'%s' key", qPrintable( pushButton->text() ) );

            QChar c = pushButton->text().at( 0 );
            Q_EMIT characterGenerated( c.unicode(), Qt::NoModifier, QString( c ) );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void NumberPadWidget::initialize()
{
    QPixmap keyBackspaceIcon( ":/images/numberPadBackspaceIcon.png" );
    QPixmap keyPrintIcon( ":/images/numberPadPrintIcon.png" );
    QPixmap keyReturnIcon( ":/images/numberPadReturnIcon.png" );
    QPixmap keySearchIcon( ":/images/panels/searchIcon.png" );

    emitKeyLayoutStyleChangedTimer_ = new QTimer( this );
    connect( emitKeyLayoutStyleChangedTimer_, SIGNAL( timeout() ), SLOT( onEmitKeyLayoutStyleChanged() ) );

    ///////////////////////////////////////////////////////////////////////////
    // create keyboard keys

    keys_["0"] = new hobart::pushButton( "0", this );
    keys_["1"] = new hobart::pushButton( "1", this );
    keys_["2"] = new hobart::pushButton( "2", this );
    keys_["3"] = new hobart::pushButton( "3", this );
    keys_["4"] = new hobart::pushButton( "4", this );
    keys_["5"] = new hobart::pushButton( "5", this );
    keys_["6"] = new hobart::pushButton( "6", this );
    keys_["7"] = new hobart::pushButton( "7", this );
    keys_["8"] = new hobart::pushButton( "8", this );
    keys_["9"] = new hobart::pushButton( "9", this );

    keys_[KEY_BACKSPACE] = new hobart::pushButton( keyBackspaceIcon, QString(), this );
    keys_[KEY_PRINT] = new hobart::pushButton( keyPrintIcon, QString(), this );
    keys_[KEY_RETURN] = new hobart::pushButton( keyReturnIcon, QString(), this );

    keys_[KEY_SEARCH] = new hobart::pushButton( keySearchIcon, QString(), this );
    keys_[KEY_BACKSPACE_SMALL] = new hobart::pushButton( keyBackspaceIcon, QString(), this );

    // backspace key repeats
    keys_[KEY_BACKSPACE]->setAutoRepeat( true );
    keys_[KEY_BACKSPACE]->setAutoRepeatDelay( keyAutoRepeatDelay_ );
    keys_[KEY_BACKSPACE]->setAutoRepeatInterval( keyAutoRepeatInterval_ );

    keys_[KEY_BACKSPACE_SMALL]->setAutoRepeat( true );
    keys_[KEY_BACKSPACE_SMALL]->setAutoRepeatDelay( keyAutoRepeatDelay_ );
    keys_[KEY_BACKSPACE_SMALL]->setAutoRepeatInterval( keyAutoRepeatInterval_ );

    keys_[KEY_BACKSPACE_SMALL]->setVisible( false );
    keys_[KEY_SEARCH]->setVisible( false );
    keys_[KEY_BACKSPACE_SMALL]->setEnabled( false );
    keys_[KEY_SEARCH]->setEnabled( false );

    ///////////////////////////////////////////////////////////////////////////
    // create signal mapper

    signalMapper_ = new QSignalMapper( this );

    connect( signalMapper_, SIGNAL( mapped(QWidget*) ), this, SLOT( onKeyClicked(QWidget*) ) );

    ///////////////////////////////////////////////////////////////////////////
    // setup keyboard keys

    // set layout
    setKeyLayoutStyle( CalculatorKeyLayoutStyle );

    // groom buttons
    for ( pushButtonMap::iterator i = keys_.begin(); i != keys_.end(); ++i )
    {
        const QString name( "button_%1" );

        hobart::pushButton *button = dynamic_cast<hobart::pushButton*>( i.value() );

        button->setFocusPolicy( Qt::NoFocus );
        button->setIconSize( buttonIconSize_ );
        button->setObjectName( name.arg( i.key() ) );

        signalMapper_->setMapping( button, button );
        connect( button, SIGNAL( clicked() ), signalMapper_, SLOT( map() ) );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void NumberPadWidget::createLayout()
{
    // delete old layout
    QLayout *oldScreenLayout = layout();

    if ( oldScreenLayout )
        delete oldScreenLayout;

    // create layouts
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout;

    QHBoxLayout *screenLayout = new QHBoxLayout( this );
    screenLayout->setSpacing( layoutSpacing_ );

    verticalLayout = new QVBoxLayout();
    verticalLayout->setSpacing( layoutSpacing_ );
    verticalLayout->addStretch();
    screenLayout->addLayout( verticalLayout );

    horizontalLayout = new QHBoxLayout();
    horizontalLayout->setSpacing( layoutSpacing_ );
    horizontalLayout->addStretch();
    horizontalLayout->addWidget( (CalculatorKeyLayoutStyle == keyLayoutStyle_ ? keys_["7"] : keys_["1"]) );
    horizontalLayout->addWidget( (CalculatorKeyLayoutStyle == keyLayoutStyle_ ? keys_["8"] : keys_["2"]) );
    horizontalLayout->addWidget( (CalculatorKeyLayoutStyle == keyLayoutStyle_ ? keys_["9"] : keys_["3"]) );
    horizontalLayout->addStretch();
    verticalLayout->addLayout( horizontalLayout );

    horizontalLayout = new QHBoxLayout();
    horizontalLayout->setSpacing( layoutSpacing_ );
    horizontalLayout->addStretch();
    horizontalLayout->addWidget( keys_["4"] );
    horizontalLayout->addWidget( keys_["5"] );
    horizontalLayout->addWidget( keys_["6"] );
    horizontalLayout->addStretch();
    verticalLayout->addLayout( horizontalLayout );

    horizontalLayout = new QHBoxLayout();
    horizontalLayout->setSpacing( layoutSpacing_ );
    horizontalLayout->addStretch();
    horizontalLayout->addWidget( (CalculatorKeyLayoutStyle == keyLayoutStyle_ ? keys_["1"] : keys_["7"]) );
    horizontalLayout->addWidget( (CalculatorKeyLayoutStyle == keyLayoutStyle_ ? keys_["2"] : keys_["8"]) );
    horizontalLayout->addWidget( (CalculatorKeyLayoutStyle == keyLayoutStyle_ ? keys_["3"] : keys_["9"]) );
    horizontalLayout->addStretch();
    verticalLayout->addLayout( horizontalLayout );

    horizontalLayout = new QHBoxLayout();
    horizontalLayout->setSpacing( layoutSpacing_ );
    horizontalLayout->addStretch();
    horizontalLayout->addWidget( keys_["0"] );

    horizontalLayout->addWidget( keys_[KEY_BACKSPACE] );
    horizontalLayout->addWidget( keys_[KEY_BACKSPACE_SMALL] );
    horizontalLayout->addWidget( keys_[KEY_SEARCH] );

    horizontalLayout->addStretch();
    verticalLayout->addLayout( horizontalLayout );

    verticalLayout->addStretch();

    verticalLayout = new QVBoxLayout();
    verticalLayout->setSpacing( layoutSpacing_ );
    verticalLayout->addStretch();
    verticalLayout->addWidget( keys_[KEY_PRINT] );
    verticalLayout->addWidget( keys_[KEY_RETURN] );
    verticalLayout->addStretch();
    screenLayout->addLayout( verticalLayout );

    screenLayout->insertStretch( 0 );
    screenLayout->addStretch();

    //updateBackKeyLayoutStyle();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void NumberPadWidget::createBackspaceLayout()
{
    if( session_)
        isDeliVisionLicensed_ = session_->uiUtilities->isFeatureLicensed( SCALE_ENHANCED_CENTERSCREEN );

    if( ( isDeliVisionLicensed_)  && ( smallBackKeyLayoutStyle == backKeyLayoutStyle_) )
    {
        keys_[KEY_BACKSPACE]->setVisible( false );
        keys_[KEY_BACKSPACE]->setEnabled( false );

        keys_[KEY_BACKSPACE_SMALL]->setEnabled( true );
        keys_[KEY_SEARCH]->setEnabled( true );

        keys_[KEY_BACKSPACE_SMALL]->setVisible( true );
        keys_[KEY_SEARCH]->setVisible( true );
    }
    else
    {
        keys_[KEY_BACKSPACE_SMALL]->setVisible( false );
        keys_[KEY_SEARCH]->setVisible( false );

        keys_[KEY_BACKSPACE_SMALL]->setEnabled( false );
        keys_[KEY_SEARCH]->setEnabled( false );

        keys_[KEY_BACKSPACE]->setEnabled( true );
        keys_[KEY_BACKSPACE]->setVisible( true );
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void NumberPadWidget::checkDragCompletion()
{
    LOG_CLASS_INFO( "NumberPadWidget", "checkDragCompletion" );

    if ( moving_ )
    {
        LOG_INFO( "Num pad drag complete", NULL );
        moving_ = false;

        // snap window to edge
        if ( parentWidget() )
        {
            int min = parentWidget()->mapToGlobal( QPoint( 0, 0 ) ).x();
            int max = min + (parentWidget()->width() - width());

            min += margins_.left();
            max -= margins_.right();

            Qt::DockWidgetArea where;

            // left side
            if ( pos().x() < (min + ((max - min) / 2)) )
            {
                move( min, pos().y() );
                where = Qt::LeftDockWidgetArea;
            }

            // right side
            else
            {
                move( max, pos().y() );
                where = Qt::RightDockWidgetArea;
            }

            // emit signal
            Q_EMIT moved( where );
        }
    }
}
