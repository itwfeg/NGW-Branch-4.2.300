/**
 * @file operatorlogindialog.h
 * Dialog for text input.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef TEXTINPUTBOXDIALOG_H
#define TEXTINPUTBOXDIALOG_H

#include "inputboxdialog.h"

#include <QLineEdit>

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class TextInputBoxDialog
 * @brief Dialog for text input.
 */
class TextInputBoxDialog : public InputBoxDialog<QLineEdit>
{
    Q_OBJECT

    typedef TextInputBoxDialog _Myt;
    typedef InputBoxDialog<QLineEdit> _Mybase;

public:

    ///////////////////////////////////////////////////////////////////////////
    // CTOR / DTOR

    /**
     * Constructor.
     *
     * @param[in] parent  parent widget
     * @param[in] f  window flags
     */
    explicit TextInputBoxDialog( QWidget *parent = 0, Qt::WindowFlags f = 0 );

    /**
     * Destructor.
     */
    virtual ~TextInputBoxDialog();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Set validator.
     *
     * @param[in] value  validator
     */
    virtual void setValidator( const QValidator *value );

    /**
     * Retrieve validator.
     *
     * @returns  validator
     */
    virtual const QValidator *validator() const;

private Q_SLOTS:

    /**
     * Slot for input editing finished.
     */
    void onInputEditingFinishedInternal();

    /**
     * Slot for input return key pressed.
     */
    void onInputReturnPressedInternal();

};

inline TextInputBoxDialog::TextInputBoxDialog( QWidget *parent, Qt::WindowFlags f ) :
    _Mybase( parent, f )
{
    setObjectName( "TextInputBoxDialog" );

    connect( input_, SIGNAL( editingFinished() ), SLOT( onInputEditingFinishedInternal() ) );
    connect( input_, SIGNAL( returnPressed() ), SLOT( onInputReturnPressedInternal() ) );
}

inline TextInputBoxDialog::~TextInputBoxDialog()
{
}

inline void TextInputBoxDialog::setValidator( const QValidator *value )
{
    input_->setValidator( value );
}

inline const QValidator *TextInputBoxDialog::validator() const
{
    return input_->validator();
}

inline void TextInputBoxDialog::onInputReturnPressedInternal()
{
    onInputReturnPressed();
}

inline void TextInputBoxDialog::onInputEditingFinishedInternal()
{
    onInputEditingFinished();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // TEXTINPUTBOXDIALOG_H
