/**
 * @file labeledlineeditarray.h
 * Line edit array widget with custom background, highlighting, and label.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef LABELEDLINEEDITARRAY_H
#define LABELEDLINEEDITARRAY_H

#include "labeledarraywidget.h"

#include "labeledlineedit.h"

class QSignalMapper;

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class labeledLineEditArray
 * @brief Line edit array widget with custom background, highlighting, and label.
 */
class labeledLineEditArray : public labeledArrayWidget<labeledLineEdit>
{
    Q_OBJECT

    typedef labeledLineEditArray _Myt;
    typedef labeledArrayWidget<labeledLineEdit> _Mybase;

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
     * Signal for editing finished.
     *
     * @param[in] index  index of widget emitting signal
     */
    void editingFinished( int index );

    /**
     * Slot for when widget gains or loses focus.
     *
     * @param[in] index  index of widget emitting signal
     * @param[in] hasFocus  @c true for when gained focus, @c false for when lost focus
     * @param[in] rect  widget geometry
     */
    void focused( int index, bool hasFocus, const QRect& rect );

    /**
     * Signal for return button pressed.
     *
     * @param[in] index  index of widget emitting signal
     */
    void returnPressed( int index );

    /**
     * Signal for selection changed.
     *
     * @param[in] index  index of widget emitting signal
     */
    void selectionChanged( int index );

    /**
     * Signal for text changed.
     *
     * @param[in] index  index of widget emitting signal
     * @param[in] text  text value
     */
    void textChanged( int index, const QString& text );

    /**
     * Signal for text edited.
     *
     * @param[in] index  index of widget emitting signal
     * @param[in] text  text value
     */
    void textEdited( int index, const QString& text );

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
    explicit labeledLineEditArray(
        int count,
        QWidget *parent = 0,
        const QPixmap& image = QPixmap() );

    /**
     * Destructor.
     */
    virtual ~labeledLineEditArray();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
      * Retrieve text alignment of line edit.
      *
      * @returns  alignment
      */
    virtual Qt::Alignment alignment() const {return child_->widgetAt( 0 )->alignment();}

    /**
     * Retrieve font of line edit.
     *
     * @returns  font
     */
    virtual QFont font() const {return child_->widgetAt( 0 )->font();}

    /**
     * Set line edit alignment.
     *
     * @param[in] value  alignment
     */
    virtual void setAlignment( Qt::Alignment value );

    /**
     * Set font of line edit.
     *
     * @param[in] value  font
     */
    virtual void setFont( const QFont& value );

    /**
     * Set line edit input validator.
     *
     * @param[in] value  pointer to validator
     */
    virtual void setValidator( const QValidator *value );

    /**
     * Retrieve line edit text.
     *
     * @param[in] index  index of widget
     * @returns  line edit text value
     */
    virtual QString text( int index ) const {return child_->widgetAt( index )->text();}

    /**
     * Retrieve validator.
     *
     * @returns  const pointer to validator
     */
    virtual const QValidator *validator() const {return child_->widgetAt( 0 )->validator();}

public Q_SLOTS:

    /**
     * Clear all edit controls.
     */
    virtual void clear();

    /**
     * Clear edit control.
     *
     * @param[in] index  index of widget
     */
    virtual void clear( int index ) {child_->widgetAt( index )->clear();}

    /**
     * Copy edit control.
     *
     * @param[in] index  index of widget
     */
    virtual void copy( int index ) const {child_->widgetAt( index )->copy();}

    /**
     * Cut edit control.
     *
     * @param[in] index  index of widget
     */
    virtual void cut( int index ) {child_->widgetAt( index )->cut();}

    /**
     * Paste edit control.
     *
     * @param[in] index  index of widget
     */
    virtual void paste( int index ) {child_->widgetAt( index )->paste();}

    /**
     * Redo action.
     *
     * @param[in] index  index of widget
     */
    virtual void redo( int index ) {child_->widgetAt( index )->redo();}

    /**
     * Select all text.
     *
     * @param[in] index  index of widget
     */
    virtual void selectAll( int index ) {child_->widgetAt( index )->selectAll();}

    /**
     * Set text.
     *
     * @param[in] index  index of widget
     * @param[in] value  text value to set
     */
    virtual void setText( int index, const QString& value ) {child_->widgetAt( index )->setText( value );}

    /**
     * Undo action.
     *
     * @param[in] index  index of widget
     */
    virtual void undo( int index ) {child_->widgetAt( index )->undo();}

private Q_SLOTS:

    /**
     * Slot for cursor position changed.
     */
    void onCursorPositionChanged( int oldPos, int newPos ) {Q_EMIT cursorPositionChanged( focusedWidget(), oldPos, newPos );}

    /**
     * Slot for editing finished.
     */
    void onEditingFinished() {Q_EMIT editingFinished( focusedWidget() );}

    /**
     * Slot for focus.
     */
    void onFocused( bool hasFocus, const QRect& rect ) {Q_EMIT focused( focusedWidget(),  hasFocus, rect );}

    /**
     * Slot for signal mapper.
     */
    void onMapped( int id ) {focusedWidget_ = id;}

    /**
     * Slot for return pressed.
     */
    void onReturnPressed() {Q_EMIT returnPressed( focusedWidget() );}

    /**
     * Slot for selection changed.
     */
    void onSelectionChanged() {Q_EMIT selectionChanged( focusedWidget() );}

    /**
     * Slot for text changed.
     */
    void onTextChanged( const QString& text ) {Q_EMIT textChanged( focusedWidget(), text );}

    /**
     * Slot for text edited.
     */
    void onTextEdited( const QString& text ) {Q_EMIT textEdited( focusedWidget(), text );}

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

#endif // LABELEDLINEEDITARRAY_H
