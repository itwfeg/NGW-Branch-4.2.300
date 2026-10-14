/**
  * @file superPrintSetup.h
  * @brief Allows supervisor to setup margins at top and bottom of a label.
  *
  * @copyright Copyright (c) 2013, ITW Hobart Food Equipment Group, U.S.A.
  * @author Doug Antonides
  * @addtogroup superPrintSetupCsu
  * @{
  */

#ifndef SUPERPRINTSETUP_H
#define SUPERPRINTSETUP_H

#include "screenwidget.h"

class QIntValidator;

namespace Ui {
class superPrintSetup;
}

/**
 * @class superPrintSetup
 * @brief Allows supervisor to setup margins at top and bottom of a label.
 *
 */

class superPrintSetup : public ScreenWidget
{
    Q_OBJECT

    typedef superPrintSetup _Myt;
    typedef ScreenWidget _Mybase;

    
public:

    /**
     * Constructor.
     *
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
     */
    explicit superPrintSetup(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~superPrintSetup();

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

public slots:

    /**
     * Slot for when a margin text field is changed.
     * Sets property indicating that the setup needs to be saved
     * when exiting.
     */
    void SetupChanged();

    /**
     * Slot for when the "Print a test report" button is clicked.
     */
    void StartPrintingTestReport();

    /**
     * Slot for when the "Stop printing" button is clicked.
     */
    void StopPrintingTestReport();


protected:

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

private:
    Ui::superPrintSetup *ui;

    bool bSetupChanged;

    QIntValidator *topOfLabelMarginValidator_;
    QIntValidator *bottomOfLabelMarginValidator_;

    /**
     * Used with numericLineEdit widget to validate Top and Bottom of Label Margin.
     *
     */
    int makeValidatorTop(int digits);

    /**
     * Builds the parameter string and saves the setup.
     *
     */
    void SaveDataOnExit();

};

#endif // SUPERPRINTSETUP_H
