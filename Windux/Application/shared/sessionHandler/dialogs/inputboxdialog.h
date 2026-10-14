/**
 * @file inputboxdialog.h
 * Message box with input area.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef INPUTBOXDIALOG_H
#define INPUTBOXDIALOG_H

#include "dcadialog.h"
#include "messageboxdialog.h"

#include "../sessionmainwindow.h"
#include "../sessioninfo.h"

#include <backgroundwidgettypes.h>
#include <palette.h>

#include <QHBoxLayout>
#include <QLineEdit>
#include <QVBoxLayout>

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Message box with input area.
/**
 * @tparam _Widget  input widget type
 */
template <class _Widget>
class InputBoxDialog : public MessageBoxDialog
{
    typedef InputBoxDialog _Myt;
    typedef MessageBoxDialog _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     * @param[in] f  window flags
     */
    explicit InputBoxDialog( QWidget *parent = 0, Qt::WindowFlags f = 0 );

    /// Destructor.
    virtual ~InputBoxDialog();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Whether or not clicking the overlay rejects the dialog.
    /**
     * @return  @c true if allowed, @c false otherwise
     */
    virtual bool allowOverlayClickDismiss() const;

    /// Retrieve echo mode.
    /**
     * @return  echo mode
     */
    virtual QLineEdit::EchoMode echoMode() const;

    /// Retrieve input as text string.
    /**
     * @return  input as string
     */
    virtual QString inputText() const;

    /// Retrieve if input is required.
    /**
     * @return  @c true if required, @c false otherwise
     */
    virtual bool isRequired() const;

    /// Set echo mode.
    /**
     * @param[in] value  echo mode
     */
    virtual void setEchoMode( QLineEdit::EchoMode value );

    /// Set if input is required.
    /**
     * @param[in] value  @c true to require input, @c false otherwise
     */
    virtual void setRequired( bool value );

protected:

    _Widget *input_;                                    ///< Input widget.

    // ========================================================================
    // Events
    // ========================================================================

    /// Show event.
    /**
     * @param[in,out] event  show event
     */
    virtual void showEvent( QShowEvent *event );

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve priority of this dialog.
    /**
     * When showing a high priority dialog all dialogs of lower priority will be automatically
     * closed.
     *
     * @return  priority
     */
    virtual int priority() const;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Callback for input editing finished.
    /**
     * This base method just calls dialog accept().
     */
    virtual void onInputEditingFinished();

    /// Callback for input return pressed.
    virtual void onInputReturnPressed();

private:

    bool required_;

    /// Initialize.
    void initialize();

};

template <class _Widget>
InputBoxDialog<_Widget>::InputBoxDialog( QWidget *parent, Qt::WindowFlags f ) :
    _Mybase( parent, f ),
    required_( false )
{
    initialize();
}

template <class _Widget>
InputBoxDialog<_Widget>::~InputBoxDialog()
{
}

template <class _Widget>
bool InputBoxDialog<_Widget>::allowOverlayClickDismiss() const
{
    return !isRequired();
}

template <class _Widget>
QLineEdit::EchoMode InputBoxDialog<_Widget>::echoMode() const
{
    return input_->echoMode();
}

template <class _Widget>
QString InputBoxDialog<_Widget>::inputText() const
{
    return input_->text();
}

template <class _Widget>
bool InputBoxDialog<_Widget>::isRequired() const
{
    return required_;
}

template <class _Widget>
void InputBoxDialog<_Widget>::setEchoMode( QLineEdit::EchoMode value )
{
    input_->setEchoMode( value );
}

template <class _Widget>
void InputBoxDialog<_Widget>::setRequired( bool value )
{
    required_ = value;
}

template <class _Widget>
int InputBoxDialog<_Widget>::priority() const
{
    return Low;
}

template <class _Widget>
void InputBoxDialog<_Widget>::onInputEditingFinished()
{
    // FT-1475
    //
    // Tricky issue here. On a dialog with ok and cancel buttons they are typing in something
    // and then pressing cancel button. The problem is input editing finished signal fires and
    // accepts the dialog before it can be rejected by the cancel button.
    //
    // For resolution I am adding code to skip the accept if there are two or more buttons... also
    // if they happen to press enter I will accept the entry if there are two or more buttons.

    QList<QAbstractButton*> bs( buttons() );

    // accept!
    if ( bs.size() < 2 )
        accept();
}

template <class _Widget>
void InputBoxDialog<_Widget>::onInputReturnPressed()
{
    // FT-1475
    //
    // Tricky issue here. On a dialog with ok and cancel buttons they are typing in something
    // and then pressing cancel button. The problem is input editing finished signal fires and
    // accepts the dialog before it can be rejected by the cancel button.
    //
    // For resolution I am adding code to skip the accept if there are two or more buttons... also
    // if they happen to press enter I will accept the entry if there are two or more buttons.

    QList<QAbstractButton*> bs( buttons() );

    // accept!
    if ( 2 <= bs.size() )
        accept();
}

template <class _Widget>
void InputBoxDialog<_Widget>::showEvent( QShowEvent *event )
{
    _Mybase::showEvent( event );

    // set focus to input widget
    input_->setFocus();
}

template <class _Widget>
void InputBoxDialog<_Widget>::initialize()
{
    setObjectName( "InputBoxDialog" );

    // create widget
    input_ = new _Widget( this );
    input_->setObjectName( "input" );
    input_->setPalette( QApplication::palette() );

    // center horizontally
    messageAreaInner_->insertStretch( 0 );
    messageAreaInner_->addWidget( input_ );
    messageAreaInner_->addStretch();

    // center vertically
    messageArea_->insertStretch( 0 );
    messageArea_->addStretch();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // INPUTBOXDIALOG_H
