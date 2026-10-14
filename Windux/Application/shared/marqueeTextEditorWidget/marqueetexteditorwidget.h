/**
 * @file marqueetexteditorwidget.h
 * Qt widget for editing marquee text.
 *
 * This class offers a user interface for editing marquee text.  That text may then be applied
 * to a @c MarqueeLabelWidget object.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup marqueeEditorCsu
 * @{
 */

#ifndef MARQUEETEXTEDITORWIDGET_H
#define MARQUEETEXTEDITORWIDGET_H

#include <QString>

#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QPlainTextEdit>
#include <QPushButton>

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class MarqueeTextEditorWidget
 * @brief Widget for editing marquee text.
 */
class MarqueeTextEditorWidget : public QWidget
{
    Q_OBJECT

    Q_PROPERTY( QFont font READ font WRITE setFont )
    Q_PROPERTY( QString text READ text WRITE setText )
    Q_PROPERTY( QString plainText READ toPlainText WRITE setPlainText )

    typedef MarqueeTextEditorWidget _Myt;
    typedef QWidget _Mybase;

Q_SIGNALS:

    /**
     * This signal is emitted whenever the position of the cursor changed.
     */
    void cursorPositionChanged();

    /**
     * This signal is emitted whenever the selection changes.
     */
    void selectionChanged();

    /**
     * This signal is emitted whenever the document's content changes; for example, when text
     * is inserted or deleted, or when formatting is applied.
     */
    void textChanged();

    /**
     * This signal is emitted when this widget receives focus.  You could then possibly pop
     * a keyboard widget.
     */
    void haveFocus();

    /**
     * This signal is emitted when this widget loses focus.  You could then possibly remove
     * a keyboard widget from the screen.
     */
    void lostFocus();

public:

    /**
     * Constructor.
     *
     * @param[in] parent  parent @c QWidget object
     */
    MarqueeTextEditorWidget( QWidget *parent = 0 );

    /**
     * Destructor.
     */
    virtual ~MarqueeTextEditorWidget();

    /**
     * Retrieves the font currently being used.
     *
     * @returns  text edit font
     */
    virtual QFont font() const {return text_->font();}

    /**
     * Sets the text editor font.
     *
     * @param[in] value  text edit font
     */
    virtual void setFont( const QFont& value ) {text_->setFont( value );}

    /**
     * Retrieve text editor text.
     *
     * This version will convert all markup string into escape sequences.  For example, the
     * <HOLD> tag will become "\H".
     *
     * @returns  editor text
     */
    virtual QString text() const;

    /**
     * Set text editor text.
     *
     * @param[in] value  text to edit
     */
    virtual void setText( const QString& value );

    /**
     * Retrieve text from widget
     *
     * @returns  text within widget
     */
    virtual QString toPlainText() const {return text_->toPlainText();}

    /**
     * Set widget text.
     *
     * @param[in] value  text to set
     */
    virtual void setPlainText( const QString& value ) {text_->setPlainText( value );}

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

protected:

    /**
     * Event filter for widget.
     *
     * @param[in,out] obj  object receiving event
     * @param[in,out] e  event being sent to object @p obj
     * @returns  @c true if event should be filtered, @c false otherwise
     */
    virtual bool eventFilter( QObject *obj, QEvent *e );

    /**
     * Inserts a tag at the current cursor position.
     *
     * @param[in] tag  tag to insert
     */
    virtual void insertTag( const QString& tag );

private Q_SLOTS:

    /**
     * Slot for when text cursor position changes.
     */
    void onCursorPositionChanged();

    /**
     * Slot for when text selection changes.
     */
    void onSelectionChanged();

    /**
     * Slot for when text changes.
     */
    void onTextChanged();

    /**
     * Slot for scroll button pressed.
     */
    void onScrollButtonClicked();

    /**
     * Slot for frame button pressed.
     */
    void onFrameButtonClicked();

    /**
     * Slot for blink button pressed.
     */
    void onBlinkButtonClicked();

    /**
     * Slot for roll button pressed.
     */
    void onRollButtonClicked();

    /**
     * Slot for hold button pressed.
     */
    void onHoldButtonClicked();

    /**
     * Slot for clear button pressed.
     */
    void onClearButtonClicked();

    /**
     * Slot for slow button pressed.
     */
    void onSlowButtonClicked();

    /**
     * Slot for normal button pressed.
     */
    void onNormalButtonClicked();

    /**
     * Slot for quick button pressed.
     */
    void onQuickButtonClicked();

    /**
     * Slot for extra quick button pressed.
     */
    void onExtraQuickButtonClicked();

private:

    enum Direction
    {
        Backward,
        Within,
        Forward
    };

    QPushButton *scrollButton_;
    QPushButton *frameButton_;
    QPushButton *blinkButton_;
    QPushButton *rollButton_;
    QPushButton *holdButton_;
    QPushButton *clearButton_;

    QPushButton *slowButton_;
    QPushButton *normalButton_;
    QPushButton *quickButton_;
    QPushButton *extraQuickButton_;

    QPlainTextEdit *text_;

    QStringList tags_;
    int tagLengthMaximum_;

    // not implemented
    MarqueeTextEditorWidget( const _Myt& );

    // not implemented
    _Myt& operator = ( const _Myt& );

    /**
     * Check if text selection is valid for tagging.
     */
    bool isSelectionValid() const;

    /**
     * Check if text at cursor is a tag or not.
     */
    bool isTag( Direction d, int& pos, int& length ) const;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // MARQUEETEXTEDITORWIDGET_H
