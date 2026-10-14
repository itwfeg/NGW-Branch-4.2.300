/**
 * @file inputwidget.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */


#include "inputwidget.h"
#include "sessionmainwindow.h"

#include "widgets/webViewer.h"

#include <backgroundwidgettypes.h>
#include <logmanager.h>
#include <palette.h>

#include <ScaleConfiguration.h>

#include <QtCore>

#include <QApplication>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QSpinBox>
#include <QTextEdit>
#include <QWebView>

const int InputWidget::keyAutoRepeatDelay_ = 500;
const int InputWidget::keyAutoRepeatInterval_ = 100;

#if ( QT_VERSION >= 0x050000 )
static const Qt::WindowFlags _InputWidgetWindowFlags( Qt::Tool | Qt::WindowStaysOnTopHint | Qt::WindowDoesNotAcceptFocus | Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint );
#else
static const Qt::WindowFlags _InputWidgetWindowFlags( Qt::Tool | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint );
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
InputWidget::InputWidget( QWidget *parent ) :
    _Mybase( parent, _InputWidgetWindowFlags ),
    mainWindow_( NULL ),
    lastFocusedWidget_( NULL )
{
    // set font
    QFont f( font() );
    f.setFamily( "Arial" );
    setFont( f );

    // set palette
    modifyPaletteForeground setForeground( modifyPaletteForeground::createPalette( hobart::colorPalette::white ) );
    setForeground( this );

    setAttribute( Qt::WA_ShowWithoutActivating );
    setFocusPolicy( Qt::NoFocus );

    // keep track of parent main window
    mainWindow_ = dynamic_cast<SessionMainWindow*>( parent );

    // connect slots
    connect( qApp, SIGNAL( focusChanged(QWidget*, QWidget*) ), this, SLOT( onFocusChanged(QWidget*, QWidget*) ) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
InputWidget::~InputWidget()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void InputWidget::acceptFocusWidget( bool print )
{
    LOG_CLASS_INFO( "InputWidget", "acceptFocusWidget" );

    ScaleConfiguration *scaleConfig( mainWindow_->session()->scaleConfig );

    // FT-957: keep track of print key
    // FT-958: keep track of print key
    scaleConfig->printKeyPressed = print;

    LOG_TRACE( "accept input widget", NULL );

    // emit!
    Q_EMIT characterGenerated( Qt::Key_Enter, Qt::NoModifier );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void InputWidget::clearFocusWidget()
{
    LOG_CLASS_INFO( "InputWidget", "clearFocusWidget" );

    QLineEdit *lineEdit;
    QPlainTextEdit *plainTextEdit;
    QTextEdit *textEdit;
    QSpinBox *spinBox;
    QDoubleSpinBox *doubleSpinBox;
    hobart::webViewer *webViewer;
    QWebView *webView;
    QComboBox *comboBox;

    if ( !lastFocusedWidget_ )
        return;

    LOG_TRACE( "clear input widget", NULL );

    if ( (lineEdit = qobject_cast<QLineEdit*>( lastFocusedWidget_ )) )
        lineEdit->setText( "" );
    else if ( (plainTextEdit = qobject_cast<QPlainTextEdit*>( lastFocusedWidget_ )) )
        plainTextEdit->setPlainText( "" );
    else if ( (textEdit = qobject_cast<QTextEdit*>( lastFocusedWidget_ )) )
        textEdit->setText( "" );

    else if ( (comboBox = qobject_cast<QComboBox*>( lastFocusedWidget_ )) )
        comboBox->setEditText( "" );

    // TODO
    // Need to clear text widget in qWebView page loaded
    else if ( (webView = qobject_cast<QWebView *>( lastFocusedWidget_ )) )
    {
        webViewer = qobject_cast<hobart::webViewer *>(webView->parent());
        webViewer->clear();
    }

    else if ( (spinBox = dynamic_cast<QSpinBox*>( lastFocusedWidget_ )) )
    {
        spinBox->setValue( spinBox->minimum() );
        spinBox->selectAll();
    }
    else if ( (doubleSpinBox = dynamic_cast<QDoubleSpinBox*>( lastFocusedWidget_ )) )
    {
        doubleSpinBox->setValue( doubleSpinBox->minimum() );
        doubleSpinBox->selectAll();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void InputWidget::dismiss()
{
    LOG_CLASS_INFO( "InputWidget", "dismiss" );

    LOG_TRACE( "dismiss input widget", NULL );

    // focus off the input widget
    if ( lastFocusedWidget_ )
        lastFocusedWidget_->clearFocus();

    hide();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool InputWidget::event( QEvent *e )
{
    switch ( e->type() )
    {
    case QEvent::WindowActivate:

        // transfer focus back to input widget
        if ( lastFocusedWidget_ )
            lastFocusedWidget_->activateWindow();

        break;

    default:
        break;
    }

    return _Mybase::event( e );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void InputWidget::hideEvent( QHideEvent *e )
{
    lastFocusedWidget_ = NULL;

    _Mybase::hideEvent( e );

    // we are now hidden
    Q_EMIT visible( false );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void InputWidget::showEvent( QShowEvent *e )
{
    LOG_CLASS_INFO( "InputWidget", "showEvent" );

    _Mybase::showEvent( e );

	// FT-1028
    if ( !lastFocusedWidget_ )
    {
        LOG_WARN( "showing input widget without target (stealing active)!", NULL );
        lastFocusedWidget_ = QApplication::focusWidget();
    }

    // we are now visible
    Q_EMIT visible( true );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void InputWidget::onFocusChanged( QWidget *oldFocus, QWidget *newFocus )
{
    LOG_CLASS_INFO( "InputWidget", "onFocusChanged" );

    Q_UNUSED( oldFocus );

    // keep track of who has focus
    if (( newFocus ) && ( !isAncestorOf( newFocus ) ))
    {
        // ignore non-sibling widgets
        if (( mainWindow_ ) && ( !mainWindow_->isAncestorOf( newFocus ) ))
            return;

        lastFocusedWidget_ = newFocus;

        LOG_TRACE( "focus changed to: '%s' on parent '%s'",
            newFocus ? qPrintable( newFocus->objectName() ) : "",
            (newFocus && newFocus->parent()) ? qPrintable( newFocus->parent()->objectName() ) : "" );
    }

    // reset focus
    else if ( !newFocus )
    {
        lastFocusedWidget_ = NULL;

        LOG_TRACE( "focus changed to: NULL", NULL );
    }
}
