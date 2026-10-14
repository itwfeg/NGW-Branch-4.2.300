/**
 * @file labeleddoublenumericlineeditarray.h
 * Double numeric line array edit widget with custom background, highlighting, and label.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef LABELEDDOUBLENUMERICLINEEDITARRAY_H
#define LABELEDDOUBLENUMERICLINEEDITARRAY_H

#include "labeledarraywidget.h"

#include "labeleddoublenumericlineedit.h"

class QSignalMapper;

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class labeledDoubleNumericLineEditArray
 * @brief Double numeric line array edit widget with custom background, highlighting, and label.
 */
class labeledDoubleNumericLineEditArray : public labeledArrayWidget<labeledDoubleNumericLineEdit>
{
    Q_OBJECT

    typedef labeledDoubleNumericLineEditArray _Myt;
    typedef labeledArrayWidget<labeledDoubleNumericLineEdit> _Mybase;

Q_SIGNALS:

    /**
     * Signal for cursor position changed.
     *
     * @param[in] index  index of widget emitting signal
     * @param[in] oldPos  old position
     * @param[in] newPos  new position
     */
    void cursorPositionChanged( int index, int oldPos, int newPos );

    /**
     * Signal for cursor position changed.
     *
     * @param[in] index  index of widget emitting signal
     * @param[in] oldPos  old position
     * @param[in] newPos  new position
     */
    void cursorPositionChangedSecond( int index, int oldPos, int newPos );

    /**
     * Signal for editing finished.
     *
     * @param[in] index  index of widget emitting signal
     */
    void editingFinished( int index );

    /**
     * Signal for editing finished.
     *
     * @param[in] index  index of widget emitting signal
     */
    void editingFinishedSecond( int index );

    /**
     * Slot for when widget gains or loses focus.
     *
     * @param[in] index  index of widget emitting signal
     * @param[in] hasFocus  @c true for when gained focus, @c false for when lost focus
     * @param[in] rect  widget geometry
     */
    void focused( int index, bool hasFocus, const QRect& rect );

    /**
     * Slot for when widget gains or loses focus.
     *
     * @param[in] index  index of widget emitting signal
     * @param[in] hasFocus  @c true for when gained focus, @c false for when lost focus
     * @param[in] rect  widget geometry
     */
    void focusedSecond( int index, bool hasFocus, const QRect& rect );

    /**
     * Signal for return button pressed.
     *
     * @param[in] index  index of widget emitting signal
     */
    void returnPressed( int index );

    /**
     * Signal for return button pressed.
     *
     * @param[in] index  index of widget emitting signal
     */
    void returnPressedSecond( int index );

    /**
     * Signal for selection changed.
     *
     * @param[in] index  index of widget emitting signal
     */
    void selectionChanged( int index );

    /**
     * Signal for selection changed.
     *
     * @param[in] index  index of widget emitting signal
     */
    void selectionChangedSecond( int index );

    /**
     * Signal for text changed.
     *
     * @param[in] index  index of widget emitting signal
     * @param[in] text  text value
     */
    void textChanged( int index, const QString& text );

    /**
     * Signal for text changed.
     *
     * @param[in] index  index of widget emitting signal
     * @param[in] text  text value
     */
    void textChangedSecond( int index, const QString& text );

    /**
     * Signal for text edited.
     *
     * @param[in] index  index of widget emitting signal
     * @param[in] text  text value
     */
    void textEdited( int index, const QString& text );

    /**
     * Signal for text edited.
     *
     * @param[in] index  index of widget emitting signal
     * @param[in] text  text value
     */
    void textEditedSecond( int index, const QString& text );

public:

    ///////////////////////////////////////////////////////////////////////////
    // CTOR / DTOR

    /**
     * Constructor.
     *
     * @param[in] count  count of widgets in array
     * @param[in] parent  parent widget
     * @param[in] image  background image for the widget
     */
    explicit labeledDoubleNumericLineEditArray(
        int count,
        QWidget *parent = 0,
        const QPixmap& image = QPixmap() );

    /**
     * Destructor.
     */
    virtual ~labeledDoubleNumericLineEditArray();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
      * Retrieve text alignment of line edit.
      *
      * @returns  alignment
      */
    virtual Qt::Alignment alignment() const {return child_->widgetAt( 0 )->alignment();}

    /**
      * Retrieve text alignment of line edit.
      *
      * @returns  alignment
      */
    virtual Qt::Alignment alignmentSecond() const {return child_->widgetAt( 0 )->alignmentSecond();}

    /**
     * Retrieve font of line edit.
     *
     * @returns  font
     */
    virtual QFont font() const {return child_->widgetAt( 0 )->font();}

    /**
     * Retrieve font of line edit.
     *
     * @returns  font
     */
    virtual QFont fontSecond() const {return child_->widgetAt( 0 )->fontSecond();}

    /**
     * Set line edit alignment.
     *
     * @param[in] value  alignment
     */
    virtual void setAlignment( Qt::Alignment value );

    /**
     * Set line edit alignment.
     *
     * @param[in] value  alignment
     */
    virtual void setAlignmentSecond( Qt::Alignment value );

    /**
     * Set if widget is enabled.
     *
     * @param[in] value  @c true to enabled, @c false otherwise
     */
    virtual void setEnabledSecond( bool value );

    /**
     * Set font of line edit.
     *
     * @param[in] value  font
     */
    virtual void setFont( const QFont& value );

    /**
     * Set font of line edit.
     *
     * @param[in] value  font
     */
    virtual void setFontSecond( const QFont& value );

    /**
     * Set line edit input validator.
     *
     * @param[in] value  pointer to validator
     */
    virtual void setValidator( const QIntValidator *value );

    /**
     * Set line edit input validator.
     *
     * @param[in] value  pointer to validator
     */
    virtual void setValidatorSecond( const QValidator *value );

    /**
     * Retrieve line edit text.
     *
     * @param[in] index  index of widget
     * @returns  line edit text value
     */
    virtual QString text( int index ) const {return child_->widgetAt( index )->text();}

    /**
     * Retrieve line edit text.
     *
     * @param[in] index  index of widget
     * @returns  line edit text value
     */
    virtual QString textSecond( int index ) const {return child_->widgetAt( index )->textSecond();}

    /**
     * Retrieve validator.
     *
     * @returns  const pointer to validator
     */
    virtual const QValidator *validator() const {return child_->widgetAt( 0 )->validator();}

    /**
     * Retrieve validator.
     *
     * @returns  const pointer to validator
     */
    virtual const QValidator *validatorSecond() const {return child_->widgetAt( 0 )->validatorSecond();}

public Q_SLOTS:

    /**
     * Clear all edit controls.
     */
    virtual void clear();

    /**
     * Clear all edit controls.
     */
    virtual void clearSecond();

    /**
     * Clear edit control.
     *
     * @param[in] index  index of widget
     */
    virtual void clear( int index ) {child_->widgetAt( index )->clear();}

    /**
     * Clear edit control.
     *
     * @param[in] index  index of widget
     */
    virtual void clearSecond( int index ) {child_->widgetAt( index )->clearSecond();}

    /**
     * Copy edit control.
     *
     * @param[in] index  index of widget
     */
    virtual void copy( int index ) const {child_->widgetAt( index )->copy();}

    /**
     * Copy edit control.
     *
     * @param[in] index  index of widget
     */
    virtual void copySecond( int index ) const {child_->widgetAt( index )->copySecond();}

    /**
     * Cut edit control.
     *
     * @param[in] index  index of widget
     */
    virtual void cut( int index ) {child_->widgetAt( index )->cut();}

    /**
     * Cut edit control.
     *
     * @param[in] index  index of widget
     */
    virtual void cutSecond( int index ) {child_->widgetAt( index )->cutSecond();}

    /**
     * Paste edit control.
     *
     * @param[in] index  index of widget
     */
    virtual void paste( int index ) {child_->widgetAt( index )->paste();}

    /**
     * Paste edit control.
     *
     * @param[in] index  index of widget
     */
    virtual void pasteSecond( int index ) {child_->widgetAt( index )->pasteSecond();}

    /**
     * Redo action.
     *
     * @param[in] index  index of widget
     */
    virtual void redo( int index ) {child_->widgetAt( index )->redo();}

    /**
     * Redo action.
     *
     * @param[in] index  index of widget
     */
    virtual void redoSecond( int index ) {child_->widgetAt( index )->redoSecond();}

    /**
     * Select all text.
     *
     * @param[in] index  index of widget
     */
    virtual void selectAll( int index ) {child_->widgetAt( index )->selectAll();}

    /**
     * Select all text.
     *
     * @param[in] index  index of widget
     */
    virtual void selectAllSecond( int index ) {child_->widgetAt( index )->selectAllSecond();}

    /**
     * Set text.
     *
     * @param[in] index  index of widget
     * @param[in] value  text value to set
     */
    virtual void setText( int index, const QString& value ) {child_->widgetAt( index )->setText( value );}

    /**
     * Set text.
     *
     * @param[in] index  index of widget
     * @param[in] value  text value to set
     */
    virtual void setTextSecond( int index, const QString& value ) {child_->widgetAt( index )->setTextSecond( value );}

    /**
     * Undo action.
     *
     * @param[in] index  index of widget
     */
    virtual void undo( int index ) {child_->widgetAt( index )->undo();}

    /**
     * Undo action.
     *
     * @param[in] index  index of widget
     */
    virtual void undoSecond( int index ) {child_->widgetAt( index )->undoSecond();}

private Q_SLOTS:

    /**
     * Slot for cursor position changed.
     */
    void onCursorPositionChanged( int oldPos, int newPos ) {Q_EMIT cursorPositionChanged( focusedWidget(), oldPos, newPos );}

    /**
     * Slot for cursor position changed second.
     */
    void onCursorPositionChangedSecond( int oldPos, int newPos ) {Q_EMIT cursorPositionChangedSecond( focusedWidget(), oldPos, newPos );}

    /**
     * Slot for editing finished.
     */
    void onEditingFinished() {Q_EMIT editingFinished( focusedWidget() );}

    /**
     * Slot for editing finished second.
     */
    void onEditingFinishedSecond() {Q_EMIT editingFinishedSecond( focusedWidget() );}

    /**
     * Slot for focused.
     */
    void onFocused( bool hasFocus, const QRect& rect ) {Q_EMIT focused( focusedWidget(), hasFocus, rect );}

    /**
     * Slot for focused second.
     */
    void onFocusedSecond( bool hasFocus, const QRect& rect ) {Q_EMIT focusedSecond( focusedWidget(), hasFocus, rect );}

    /**
     * Slot for signal mapper.
     */
    void onMapped( int id ) {focusedWidget_ = id;}

    /**
     * Slot for return pressed.
     */
    void onReturnPressed() {Q_EMIT returnPressed( focusedWidget() );}

    /**
     * Slot for return pressed second.
     */
    void onReturnPressedSecond() {Q_EMIT returnPressedSecond( focusedWidget() );}

    /**
     * Slot for selection changed.
     */
    void onSelectionChanged() {Q_EMIT selectionChanged( focusedWidget() );}

    /**
     * Slot for selection changed second.
     */
    void onSelectionChangedSecond() {Q_EMIT selectionChangedSecond( focusedWidget() );}

    /**
     * Slot for text changed.
     */
    void onTextChanged( const QString& text ) {Q_EMIT textChanged( focusedWidget(), text );}

    /**
     * Slot for text changed second.
     */
    void onTextChangedSecond( const QString& text ) {Q_EMIT textChangedSecond( focusedWidget(), text );}

    /**
     * Slot for text edited.
     */
    void onTextEdited( const QString& text ) {Q_EMIT textEdited( focusedWidget(), text );}

    /**
     * Slot for text edited second.
     */
    void onTextEditedSecond( const QString& text ) {Q_EMIT textEditedSecond( focusedWidget(), text );}

private:

    QSignalMapper *signalMapper_;

    int focusedWidget_;

    /**
     * Initialize.
     */
    void initialize();

    /**
     * Figure out who has focus.
     */
    int focusedWidget() const {return focusedWidget_;}

};

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

/** @} */

#endif // LABELEDDOUBLENUMERICLINEEDITARRAY_H
