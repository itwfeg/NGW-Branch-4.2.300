/**
 * @file numericinputboxdialog.h
 * Dialog for numeric input.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef NUMERICINPUTBOXDIALOG_H
#define NUMERICINPUTBOXDIALOG_H

#include "inputboxdialog.h"

#include <numericlineedit.h>

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class NumericInputBoxDialog
 * @brief Dialog for numeric input.
 */
class NumericInputBoxDialog : public InputBoxDialog<hobart::numericLineEdit>
{
    Q_OBJECT

    typedef NumericInputBoxDialog _Myt;
    typedef InputBoxDialog<hobart::numericLineEdit> _Mybase;

public:

    ///////////////////////////////////////////////////////////////////////////
    // CTOR / DTOR

    /**
     * Constructor.
     *
     * @param[in] parent  parent widget
     * @param[in] f  window flags
     */
    explicit NumericInputBoxDialog( QWidget *parent = 0, Qt::WindowFlags f = 0 );

    /**
     * Destructor.
     */
    virtual ~NumericInputBoxDialog();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Retrieve decimal place character.
     *
     * @returns  decimal place character
     */
    virtual QString decimal() const;

    /**
     * Retrieve number of decimal places (precision).
     *
     * @returns  number of decimal places
     */
    virtual int decimalPlaces() const;

    /**
     * Retrieve if leading zeros are shown or not.
     *
     * @note
     * This only applies to widgets with wrapping and no decimal places.
     *
     * @returns  @c true if allow leading zeros, @c false otherwise
     */
    virtual bool inputLeadingZeros() const;

    /**
     * Retrieve input as text string.
     *
     * @returns  input as string
     */
    virtual QString inputText() const;

    /**
     * Retrieve if first input into the field should reset existing input.
     *
     * @returns  @c true if enable trucation, @c false otherwise
     */
    virtual bool inputTruncation() const;

    /**
     * Retrieve input as numeric value.
     *
     * @returns  input as number
     */
    virtual int inputValue() const;

    /**
     * Retrieve if input wrapping is enabled or not.
     *
     * @returns  @c true if allow wrapping, @c false otherwise
     */
    virtual bool inputWrapping() const;

    /**
     * Retrieve prefix string.
     *
     * @returns  prefix
     */
    virtual QString prefix() const;

    /**
     * Set the decimal place character.
     *
     * @example
     * When this is set to ',' the value 12.34 will display as "12,34". In the same fashion, you
     * could also set this property to '-' in which case it wouls be "12-34".
     *
     * @param[in] value  decimal place character
     */
    virtual void setDecimal( const QString& value );

    /**
     * Set number of decimal places (precision).
     *
     * @example
     * For two decimal place precision (i.e. 12.34) use 2. For three decimal place precision
     * (i.e. 12.345) use 3.
     *
     * @param[in] value  number of decimal places
     */
    virtual void setDecimalPlaces( int value );

    /**
     * Set if leading zeros are shown or not.
     *
     * @example
     * Suppose your widget contains 555000 and has a top of 999999. Now you type 555 into the
     * widget. Without this setting your widget will now display 555. With this setting your
     * widget will display 000555.
     *
     * @note
     * This only applies to widgets with wrapping and no decimal places.
     *
     * @param[in] value  @c true to allow leading zeros, @c false otherwise
     */
    virtual void setInputLeadingZeros( bool value );

    /**
     * Set if first input into the field should reset existing input.
     *
     * @example
     * Suppose your widget contains 123456 and you click into the field for editing. Now you type
     * 4 into the widget. When this setting is enabled the entire field will be truncated down to
     * just the value 4. Normally the value 4 would be tacked onto the end (i.e. 1234564).
     *
     * @param[in] value  @c true to enable trucation, @c false otherwise
     */
    virtual void setInputTruncation( bool value );

    /**
     * Set input numeric value.
     *
     * @param[in]  input as number
     */
    virtual void setInputValue( int value );

    /**
     * Set if input wrapping is enabled or not.
     *
     * @example
     * Suppose your widget contains 123456 and has a top of 999999. Now you type 7 into the
     * widget. Without this setting your widget will now display 999999 (since 1234567 is larger
     * than 999999). With this setting your widget will display 234567.
     *
     * @param[in] value  @c true to allow wrapping, @c false otherwise
     */
    virtual void setInputWrapping( bool value );

    /**
     * Set prefix string.
     *
     * This is the text string that appears before all widget input.
     *
     * @example
     * Set this property to "$" and all widget values will be prepended with this symbol (i.e.
     * 123.45 would display as "$123.45").
     *
     * @param[in] value  prefix
     */
    virtual void setPrefix( const QString& value );

    /**
     * Set suffix string.
     *
     * This is the text string that appears after all widget input.
     *
     * @example
     * Set this property to " lb" and all widget values will be appended with this symbol (i.e.
     * 12.34 would display as "12.34 lb").
     *
     * @param[in] value  suffix
     */
    virtual void setSuffix( const QString& value );

    /**
     * Set validator.
     *
     * @param[in] value  validator
     */
    virtual void setValidator( const QIntValidator *value );

    /**
     * Retrieve suffix string.
     *
     * @returns  suffix
     */
    virtual QString suffix() const;

    /**
     * Retrieve validator.
     *
     * @returns  validator
     */
    virtual const QIntValidator *validator() const;

private Q_SLOTS:

    /**
     * Slot for input editing finished.
     */
    void onInputEditingFinishedInternal();

};

inline NumericInputBoxDialog::NumericInputBoxDialog( QWidget *parent, Qt::WindowFlags f ) :
    _Mybase( parent, f )
{
    setObjectName( "NumericInputBoxDialog" );

    connect( input_, SIGNAL( editingFinished() ), SLOT( onInputEditingFinishedInternal() ) );
}

inline NumericInputBoxDialog::~NumericInputBoxDialog()
{
}

inline QString NumericInputBoxDialog::decimal() const
{
    return input_->decimal();
}

inline int NumericInputBoxDialog::decimalPlaces() const
{
    return input_->decimalPlaces();
}

inline bool NumericInputBoxDialog::inputLeadingZeros() const
{
    return input_->inputLeadingZeros();
}

inline bool NumericInputBoxDialog::inputTruncation() const
{
    return input_->inputTruncation();
}

inline bool NumericInputBoxDialog::inputWrapping() const
{
    return input_->inputWrapping();
}

inline QString NumericInputBoxDialog::prefix() const
{
    return input_->prefix();
}

inline void NumericInputBoxDialog::setDecimal( const QString& value )
{
    input_->setDecimal( value );
}

inline void NumericInputBoxDialog::setDecimalPlaces( int value )
{
    input_->setDecimalPlaces( value );
}

inline void NumericInputBoxDialog::setInputLeadingZeros( bool value )
{
    input_->setInputLeadingZeros( value );
}

inline QString NumericInputBoxDialog::inputText() const
{
    return input_->toSessionField();
}

inline void NumericInputBoxDialog::setInputTruncation( bool value )
{
    input_->setInputTruncation( value );
}

inline void NumericInputBoxDialog::setInputValue( int value )
{
    input_->fromInt( value );
}

inline void NumericInputBoxDialog::setInputWrapping( bool value )
{
    input_->setInputWrapping( value );
}

inline int NumericInputBoxDialog::inputValue() const
{
    return input_->toInt();
}

inline void NumericInputBoxDialog::setPrefix( const QString& value )
{
    input_->setPrefix( value );
}

inline void NumericInputBoxDialog::setSuffix( const QString& value )
{
    input_->setSuffix( value );
}

inline void NumericInputBoxDialog::setValidator( const QIntValidator *value )
{
    input_->setValidator( value );
}

inline QString NumericInputBoxDialog::suffix() const
{
    return input_->suffix();
}

inline const QIntValidator *NumericInputBoxDialog::validator() const
{
    return input_->validator();
}

inline void NumericInputBoxDialog::onInputEditingFinishedInternal()
{
    onInputEditingFinished();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // NUMERICINPUTBOXDIALOG_H
