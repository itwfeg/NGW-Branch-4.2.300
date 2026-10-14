 /**
 * @file messageboxdialog.h
 * Generic dialog with text and buttons.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef MESSAGEBOXDIALOG_H
#define MESSAGEBOXDIALOG_H

#include "../sessiondialog.h"

#include <QDialogButtonBox>
#include <QLabel>

class QHBoxLayout;
class QVBoxLayout;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Create @c MessageBoxDialog with real time priority.
struct real_time_priority_t {};

/// Used to construct @c MessageBoxDialog with real time priority.
static const real_time_priority_t real_time_priority = {};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Generic dialog with text and buttons.
class MessageBoxDialog : public SessionDialog
{
    Q_OBJECT

    Q_PROPERTY( bool blockDialogs READ blockDialogs WRITE setBlockDialogs )
    Q_PROPERTY( QPixmap icon READ icon WRITE setIcon )
    Q_PROPERTY( QString text READ text WRITE setText )
    Q_PROPERTY( int textFontPointSize READ textFontPointSize WRITE setTextFontPointSize )
    Q_PROPERTY( QString title READ title WRITE setTitle )
    Q_PROPERTY( int titleFontPointSize READ titleFontPointSize WRITE setTitleFontPointSize )
    Q_PROPERTY( QDialogButtonBox::StandardButtons standardButtons READ standardButtons WRITE setStandardButtons )

    typedef MessageBoxDialog _Myt;
    typedef SessionDialog _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     * @param[in] f  window flags
     */
    explicit MessageBoxDialog( QWidget *parent = 0, Qt::WindowFlags f = 0 );

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     * @param[in] f  window flags
     */
    explicit MessageBoxDialog( real_time_priority_t, QWidget *parent = 0, Qt::WindowFlags f = 0 );

    /// Destructor.
    virtual ~MessageBoxDialog();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Whether or not clicking the overlay rejects the dialog.
    /**
     * @return  @c true if allowed, @c false otherwise
     */
    virtual bool allowOverlayClickDismiss() const {return allowOverlayClickDismiss_;}

    /// Check if should block dialogs.
    /**
     * @return  @c true if blocking dialogs, @c false otherwise
     */
    virtual bool blockDialogs() const {return blockDialogs_;}

    /// Find button for specific role.
    /**
     * @param[in] role  button role
     * @return  pointer to button
     */
    virtual QAbstractButton *button( QDialogButtonBox::StandardButton role ) const;

    /// Retrieve dialog icon.
    /**
     * @returns  dialog icon
     */
    virtual QPixmap icon() const;

    /// Retrieve dialog text.
    /**
     * @return  dialog text
     */
    virtual QString text() const;

    /// Retrieve dialog text font pointsize.
    /**
     * @returns  text font point size
     */
    virtual int textFontPointSize() const;

    /// Retrieve dialog title.
    /**
     * @returns  dialog title
     */
    virtual QString title() const;

    /// Retrieve dialog title font pointsize.
    /**
     * @returns  title font point size
     */
    virtual int titleFontPointSize() const;

    /// Set whether or not clicking the overlay rejects the dialog.
    /**
     * @param[in] value  @c true if allowed, @c false otherwise
     */
    virtual void setAllowOverlayClickDismiss( bool value ) {allowOverlayClickDismiss_ = value;}

    /// Set if should block dialogs.
    /**
     * @note:
     * Once set to @c true it may not be reversed.
     *
     * @param[in] value  @c true to block dialogs, @c false otherwise
     */
    virtual void setBlockDialogs( bool value );

    /// Set button role role.
    /**
     * @param[in] button  button
     * @param[in] role  button role
     */
    virtual void setButtonRole( QAbstractButton *button, QDialogButtonBox::ButtonRole role );

    /// Set the dialog icon.
    /**
     * @param[in] value  dialog icon
     */
    virtual void setIcon( const QPixmap& value );

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

    /// Set the dialog text.
    /**
     * @param[in] value  dialog text
     */
    virtual void setText( const QString& value );

    /// Set dialog text font point size.
    /**
     * @param[in] value  font point size
     */
    virtual void setTextFontPointSize( int value );

    /// Set the dialog title.
    /**
     * @param[in] value  dialog title
     */
    virtual void setTitle( const QString& value );

    /// Set dialog title font point size.
    /**
     * @param[in] value  font point size
     */
    virtual void setTitleFontPointSize( int value );

    /// Retrieve dialog standard buttons.
    /**
     * @returns  standard buttons
     */
    QDialogButtonBox::StandardButtons standardButtons() const;

    /// Set dialog standard buttons.
    /**
     * @param[in] value  standard buttons
     */
    virtual void setStandardButtons( QDialogButtonBox::StandardButtons value );

    /// Retrieve size hint.
    /**
     * @return  size hint
     */
    virtual QSize sizeHint() const;

protected:

    QHBoxLayout *messageArea_;                          ///< Layout for message area.
    QVBoxLayout *messageAreaInner_;                     ///< Inner layout for message area.

    QHBoxLayout *buttonsArea_;                          ///< Layout for buttons area.

    // ========================================================================
    // Events
    // ========================================================================

    /// Hide event.
    /**
     * @param[in,out] e  event
     */
    virtual void hideEvent( QHideEvent *e );

    /// Show event.
    /**
     * @param[in,out] e  event
     */
    virtual void showEvent( QShowEvent *e );

    /// Resize event.
    /**
     * @param[in,out] e  resize event
     */
    virtual void resizeEvent( QResizeEvent *e );

    // ========================================================================
    // Properties
    // ========================================================================

    /// Check if dialog can be shown in dynamic content area.
    /**
     * @return  @c true if allowed, @c false otherwise
     */
    virtual bool allowDynamicContentArea() const {return true;}

    /// Retrieve button role.
    /**
     * @param[in] button  button to retrieve role of
     * @return  button role
     */
    QDialogButtonBox::ButtonRole buttonRole( QAbstractButton *button ) const;

    /// Retrieve list of buttons.
    /**
     * @returns  list of buttons
     */
    QList<QAbstractButton*>	buttons() const;

    /// Retrieve priority of this dialog.
    /**
     * When showing a high priority dialog all dialogs of lower priority will be automatically
     * closed.
     *
     * @return  priority level
     */
    virtual int priority() const {return priority_;}

    /// Set foreground palette of text.
    /**
     * @param[in] value  color
     */
    virtual void setForegroundPalette( const QColor& color );

    // ========================================================================
    // Methods
    // ========================================================================

    /// Add standard button to button box.
    /**
     * @param[in] button  standardbutton type
     * @return  pointer to button
     */
    virtual QPushButton *addButton( QDialogButtonBox::StandardButton button );

    /// Add custom button to button box.
    /**
     * @param[in] text  button text
     * @param[in] role  button role
     * @return  pointer to button
     */
    virtual QPushButton *addButton( const QString& text, QDialogButtonBox::ButtonRole role );

    /// Handle button click.
    /**
     * @param[in] button  pointer to button clicked
     */
    virtual void onButtonClicked( QAbstractButton *button );

private Q_SLOTS:

    /// Slot for button click.
    void onButtonClickedInternal( QAbstractButton *button );

private:

    bool allowOverlayClickDismiss_;
    bool blockDialogs_;

    QWidget *icon_;
    QLabel *title_;

    QLabel *text_;
    QDialogButtonBox *buttons_;

    QPixmap iconPixmap_;
    Priority priority_;

    bool sizeToFit_;
    int sizeToFitFontPointSize_;

    static const bool convertUpperCase_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

    /// Groom button.
    void groomPushButton( QPushButton *button );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // MESSAGEBOXDIALOG_H
