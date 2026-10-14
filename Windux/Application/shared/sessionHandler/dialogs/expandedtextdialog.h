/**
 * @file expandedtextdialog.h
 * Dialog for viewing text fields.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef EXPANDEDTEXTDIALOG_H
#define EXPANDEDTEXTDIALOG_H

#include "../sessiondialog.h"

class QPlainTextEdit;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Dialog for viewing text fields.
class ExpandedTextDialog : public SessionDialog
{
    Q_OBJECT

    Q_PROPERTY( QString text READ text WRITE setText )
    Q_PROPERTY( QFont textFont READ textFont WRITE setTextFont )

    typedef ExpandedTextDialog _Myt;
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
    explicit ExpandedTextDialog( QWidget *parent = 0, Qt::WindowFlags f = 0 );

    /// Destructor.
    virtual ~ExpandedTextDialog();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve dialog text.
    /**
     * @return  dialog text
     */
    virtual QString text() const;

    /// Retrieve dialog text font.
    /**
     * @return  fonst
     */
    virtual QFont textFont() const;

    /// Set the dialog text.
    /**
     * @param[in] value  dialog text
     */
    virtual void setText( const QString& value );

    /// Set the dialog text font.
    /**
     * @param[in] value  font
     */
    virtual void setTextFont( const QFont& value );

    /// Retrieve size hint.
    /**
     * @return  size hint
     */
    virtual QSize sizeHint() const;

protected:

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve renderer appearance.
    /**
     * @return  appearance object
     */
    virtual hobart::widgetRendererAppearanceBase *rendererAppearance() const;

    /// Retrieve renderer style.
    /**
     * @return  style
     */
    virtual int rendererStyle() const;

private Q_SLOTS:

    /// Slot for dialog activation.
    void onActive( bool value );

private:

    QPlainTextEdit *text_;

    /// Initialize.
    void initialize();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // EXPANDEDTEXTDIALOG_H
