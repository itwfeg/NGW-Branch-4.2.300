/**
  * @file serviceWeigherPowerupTestResults.h
  * @brief Screen for displaying powerup test result information about the weigher attached to the system.
  *
  * @copyright Copyright (c) 2013, ITW Hobart Food Equipment Group, U.S.A.
  * @author Doug Antonides
  * @addtogroup serviceWeigherPowerupTestResultsCsu
  * @{
  */

#ifndef SERVICEWEIGHERPOWERUPTESTRESULTS_H
#define SERVICEWEIGHERPOWERUPTESTRESULTS_H

#include "screenwidget.h"

namespace Ui {
class serviceWeigherPowerupTestResults;
}

/**
 * @class serviceWeigherPowerupTestResults
 * @brief Screen for displaying powerup test result information about the weigher attached to the system.
 */
class serviceWeigherPowerupTestResults : public ScreenWidget
{
    Q_OBJECT
    
    typedef serviceWeigherPowerupTestResults _Myt;
    typedef ScreenWidget _Mybase;

public:

    /**
     * Constructor.
     *
     * @param[in] mainWindow  the @c SessionMainWindow object
     * @param[in] parent  parent @c ModeWidget widget
     */
    explicit serviceWeigherPowerupTestResults(SessionMainWindow *mainWindow, ModeWidget *parent = 0);

    /**
     * Destructor.
     */
    virtual~serviceWeigherPowerupTestResults();

    /**
     * Translate the user interface to current language.
     */
    virtual void translate();

protected:

    /**
     * Handle a session message.
     *
     * @param[in] msg  session message
     */
    virtual void onSessionMessage( const SessionMessage& msg );

private:
    Ui::serviceWeigherPowerupTestResults *ui;
};

#endif // SERVICEWEIGHERPOWERUPTESTRESULTS_H
