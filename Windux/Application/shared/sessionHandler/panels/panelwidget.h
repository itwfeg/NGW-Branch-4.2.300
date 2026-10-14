/**
 * @file panelwidget.h
 * Base class for all panel widget objects.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef PANELWIDGET_H
#define PANELWIDGET_H

#include <Session.h>

#include <QWidget>

class ModeWidget;
class SessionDialog;
class SessionMainWindow;

namespace hobart {
    class backgroundWidgetBase;
    class widgetRendererAppearanceBase;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Base class for all panel widget objects.
/**
 * Panel widgets are embedded in a @c SessionDialog object.
 */
class PanelWidget : public QWidget
{
    Q_OBJECT

    typedef PanelWidget _Myt;
    typedef QWidget _Mybase;

public:

    // ========================================================================
    // DTOR
    // ========================================================================

    /// Destructor.
    virtual ~PanelWidget();

public Q_SLOTS:

    // ========================================================================
    // Methods
    // ========================================================================

    /// Accept panel.
    /**
     * Hides the panel dialog and sets the result code to @c Accepted.
     *
     * @sa reject()
     * @sa done()
     */
    virtual void accept();

    /// Close panel.
    /**
     * Closes the panel dialog and sets its result code to @p r. If this panel is shown with
     * exec(), done() causes the local event loop to finish, and exec() to return @p r.
     *
     * As with QWidget::close(), done() deletes the dialog if the Qt::WA_DeleteOnClose flag is
     * set. If the panel dialog is the application's main widget, the application terminates. If
     * the panel dialog is the last window closed, the QApplication::lastWindowClosed() signal is
     * emitted.
     *
     * @param[in] r  result code
     *
     * @sa accept()
     * @sa reject()
     */
    virtual void done( int r );

    /// Reject panel.
    /**
     * Hides the panel dialog and sets the result code to @c Rejected.
     *
     * @sa accept()
     * @sa done()
     */
    virtual void reject();

protected:

    SessionMainWindow *mainWindow_;                 ///< Pointer to the main window.
    ModeWidget *mode_;                              ///< Pointer to the current mode.

    Session *session_;                              ///< Pointer to the session.

    SessionDialog *parentDialog_;                   ///< Pointer to parent dialog

    // ========================================================================
    // CTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c SessionDialog object
     */
    explicit PanelWidget( SessionMainWindow *mainWindow, SessionDialog *parent = 0 );

    // ========================================================================
    // Events
    // ========================================================================

    /// Paint event.
    /**
     * @param[in,out] e  event
     */
    virtual void paintEvent( QPaintEvent *e );

    /// Resize event.
    /**
     * @param[in,out] e  event
     */
    virtual void resizeEvent( QResizeEvent *e );

    // ========================================================================
    // Properties
    // ========================================================================

    /// Retrieve renderer appearance.
    /**
     * @return  appearance object
     */
    virtual hobart::widgetRendererAppearanceBase *rendererAppearance() const;

    /// Retrieve renderer traits.
    /**
     * @return  traits object
     */
    virtual hobart::backgroundWidgetBase *rendererTraits() const;

    /// Retrieve renderer style.
    /**
     * @return  style
     */
    virtual int rendererStyle() const;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Handle a session message.
    /**
     * @param[in] msg  session message
     */
    virtual void onSessionMessage( const SessionMessage &msg );

private Q_SLOTS:

    /// Slot to handle session message.
    void onSessionMessageReceive( const SessionMessage &msg );

private:

    QPixmap b_;

    static const int fontPointSize_;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // PANELWIDGET_H
