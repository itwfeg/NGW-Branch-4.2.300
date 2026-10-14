/**
  * @file operatorsCanModify.h
  * @brief Allows supervisor to select which fields an operator is allowed to modify.
  *
  * @copyright Copyright (c) 2013, ITW Hobart Food Equipment Group, U.S.A.
  * @author Doug Antonides
  * @addtogroup operatorsCanModifyCsu
  * @{
  */

#ifndef OPERATORSCANMODIFY_H
#define OPERATORSCANMODIFY_H

#include "screenwidget.h"

namespace Ui {
class operatorsCanModify;
}

/**
 * @class operatorsCanModify
 * @brief Allows supervisor to select which fields an operator is allowed to modify.
 *
 * The fields include:  Price, By Count, Tare, Fixed Weight, Shelf life, Product life, Label Type and Portions
 */

class operatorsCanModify : public ScreenWidget
{
    Q_OBJECT
    
    typedef operatorsCanModify _Myt;
    typedef ScreenWidget _Mybase;

public:

    /**
     * Constructor.
     *
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
     */
    explicit operatorsCanModify(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~operatorsCanModify();

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

public slots:

    /**
     * Slot for when a checkbox is checked/un-checked.
     * Sets property indicating that the setup needs to be saved
     * when exiting.
     */
    void SetupChangedPrice(int state);

    /**
     * Slot for when a checkbox is checked/un-checked.
     * Sets property indicating that the setup needs to be saved
     * when exiting.
     */
    void SetupChangedByCount(int state);

    /**
     * Slot for when a checkbox is checked/un-checked.
     * Sets property indicating that the setup needs to be saved
     * when exiting.
     */
    void SetupChangedTare(int state);

    /**
     * Slot for when a checkbox is checked/un-checked.
     * Sets property indicating that the setup needs to be saved
     * when exiting.
     */
    void SetupChangedFixedWeight(int state);

    /**
     * Slot for when a checkbox is checked/un-checked.
     * Sets property indicating that the setup needs to be saved
     * when exiting.
     */
    void SetupChangedShelfLife(int state);

    /**
     * Slot for when a checkbox is checked/un-checked.
     * Sets property indicating that the setup needs to be saved
     * when exiting.
     */
    void SetupChangedProductLife(int state);

    /**
     * Slot for when a checkbox is checked/un-checked.
     * Sets property indicating that the setup needs to be saved
     * when exiting.
     */
    void SetupChangedLabelType(int state);

    /**
     * Slot for when a checkbox is checked/un-checked.
     * Sets property indicating that the setup needs to be saved
     * when exiting.
     */
    void SetupChangedPortions(int state);

    /**
     * Slot for when a combbox index is change.
     * Sets property indicating that the setup needs to be saved
     * when exiting.
     */
    void comboboxChanged(int newIndex);



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
    Ui::operatorsCanModify *ui;

    QString bDateTime;
    QString bDateTimeAccessLevel;

    bool bSetupChanged;

    /**
     * Builds the parameter string and saves the setup.
     *
     */
    void SaveDataOnExit();

};

#endif // OPERATORSCANMODIFY_H
