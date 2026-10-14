/**
 * @file operatemodedialog.h
 * Dialog for choosing operate mode.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef OPERATEMODEDIALOG_H
#define OPERATEMODEDIALOG_H

#include "../sessiondialog.h"

class ScaleConfiguration;

class QCheckBox;
class QToolButton;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Dialog for choosing operate mode.
class OperateModeDialog : public SessionDialog
{
    Q_OBJECT

    typedef OperateModeDialog _Myt;
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
    explicit OperateModeDialog( QWidget *parent = 0, Qt::WindowFlags f = 0 );

    /// Destructor.
    virtual ~OperateModeDialog();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve size hint.
    /**
     * @return  size hint
     */
    virtual QSize sizeHint() const;

    // ========================================================================
    // Static Methods
    // ========================================================================

    /// Check if operate mode dialog is enabled or not.
    /**
     * @param scaleConfig  pointer to scale configuration
     * @return  @c true if enabled, @c false otherwise
     */
    static bool isEnabled( const ScaleConfiguration *scaleConfig );

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

    /// Handle a session message.
    /**
     * @param[in] msg  session message
     */
    virtual void onSessionMessageReceive( const SessionMessage& msg );

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

    /// Slot for accumulate items.
    void onAccumulateItemEntryTypeToggled( bool value );

    /// Slot for manual mode.
    void onManualModeTypeToggled( bool value );

    /// Slot for normal item entry.
    void onNormalItemEntryTypeToggled( bool value );

    /// Slot for prepack mode.
    void onPrepackModeTypeToggled( bool value );

    /// Slot for production entry.
    void onProductionItemEntryTypeToggled( bool value );

    /// Slot for self service.
    void onSelfServiceModeTypeToggled( bool value );

private:

    QCheckBox *manualModeType_;
    QCheckBox *prepackModeType_;
    QCheckBox *selfServiceModeType_;

    QToolButton *normalItemEntryType_;
    QToolButton *accumulateItemEntryType_;
    QToolButton *productionItemEntryType_;

    bool sendMessages_;

    static const bool convertUppercase_;

    /// Initialize.
    void initialize();

    /// Set mode to current.
    void setMode();

    /// Set item entry to current.
    void setItemEntry();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // OPERATEMODEDIALOG_H
