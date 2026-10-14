/**
 * @file productnotedialog.h
 * Generic dca dialog for showing product note and other expanded text fields.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef PRODUCTNOTEDIALOG_H
#define PRODUCTNOTEDIALOG_H

#include "dcadialog.h"

#include <QDialogButtonBox>

class QLabel;
class QPlainTextEdit;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Generic dca dialog for showing product note and other expanded text fields.
class ProductNoteDialog : public DynamicContentAreaDialog
{
    Q_OBJECT

    Q_PROPERTY( QString text READ text WRITE setText )
    Q_PROPERTY( QString title READ title WRITE setTitle )

    typedef ProductNoteDialog _Myt;
    typedef DynamicContentAreaDialog _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] parent  parent widget
     * @param[in] f  window flags
     */
    explicit ProductNoteDialog( QWidget *parent = 0, Qt::WindowFlags f = 0 );

    /// Destructor.
    virtual ~ProductNoteDialog();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Whether or not clicking the overlay rejects the dialog.
    /**
     * @return  @c true if allowed, @c false otherwise
     */
    virtual bool allowOverlayClickDismiss() const {return allowOverlayDismiss_;}

    /// Retrieve button role.
    /**
     * @param[in] button  button to retrieve role of
     * @return  button role
     */
    virtual QDialogButtonBox::ButtonRole buttonRole( QAbstractButton *button ) const;

    /// Retrieve list of buttons.
    /**
     * @return  list of buttons
     */
    virtual QList<QAbstractButton*>	buttons() const;

    /// Retrieve dialog text.
    /**
     * @return  dialog text
     */
    virtual QString text() const;

    /// Retrieve dialog title.
    /**
     * @return  dialog title
     */
    virtual QString title() const;

    /// Set whether or not clicking the overlay rejects the dialog.
    /**
     * @param[in]  @c true if allowed, @c false otherwise
     */
    virtual void setAllowOverlayClickDismiss( bool value ) {allowOverlayDismiss_ = value;}

    /// Set the dialog text.
    /**
     * @param[in] value  dialog text
     */
    virtual void setText( const QString& value );

    /// Set the dialog title.
    /**
     * @param[in] value  dialog title
     */
    virtual void setTitle( const QString& value );

    // ========================================================================
    // Methods
    // ========================================================================

    /// Add standard button to button box.
    /**
     * @param[in] button  standardbutton type
     * @returns  pointer to button
     */
    virtual QPushButton *addButton( QDialogButtonBox::StandardButton button );

    /// Add custom button to button box.
    /**
     * @param[in] text  button text
     * @param[in] role  button role
     * @returns  pointer to button
     */
    virtual QPushButton *addButton( const QString& text, QDialogButtonBox::ButtonRole role );

protected:

    /// Retrieve priority of this dialog.
    /**
     * When showing a high priority dialog all dialogs of lower priority will be automatically
     * closed.
     *
     * @return  priority of this dialog
     */
    virtual int priority() const {return Low;}

private Q_SLOTS:

    /// Slot for button clicked.
    void onButtonClicked( QAbstractButton *button );

private:

    QLabel *title_;
    QPlainTextEdit *text_;
    QDialogButtonBox *buttons_;

    bool allowOverlayDismiss_;

    static const bool convertUpperCase_;

    /// Initialize.
    void initialize();

    /// Groom push button.
    void groomPushButton( QPushButton *button );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // PRODUCTNOTEDIALOG_H
