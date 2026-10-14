/**
 * @file inputwidgethook.cpp
 *
 * @copyright Copyright (c) 2015 Hobart Corporation
 * @author Randy Blankley
 */

#include "inputwidgethook.h"
#include "keyboardinputwidget.h"

#include <QtCore>

#include <QApplication>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QSpinBox>
#include <QTextEdit>
#include <QWidget>

///////////////////////////////////////////////////////////////////////////////////////////////////
inputWidgetHook::inputWidgetHook( QObject *parent ) :
    _Mybase( parent ),
    focusWidget_( NULL )
{
    // init
    initialize();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
inputWidgetHook::~inputWidgetHook()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void inputWidgetHook::activate( QWidget *widget )
{
    // use what is currently focused
    if ( !widget )
    {
        qWarning() << "showing input widget without target (stealing active)!";
        widget = QApplication::focusWidget();
    }

    if ( !widget )
        qWarning() << "nothing is focused!";
    else
    {
        updateFocusWidget( widget );
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void inputWidgetHook::dismiss()
{
    updateFocusWidget( NULL );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void inputWidgetHook::onCharacterGenerated( int key, Qt::KeyboardModifiers modifiers, const QString &text )
{
    if ( !focusWidget_ )
        return;

    // simulate key press
    QKeyEvent keyPress( QEvent::KeyPress, key, modifiers, text );
    QApplication::sendEvent( focusWidget_, &keyPress );

    // simulate key release
    QKeyEvent keyRelease( QEvent::KeyRelease, key, modifiers );
    QApplication::sendEvent( focusWidget_, &keyRelease );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void inputWidgetHook::onFocusChanged( QWidget* /*oldFocus*/, QWidget *newFocus )
{
    QWidget *widget( NULL );

    // keep track of who has focus
    if (( newFocus ) && ( !keyboard_->isAncestorOf( newFocus ) ))
    {
        widget = newFocus;

        qDebug() << "focus changed to:" << (newFocus ? newFocus->objectName() : QString()) <<
            "on parent" <<  ((newFocus && newFocus->parent()) ? newFocus->parent()->objectName() : QString());
    }

    // reset focus
    else if ( !newFocus )
    {
        widget = NULL;

        qDebug() << "focus changed to: NULL";
    }

    // update
    updateFocusWidget( widget );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void inputWidgetHook::initialize()
{
    // create keyboard
    keyboard_ = new keyboardInputWidget();

    connect( keyboard_, SIGNAL(characterGenerated(int,Qt::KeyboardModifiers,QString)), SLOT(onCharacterGenerated(int,Qt::KeyboardModifiers,QString)) );

    // connect slots
    connect( qApp, SIGNAL(focusChanged(QWidget*,QWidget*)), SLOT(onFocusChanged(QWidget*,QWidget*)) );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void inputWidgetHook::updateFocusWidget( QWidget *widget )
{
    // nothing to do
    if ( focusWidget_ == widget )
        return;

    // set new focus
    if ( (focusWidget_ = widget) )
    {
        QLineEdit *lineEdit;
        QPlainTextEdit *plainTextEdit;
        QTextEdit *textEdit;
        QComboBox *comboBox;
        QSpinBox *spinBox;
        QDoubleSpinBox *doubleSpinBox;

        // determine which input widget to show
        bool isKeyboardInput( false );
        bool isNumberPadInput( false );

        // check for keyboard input
        if ( (lineEdit = dynamic_cast<QLineEdit*>( focusWidget_ )) )
        {
            const QValidator *v = lineEdit->validator();

            if (( v ) && (( dynamic_cast<const QIntValidator*>( v ) ) || ( dynamic_cast<const QDoubleValidator*>( v ) )))
                isNumberPadInput = true;
            else
                isKeyboardInput = true;
        }
        else if ( (plainTextEdit = dynamic_cast<QPlainTextEdit*>( focusWidget_ )) )
            isKeyboardInput = true;
        else if ( (textEdit = dynamic_cast<QTextEdit*>( focusWidget_ )) )
            isKeyboardInput = true;
        else if ( (comboBox = dynamic_cast<QComboBox*>( focusWidget_ )) )
            isKeyboardInput = comboBox->isEditable();

        // check for number pad input
        else if ( (spinBox = dynamic_cast<QSpinBox*>( focusWidget_ )) )
            isNumberPadInput = true;
        else if ( (doubleSpinBox = dynamic_cast<QDoubleSpinBox*>( focusWidget_ )) )
            isNumberPadInput = true;

        // activate input widget
        inputWidget *device( NULL );

        if ( isKeyboardInput )
            device = keyboard_;
        else if ( isNumberPadInput )
            device = keyboard_; // TODO: add number pad?

        // activate!!
        if ( device )
        {
            device->activate( focusWidget_ );
            return;
        }
    }

    // dismiss all input devices
    keyboard_->dismiss();
}
