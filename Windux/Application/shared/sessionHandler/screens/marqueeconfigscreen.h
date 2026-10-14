/**
 * @file marqueeconfigscreen.h
 * Marquee configuration.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef MARQUEECONFIGSCREEN_H
#define MARQUEECONFIGSCREEN_H

#include "screenwidget.h"

class DomMarqueeConfig;

class QCheckBox;
class QComboBox;
class QLabel;
class QPlainTextEdit;
class QPushButton;
class QSpinBox;

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class MarqueeConfigScreen
 * @brief Screen for marquee configuration.
 */
class MarqueeConfigScreen : public ScreenWidget
{
    Q_OBJECT

    typedef MarqueeConfigScreen _Myt;
    typedef ScreenWidget _Mybase;

public:

    ///////////////////////////////////////////////////////////////////////////
    // CTOR / DTOR

    /**
     * Constructor.
     *
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
     */
    explicit MarqueeConfigScreen( SessionMainWindow *mainWindow, ModeWidget *parent = 0 );

    /**
     * Destructor.
     */
    virtual ~MarqueeConfigScreen();

    ///////////////////////////////////////////////////////////////////////////
    // Methods

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

protected:

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Screen that gets displayed when swiping (gesturing) the screen left.
     *
     * @returns  next screen in sequence, @c NULL if none
     */
    virtual ScreenWidget *swipeScreenLeft() const;

    ///////////////////////////////////////////////////////////////////////////
    // Methods

    /**
     * Event filter.
     *
     * @param[in,out] obj  object event filter was attached to
     * @param[in,out] event  event that is occuring
     * @returns  @c true if event @p event should be filtered, @c false otherwise
     */
    virtual bool eventFilter( QObject *obj, QEvent *event );

    /**
     * Handle screen changing.
     *
     * This method is called before the screen is changed to something new. If for some reason the
     * screen should not be unloaded and destroyed then a value of @c false should be returned.
     *
     * @returns  @c true to unload screen, @c false to keep screen resident
     */
    virtual bool onScreenChanging();

    /**
     * Handle a session message.
     *
     * @param[in] msg  session message
     */
    virtual void onSessionMessage( const SessionMessage& msg );

private Q_SLOTS:

    /**
     * Slot to edit the marquee.
     */
    void onMarqueeEditClicked();

    /**
     * Slot to clear the marquee.
     */
    void onMarqueeClearClicked();

private:

    QLabel *marqueeLabel_;
    QPushButton *marqueeEditButton_;
    QPushButton *marqueeClearButton_;
    QPlainTextEdit *marqueeText_;

    DomMarqueeConfig *marqueeConfig_;

    bool blockEvents_;
    bool saveOnExit_;

    /**
     * Initialize.
     */
    void initialize();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // MARQUEECONFIGSCREEN_H
