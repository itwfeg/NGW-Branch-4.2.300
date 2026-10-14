/**
 * @file labelednumericlineedit.h
 * Numeric line edit widget with custom background, highlighting, and label.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef LABELEDNUMERICLINEEDIT_H
#define LABELEDNUMERICLINEEDIT_H

#include "labeledwidget.h"
#include "numericlineedit.h"

#include <QIntValidator>

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class labeledNumericLineEdit
 * @brief Numeric line edit widget with custom background, highlighting, and label.
 */
class labeledNumericLineEdit : public labeledWidget<numericLineEdit>
{
    Q_OBJECT

    Q_PROPERTY( QString text READ text WRITE setText )

    typedef labeledNumericLineEdit _Myt;
    typedef labeledWidget<numericLineEdit> _Mybase;

Q_SIGNALS:

    /**
     * Signal for cursor position changed.
     *
     * @param[in] oldPos  old position
     * @param[in] newPos  new position
     */
    void cursorPositionChanged( int oldPos, int newPos );

    /**
     * Signal for editing finished.
     */
    void editingFinished();

    /**
     * Slot for when widget gains or loses focus.
     *
     * @param[in] hasFocus  @c true for when gained focus, @c false for when lost focus
     * @param[in] rect  widget geometry
     */
    void focused( bool hasFocus, const QRect& rect );

    /**
     * Signal for invalid access level.
     */
    void invalidAccessLevel();

    /**
     * Signal for return button pressed.
     */
    void returnPressed();

    /**
     * Signal for selection changed.
     */
    void selectionChanged();

    /**
     * Signal for text changed.
     *
     * @param[in] text  text value
     */
    void textChanged( const QString& text );

    /**
     * Signal for text edited.
     *
     * @param[in] text  text value
     */
    void textEdited( const QString& text );

public:

    ///////////////////////////////////////////////////////////////////////////
    // CTOR / DTOR

    /**
     * Constructor.
     *
     * @param[in] parent  parent widget
     * @param[in] image  background image for the widget
     */
    explicit labeledNumericLineEdit(
        QWidget *parent = 0,
        const QPixmap& image = QPixmap() );

    /**
     * Constructor.
     *
     * @param[in] text  label text
     * @param[in] parent  parent widget
     * @param[in] image  background image for the widget
     */
    explicit labeledNumericLineEdit(
        const QString& text,
        QWidget *parent = 0,
        const QPixmap& image = QPixmap() );

    /**
     * Destructor.
     */
    virtual ~labeledNumericLineEdit();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
      * Retrieve text alignment of line edit.
      *
      * @returns  alignment
      */
    virtual Qt::Alignment alignment() const {return child_->alignment();}

    /**
     * Retrieve decimal place character.
     *
     * @returns  decimal place character
     */
    virtual QString decimal() const {return child_->decimal();}

    /**
     * Retrieve number of decimal places (precision).
     *
     * @returns  number of decimal places
     */
    virtual int decimalPlaces() const {return child_->decimalPlaces();}

    /**
     * Retrieve font of line edit.
     *
     * @returns  font
     */
    virtual QFont font() const {return child_->font();}

    /**
     * Check if focusing into field should zero an empty field or not.
     *
     * When the edit box is empty and you focus into it it will be zeroed out. When this property
     * is @c false the edit box will remain empty.
     *
     * @returns  @c true to zero the field, @c false otherwise
     */
    virtual bool inputFocusZeroing() const {return child_->inputFocusZeroing();}

    /**
     * Retrieve if leading zeros are shown or not.
     *
     * @note
     * This only applies to widgets with wrapping and no decimal places.
     *
     * @returns  @c true if allow leading zeros, @c false otherwise
     */
    virtual bool inputLeadingZeros() const {return child_->inputLeadingZeros();}

    /**
     * Retrieve input decimal place precision.
     *
     * @returns  decimal place input precision
     */
    virtual int inputPrecision() const {return child_->inputPrecision();}

    /**
     * Retrieve if first input into the field should reset existing input.
     *
     * @returns  @c true if enable trucation, @c false otherwise
     */
    virtual bool inputTruncation() const {return child_->inputTruncation();}

    /**
     * Retrieve current input truncation state.
     *
     * @returns  @c true if setn, @c false otherwise
     */
    virtual bool inputTruncationFlag() const {return child_->inputTruncationFlag();}

    /**
     * Retrieve if input wrapping is enabled or not.
     *
     * @returns  @c true if allow wrapping, @c false otherwise
     */
    virtual bool inputWrapping() const {return child_->inputWrapping();}

    /**
     * Retrieve prefix string.
     *
     * @returns  prefix
     */
    virtual QString prefix() const {return child_->prefix();}

    /**
     * Set line edit alignment.
     *
     * @param[in] value  alignment
     */
    virtual void setAlignment( Qt::Alignment value ) {child_->setAlignment( value );}

    /**
     * Set the decimal place character.
     *
     * @example
     * When this is set to ',' the value 12.34 will display as "12,34". In the same fashion, you
     * could also set this property to '-' in which case it wouls be "12-34".
     *
     * @param[in] value  decimal place character
     */
    virtual void setDecimal( const QString& value ) {child_->setDecimal( value );}

    /**
     * Set number of decimal places (precision).
     *
     * @example
     * For two decimal place precision (i.e. 12.34) use 2. For three decimal place precision
     * (i.e. 12.345) use 3.
     *
     * @param[in] value  number of decimal places
     */
    virtual void setDecimalPlaces( int value ) {child_->setDecimalPlaces( value );}

    /**
     * Set font of line edit.
     *
     * @param[in] value  font
     */
    virtual void setFont( const QFont& value ) {child_->setFont( value );}

    /**
     * Set if control is forced.
     *
     * @param[in] value  @c true for forced, @c false for otherwise
     */
    virtual void setForced( bool value );

    /**
     * Set if focusing into field should zero an empty field or not.
     *
     * When the edit box is empty and you focus into it it will be zeroed out. When this property
     * is @c false the edit box will remain empty.
     *
     * @param[in] value  @c true to zero the field, @c false otherwise
     */
    virtual void setInputFocusZeroing( bool value ) {child_->setInputFocusZeroing( value );}

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
    virtual void setInputLeadingZeros( bool value ) {child_->setInputLeadingZeros( value );}

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
    virtual void setInputPrecision( int value ) {child_->setInputPrecision( value );}

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
    virtual void setInputTruncation( bool value ) {child_->setInputTruncation( value );}

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
    virtual void setInputWrapping( bool value ) {child_->setInputWrapping( value );}

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
    virtual void setPrefix( const QString& value ) {child_->setPrefix( value );}

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
    virtual void setSuffix( const QString& value ) {child_->setSuffix( value );}

    /**
     * Set line edit input validator.
     *
     * @param[in] value  pointer to validator
     */
    virtual void setValidator( const QIntValidator *value ) {child_->setValidator( value );}

    /**
     * Retrieve suffix string.
     *
     * @returns  suffix
     */
    virtual QString suffix() const {return child_->suffix();}

    /**
     * Retrieve line edit text.
     *
     * @returns  line edit text value
     */
    virtual QString text() const {return child_->text();}

    /**
     * Retrieve validator.
     *
     * @returns  const pointer to validator
     */
    virtual const QIntValidator *validator() const {return child_->validator();}

    ///////////////////////////////////////////////////////////////////////////
    // Methods

    /**
     * Set text value from an integer.
     *
     * @param[in] value  value to set
     */
    virtual void fromInt( int value ) {child_->fromInt( value );}

    /**
     * Set text value from a session message field value.
     *
     * This method will also set the decimal places. For example setting from the value "1.005"
     * will set the decimal places to 3 and then set the value to 1005.
     *
     * @param[in] value  value to set
     */
    virtual void fromSessionField( const QString& value ) {child_->fromSessionField( value );}

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
    virtual void fromSessionField( const QString& value, no_numeric_validation_t ) {child_->fromSessionField( value, no_numeric_validation );}

    /**
     * Reset input truncation flag.
     *
     * When input trunction is turned on the first digit you type into a newly focused field will
     * truncate the existing text. This method will allow you to truncate text when focus has
     * remained in the widget.
     */
    virtual void resetInputTruncationFlag() {child_->resetInputTruncationFlag();}

    /**
     * Convert text input into an integer value.
     *
     * @returns  value
     */
    virtual int toInt() const {return child_->toInt();}

    /**
     * Convert text input into a session message field value.
     *
     * For example, if the field current has "1,302 kg" this method will return "1.302".
     *
     * @returns  value
     */
    virtual QString toSessionField() const {return child_->toSessionField();}

public Q_SLOTS:

    /**
     * Clear edit control.
     */
    virtual void clear() {child_->clear();}

    /**
     * Copy edit control.
     */
    virtual void copy() const {child_->copy();}

    /**
     * Cut edit control.
     */
    virtual void cut() {child_->cut();}

    /**
     * Paste edit control.
     */
    virtual void paste() {child_->paste();}

    /**
     * Redo action.
     */
    virtual void redo() {child_->redo();}

    /**
     * Select all text.
     */
    virtual void selectAll() {child_->selectAll();}

    /**
     * Set text.
     *
     * @param[in] value  text value to set
     */
    virtual void setText( const QString& value ) {child_->setText( value );}

    /**
     * Undo action.
     */
    virtual void undo() {child_->undo();}

protected:

    /**
     * Event filter.
     *
     * @param[in,out] obj  object event filter was attached to
     * @param[in,out] event  event that is occuring
     * @returns  @c true if event @p event should be filtered, @c false otherwise
     */
    virtual bool eventFilter( QObject *obj, QEvent *event );

private:

    /**
     * Initialize.
     */
    void initialize();

};

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

/** @} */

#endif // LABELEDNUMERICLINEEDIT_H
