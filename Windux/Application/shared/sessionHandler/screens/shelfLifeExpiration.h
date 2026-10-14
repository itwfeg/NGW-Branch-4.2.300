/**
  * @file shelfLifeExpiration.h
  * @brief Allows supervisor to modify the Shelf Life Expiration notes for the Operator.
  *
  * @copyright Copyright (c) 2012, ITW Hobart Food Equipment Group, U.S.A.
  * @author Doug Antonides
  * @addtogroup shelfLifeExpirationCsu
  * @{
  */

#ifndef SHELFLIFEEXPIRATION_H
#define SHELFLIFEEXPIRATION_H

#include "screenwidget.h"

namespace Ui {
class shelfLifeExpiration;
}

/**
 * @class shelfLifeExpiration
 * @brief Allows supervisor to modify the Shelf Life Expiration notes for the Operator.
 *
 * These notes can be viewed by the Operator on the Expiring Items screen.
 */

class shelfLifeExpiration : public ScreenWidget
{
    Q_OBJECT
    
    typedef shelfLifeExpiration _Myt;
    typedef ScreenWidget _Mybase;

public:

    /**
     * Constructor.
     *
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
     */
    explicit shelfLifeExpiration(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual ~shelfLifeExpiration();

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

public slots:

    /**
     * Slot for when a note (i.e. a text edit field) is changed.
     * Sets property indicating that the setup needs to be saved
     * when exiting.
     */
    void SetupChanged();

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
    Ui::shelfLifeExpiration *ui;

    bool bSetupChanged;

    /**
     * Builds the parameter string and saves the setup.
     *
     */
    void SaveDataOnExit();

};

#endif // SHELFLIFEEXPIRATION_H
