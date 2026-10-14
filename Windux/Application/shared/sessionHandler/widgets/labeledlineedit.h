/**
 * @file labeledlineedit.h
 * Line edit widget with custom background, highlighting, and label.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef LABELEDLINEEDIT_H
#define LABELEDLINEEDIT_H

#include "labeledwidget.h"

#include <QLineEdit>
#include <QValidator>

/// Hobart namespace.
namespace hobart {

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class labeledLineEdit
 * @brief Line edit widget with custom background, highlighting, and label.
 */
class labeledLineEdit : public labeledWidget<QLineEdit>
{
    Q_OBJECT

    Q_PROPERTY( QString text READ text WRITE setText )

    typedef labeledLineEdit _Myt;
    typedef labeledWidget<QLineEdit> _Mybase;

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
    explicit labeledLineEdit(
        QWidget *parent = 0,
        const QPixmap& image = QPixmap() );

    /**
     * Constructor.
     *
     * @param[in] text  label text
     * @param[in] parent  parent widget
     * @param[in] image  background image for the widget
     */
    explicit labeledLineEdit(
        const QString& text,
        QWidget *parent = 0,
        const QPixmap& image = QPixmap() );

    /**
     * Destructor.
     */
    virtual ~labeledLineEdit();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
      * Retrieve text alignment of line edit.
      *
      * @returns  alignment
      */
    virtual Qt::Alignment alignment() const {return child_->alignment();}

    /**
     * Retrieve font of line edit.
     *
     * @returns  font
     */
    virtual QFont font() const {return child_->font();}

    /**
     * Set line edit alignment.
     *
     * @param[in] value  alignment
     */
    virtual void setAlignment( Qt::Alignment value ) {child_->setAlignment( value );}

    /**
     * Set font of line edit.
     *
     * @param[in] value  font
     */
    virtual void setFont( const QFont& value ) {child_->setFont( value );}

    /**
     * Set line edit input validator.
     *
     * @param[in] value  pointer to validator
     */
    virtual void setValidator( const QValidator *value ) {child_->setValidator( value );}

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
    virtual const QValidator *validator() const {return child_->validator();}

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

private Q_SLOTS:

    /**
     * Slot for toggle button.
     */
    virtual void onToggleButtonClicked();

private:

    /**
     * Initialize.
     */
    void initialize();

};

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

/** @} */

#endif // LABELEDLINEEDIT_H
