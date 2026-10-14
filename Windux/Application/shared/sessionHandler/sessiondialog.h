/**
 * @file sessiondialog.h
 * Dialog widget that darkens the background.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef SESSIONDIALOG_H
#define SESSIONDIALOG_H

//#include <overlaywidget.h>
#include "overlaywidget.h"

#include <Session.h>
#include <backgroundwidgettypes.h>

#include <QDialog>
#include <QList>
#include <QMutex>

class SessionMainWindow;

namespace hobart
{
    class backgroundWidgetBase;
    class widgetRendererAppearanceBase;
}

class QBoxLayout;

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Locations where the dialog may be displayed.
enum DialogLocation
{
    BottomDialogLocation,                           ///< Dialog is coming from the bottom.
    CenterDialogLocation,                           ///< Dialog is in the center.
    LeftDialogLocation,                             ///< Dialog is coming from the left.
    RightDialogLocation,                            ///< Dialog is coming from the right.
    TopDialogLocation                               ///< Dialog is coming from the top.
};

///////////////////////////////////////////////////////////////////////////////////////////////////

/// Dialog widget that darkens the background.
/**
 * @note
 * When creating a dialog and displaying it use the exec() or open() methods. Please do not show()
 * it otherwise it will not be managed by the session dialog system.
 */
class SessionDialog : public QDialog
{
    Q_OBJECT

    Q_PROPERTY( bool allowOverlayClickDismiss READ allowOverlayClickDismiss )
    Q_PROPERTY( Qt::Alignment dialogAlignment READ dialogAlignment WRITE setDialogAlignment )
    Q_PROPERTY( int dialogAlignmentOffset READ dialogAlignmentOffset WRITE setDialogAlignmentOffset )
    Q_PROPERTY( int dialogOffset READ dialogOffset WRITE setDialogOffset )
    Q_PROPERTY( DialogLocation location READ location WRITE setLocation )

    friend class Overlay;

    typedef SessionDialog _Myt;
    typedef QDialog _Mybase;

Q_SIGNALS:

    /// Slot for when dialog is active.
    /**
     * @param[in] value  @c true when dialog is opened, @c false when closed.
     */
    void active( bool value );

public:

    // ========================================================================
    // CTOR / DTOR
    // ========================================================================

    /// Constructor.
    /**
     * @param[in] location  location of the dialog
     * @param[in] parent  parent widget
     * @param[in] f  window flags
     */
    explicit SessionDialog( DialogLocation location, QWidget *parent = 0, Qt::WindowFlags f = 0 );

    /// Destructor.
    virtual ~SessionDialog();

    // ========================================================================
    // Properties
    // ========================================================================

    /// Whether or not the dialog may be minimized.
    /**
     * @note
     * Currently this functionality is only available and supported for the help/video dialog!
     *
     * @return  @c true if allowed, @c false otherwise
     */
    virtual bool allowMinimize() const {return false;}

    /// Whether or not clicking the overlay rejects the dialog.
    /**
     * @return  @c true if allowed, @c false otherwise
     */
    virtual bool allowOverlayClickDismiss() const {return true;}

    /// Retrieve dialog alignment.
    /**
     * @return  alignment of dialog.
     */
    virtual Qt::Alignment dialogAlignment() const {return dialogAlignment_;}

    /// Retrieve dialog alignment offset.
    /**
     * @return  alignment offset of dialog
     */
    virtual int dialogAlignmentOffset() const {return dialogAlignmentOffset_;}

    /// Retrieve dialog offset.
    /**
     * @return  offset of dialog
     */
    virtual int dialogOffset() const {return dialogOffset_;}

    /// Check if dialog was dismissed from overlay.
    /**
     * @return  @c true if dialog was dismissed from overlay cancellation, @c false otherwise
     */
    virtual bool dismissedFromOverlayClick() const {return dismissedFromOverlay_;}

    /// Retrieve dialog location.
    /**
     * @return  location of dialog
     */
    virtual DialogLocation location() const {return location_;}

    /// Set dialog alignment.
    /**
     * This property can be combined with location() to further groom where your dialog is show.
     * For example a @c LeftDialogLocation location with an alignment of @c AlignBottom will show
     * the dialog in the bottom left corner.
     *
     * @note
     * This will only work when you set it before the dialog is shown! It does dynamically adjust
     * the location when already visible.
     *
     * @param[in] value  alignment of dialog
     */
    virtual void setDialogAlignment( Qt::Alignment value ) {dialogAlignment_ = value;}

    /// Set dialog alignment offset.
    /**
     * This property can be combined with alignment() to further groom where your dialog is show.
     * For example a @c AlignBottom alignment with an offset of 10 pixels will show the
     * dialog 10 pixels in from the bottom border.
     *
     * @note
     * This will only work when you set it before the dialog is shown! It does dynamically adjust
     * the location when already visible.
     *
     * @param[in] value  alignment offset of dialog
     */
    virtual void setDialogAlignmentOffset( int value ) {dialogAlignmentOffset_ = value;}

    /// Set dialog offset.
    /**
     * This property can be combined with location() to further groom where your dialog is show.
     * For example a @c LeftDialogLocation location with an offset of 10 pixels will show the
     * dialog 10 pixels in from the left border.
     *
     * @note
     * This will only work when you set it before the dialog is shown! It does dynamically adjust
     * the location when already visible.
     *
     * @param[in] value  offset of dialog
     */
    virtual void setDialogOffset( int value ) {dialogOffset_ = value;}

    /// Set dialog location.
    /**
     * @note
     * This will only work when you set it before the dialog is shown! It does dynamically adjust
     * the location when already visible.
     *
     * @param[in] value  location of dialog
     */
    virtual void setLocation( DialogLocation value ) {location_ = value;}

    /// Retrieve size hint.
    /**
     * @return  size hint
     */
    virtual QSize sizeHint() const;

    // ========================================================================
    // Methods
    // ========================================================================

    /// Show dialog.
    /**
     * Shows the dialog as a modal dialog, blocking until the user closes it. The function returns
     * a DialogCode result.
     *
     * If the dialog is application modal, users cannot interact with any other window in the same
     * application until they close the dialog. If the dialog is window modal, only interaction
     * with the parent window is blocked while the dialog is open. By default, the dialog is
     * application modal.
     *
     * @warning
     * This method will not delete the dialog, you are resposible for memory! If you allocate a
     * pointer and do not delete it a memory leak will occur. Example usage:
     * @code
     *   // create a dialog and execute it
     *   SessionDialog *myDialog = new myDialog();
     *   myDialog->exec();
     *   delete myDialog;
     *
     *   // create a dialog and execute it
     *   SessionDialog myDialog();
     *   myDialog.exec();
     * @endcode
     *
     * @return  dialog result code
     *
     * @sa open()
     * @sa show()
     * @sa result()
     * @sa setWindowModality()
     */
    virtual int exec();

    /// Show dialog.
    /**
     * Shows the dialog as a window modal dialog, returning immediately.
     *
     * @warning
     * Use this method with a dialog pointer. When the dialog completes it will be automatically
     * deleted by the system. Example usage:
     * @code
     *   // create a dialog and open it
     *   SessionDialog *myDialog = new myDialog();
     *   myDialog->open();
     *
     *   // DO NOT DELETE IT! DO NOT DO THIS!!
     *   delete myDialog; // bad mojo! will crash!!
     * @endcode
     *
     * @sa exec()
     * @sa show()
     * @sa result()
     * @sa setWindowModality()
     */
    virtual void open();

protected:

    /// Dialog prioritys.
    enum Priority
    {
        Low                 = 0,                    ///< Low priority.
        BelowNormal         = 1,                    ///< Below normal priority.
        Normal              = 2,                    ///< Normal priority.
        AboveNormal         = 3,                    ///< Above normal priority.
        High                = 4,                    ///< High priority.
        RealTime            = 9                     ///< Real time priority.
    };

    SessionMainWindow *mainWindow_;                 ///< Pointer to session main window.
    Session *session_;                              ///< Pointer to session.

    static int scalingTableX_[2048];            ///< Protected so that all child widgets can reference table lookups for screen-specific scaling
    static int scalingTableY_[1024];            ///  which may require more fine tuning than scaleWidget()'s recursion allows


    // ========================================================================
    // Events
    // ========================================================================

    /// Hide event.
    /**
     * @param[in,out] event  hide event
     */
    virtual void hideEvent( QHideEvent *event );

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

    /// Show event.
    /**
     * @param[in,out] event  show event
     */
    virtual void showEvent( QShowEvent *event );

    // ========================================================================
    // Properties
    // ========================================================================

    /// Check if dialog can be shown in dynamic content area.
    /**
     * @return  @c true if allowed, @c false otherwise
     */
    virtual bool allowDynamicContentArea() const {return false;}

    /// Block focus of next and previous widgets.
    /**
     * @param[in] value  @c true to block, @c false otherwise
     */
    virtual void blockFocusNextPrev( bool value ) {blockFocusNextPrev_ = value;}

    /// Retrieve priority of this dialog.
    /**
     * When showing a high priority dialog all dialogs of lower priority will be automatically
     * closed.
     *
     * @return  priority
     */
    virtual int priority() const {return Normal;}

    /// Scale widget to screen size.
    void scaleWidget( QWidget *w );

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
    /// Now only renders standard light panel style with white border
    /// Does not switch to dark if rendered in !OperateMode
    /**
     * @return  style
     */
    virtual int rendererStyle() const {return hobart::ReadOnly;}

    // ========================================================================
    // Methods
    // ========================================================================

    /// Event filter.
    /**
     * @param[in,out] obj  object event filter was attached to
     * @param[in,out] event  event that is occuring
     * @return  @c true if event @p event should be filtered, @c false otherwise
     */
    virtual bool eventFilter( QObject *obj, QEvent *event );

    /// Focus next child.
    /**
     * Finds a new widget to give the keyboard focus to, as appropriate for Tab, and returns
     * @c true if it can find a new widget, or @c false if it can't.
     *
     * @return  @c true if widget focused, @c false otherwise
     */
    virtual bool focusNextChild();

    /// Focus next or previious child.
    /**
     * Finds a new widget to give the keyboard focus to, as appropriate for Tab and Shift+Tab, and
     * returns @c true if it can find a new widget, or @c false if it can't.
     *
     * If @p next is true, this function searches forward, if @p next is false, it searches
     * backward.
     *
     * @param next  direction to search
     * @return  @c true if widget focused, @c false otherwise
     */
    virtual bool focusNextPrevChild( bool next );

    /// Focus previous child.
    /**
     * Finds a new widget to give the keyboard focus to, as appropriate for Shift+Tab, and returns
     * @c true if it can find a new widget, or @c false if it can't.
     *
     * @return  @c true if widget focused, @c false otherwise
     */
    virtual bool focusPreviousChild();

    /// Callback for session messages.
    /**
     * This base method does nothing. Override to implement your own functionality.
     *
     * @param[in] msg  session message
     */
    virtual void onSessionMessageReceive( const SessionMessage& msg );

private Q_SLOTS:

    /// Slot to recieve dialog finished signal.
    void onFinished( int result );

    /// Slot to handle session messages.
    void onSessionMessageReceiveInternal( const SessionMessage& msg );

private:

    static bool scalingTableInit_;

    /// Overlay class for painting the slide button.
    class Overlay : public OverlayWidget
    {
        typedef Overlay _Myt;
        typedef OverlayWidget _Mybase;

    public:

        /// Constructor.
        explicit Overlay( SessionDialog *dialog, QWidget *parent = 0 );

        /// Destructor.
        virtual ~Overlay();

    protected:

        /// Paint event.
        virtual void paintEvent( QPaintEvent *event );

    private:

        SessionDialog *dialog_;

    };

    DialogLocation location_;
    Overlay *overlay_;

    bool dismissedFromOverlay_;

    QMutex finishedSignal_;

    QPixmap b_;

    bool blocking_;
    bool done_;

    Qt::Alignment dialogAlignment_;
    int dialogAlignmentOffset_;

    int dialogOffset_;

    bool blockFocusNextPrev_;

    /// Calculate position.
    QPoint calcPosition() const;

    /// Open dialog.
    void openImpl();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // SESSIONDIALOG_H
