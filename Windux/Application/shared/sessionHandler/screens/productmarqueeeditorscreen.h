/**
 * @file marqueeeditorscreen.h
 * Marquee editor.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Todd Davis
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef PRODUCTMARQUEEEDITORSCREEN_H
#define PRODUCTMARQUEEEDITORSCREEN_H

#include "screenwidget.h"

//class DomMarqueeConfig;
class DomPluSupport;
class DomCurrentPLU;
class SupervisorMode;

class MarqueeTextEditorWidget;
class MarqueeLabelWidget;
class SupervisorMode;

class QPushButton;

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class MarqueeEditorScreen
 * @brief Screen for marquee editing.
 */
class ProductMarqueeEditorScreen : public ScreenWidget
{
    Q_OBJECT

    typedef ProductMarqueeEditorScreen _Myt;
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
    explicit ProductMarqueeEditorScreen( SessionMainWindow *mainWindow, ModeWidget *parent = 0 );

    /**
     * Destructor.
     */
    virtual ~ProductMarqueeEditorScreen();

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


    /**
     * Handle a session message.
     *
     * @param[in] msg  session message
     */
    virtual void onSessionMessage( const SessionMessage& msg );

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

    DomPluSupport *pluSupport_;
    bool showAllFieldsReuseMarquee_;
    bool showAllFieldsReuseNutrifacts_;

    SupervisorMode *supervisorMode_;
    DomCurrentPLU *product_;

    int textTypeLoc; //3
    QString usedByManyLoc; //4
    QString textFunctionLoc; //5
    QString supportPLULoc; //6
    QString supportTextLoc; //7
    QString supportText2Loc; //8
    QString supportText3Loc; //9
    QString supportText4Loc; //10
    QString supportText5Loc; //11

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

#endif // PRODUCTMARQUEEEDITORSCREEN_H

