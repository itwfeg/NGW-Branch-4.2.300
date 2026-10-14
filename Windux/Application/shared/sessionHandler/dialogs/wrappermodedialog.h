/**
 * @file wrappermodedialog.h
 * Dialog for choosing wrapper mode.
 *
 * @copyright Copyright (c) 2014 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef WRAPPERMODEDIALOG_H
#define WRAPPERMODEDIALOG_H

#include "../sessiondialog.h"

class QToolButton;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Dialog for choosing wrapper mode.
class WrapperModeDialog : public SessionDialog
{
    Q_OBJECT

    typedef WrapperModeDialog _Myt;
    typedef SessionDialog _Mybase;

public:

    /// Mode flags.
    enum modeFlags
    {
        None            = 0x0000,                   ///< None.
        Apply           = 0x0001,                   ///< Apply.
        Label           = 0x0002,                   ///< Label.
        Wrap            = 0x0004,                   ///< Wrap.
        ApplyHidden     = 0x0100,                   ///< Apply is hidden.
        LabelHidden     = 0x0200,                   ///< Label is hidden.
        WrapHidden      = 0x0400                    ///< Wrap is hidden.
    };

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] mf  mode flags
     * @param[in] parent  parent widget
     * @param[in] f  window flags
     */
    explicit WrapperModeDialog( modeFlags mf, QWidget *parent = 0, Qt::WindowFlags f = 0 );

    /// Destructor.
    virtual ~WrapperModeDialog();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve size hint.
    /**
     * @return  size hint
     */
    virtual QSize sizeHint() const;

protected:

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve priority of this dialog.
    /**
     * When showing a high priority dialog all dialogs of lower priority will be automatically
     * closed.
     *
     * @return  priority of this dialog
     */
    virtual int priority() const {return Low;}

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

    /* Show event.
    *
    * @param[in,out] e  event
    */
   virtual void showEvent( QShowEvent *e );


private Q_SLOTS:

    /// Slot for apply.
    void onApplyToggled( bool value );

    /// Slot for wrap.
    void onWrapToggled( bool value );

    /// Slot for label.
    void onLabelToggled( bool value );

private:

    typedef QMap<int, int> indicatorModeMap;

    QToolButton *apply_;
    QToolButton *wrap_;
    QToolButton *label_;

    bool sendMessages_;

    indicatorModeMap indicatorModes_;

    static const bool convertUppercase_;

    /// Initialize.
    void initialize( modeFlags mf );

    /// Create layout.
    void createLayout();

    /// Update mode.
    void updateCurrentMode() const;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // WRAPPERMODEDIALOG_H
