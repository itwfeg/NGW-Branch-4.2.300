/**
 * @file labeledplaintextedit.h
 * Plain text edit widget with custom background, highlighting, and label.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef LABELEDPLAINTEXTEDIT_H
#define LABELEDPLAINTEXTEDIT_H

#include "labeledwidget.h"
#include "icons.h"

#include <QPlainTextEdit>

class QVBoxLayout;

/// Hobart namespace.
namespace hobart {

class pushButton;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Plain text edit widget with custom background, highlighting, and label.
class labeledPlainTextEdit : public labeledWidget<QPlainTextEdit>
{
    Q_OBJECT

    Q_ENUMS( IconVisiblityMode )
    Q_PROPERTY( bool expandable READ expandable WRITE setExpandable )
    Q_PROPERTY( IconVisiblityMode expandedTextIconVisible READ expandedTextIconVisible WRITE setExpandedTextIconVisible )
    Q_PROPERTY( bool printButtonVisible READ printButtonVisible WRITE setPrintButtonVisible )
    Q_PROPERTY( bool sizeToFit READ sizeToFit WRITE setSizeToFit )
    Q_PROPERTY( QString text READ text WRITE setText )

    typedef labeledPlainTextEdit _Myt;
    typedef labeledWidget<QPlainTextEdit> _Mybase;

Q_SIGNALS:

    /// Signal for block count changed.
    /**
     * @param[in] newBlockCount  new block count
     */
    void blockCountChanged( int newBlockCount );

    /// Signal for when copy is available.
    /**
     * @param[in] yes  @c true when copy available, @c false otherwise
     */
    void copyAvailable( bool yes );

    /// Signal for cursor position changed.
    void cursorPositionChanged();

    /// Signal for when widget gains or loses focus.
    /**
     * @param[in] hasFocus  @c true for when gained focus, @c false for when lost focus
     * @param[in] rect  widget geometry
     */
    void focused( bool hasFocus, const QRect& rect );

    /// Signal for invalid access level.
    void invalidAccessLevel();

    /// Signal for modification changed.
    /**
     * @param[in] changed  @c true when changed, @c false otherwise
     */
    void modificationChanged( bool changed );

    /// Signal for when the print button is clicked.
    void printButtonClicked();

    /// Signal for redo available.
    /**
     * @param[in] available  @c true when available, @c false otherwise
     */
    void redoAvailable( bool available );

    /// Signal for selection changed.
    void selectionChanged();

    /// Signal for text changed.
    void textChanged();

    /// Signal for undo available.
    /**
     * @param[in] available  @c true when available, @c false otherwise
     */
    void undoAvailable( bool available );

    /// Signal for update request.
    /**
     * @param[in] rect  update rect
     * @param[in] dy  location
     */
    void updateRequest( const QRect& rect, int dy );

public:

    /// Modes for expanded text icon visiblity.
    enum IconVisiblityMode
    {
        Never,                                      ///< Never show the icon.
        WhenToBigToFit,                             ///< Show when text is too large.
        Always                                      ///< Always show the icon.
    };

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /**
     * Constructor.
     *
     * @param[in] parent  parent widget
     * @param[in] image  background image for the widget
     */
    explicit labeledPlainTextEdit(
        QWidget *parent = 0,
        const QPixmap& image = QPixmap() );

    /**
     * Constructor.
     *
     * @param[in] text  label text
     * @param[in] parent  parent widget
     * @param[in] image  background image for the widget
     */
    explicit labeledPlainTextEdit(
        const QString& text,
        QWidget *parent = 0,
        const QPixmap& image = QPixmap() );

    /**
     * Destructor.
     */
    virtual ~labeledPlainTextEdit();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve if widget is expandable.
    /**
     * @return  @c true if expandable, @c false otherwise
     */
    virtual bool expandable() const {return expandable_;}

    /// Retrieve visibility of the expanded text icon.
    /**
     * This property controls whether the icon will become visible when there is too much text to
     * display in the widget.
     *
     * @return  @c true if icon can become visible, @c false otherwise
     */
    virtual IconVisiblityMode expandedTextIconVisible() const {return expandedTextIconVisible_;}

    /// Retrieve font of line edit.
    /**
     * @return  font
     */
    virtual QFont font() const {return child_->font();}

    /// Retrieve visibility of the print button.
    /**
     * @return  @c true if button is visible, @c false otherwise
     */
    virtual bool printButtonVisible() const;

    /// Set if widget is expandable.
    /**
     * @return  @c true if expandable, @c false otherwise
     */
    virtual void setExpandable( bool value ) {expandable_ = value;}

    /// Set visibility of the expanded text icon.
    /**
     * This property controls whether the icon will become visible when there is too much text to
     * display in the widget.
     *
     * @param[in]  @c true if icon can become visible when text too large, @c false otherwise
     */
    virtual void setExpandedTextIconVisible( IconVisiblityMode value );

    /// Set font of line edit.
    /**
     * @param[in] value  font
     */
    virtual void setFont( const QFont& value ) {child_->setFont( value );}

    /// Set visibility of the print button.
    /**
     * @param[in] value  @c true to make button visible, @c false otherwise
     */
    virtual void setPrintButtonVisible( bool value );

    /// Set if text sizes to fit.
    /**
     * @param[in] value  @c true if sizes, @c false otherwise
     */
    virtual void setSizeToFit( bool value ) {sizeToFit_ = value;}

    /// Retrieve if text sizes to fit.
    /**
     * @return  @c true if sizes, @c false otherwise
     */
    virtual bool sizeToFit() const {return sizeToFit_;}

    /// Retrieve line edit text.
    /**
     * @return  line edit text value
     */
    virtual QString text() const {return child_->toPlainText();}

public Q_SLOTS:

    /// Append html.
    /**
     * @param[in] html  data to append
     */
    virtual void appendHtml( const QString& html ) {child_->appendHtml( html );}

    /// Append plain text.
    /**
     * @param[in] text  data to append
     */
    virtual void appendPlainText( const QString& text ) {child_->appendPlainText( text );}

    /// Center cursor.
    virtual void centerCursor() {child_->centerCursor();}

    /// Clear edit control.
    virtual void clear() {child_->clear();}

    /// Copy edit control.
    virtual void copy() const {child_->copy();}

    /// Cut edit control.
    virtual void cut() {child_->cut();}

    /// Insert plain text.
    /**
     * @param[in] text  data to append
     */
    virtual void insertPlainText( const QString& text ) {child_->insertPlainText( text );}

    /// Paste edit control.
    virtual void paste() {child_->paste();}

    /// Redo action.
    virtual void redo() {child_->redo();}

    /// Select all text.
    virtual void selectAll() {child_->selectAll();}

    /// Set plain text.
    /**
     * @param[in] text  text value to set
     */
    virtual void setPlainText( const QString& text ) {setText( text );}

    /// Set text.
    /**
     * @param[in] value  text value to set
     */
    virtual void setText( const QString& value );

    /// Undo action.
    virtual void undo() {child_->undo();}

protected:

    // ========================================================================
    // Events
    // ========================================================================

    /// Mouse press event.
    /**
     * @param[in,out] e  mouse event
     */
    virtual void mousePressEvent( QMouseEvent *e );

    // ========================================================================
    // Methods
    // ========================================================================

    /**
     * Event filter.
     *
     * @param[in,out] obj  object event filter was attached to
     * @param[in,out] event  event that is occuring
     * @returns  @c true if event @p event should be filtered, @c false otherwise
     */
    virtual bool eventFilter( QObject *obj, QEvent *event );

private Q_SLOTS:

    /// Slot for when drop down icon is clicked.
    void onExpandedTextIconClicked();

private:

    bool expandable_;
    IconVisiblityMode expandedTextIconVisible_;

    bool sizeToFit_;

    pushButton *printButton_;

    QTimer *timer_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

    //
    // drop down icon
    //

    /// Drop down arrow icon
    class expandedTextIcon : public hobart::icon
    {
        typedef expandedTextIcon _Myt;
        typedef hobart::icon _Mybase;

    public:

        // ====================================================================
        // CTOR / DTOR
        // ====================================================================

        /// Constructor.
        /**
         * @param[in] parent  parent widget
         */
        explicit expandedTextIcon( QWidget *parent = 0 );

        /// Destructor.
        virtual ~expandedTextIcon();

    };

    expandedTextIcon *expandedTextIcon_;

};

///////////////////////////////////////////////////////////////////////////////////////////////////
} // hobart namespace

/** @} */

#endif // LABELEDPLAINTEXTEDIT_H
