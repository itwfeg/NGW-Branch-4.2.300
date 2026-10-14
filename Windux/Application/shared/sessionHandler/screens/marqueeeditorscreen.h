/**
 * @file marqueeeditorscreen.h
 * Marquee editor.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef MARQUEEEDITORSCREEN_H
#define MARQUEEEDITORSCREEN_H

#include "screenwidget.h"

class DomMarqueeConfig;

class MarqueeTextEditorWidget;
class MarqueeLabelWidget;

class QPushButton;

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class MarqueeEditorScreen
 * @brief Screen for marquee editing.
 */
class MarqueeEditorScreen : public ScreenWidget
{
    Q_OBJECT

    typedef MarqueeEditorScreen _Myt;
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
    explicit MarqueeEditorScreen( SessionMainWindow *mainWindow, ModeWidget *parent = 0 );

    /**
     * Destructor.
     */
    virtual ~MarqueeEditorScreen();

    ///////////////////////////////////////////////////////////////////////////
    // Properties

    /**
     * Retrieve screen help text.
     *
     * When help is activated on a screen this property returns the screen title and help text.
     *
     * @param[out] title  screen title
     * @param[out] description  screen help description
     * @returns  @c true if help exists for the screen, @c false otherwise
     */
    virtual bool helpText( QString& title, QString& description ) const;

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
     * Screen that gets displayed when swiping (gesturing) the screen right.
     *
     * @returns  previous screen in sequence, @c NULL if none
     */
    virtual ScreenWidget *swipeScreenRight() const;

private Q_SLOTS:

    /**
     * Slot for cancel button.
     */
    void onCancelButtonClicked();

    /**
     * Slot for done button.
     */
    void onDoneButtonClicked();

    /**
     * Slot for when marquee text changes.
     */
    void onMarqueeEditorTextChanged();

private:

    MarqueeTextEditorWidget *marqueeEditor_;
    MarqueeLabelWidget *marqueeLabel_;

    QPushButton *cancelButton_;
    QPushButton *doneButton_;

    DomMarqueeConfig *marqueeConfig_;

    bool dirty_;

    /**
     * Initialize.
     */
    void initialize();

    /**
     * Save configuration.
     */
    void saveMarqueeConfig() const;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // MARQUEEEDITORSCREEN_H
