/**
 * @file wrapperforcetraysizedialog.h
 * Dialog for choosing wrapper tray size.
 *
 * @copyright Copyright (c) 2021 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef WRAPPERFORCETRAYSIZEDIALOG_H
#define WRAPPERFORCETRAYSIZEDIALOG_H

#include "../sessiondialog.h"

namespace hobart {
class softkeysWidget;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Dialog for choosing wrapper tray size.
class WrapperForcedTraySizeDialog : public SessionDialog
{
    Q_OBJECT

    typedef WrapperForcedTraySizeDialog _Myt;
    typedef SessionDialog _Mybase;

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] mf  mode flags
     * @param[in] f  window flags
     */
    explicit WrapperForcedTraySizeDialog( QWidget *parent = 0, Qt::WindowFlags f = 0 );

    /// Destructor.
    virtual ~WrapperForcedTraySizeDialog();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve size hint.
    /**
     * @return  size hint
     */
    virtual QSize sizeHint() const override;

    /// Check for existance of tray sizes.
    /**
     * @return  @c true if trays exist, @c false otherwise
     */
    virtual int traySizesExist();

protected:

    // ========================================================================
    // Events
    // ========================================================================

    /// Handle a session message.
    /**
     * @param[in] msg  session message
     */
    virtual void onSessionMessageReceive( const SessionMessage& msg ) override;

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
    virtual int priority() const override {return Low;}

    /// Retrieve renderer appearance.
    /**
     * @return  appearance object
     */
    virtual hobart::widgetRendererAppearanceBase *rendererAppearance() const override;

    /// Retrieve renderer style.
    /**
     * @return  style
     */
    virtual int rendererStyle() const override;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Reset flashkeys to default (initial) group.
    /**
     * @param[in] force  @c true to force reset all flashkey banks, @c false otherwise
     */
    virtual void resetFlashkeys( bool force = false );

private Q_SLOTS:

    /// Slot for softkey button click.
    void onSoftkeyClicked( QWidget *w, QWidget *b, int action, const QString& param, int feature );

private:

    static const bool convertUppercase_;

    hobart::softkeysWidget *trays_;

    bool response_;
    int numTrays_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // WRAPPERFORCETRAYSIZEDIALOG_H
