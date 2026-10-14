/**
 * @file tabbableiplineedit.h
 * Line edit class for IP Address entry allowing tabs.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Todd Davis
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef TABBABLEIPLINEEDIT_H
#define TABBABLEIPLINEEDIT_H

#include <QLineEdit>

class QIntValidator;
class QMutex;

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @struct no_numeric_validation_t
 * @brief Allow non-numeric text (i.e. no validation).
 */
struct no_numeric_ip_validation_t {};

/**
 * Used to populate a @c labeledNumericLineEdit with non-numeric text.
 */
static const no_numeric_ip_validation_t no_numeric_ip_validation = {};

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class numericLineEdit
 * @brief Line edit class for numeric entry.
 */
class tabbableIPLineEdit : public QLineEdit
{
    Q_OBJECT

    Q_PROPERTY( QString decimal READ decimal WRITE setDecimal )
    Q_PROPERTY( int decimalPlaces READ decimalPlaces WRITE setDecimalPlaces )
    Q_PROPERTY( bool inputFocusZeroing READ inputFocusZeroing WRITE setInputFocusZeroing )
    Q_PROPERTY( bool inputLeadingZeros READ inputLeadingZeros WRITE setInputLeadingZeros )
    Q_PROPERTY( int inputPrecision READ inputPrecision WRITE setInputPrecision )
    Q_PROPERTY( bool inputTruncation READ inputTruncation WRITE setInputTruncation )
    Q_PROPERTY( bool inputWrapping READ inputWrapping WRITE setInputWrapping )
    Q_PROPERTY( QString prefix READ prefix WRITE setPrefix )
    Q_PROPERTY( QString suffix READ suffix WRITE setSuffix )
    Q_PROPERTY( QString text READ text WRITE setText )

    typedef tabbableIPLineEdit _Myt;
    typedef QLineEdit _Mybase;

public:

    ///////////////////////////////////////////////////////////////////////////
    // CTOR / DTOR

    /**
     * Constructor.
     *
     * @param[in] parent  parent widget
     */
    explicit tabbableIPLineEdit(
        QWidget *parent = 0 );

    /**
     * Constructor.
     *
     * @param[in] decimalPlaces  number of decimal places
     * @param[in] parent  parent widget
     */
    explicit tabbableIPLineEdit(
        int decimalPlaces,
        QWidget *parent = 0 );

    /**
     * Destructor.
     */
    virtual ~tabbableIPLineEdit();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Retrieve decimal place character.
     *
     * @returns  decimal place character
     */
    virtual QString decimal() const {return decimal_;}

    /**
     * Retrieve number of decimal places (precision).
     *
     * @returns  number of decimal places
     */
    virtual int decimalPlaces() const {return decimalPlaces_;}

    /**
     * Check if focusing into field should zero an empty field or not.
     *
     * When the edit box is empty and you focus into it it will be zeroed out. When this property
     * is @c false the edit box will remain empty.
     *
     * @returns  @c true to zero the field, @c false otherwise
     */
    virtual bool inputFocusZeroing() const {return zeroEmpty_;}

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
     * Retrieve input decimal place precision.
     *
     * @returns  decimal place input precision
     */
    virtual int inputPrecision() const {return precision_;}

    /**
     * Retrieve if first input into the field should reset existing input.
     *
     * @returns  @c true if enable trucation, @c false otherwise
     */
    virtual bool inputTruncation() const {return truncate_;}

    /**
     * Retrieve if input wrapping is enabled or not.
     *
     * @returns  @c true if allow wrapping, @c false otherwise
     */
    virtual bool inputWrapping() const {return wrap_;}

    /**
     * Retrieve prefix string.
     *
     * @returns  prefix
     */
    virtual QString prefix() const {return prefix_;}

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
     * Set if focusing into field should zero an empty field or not.
     *
     * When the edit box is empty and you focus into it it will be zeroed out. When this property
     * is @c false the edit box will remain empty.
     *
     * @param[in] value  @c true to zero the field, @c false otherwise
     */
    virtual void setInputFocusZeroing( bool value ) {zeroEmpty_ = value;}

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
    virtual void setInputLeadingZeros( bool value ) {leadingZeros_ = value;}

    /**
     * Set input decimal place precision.
     *
     * The value of -1 indicates this feature is disabled.
     *
     * @example
     * Suppose your widget is setup for 3 decimal places and an input precision of 2. The widget
     * currently contains a value of 0.000. Now you type a 2 into the widget. The value you type
     * will be appended to the precision location and now the widget will show 0.020. Now you type
     * a 5 and the widget will show 0.250.
     *
     * @param[in]  decimal place input precision, or -1 to disable
     */
    virtual void setInputPrecision( int value ) {precision_ = value;}

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
    virtual void setInputTruncation( bool value ) {truncate_ = value;}

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
    virtual void setInputWrapping( bool value ) {wrap_ = value;}

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
     * Set input validator.
     *
     * @param[in] value  validator
     */
    virtual void setValidator( const QIntValidator *value );

    /**
     * Retrieve suffix string.
     *
     * @returns  suffix
     */
    virtual QString suffix() const {return suffix_;}

    /**
     * Retrieve input validator.
     *
     * @returns  pointer to validator
     */
    virtual const QIntValidator *validator() const;

    ///////////////////////////////////////////////////////////////////////////
    // Methods

    /**
     * Set text value from an integer.
     *
     * @param[in] value  value to set
     */
    virtual void fromInt( int value );

    /**
     * Set text value from a session message field value.
     *
     * This method will also set the decimal places. For example setting from the value "1.005"
     * will set the decimal places to 3 and then set the value to 1005.
     *
     * @param[in] value  value to set
     */
    virtual void fromSessionField( const QString& value );

    /**
     * Set text value from a session message field value.
     *
     * This method will also set the decimal places. For example setting from the value "1.005"
     * will set the decimal places to 3 and then set the value to 1005.
     *
     * @warning
     * Use extreme caution with this version, if input does not contain numeric values the
     * validator will be removed.
     *
     * @param[in] value  value to set
     */
    virtual void fromSessionField( const QString& value, no_numeric_ip_validation_t );

    /**
     * Reset input truncation flag.
     *
     * When input trunction is turned on the first digit you type into a newly focused field will
     * truncate the existing text. This method will allow you to truncate text when focus has
     * remained in the widget.
     */
    virtual void resetInputTruncationFlag();

    /**
     * Convert text input into an integer value.
     *
     * @returns  value
     */
    virtual int toInt() const;

    /**
     * Convert text input into a session message field value.
     *
     * For example, if the field current has "1,302 kg" this method will return "1.302".
     *
     * @returns  value
     */
    virtual QString toSessionField() const;

public Q_SLOTS:

    /**
     * Set widget text.
     *
     * @param[in] value  text value
     */
    virtual void setText( const QString& value );

protected:

    QString decimal_;                   ///< Character that represents a decimal point.
    int decimalPlaces_;                 ///< Number of decimal places.
    const QIntValidator *validator_;    ///< Line edit default integer validator.

    QString prefix_;                    ///< Prefix string when displayed.
    QString suffix_;                    ///< Suffix string when displayed.

    bool wrap_;                         ///< Wrap mode enabled or not.
    bool leadingZeros_;                 ///< Allow leading zeros or not.
    bool truncate_;                     ///< First input into the field truncates value to input.
    bool zeroEmpty_;                    ///< Zero out empty field upon focus gain.
    int precision_;                     ///< Input decimal place precision.

    bool allowUpdatesWhileFocused_;     ///< Allow programmatic updates when focused.

    ///////////////////////////////////////////////////////////////////////////
    // Methods

    /**
     * Event filter.
     *
     * @param[in,out] e  event
     * @returns  @c true to ignore event, @c false otherwise
     */
    virtual bool event( QEvent *e );

    /**
     * Focus in event.
     *
     * @param[in,out] event  focus event
     */
    virtual void focusInEvent( QFocusEvent *event );

    /**
     * Focus out event.
     *
     * @param[in, out] event  focus event
     */
    virtual void focusOutEvent( QFocusEvent *event );

    /**
     * Key press event.
     *
     * @param[in,out] event  key event
     */
    virtual void keyPressEvent( QKeyEvent *event );

private Q_SLOTS:

    /**
     * Slot for cursor position change.
     */
    void onCursorPositionChanged( int oldPos, int newPos );

    /**
     * Slot for when text changes.
     */
    void onTextChanged( const QString& text );

private:

    int fixedLengthFormat_;

    bool firstEntry_;
    bool userTextInput_;

    QIntValidator *defaultValidator_;
    QMutex *mutex_;

    /**
     * Initialize.
     */
    void initialize();

    /**
     * Retrieve formatting.
     */
    QString format() const;

    /**
     * Add prefix and suffix to passed in string.
     */
    QString decorate( const QString& text ) const;

    /**
     * Remove prefix and suffix from passed in string.
     */
    QString strip( const QString& text ) const;

    /**
     * Convert text to integer.
     */
    int convertToInt( const QString& text ) const;

    /**
     * Force cursor position.
     */
    void forceCursorPosition();

    /**
     * Emit editing finished signal.
     */
    void emitEditingFinished();

};

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

/** @} */

#endif // TABBABLEIPLINEEDIT_H
