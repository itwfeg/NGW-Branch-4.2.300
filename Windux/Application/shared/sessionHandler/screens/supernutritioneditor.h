/**
 * @file supernutritioneditor.h
 * Nutrition editor for supervisor mode.
 *
 * @copyright Copyright (c) 2012 Hobart Corporation
 * @author Randy Blankley
 *
 * @addtogroup sessionHandlerCsu
 * @{
 */

#ifndef SUPERNUTRITIONEDITOR_H
#define SUPERNUTRITIONEDITOR_H

#include "screenwidget.h"

#include <label.h>
#include <mapdataobjectcontainer.h>

class DomCurrentPLU;
class LabelViewerWidget;
class SupervisorMode;

class QLabel;
class QPushButton;

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @class superNutritionEditorScreen
 * @brief Nutrition editor for supervisor mode.
 */
class superNutritionEditorScreen : public ScreenWidget
{
    Q_OBJECT

    typedef superNutritionEditorScreen _Myt;
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
    explicit superNutritionEditorScreen( SessionMainWindow *mainWindow, ModeWidget *parent = 0 );

    /**
     * Destructor.
     */
    virtual ~superNutritionEditorScreen();

    ///////////////////////////////////////////////////////////////////////////
    // Methods

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

protected:

    ///////////////////////////////////////////////////////////////////////////
    // Methods

    /**
     * Handle a session message.
     *
     * @param[in] msg  session message
     */
    virtual void onSessionMessage( const SessionMessage& msg );

private Q_SLOTS:

    /**
     * Slot for done button clicked.
     */
    void onDoneButtonClicked();

private:

    int requiredDataCounter_;

    LabelViewerWidget *labelViewer_;

    QPushButton *doneButton_;
    QLabel *message_;

    SupervisorMode *supervisorMode_;
    DomCurrentPLU *product_;

    bool showAllFieldsReuseMarquee_;
    bool showAllFieldsReuseNutrifacts_;

    /**
     * Initialize.
     */
    void initialize();

    /**
     * Update label view visible.
     */
    void updateLabelViewerVisible();

};

///////////////////////////////////////////////////////////////////////////////////////////////////

/** @} */

#endif // SUPERNUTRITIONEDITOR_H
