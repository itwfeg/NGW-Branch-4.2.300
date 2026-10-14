/**
 * @file productdescriptioneditorscreen.h
 * Product Description editor.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Todd Davis
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef PRODUCTDESCRIPTIONEDITORSCREEN_H
#define PRODUCTDESCRIPTIONEDITORSCREEN_H

#include "screenwidget.h"

//class DomMarqueeConfig;
class DomPluSupport;
class DomCurrentPLU;
class SupervisorMode;

//class MarqueeTextEditorWidget;
//class MarqueeLabelWidget;
class TextEditor;
class SupervisorMode;

class QPushButton;

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class ProductDescriptionEditorScreen
 * @brief Screen for product description editing.
 */
class ProductDescriptionEditorScreen : public ScreenWidget
{
    Q_OBJECT

    typedef ProductDescriptionEditorScreen _Myt;
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
    explicit ProductDescriptionEditorScreen( SessionMainWindow *mainWindow, ModeWidget *parent = 0 );

    /**
     * Destructor.
     */
    virtual ~ProductDescriptionEditorScreen();

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
    void onTextEditorTextChanged();

private:

    //MarqueeTextEditorWidget *marqueeEditor_;
    TextEditor *textEditor_;
    //MarqueeLabelWidget *marqueeLabel_;

    //QPushButton *cancelButton_;
    //QPushButton *doneButton_;

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
    void saveProductDescriptionConfig() const;

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // PRODUCTDESCRIPTIONEDITORSCREEN_H

