/**
 * @file labeleddoublenumericlineedit.h
 * Double numeric line edit widget with custom background, highlighting, and label.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef LABELEDDOUBLENUMERICLINEEDIT_H
#define LABELEDDOUBLENUMERICLINEEDIT_H

#include "labelednumericlineedit.h"

class QTimer;

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class labeledDoubleNumericLineEdit
 * @brief Double numeric line edit widget with custom background, highlighting, and label.
 */
class labeledDoubleNumericLineEdit : public labeledNumericLineEdit
{
    Q_OBJECT

    Q_PROPERTY( QString text READ text WRITE setText )
    Q_PROPERTY( QString textSecond READ textSecond WRITE setTextSecond )

    typedef labeledDoubleNumericLineEdit _Myt;
    typedef labeledNumericLineEdit _Mybase;

Q_SIGNALS:

    /**
     * Signal for cursor position changed.
     *
     * @param[in] oldPos  old position
     * @param[in] newPos  new position
     */
    void cursorPositionChangedSecond( int oldPos, int newPos );

    /**
     * Signal for editing finished.
     */
    void editingFinishedSecond();

    /**
     * Slot for when widget gains or loses focus.
     *
     * @param[in] hasFocus  @c true for when gained focus, @c false for when lost focus
     * @param[in] rect  widget geometry
     */
    void focusedSecond( bool hasFocus, const QRect& rect );

    /**
     * Signal for return button pressed.
     */
    void returnPressedSecond();

    /**
     * Signal for selection changed.
     */
    void selectionChangedSecond();

    /**
     * Signal for text changed.
     *
     * @param[in] text  text value
     */
    void textChangedSecond( const QString& text );

    /**
     * Signal for text edited.
     *
     * @param[in] text  text value
     */
    void textEditedSecond( const QString& text );

public:

    ///////////////////////////////////////////////////////////////////////////
    // CTOR / DTOR

    /**
     * Constructor.
     *
     * @param[in] parent  parent widget
     * @param[in] image  background image for the widget
     */
    explicit labeledDoubleNumericLineEdit(
        QWidget *parent = 0,
        const QPixmap& image = QPixmap() );

    /**
     * Constructor.
     *
     * @param[in] text  label text
     * @param[in] parent  parent widget
     * @param[in] image  background image for the widget
     */
    explicit labeledDoubleNumericLineEdit(
        const QString& text,
        QWidget *parent = 0,
        const QPixmap& image = QPixmap() );

    /**
     * Destructor.
     */
    virtual ~labeledDoubleNumericLineEdit();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Retrieve text alignment of line edit.
     *
     * @returns  alignment
     */
    virtual Qt::Alignment alignmentSecond() const {return childSecond_->alignment();}

    /**
     * Retrieve if widget is enabled.
     *
     * @returns  @c true if enabled, @c false otherwise
     */
    virtual bool isEnabledSecond() const {return childSecond_->isEnabled();}

    /**
     * Retrieve font of line edit.
     *
     * @returns  font
     */
    virtual QFont fontSecond() const {return childSecond_->font();}

    /**
     * Set line edit alignment.
     *
     * @param[in] value  alignment
     */
    virtual void setAlignmentSecond( Qt::Alignment value ) {childSecond_->setAlignment( value );}

    /**
     * Set if widget is enabled.
     *
     * @param[in] value  @c true to enabled, @c false otherwise
     */
    virtual void setEnabledSecond( bool value ) {childSecond_->setEnabled( value );}

    /**
     * Set font of line edit.
     *
     * @param[in] value  font
     */
    virtual void setFontSecond( const QFont& value ) {childSecond_->setFont( value );}

    /**
     * Set if control is read only.
     *
     * @param[in] value  @c true for read only, @c false for otherwise
     */
    virtual void setReadOnly( bool value );

    /**
     * Set line edit input validator.
     *
     * @param[in] value  pointer to validator
     */
    virtual void setValidatorSecond( const QValidator *value ) {childSecond_->setValidator( value );}

    /**
     * Retrieve line edit text.
     *
     * @returns  line edit text value
     */
    virtual QString textSecond() const {return childSecond_->text();}

    /**
     * Retrieve validator.
     *
     * @returns  const pointer to validator
     */
    virtual const QValidator *validatorSecond() const {return childSecond_->validator();}

public Q_SLOTS:

    /**
     * Clear edit control.
     */
    virtual void clearSecond() {childSecond_->clear();}

    /**
     * Copy edit control.
     */
    virtual void copySecond() const {childSecond_->copy();}

    /**
     * Cut edit control.
     */
    virtual void cutSecond() {childSecond_->cut();}

    /**
     * Paste edit control.
     */
    virtual void pasteSecond() {childSecond_->paste();}

    /**
     * Redo action.
     */
    virtual void redoSecond() {childSecond_->redo();}

    /**
     * Select all text.
     */
    virtual void selectAllSecond() {childSecond_->selectAll();}

    /**
     * Set text.
     *
     * @param[in] value  text value to set
     */
    virtual void setTextSecond( const QString& value ) {childSecond_->setText( value );}

    /**
     * Undo action.
     */
    virtual void undoSecond() {childSecond_->undo();}

protected:

    QLineEdit *childSecond_;                        ///< Second child widget.

    /**
     * Event filter.
     *
     * @param[in,out] obj  object event filter was attached to
     * @param[in,out] event  event that is occuring
     * @returns  @c true if event @p event should be filtered, @c false otherwise
     */
    virtual bool eventFilter( QObject *obj, QEvent *event );

private Q_SLOTS:

    /**
     * Slot for timer timeout.
     */
    void onTimeout();

private:

    QTimer *timer_;

    /**
     * Initialize.
     */
    void initialize();

};

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

/** @} */

#endif // LABELEDDOUBLENUMERICLINEEDIT_H
