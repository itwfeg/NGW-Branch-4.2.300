/**
 * @file inputwidget.cpp
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 */

#include "inputwidget.h"

#include <QtCore>

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QSpinBox>
#include <QTextEdit>

const int inputWidget::autoRepeatDelay_ = 500;
const int inputWidget::autoRepeatInterval_ = 100;

static const Qt::WindowFlags _InputWidgetWindowFlags( Qt::Tool | Qt::WindowStaysOnTopHint | Qt::WindowDoesNotAcceptFocus | Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint );

///////////////////////////////////////////////////////////////////////////////////////////////////
inputWidget::inputWidget( QWidget *parent ) :
    _Mybase( parent, _InputWidgetWindowFlags ),
    w_( NULL )
{
    setAttribute( Qt::WA_ShowWithoutActivating );
    setFocusPolicy( Qt::NoFocus );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
inputWidget::~inputWidget()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void inputWidget::activate( QWidget *widget )
{
    show();

    // activate input widget
    if ( (w_ = widget) )
        w_->activateWindow();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void inputWidget::dismiss()
{
    // focus off the input widget
    if ( w_ )
        w_->clearFocus();

    hide();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void inputWidget::hideEvent( QHideEvent *e )
{
    _Mybase::hideEvent( e );
    w_ = NULL;

    // we are now hidden
    Q_EMIT visible( false );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void inputWidget::showEvent( QShowEvent *e )
{
    _Mybase::showEvent( e );

    // we are now visible
    Q_EMIT visible( true );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void inputWidget::acceptFocusWidget()
{
    qDebug() << "accept focus widget";

    // emit!
    Q_EMIT characterGenerated( Qt::Key_Enter, Qt::NoModifier );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void inputWidget::clearFocusWidget()
{
    QComboBox *comboBox;
    QDoubleSpinBox *doubleSpinBox;
    QLineEdit *lineEdit;
    QPlainTextEdit *plainTextEdit;
    QSpinBox *spinBox;
    QTextEdit *textEdit;

    if ( !w_ )
        return;

    qDebug() << "clear focus widget";

    if ( (lineEdit = dynamic_cast<QLineEdit*>( w_ )) )
        lineEdit->setText( "" );
    else if ( (plainTextEdit = dynamic_cast<QPlainTextEdit*>( w_ )) )
        plainTextEdit->setPlainText( "" );
    else if ( (textEdit = dynamic_cast<QTextEdit*>( w_ )) )
        textEdit->setText( "" );

    else if ( (comboBox = dynamic_cast<QComboBox*>( w_ )) )
        comboBox->setEditText( "" );

    else if ( (spinBox = dynamic_cast<QSpinBox*>( w_ )) )
    {
        spinBox->setValue( spinBox->minimum() );
        spinBox->selectAll();
    }
    else if ( (doubleSpinBox = dynamic_cast<QDoubleSpinBox*>( w_ )) )
    {
        doubleSpinBox->setValue( doubleSpinBox->minimum() );
        doubleSpinBox->selectAll();
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool inputWidget::event( QEvent *e )
{
    switch ( e->type() )
    {
    case QEvent::WindowActivate:

        // transfer focus back to input widget
        if ( w_ )
            w_->activateWindow();

        break;

    default:
        break;
    }

    return _Mybase::event( e );
}
