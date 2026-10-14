/**
 * @file camerapredictionsdialog.h
 * Dialog to choose a product from camera system predictions.
 *
 * @copyright Copyright (c) 2021 Hobart Corporation
 * @author Randy Blankley
 */

#ifndef CAMERAPREDICTIONSDIALOG_H
#define CAMERAPREDICTIONSDIALOG_H

#include "../sessiondialog.h"

namespace hobart {
class pushButton;
class softkeysWidget;
}

class DomLoadSoftkeyGroup;

class QLabel;
class QPushButton;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Dialog to choose a product from camera system predictions.
class CameraPredictionsDialog : public SessionDialog
{
    Q_OBJECT

    typedef CameraPredictionsDialog _Myt;
    typedef SessionDialog _Mybase;

signals:

    /// Signal for product search requested.
    void productSearchRequested();

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] mf  mode flags
     * @param[in] f  window flags
     */
    explicit CameraPredictionsDialog( QWidget *parent = 0, Qt::WindowFlags f = 0 );

    /// Destructor.
    virtual ~CameraPredictionsDialog();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve size hint.
    /**
     * @return  size hint
     */
    virtual QSize sizeHint() const override;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Set softkey group.
    /**
     * @brief setSoftkeyGroup
     * @param value
     */
    virtual void setSoftkeyGroup( const DomLoadSoftkeyGroup *value );

    // ========================================================================
    // Static Methods
    // ========================================================================

    /// Check if dialog is shown or not.
    /**
     * @return  @c true if shown, @c false otherwise.
     */
    static bool isShown();

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

    // ========================================================================
    // Methods
    // ========================================================================

    /// Reset flashkeys to default (initial) group.
    /**
     * @param[in] force  @c true to force reset all flashkey banks, @c false otherwise
     */
    virtual void resetFlashkeys( bool force = false );

private Q_SLOTS:

    /// Slot for product search clicked.
    void onProductSearchClicked();

    /// Slot for softkey button click.
    void onSoftkeyClicked( QWidget *w, QWidget *b, int action, const QString& param, int feature );

private:

    static QMutex m_;
    static bool shown_;

    QLabel *title_;

    hobart::softkeysWidget *products_;

    hobart::pushButton *productSearch_;
    hobart::pushButton *cancel_;

    /// Initialize.
    void initialize();

    /// Create layout.
    void createLayout();

    /// Set shown flag.
    static void setShown( bool value );

};

///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // CAMERAPREDICTIONSDIALOG_H
